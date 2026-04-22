#pragma once

///////////////////////////////////////////////////////////////////////////////
// FILE : SocketServerBase.h
// Class: CSocketServerBase
//
// 中文说明：
//   Socket Server 断线自动重连基类。
//   继承自 `CSocketComm`，专门用于作为 Server 端的通信类（如 Align、PC 等），
//   提供断线后自动重新监听的能力。
//
//   主要功能：
//     1. 保存 Server 监听端口，断线后可重新监听
//     2. `m_bWaitingForReconnect` 标志：在重新监听期间（socket 存在但尚无 client 连上）
//        将 `getConectCheck()` 置为 FALSE，防止业务层误判为已连接
//     3. `OnEvent(EVT_CONDROP)` 中自动执行重连（最多 10 次，每次间隔 500ms）
//     4. `OnEvent(EVT_CONSUCCESS)` 中取消等待状态
//
//   子类只需：
//     - 实现 `GetDeviceName()` 返回设备名称（如 "Align"/"PG"），用于日志输出
//     - 实现 `LogServerMsg()` 输出日志到各自的日志系统
//     - 在 `SocketServerOpen()` 中调用 `SocketServerOpenBase()` 保存端口
//     - 在 `OnEvent()` 中调用 `OnEventReconnectBase()` 处理公共重连逻辑
//
//   注意：重连期间 `CreateSocket()` 使用 `SO_REUSEADDR`，避免 TIME_WAIT 导致 bind 失败
///////////////////////////////////////////////////////////////////////////////

#include "SocketComm.h"

// 自动重连配置
#define SOCKET_RECONNECT_MAX_RETRY    10
#define SOCKET_RECONNECT_DELAY_MS     500

class CSocketServerBase : public CSocketComm
{
public:
	CSocketServerBase();
	virtual ~CSocketServerBase();

protected:
	// 中文说明：
	//   **功能：** 通用 Server 监听打开逻辑。
	//   **参数：** strServerPort - 监听端口号
	//   **说明：** 在子类 `SocketServerOpen()` 中调用，子类应先做业务相关初始化，
	//             再调用本函数保存端口和公共配置。
	void SocketServerOpenBase(CString strServerPort);

	// 中文说明：
	//   **功能：** 通用连接状态检查。
	//   **返回：** TRUE = 已连接（有 client）；FALSE = 未连接或正在等待重连
	//   **说明：** 子类 `getConectCheck()` 应先调用本函数进行公共检查，
	//             如返回 FALSE 则直接返回，无需继续业务层检查。
	BOOL getConectCheckBase();

	// 中文说明：
	//   **功能：** 公共断线重连事件处理。
	//   **返回：** TRUE = 公共逻辑已处理，无需子类继续；
	//             FALSE = 非公共事件（如 EVT_CONSUCCESS 等），子类应自行处理
	//   **说明：** 子类 `OnEvent()` 应先调用本函数，如返回 TRUE 则直接返回。
	BOOL OnEventReconnectBase(UINT uEvent);

	// 中文说明：
	//   **功能：** 返回设备名称，用于日志输出。
	//   **示例：** AlignManager 返回 "Align"；PGManager 返回 "PG"
	virtual LPCTSTR GetDeviceName() const = 0;

	// 中文说明：
	//   **功能：** 输出 Server 重连相关的日志。
	//   **说明：** 由子类实现，输出到各自的日志系统（如 `LogWrite`/`PgLogMessage`）
	virtual void LogServerMsg(LPCTSTR szMsg) = 0;

private:
	// 禁止外部调用基类的空实现
	virtual void OnEvent(UINT uEvent, LPVOID lpvData) override;

private:
	CString m_strServerPort;         // 保存 Server 监听端口，断线后用于自动重新监听
	BOOL    m_bWaitingForReconnect;   // 断线后重新监听期间为 TRUE，防止 getConectCheck() 误报
};
