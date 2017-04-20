/*
 * The MIT License (MIT)
 *
 * Copyright (c) <2017> <Stephan Gatzka>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <sys/epoll.h>

#include "fff.h"
#include "unity.h"

#include "cio_linux_epoll.h"

DEFINE_FFF_GLOBALS

FAKE_VALUE_FUNC(int, epoll_create, int)
FAKE_VALUE_FUNC(int, close, int)

void setUp(void)
{
	FFF_RESET_HISTORY();

	RESET_FAKE(epoll_create);
	RESET_FAKE(close);
}

static void test_create_loop(void)
{
	struct cio_linux_eventloop_epoll loop;
	enum cio_error err = cio_linux_eventloop_init(&loop);
	TEST_ASSERT_EQUAL(cio_success, err);
	TEST_ASSERT_EQUAL(1, epoll_create_fake.call_count);

	cio_linux_eventloop_destroy(&loop);
	TEST_ASSERT_EQUAL(1, close_fake.call_count);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_create_loop);
	return UNITY_END();
}
