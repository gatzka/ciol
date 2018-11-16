set(ENV{LC_MESSAGES} "en_US")
set(ENV{LANG} "C")
set(CTEST_SOURCE_DIRECTORY "${CTEST_SCRIPT_DIRECTORY}")
set(CTEST_BINARY_DIRECTORY "/tmp/cio/")

cmake_host_system_information(RESULT FQDN QUERY FQDN)

set(CTEST_BUILD_NAME "${FQDN}")
set(CTEST_SITE "${FQDN}")

SET (CTEST_ENVIRONMENT
    "CC=gcc-8"
    "CXX=g++-8"
)
#SET (CTEST_ENVIRONMENT "PATH=c:/WINDOWS/system32\;c:/WINDOWS\;c:/Programs/Borland/Bcc55/Bin")

ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

set(ENV{CFLAGS} "--coverage")
set(CTEST_CMAKE_GENERATOR "Ninja")
set(CTEST_USE_LAUNCHERS 1)

set(CTEST_CUSTOM_POST_TEST
    "mkdir ${CTEST_BINARY_DIRECTORY}/cov-html/"
    "gcovr --html --html-details -f ${CTEST_SCRIPT_DIRECTORY}/src/.* -e ${CTEST_SCRIPT_DIRECTORY}/src/http-parser/.* -e ${CTEST_SCRIPT_DIRECTORY}/src/miniz/.* -e ${CTEST_SCRIPT_DIRECTORY}/src/sha1/.* -r ${CTEST_SCRIPT_DIRECTORY} --object-directory=${CTEST_BINARY_DIRECTORY} -o ${CTEST_BINARY_DIRECTORY}/cov-html/index.html"
)

set(CTEST_COVERAGE_COMMAND "gcov")
#set(CTEST_COVERAGE_EXTRA_FLAGS "-b -c")
set(CTEST_CUSTOM_COVERAGE_EXCLUDE
${CTEST_CUSTOM_COVERAGE_EXCLUDE}
    ".*/tests/.*"
    ".*/src/http-parser/.*"
    ".*/src/sha1/.*"
)
set(CTEST_MEMORYCHECK_COMMAND "valgrind")
#set(CTEST_MEMORYCHECK_TYPE "ThreadSanitizer")
ctest_start("Experimental")
#ctest_start("Continuous")
ctest_configure()
ctest_build()
ctest_test()
ctest_coverage()

include(CTestCoverageCollectGCOV)
ctest_coverage_collect_gcov(
    TARBALL gcov.tar
    SOURCE ${CTEST_SOURCE_DIRECTORY}
    BUILD ${CTEST_BINARY_DIRECTORY}
    GCOV_COMMAND ${CTEST_COVERAGE_COMMAND}
)
# if(EXISTS "${CTEST_BINARY_DIRECTORY}/gcov.tar")
#     ctest_submit(CDASH_UPLOAD "${CTEST_BINARY_DIRECTORY}/gcov.tar"
#     CDASH_UPLOAD_TYPE GcovTar)
# endif()

#ctest_memcheck()
#ctest_submit()
