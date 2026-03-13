
#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "DlgMainLog.h"
#include "TpManager.h"

/*
======================================================================
TP（触摸测试）通信流程简要说明（给非软件工程人员看的说明）
----------------------------------------------------------------------
1. 模块作用
   - `CTpManager` 负责与 TP 测试设备之间的 Socket 通信：
     - 发送：开始测试、下发产品/面板信息等命令
     - 接收：TP 测试结果 OK/NG、TP Code 等
   - 一端连 TP 控制器，另一端通过 `MNetH` 把 TP 结果写入 PLC，并记录到 DFS 文件。

2. 命令方向：Data Server → TP（发命令）
   - 命令格式统一以 `#` 开头和结束，中间用 `*` 分隔：
     - `#CK*Ch#`：连接检查（Check Connect）
     - `#JZ*Ch*ProductID#`：下发产品 ID
     - `#PD*Ch*PanelID#`：下发面板 ID
     - `#ST*Ch#`：开始 TP 检测
     - `#RT*Ch*OK#`：本系统给 TP 的结果回应（确认已收到 TP 结果）
     - `#RE*Ch#`：请求 TP Code（与 MES/PG Code 做一致性比对）
   - 对应实现函数：`SendTPMessage`，用于根据命令类型拼好字符串并通过 Socket 发送。

3. 响应方向：TP → Data Server（回结果/代码）
   - TP 测试结束会返回类似：
     - `#RT*1*OK#`：1 通道测试 OK
     - `#RT*2*NG#`：2 通道测试 NG
     - `#TP*1*Code123#`：1 通道的 TP Code 为 Code123
   - 在 `OnDataReceived` 中：
     1）先按 `#` 找出一个完整数据包（兼容多个包连续粘在一起的情况）
     2）再用 `*` 分割出“命令类型、通道号、结果/Code”
     3）根据通道号判断属于哪个区域/工位（AOI 侧 or UNLOADER 手动侧）
     4）如果是结果包 `#RT`：
        - 把 OK/NG 写入 PLC 对应字地址（例如 AZoneTouchResult / MStageATouchResult）
        - 置位“TP 检测结束”比特，通知 PLC 该 Panel 的 TP 测试完成
        - 回发 `#RT*Ch*OK#` 给 TP，表示系统已接收结果
     5）如果是 Code 包 `#TP`：
        - 存入内部结构，与 MES/PG 下发的标准 Code 做比对，并给出 OK/NG 结果写回 PLC。

4. 与其他模块的协同关系
   - 与 PG：
     - 一般先由 PG 完成 CONTACT ON（接触），再由 TP 执行触摸测试；
     - 某些异常情况下，会通过 PG 通道来重新点亮或复位，然后再触发 TP。
   - 与 VS/OPV：
     - TP 结果会作为面板综合判定的一部分，最终与 AOI/OPV 结果一起写入 DFS 和 MES。

5. 现场排查提示（给非软件人员参考）
   - TP 界面显示测试完成，但本机 TP 日志中看不到 `#RT*Ch*...#`：优先检查 TP → Data Server 网络链路。
   - 日志中收到 `#RT*Ch*OK#`，但 PLC 侧 TP 结果不变：检查 PLC 地址表/MNetH 配置。
   - 多个通道结果错位：重点检查通道号与区域/工位的映射关系（iChNum、Zone/PanelIndex 计算部分）。
======================================================================
*/

CTpManager::CTpManager()
{	
	m_lastContent.resize(18);
	m_lastResult.resize(18);
	m_lastRequest.resize(18);
}

CTpManager::~CTpManager()
{
}

