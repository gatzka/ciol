#
# This file is supposed to run in ctest script mode:
# ctest -S <path-to-this-file>/build.cmake
#
# You can set some command line variables to change the
# behaviour of this script:
# -DCIO_CTEST_CONFIGURATION_TYPE:STRING=Debug|Release
# -DCIO_CTEST_MODEL:STRING=Experimental|Nightly|Continuous
# -DCIO_CTEST_COVERAGE:BOOL=OFF|ON
# -DCIO_CTEST_COMPILER:STRING=gcc|gcc-<version-number>|clang|clang-<version-number>|scan-build-<version-number>

set(CTEST_USE_LAUNCHERS 1)

if(NOT DEFINED CIO_CTEST_MODEL)
    set(CIO_CTEST_MODEL "Experimental")
endif()

if(NOT DEFINED CIO_CTEST_COVERAGE)
    set(CIO_CTEST_COVERAGE OFF)
endif()

set(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}")
set(CTEST_BINARY_DIRECTORY "/tmp/cio/")

if (NOT DEFINED CIO_CTEST_CONFIGURATION_TYPE)
    set(CIO_CTEST_CONFIGURATION_TYPE "Debug")
endif()
set(CTEST_CONFIGURATION_TYPE ${CIO_CTEST_CONFIGURATION_TYPE})

if(NOT CIO_CTEST_MODEL STREQUAL "Experimental")
    ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
endif()

if(NOT DEFINED CIO_CTEST_COMPILER)
    set(CIO_CTEST_COMPILER "gcc")
endif()

string(REGEX MATCH "^gcc|^clang|^scan-build" C_COMPILER_TYPE ${CIO_CTEST_COMPILER})
string(REPLACE ${C_COMPILER_TYPE} "" COMPILER_VERSION ${CIO_CTEST_COMPILER})
if(C_COMPILER_TYPE STREQUAL "gcc")
    set(CXX_COMPILER_TYPE "g++")
    set(COVERAGE_TOOL "gcov${COMPILER_VERSION}")
elseif(C_COMPILER_TYPE STREQUAL "clang")
    set(CXX_COMPILER_TYPE "clang++")
    set(COVERAGE_TOOL "llvm-cov${COMPILER_VERSION}")
    set(CTEST_COVERAGE_EXTRA_FLAGS "gcov")
else()
    set(C_COMPILER_TYPE "clang")
    set(CXX_COMPILER_TYPE "clang++")
    set(COVERAGE_TOOL "llvm-cov${COMPILER_VERSION}")

	set(ENV{CCC_CC} "${C_COMPILER_TYPE}${COMPILER_VERSION}")
	set(ENV{CCC_CXX} "${CXX_COMPILER_TYPE}${COMPILER_VERSION}")
    set(CTEST_COVERAGE_EXTRA_FLAGS "gcov")
	set(CTEST_CONFIGURE_COMMAND "scan-build${COMPILER_VERSION} ${CMAKE_COMMAND} -DCMAKE_C_FLAGS=--coverage ${CTEST_SOURCE_DIRECTORY}")
    set(ANALYZER_REPORT_DIR "${CTEST_BINARY_DIRECTORY}analyzer-scan/")
    set(CTEST_BUILD_COMMAND "scan-build${COMPILER_VERSION} --status-bugs -o ${ANALYZER_REPORT_DIR} ${CMAKE_COMMAND} --build ${CTEST_BINARY_DIRECTORY}")
    set(IS_CLANG_STATIC_ANALYZER TRUE)
endif()

set(CONFIGURE_OPTIONS
    "-DCMAKE_CXX_COMPILER=${CXX_COMPILER_TYPE}${COMPILER_VERSION}"
    "-DCMAKE_C_COMPILER=${C_COMPILER_TYPE}${COMPILER_VERSION}"
    "-DCMAKE_C_FLAGS_INIT=-pipe -fno-common"
)

if(CIO_CTEST_COVERAGE)
    set(CONFIGURE_OPTIONS ${CONFIGURE_OPTIONS} "-DCMAKE_C_FLAGS=--coverage")
endif()

###########
cmake_host_system_information(RESULT FQDN QUERY FQDN)

set(CTEST_BUILD_NAME "${FQDN}")
set(CTEST_SITE "${FQDN}")

include(ProcessorCount)
ProcessorCount(NUMBER_OF_CORES)
if(NUMBER_OF_CORES EQUAL 0)
    set(NUMBER_OF_CORES 1)
endif()

ctest_start(${CIO_CTEST_MODEL})

set(CTEST_CMAKE_GENERATOR "Ninja")
ctest_configure(OPTIONS "${CONFIGURE_OPTIONS}")


