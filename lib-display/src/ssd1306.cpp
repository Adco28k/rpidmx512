/**
 * @file ssd1306.cpp
 *
 */
/* Copyright (C) 2017-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
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

#include "ssd1306.h"

#include "i2c.h"

#define SSD1306_LCD_WIDTH				128

#define SSD1306_COMMAND_MODE			0x00
#define SSD1306_DATA_MODE				0x40

#define SSD1306_CMD_SET_LOWCOLUMN		0x00
#define SSD1306_CMD_SET_HIGHCOLUMN		0x10
#define SSD1306_CMD_SET_MEMORYMODE		0x20
#define SSD1306_CMD_SET_COLUMNADDR		0x21
#define SSD1306_CMD_SET_PAGEADDR		0x22
#define SSD1306_CMD_SET_STARTLINE		0x40
#define SSD1306_CMD_SET_CONTRAST		0x81
#define SSD1306_CMD_SET_CHARGEPUMP		0x8D
#define SSD1306_CMD_SEGREMAP			0xA0
#define SSD1306_CMD_OUPUT_RAM			0xA4
#define SSD1306_CMD_DISPLAY_NORMAL		0xA6
#define SSD1306_CMD_DISPLAY_INVERT		0xA7
#define SSD1306_CMD_SET_MULTIPLEX		0xA8
#define SSD1306_CMD_DISPLAY_OFF			0xAE
#define SSD1306_CMD_DISPLAY_ON			0xAF
#define SSD1306_CMD_SET_STARTPAGE		0xB0
#define SSD1306_CMD_COMSCAN_INC			0xC0
#define SSD1306_CMD_COMSCAN_DEC			0xC8
#define SSD1306_CMD_SET_DISPLAYOFFSET	0xD3
#define SSD1306_CMD_SET_DISPLAYCLOCKDIV	0xD5
#define SSD1306_CMD_SET_PRECHARGE		0xD9
#define SSD1306_CMD_SET_COMPINS			0xDA
#define SSD1306_CMD_SET_VCOMDETECT		0xDB

#define OLED_FONT8x6_CHAR_H				8
#define OLED_FONT8x6_CHAR_W				6
#define OLED_FONT8x6_COLS				(SSD1306_LCD_WIDTH / OLED_FONT8x6_CHAR_W)

static uint8_t _OledFont8x6[] __attribute__((aligned(4))) = {
	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00,
	0x40, 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00,
	0x40, 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00,
	0x40, 0x23, 0x13, 0x08, 0x64, 0x62, 0x00,
	0x40, 0x36, 0x49, 0x56, 0x20, 0x50, 0x00,
	0x40, 0x00, 0x08, 0x07, 0x03, 0x00, 0x00,
	0x40, 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00,
	0x40, 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00,
	0x40, 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x00,
	0x40, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00,
	0x40, 0x00, 0x80, 0x70, 0x30, 0x00, 0x00,
	0x40, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00,
	0x40, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00,
	0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00,
	0x40, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00,
	0x40, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00,
	0x40, 0x72, 0x49, 0x49, 0x49, 0x46, 0x00,
	0x40, 0x21, 0x41, 0x49, 0x4D, 0x33, 0x00,
	0x40, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00,
	0x40, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00,
	0x40, 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x00,
	0x40, 0x41, 0x21, 0x11, 0x09, 0x07, 0x00,
	0x40, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00,
	0x40, 0x46, 0x49, 0x49, 0x29, 0x1E, 0x00,
	0x40, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x40, 0x34, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x08, 0x14, 0x22, 0x41, 0x00,
	0x40, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00,
	0x40, 0x00, 0x41, 0x22, 0x14, 0x08, 0x00,
	0x40, 0x02, 0x01, 0x59, 0x09, 0x06, 0x00,
	0x40, 0x3E, 0x41, 0x5D, 0x59, 0x4E, 0x00,
	0x40, 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00,
	0x40, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00,
	0x40, 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00,
	0x40, 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00,
	0x40, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00,
	0x40, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00,
	0x40, 0x3E, 0x41, 0x41, 0x51, 0x73, 0x00,
	0x40, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00,
	0x40, 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00,
	0x40, 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00,
	0x40, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00,
	0x40, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00,
	0x40, 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x00,
	0x40, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00,
	0x40, 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00,
	0x40, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00,
	0x40, 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00,
	0x40, 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00,
	0x40, 0x26, 0x49, 0x49, 0x49, 0x32, 0x00,
	0x40, 0x03, 0x01, 0x7F, 0x01, 0x03, 0x00,
	0x40, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00,
	0x40, 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00,
	0x40, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00,
	0x40, 0x63, 0x14, 0x08, 0x14, 0x63, 0x00,
	0x40, 0x03, 0x04, 0x78, 0x04, 0x03, 0x00,
	0x40, 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00,
	0x40, 0x00, 0x7F, 0x41, 0x41, 0x41, 0x00,
	0x40, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00,
	0x40, 0x00, 0x41, 0x41, 0x41, 0x7F, 0x00,
	0x40, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00,
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00,
	0x40, 0x00, 0x03, 0x07, 0x08, 0x00, 0x00,
	0x40, 0x20, 0x54, 0x54, 0x78, 0x40, 0x00,
	0x40, 0x7F, 0x28, 0x44, 0x44, 0x38, 0x00,
	0x40, 0x38, 0x44, 0x44, 0x44, 0x28, 0x00,
	0x40, 0x38, 0x44, 0x44, 0x28, 0x7F, 0x00,
	0x40, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00,
	0x40, 0x00, 0x08, 0x7E, 0x09, 0x02, 0x00,
	0x40, 0x18, 0xA4, 0xA4, 0x9C, 0x78, 0x00,
	0x40, 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00,
	0x40, 0x00, 0x44, 0x7D, 0x40, 0x00, 0x00,
	0x40, 0x20, 0x40, 0x40, 0x3D, 0x00, 0x00,
	0x40, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00,
	0x40, 0x00, 0x41, 0x7F, 0x40, 0x00, 0x00,
	0x40, 0x7C, 0x04, 0x78, 0x04, 0x78, 0x00,
	0x40, 0x7C, 0x08, 0x04, 0x04, 0x78, 0x00,
	0x40, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00,
	0x40, 0xFC, 0x18, 0x24, 0x24, 0x18, 0x00,
	0x40, 0x18, 0x24, 0x24, 0x18, 0xFC, 0x00,
	0x40, 0x7C, 0x08, 0x04, 0x04, 0x08, 0x00,
	0x40, 0x48, 0x54, 0x54, 0x54, 0x24, 0x00,
	0x40, 0x04, 0x04, 0x3F, 0x44, 0x24, 0x00,
	0x40, 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00,
	0x40, 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00,
	0x40, 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00,
	0x40, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00,
	0x40, 0x4C, 0x90, 0x90, 0x90, 0x7C, 0x00,
	0x40, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00,
	0x40, 0x00, 0x08, 0x36, 0x41, 0x00, 0x00,
	0x40, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00,
	0x40, 0x00, 0x41, 0x36, 0x08, 0x00, 0x00,
	0x40, 0x02, 0x01, 0x02, 0x04, 0x02, 0x00,
	0x40, 0x3C, 0x26, 0x23, 0x26, 0x3C, 0x00
};

static const uint8_t oled_128x64_init[] __attribute__((aligned(4))) = {
		SSD1306_CMD_DISPLAY_OFF,
		SSD1306_CMD_SET_DISPLAYCLOCKDIV, 0x80, 		// The suggested value
		SSD1306_CMD_SET_MULTIPLEX, 0x3F,			// 1/64
		SSD1306_CMD_SET_DISPLAYOFFSET, 0x00,		// No offset
		SSD1306_CMD_SET_STARTLINE | 0x00,			// line #0
		SSD1306_CMD_SET_CHARGEPUMP, 0x14,
		SSD1306_CMD_SET_MEMORYMODE, 0x00,			// Horizontal addressing
		SSD1306_CMD_SEGREMAP | 0x01,				// Flip horizontally
		SSD1306_CMD_COMSCAN_DEC,					// Flip vertically
		SSD1306_CMD_SET_COMPINS, 0x12,
		SSD1306_CMD_SET_CONTRAST, 0x7F,				// 0x00 to 0xFF
		SSD1306_CMD_SET_PRECHARGE, 0xF1,
		SSD1306_CMD_SET_VCOMDETECT, 0x40,
		SSD1306_CMD_OUPUT_RAM,
		SSD1306_CMD_DISPLAY_NORMAL };

static const uint8_t oled_128x32_init[] __attribute__((aligned(4))) = {
		SSD1306_CMD_DISPLAY_OFF,
		SSD1306_CMD_SET_DISPLAYCLOCKDIV, 0x80, 		// The suggested value
		SSD1306_CMD_SET_MULTIPLEX, 0x1F,			// 1/32
		SSD1306_CMD_SET_DISPLAYOFFSET, 0x00,		// No offset
		SSD1306_CMD_SET_STARTLINE | 0x00,			// line #0
		SSD1306_CMD_SET_CHARGEPUMP, 0x14,
		SSD1306_CMD_SET_MEMORYMODE, 0x00,			// Horizontal addressing
		SSD1306_CMD_SEGREMAP | 0x01,				// Flip horizontally
		SSD1306_CMD_COMSCAN_DEC,					// Flip vertically
		SSD1306_CMD_SET_COMPINS, 0x02,
		SSD1306_CMD_SET_CONTRAST, 0x7F,				// 0x00 to 0xFF
		SSD1306_CMD_SET_PRECHARGE, 0xF1,
		SSD1306_CMD_SET_VCOMDETECT, 0x40,
		SSD1306_CMD_OUPUT_RAM,
		SSD1306_CMD_DISPLAY_NORMAL };

static uint8_t _ClearBuffer[133 + 1] __attribute__((aligned(4)));

Ssd1306 *Ssd1306::s_pThis = 0;

Ssd1306::Ssd1306(void) : m_nSlaveAddress(OLED_I2C_SLAVE_ADDRESS_DEFAULT), m_OledPanel(OLED_PANEL_128x64_8ROWS), m_bHaveSH1106(false) {
	assert(s_pThis == 0);
	s_pThis = this;

	InitMembers();
}

Ssd1306::Ssd1306(TOledPanel tOledPanel) : m_nSlaveAddress(OLED_I2C_SLAVE_ADDRESS_DEFAULT), m_OledPanel(tOledPanel), m_bHaveSH1106(false) {
	assert(s_pThis == 0);
	s_pThis = this;

	InitMembers();
}

Ssd1306::Ssd1306(uint8_t nSlaveAddress, TOledPanel tOledPanel) : m_OledPanel(tOledPanel), m_bHaveSH1106(false) {
	assert(s_pThis == 0);
	s_pThis = this;

	if (nSlaveAddress == 0) {
		m_nSlaveAddress = OLED_I2C_SLAVE_ADDRESS_DEFAULT;
	}

	InitMembers();
}

Ssd1306::~Ssd1306(void) {
	delete[] m_pShadowRam;
	m_pShadowRam = 0;
}

void Ssd1306::CheckSH1106(void) {
	// Check for columns 128-133
	SendCommand(SSD1306_CMD_SET_LOWCOLUMN | (128 & 0XF));
	SendCommand(SSD1306_CMD_SET_HIGHCOLUMN | (128));
	SendCommand(SSD1306_CMD_SET_STARTPAGE);

	const uint8_t aTestBytes[5] = { 0x40, 0xAA, 0xEE, 0xAA, 0xEE };
	SendData(aTestBytes, sizeof(aTestBytes));

	// Check for columns 128-133
	SendCommand(SSD1306_CMD_SET_LOWCOLUMN | (128 & 0XF));
	SendCommand(SSD1306_CMD_SET_HIGHCOLUMN | (128));
	SendCommand(SSD1306_CMD_SET_STARTPAGE);

	char aResultBytes[5];

	i2c_write(0x40);
	i2c_read(aResultBytes, sizeof(aResultBytes));

#ifndef NDEBUG
	printf("%.2x %.2x %.2x %.2x %.2x\n", aResultBytes[0], aResultBytes[1], aResultBytes[2], aResultBytes[3], aResultBytes[4]);
#endif

	m_bHaveSH1106 = (memcmp(&aTestBytes[1], &aResultBytes[1], 4) == 0);

#ifndef NDEBUG
	printf("m_bHaveSH1106=%d\n", m_bHaveSH1106);
#endif
}

bool Ssd1306::Start(void) {
	i2c_begin();

	Setup();

	if (!i2c_is_connected(m_nSlaveAddress)) {
		return false;
	}

	switch (m_OledPanel) {
	case OLED_PANEL_128x64_8ROWS:
		for (unsigned i = 0; i < sizeof(oled_128x64_init); i++) {
			SendCommand(oled_128x64_init[i]);
		}
		break;
	case OLED_PANEL_128x64_4ROWS:
		/* no break */
	case OLED_PANEL_128x32_4ROWS:
		for (unsigned i = 0; i < sizeof(oled_128x32_init); i++) {
			SendCommand(oled_128x32_init[i]);
		}
		break;
	default:
		return false;
	}

	for (unsigned i = 0; i < sizeof(_ClearBuffer); i++) {
		_ClearBuffer[i] = 0x00;
	}

	_ClearBuffer[0] = 0x40;

	CheckSH1106();

	Cls();

	SendCommand(SSD1306_CMD_DISPLAY_ON);

	return true;
}

