/**
 * @file artnet4params.cpp
 *
 */
/**
 * Art-Net Designed by and Copyright Artistic Licence Holdings Ltd.
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

#include <stdint.h>
#include <string.h>
#ifndef NDEBUG
 #include <stdio.h>
#endif
#include <cassert>

#include "debug.h"

#include "artnet4params.h"
#include "artnet4paramsconst.h"
#include "artnetparamsconst.h"

#include "readconfigfile.h"
#include "sscan.h"

#include "spiflashstore.h"

ArtNet4Params::ArtNet4Params(ArtNet4ParamsStore *pArtNet4ParamsStore):
	ArtNetParams(pArtNet4ParamsStore == 0 ? 0 : SpiFlashStore::Get()->GetStoreArtNet()),
	m_pArtNet4ParamsStore(pArtNet4ParamsStore)
{
	m_tArtNet4Params.nSetList = 0;
	m_tArtNet4Params.bMapUniverse0 = false;
}

ArtNet4Params::~ArtNet4Params(void) {
}

bool ArtNet4Params::Load(void) {
	DEBUG_ENTRY

	ArtNetParams::Load();

	m_tArtNet4Params.nSetList = 0;

	ReadConfigFile configfile(ArtNet4Params::staticCallbackFunction, this);

	if (configfile.Read(ArtNetParamsConst::FILE_NAME)) {
		// There is a configuration file
		if (m_pArtNet4ParamsStore != 0) {
			m_pArtNet4ParamsStore->Update(&m_tArtNet4Params);
		}
	} else if (m_pArtNet4ParamsStore != 0) {
		m_pArtNet4ParamsStore->Copy(&m_tArtNet4Params);
	} else {
		DEBUG_EXIT
		return false;
	}

	DEBUG_EXIT
	return true;
}

void ArtNet4Params::Load(const char* pBuffer, uint32_t nLength) {
	DEBUG_ENTRY

	assert(pBuffer != 0);
	assert(nLength != 0);
	assert(m_pArtNet4ParamsStore != 0);

	ArtNetParams::Load(pBuffer, nLength);

	if (m_pArtNet4ParamsStore == 0) {
		return;
	}

	m_tArtNet4Params.nSetList = 0;

	ReadConfigFile config(ArtNet4Params::staticCallbackFunction, this);

	config.Read(pBuffer, nLength);

	m_pArtNet4ParamsStore->Update(&m_tArtNet4Params);

	DEBUG_EXIT
}

void ArtNet4Params::callbackFunction(const char* pLine) {
	assert(pLine != 0);

	uint8_t value8;

	if (Sscan::Uint8(pLine, ArtNet4ParamsConst::MAP_UNIVERSE0, &value8) == SSCAN_OK) {
		if (value8 != 0) {
			m_tArtNet4Params.bMapUniverse0 = 1;
			m_tArtNet4Params.nSetList |= ArtNet4ParamsMask::MAP_UNIVERSE0;
		} else {
			m_tArtNet4Params.bMapUniverse0 = 0;
			m_tArtNet4Params.nSetList &= ~ArtNet4ParamsMask::MAP_UNIVERSE0;
		}
		return;
	}
}

void ArtNet4Params::Dump(void) {
#ifndef NDEBUG
	ArtNetParams::Dump();

	printf("%s::%s \'%s\':\n", __FILE__, __FUNCTION__, ArtNetParamsConst::FILE_NAME);

	if(isMaskSet(ArtNet4ParamsMask::MAP_UNIVERSE0)) {
		printf(" %s=%d [%s]\n", ArtNet4ParamsConst::MAP_UNIVERSE0, m_tArtNet4Params.bMapUniverse0, BOOL2STRING::Get(m_tArtNet4Params.bMapUniverse0));
	}
#endif
}

void ArtNet4Params::staticCallbackFunction(void *p, const char *s) {
	assert(p != 0);
	assert(s != 0);

	(static_cast<ArtNet4Params*>(p))->callbackFunction(s);
}
