/*
 * SPDX-License-Identifier: MIT
 *
 * The MIT License (MIT)
 *
 * Copyright (c) <2020> <Stephan Gatzka>
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

#ifndef CIO_UART_H
#define CIO_UART_H

#include <stddef.h>

#include "cio_error_code.h"
#include "cio_export.h"
#include "cio_io_stream.h"
#include "cio_uart_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

struct cio_uart;

typedef void (*cio_uart_close_hook_t)(struct cio_uart *uart);

struct cio_uart {

	/**
	 * @privatesection
	 */
	struct cio_io_stream stream;
	cio_uart_close_hook_t close_hook;
	struct cio_uart_impl impl;
};

/**
 * @brief Get the number of UARTs.
 * 
 * @return The number of UARTs.
 */
CIO_EXPORT size_t cio_uart_get_number_of_uarts(void);

/**
 * @brief Detects the UART ports.
 * 
 * Detects the UART ports in a system and fills them into an
 * array of cio_uart @p ports. If there are more ports in a system
 * then provided by @p num_port_entries, only upto @p num_port_entries
 * will be filled into ports.
 * 
 * @param[out] ports A buffer array filled with the detected UART ports.
 * @param num_ports_entries The size of the @p ports buffer.
 * @param[out] num_detected_ports The number of UARTs filled into @p ports.
 * 
 * @return ::CIO_SUCCESS on success.
 */
CIO_EXPORT enum cio_error cio_uart_get_ports(struct cio_uart ports[], size_t num_ports_entries, size_t *num_detected_ports);

#ifdef __cplusplus
}
#endif

#endif
