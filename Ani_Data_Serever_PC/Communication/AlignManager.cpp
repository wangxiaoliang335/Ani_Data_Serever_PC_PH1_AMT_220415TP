
#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "DlgMainLog.h"
#else
#include "DlgGammaMain.h"
#endif
#include "AlignManager.h"

/*
======================================================================
VS/MC 对齐通信流程简要说明（给非软件工程人员看的说明）
----------------------------------------------------------------------
1. 模块作用
   - `CAlignManager` 负责“对位（Align）相关的 Socket 通信”和“结果写入 PLC”。
   - 一端连接对位视觉 PC（VS，对齐相机），另一端通过 `MNetH` 接口把对位结果写入 PLC。

2. 典型对位流程（Pattern Align / Tray Align 等）
   1）PLC 侧：面板到位 → PLC 置位“开始对位”信号比特
   2）Data Server：PLC 线程检测到信号后，调用对位线程，最终通过 `CAlignManager::SocketSendto`
       - 发送 MC_* 命令给 VS（例如：MC_INSPECTION_START / MC_GRAB_READY_REQUEST 等）
   3）VS 侧：收到命令后进行拍照和运算，完成后通过 VS_* 命令把结果回传
       - 结果格式：STX + 命令号 + "," + 结果参数 + ETX
   4）Data Server：在 `CAlignManager::OnDataReceived` 中解析 VS 返回的数据
       - 按命令号分支处理（心跳、状态、模型切换、GRAB_END 等）
       - 对于 GRAB_END，会解析出对位结果 X/Y/T 和 OK/NG
   5）Data Server：在 `AlignGrabEnd` / `AlignTrayAlignGrabEnd` / `AlignTrayCheckGrabEnd`
       - 把结果转换成 PLC 需要的数值格式（乘以 10000，转成整形）
       - 通过 `MNetH` (`SetAlignResult` / `SetTrayCheckResult` 等) 写入 PLC 数据区
       - 置位“对位结束”比特，通知 PLC 对位完成
   6）PLC：读取对位结果和结束信号，决定后续机械动作（继续搬运、NG 流程等）。

3. 日志与问题排查
   - 所有收发 VS 的命令都会写入 `AlignSendReceiverLog` 和主界面对位日志窗口：
     - `[MC -> VS] ...`：本系统发给对位 PC 的命令
     - `[VS -> MC] ...`：对位 PC 回来的响应
   - 对位异常（如 STX/ETX 错误、命令解析失败），会在 `LogWrite` 日志中体现。

4. 如何在现场快速判断问题方向（给非软件人员参考）
   - 日志中只有 `[MC -> VS]` 没有 `[VS -> MC]`：多为网络或对位 PC 端问题
   - `[VS -> MC]` 有返回，但 PLC 侧结果不变：多为 PLC 通信（MNetH）或地址配置问题
   - PLC 结果 OK，但机械动作不对：需检查 PLC 程序逻辑，与本模块无直接关系。
======================================================================
*/

CAlignManager::CAlignManager(int iAlignType, int iAlignTypeNum, int iAlignNum)
{
	m_iAlignType = iAlignType;
	m_iAlignTypeNum = iAlignTypeNum;
	m_iAlignNum = iAlignNum;
}

CAlignManager::~CAlignManager()
{
}

BOOL CAlignManager::getConectCheck()
{
	SockAddrIn addrin;
	GetSockName(addrin);
	LONG  uAddr = addrin.GetIPAddr();
	if (uAddr == 0)
		return FALSE;
	else
		return TRUE;
}

bool CAlignManager::SocketServerOpen(CString strServerPort)
{
	m_bMelsecSimulaion = true;
	SetSmartAddressing(false);
	SetServerState(true);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Opening socket server on port %s"), m_iAlignNum + 1, strServerPort));
	bool ret = CreateSocket(strServerPort, AF_INET, SOCK_STREAM, 0);
	if (ret) {
		LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Socket server created successfully"), m_iAlignNum + 1));
		return WatchComm();
	} else {
		LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Failed to create socket server"), m_iAlignNum + 1));
		return false;
	}
}

