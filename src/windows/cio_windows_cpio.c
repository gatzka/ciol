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

#include "cio_compiler.h"
#include "cio_error_code.h"
#include "cio_eventloop.h"
#include "windows/cio_eventloop_impl.h"

enum cio_error cio_eventloop_init(struct cio_eventloop *loop)
{
	return CIO_SUCCESS;
}

void cio_eventloop_destroy(const struct cio_eventloop *loop)
{
}

enum cio_error cio_windows_eventloop_add(const struct cio_eventloop *loop, struct cio_event_notifier *ev)
{
	return CIO_SUCCESS;
}

enum cio_error cio_windows_eventloop_register_read(const struct cio_eventloop *loop, struct cio_event_notifier *ev)
{
	return CIO_SUCCESS;
}

enum cio_error cio_windows_eventloop_unregister_read(const struct cio_eventloop *loop, struct cio_event_notifier *ev)
{
	return CIO_SUCCESS;
}

enum cio_error cio_windows_eventloop_register_write(const struct cio_eventloop *loop, struct cio_event_notifier *ev)
{
	return CIO_SUCCESS;
}

enum cio_error cio_windows_eventloop_unregister_write(const struct cio_eventloop *loop, struct cio_event_notifier *ev)
{
	return CIO_SUCCESS;
}

void cio_windows_eventloop_remove(struct cio_eventloop *loop, const struct cio_event_notifier *ev)
{
}

enum cio_error cio_eventloop_run(struct cio_eventloop *loop)
{
	return CIO_SUCCESS;
}

void cio_eventloop_cancel(struct cio_eventloop *loop)
{
}