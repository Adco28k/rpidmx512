/**
 * @file display.h
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

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdarg.h>
#include <stdint.h>

#include "displayset.h"

#include "display7segment.h"

#if defined (BARE_METAL)
# include "console.h"
#endif

static constexpr uint32_t DISPLAY_SLEEP_TIMEOUT_DEFAULT	= 5;

enum class DisplayType {
	BW_UI_1602,
	BW_LCD_1602,
	PCF8574T_1602,
	PCF8574T_2004,
	SSD1306,
	UNKNOWN
};

class Display {
public:
	Display(uint32_t nCols, uint32_t nRows);
	Display(DisplayType tDisplayType = DisplayType::SSD1306);
	~Display(void);

#if !defined(NO_HAL)
	void SetSleep(bool bSleep);
	bool isSleep(void) {
		return m_bIsSleep;
	}

	void Run(void);
#endif

	void Cls(void);
	void ClearLine(uint8_t nLine);

	void PutChar(int c);
	void PutString(const char *pText);

	int Write(uint8_t, const char *);
	int Printf(uint8_t, const char *, ...);

	void TextLine(uint8_t, const char *, uint8_t);

	void TextStatus(const char *pText);
	void TextStatus(const char *pText, uint16_t n7SegmentData, uint32_t nConsoleColor = UINT32_MAX);
	void TextStatus(const char *pText, TDisplay7SegmentMessages msg, uint32_t nConsoleColor = UINT32_MAX) {	// TODO Subject for removal
		TextStatus(pText, static_cast<uint16_t>(msg), nConsoleColor);
	}
	void TextStatus(const char *pText, uint8_t nValue7Segment, bool bHex = false);


	void Status(uint16_t n7SegmentData);
	void Status(TDisplay7SegmentMessages msg) {	// TODO Subject for removal
		Status(static_cast<uint16_t>(msg));
	}
	void Status(uint8_t nValue7Segment, bool bHex = false);

	bool isDetected(void) {
		return m_LcdDisplay == 0 ? false : true;
	}

	DisplayType GetDetectedType(void) {
		return m_tType;
	}

#if defined(ENABLE_CURSOR_MODE)
	void SetCursor(CursorMode EnumTCursorOnOff);
#endif

	void SetCursorPos(uint8_t nCol, uint8_t nRow);

	void SetSleepTimeout(uint32_t nSleepTimeout = DISPLAY_SLEEP_TIMEOUT_DEFAULT) {
		m_nSleepTimeout = 1000 * 60 * nSleepTimeout;
	}
	uint32_t GetSleepTimeout(void) {
		return m_nSleepTimeout / 1000 / 60;
	}

	uint32_t getCols(void) {
		return m_nCols;
	}

	uint32_t getRows(void) {
		return m_nRows;
	}

	bool isHave7Segment(void) {
		return m_bHave7Segment;
	}

	static Display* Get(void) {
		return s_pThis;
	}

private:
	void Detect(uint32_t nCols, uint32_t nRows);
	void Init7Segment(void);
	uint16_t Get7SegmentData(uint8_t nValue);

private:
	uint32_t m_nCols;
	uint32_t m_nRows;
	DisplayType m_tType;
	DisplaySet *m_LcdDisplay;
	bool m_bIsSleep;
	bool m_bHave7Segment;
#if !defined(NO_HAL)
	uint32_t m_nMillis;
#endif
	uint32_t m_nSleepTimeout;

	static Display *s_pThis;
};

#endif /* DISPLAY_H_ */