void CAlignManager::LogWrite(int iNum, CString strContents)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	g_MainLog->m_AlignListBox.InsertString(0, CStringSupport::FormatString(_T("[%s] %s"), GetNowSystemTimeMilliseconds(), strContents));
	theApp.m_AlignLog->LOG_INFO(strContents);
}

/**
 * @brief 向VS (视觉系统) 发送协议数据包
 * 
 * 协议格式：STX + 命令内容 + ETX
 * - STX: 起始符 (0x02)
 * - 命令内容：命令码,参数1,参数2,... (逗号分隔)
 * - ETX: 结束符 (0x03)
 * 
 * @param iNum 对齐系统编号 (0, 1, 2...)
 * @param strContents 命令内容，格式：命令码,参数1,参数2,...
 *                    例如："0,ModelName" 表示 MC_ARE_YOU_THERE 命令
 * @param iCommand 命令枚举值，对应 MC_PacketNameTable 中的索引
 *                 用于日志记录，标识发送的命令类型
 */
void CAlignManager::SocketSendto(int iNum, CString strContents, int iCommand)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	// 构造协议数据包：STX + 命令内容 + ETX
	CString strCommand = CStringSupport::FormatString(_T("%c%s%c"), _STX, strContents, _ETX);
	char *lpCommand = StringToChar(strCommand);
	
	// Log send command details
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Sending command %s to VS"), m_iAlignNum + 1, MC_PacketNameTable[iCommand]));
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Command parameters: %s"), m_iAlignNum + 1, strContents));
	
	// 通过Socket发送数据包，超时时间100ms
	int bytesSent = theApp.m_AlignSocketManager[iNum]->WriteComm((BYTE*)lpCommand, strlen(lpCommand), 100L);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Sent %d bytes"), m_iAlignNum + 1, bytesSent));
	
	delete lpCommand;

	m_lastContent = strContents;
	// 记录发送日志：[MC -> VS] 表示从机器控制器发送到视觉系统
	theApp.m_pAlignSendReceiverLog[iNum]->LOG_INFO(CStringSupport::FormatString(_T("[MC -> VS] [Command : %s] ->%s"), MC_PacketNameTable[iCommand], strContents));
}

void CAlignManager::AlignLightOff(int iNum)
{
	CString sendMsg;
	for (int ii = 0; ii < 2; ii++)
	{
		sendMsg.Format(_T("%d,%d"), MC_ALIGN_LIGHT_OFF, ii);
		SocketSendto(iNum, sendMsg, MC_ALIGN_LIGHT_OFF);
	}
}

/**
 * @brief 接收来自VS (视觉系统) 的协议数据包并处理
 * 
 * 协议解析流程：
 * 1. 数据包格式：STX + 命令码,参数1,参数2,... + ETX
 * 2. 按ETX分割，可能包含多个数据包
 * 3. 验证STX起始符
 * 4. 提取命令码和参数
 * 5. 根据命令码执行相应处理
 * 
 * @param lpBuffer 接收到的数据缓冲区
 * @param dwCount 数据长度（字节数）
 */
