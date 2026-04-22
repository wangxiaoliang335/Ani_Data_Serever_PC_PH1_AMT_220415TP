#include "stdafx.h"
#include "SocketServerBase.h"
#include "StringSupport.h"

CSocketServerBase::CSocketServerBase()
{
	m_strServerPort = _T("");
	m_bWaitingForReconnect = FALSE;
}

CSocketServerBase::~CSocketServerBase()
{
}

void CSocketServerBase::SocketServerOpenBase(CString strServerPort)
{
	m_strServerPort = strServerPort;
}

BOOL CSocketServerBase::getConectCheckBase()
{
	// 正在等待重连（Server socket 存在但尚未有 client 连上），此时不认为已连接
	if (m_bWaitingForReconnect)
		return FALSE;

	// 使用 IsOpen() 检查 socket 是否有效，而不是 GetSockName()
	// 因为 Server 模式下，m_hComm 在 accept() 后变成客户端socket，
	// 客户端断开后 m_hComm 可能变为 INVALID_HANDLE_VALUE，
	// 导致 GetSockName() 返回 IP=0，无法正确判断连接状态
	BOOL bResult = IsOpen() ? TRUE : FALSE;

	// 连接状态变化时输出调试日志
	static BOOL s_bLastResult = FALSE;
	if (bResult != s_bLastResult)
	{
		CString strDebug;
		strDebug.Format(_T("[DEBUG][%s] getConectCheckBase - Port:%s, IsOpen:%s, m_hComm:0x%X, m_bWaitingForReconnect:%d"),
			GetDeviceName(), m_strServerPort,
			bResult ? _T("TRUE(Connected)") : _T("FALSE(Disconnected)"),
			m_hComm, m_bWaitingForReconnect);
		LogServerMsg(strDebug);
		s_bLastResult = bResult;
	}

	return bResult;
}

BOOL CSocketServerBase::OnEventReconnectBase(UINT uEvent)
{
	switch (uEvent)
	{
	case EVT_CONDROP:
	{
		// 标记为等待重连状态，防止 getConectCheck() 在此期间误报 TRUE
		m_bWaitingForReconnect = TRUE;

		CString strLog;
		strLog.Format(_T("%s Connect Drop (m_bWaitingForReconnect=TRUE, will wait for re-accept)"), GetDeviceName());
		LogServerMsg(strLog);

		// 不在这里创建新 socket，让 Socket Thread 自动重新监听
		// 因为 Server 模式下 accept() 后 m_hComm 被客户端 socket 覆盖，
		// 监听 socket 保存在线程的 Accept 变量中，断开后线程会自动重新监听

		return TRUE;
	}

	case EVT_CONSUCCESS:
	{
		// Client 重连成功，取消等待状态
		m_bWaitingForReconnect = FALSE;
		CString strLog;
		strLog.Format(_T("%s Connect Success (EVT_CONSUCCESS received, m_bWaitingForReconnect=FALSE)"), GetDeviceName());
		LogServerMsg(strLog);

		// 诊断日志：记录连接成功后的状态
		strLog.Format(_T("%s [DIAG] Connection restored - IsOpen:%d, m_hComm:0x%X"), GetDeviceName(), IsOpen(), m_hComm);
		LogServerMsg(strLog);

		return TRUE;  // 公共逻辑已处理
	}

	default:
		// 其他事件（EVT_ZEROLENGTH、EVT_CONFAILURE 等）由子类自行处理
		return FALSE;
	}
}

// 基类的 OnEvent 禁止外部调用（子类必须通过 OnEventReconnectBase 处理）
void CSocketServerBase::OnEvent(UINT uEvent, LPVOID lpvData)
{
	// 空实现，子类应重写并在内部调用 OnEventReconnectBase
}
