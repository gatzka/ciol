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

#include <stdint.h>
#include <string.h>

#include "cio_allocator.h"
#include "cio_buffered_stream.h"
#include "cio_compiler.h"
#include "cio_error_code.h"
#include "cio_io_stream.h"
#include "cio_string.h"
#include "cio_write_buffer.h"

#undef MIN
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

static inline size_t unread_bytes(const struct cio_buffered_stream *bs)
{
	return bs->unread_bytes;
}

static inline size_t space_in_buffer(const struct cio_buffered_stream *bs)
{
	return bs->read_buffer_size - unread_bytes(bs);
}

static void handle_read(struct cio_io_stream *context, void *handler_context, enum cio_error err, uint8_t *buf, size_t bytes_transferred)
{
	(void)context;
	(void)buf;

	struct cio_buffered_stream *bs = handler_context;
	bs->last_error = err;
	if (likely(err == cio_success)) {
		bs->unread_bytes += bytes_transferred;
	}

	bs->read_job(bs);
}

static void fill_buffer(struct cio_buffered_stream *bs)
{
	if (bs->read_buffer != bs->read_from_ptr) {
		memmove(bs->read_buffer, bs->read_from_ptr, unread_bytes(bs));
		bs->read_from_ptr = bs->read_buffer;
	}

	bs->stream->read_some(bs->stream, bs->read_buffer + unread_bytes(bs), space_in_buffer(bs), handle_read, bs);
}

static void internal_read(struct cio_buffered_stream *bs)
{
	size_t available = unread_bytes(bs);
	if (available > 0) {
		size_t to_read = MIN(available, bs->read_info.bytes_to_read);
		bs->read_handler(bs, bs->read_handler_context, cio_success, bs->read_from_ptr, to_read);
		bs->read_from_ptr += to_read;
		bs->unread_bytes -= to_read;
	} else {
		fill_buffer(bs);
	}
}

static void bs_read(struct cio_buffered_stream *bs, size_t num, cio_buffered_stream_read_handler handler, void *handler_context)
{
	bs->read_info.bytes_to_read = num;
	bs->read_job = internal_read;
	bs->read_handler = handler;
	bs->read_handler_context = handler_context;
	bs->last_error = cio_success;
	internal_read(bs);
}

static void internal_read_until(struct cio_buffered_stream *bs)
{
	const uint8_t *haystack = bs->read_from_ptr;
	const char *needle = bs->read_info.until.delim;
	size_t needle_length = bs->read_info.until.delim_length;
	uint8_t *found = cio_memmem(haystack, unread_bytes(bs), needle, needle_length);
	if (found != NULL) {
		ptrdiff_t diff = (found + needle_length) - bs->read_from_ptr;
		bs->read_handler(bs, bs->read_handler_context, cio_success, bs->read_from_ptr, diff);
		bs->read_from_ptr += diff;
	} else {
		fill_buffer(bs);
	}
}

static void bs_read_until(struct cio_buffered_stream *bs, const char *delim, cio_buffered_stream_read_handler handler, void *handler_context)
{
	if (unlikely((bs == NULL) || (handler == NULL))) {
		handler(bs, handler_context, cio_invalid_argument, NULL, 0);
		return;
	}

	if (unlikely(delim == NULL)) {
		handler(bs, handler_context, cio_invalid_argument, NULL, 0);
		return;
	}

	bs->read_info.until.delim = delim;
	bs->read_info.until.delim_length = strlen(delim);
	bs->read_job = internal_read_until;
	bs->read_handler = handler;
	bs->read_handler_context = handler_context;
	bs->last_error = cio_success;
	internal_read_until(bs);
}

static void internal_read_exactly(struct cio_buffered_stream *bs)
{
	if (unlikely(bs->last_error != cio_success)) {
		bs->read_handler(bs, bs->read_handler_context, bs->last_error, NULL, 0);
		return;
	}

	if (bs->read_info.bytes_to_read <= unread_bytes(bs)) {
		bs->read_handler(bs, bs->read_handler_context, cio_success, bs->read_from_ptr, bs->read_info.bytes_to_read);
		bs->read_from_ptr += bs->read_info.bytes_to_read;
		bs->unread_bytes -= bs->read_info.bytes_to_read;
	} else {
		fill_buffer(bs);
	}
}

static void bs_read_exactly(struct cio_buffered_stream *bs, size_t num, cio_buffered_stream_read_handler handler, void *handler_context)
{
	if (unlikely((bs == NULL) || (handler == NULL))) {
		handler(bs, handler_context, cio_invalid_argument, NULL, 0);
		return;
	}

	if (unlikely(num > bs->read_buffer_size)) {
		handler(bs, handler_context, cio_message_too_long, NULL, 0);
		return;
	}

	bs->read_info.bytes_to_read = num;
	bs->read_job = internal_read_exactly;
	bs->read_handler = handler;
	bs->read_handler_context = handler_context;
	bs->last_error = cio_success;
	internal_read_exactly(bs);
}