void CAlignManager::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	CString strData, m_strHeader, m_strCommand, m_strContents, strParsing;
	int iFind, iFindSTX;
	// 将接收到的字节数据转换为Unicode字符串
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount, strData.GetBuffer(dwCount + 1), dwCount + 1);
	strData.ReleaseBuffer(dwCount);
	
	// Log raw received data
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Received %d bytes from VS"), m_iAlignNum + 1, dwCount));
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Raw data: %s"), m_iAlignNum + 1, strData));

	CStringArray responseTokens;
	// 按ETX (0x03) 分割数据包，可能包含多个数据包
	CStringSupport::GetTokenArray(strData, _ETX, responseTokens);

	// 如果没有ETX分隔符，说明数据包格式错误
	if (responseTokens.GetSize() == 1)
	{
		LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: ETX No Message!!! Raw data: %s"), m_iAlignNum + 1, strData));
		return;
	}

	// 遍历每个数据包（最后一个token是空字符串，所以减1）
	for (int ii = 0; ii < responseTokens.GetSize() - 1; ii++)
	{
		strParsing = responseTokens[ii];

		// 检查STX起始符 (0x02)
		m_strHeader.Format(_T("%x"), strParsing.GetAt(0));
		UINT iHeader = (UINT)_ttoi(m_strHeader);

		if (iHeader != _STX)
		{
			LogWrite(m_iAlignNum,_T("STX No Message!!!"));
			return;
		}

		// 查找第一个逗号，分隔命令码和参数
		iFind = strParsing.Find(',');
		m_strCommand = strParsing.Left(iFind);

		// 跳过STX字符，提取命令码（数字字符串）
		iFindSTX = strParsing.Find((char)_STX);
		m_strCommand = m_strCommand.Mid(iFindSTX + 1, m_strCommand.GetLength());

		// 将命令码字符串转换为整数，对应VS_PacketNameTable中的索引
		int iCommand = _ttoi(m_strCommand);

		// 提取命令参数部分（逗号后的内容）
		m_strContents = strParsing.Mid(iFind + 1, strParsing.GetLength());

		m_lastCommand = VS_PacketNameTable[iCommand];
		m_lastRequest = m_strContents;

		// 记录接收日志：[VS -> MC] 表示从视觉系统发送到机器控制器
		theApp.m_pAlignSendReceiverLog[m_iAlignNum]->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), m_lastCommand, m_strContents));
		LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Received command %s with parameters: %s"), m_iAlignNum + 1, m_lastCommand, m_strContents));

		CString sendMsg;
		// 根据命令码执行相应处理
		switch (iCommand)
		{
		case VS_ARE_YOU_THERE:  // VS心跳检测命令
			// 重置心跳检测计数器，表示VS在线
			theApp.m_AlignThread[m_iAlignNum]->m_AlignCheckCount = 0;
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Received VS_ARE_YOU_THERE heartbeat"), m_iAlignNum + 1));
			break;
		case VS_PCTIME_REQUEST:  // VS请求PC时间同步
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Received VS_PCTIME_REQUEST"), m_iAlignNum + 1));
			AlignPcTimeRequest();  // 发送PC时间给VS
			break;
		case VS_STATE:  // VS发送状态信息
			// 参数：0=停止，1=运行
			theApp.m_AlignPCStatus[m_iAlignNum] = m_strContents == _T("0") ? FALSE : TRUE;
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Received VS_STATE: %s"), m_iAlignNum + 1, theApp.m_AlignPCStatus[m_iAlignNum] == TRUE ? _T("Start") : _T("Stop")));
			break;
		case VS_MODEL_REQUEST:  // VS请求当前模型名称
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Received VS_MODEL_REQUEST"), m_iAlignNum + 1));
			AlignModelRequest();  // 发送当前模型名称给VS
			break;
		case VS_MODEL_CREATE:  // VS通知模型创建完成
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align Manager %d: Received VS_MODEL_CREATE"), m_iAlignNum + 1));
			theApp.m_CreateModelAlign = FALSE;  // 清除模型创建标志
			// 通知PLC线程模型创建完成
			theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelCreate"), _T("Align"), theApp.m_CreateModelAlign);
			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Align Model Create Success")));

			// 如果模型需要切换，发送模型切换命令
			if (theApp.m_ChangeModelAlign)
			{
				sendMsg.Format(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
				theApp.m_AlignSocketManager[m_iAlignNum]->SocketSendto(m_iAlignNum, sendMsg, MC_MODEL_CHANGE);
			}
			break;
		case VS_MODEL_CHANGE:  // VS通知模型切换完成
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[VS -> MC] %s"), _T("RCV : VS_MODEL_CHANGE")));
			theApp.m_ChangeModelAlign = FALSE;  // 清除模型切换标志
			// 通知PLC线程模型切换完成
			theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelChange"), _T("Align"), theApp.m_ChangeModelAlign);
			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Align Model Change Success")));
			break;
		case VS_GRAB_END:  // VS通知图像采集完成
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[VS -> MC] %s->%s"), _T("RCV : VS_GRAB_END"), m_strContents));
	
			// 根据对齐类型处理图像采集结果
			if (m_iAlignType == PatternAlign)  // 图案对齐
				AlignGrabEnd(m_strContents);
			else if (m_iAlignType == TrayCheck)  // 托盘检查
				AlignTrayCheckGrabEnd(m_strContents);
			else if (m_iAlignType == TrayLowerAlign)  // 托盘下对齐
				AlignTrayLowerAlignGrabEnd(m_strContents); 
			else if (m_iAlignType == TrayAlign)  // 托盘对齐
				AlignTrayAlignGrabEnd(m_strContents);
		
			break;
		}
	}
}

