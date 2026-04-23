#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "Ani_Data_Serever_PC.h"
#include "LightingSocketClient.h"
#include "Util/CLightingDB.h"

// 使用 OutputDebugString 输出日志
void LightingDbgPrint(LPCTSTR pszFmt, ...)
{
	TCHAR szBuf[512];
	va_list args;
	va_start(args, pszFmt);
	_vsntprintf_s(szBuf, _TRUNCATE, pszFmt, args);
	va_end(args);
	OutputDebugString(szBuf);
}

// CLightingSocketClient

CLightingSocketClient::CLightingSocketClient()
	: m_bConnected(FALSE)
	, m_pHandler(nullptr)
	, m_hAutoTestTimer(NULL)
	, m_hAutoTestStopEvent(NULL)
	, m_hReconnectThread(NULL)
	, m_hReconnectStopEvent(NULL)
	, m_dwReconnectInterval(30000)
	, m_dwMaxReconnectAttempts(0)
{
}

CLightingSocketClient::~CLightingSocketClient()
{
	Close();
}

void CLightingSocketClient::SetConnectionParameters(const CString& ip, const CString& port)
{
	m_strServerIP = ip;
	m_strServerPort = port;
}

bool CLightingSocketClient::Connect(const CString& ip, const CString& port)
{
	SetConnectionParameters(ip, port);

	LightingDbgPrint(_T("[Lighting] Connecting to %s:%s...\n"), ip, port);
	//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Connecting to %s:%s"), ip, port));

	if (!ConnectTo(ip, port, AF_INET, SOCK_STREAM))
	{
		LightingDbgPrint(_T("[Lighting] Connection failed!\n"));
		//theApp.m_pLightingLog->LOG_INFO(_T("Connection failed!"));
		m_bConnected = FALSE;
		theApp.m_LightingConectStatus = FALSE;
		
		StartReconnectThread();
		return false;
	}

	LightingDbgPrint(_T("[Lighting] Connected successfully!\n"));
	//theApp.m_pLightingLog->LOG_INFO(_T("Connected successfully!"));

	if (!WatchComm())
	{
		m_bConnected = FALSE;
		theApp.m_LightingConectStatus = FALSE;
		CloseComm();
		
		StartReconnectThread();
		return false;
	}

	m_bConnected = TRUE;
	theApp.m_LightingConectStatus = TRUE;

	if (_ttoi(theApp.m_strLightingAutoTest) != 0)
	{
		StartAutoTestTimer(10000);
	}
	else
	{
		LightingDbgPrint(_T("[Lighting] Auto-test disabled by config, skipping timer...\n"));
		//theApp.m_pLightingLog->LOG_INFO(_T("Auto-test disabled by config, skipping timer"));
	}
	return true;
}

void CLightingSocketClient::Close()
{
	StopAutoTestTimer();
	StopReconnectThread();
	StopComm();
	CloseComm();
	m_bConnected = FALSE;
	theApp.m_LightingConectStatus = FALSE;
}

BOOL CLightingSocketClient::IsConnected()
{
	return m_bConnected;
}

CString CLightingSocketClient::EncodeSlots(const int slots[4])
{
	CString encoded;
	for (int i = 0; i < 4; ++i)
	{
		int v = slots[i];
		if (v <= 0)
			encoded.Append(_T("00"));
		else
			encoded.AppendFormat(_T("%02d"), v);
	}
	return encoded;
}

void CLightingSocketClient::SendStart(CString curStr, CString maxStr)
{
	if (!m_bConnected)
	{
		return;
	}

	CString cmd;
	cmd.Format(_T("Start$%s$%s@"), curStr, maxStr);

	m_csSocketSend.Lock();

	CStringA cmdA(cmd);
	LPBYTE pData = reinterpret_cast<LPBYTE>(const_cast<CHAR*>(static_cast<LPCSTR>(cmdA)));

	WriteComm(pData, static_cast<DWORD>(cmdA.GetLength()), 100L);

	theApp.m_VisionLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] [MC -> Lighting] %s"),
		GetNowSystemTimeMilliseconds(), cmd));

	m_csSocketSend.Unlock();
}