void CTpManager::SendTPMessage(int iChNum, int iCommand, CString strContent)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	m_csTpData.Lock();

	CString strSendMsg = _T("");
	switch (iCommand)
	{
	case TP_CheckConnect: strSendMsg.Format(_T("#CK*%d#"), iChNum + 1); break;
	case TP_ProductID: strSendMsg.Format(_T("#JZ*%d*%s#"), iChNum + 1 , strContent); break;
	case TP_SendPanelID: strSendMsg.Format(_T("#PD*%d*%s#"), iChNum + 1, strContent); break;
	case TP_InspStart: strSendMsg.Format(_T("#ST*%d#"), iChNum + 1); break;
	case TP_InspResult: strSendMsg.Format(_T("#RT*%d*OK#"), iChNum + 1); break;
	case TP_CodeRequest: strSendMsg.Format(_T("#RE*%d#"), iChNum + 1); break; //210422
	}

	int iLen = strSendMsg.GetLength();

	char *lpCommand = StringToChar(strSendMsg);
	theApp.m_TpSocketManager.WriteComm((BYTE*)lpCommand, iLen, 100L);

	m_lastContent[iChNum] = strSendMsg;
	delete lpCommand;

	theApp.m_pTpSendReceiverLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] [MC -> TP] %s"), GetNowSystemTimeMilliseconds(), strSendMsg));

	m_csTpData.Unlock();
}

void CTpManager::TpLogMessage(CString strContents)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	m_csTpData.Lock();

	g_MainLog->m_TouchListBox.InsertString(0, CStringSupport::FormatString(_T("[%s] %s"), GetNowSystemTimeMilliseconds(), strContents));
	theApp.m_pTpLog->LOG_INFO(strContents);

	m_csTpData.Unlock();
}

/**
 * @brief 接收来自TP (Test Pattern, 测试模式) 设备的协议数据包并处理
 * 
 * TP协议格式：# + 命令类型 + * + 通道号 + * + 结果 + #
 * - #: 起始和结束标识符
 * - 命令类型：
 *   - "#RT": 检测结果 (Result Test)
 *   - "#TP": TP代码 (Test Pattern Code)
 * - 通道号：1-18，标识检测通道
 * - 结果：
 *   - "OK#": 检测通过
 *   - "NG#": 检测失败
 *   - 其他：错误码或状态信息
 * 
 * 协议示例：
 * - "#RT*1*OK#" - 通道1检测通过
 * - "#RT*2*NG#" - 通道2检测失败
 * - "#TP*1*Code123#" - 通道1的TP代码为Code123
 * 
 * 注意：多个数据包可能连续发送，需要按#分割处理
 * 
 * @param lpBuffer 接收到的数据缓冲区
 * @param dwCount 数据长度（字节数）
 */
