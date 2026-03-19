#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "DlgMainView.h"
#include "DlgMainLog.h"
#include "LightingManager.h"

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
{
}

CLightingManager::~CLightingManager()
{
	Close();
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

	return true;
}

void CLightingManager::Close()
{
	StopComm();
	CloseComm();
	m_bConnected = FALSE;
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
	if (theApp.m_bExitFlag == TRUE)
		return;

	// 将接收到的数据转换为 CString（注意：TCP 可能半包/粘包）
	CString strChunk;
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount,
		strChunk.GetBuffer(static_cast<int>(dwCount) + 1), static_cast<int>(dwCount) + 1);
	strChunk.ReleaseBuffer(static_cast<int>(dwCount));

	// 追加到缓存，然后按 '@' 作为结束符拆分完整消息
	m_recvCache += strChunk;

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
		break;
	case EVT_CONDROP:
	case EVT_CONFAILURE:
		m_bConnected = FALSE;
		theApp.m_LightingConectStatus = FALSE;
		LightingDbgPrint(_T("[Lighting] Connection lost!\n"));
		theApp.m_pLightingLog->LOG_INFO(_T("Lighting connection lost"));
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
	// 点灯检开始 Running，回调到上层（由上层决定是否写 PLC / 日志）
	if (m_pHandler != nullptr)
	{
		m_pHandler->OnLightingRunning();
	}
}

void CLightingManager::HandleSnapFN()
{
	// 点灯采图完成 SnapFN，回调到上层（由上层决定是否写 PLC / 日志）
	if (m_pHandler != nullptr)
	{
		m_pHandler->OnLightingSnapFN();
	}
}

void CLightingManager::HandleFNResult(const CString& payload)
{
	// payload 应为 8 位数字字符串，每两位对应一个治具位
	// 例如 "01020300" -> 01, 02, 03 三个治具有结果，第四位为空
	if (payload.GetLength() < 8)
	{
		theApp.m_pLightingSendReceiverLog->LOG_INFO(CStringSupport::FormatString(
			_T("[%s] [Lighting -> MC] FN$ payload length invalid: '%s'"),
			GetNowSystemTimeMilliseconds(), payload));
		return;
	}

	// 解析成 4 个两位码
	int resultCode[4] = { 0, };
	for (int i = 0; i < 4; ++i)
	{
		CString tmp = payload.Mid(i * 2, 2);
		resultCode[i] = _ttoi(tmp);
	}

	// 将解析好的 4 个治具结果码回调到上层，由上层完成：
	//  - 结果与治具 / 条码 / UniqueID 的映射
	//  - PLC 写入、MySQL 查询 / 回写、MES / DFS 上传等逻辑
	if (m_pHandler != nullptr)
	{
		m_pHandler->OnLightingResult(resultCode);
	}
}

#endif // _SYSTEM_AMTAFT_
