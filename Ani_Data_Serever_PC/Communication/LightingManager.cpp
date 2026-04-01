#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "DlgMainView.h"
#include "DlgMainLog.h"
#include "LightingManager.h"

// 点灯检软件使用的 4 个治具位默认值（全开）
static const int DEFAULT_USED_SLOTS[4] = { 1, 2, 3, 4 };
static const int DEFAULT_MAX_SLOTS[4]  = { 1, 2, 3, 4 };

// 使用 OutputDebugString 输出日志
static void LightingDbgPrint(LPCTSTR pszFmt, ...)
{
	TCHAR szBuf[512];
	va_list args;
	va_start(args, pszFmt);
	_vsntprintf_s(szBuf, _TRUNCATE, pszFmt, args);
	va_end(args);
	OutputDebugString(szBuf);
}

CLightingManager::CLightingManager()
	: m_bConnected(FALSE)
	, m_pHandler(nullptr)
	, m_hAutoTestTimer(NULL)
	, m_hAutoTestStopEvent(NULL)
	, m_hReconnectThread(NULL)
	, m_hReconnectStopEvent(NULL)
	, m_dwReconnectInterval(30000)  // 默认重试间隔：30秒
	, m_dwMaxReconnectAttempts(0)  // 默认无限重试
{
	// 数据库连接已改为 TLS 方式，每个线程自动拥有独立连接
}

CLightingManager::~CLightingManager()
{
	Close();
	// 数据库连接已改为 TLS 方式，线程结束时自动释放
}

void CLightingManager::SetConnectionParameters(const CString& ip, const CString& port)
{
	m_strServerIP = ip;
	m_strServerPort = port;
}

bool CLightingManager::ConnectToLighting(const CString& ip, const CString& port)
{
	// 保存连接参数
	SetConnectionParameters(ip, port);

	// 作为 TCP Client 连接到点灯检软件
	LightingDbgPrint(_T("[Lighting] Connecting to %s:%s...\n"), ip, port);
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Connecting to %s:%s"), ip, port));

	if (!ConnectTo(ip, port, AF_INET /*IPPROTO_TCP*/, SOCK_STREAM))
	{
		LightingDbgPrint(_T("[Lighting] Connection failed!\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("Connection failed!"));
		m_bConnected = FALSE;
		theApp.m_LightingConectStatus = FALSE;
		
		// 启动重试线程
		StartReconnectThread();
		return false;
	}

	LightingDbgPrint(_T("[Lighting] Connected successfully!\n"));
	theApp.m_pLightingLog->LOG_INFO(_T("Connected successfully!"));

	// 启动 Socket 线程，开始接收数据
	if (!WatchComm())
	{
		m_bConnected = FALSE;
		theApp.m_LightingConectStatus = FALSE;
		CloseComm();
		
		// 启动重试线程
		StartReconnectThread();
		return false;
	}

	// TCP Client 模式不会触发 OnEvent，连接成功后手动设置状态
	m_bConnected = TRUE;
	theApp.m_LightingConectStatus = TRUE;

	if (_ttoi(theApp.m_strLightingAutoTest) != 0)
	{
		// 启动自动测试定时器
		StartAutoTestTimer(10000);
	}
	else
	{
		LightingDbgPrint(_T("[Lighting] Auto-test disabled by config, skipping timer...\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("Auto-test disabled by config, skipping timer"));
	}
	return true;
}

void CLightingManager::Close()
{
	StopAutoTestTimer();
	StopReconnectThread();
	StopComm();
	CloseComm();
	m_bConnected = FALSE;
	theApp.m_LightingConectStatus = FALSE;
	// 数据库连接已改为 TLS 方式
}

BOOL CLightingManager::getConectCheck()
{
	return m_bConnected;
}

CString CLightingManager::EncodeSlots(const int slots[4])
{
	// slots[0..3] -> "01020304" 形式，空位用 "00"
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

void CLightingManager::SendStart(const int usedSlots[4], const int maxSlots[4])
{
	if (!m_bConnected)
	{
		// 连接未建立时直接返回
		return;
	}

	CString curStr = EncodeSlots(usedSlots);
	CString maxStr = EncodeSlots(maxSlots);

	CString cmd;
	cmd.Format(_T("Start$%s$%s@"), curStr, maxStr);

	m_csSocketSend.Lock();

	// Lighting 软件按 ASCII 收包，这里直接按多字节发送
	CStringA cmdA(cmd);
	LPBYTE pData = reinterpret_cast<LPBYTE>(const_cast<CHAR*>(static_cast<LPCSTR>(cmdA)));

	WriteComm(pData, static_cast<DWORD>(cmdA.GetLength()), 100L);

	theApp.m_pLightingSendReceiverLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] [MC -> Lighting] %s"), 
		GetNowSystemTimeMilliseconds(), cmd));

	m_csSocketSend.Unlock();
}

void CLightingManager::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	// 调试日志：确认函数被调用
	CString strDebug;
	strDebug.Format(_T("[Lighting] OnDataReceived: dwCount=%u\n"), dwCount);
	OutputDebugString(strDebug);
	theApp.m_pLightingLog->LOG_INFO(strDebug);

	// TODO: m_bExitFlag 检查暂时注释，因为 Lighting 通信是独立模块不应受 UI 状态影响
	// if (theApp.m_bExitFlag == TRUE)
	// {
	// 	OutputDebugString(_T("[Lighting] OnDataReceived: EXIT early - m_bExitFlag is TRUE\n"));
	// 	return;
	// }

	// 将接收到的数据转换为 CString（注意：TCP 可能半包/粘包）
	CString strChunk;
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount,
		strChunk.GetBuffer(static_cast<int>(dwCount) + 1), static_cast<int>(dwCount) + 1);
	strChunk.ReleaseBuffer(static_cast<int>(dwCount));

	// 追加到缓存，然后按 '@' 作为结束符拆分完整消息
	m_recvCache += strChunk;

	LightingDbgPrint(_T("[Lighting -> MC] RECV raw: '%s', cache: '%s'\n"), strChunk, m_recvCache);
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("RECV raw: '%s', cache: '%s'"), strChunk, m_recvCache));

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

