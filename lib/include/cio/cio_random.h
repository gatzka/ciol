/*
 * SPDX-License-Identifier: MIT
 *
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

#ifndef CIO_RANDOM_H
#define CIO_RANDOM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cio/cio_error_code.h"
#include "cio/cio_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @private
 */
struct pcg_state_setseq_64 {
	uint64_t state; // RNG state. All values are possible.
	uint64_t inc; // Controls which RNG sequence (stream) is selected. Must *always* be odd.
};

/**
 * @private
 */
typedef struct pcg_state_setseq_64 cio_rng_t;

CIO_EXPORT enum cio_error cio_random_seed_rng(cio_rng_t *rng);
CIO_EXPORT void cio_random_get_bytes(cio_rng_t *rng, void *bytes, size_t num_bytes);

/**
 * @private
 */
enum cio_error cio_entropy_get_bytes(void *bytes, size_t num_bytes);

#ifdef __cplusplus
}
#endif

#endif