void CAlignManager::AlignModelRequest()
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_MODEL, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
	SocketSendto(m_iAlignNum, sendMsg, MC_MODEL);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[MC -> VS] %s->%s"), MC_PacketNameTable[MC_MODEL], sendMsg));
}

void CAlignManager::AlignPcTimeRequest()
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_PCTIME, GetDateString4());
	SocketSendto(m_iAlignNum, sendMsg, MC_PCTIME);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[MC -> VS] %s->%s"), MC_PacketNameTable[MC_PCTIME], sendMsg));
}

void CAlignManager::AlignGrabEnd(CString strContents)
{
	
	AlignResult AlignResult;
	CString sendMsg;
	int plcResultMap, plcEndMap;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	int iPanelNum = (_ttoi(responseTokens[2]) - 1) % 2;
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_0"), iPanelNum));
	sendMsg.Format(_T("%d,"), MC_GRAB_END_RECEIVE);
	SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_END_RECEIVE);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_1"), iPanelNum));
	switch (iPanelNum)
	{
	case 0: plcEndMap = eBitType_Align1End1 + m_iAlignTypeNum, plcResultMap = eWordType_Align1Result1 + m_iAlignTypeNum; break;
	case 1: plcEndMap = eBitType_Align1End2 + m_iAlignTypeNum, plcResultMap = eWordType_Align1Result2 + m_iAlignTypeNum; break;
	}

	AlignResult.resultValue = responseTokens[3] == _T("0") ? m_codeOk : m_codeFail;
	AlignResult.resultX = _ttof(responseTokens[4]) * 10000;
	AlignResult.resultY = _ttof(responseTokens[5]) * 10000;
	AlignResult.resultT = _ttof(responseTokens[6]) * 10000;
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_2"), iPanelNum));
#if _SYSTEM_AMT_AFT_
	AlignDataSum(_ttoi(responseTokens[2]), AlignResult.resultValue, m_iAlignTypeNum);
#else
	//AlignDataSum(_ttoi(responseTokens[2]), AlignResult.resultValue, Machine_GAMMA);
#endif
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_3"), iPanelNum));
	theApp.m_pEqIf->m_pMNetH->SetAlignResult(plcResultMap, &AlignResult); 
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("%s Panel %d Result : %s[%d]"), AlignTypeName[m_iAlignType], iPanelNum, PLC_ResultValue[AlignResult.resultValue], AlignResult.resultValue));
	Delay(10, TRUE);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(plcEndMap, OffSet_0, TRUE);
	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].Reset();
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_4"), iPanelNum));
	//hdm test
	if (m_bstart && m_testT[iPanelNum])
	{
		m_testT[iPanelNum] = FALSE;
		theApp.m_AlignThread[m_iAlignType]->AlignGrabMethod(iPanelNum, Align_Start_XY, m_iAlignTypeNum);
	}
	else if (m_bstart)
	{
		m_testT[iPanelNum] = TRUE;
		theApp.m_AlignThread[m_iAlignType]->AlignGrabMethod(iPanelNum, Align_Start_T, m_iAlignTypeNum);
	}
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_5"), iPanelNum));
}

