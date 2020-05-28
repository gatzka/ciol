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

#include <Windows.h>
#include <setupapi.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <tchar.h>

#include "cio_compiler.h"
#include "cio_error_code.h"
#include "cio_uart.h"
#include "cio_util.h"

static enum cio_error stream_read(struct cio_io_stream *stream, struct cio_read_buffer *buffer, cio_io_stream_read_handler_t handler, void *handler_context)
{
	if (cio_unlikely((stream == NULL) || (buffer == NULL) || (handler == NULL))) {
		return CIO_INVALID_ARGUMENT;
	}

	struct cio_uart *uart = cio_container_of(stream, struct cio_uart, stream);

	return CIO_SUCCESS;
}

static enum cio_error stream_write(struct cio_io_stream *stream, struct cio_write_buffer *buffer, cio_io_stream_write_handler_t handler, void *handler_context)
{
	if (cio_unlikely((stream == NULL) || (buffer == NULL) || (handler == NULL))) {
		return CIO_INVALID_ARGUMENT;
	}

	struct cio_uart *uart = cio_container_of(stream, struct cio_uart, stream);
	size_t chain_length = cio_write_buffer_get_num_buffer_elements(buffer);
	return CIO_SUCCESS;
}

static enum cio_error stream_close(struct cio_io_stream *stream)
{
	struct cio_uart *port = cio_container_of(stream, struct cio_uart, stream);
	return cio_uart_close(port);
}

static char *get_string_value_form_registry(HDEVINFO dev_info_set, SP_DEVINFO_DATA dev_info)
{
	HKEY dev_key = SetupDiOpenDevRegKey(dev_info_set, &dev_info, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
	if (cio_unlikely(dev_key == INVALID_HANDLE_VALUE)) {
		return NULL;
	}

	//Get the length of the registry entry
	DWORD entry_type = 0;
	DWORD value_size = 0;
	LONG err = RegQueryValueExW(dev_key, L"PortName", NULL, &entry_type, NULL, &value_size);
	if (cio_unlikely(err != ERROR_SUCCESS)) {
		goto out;
	}

	if (cio_unlikely(entry_type != REG_SZ)) {
		goto out;	
	}

	LPTSTR port_name = (LPTSTR)_malloca(value_size);
	if (cio_unlikely(port_name == NULL)) {
		goto out;
	}

	DWORD return_size = value_size;
	err = RegQueryValueEx(dev_key, TEXT("PortName"), NULL, &entry_type, (LPBYTE)port_name, &return_size);
	if (cio_unlikely(err != ERROR_SUCCESS)) {
		goto free_port_name_mem;
	}

	RegCloseKey(dev_key);
	int utf8_len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, port_name, return_size, NULL, 0, NULL, NULL);
	if (cio_unlikely(utf8_len == 0)) {
		goto free_port_name_mem;	
	}

	char* utf8_port_name = malloc((size_t)utf8_len + 1);
	if (cio_unlikely(utf8_port_name == NULL)) {
		goto free_port_name_mem;
	}

	utf8_len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, port_name, return_size, utf8_port_name, utf8_len + 1, NULL, NULL);
	if (cio_unlikely(utf8_len == 0)) {
		goto free_utf8_port_name_mem;
	}

	_freea(port_name);
	return utf8_port_name;

free_utf8_port_name_mem:
	_freea(utf8_port_name);
free_port_name_mem:
	_freea(port_name);
out:
	RegCloseKey(dev_key);
	return NULL;
}

static bool is_com_port(const char *port_name)
{
	size_t len = strlen(port_name);
	if (len > 3) {
		if (strncmp(port_name, "COM", 3) == 0) {
			if (isdigit(port_name[3]) != 0) {
				return true;
			}
		}
	}

	return false;
}

size_t cio_uart_get_number_of_uarts(void)
{
	DWORD num_guids = 0;
	SetupDiClassGuidsFromName(TEXT("Ports"), NULL, 0, &num_guids);

	GUID *guid_buffer = (GUID *)_malloca(num_guids * sizeof(*guid_buffer));
	if (cio_unlikely(guid_buffer == NULL)) {
		return 0;
	}

	if (SetupDiClassGuidsFromNameW(L"Ports", guid_buffer, num_guids, &num_guids) == FALSE) {
		goto free_guid_buffer;
	}

	HDEVINFO dev_info_set = SetupDiGetClassDevs(guid_buffer, NULL, NULL, DIGCF_PRESENT);
	if (dev_info_set == INVALID_HANDLE_VALUE) {
		goto free_guid_buffer;
	}

	SP_DEVINFO_DATA dev_info;
	dev_info.cbSize = sizeof(dev_info);
	DWORD member_index = 0;
	size_t detected_ports = 0;
	BOOL more_items = SetupDiEnumDeviceInfo(dev_info_set, member_index, &dev_info);

	while (more_items) {
		char *port_name = get_string_value_form_registry(dev_info_set, dev_info);
		if (cio_likely(port_name != NULL)) {
			if (is_com_port(port_name)) {
				detected_ports++;
			}

			free(port_name);
		}

		member_index++;
		dev_info.cbSize = sizeof(dev_info);
		more_items = SetupDiEnumDeviceInfo(dev_info_set, member_index, &dev_info);
	}

	SetupDiDestroyDeviceInfoList(dev_info_set);
	return detected_ports;

free_guid_buffer:
	_freea(guid_buffer);
	return 0;
}