void Ssd1306::Cls(void) {
	uint32_t nColumnAdd = 0;

	if (m_bHaveSH1106) {
		nColumnAdd = 4;
	}

	for (uint32_t nPage = 0; nPage < m_nPages; nPage++) {
		SendCommand(SSD1306_CMD_SET_LOWCOLUMN | (nColumnAdd & 0XF));
		SendCommand(SSD1306_CMD_SET_HIGHCOLUMN | (nColumnAdd));
		SendCommand(SSD1306_CMD_SET_STARTPAGE | nPage);

		switch (m_OledPanel) {
		case OLED_PANEL_128x64_8ROWS:
			SendData(reinterpret_cast<const uint8_t*>(&_ClearBuffer), nColumnAdd + SSD1306_LCD_WIDTH + 1);
			break;
		case OLED_PANEL_128x64_4ROWS:
			/* no break */
		case OLED_PANEL_128x32_4ROWS:
			SendData(reinterpret_cast<const uint8_t*>(&_ClearBuffer), nColumnAdd + SSD1306_LCD_WIDTH + 1);
			break;
		default:
			break;
		}
	}

	SendCommand(SSD1306_CMD_SET_LOWCOLUMN | (nColumnAdd & 0XF));
	SendCommand(SSD1306_CMD_SET_HIGHCOLUMN | (nColumnAdd));
	SendCommand(SSD1306_CMD_SET_STARTPAGE);

	m_nShadowRamIndex = 0;
	memset(m_pShadowRam, ' ', static_cast<size_t>(m_nCols * m_nRows));
}

