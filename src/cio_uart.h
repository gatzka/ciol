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
#include "cio_eventloop.h"
#include "cio_export.h"
#include "cio_io_stream.h"
#include "cio_uart_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

enum cio_uart_parity {
	CIO_UART_PARITY_EVEN,
	CIO_UART_PARITY_ODD,
	CIO_UART_PARITY_NONE,
	CIO_UART_PARITY_MARK,
	CIO_UART_PARITY_SPACE
};

enum cio_uart_num_stop_bits {
	CIO_UART_ONE_STOP_BIT,
	CIO_UART_TWO_STOP_BITS
};

enum cio_uart_num_data_bits {
	CIO_UART_5_DATA_BITS,
	CIO_UART_6_DATA_BITS,
	CIO_UART_7_DATA_BITS,
	CIO_UART_8_DATA_BITS
};

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

/**
 * @brief Initializes a UART port
 * 
 * @param port The port to be initialized.
 * @param loop The event loop the socket shall operate on.
 * @param close_hook A close hook function. If this parameter is non @c NULL,
 * the function will be called directly after
 * @ref cio_uart_close "closing" the cio_uart.
 * It is guaranteed the the cio library will not access any memory of
 * cio_uart that is passed to the close hook. Therefore
 * the hook could be used to free the memory of the UART.
 * 
 * @return ::CIO_SUCCESS on success.
 */
CIO_EXPORT enum cio_error cio_uart_init(struct cio_uart *port, struct cio_eventloop *loop, cio_uart_close_hook_t close_hook);

/**
 * @brief Closes a UART port.
 * 
 * Once a UART has been closed, no further communication is possible.
 * Closing the UART also closes the UART's cio_io_stream.
 * 
 * @param port The port to be closed.
 * 
 * @return ::CIO_SUCCESS on success.
 */
CIO_EXPORT enum cio_error cio_uart_close(struct cio_uart *port);

/**
 * @brief Set the new parity.
 * 
 * @param port The port on which the parity will be set.
 * @param parity The new parity.
 * 
 * @return ::CIO_SUCCESS on success.
 */
CIO_EXPORT enum cio_error cio_uart_set_parity(const struct cio_uart *port, enum cio_uart_parity parity);

/**
 * @brief Get the parity.
 * 
 * @param port The port from which the parity will be got.
 * @param[out] parity The parity will be stored here.
 * 
 * @return ::CIO_SUCCESS on success.
 */
CIO_EXPORT enum cio_error cio_uart_get_parity(const struct cio_uart *port, enum cio_uart_parity *parity);

/**
 * @brief Set the number of stop bits.
 * 
 * @param port The UART port to be configured.
 * @param num_stop_bits The new number of stop bits to be set.
 *
 * @return ::CIO_SUCCESS on success.
 */
CIO_EXPORT enum cio_error cio_uart_set_num_stop_bits(const struct cio_uart *port, enum cio_uart_num_stop_bits num_stop_bits);

/**
 * @brief Get the current number of stop bits.
 * 
 * @param port The UART port fro which the number of stop bits will be gathered.
 * @param[out] num_stop_bits The number of stop bits will be stored here.
 * 
 * @return ::CIO_SUCCESS on success.
 */
CIO_EXPORT enum cio_error cio_uart_get_num_stop_bits(const struct cio_uart *port, enum cio_uart_num_stop_bits *num_stop_bits);

/**
 * @brief Set the number of data bits.
 * 
 * @param port The UART to be configured.
 * @param num_data_bits The new number of data bits to be set.
 * 
 * @return ::CIO_SUCCESS on success.
 */
CIO_EXPORT enum cio_error cio_uart_set_num_data_bits(const struct cio_uart *port, enum cio_uart_num_data_bits num_data_bits);

/**
 * @brief Get the configured number of data bits.
 * 
 * @param port The UART port fro which the number of data bits will be gathered.
 * @param[out] num_data_bits The number of data bits will be stored here.
 * 
 * @return ::CIO_SUCCESS on success.
 */
CIO_EXPORT enum cio_error cio_uart_get_num_data_bits(const struct cio_uart *port, enum cio_uart_num_data_bits *num_data_bits);

#ifdef __cplusplus
}
#endif

#endif