void CTpManager::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	m_strDummyContents.Format(_T(""));
	m_strDummyContents.ReleaseBuffer();

	if (theApp.m_bExitFlag == FALSE)
		return;

	CString strData;
	// 将接收到的字节数据转换为Unicode字符串
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount, strData.GetBuffer(dwCount + 1), dwCount + 1);
	strData.ReleaseBuffer(dwCount);

	// 记录接收日志：[TP -> MC] 表示从测试模式设备发送到机器控制器
	theApp.m_pTpSendReceiverLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] [TP -> MC] %s"), GetNowSystemTimeMilliseconds(), strData));

	CString strHeader = _T(""), strContent = _T("");
	strHeader.Format(_T("#"));
	// 处理连续发送的多个数据包（Packet이 연달아 붙어서 올 경우 처리 할 수 있도록）
	// 查找第一个完整的数据包（以#开始，以#结束）
	int iFind = 0;
	if (strHeader == strData.Left(1))
	{
		// 从第2个字符开始查找下一个#，找到第一个完整数据包的结束位置
		for (int ii = 2; ii < strData.GetLength() + 1; ii++)
		{
			CString Msg = strData.Left(ii).Right(1);
			if (strHeader == Msg)
			{
				strContent = strData.Left(ii);  // 提取第一个完整数据包
				iFind = ii;
				break;
			}
		}
	}
	else
	{
		theApp.m_pTpLog->LOG_INFO(_T("[%s] TP Protocol Error !!!"), strData);
		return;
	}
	// 如果还有剩余数据，保存到m_strDummyContents，下次处理
	int iLen = strData.GetLength();
	if (iFind < iLen - 1)
	{
		m_strDummyContents = strData.Mid(iFind, strData.GetLength());
	}

	// 按*分割数据包内容：命令类型 * 通道号 * 结果
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContent, _T('*'), responseTokens);

	CString strIndexName = _T("");
	int iZoneNum = 0;
	int iPanelNum = 0;
	// responseTokens[1] 是通道号（字符串），转换为整数并减1得到索引（0-17）
	int iChNum = _ttoi(responseTokens[1]) - 1;
	BOOL bMachine = FALSE;

	// 计算区域号和面板号
	// 面板号 = 通道号 % 4（每个区域4个面板）
	// 区域号 = 通道号 / 4（每4个通道为一个区域）
	iPanelNum = iChNum % 4;
	iZoneNum = iChNum / 4;

	//int iTTT = iChNum + 1 / 16; // 2/16 이 왜 1이야??????????
	//switch (iTTT)
	//{
	//case Machine_AOI: strIndexName = PG_IndexName[iZoneNum],      bMachine = Machine_AOI; break;
	//case Machine_ULD: strIndexName = ULD_PG_IndexName[iPanelNum], bMachine = Machine_ULD; break;
	//}

	if (iChNum < 16)
	{
		strIndexName = PG_IndexName[iZoneNum];
		bMachine = Machine_AOI;
	}
	else
	{
		strIndexName = ULD_PG_IndexName[iPanelNum];
		bMachine = Machine_ULD;
	}

	BOOL bTpResponseResult = TRUE;

	m_lastRequest[iChNum] = strContent;

	if (bMachine == Machine_AOI)
	{
		for (auto &InspResult : theApp.m_lastIndexPgVec[iZoneNum])
		{
			if (InspResult.m_bInspStart == TRUE && InspResult.m_iStatus == PG_TOUCH)
			{
				if (InspResult.m_iPanelNum == iPanelNum)
				{
					if (!responseTokens[0].CompareNoCase(_T("#RT")))
					{
						if (!responseTokens[2].CompareNoCase(_T("OK#")))
						{
							theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneTouchResult + iZoneNum, iPanelNum, &m_codeOk);
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchInspectionEnd + iZoneNum, iPanelNum, TRUE);

							if (bTpResponseResult)
							{
								bTpResponseResult = FALSE;
								InspResult.time_check.StopTimer();
								InspResult.m_bResult = TRUE;
								theApp.m_TpSocketManager.SendTPMessage(iChNum, TP_InspResult);
								TpLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d TP OK"), strIndexName, iChNum + 1));
							}
						}
						else
						{
							if (theApp.m_TpPassMode)
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneTouchResult + iZoneNum, iPanelNum, &m_codeOk);
							else
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneTouchResult + iZoneNum, iPanelNum, &m_codeFail);

							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchInspectionEnd + iZoneNum, iPanelNum, TRUE);

							if (bTpResponseResult)
							{
								bTpResponseResult = FALSE;
								InspResult.time_check.StopTimer();
								InspResult.m_bResult = TRUE;
								theApp.m_TpSocketManager.SendTPMessage(iChNum, TP_InspResult);
								TpLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d TP NG"), strIndexName, iChNum + 1));
								
							}
						}

						m_lastResult[iChNum] = responseTokens[2];
					}
				}
				//>>210422
				if (!responseTokens[0].CompareNoCase(_T("#TP")))
				{
					//>>0126 yjlim
					if (theApp.m_bPGCodeUsable == TRUE)
					{
						int iCheckErr(PGCode_NG); // 0: GOOD, 1: NGCode PG, Mes 안맞음, 2: TimeOver						
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh1Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh2Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh3Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh4Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh5Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh6Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh7Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh8Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh9Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh10Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh11Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh12Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh13Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh14Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh15Result + iZoneNum, iPanelNum, &iCheckErr);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_TPCodeCh16Result + iZoneNum, iPanelNum, &iCheckErr);
					}

				}
				//<<
			}
		}
	}
	else
	{
		if (theApp.m_VecManualStage[ManualStageTouch][iPanelNum].m_bInspStart == TRUE)
		{
			if (!responseTokens[0].CompareNoCase(_T("#RT")))
			{
				if (!responseTokens[2].CompareNoCase(_T("OK#")))
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageATouchResult + iPanelNum, &m_codeOk);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchEnd + iPanelNum, OffSet_0, TRUE);

					if (bTpResponseResult)
					{
						bTpResponseResult = FALSE;
						theApp.m_VecManualStage[ManualStageTouch][iPanelNum].time_check.StopTimer();
						theApp.m_VecManualStage[ManualStageTouch][iPanelNum].m_bResult = TRUE;
						theApp.m_TpSocketManager.SendTPMessage(iChNum, TP_InspResult);
						TpLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d TP OK"), strIndexName, iChNum + 1));
					}

					if (iChNum + 1 == 17)
					{
						CString strMsg;
						strMsg = CStringSupport::FormatString(_T("%d,%d"), MC_MANUAL_TP_NG, 1);
						theApp.m_OpvSocketManager[0].SendOpvMessage(strMsg, 0, MC_MANUAL_TP_NG);

					}
					if (iChNum + 1 == 18)
					{
						CString strMsg;
						strMsg = CStringSupport::FormatString(_T("%d,%d"), MC_MANUAL_TP_NG, 1);
						theApp.m_OpvSocketManager[1].SendOpvMessage(strMsg, 1, MC_MANUAL_TP_NG);
					}

				}
				else
				{
					if (theApp.m_TpPassMode)
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageATouchResult + iPanelNum, &m_codeOk);
					else
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageATouchResult + iPanelNum, &m_codeFail);

					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchEnd + iPanelNum, OffSet_0, TRUE);

					if (bTpResponseResult)
					{
						bTpResponseResult = FALSE;
						theApp.m_VecManualStage[ManualStageTouch][iPanelNum].time_check.StopTimer();
						theApp.m_VecManualStage[ManualStageTouch][iPanelNum].m_bResult = TRUE;
						theApp.m_TpSocketManager.SendTPMessage(iChNum, TP_InspResult);
						TpLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d TP NG"), strIndexName, iChNum + 1));
					}
					if (iChNum + 1 == 17)
					{
						CString strMsg;
						strMsg = CStringSupport::FormatString(_T("%d,%d"), MC_MANUAL_TP_NG, 0);
						theApp.m_OpvSocketManager[0].SendOpvMessage(strMsg, 0, MC_MANUAL_TP_NG);

					}
					if (iChNum + 1 == 18)
					{
						CString strMsg;
						strMsg = CStringSupport::FormatString(_T("%d,%d"), MC_MANUAL_TP_NG, 0);
						theApp.m_OpvSocketManager[1].SendOpvMessage(strMsg, 1, MC_MANUAL_TP_NG);
					}
				}

				if (theApp.m_iMachineType == SetAMT)
				{
					int iPgServerNum = iChNum == PG_CH_17 ? PgServer_2 : PgServer_3;
					CString strMsg = CStringSupport::FormatString(_T("Ch,%d,TURNON"), iChNum + 1);
					theApp.m_PgSocketManager[iPgServerNum].SendPGMessage(strMsg, iChNum + 1);
				}

				m_lastResult[iChNum] = responseTokens[2];
			}
		}
	}
	CStringA str;
	str = m_strDummyContents;

	if (m_strDummyContents.GetLength() > 1)
		OnDataReceived((LPBYTE)(LPCSTR)str, str.GetLength());
}

