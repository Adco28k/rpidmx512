/**
 * @file fota_params.c
 *
 */
/* Copyright (C) 2016-2018 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#include <stdint.h>
#include <stdbool.h>

#include "read_config_file.h"
#include "sscan.h"

#ifndef ALIGNED
 #define ALIGNED __attribute__ ((aligned (4)))
#endif

static const char PARAMS_FILE_NAME[] ALIGNED = "fota.txt";
static const char PARAMS_SERVER[] ALIGNED = "server";

static uint32_t fota_params_server = 0;

uint32_t fota_params_get_server(void) {
	return fota_params_server;
}

static void process_line_read(const char *line) {
	uint32_t value32;

	if (sscan_ip_address(line, PARAMS_SERVER, &value32) == SSCAN_OK) {
		fota_params_server = value32;
	}
}

bool fota_params_init(void) {
	return read_config_file(PARAMS_FILE_NAME, &process_line_read);
}