void Ssd1306::PutChar(int c) {
	uint8_t i;

	if (c < 32 || c > 127) {
		c = 32;
		i = 0;
	} else {
		i = (c - 32);
	}

	m_pShadowRam[m_nShadowRamIndex++] = c;

	const uint8_t *base = _OledFont8x6 + (OLED_FONT8x6_CHAR_W + 1) * i;
	SendData(base, OLED_FONT8x6_CHAR_W + 1);
}

void Ssd1306::PutString(const char *pString) {
	const char *p = pString;

	for (uint32_t i = 0; *p != '\0'; i++) {
		PutChar(static_cast<int>(*p));
		p++;
	}
}

void Ssd1306::ClearLine(uint8_t nLine) {
	if (nLine > m_nRows) {
		return;
	}

	SetCursorPos(0, nLine - 1);
	SendData(reinterpret_cast<const uint8_t*>(&_ClearBuffer), SSD1306_LCD_WIDTH + 1);
	SetCursorPos(0, nLine - 1);
}

void Ssd1306::TextLine(uint8_t nLine, const char *pData, uint8_t nLength) {
	if (nLine > m_nRows) {
		return;
	}

	SetCursorPos(0, nLine - 1);
	Text(pData, nLength);
}

void Ssd1306::Text(const char *data, uint8_t nLength) {
	if (nLength > m_nCols) {
		nLength = m_nCols;
	}

	for (uint32_t i = 0; i < nLength; i++) {
		PutChar(data[i]);
	}
}