# Test/Coverage step

find_program(GCOVR_BIN gcovr)
if(CIO_CTEST_COVERAGE AND GCOVR_BIN)
    string(LENGTH ${CTEST_BINARY_DIRECTORY} CTEST_BINARY_DIRECTORY_LEN)
    MATH(EXPR CTEST_BINARY_DIRECTORY_LEN "${CTEST_BINARY_DIRECTORY_LEN}-1")
    string(SUBSTRING ${CTEST_BINARY_DIRECTORY} 0 ${CTEST_BINARY_DIRECTORY_LEN} CIO_OBJECT_DIRECTORY)
    
    string(TIMESTAMP CIO_CTEST_TIMESTAMP UTC)
    set(CIO_CTEST_COVERAGE_DIR "${CTEST_BINARY_DIRECTORY}cov-html/${CIO_CTEST_TIMESTAMP}")
    set(CTEST_CUSTOM_POST_TEST
        "cmake -E make_directory ${CIO_CTEST_COVERAGE_DIR}"
        "${GCOVR_BIN} \"--gcov-executable=${COVERAGE_TOOL} ${CTEST_COVERAGE_EXTRA_FLAGS}\" --html --html-details --html-title cio -f ${CTEST_SCRIPT_DIRECTORY}/src/\\* -e ${CTEST_SCRIPT_DIRECTORY}/src/http-parser/\\* -e ${CTEST_SCRIPT_DIRECTORY}/src/miniz/\\* -e ${CTEST_SCRIPT_DIRECTORY}/src/sha1/\\* --exclude-directories .\\*CompilerIdC\\* -r ${CTEST_SCRIPT_DIRECTORY} --object-directory=${CIO_OBJECT_DIRECTORY} -o ${CIO_CTEST_COVERAGE_DIR}/index.html")
    
    set(CTEST_COVERAGE_COMMAND "${COVERAGE_TOOL}")
    set(CTEST_CUSTOM_COVERAGE_EXCLUDE
    ${CTEST_CUSTOM_COVERAGE_EXCLUDE}
        ".*/tests/.*"
        ".*/src/http-parser/.*"
        ".*/src/sha1/.*"
    )
endif()

if(CTEST_CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    set(CTEST_BUILD_FLAGS -j${NUMBER_OF_CORES})
endif()
ctest_build(NUMBER_ERRORS CIO_NUMBER_OF_ERRORS NUMBER_WARNINGS CIO_NUMBER_OF_WARNING)
if(CIO_NUMBER_OF_ERRORS OR CIO_NUMBER_OF_WARNING)
    message(" -- Error or warning occured while building!")
    if (IS_CLANG_STATIC_ANALYZER )
        message(" -- Look into ${ANALYZER_REPORT_DIR} to see clang static analyzer report!")
    endif()
    return()
endif()

ctest_test(PARALLEL_LEVEL ${NUMBER_OF_CORES})

if(CIO_CTEST_COVERAGE AND GCOVR_BIN)
    ctest_coverage()
    message(" -- Open ${CIO_CTEST_COVERAGE_DIR}/index.html to see collected coverage")
endif()

find_program(CTEST_MEMORYCHECK_COMMAND NAMES valgrind)
set(CTEST_MEMORYCHECK_TYPE Valgrind)
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--errors-for-leak-kinds=all --show-leak-kinds=all --leak-check=full --error-exitcode=1")
#set(CTEST_MEMORYCHECK_SUPPRESSIONS_FILE ${CTEST_SOURCE_DIRECTORY}/tests/valgrind.supp)
#ctest_memcheck(PARALLEL_LEVEL ${NUMBER_OF_CORES})

# unset(CTEST_MEMORYCHECK_COMMAND)
# unset(CTEST_MEMORYCHECK_COMMAND_OPTIONS)
# set(CTEST_MEMORYCHECK_TYPE UndefinedBehaviorSanitizer)
# ctest_memcheck(PARALLEL_LEVEL ${NUMBER_OF_CORES})


# include(CTestCoverageCollectGCOV)
# ctest_coverage_collect_gcov(
#     TARBALL gcov.tar
#     SOURCE ${CTEST_SOURCE_DIRECTORY}
#     BUILD ${CTEST_BINARY_DIRECTORY}
#     GCOV_COMMAND ${CTEST_COVERAGE_COMMAND}
# )
# if(EXISTS "${CTEST_BINARY_DIRECTORY}/gcov.tar")
#     ctest_submit(CDASH_UPLOAD "${CTEST_BINARY_DIRECTORY}/gcov.tar"
#     CDASH_UPLOAD_TYPE GcovTar)
# endif()
# 
# ctest_submit()