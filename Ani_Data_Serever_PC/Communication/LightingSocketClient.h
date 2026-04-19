#pragma once

// 前向声明
class CAni_Data_Serever_PCApp;

#include "SocketComm.h"
#include "StringSupport.h"
#include "TLSConnection.h"
#include "LightingEventHandler.h"

// 点灯检（Lighting）软件 TCP 客户端
// 协议示例：
//   MC -> Lighting : Start$xxxxxxxx$xxxxxxxx@
//   Lighting -> MC : Running@
//   Lighting -> MC : SnapFN@
//   Lighting -> MC : FN$xxxxxxxx@
//
// 其中 xxxxxxxx 为 8 位数字字符串，每两位对应 1 个治具位：
//   01 02 03 04  -> 4 个治具（最大 4 个），空位用 00 填充
//
// 本类只负责 Socket 通信与协议解析：
//   - 发送 Start 指令
//   - 解析 Running / SnapFN / FN$ 消息
// 实际的业务逻辑由调用方在回调中完成

// 调试日志函数（使用 OutputDebugString 输出）
static void LightingDbgPrint(LPCTSTR pszFmt, ...);

class CLightingSocketClient : public CSocketComm
{
public:
	CLightingSocketClient();
	virtual ~CLightingSocketClient();

	// 建立与点灯检软件的 TCP 连接
	bool Connect(const CString& ip, const CString& port);

	// 关闭连接与线程
	void Close();

	// 当前是否处于连接成功状态
	BOOL IsConnected();

	// 发送 Start 指令
	void SendStart(CString curStr, CString maxStr);

	// CSocketComm 事件回调
	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount) override;
	virtual void OnEvent(UINT uEvent, LPVOID lpvData) override;

	// 注册点灯检事件回调对象
	void SetEventHandler(ILightingEventHandler* pHandler);

protected:
	// 处理单条去掉结尾 '@' 的消息
	void HandleSingleMessage(const CString& msg);

	// Lighting -> MC : Running@
	void HandleRunning();

	// Lighting -> MC : SnapFN@
	void HandleSnapFN();

	// Lighting -> MC : FN$xxxxxxxx@
	void HandleFNResult(const CString& payload);

	// 将 4 个位置编码为 8 位数字字符串（每两位代表一位，空位为 "00"）
	CString EncodeSlots(const int slots[4]);

private:
	BOOL              m_bConnected;
	CCriticalSection  m_csSocketSend;
	CCriticalSection  m_csReconnect;
	ILightingEventHandler* m_pHandler;

	// TCP 可能出现半包/粘包：用 '@' 作为消息结束符
	CString           m_recvCache;

	// 定时器：连接成功后自动发送测试信号
	HANDLE            m_hAutoTestTimer;
	HANDLE            m_hAutoTestStopEvent;
	static DWORD WINAPI AutoTestTimerThread(LPVOID lpParam);
	void StopAutoTestTimer();
	void StartAutoTestTimer(DWORD dwDelayMs);

	// 连接重试相关
	CString           m_strServerIP;
	CString           m_strServerPort;
	HANDLE            m_hReconnectThread;
	HANDLE            m_hReconnectStopEvent;
	DWORD             m_dwReconnectInterval;
	DWORD             m_dwMaxReconnectAttempts;
	static DWORD WINAPI ReconnectThread(LPVOID lpParam);
	void StartReconnectThread();
	void StopReconnectThread();
	void SetConnectionParameters(const CString& ip, const CString& port);
};