void CLightingManager::OnEvent(UINT uEvent, LPVOID /*lpvData*/)
{
	switch (uEvent)
	{
	case EVT_CONSUCCESS:
		m_bConnected = TRUE;
		theApp.m_LightingConectStatus = TRUE;
		LightingDbgPrint(_T("[Lighting] Connection established!\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("Connection established"));
		// 连接成功后检查配置，只有启用自动测试时才启动定时器
		if (_ttoi(theApp.m_strLightingAutoTest) != 0)
		{
			StartAutoTestTimer(10000);
		}
		else
		{
			LightingDbgPrint(_T("[Lighting] Auto-test disabled by config, skipping timer...\n"));
			theApp.m_pLightingLog->LOG_INFO(_T("Auto-test disabled by config"));
		}
		break;
	case EVT_CONDROP:
	case EVT_CONFAILURE:
		m_bConnected = FALSE;
		theApp.m_LightingConectStatus = FALSE;
		LightingDbgPrint(_T("[Lighting] Connection lost!\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("Connection lost"));
		// 连接断开时停止定时器并关闭连接，防止重复触发
		if (_ttoi(theApp.m_strLightingAutoTest) != 0)
		{
			StopAutoTestTimer();
		}
		StopComm();
		
		// 启动重试线程
		StartReconnectThread();
		break;
	default:
		break;
	}
}

void CLightingManager::SetEventHandler(ILightingEventHandler* pHandler)
{
	m_pHandler = pHandler;
}

void CLightingManager::HandleSingleMessage(const CString& msg)
{
	LightingDbgPrint(_T("[Lighting -> MC] HANDLE msg: '%s'\n"), msg);
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("HANDLE msg: '%s'"), msg));

	// Running / SnapFN / FN$xxxxxxxx 三种类型
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
		CString payload = msg.Mid(3); // 去掉 "FN$"
		HandleFNResult(payload);
	}
	else
	{
		// 其他消息类型，如需扩展可在此处解析
		theApp.m_pLightingSendReceiverLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] [Lighting -> MC] Unknown message: %s"), 
			GetNowSystemTimeMilliseconds(), msg));
	}
}

