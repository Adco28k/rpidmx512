/**
 * @file displayudfhandler.h
 *
 */
/* Copyright (C) 2019-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#ifndef DISPLAYUDFHANDLER_H_
#define DISPLAYUDFHANDLER_H_

#include <stdint.h>

#include "displayudf.h"

#include "artnetnode.h"

#include "artnetdisplay.h"
#include "lightsetdisplay.h"
#include "networkdisplay.h"

class DisplayUdfHandler: public ArtNetDisplay, public LightSetDisplay, public NetworkDisplay {
public:
	DisplayUdfHandler(void) {}
	~DisplayUdfHandler(void) {}

	void ShowShortName(const char *pShortName) {
		DisplayUdf::Get()->ShowNodeName(ArtNetNode::Get());
	}

	void ShowLongName(const char *pLongName) {}

	void ShowUniverseSwitch(uint8_t nPortIndex, uint8_t nAddress) {
		DisplayUdf::Get()->ShowUniverse(ArtNetNode::Get());
	}

	void ShowNetSwitch(uint8_t nAddress) {
		DisplayUdf::Get()->ShowUniverse(ArtNetNode::Get());
	}

	void ShowSubnetSwitch(uint8_t nAddress) {
		DisplayUdf::Get()->ShowUniverse(ArtNetNode::Get());
	}

	void ShowMergeMode(uint8_t nPortIndex, TMerge tMerge) {
		DisplayUdf::Get()->ShowUniverse(ArtNetNode::Get());
	}

	void ShowPortProtocol(uint8_t nPortIndex, TPortProtocol tPortProtocol) {
		DisplayUdf::Get()->ShowUniverse(ArtNetNode::Get());
	}

	void ShowDmxStartAddress(void) {
		DisplayUdf::Get()->ShowDmxStartAddress();
	}

	void ShowIp(void) {
		DisplayUdf::Get()->ShowIpAddress();
	}

	void ShowNetMask(void) {
		DisplayUdf::Get()->ShowNetmask();
	}

	void ShowHostName(void) {
		DisplayUdf::Get()->ShowHostName();
	}
};

#endif /* DISPLAYUDFHANDLER_H_ */
