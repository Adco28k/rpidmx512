/**
 * @file mcp4822.h
 *
 */
/* Copyright (C) 2018 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef MCP4822_H_
#define MCP4822_H_

#include <stdint.h>
#include <stdbool.h>

#include "device_info.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool mcp4822_start(device_info_t *);

extern void mcp4822_write_a(const device_info_t *, uint16_t);
extern void mcp4822_write_b(const device_info_t *, uint16_t);
extern void mcp4822_write_ab(const device_info_t *, uint16_t, uint16_t);

#ifdef __cplusplus
}
#endif

#endif /* MCP4822_H_ */