BOOL CTpManager::getConectCheck()
{
	SockAddrIn addrin;
	GetSockName(addrin);
	LONG  uAddr = addrin.GetIPAddr();
	if (uAddr == 0)
		return FALSE;
	else
		return TRUE;	
}

bool CTpManager::SocketServerOpen(CString strServerPort)
{
	m_bMelsecSimulaion = true;
	SetSmartAddressing(false);
	SetServerState(true);
	bool ret = CreateSocket(strServerPort, AF_INET, SOCK_STREAM, 0);
	if (ret) return WatchComm();
	else return false;
}

void CTpManager::OnEvent(UINT uEvent, LPVOID lpvData)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	switch (uEvent)
	{
	case EVT_CONDROP:
		TpLogMessage(CStringSupport::FormatString(_T("TP Connect Drop")));
		break;
	case EVT_CONSUCCESS:
		TpLogMessage(CStringSupport::FormatString(_T("TP Connect Success")));
		break;
	case EVT_ZEROLENGTH:
		TpLogMessage(CStringSupport::FormatString(_T("TP EVT_ZEROLENGTH")));
		break;
	case EVT_CONFAILURE:
		TpLogMessage(CStringSupport::FormatString(_T("TP EVT_CONFAILURE")));
		break;
	default:
		TpLogMessage(CStringSupport::FormatString(_T("Unknown Socket event")));
		break;
	}
}
#endif