/**
 * @file oscclient.h
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

#ifndef OSCCLIENT_H_
#define OSCCLIENT_H_

#include <stdint.h>
#include <time.h>

#include "oscclientled.h"

struct OscClientDefault {
	static constexpr auto PORT_OUTGOING = 8000;
	static constexpr auto PORT_INCOMING = 9000;
	static constexpr auto PING_DELAY_SECONDS = 10;
};

struct OscClientMax {
	static constexpr uint32_t CMD_COUNT = 8;
	static constexpr uint32_t CMD_PATH_LENGTH = 64;
	static constexpr uint32_t LED_COUNT = 8;
	static constexpr uint32_t LED_PATH_LENGTH = 48;
};

class OscClient {
public:
	OscClient(void);
	~OscClient(void);

	void Start(void);
	void Stop(void);

	int Run(void);

	void Send(const char *pPath);
	void SendCmd(uint8_t nCmd);

	void Print(void);

	void SetServerIP(uint32_t nServerIP) {
		m_nServerIP = nServerIP;
	}
	uint32_t GetServerIP(void) {
		return m_nServerIP;
	}

	void SetPortOutgoing(uint16_t nPortOutgoing = OscClientDefault::PORT_OUTGOING);
	uint16_t GetPortOutgoing(void) {
		return m_nPortOutgoing;
	}

	void SetPortIncoming(uint16_t nPortIncoming = OscClientDefault::PORT_INCOMING);
	uint16_t GetPortIncoming(void) {
		return m_nPortIncoming;
	}

	void SetPingDisable(bool nPingDisable = true) {
		m_bPingDisable = nPingDisable;
	}
	bool GetPingDisable(void) {
		return m_bPingDisable;
	}

	void SetPingDelay(uint32_t nPingDelay = OscClientDefault::PING_DELAY_SECONDS);
	uint8_t GetPingDelay(void) {
		return m_nPingDelayMillis / 1000;
	}

	void CopyCmds(const char *pCmds, uint32_t nCount, uint32_t nLength);
	void CopyLeds(const char *pLeds, uint32_t nCount, uint32_t nLength);

	void SetLedHandler(OscClientLed *pOscClientLed);

private:
	bool HandleLedMessage(void);

private:
	uint32_t m_nServerIP;
	uint16_t m_nPortOutgoing;
	uint16_t m_nPortIncoming;
	int32_t m_nHandle;
	bool m_bPingDisable;
	uint32_t m_nPingDelayMillis;
	bool m_bPingSent;
	bool m_bPongReceived;
	char *m_pBuffer;
	uint16_t m_nBytesReceived;
	uint32_t m_nCurrenMillis;
	uint32_t m_nPreviousMillis;
	uint32_t m_nPingTimeMillis;
	char *m_pCmds;
	char *m_pLeds;
	OscClientLed *m_pOscClientLed;
};

#endif /* OSCCLIENT_H_ */