void CAlignManager::AlignTrayCheckGrabEnd(CString strContents)
{
	TrayCheckResult AlignResult;
	CString sendMsg;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	sendMsg.Format(_T("%d,"), MC_GRAB_END_RECEIVE);
	SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_END_RECEIVE);

	//종류			Comment				결과			    1번		2번
	//Tray Check	11(VS_GRAB_END)		1 = ok, 2 = ng	11100	11000

	int iResult = responseTokens[0] == _T("0") ? m_codeOk : m_codeFail;
	AlignResult.result[0] = _ttoi(responseTokens[1]); 
	AlignResult.result[1] = _ttoi(responseTokens[2]); 
	AlignResult.result[2] = _ttoi(responseTokens[3]);
	AlignResult.result[3] = _ttoi(responseTokens[4]);

	theApp.m_pEqIf->m_pMNetH->SetTrayCheckResult(eWordType_TrayCheckResult1 + m_iAlignTypeNum, &AlignResult);
	CString strMsg = m_iAlignNum == 0 ? _T("AOI") : _T("UNLOADER");
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[%s] %s Result : %s[%d]"), strMsg, AlignTypeName[m_iAlignType], PLC_ResultValue[iResult], iResult));
	Delay(10, TRUE);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayCheckEnd1 + m_iAlignTypeNum, OffSet_0, TRUE);
	theApp.m_lastAlignVec[m_iAlignNum][PanelNum1].Reset();

	//hdm test
	if (m_bstart)
		theApp.m_AlignThread[m_iAlignType]->TrayCheckNTrayAlignGrabMethod(PanelNum1);
}

void CAlignManager::AlignTrayAlignGrabEnd(CString strContents)
{
	AlignResult AlignResult;
	CString sendMsg;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	sendMsg.Format(_T("%d,"), MC_GRAB_END_RECEIVE);
	SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_END_RECEIVE);

	//종류			Comment			결과			X	Y	T
	//Tray Align	11(VS_GRAB_END)	1 = ok, 2 = ng	값	값	값

	AlignResult.resultValue = responseTokens[0] == _T("0") ? m_codeOk : m_codeFail;
	AlignResult.resultX = _ttof(responseTokens[1]) * 10000;
	AlignResult.resultY = _ttof(responseTokens[2]) * 10000;
	AlignResult.resultT = _ttof(responseTokens[3]) * 10000;

	theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_TrayAlignResult1 + m_iAlignTypeNum, &AlignResult);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("%s Result : %s[%d]"), AlignTypeName[m_iAlignType], PLC_ResultValue[AlignResult.resultValue], AlignResult.resultValue));
	Delay(10, TRUE);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayAlignEnd1 + m_iAlignTypeNum, OffSet_0, TRUE);
	theApp.m_lastAlignVec[m_iAlignNum][PanelNum1].Reset();

	//hdm test
	if (m_bstart)
		theApp.m_AlignThread[m_iAlignType]->TrayCheckNTrayAlignGrabMethod(PanelNum1);
}

void CAlignManager::AlignTrayLowerAlignGrabEnd(CString strContents)
{
	TrayLowerAlignResult AlignResult;
	CString sendMsg;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	sendMsg.Format(_T("%d,"), MC_GRAB_END_RECEIVE);
	SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_END_RECEIVE);

	//종류				Comment				PanelID		PanelID		Result			X	Y	T	X	Y	T
	//Tray Lower Align	11(VS_GRAB_END)		PanelID_1	PanelID_2	1 = ok, 2 = ng	값	값	값	값	값	값

	AlignResult.resultValue = responseTokens[2] == _T("0") ? m_codeOk : m_codeFail;
	AlignResult.resultX1 = _ttof(responseTokens[3]) * 10000;
	AlignResult.resultY1 = _ttof(responseTokens[4]) * 10000;
	AlignResult.resultT1 = _ttof(responseTokens[5]) * 10000;
	AlignResult.resultX2 = _ttof(responseTokens[6]) * 10000;
	AlignResult.resultY2 = _ttof(responseTokens[7]) * 10000;
	AlignResult.resultT2 = _ttof(responseTokens[8]) * 10000;

	theApp.m_pEqIf->m_pMNetH->SetTrayLowerAlignResult(eWordType_TrayLowerAlignResult1 + m_iAlignTypeNum, &AlignResult);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("%s Result : %s[%d]"), AlignTypeName[m_iAlignType], PLC_ResultValue[AlignResult.resultValue], AlignResult.resultValue));
	Delay(10, TRUE);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayLowerAlignEnd1 + m_iAlignTypeNum, OffSet_0, TRUE);
	theApp.m_lastAlignVec[m_iAlignNum][PanelNum1].Reset();


	//hdm test
	if (m_bstart)
		theApp.m_AlignThread[m_iAlignNum]->TrayLowerAlignGrabMethod(PanelNum1, m_iAlignTypeNum);
}