void Ssd1306::SetCursorPos(uint8_t col, uint8_t row) {
	if ((row > m_nRows) || (col > OLED_FONT8x6_COLS)) {
		return;
	}

	col = col * OLED_FONT8x6_CHAR_W;

	if (m_bHaveSH1106) {
		col += 4;
	}

	SendCommand(SSD1306_CMD_SET_LOWCOLUMN | (col & 0XF));
	SendCommand(SSD1306_CMD_SET_HIGHCOLUMN | (col >> 4));
	SendCommand(SSD1306_CMD_SET_STARTPAGE | row);

	m_nShadowRamIndex = (row * OLED_FONT8x6_COLS) + (col / OLED_FONT8x6_CHAR_W);

#if defined(ENABLE_CURSOR_MODE)
	if (m_tCursorMode == SET_CURSOR_ON) {
		SetCursorOff();
		SetCursorOn();
	} else if (static_cast<uint32_t>(m_tCursorMode) == static_cast<uint32_t>((SET_CURSOR_ON | SET_CURSOR_BLINK_ON))) {
		SetCursorOff();
		SetCursorBlinkOn();
	}
#endif
}

void Ssd1306::SetSleep(bool bSleep) {
	if (bSleep) {
		SendCommand(SSD1306_CMD_DISPLAY_OFF);
	} else {
		SendCommand(SSD1306_CMD_DISPLAY_ON);
	}
}

