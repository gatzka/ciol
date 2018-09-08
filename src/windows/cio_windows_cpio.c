/*
 * The MIT License (MIT)
 *
 * Copyright (c) <2018> <Stephan Gatzka>
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

#include <Windows.h>
#include <stddef.h>

#include "cio_compiler.h"
#include "cio_error_code.h"
#include "cio_eventloop.h"
#include "windows/cio_eventloop_impl.h"

static const ULONG_PTR STOP_COMPLETION_KEY = 0x1;

enum cio_error cio_eventloop_init(struct cio_eventloop *loop)
{
	loop->loop_complion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	if (cio_unlikely(loop->loop_complion_port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}

	loop->go_ahead = true;

	return CIO_SUCCESS;
}

void cio_eventloop_destroy(const struct cio_eventloop *loop)
{
	CloseHandle(loop->loop_complion_port);
}

enum cio_error cio_eventloop_run(struct cio_eventloop *loop)
{
	while (cio_likely(loop->go_ahead)) {
		DWORD size = 0;
		ULONG_PTR completion_key = 0;
		OVERLAPPED *overlapped = NULL;
		BOOL ret = GetQueuedCompletionStatus(loop->loop_complion_port, &size, &completion_key, &overlapped, INFINITE);

		if (cio_unlikely(ret == false)) {
			if (cio_unlikely(overlapped == NULL)) {
				// An unrecoverable error occurred in the completion port. Wait for the next notification.
				continue;
			} else {
				DWORD last_error = GetLastError();
			}
		}

		if (completion_key == STOP_COMPLETION_KEY) {
			break;
		}

		struct cio_event_notifier *ev = (struct cio_event_notfier *)completion_key;
		ev->callback(ev->context);

	}

	return CIO_SUCCESS;
}

void cio_eventloop_cancel(struct cio_eventloop *loop)
{
	loop->go_ahead = false;
	PostQueuedCompletionStatus(loop->loop_complion_port, 0, STOP_COMPLETION_KEY, NULL);
}