enum cio_error cio_uart_get_ports(struct cio_uart ports[], size_t num_ports_entries, size_t *num_detected_ports)
{
	if (cio_unlikely(num_ports_entries == 0)) {
		*num_detected_ports = 0;
		return CIO_SUCCESS;
	}

	GUID *guid_buffer = (GUID *)_malloca(num_ports_entries * sizeof(*guid_buffer));
	if (cio_unlikely(guid_buffer == NULL)) {
		return CIO_NO_MEMORY;
	}

	enum cio_error error = CIO_SUCCESS;
	DWORD num_guids = (DWORD)num_ports_entries;
	if (SetupDiClassGuidsFromName(TEXT("Ports"), guid_buffer, num_guids, &num_guids) == FALSE) {
		error = (enum cio_error) - (signed int)GetLastError();
		goto free_guid_buffer;
	}
	
	HDEVINFO dev_info_set = SetupDiGetClassDevs(guid_buffer, NULL, NULL, DIGCF_PRESENT);
	if (dev_info_set == INVALID_HANDLE_VALUE) {
		error = (enum cio_error) - (signed int)GetLastError();
		goto free_guid_buffer;
	}

	SP_DEVINFO_DATA dev_info;
	dev_info.cbSize = sizeof(dev_info);
	DWORD member_index = 0;
	size_t detected_ports = 0;
	BOOL more_items = SetupDiEnumDeviceInfo(dev_info_set, member_index, &dev_info);

	while (more_items) {
		char *port_name = get_string_value_form_registry(dev_info_set, dev_info);
		if (cio_likely(port_name != NULL)) {
			if (is_com_port(port_name)) {
				strncpy_s(ports[detected_ports].impl.name, sizeof(ports[detected_ports].impl.name), port_name, _TRUNCATE);
				detected_ports++;
			}

			free(port_name);
			if (detected_ports >= num_ports_entries) {
				goto out;
			}
		}

		member_index++;
		dev_info.cbSize = sizeof(dev_info);
		more_items = SetupDiEnumDeviceInfo(dev_info_set, member_index, &dev_info);
	}

out:
	*num_detected_ports = detected_ports;
	SetupDiDestroyDeviceInfoList(dev_info_set);
free_guid_buffer:
	_freea(guid_buffer);
	return CIO_SUCCESS;
}

enum cio_error cio_uart_init(struct cio_uart *port, struct cio_eventloop *loop, cio_uart_close_hook_t close_hook)
{
	return CIO_OPERATION_NOT_SUPPORTED;
}

enum cio_error cio_uart_close(struct cio_uart *port)
{
	if (cio_unlikely(port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}

	return CIO_SUCCESS;
}

enum cio_error cio_uart_set_parity(const struct cio_uart *port, enum cio_uart_parity parity)
{
	if (cio_unlikely(port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}

	return CIO_OPERATION_NOT_SUPPORTED;
}

enum cio_error cio_uart_get_parity(const struct cio_uart *port, enum cio_uart_parity *parity)
{
	if (cio_unlikely(port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}

	return CIO_OPERATION_NOT_SUPPORTED;
}

enum cio_error cio_uart_set_num_stop_bits(const struct cio_uart *port, enum cio_uart_num_stop_bits num_stop_bits)
{
	if (cio_unlikely(port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}

	return CIO_OPERATION_NOT_SUPPORTED;
}

enum cio_error cio_uart_get_num_stop_bits(const struct cio_uart *port, enum cio_uart_num_stop_bits *num_stop_bits)
{
	if (cio_unlikely(port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}

	return CIO_OPERATION_NOT_SUPPORTED;
}

enum cio_error cio_uart_set_num_data_bits(const struct cio_uart *port, enum cio_uart_num_data_bits num_data_bits)
{
	return CIO_OPERATION_NOT_SUPPORTED;
}

enum cio_error cio_uart_get_num_data_bits(const struct cio_uart *port, enum cio_uart_num_data_bits *num_data_bits)
{
	if (cio_unlikely(port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}

	return CIO_OPERATION_NOT_SUPPORTED;
}

enum cio_error cio_uart_set_flow_control(const struct cio_uart *port, enum cio_uart_flow_control flow_control)
{
	if (cio_unlikely(port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}

	return CIO_OPERATION_NOT_SUPPORTED;
}

enum cio_error cio_uart_get_flow_control(const struct cio_uart *port, enum cio_uart_flow_control *flow_control)
{
	if (cio_unlikely(port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}

	return CIO_OPERATION_NOT_SUPPORTED;
}

enum cio_error cio_uart_set_baud_rate(const struct cio_uart *port, enum cio_uart_baud_rate baud_rate)
{
	if (cio_unlikely(port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}

	return CIO_OPERATION_NOT_SUPPORTED;
}

enum cio_error cio_uart_get_baud_rate(const struct cio_uart *port, enum cio_uart_baud_rate *baud_rate)
{
	if (cio_unlikely(port == NULL)) {
		return CIO_INVALID_ARGUMENT;
	}
	return CIO_OPERATION_NOT_SUPPORTED;
}

struct cio_io_stream *cio_uart_get_io_stream(struct cio_uart *port)
{
	if (cio_unlikely(port == NULL)) {
		return NULL;
	}

	return &port->stream;
}

const char *cio_uart_get_name(const struct cio_uart *port)
{
	if (cio_unlikely(port == NULL)) {
		return NULL;
	}

	return port->impl.name;
}