void Ssd1306::Setup(void) {
	i2c_set_address(m_nSlaveAddress);
	i2c_set_baudrate(I2C_FULL_SPEED);
}

void Ssd1306::InitMembers(void) {
	switch (m_OledPanel) {
	case OLED_PANEL_128x64_8ROWS:
		m_nCols = OLED_FONT8x6_COLS;
		m_nRows = 64 / OLED_FONT8x6_CHAR_H;
		break;
	case OLED_PANEL_128x64_4ROWS:			// Trick : 128x32
		/* no break */
	case OLED_PANEL_128x32_4ROWS:
		m_nCols = OLED_FONT8x6_COLS;
		m_nRows = 32 / OLED_FONT8x6_CHAR_H;
		break;
	default:
		m_nCols = OLED_FONT8x6_COLS;
		m_nRows = 64 / OLED_FONT8x6_CHAR_H;
		break;
	}

	m_nPages = (m_OledPanel == OLED_PANEL_128x64_8ROWS ? 8 : 4);

	m_pShadowRam = new char[OLED_FONT8x6_COLS * m_nRows];
	m_nShadowRamIndex = 0;
	memset(m_pShadowRam, ' ', static_cast<size_t>(OLED_FONT8x6_COLS * m_nRows));
}

void Ssd1306::SendCommand(uint8_t cmd) {
	Setup();
	i2c_write_reg_uint8(SSD1306_COMMAND_MODE, cmd);
}

void Ssd1306::SendData(const uint8_t *pData, uint32_t nLength) {
	Setup();
	i2c_write_nb(reinterpret_cast<const char*>(pData), nLength);
}

#ifndef NDEBUG
void Ssd1306::DumpShadowRam(void) {
	for (int i = 0; i < m_nRows; i++) {
		printf("%d: [%.*s]\n", i, OLED_FONT8x6_COLS, &m_pShadowRam[i * OLED_FONT8x6_COLS]);
	}
}
#endif

//TODO Below is not compatible with SH1106
#if defined(ENABLE_CURSOR_MODE)
void Ssd1306::SetCursor(CursorMode tCursorMode) {
	if (tCursorMode == m_tCursorMode) {
		return;
	}

	m_tCursorMode = tCursorMode;

	switch (static_cast<int>(tCursorMode)) {
	case SET_CURSOR_OFF:
		SetCursorOff();
		break;
	case SET_CURSOR_ON:
		SetCursorOn();
		break;
	case SET_CURSOR_ON | SET_CURSOR_BLINK_ON:
		SetCursorBlinkOn();
		break;
	default:
		break;
	}
}