void CLightingSocketClient::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	CString strDebug;
	strDebug.Format(_T("[Lighting] OnDataReceived: dwCount=%u\n"), dwCount);
	OutputDebugString(strDebug);
	//theApp.m_pLightingLog->LOG_INFO(strDebug);

	CString strChunk;
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount,
		strChunk.GetBuffer(static_cast<int>(dwCount) + 1), static_cast<int>(dwCount) + 1);
	strChunk.ReleaseBuffer(static_cast<int>(dwCount));

	m_recvCache += strChunk;

	LightingDbgPrint(_T("[Lighting -> MC] RECV raw: '%s', cache: '%s'\n"), strChunk, m_recvCache);
	//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("RECV raw: '%s', cache: '%s'"), strChunk, m_recvCache));

	int atPos = -1;
	while ((atPos = m_recvCache.Find(_T('@'))) >= 0)
	{
		CString msg = m_recvCache.Left(atPos);
		m_recvCache = m_recvCache.Mid(atPos + 1);

		msg.Trim();
		if (msg.IsEmpty())
			continue;

		HandleSingleMessage(msg);
	}
}

void CLightingSocketClient::OnEvent(UINT uEvent, LPVOID /*lpvData*/)
{
	switch (uEvent)
	{
	case EVT_CONSUCCESS:
		m_bConnected = TRUE;
		theApp.m_LightingConectStatus = TRUE;
		LightingDbgPrint(_T("[Lighting] Connection established!\n"));
		//theApp.m_pLightingLog->LOG_INFO(_T("Connection established"));
		if (_ttoi(theApp.m_strLightingAutoTest) != 0)
		{
			StartAutoTestTimer(10000);
		}
		else
		{
			LightingDbgPrint(_T("[Lighting] Auto-test disabled by config, skipping timer...\n"));
			//theApp.m_pLightingLog->LOG_INFO(_T("Auto-test disabled by config"));
		}
		break;
	case EVT_CONDROP:
	case EVT_CONFAILURE:
		{
			CSingleLock lock(&m_csReconnect, TRUE);

			m_bConnected = FALSE;
			theApp.m_LightingConectStatus = FALSE;
			LightingDbgPrint(_T("[Lighting] Connection lost!\n"));
			//theApp.m_pLightingLog->LOG_INFO(_T("Connection lost"));
			if (_ttoi(theApp.m_strLightingAutoTest) != 0)
			{
				StopAutoTestTimer();
			}
			StopComm();
			
			StartReconnectThread();
		}
		break;
	default:
		break;
	}
}

void CLightingSocketClient::SetEventHandler(ILightingEventHandler* pHandler)
{
	m_pHandler = pHandler;
}

void CLightingSocketClient::HandleSingleMessage(const CString& msg)
{
	LightingDbgPrint(_T("[Lighting -> MC] HANDLE msg: '%s'\n"), msg);
	//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("HANDLE msg: '%s'"), msg));

	if (msg.CompareNoCase(_T("Running")) == 0)
	{
		HandleRunning();
	}
	else if (msg.CompareNoCase(_T("SnapFN")) == 0)
	{
		HandleSnapFN();
	}
	else if (msg.Left(3).CompareNoCase(_T("FN$")) == 0)
	{
		CString payload = msg.Mid(3);
		HandleFNResult(payload);
	}
	else
	{
		theApp.m_pLightingSendReceiverLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] [Lighting -> MC] Unknown message: %s"), 
			GetNowSystemTimeMilliseconds(), msg));
	}
}

void CLightingSocketClient::HandleRunning()
{
	LightingDbgPrint(_T("[Lighting -> MC] Received: Running@\n"));
	//theApp.m_pLightingLog->LOG_INFO(_T("Received: Running@"));

	if (m_pHandler == nullptr)
	{
		LightingDbgPrint(_T("[Lighting -> MC] ERROR: m_pHandler is nullptr!\n"));
		//theApp.m_pLightingLog->LOG_INFO(_T("ERROR: m_pHandler is nullptr!"));
		return;
	}
	m_pHandler->OnLightingRunning();
}