void CAlignManager::AlignDataSum(int iChNum, int iResultValue, int iInspSection)
{
	if (m_iAlignTypeNum == 0)
	{
#if _SYSTEM_AMT_AFT_
		int iIndexZone = (iChNum - 1) / 4;
		int iPanelNum = (iChNum - 1) % MaxZone;
#else
		int iPanelNum, iIndexZone;

		iPanelNum = (iChNum - 1) % 2;

		if (iPanelNum == PanelNum1)
			iIndexZone = iChNum / 2;
		else
			iIndexZone = (iChNum / 2) - 1;
#endif
		if (iInspSection == Machine_AOI || iInspSection == Machine_GAMMA)
		{
			if (iResultValue == m_codeFail)
			{
				theApp.m_shiftProduction[iIndexZone].m_AlignResult[theApp.m_lastShiftIndex]++;
				theApp.m_UiShiftProduction[iIndexZone].m_AlignResult[theApp.m_lastShiftIndex]++;

				theApp.m_shift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignResult[theApp.m_lastShiftIndex]++;
				theApp.m_UiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignResult[theApp.m_lastShiftIndex]++;

				theApp.m_shiftProduction[iIndexZone].m_AlignShiftNg[theApp.m_lastShiftIndex][iPanelNum]++;
				theApp.m_UiShiftProduction[iIndexZone].m_AlignShiftNg[theApp.m_lastShiftIndex][iPanelNum]++;

				theApp.m_shift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftNg[theApp.m_lastShiftIndex][iPanelNum]++;
				theApp.m_UiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftNg[theApp.m_lastShiftIndex][iPanelNum]++;
			}
			else
			{
				theApp.m_shiftProduction[iIndexZone].m_AlignShiftGood[theApp.m_lastShiftIndex][iPanelNum]++;
				theApp.m_UiShiftProduction[iIndexZone].m_AlignShiftGood[theApp.m_lastShiftIndex][iPanelNum]++;

				theApp.m_shift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftGood[theApp.m_lastShiftIndex][iPanelNum]++;
				theApp.m_UiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftGood[theApp.m_lastShiftIndex][iPanelNum]++;
			}

			theApp.AlignDataSave(theApp.m_lastShiftIndex);
		}
		else
		{
#if _SYSTEM_AMT_AFT_
			int iPanelNum = (iChNum - 1) % 2;

			if (iResultValue == m_codeFail)
			{
				theApp.m_ULDshiftProduction[iPanelNum].m_AlignResult[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShiftProduction[iPanelNum].m_AlignResult[theApp.m_lastShiftIndex]++;

				theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignResult[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignResult[theApp.m_lastShiftIndex]++;

				theApp.m_ULDshiftProduction[iPanelNum].m_AlignShiftNg[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShiftProduction[iPanelNum].m_AlignShiftNg[theApp.m_lastShiftIndex]++;

				theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftNg[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftNg[theApp.m_lastShiftIndex]++;
			}
			else
			{
				theApp.m_ULDshiftProduction[iPanelNum].m_AlignShiftGood[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShiftProduction[iPanelNum].m_AlignShiftGood[theApp.m_lastShiftIndex]++;

				theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftGood[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftGood[theApp.m_lastShiftIndex]++;
			}

			theApp.ULDAlignDataSave(theApp.m_lastShiftIndex);
#endif
		}
	}
}

void CAlignManager::OnEvent(UINT uEvent, LPVOID lpvData)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	switch (uEvent)
	{
	case EVT_CONDROP:
		LogWrite(m_iAlignNum, _T("Align PC Connect Drop"));
		break;
 	case EVT_CONSUCCESS:
		LogWrite(m_iAlignNum, _T("Align PC Connect Success"));
		break;
	case EVT_ZEROLENGTH:
		LogWrite(m_iAlignNum, _T("Align PC EVT_ZEROLENGTH"));
		break;
	case EVT_CONFAILURE:
		LogWrite(m_iAlignNum, _T("Align PC EVT_CONFAILURE"));
		break;
	default:
		LogWrite(m_iAlignNum, _T("Unknown Socket event"));
		break;
	}
}