/**
 * @file oscstring.cpp
 *
 */
/* Copyright (C) 2016-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#include "oscstring.h"
#include "osc.h"

/*
 * OSC-string
 * A sequence of non-null ASCII characters followed by a null,
 * followed by 0-3 additional null characters to make the total number of bits a multiple of 32.
 */

int OSCString::Validate(void *pData, int nSize) {
	int nLength = 0;
	char *pSrc = reinterpret_cast<char*>(pData);

	int i = 0;

	for (i = 0; i < nSize; ++i) {
		if (pSrc[i] == '\0') {
			nLength = 4 * (i / 4 + 1);
			break;
		}
	}

	if (0 == nLength) {
		return -NOT_TERMINATED;
	}

	if (nLength > nSize) {
		return -INVALID_SIZE;
	}

	for (; i < nLength; ++i) {
		if (pSrc[i] != '\0') {
			return -NONE_ZERO_IN_PADDING;
		}
	}

	return nLength;
}