void CLightingSocketClient::HandleSnapFN()
{
	LightingDbgPrint(_T("[Lighting -> MC] Received: SnapFN@\n"));
	//theApp.m_pLightingLog->LOG_INFO(_T("Received: SnapFN@"));

	if (m_pHandler == nullptr)
	{
		LightingDbgPrint(_T("[Lighting -> MC] ERROR: m_pHandler is nullptr!\n"));
		//theApp.m_pLightingLog->LOG_INFO(_T("ERROR: m_pHandler is nullptr!"));
		return;
	}
	m_pHandler->OnLightingSnapFN();
}

void CLightingSocketClient::HandleFNResult(const CString& payload)
{
	LightingDbgPrint(_T("[Lighting -> MC] Received: FN$%s@\n"), payload);
	//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Received: FN$%s@"), payload));

	CString cleanPayload = payload;
	cleanPayload.Remove(_T('$'));

	if (cleanPayload.GetLength() != 8)
	{
		LightingDbgPrint(_T("[Lighting -> MC] FN$ payload length invalid after clean: '%s' (len=%d)\n"), 
			cleanPayload, cleanPayload.GetLength());
		//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("FN$ payload length invalid after clean: '%s' (len=%d)"), 
		//	cleanPayload, cleanPayload.GetLength()));
		return;
	}

	int resultCode[4] = { 0, };
	for (int i = 0; i < 4; ++i)
	{
		CString tmp = cleanPayload.Mid(i * 2, 2);
		resultCode[i] = _ttoi(tmp);
		LightingDbgPrint(_T("[Lighting -> MC] FN$ resultCode[%d]=%d\n"), i, resultCode[i]);
		//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("FN$ resultCode[%d]=%d"), i, resultCode[i]));
	}

	if (m_pHandler == nullptr)
	{
		LightingDbgPrint(_T("[Lighting -> MC] ERROR: m_pHandler is nullptr!\n"));
		//theApp.m_pLightingLog->LOG_INFO(_T("ERROR: m_pHandler is nullptr!"));
		return;
	}

	m_pHandler->OnLightingResult(resultCode);
}

//==============================================================================
// 自动发送 Start 命令的定时器实现
//==============================================================================

void CLightingSocketClient::StartAutoTestTimer(DWORD dwDelayMs)
{
	StopAutoTestTimer();

	LightingDbgPrint(_T("[Lighting] Starting auto-test timer (delay: %ums)...\n"), dwDelayMs);
	//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Starting auto-test timer (delay: %ums)"), dwDelayMs));

	m_hAutoTestStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hAutoTestStopEvent)
		return;

	m_hAutoTestTimer = CreateThread(NULL, 0, AutoTestTimerThread, this, 0, NULL);
	if (!m_hAutoTestTimer)
	{
		CloseHandle(m_hAutoTestStopEvent);
		m_hAutoTestStopEvent = NULL;
		return;
	}
}

void CLightingSocketClient::StopAutoTestTimer()
{
	LightingDbgPrint(_T("[Lighting] Stopping auto-test timer...\n"));
	//theApp.m_pLightingLog->LOG_INFO(_T("Stopping auto-test timer"));

	if (m_hAutoTestStopEvent)
	{
		SetEvent(m_hAutoTestStopEvent);
	}

	if (m_hAutoTestTimer)
	{
		WaitForSingleObject(m_hAutoTestTimer, INFINITE);
		CloseHandle(m_hAutoTestTimer);
		m_hAutoTestTimer = NULL;
	}

	if (m_hAutoTestStopEvent)
	{
		CloseHandle(m_hAutoTestStopEvent);
		m_hAutoTestStopEvent = NULL;
	}

	LightingDbgPrint(_T("[Lighting] Auto-test timer stopped.\n"));
	//theApp.m_pLightingLog->LOG_INFO(_T("Auto-test timer stopped"));
}