static inline bool buffer_partially_written(const struct cio_write_buffer *wb, size_t bytes_transferred)
{
	return wb->length > bytes_transferred;
}

static inline bool buffer_is_temp_buffer(const struct cio_buffered_stream *bs, const struct cio_write_buffer *wb)
{
	return &bs->wb == wb;
}

static void handle_write(struct cio_io_stream *io_stream, void *handler_context, const struct cio_write_buffer_head *buffer, enum cio_error err, size_t bytes_transferred)
{
	struct cio_buffered_stream *bs = handler_context;
	if (unlikely(err != cio_success)) {

		while (!cio_write_buffer_queue_empty(&bs->wbh)) {
			struct cio_write_buffer *wb = cio_write_buffer_queue_dequeue(&bs->wbh);
			if (!buffer_is_temp_buffer(bs, wb)) {
				cio_write_buffer_queue_tail(bs->original_wbh, wb);
			}
		}

		bs->write_handler(bs, bs->write_handler_context, buffer, err);
	}

	while (bytes_transferred != 0) {
		struct cio_write_buffer *wb = cio_write_buffer_queue_dequeue(&bs->wbh);
		if (buffer_partially_written(wb, bytes_transferred)) {
			if (!buffer_is_temp_buffer(bs, wb)) {
				cio_write_buffer_queue_tail(bs->original_wbh, wb);
			}

			const void *new_data = &((const uint8_t *)wb->data)[bytes_transferred];
			size_t new_length = wb->length - bytes_transferred;
			cio_write_buffer_init(&bs->wb, new_data, new_length);
			cio_write_buffer_queue_head(&bs->wbh, &bs->wb);
			bytes_transferred = 0;
		} else {
			bytes_transferred -= wb->length;
			if (!buffer_is_temp_buffer(bs, wb)) {
				cio_write_buffer_queue_tail(bs->original_wbh, wb);
			}
		}
	}

	if (cio_write_buffer_queue_empty(&bs->wbh)) {
		bs->write_handler(bs, bs->write_handler_context, bs->original_wbh, cio_success);
	} else {
		bs->stream->write_some(io_stream, &bs->wbh, handle_write, bs);
	}
}

static void bs_write(struct cio_buffered_stream *bs, struct cio_write_buffer_head *buffer, cio_buffered_stream_write_handler handler, void *handler_context)
{
	if (unlikely((bs == NULL) || (buffer == NULL) || (handler == NULL))) {
		handler(bs, handler_context, buffer, cio_invalid_argument);
		return;
	}

	bs->write_handler = handler;
	bs->write_handler_context = handler_context;
	bs->original_wbh = buffer;

	while (!cio_write_buffer_queue_empty(buffer)) {
		struct cio_write_buffer *wb = cio_write_buffer_queue_dequeue(buffer);
		cio_write_buffer_queue_tail(&bs->wbh, wb);
	}

	bs->stream->write_some(bs->stream, &bs->wbh, handle_write, bs);
}

static void bs_close(struct cio_buffered_stream *context)
{
	context->read_buffer_allocator->free(context->read_buffer_allocator, context->read_buffer);
	context->stream->close(context->stream);
}

enum cio_error cio_buffered_stream_init(struct cio_buffered_stream *bs,
                                        struct cio_io_stream *stream,
                                        size_t read_buffer_size,
                                        struct cio_allocator *read_buffer_allocator)
{
	if (unlikely((bs == NULL) || (read_buffer_allocator == NULL) || (stream == NULL))) {
		return cio_invalid_argument;
	}

	bs->stream = stream;
	struct cio_buffer read_buffer = read_buffer_allocator->alloc(read_buffer_allocator, read_buffer_size);
	if (unlikely(read_buffer.address == NULL)) {
		return cio_not_enough_memory;
	}

	bs->read_buffer = read_buffer.address;
	bs->read_buffer_size = read_buffer.size;
	bs->read_buffer_allocator = read_buffer_allocator;
	bs->read_from_ptr = read_buffer.address;
	bs->unread_bytes = 0;

	cio_write_buffer_head_init(&bs->wbh);

	bs->read = bs_read;
	bs->read_exactly = bs_read_exactly;
	bs->read_until = bs_read_until;
	bs->write = bs_write;
	bs->close = bs_close;

	return cio_success;
}
