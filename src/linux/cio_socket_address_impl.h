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

#ifndef CIO_SOCKET_ADDRESS_IMPL_H
#define CIO_SOCKET_ADDRESS_IMPL_H

#include <sys/socket.h>

#include "cio_error_code.h"
#include "cio_export.h"
#include "cio_inet_address.h"
#include "cio_inet4_socket_address.h"
#include "cio_inet6_socket_address.h"
#include "cio_socket_address.h"
#include "cio_unix_address.h"

#ifdef __cplusplus
extern "C" {
#endif

struct cio_inet_address;
struct cio_socket_address;

struct cio_socket_address_common {
	struct sockaddr addr;
};

union cio_socket_address_impl {
	struct cio_socket_address_common socket_address;
	struct cio_inet4_socket_address inet_addr4;
	struct cio_inet6_socket_address inet_addr6;
	struct cio_unix_address unix_address;
};

/**
 * @brief Initializes a inet socket address from an IP address and a port number.
 *
 * @param sock_address The inet socket address to be initalized.
 * @param inet_address The IP address.
 * @param port The port number.
 *
 * @return ::CIO_SUCCESS for success.
 */
CIO_EXPORT enum cio_socket_address_family cio_socket_address_get_family(const struct cio_socket_address *endpoint);
CIO_EXPORT enum cio_error cio_init_inet_socket_address(struct cio_socket_address *sock_address, const struct cio_inet_address *inet_address, uint16_t port);

#ifdef __cplusplus
}
#endif

#endif
