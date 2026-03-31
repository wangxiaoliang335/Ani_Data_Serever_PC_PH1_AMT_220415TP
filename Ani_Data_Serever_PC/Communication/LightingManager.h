#pragma once

#include "Ani_Data_Serever_PC.h"
#include "SocketComm.h"
#include "StringSupport.h"
#include "TLSConnection.h"

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
// 实际的 PLC 写入、数据库 (ivs_lcd_idmap / IVS_LCD_InspectionResult / IVS_LCD_AOIResult)
// 由调用方在回调或事件里完成（在对应的 Handle* 函数中预留钩子）。
//
// 点灯检事件回调接口：
//  - 在 UI / 设备接口类中实现该接口，并通过 CLightingManager::SetEventHandler 注册，
//    即可在 HandleRunning / HandleSnapFN / HandleFNResult 中收到解析后的事件和结果。
class ILightingEventHandler
{
public:
	virtual ~ILightingEventHandler() {}

	// Lighting -> MC : Running@
	// 通知"点灯检测已经开始 Running"
	virtual void OnLightingRunning() {}

	// Lighting -> MC : SnapFN@
	// 通知"点灯采图完成，可以出栈/移动工位"
	virtual void OnLightingSnapFN() {}

	// Lighting -> MC : FN$xxxxxxxx@
	// payload 已经被解析成 4 个治具结果码（resultCode[0..3]），空位为 0
	// 连接已改为 TLS 方式，回调中直接调用 GetTlsLightingConnPtr() 获取线程局部连接
	virtual void OnLightingResult(const int resultCode[4]) {}

	// 连接成功后定时发送测试信号
	virtual void OnLightingAutoTest() {}
};

class CLightingManager : public CSocketComm
{
public:
	CLightingManager();
	virtual ~CLightingManager();

	// 建立与点灯检软件的 TCP 连接
	// ip  : 点灯检软件所在 PC 的 IP，例如 "192.168.0.10"
	// port: 服务端口，文档中默认使用 "6501"
	bool ConnectToLighting(const CString& ip, const CString& port);

	// 关闭连接与线程
	void Close();

	// 当前是否处于连接成功状态
	BOOL getConectCheck();

	// 发送 Start 指令
	// usedSlots[4] : 当前实际使用的治具位号（1~4），未使用填 0
	// maxSlots[4]  : 最大可用治具位号（1~4），未使用填 0
	// 会自动打包成 "Start$xxxxxxxx$xxxxxxxx@" 格式并通过 Socket 发送
	void SendStart(const int usedSlots[4], const int maxSlots[4]);

	// CSocketComm 事件回调
	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount) override;
	virtual void OnEvent(UINT uEvent, LPVOID lpvData) override;

	// 注册点灯检事件回调对象（非线程安全，需在连接前或初始化阶段调用）
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
	ILightingEventHandler* m_pHandler;

	// TCP 可能出现半包/粘包：用 '@' 作为消息结束符
	// 将每次 OnDataReceived 的数据追加到缓存，按 '@' 拆分后逐条处理，剩余半包留到下次
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
	DWORD             m_dwReconnectInterval;  // 重试间隔（毫秒）
	DWORD             m_dwMaxReconnectAttempts;  // 最大重试次数（0表示无限重试）
	static DWORD WINAPI ReconnectThread(LPVOID lpParam);
	void StartReconnectThread();
	void StopReconnectThread();
	void SetConnectionParameters(const CString& ip, const CString& port);
};