DWORD WINAPI CLightingSocketClient::AutoTestTimerThread(LPVOID lpParam)
{
	CLightingSocketClient* pThis = static_cast<CLightingSocketClient*>(lpParam);
	if (!pThis)
		return 0;

	DWORD threadId = GetCurrentThreadId();
	LightingDbgPrint(_T("[Lighting] Auto-test timer started (delay: %ums), ThreadID=%lu\n"), 10000, threadId);
	//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Auto-test timer started (delay: %ums), ThreadID=%lu"), 10000, threadId));

	HANDLE handles[2] = { pThis->m_hAutoTestStopEvent, NULL };
	DWORD dwRet = WaitForMultipleObjects(1, handles, FALSE, 10000);

	if (dwRet == WAIT_TIMEOUT)
	{
		LightingDbgPrint(_T("[Lighting] Auto-test timer triggered, sending Start command... ThreadID=%lu\n"), threadId);
		//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Auto-test timer triggered, sending Start command, ThreadID=%lu"), threadId));

		for (int i = 0; i < 4; ++i)
			theApp.m_bLightingActiveSlot[i] = TRUE;

		for (int i = 0; i < 4; ++i)
		{
			if (i == 0)
			{
				theApp.m_VisionLog->LOG_INFO(_T("[DBG] Clearing ivs_lcd_idmap table before inserting new records"));
				CLightingDB::Get().ClearLightingIdMap();
			}

			int fixtureNo = i + 1;
			CString strBarcode;
			strBarcode.Format(_T("SIM_BARCODE_%d"), fixtureNo);
			
			CString strUniqueID;
			CStringSupport::GetGuid(strUniqueID);
			
			CString strMarkID;
			strMarkID.Format(_T("%02d"), fixtureNo);
			
			CLightingDB::Get().UpdateLightingIdMap(fixtureNo, strUniqueID, strBarcode, strMarkID);
		}
		
		pThis->SendStart(_T("01020304"), _T("01020304"));
	}
	else
	{
		LightingDbgPrint(_T("[Lighting] Auto-test timer cancelled\n"));
		//theApp.m_pLightingLog->LOG_INFO(_T("Auto-test timer cancelled"));
	}

	return 0;
}

//==============================================================================
// 连接重试线程实现
//==============================================================================

void CLightingSocketClient::StartReconnectThread()
{
	CSingleLock lock(&m_csReconnect, TRUE);

	StopReconnectThread();

	LightingDbgPrint(_T("[Lighting] Starting reconnect thread...\n"));
	//theApp.m_pLightingLog->LOG_INFO(_T("Starting reconnect thread"));

	m_hReconnectStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hReconnectStopEvent)
	{
		LightingDbgPrint(_T("[Lighting] Failed to create reconnect stop event\n"));
		//theApp.m_pLightingLog->LOG_INFO(_T("Failed to create reconnect stop event"));
		return;
	}

	m_hReconnectThread = CreateThread(NULL, 0, ReconnectThread, this, 0, NULL);
	if (!m_hReconnectThread)
	{
		LightingDbgPrint(_T("[Lighting] Failed to create reconnect thread\n"));
		//theApp.m_pLightingLog->LOG_INFO(_T("Failed to create reconnect thread"));
		CloseHandle(m_hReconnectStopEvent);
		m_hReconnectStopEvent = NULL;
		return;
	}
}

void CLightingSocketClient::StopReconnectThread()
{
	if (m_hReconnectStopEvent)
	{
		SetEvent(m_hReconnectStopEvent);
	}

	if (m_hReconnectThread)
	{
		WaitForSingleObject(m_hReconnectThread, INFINITE);
		CloseHandle(m_hReconnectThread);
		m_hReconnectThread = NULL;
	}

	if (m_hReconnectStopEvent)
	{
		CloseHandle(m_hReconnectStopEvent);
		m_hReconnectStopEvent = NULL;
	}

	LightingDbgPrint(_T("[Lighting] Reconnect thread stopped\n"));
	//theApp.m_pLightingLog->LOG_INFO(_T("Reconnect thread stopped"));
}

