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
	, m_pLightingConn(NULL)
	, m_bLightingDBConnected(FALSE)
{
	// 不在构造函数中初始化数据库连接，延迟到第一次使用时初始化
}

CLightingManager::~CLightingManager()
{
	Close();
	// 关闭 Lighting 线程专用的数据库连接
	CloseLightingDatabase();
}

bool CLightingManager::ConnectToLighting(const CString& ip, const CString& port)
{
	// 作为 TCP Client 连接到点灯检软件
	LightingDbgPrint(_T("[Lighting] Connecting to %s:%s...\n"), ip, port);

	if (!ConnectTo(ip, port, AF_INET /*IPPROTO_TCP*/, SOCK_STREAM))
	{
		LightingDbgPrint(_T("[Lighting] Connection failed!\n"));
		m_bConnected = FALSE;
		return false;
	}

	LightingDbgPrint(_T("[Lighting] Connected successfully!\n"));

	// 启动 Socket 线程，开始接收数据
	if (!WatchComm())
	{
		m_bConnected = FALSE;
		CloseComm();
		return false;
	}

	m_bConnected = TRUE;
	// 启动自动测试定时器
	StartAutoTestTimer(10000);

	return true;
}

void CLightingManager::Close()
{
	StopAutoTestTimer();
	StopComm();
	CloseComm();
	m_bConnected = FALSE;
	// 关闭数据库连接
	CloseLightingDatabase();
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
		theApp.m_pLightingLog->LOG_INFO(_T("Lighting connection established"));
		// 连接成功后启动10秒定时器，定时器到期自动发送 Start 命令
		StartAutoTestTimer(10000);
		break;
	case EVT_CONDROP:
	case EVT_CONFAILURE:
		m_bConnected = FALSE;
		theApp.m_LightingConectStatus = FALSE;
		LightingDbgPrint(_T("[Lighting] Connection lost!\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("Lighting connection lost"));
		// 连接断开时停止定时器
		StopAutoTestTimer();
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

	if (m_pHandler == nullptr)
	{
		LightingDbgPrint(_T("[Lighting -> MC] ERROR: m_pHandler is nullptr!\n"));
		return;
	}
	m_pHandler->OnLightingRunning();
}

void CLightingManager::HandleSnapFN()
{
	LightingDbgPrint(_T("[Lighting -> MC] Received: SnapFN@\n"));

	if (m_pHandler == nullptr)
	{
		LightingDbgPrint(_T("[Lighting -> MC] ERROR: m_pHandler is nullptr!\n"));
		return;
	}
	m_pHandler->OnLightingSnapFN();
}

void CLightingManager::HandleFNResult(const CString& payload)
{
	LightingDbgPrint(_T("[Lighting -> MC] Received: FN$%s@\n"), payload);

	// payload 格式: "01$02$03$04" (8位数字+3个$分隔符=11字符) 或 "01$02$03$00" 等
	// 解析方式: 去掉 '$' 分隔符，提取每两位作为一个治具号
	CString cleanPayload = payload;
	cleanPayload.Remove(_T('$')); // 去掉所有 '$'

	// 验证清理后的长度应为 8
	if (cleanPayload.GetLength() != 8)
	{
		LightingDbgPrint(_T("[Lighting -> MC] FN$ payload length invalid after clean: '%s' (len=%d)\n"), 
			cleanPayload, cleanPayload.GetLength());
		return;
	}

	// 解析成 4 个两位码
	int resultCode[4] = { 0, };
	for (int i = 0; i < 4; ++i)
	{
		CString tmp = cleanPayload.Mid(i * 2, 2);
		resultCode[i] = _ttoi(tmp);
		LightingDbgPrint(_T("[Lighting -> MC] FN$ resultCode[%d]=%d\n"), i, resultCode[i]);
	}

	if (m_pHandler == nullptr)
	{
		LightingDbgPrint(_T("[Lighting -> MC] ERROR: m_pHandler is nullptr!\n"));
		return;
	}

	// 延迟初始化数据库连接（确保主程序已完成初始化）
	InitLightingDatabase();

	// 将 Lighting 线程专用的数据库连接传递给回调
	m_pHandler->OnLightingResult(resultCode, m_pLightingConn);
}

BOOL CLightingManager::InitLightingDatabase()
{
	if (m_bLightingDBConnected && m_pLightingConn != NULL)
		return TRUE;

	// 先关闭旧连接（如果有）
	if (m_pLightingConn != NULL)
	{
		try {
			delete m_pLightingConn;
		}
		catch (...) {
		}
		m_pLightingConn = NULL;
		m_bLightingDBConnected = FALSE;
	}

	// 从配置文件读取数据库连接信息（如果主程序还未读取）
	if (theApp.m_strLightingDBServer.IsEmpty())
	{
		EZIni ini(_T("D:\\ANI\\DataServer\\Data\\System\\sysData.ini"));
		CString sTemp;
		theApp.m_strLightingDBServer = ini[_T("DATABASE")][_T("HOST")]; sTemp = _T("127.0.0.1"); theApp.m_strLightingDBServer = (theApp.m_strLightingDBServer.IsEmpty()) ? sTemp : theApp.m_strLightingDBServer;
		theApp.m_strLightingDBName = ini[_T("DATABASE")][_T("NAME")]; sTemp = _T("IVS_LCD"); theApp.m_strLightingDBName = (theApp.m_strLightingDBName.IsEmpty()) ? sTemp : theApp.m_strLightingDBName;
		theApp.m_strLightingDBUser = ini[_T("DATABASE")][_T("USER")]; sTemp = _T("root"); theApp.m_strLightingDBUser = (theApp.m_strLightingDBUser.IsEmpty()) ? sTemp : theApp.m_strLightingDBUser;
		theApp.m_strLightingDBPassword = ini[_T("DATABASE")][_T("PASSWORD")]; sTemp = _T("password"); theApp.m_strLightingDBPassword = (theApp.m_strLightingDBPassword.IsEmpty()) ? sTemp : theApp.m_strLightingDBPassword;
	}

	try {
		sql::Driver* driver = get_driver_instance();
		if (!driver) {
			LightingDbgPrint(_T("[Lighting] Failed to get MySQL driver\n"));
			return FALSE;
		}

		CString strUrl;
		strUrl.Format(_T("tcp://%s:3306"), theApp.m_strLightingDBServer);

		sql::SQLString sql_str1((std::string)CT2A(strUrl));
		sql::SQLString sql_user((std::string)CT2A(theApp.m_strLightingDBUser));
		sql::SQLString sql_password((std::string)CT2A(theApp.m_strLightingDBPassword));

		CString dbgMsg;
		dbgMsg.Format(_T("[Lighting] Connecting to database: %s, User: %s"), strUrl, theApp.m_strLightingDBUser);
		LightingDbgPrint(dbgMsg + _T("\n"));

		m_pLightingConn = driver->connect(sql_str1, sql_user, sql_password);
		if (!m_pLightingConn) {
			LightingDbgPrint(_T("[Lighting] Failed to connect to MySQL database\n"));
			return FALSE;
		}

		// 设置连接选项
		m_pLightingConn->setClientOption("optReadTimeout", "30");
		m_pLightingConn->setClientOption("optWriteTimeout", "30");
		m_pLightingConn->setClientOption("optConnectTimeout", "10");
		m_pLightingConn->setClientOption("characterSetResults", "utf8mb4");

		m_pLightingConn->setSchema((std::string)CT2A(theApp.m_strLightingDBName));

		m_bLightingDBConnected = TRUE;
		LightingDbgPrint(_T("[Lighting] Database connected successfully\n"));
		return TRUE;
	}
	catch (sql::SQLException& e) {
		CString strMsg;
		strMsg.Format(_T("[Lighting] Database connection failed: %s, errCode=%d"),
			CString(e.what()), e.getErrorCode());
		LightingDbgPrint(strMsg + _T("\n"));
		return FALSE;
	}
}

void CLightingManager::CloseLightingDatabase()
{
	if (m_pLightingConn != NULL)
	{
		try {
			m_pLightingConn->close();
		}
		catch (...) {
		}
		try {
			delete m_pLightingConn;
		}
		catch (...) {
		}
		m_pLightingConn = NULL;
		m_bLightingDBConnected = FALSE;
		LightingDbgPrint(_T("[Lighting] Database connection closed\n"));
	}
}

#endif // _SYSTEM_AMTAFT_

//==============================================================================
// 自动发送 Start 命令的定时器实现
//==============================================================================

void CLightingManager::StartAutoTestTimer(DWORD dwDelayMs)
{
	StopAutoTestTimer(); // 确保只有一个定时器在运行

	LightingDbgPrint(_T("[Lighting] Starting auto-test timer (delay: %ums)...\n"), dwDelayMs);

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
}

DWORD WINAPI CLightingManager::AutoTestTimerThread(LPVOID lpParam)
{
	CLightingManager* pThis = static_cast<CLightingManager*>(lpParam);
	if (!pThis)
		return 0;

	LightingDbgPrint(_T("[Lighting] Auto-test timer started (delay: %ums)\n"), 10000);

	// 等待 10 秒或收到停止信号
	HANDLE handles[2] = { pThis->m_hAutoTestStopEvent, NULL };
	DWORD dwRet = WaitForMultipleObjects(1, handles, FALSE, 10000);

	if (dwRet == WAIT_TIMEOUT)
	{
		// 超时，说明是定时器触发，正常发送 Start 命令
		LightingDbgPrint(_T("[Lighting] Auto-test timer triggered, sending Start command...\n"));

		// 设置所有槽位为激活状态（auto-test 模式下全开）
		for (int i = 0; i < 4; ++i)
			theApp.m_bLightingActiveSlot[i] = TRUE;

		// 使用默认的 4 个治具位（全开）
		pThis->SendStart(DEFAULT_USED_SLOTS, DEFAULT_MAX_SLOTS);
	}
	else
	{
		// 收到停止信号（手动关闭或连接断开）
		LightingDbgPrint(_T("[Lighting] Auto-test timer cancelled\n"));
	}

	return 0;
}