void CLightingManager::HandleRunning()
{
	LightingDbgPrint(_T("[Lighting -> MC] Received: Running@\n"));
	theApp.m_pLightingLog->LOG_INFO(_T("Received: Running@"));

	if (m_pHandler == nullptr)
	{
		LightingDbgPrint(_T("[Lighting -> MC] ERROR: m_pHandler is nullptr!\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("ERROR: m_pHandler is nullptr!"));
		return;
	}
	m_pHandler->OnLightingRunning();
}

void CLightingManager::HandleSnapFN()
{
	LightingDbgPrint(_T("[Lighting -> MC] Received: SnapFN@\n"));
	theApp.m_pLightingLog->LOG_INFO(_T("Received: SnapFN@"));

	if (m_pHandler == nullptr)
	{
		LightingDbgPrint(_T("[Lighting -> MC] ERROR: m_pHandler is nullptr!\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("ERROR: m_pHandler is nullptr!"));
		return;
	}
	m_pHandler->OnLightingSnapFN();
}

void CLightingManager::HandleFNResult(const CString& payload)
{
	LightingDbgPrint(_T("[Lighting -> MC] Received: FN$%s@\n"), payload);
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Received: FN$%s@"), payload));

	// payload 格式: "01$02$03$04" (8位数字+3个$分隔符=11字符) 或 "01$02$03$00" 等
	// 解析方式: 去掉 '$' 分隔符，提取每两位作为一个治具号
	CString cleanPayload = payload;
	cleanPayload.Remove(_T('$')); // 去掉所有 '$'

	// 验证清理后的长度应为 8
	if (cleanPayload.GetLength() != 8)
	{
		LightingDbgPrint(_T("[Lighting -> MC] FN$ payload length invalid after clean: '%s' (len=%d)\n"), 
			cleanPayload, cleanPayload.GetLength());
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("FN$ payload length invalid after clean: '%s' (len=%d)"), 
			cleanPayload, cleanPayload.GetLength()));
		return;
	}

	// 解析成 4 个两位码
	int resultCode[4] = { 0, };
	for (int i = 0; i < 4; ++i)
	{
		CString tmp = cleanPayload.Mid(i * 2, 2);
		resultCode[i] = _ttoi(tmp);
		LightingDbgPrint(_T("[Lighting -> MC] FN$ resultCode[%d]=%d\n"), i, resultCode[i]);
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("FN$ resultCode[%d]=%d"), i, resultCode[i]));
	}

	if (m_pHandler == nullptr)
	{
		LightingDbgPrint(_T("[Lighting -> MC] ERROR: m_pHandler is nullptr!\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("ERROR: m_pHandler is nullptr!"));
		return;
	}

	// 通过 TLS 获取数据库连接（每个线程自动拥有独立连接）
	// 回调中使用 GetTlsLightingConnPtr() 获取
	m_pHandler->OnLightingResult(resultCode);
}

//==============================================================================
// 自动发送 Start 命令的定时器实现
//==============================================================================

void CLightingManager::StartAutoTestTimer(DWORD dwDelayMs)
{
	StopAutoTestTimer(); // 确保只有一个定时器在运行

	LightingDbgPrint(_T("[Lighting] Starting auto-test timer (delay: %ums)...\n"), dwDelayMs);
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Starting auto-test timer (delay: %ums)"), dwDelayMs));

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

void CLightingManager::StopAutoTestTimer()
{
	LightingDbgPrint(_T("[Lighting] Stopping auto-test timer...\n"));
	theApp.m_pLightingLog->LOG_INFO(_T("Stopping auto-test timer"));

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
	theApp.m_pLightingLog->LOG_INFO(_T("Auto-test timer stopped"));
}

DWORD WINAPI CLightingManager::AutoTestTimerThread(LPVOID lpParam)
{
	CLightingManager* pThis = static_cast<CLightingManager*>(lpParam);
	if (!pThis)
		return 0;

	DWORD threadId = GetCurrentThreadId();
	LightingDbgPrint(_T("[Lighting] Auto-test timer started (delay: %ums), ThreadID=%lu\n"), 10000, threadId);
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Auto-test timer started (delay: %ums), ThreadID=%lu"), 10000, threadId));

	// 等待 10 秒或收到停止信号
	HANDLE handles[2] = { pThis->m_hAutoTestStopEvent, NULL };
	DWORD dwRet = WaitForMultipleObjects(1, handles, FALSE, 10000);

	if (dwRet == WAIT_TIMEOUT)
	{
		// 超时，说明是定时器触发，正常发送 Start 命令
		LightingDbgPrint(_T("[Lighting] Auto-test timer triggered, sending Start command... ThreadID=%lu\n"), threadId);
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Auto-test timer triggered, sending Start command, ThreadID=%lu"), threadId));

		// 设置所有槽位为激活状态（auto-test 模式下全开）
		for (int i = 0; i < 4; ++i)
			theApp.m_bLightingActiveSlot[i] = TRUE;

		// 发送开始检测前，更新 ivs_lcd_idmap 对应治具号记录，供检测软件使用
		// 使用线程局部连接（TLS），避免依赖全局连接
		for (int i = 0; i < 4; ++i)
		{
			int fixtureNo = i + 1;  // 治具号 1~4
			
			// Auto-test 模式使用模拟的 Barcode
			CString strBarcode;
			strBarcode.Format(_T("SIM_BARCODE_%d"), fixtureNo);
			
			// UniqueID 使用 GUID 保证全局唯一性
			CString strUniqueID;
			CStringSupport::GetGuid(strUniqueID);
			
			// MarkID=治具号 "01"~"04"
			CString strMarkID;
			strMarkID.Format(_T("%02d"), fixtureNo);
			
			// UpdateLightingIdMap 内部会自动处理线程局部连接
			theApp.UpdateLightingIdMap(fixtureNo, strUniqueID, strBarcode, strMarkID);
		}
		
		// 使用默认的 4 个治具位（全开）
		pThis->SendStart(DEFAULT_USED_SLOTS, DEFAULT_MAX_SLOTS);
	}
	else
	{
		// 收到停止信号（手动关闭或连接断开）
		LightingDbgPrint(_T("[Lighting] Auto-test timer cancelled\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("Auto-test timer cancelled"));
	}

	return 0;
}

//==============================================================================
// 连接重试线程实现
//==============================================================================

void CLightingManager::StartReconnectThread()
{
	// 如果已经有重试线程在运行，先停止
	StopReconnectThread();

	LightingDbgPrint(_T("[Lighting] Starting reconnect thread...\n"));
	theApp.m_pLightingLog->LOG_INFO(_T("Starting reconnect thread"));

	m_hReconnectStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hReconnectStopEvent)
	{
		LightingDbgPrint(_T("[Lighting] Failed to create reconnect stop event\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("Failed to create reconnect stop event"));
		return;
	}

	m_hReconnectThread = CreateThread(NULL, 0, ReconnectThread, this, 0, NULL);
	if (!m_hReconnectThread)
	{
		LightingDbgPrint(_T("[Lighting] Failed to create reconnect thread\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("Failed to create reconnect thread"));
		CloseHandle(m_hReconnectStopEvent);
		m_hReconnectStopEvent = NULL;
		return;
	}
}

void CLightingManager::StopReconnectThread()
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
	theApp.m_pLightingLog->LOG_INFO(_T("Reconnect thread stopped"));
}

DWORD WINAPI CLightingManager::ReconnectThread(LPVOID lpParam)
{
	CLightingManager* pThis = static_cast<CLightingManager*>(lpParam);
	if (!pThis)
		return 0;

	LightingDbgPrint(_T("[Lighting] Reconnect thread started\n"));
	theApp.m_pLightingLog->LOG_INFO(_T("Reconnect thread started"));

	DWORD dwAttemptCount = 0;
	HANDLE hStopEvent = pThis->m_hReconnectStopEvent;
	CString strIP = pThis->m_strServerIP;
	CString strPort = pThis->m_strServerPort;

	while (TRUE)
	{
		// 检查是否需要停止
		if (WaitForSingleObject(hStopEvent, 0) == WAIT_OBJECT_0)
		{
			LightingDbgPrint(_T("[Lighting] Reconnect thread received stop signal\n"));
			theApp.m_pLightingLog->LOG_INFO(_T("Reconnect thread received stop signal"));
			break;
		}

		// 检查连接参数是否有效
		if (strIP.IsEmpty() || strPort.IsEmpty())
		{
			LightingDbgPrint(_T("[Lighting] Invalid connection parameters, stopping reconnect\n"));
			theApp.m_pLightingLog->LOG_INFO(_T("Invalid connection parameters, stopping reconnect"));
			break;
		}

		// 检查是否已经连接
		if (pThis->m_bConnected)
		{
			LightingDbgPrint(_T("[Lighting] Connection already established, stopping reconnect\n"));
			theApp.m_pLightingLog->LOG_INFO(_T("Connection already established, stopping reconnect"));
			break;
		}

		// 递增重试计数
		dwAttemptCount++;
		
		// 检查是否超过最大重试次数
		if (pThis->m_dwMaxReconnectAttempts > 0 && dwAttemptCount > pThis->m_dwMaxReconnectAttempts)
		{
			LightingDbgPrint(_T("[Lighting] Max reconnect attempts reached (%d), stopping\n"), pThis->m_dwMaxReconnectAttempts);
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Max reconnect attempts reached (%d), stopping"), pThis->m_dwMaxReconnectAttempts));
			break;
		}

		// 尝试重新连接
		LightingDbgPrint(_T("[Lighting] Reconnect attempt %d to %s:%s...\n"), dwAttemptCount, strIP, strPort);
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Reconnect attempt %d to %s:%s"), dwAttemptCount, strIP, strPort));

		// 关闭旧连接（如果有）
		pThis->StopComm();
		pThis->CloseComm();

		// 尝试连接
		if (pThis->ConnectTo(strIP, strPort, AF_INET, SOCK_STREAM))
		{
			LightingDbgPrint(_T("[Lighting] Reconnect successful!\n"));
			theApp.m_pLightingLog->LOG_INFO(_T("Reconnect successful!"));

			// 启动 Socket 线程
			if (pThis->WatchComm())
			{
				pThis->m_bConnected = TRUE;
				theApp.m_LightingConectStatus = TRUE;

				// 启动自动测试定时器
				if (_ttoi(theApp.m_strLightingAutoTest) != 0)
				{
					pThis->StartAutoTestTimer(10000);
				}
				
				break; // 连接成功，退出重试循环
			}
			else
			{
				LightingDbgPrint(_T("[Lighting] Reconnect failed: Failed to start communication thread\n"));
				theApp.m_pLightingLog->LOG_INFO(_T("Reconnect failed: Failed to start communication thread"));
				pThis->CloseComm();
			}
		}
		else
		{
			LightingDbgPrint(_T("[Lighting] Reconnect attempt %d failed\n"), dwAttemptCount);
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("Reconnect attempt %d failed"), dwAttemptCount));
		}

		// 等待重试间隔
		if (WaitForSingleObject(hStopEvent, pThis->m_dwReconnectInterval) == WAIT_OBJECT_0)
		{
			LightingDbgPrint(_T("[Lighting] Reconnect thread received stop signal during wait\n"));
			theApp.m_pLightingLog->LOG_INFO(_T("Reconnect thread received stop signal during wait"));
			break;
		}
	}

	return 0;
}

#endif // _SYSTEM_AMTAFT_