DWORD WINAPI CLightingSocketClient::ReconnectThread(LPVOID lpParam)
{
	CLightingSocketClient* pThis = static_cast<CLightingSocketClient*>(lpParam);
	if (!pThis)
		return 0;

	LightingDbgPrint(_T("[Lighting] Reconnect thread started\n"));
	//theApp.m_pLightingLog->LOG_INFO(_T("Reconnect thread started"));

	DWORD dwAttemptCount = 0;
	HANDLE hStopEvent = pThis->m_hReconnectStopEvent;
	CString strIP = pThis->m_strServerIP;
	CString strPort = pThis->m_strServerPort;

	while (TRUE)
	{
		if (WaitForSingleObject(hStopEvent, 0) == WAIT_OBJECT_0)
		{
			LightingDbgPrint(_T("[Lighting] Reconnect thread received stop signal\n"));
			//theApp.m_pLightingLog->LOG_INFO(_T("Reconnect thread received stop signal"));
			break;
		}

		if (strIP.IsEmpty() || strPort.IsEmpty())
		{
			LightingDbgPrint(_T("[Lighting] Invalid connection parameters, stopping reconnect\n"));
			//theApp.m_pLightingLog->LOG_INFO(_T("Invalid connection parameters, stopping reconnect"));
			break;
		}

		if (pThis->m_bConnected)
		{
			LightingDbgPrint(_T("[Lighting] Connection already established, stopping reconnect\n"));
			//theApp.m_pLightingLog->LOG_INFO(_T("Connection already established, stopping reconnect"));
			break;
		}

		dwAttemptCount++;
		
		if (pThis->m_dwMaxReconnectAttempts > 0 && dwAttemptCount > pThis->m_dwMaxReconnectAttempts)
		{
			LightingDbgPrint(_T("[Lighting] Max reconnect attempts reached (%d), stopping\n"), pThis->m_dwMaxReconnectAttempts);
			//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Max reconnect attempts reached (%d), stopping"), pThis->m_dwMaxReconnectAttempts));
			break;
		}

		LightingDbgPrint(_T("[Lighting] Reconnect attempt %d to %s:%s...\n"), dwAttemptCount, strIP, strPort);
		//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Reconnect attempt %d to %s:%s"), dwAttemptCount, strIP, strPort));

		pThis->StopComm();
		pThis->CloseComm();

		if (pThis->ConnectTo(strIP, strPort, AF_INET, SOCK_STREAM))
		{
			LightingDbgPrint(_T("[Lighting] Reconnect successful!\n"));
			//theApp.m_pLightingLog->LOG_INFO(_T("Reconnect successful!"));

			if (pThis->WatchComm())
			{
				pThis->m_bConnected = TRUE;
				theApp.m_LightingConectStatus = TRUE;

				if (_ttoi(theApp.m_strLightingAutoTest) != 0)
				{
					pThis->StartAutoTestTimer(10000);
				}
				
				break;
			}
			else
			{
				LightingDbgPrint(_T("[Lighting] Reconnect failed: Failed to start communication thread\n"));
				//theApp.m_pLightingLog->LOG_INFO(_T("Reconnect failed: Failed to start communication thread"));
				pThis->CloseComm();
			}
		}
		else
		{
			LightingDbgPrint(_T("[Lighting] Reconnect attempt %d failed\n"), dwAttemptCount);
			//theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Reconnect attempt %d failed"), dwAttemptCount));
		}

		if (WaitForSingleObject(hStopEvent, pThis->m_dwReconnectInterval) == WAIT_OBJECT_0)
		{
			LightingDbgPrint(_T("[Lighting] Reconnect thread received stop signal during wait\n"));
			//theApp.m_pLightingLog->LOG_INFO(_T("Reconnect thread received stop signal during wait"));
			break;
		}
	}

	return 0;
}

#endif // _SYSTEM_AMTAFT_