void Ssd1306::SetCursorOn(void) {
	uint8_t data[OLED_FONT8x6_COLS + 1];

	m_nCursorOnCol = m_nShadowRamIndex % OLED_FONT8x6_COLS;
	m_nCursorOnRow =  m_nShadowRamIndex / OLED_FONT8x6_COLS;
	m_nCursorOnChar = m_pShadowRam[m_nShadowRamIndex] - 32;

	uint8_t *base = _OledFont8x6 + 1 + (OLED_FONT8x6_CHAR_W + 1) * m_nCursorOnChar;

	data[0] = 0x40;

	for (int i = 1 ; i <= OLED_FONT8x6_CHAR_W; i++) {
		data[i] = *base | 0x80;
		base++;
	}

	SendData(data, (OLED_FONT8x6_CHAR_W + 1));

	SendCommand(SSD1306_CMD_SET_LOWCOLUMN | ((m_nCursorOnCol * OLED_FONT8x6_CHAR_W) & 0XF));
	SendCommand(SSD1306_CMD_SET_HIGHCOLUMN | ((m_nCursorOnCol * OLED_FONT8x6_CHAR_W) >> 4));
	SendCommand(SSD1306_CMD_SET_STARTPAGE | m_nCursorOnRow);
}

void Ssd1306::SetCursorBlinkOn(void) {
	uint8_t data[OLED_FONT8x6_COLS + 1];

	m_nCursorOnCol = m_nShadowRamIndex % OLED_FONT8x6_COLS;
	m_nCursorOnRow =  m_nShadowRamIndex / OLED_FONT8x6_COLS;
	m_nCursorOnChar = m_pShadowRam[m_nShadowRamIndex] - 32;

	uint8_t *base = _OledFont8x6 + 1 + (OLED_FONT8x6_CHAR_W + 1) * m_nCursorOnChar;

	data[0] = 0x40;

	for (int i = 1 ; i <= OLED_FONT8x6_CHAR_W; i++) {
		data[i] = ~*base;
		base++;
	}

	SendData(data, (OLED_FONT8x6_CHAR_W + 1));

	SendCommand(SSD1306_CMD_SET_LOWCOLUMN | ((m_nCursorOnCol * OLED_FONT8x6_CHAR_W) & 0XF));
	SendCommand(SSD1306_CMD_SET_HIGHCOLUMN | ((m_nCursorOnCol * OLED_FONT8x6_CHAR_W) >> 4));
	SendCommand(SSD1306_CMD_SET_STARTPAGE | m_nCursorOnRow);
}

void Ssd1306::SetCursorOff(void) {
	const uint8_t col = m_nShadowRamIndex % OLED_FONT8x6_COLS;
	const uint8_t row =  m_nShadowRamIndex / OLED_FONT8x6_COLS;

	SendCommand(SSD1306_CMD_SET_LOWCOLUMN | ((m_nCursorOnCol * OLED_FONT8x6_CHAR_W) & 0XF));
	SendCommand(SSD1306_CMD_SET_HIGHCOLUMN | ((m_nCursorOnCol * OLED_FONT8x6_CHAR_W) >> 4));
	SendCommand(SSD1306_CMD_SET_STARTPAGE | m_nCursorOnRow);

	const uint8_t *base = _OledFont8x6 + (OLED_FONT8x6_CHAR_W + 1) * m_nCursorOnChar;

	SendData(base, (OLED_FONT8x6_CHAR_W + 1));

	SendCommand(SSD1306_CMD_SET_LOWCOLUMN | ((col * OLED_FONT8x6_CHAR_W) & 0XF));
	SendCommand(SSD1306_CMD_SET_HIGHCOLUMN | ((col * OLED_FONT8x6_CHAR_W) >> 4));
	SendCommand(SSD1306_CMD_SET_STARTPAGE | row);
}
#endif

