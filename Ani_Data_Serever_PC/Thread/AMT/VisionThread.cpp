
#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "DlgMainView.h"
#include "VisionThread.h"
#include "DFSInfo.h"


CVisionThread::CVisionThread()
{
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bFirstStatus = TRUE;
	m_lastContent.resize(2);
	m_lastCommand.resize(2);
	m_lastRequest.resize(2);
	theApp.m_bVisionDeleteFlag = TRUE;
	theApp.m_lastInspResultVec.resize(8);
}

CVisionThread::~CVisionThread()
{
}

void CVisionThread::ThreadRun()
{
	theApp.m_PlcLog->LOG_INFO(_T("[VisionThread] Thread started"));
	AutoFocusData pAutoFocusData;
	for (auto &InspResult : theApp.m_lastInspResultVec)
		InspResult.Reset();

	while (::WaitForSingleObject(m_hQuit, 50) != WAIT_OBJECT_0)
		{
			// 5601 端口 (Lighting) 连接状态
			theApp.m_VisionConectStatus[0] = theApp.m_LightingConectStatus;
			theApp.m_VisionConectStatus[1] = theApp.m_LightingConectStatus;

			//if (theApp.m_bAllPassMode)
			//	continue;

			theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
				_T("[VisionThread] Connection status - LightingConect=%d, AOIPassMode=%d"), 
				theApp.m_LightingConectStatus, theApp.m_AOIPassMode));

			if (theApp.m_LightingConectStatus || theApp.m_AOIPassMode)
		{
			// Lighting(点灯检) 新协议流程超时兜底：避免 PLC 因未回包一直卡住
			theApp.LightingFlowTimeoutCheck();

			if (m_bFirstStatus)
			{
				theApp.m_PlcLog->LOG_INFO(_T("[VisionThread] First connection status detected"));
				m_bFirstStatus = FALSE;
				time_check.SetCheckTime(60000);
			time_check.StartTimer();
				for (int ii = 0; ii < PCMaxCount; ii++)
				{
					// VisionFirstCheckMethod is commented out as Vision PC is not available
					// VisionFirstCheckMethod(ii);
					// 已禁用 Vision PC 协议，现在只使用 Lighting
					// theApp.m_VisionPCStatus[ii] = TRUE;
				}

				for (int jj = 0; jj < PanelMaxCount; jj++)
					m_bStartVision[jj] = FALSE;
				theApp.m_PlcLog->LOG_INFO(_T("[VisionThread] Initialized all panel start flags to FALSE"));
			}

			//TEST Model �� (TRUE) Model ���浵 �Ⱥ��� �׳� ��� ���� �մϴ�.
			//TEST Model �� (FALSE) Model ���� �� ���� ��� check 
			if (theApp.m_AOIPassMode == FALSE)
			{
				theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
					_T("[VisionThread] AOIPassMode=FALSE, checking PLC connection and model change - PLC=%d, ModelChange1=%d, ModelChange2=%d"),
					theApp.m_PlcConectStatus, theApp.m_ChangeModelVision1, theApp.m_ChangeModelVision2));
				/*if (theApp.m_PlcConectStatus == FALSE || theApp.m_ChangeModelVision1 == TRUE || theApp.m_ChangeModelVision2 == TRUE)
				{
					theApp.m_PlcLog->LOG_INFO(_T("[VisionThread] Skipping due to PLC disconnected or model change in progress"));
					continue;
				}*/
			}

			if (time_check.IsTimeOver())
			{
				// VisionCheckMethod is commented out as Vision PC is not available
				time_check.StartTimer();
			}

			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_VisionPlcSend, 0))
			{
				theApp.m_PlcLog->LOG_INFO(_T("[VisionThread] Received eBitType_VisionPlcSend signal from PLC"));
				VisionPanelCheck();
			}
			else
			{
				theApp.m_PlcLog->LOG_INFO(_T("[VisionThread] Received eBitType_VisionPlcSend not signal from PLC"));
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionPcReceiver, 0, FALSE);
			}

			for (int ii = 0; ii < PanelMaxCount; ii++)
			{
				m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_VisionStart1, OffSet_0 + ii);
				theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
					_T("[VisionThread] Panel %d StartFlag=%d"), ii + 1, m_bStartFlag));

				if (m_bStartFlag == FALSE)
				{
				theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
					_T("[VisionThread] Resetting Panel %d results and flags"), ii + 1));
				theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
					_T("[VisionThread] SetPlcWordData eWordType_VisionResult%d = %d (%s)"), 
					ii + 1, m_codeReset, PLC_ResultValue[m_codeReset]));
				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_VisionResult1 + ii, &m_codeReset);
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionGrabEnd1 + ii, OffSet_0, FALSE);
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionEnd1 + ii, OffSet_0, FALSE);
			 }

				if (m_bStartVision[ii] == !m_bStartFlag)
				{
					m_bStartVision[ii] = m_bStartFlag;
					theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(_T("[VisionThread] Panel %d StartFlag changed to %d"), ii + 1, m_bStartVision[ii]));
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Vision Panel %d Start Flag [%s]"), ii + 1, m_bStartVision[ii] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bStartFlag == TRUE)
					{
						theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(_T("[VisionThread] Starting inspection for Panel %d"), ii + 1));
						theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
							_T("[VisionThread] SetPlcWordData eWordType_VisionResult%d = %d (%s)"), 
							ii + 1, m_codeReset, PLC_ResultValue[m_codeReset]));
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_VisionResult1 + ii, &m_codeReset);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionGrabEnd1 + ii, OffSet_0, FALSE);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionEnd1 + ii, OffSet_0, FALSE);

						// 新版点灯检软件：优先走 Lighting 协议 Start$...@
						theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
							_T("[VisionThread] Lighting check: m_LightingThreadOpenFlag=%d, m_LightingConectStatus=%d"),
							theApp.m_LightingThreadOpenFlag, theApp.m_LightingConectStatus));

						if (theApp.m_LightingThreadOpenFlag && theApp.m_LightingConectStatus)
						{
							theApp.m_PlcLog->LOG_INFO(_T("[VisionThread] Using Lighting protocol (6501 port) for inspection"));
							BOOL startFlags[4] = { FALSE, FALSE, FALSE, FALSE };
							for (int jj = 0; jj < PanelMaxCount; ++jj)
								startFlags[jj] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_VisionStart1, OffSet_0 + jj);

							theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
								_T("[VisionThread] startFlags[] = [%d, %d, %d, %d] from VisionStart1~4"),
								startFlags[0], startFlags[1], startFlags[2], startFlags[3]));
							theApp.TryStartLightingFromPlc(startFlags);
						}
						else
						{
							theApp.m_PlcLog->LOG_WARN(CStringSupport::FormatString(
								_T("[VisionThread] Lighting NOT available, skipping TryStartLightingFromPlc")));
							// 旧版 Vision PC 协议 - 已禁用，现在只使用 Lighting (5601端口)
							// m_iPcNum = ii <= PanelNum2 ? PC1 : PC2;
							// VisionInspectionMethod(m_iPcNum, PanelNum1 + ii);
						}
					}
				}
			}

			for (int ii = 0; ii < MaxCamCount; ii++)
			{
				m_bAutoFocusStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AutoFocusEnd1 + ii, OffSet_0);
				theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
					_T("[VisionThread] Cam %d AutoFocusEndFlag=%d"), ii + 1, m_bAutoFocusStartFlag));

				if (m_bAutoFocusStartFlag == TRUE)
				{
					theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(_T("[VisionThread] Processing AutoFocusEnd for Cam %d"), ii + 1));
					theApp.m_pEqIf->m_pMNetH->SetAutoFocusData(eWordType_AutoFocusMoter1, &pAutoFocusData);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AutoFocusSave1 + ii, OffSet_0, FALSE);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AutoFocusStart1 + ii, OffSet_0, FALSE);
			 }

				if (m_bAutoFocusStart[ii] == !m_bAutoFocusStartFlag)
				{
					m_bAutoFocusStart[ii] = m_bAutoFocusStartFlag;
					theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(_T("[VisionThread] Cam %d AutoFocusEndFlag changed to %d"), ii + 1, m_bAutoFocusStart[ii]));
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Auto Focus Cam_%d End Flag [%s]"), ii + 1, m_bAutoFocusStart[ii] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bAutoFocusStartFlag == TRUE)
					{
						theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(_T("[VisionThread] Processing AutoFocusEnd for Cam %d"), ii + 1));
						if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AutoFocusSave1 + ii, OffSet_0))
					{
						theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(_T("[VisionThread] AutoFocus Save position for Cam %d"), ii + 1));
						//save - 已禁用 Vision PC 协议，现在只使用 Lighting
						//theApp.m_VisionSocketManager->SocketSendto(ii, _T("GOOD"), MC_FOCUS_SAVE_POS_DONE);
						theApp.m_pEqIf->m_pMNetH->SetAutoFocusData(eWordType_AutoFocusMoter1, &pAutoFocusData);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AutoFocusSave1 + ii, OffSet_0, FALSE);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AutoFocusStart1 + ii, OffSet_0, FALSE);

						LogWrite(CStringSupport::FormatString(_T("[CAM_%d] Auto Focus Save Success"), ii + 1), ii);
					}
						else
						{
							theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(_T("[VisionThread] AutoFocus Axis Move for Cam %d"), ii + 1));
							//axis Move - 已禁用 Vision PC 协议，现在只使用 Lighting
							//theApp.m_pEqIf->m_pMNetH->GetPlcWordData(eWordType_AutoFocusMoter1, &m_AutoFocusPosition);
							//if (m_AutoFocusPosition == 1)
							//	theApp.m_VisionSocketManager->SocketSendto(ii, _T("GOOD"), MC_Z_MOVE_DONE);
							//else
							//	theApp.m_VisionSocketManager->SocketSendto(ii, _T("GOOD"), MC_FOCUS_MOVE_DONE);

							theApp.m_pEqIf->m_pMNetH->SetAutoFocusData(eWordType_AutoFocusMoter1, &pAutoFocusData);
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AutoFocusSave1 + ii, OffSet_0, FALSE);
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AutoFocusStart1 + ii, OffSet_0, FALSE);

							LogWrite(CStringSupport::FormatString(_T("[CAM_%d] Auto Focus Axis Move Success"), ii + 1), ii);
						}
					}
				}
			}
			
			for (auto &InspResult : theApp.m_lastInspResultVec)
			{
				if (InspResult.m_LastCheck == TRUE)
				{
					theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
						_T("[VisionThread] Processing LastCheck for PC%d, Panel%d, Cell=%s"),
						InspResult.m_iPCNum, InspResult.m_iPanelNum, InspResult.m_cellId));
					if (InspResult.time_check.IsTimeOver())
					{
						theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
							_T("[VisionThread] Timeout occurred for PC%d, Panel%d, Cell=%s"),
							InspResult.m_iPCNum, InspResult.m_iPanelNum, InspResult.m_cellId));
						VisionPLCResult(InspResult.m_iPCNum,
							InspResult.m_iPanelNum,
							PLC_ResultValue[m_codeTimeOut],
							m_codeTimeOut,
							InspResult.m_cellId);

						InspResult.m_bResult = TRUE;
						theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[PC : %d] AOI [%s] Time out"), InspResult.m_iPCNum, InspResult.m_cellId));
					}
				}
				else if (InspResult.m_bInspStart == TRUE)
				{
					theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
						_T("[VisionThread] Processing InspectionStart for PC%d, Panel%d, Cell=%s"),
						InspResult.m_iPCNum, InspResult.m_iPanelNum, InspResult.m_cellId));
					if (InspResult.time_check.IsTimeOver())
					{
						theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
							_T("[VisionThread] Sending inspection end command for PC%d, Cell=%s"),
							InspResult.m_iPCNum, InspResult.m_cellId));
						CString sendMsg;
						sendMsg.Format(_T("%d,%s"), MC_INSPECTION_END, InspResult.m_cellId);
						SocketSendto(InspResult.m_iPCNum, sendMsg, MC_INSPECTION_END);
						LogWrite(CStringSupport::FormatString(_T("Vision %d Inspection Last Request Start")), InspResult.m_iPCNum);
						InspResult.m_LastCheck = TRUE;
						if (theApp.m_iTimer[VisionLastGrabTimer] == 0)
							InspResult.time_check.SetCheckTime(1000);
						else
							InspResult.time_check.SetCheckTime(theApp.m_iTimer[VisionLastGrabTimer] * 1000);

						InspResult.time_check.StartTimer();
					}
				}

				if (InspResult.m_bResult == TRUE && theApp.m_bVisionDeleteFlag == TRUE)
				{
					theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
						_T("[VisionThread] Resetting inspection result for PC%d, Panel%d, Cell=%s"),
						InspResult.m_iPCNum, InspResult.m_iPanelNum, InspResult.m_cellId));
					InspResult.Reset();
				}

			}
			
		}
		else
		{
			theApp.m_PlcLog->LOG_INFO(_T("[VisionThread] Vision PC connection lost, resetting status"));
			// Vision PC 状态已禁用，现在只使用 Lighting (5601端口)
			// theApp.m_VisionPCStatus[0] = FALSE;
			// theApp.m_VisionPCStatus[1] = FALSE;
			m_bFirstStatus = TRUE;
		}
	}
	theApp.m_PlcLog->LOG_INFO(_T("[VisionThread] Thread exited"));
}


// 已禁用 - 现在只使用 Lighting 协议 (5601端口)，不再处理 Vision PC 协议 (8011/8012端口)
// void CVisionThread::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
// {
// 	if (theApp.m_bExitFlag == FALSE)
// 		return;
// 
// 	SockAddrIn addrin;
// 	GetSockName(addrin);
// 	int Num = ntohs(addrin.GetPort()) == _ttoi(VISION_PC1_PORT_NUM) ? PC1 : PC2;
// 
// 	CString strData, m_strHeader, m_strCommand, m_strContents, strParsing;
// 	int iFind, iFindSTX;
// 	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount, strData.GetBuffer(dwCount + 1), dwCount + 1);
// 	strData.ReleaseBuffer(dwCount);
// 
// 	CStringArray responseTokens;
// 	CStringSupport::GetTokenArray(strData, _ETX, responseTokens);
// 
// 	if (responseTokens.GetSize() == 1)
// 	{
// 		LogWrite(_T("ETX No Message!!!"), Num);
// 		return;
// 	}
// 
// 	for (int ii = 0; ii < responseTokens.GetSize() - 1; ii++)
// 	{
// 		strParsing = responseTokens[ii];
// 
// 		m_strHeader.Format(_T("%x"), strParsing.GetAt(0));
// 
// 		UINT iHeader = (UINT)_ttoi(m_strHeader);
// 
// 		if (iHeader != _STX)
// 		{
// 			LogWrite(_T("STX No Message!!!"), Num);
// 			return;
// 		}
// 
// 		iFind = strParsing.Find(',');
// 		m_strCommand = strParsing.Left(iFind);
// 
// 		iFindSTX = strParsing.Find((char)_STX);
// 		m_strCommand = m_strCommand.Mid(iFindSTX + 1, m_strCommand.GetLength());
// 
// 		int iCommand = _ttoi(m_strCommand);
// 
// 		m_strContents = strParsing.Mid(iFind + 1, strParsing.GetLength());
// 
// 		m_lastCommand[Num] = VS_PacketNameTable[iCommand];
// 		m_lastRequest[Num] = m_strContents;
// 
// 		if (Num == PC1)
// 			theApp.m_pVisionSendReceiver1Log->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), m_lastCommand[Num], strData));
// 		else
// 			theApp.m_pVisionSendReceiver2Log->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), m_lastCommand[Num], strData));
// 
// 		CString sendMsg;
// 		switch (iCommand)
// 		{
// 		case VS_ARE_YOU_THERE:
// 			theApp.m_VisionSocketManager[Num].m_iVisionSocketCheckCount = 0;
// 			break;
// 		case VS_PCTIME_REQUEST:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_PCTIME_REQUEST")), Num);
// 			ParsingPcTimeRequest(Num, m_strContents);			
// 			sendMsg.Format(_T("%d,%d"), MC_STATE, !theApp.m_PlcThread->m_plcStart);
// 			for (int ii = 0; ii < PCMaxCount; ii++)
// 				theApp.m_VisionSocketManager[ii].SocketSendto(ii, sendMsg, MC_STATE);
// 			break;
// 		case VS_STATE:
// 			theApp.m_VisionPCStatus[Num] = m_strContents == _T("0") ? FALSE : TRUE;
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_STATE"), theApp.m_VisionPCStatus[Num] == TRUE ? _T("Start") : _T("Stop")), Num);
// 			break;
// 		case VS_MODEL:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL")), Num);
// 			ParshingVisionData(Num, m_strContents);
// 			break;
// 		case VS_MODEL_REQUEST:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL_REQUEST")), Num);
// 			ParsingModelRequest(Num, m_strContents);
// 			break;
// 		case VS_MODEL_CREATE:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL_CREATE")), Num);
// 			if (Num == PC1)
// 			{
// 				theApp.m_CreateModelVision1 = FALSE;
// 				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelCreate"), _T("Vision1"), theApp.m_CreateModelVision1);
// 
// 				if (theApp.m_ChangeModelVision1)
// 				{
// 					sendMsg.Format(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
// 					theApp.m_VisionSocketManager[PC1].SocketSendto(PC1, sendMsg, MC_MODEL_CHANGE);
// 				}
// 			}
// 			else
// 			{
// 				theApp.m_CreateModelVision2 = FALSE;
// 				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelCreate"), _T("Vision2"), theApp.m_CreateModelVision2);
// 
// 				if (theApp.m_ChangeModelVision2)
// 				{
// 					sendMsg.Format(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
// 					theApp.m_VisionSocketManager[PC2].SocketSendto(PC2, sendMsg, MC_MODEL_CHANGE);
// 				}
// 			}
// 
// 			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Vision %d PC Model Create Success"), Num));
// 			break;
// 		case VS_MODEL_CHANGE:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL_CHANGE")), Num);
// 			if (Num == PC1)
// 			{
// 				theApp.m_ChangeModelVision1 = FALSE;
// 				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelChange"), _T("Vision1"), theApp.m_ChangeModelVision1);
// 			}
// 			else
// 			{
// 				theApp.m_ChangeModelVision2 = FALSE;
// 				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelChange"), _T("Vision2"), theApp.m_ChangeModelVision2);
// 			}
// 
// 			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Vision %d PC Model Change Success"), Num));
// 			break;
// 		case VS_GRAB_END:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_GRAB_END"), m_strContents), Num);
// 			ParsingGrabEnd(Num, m_strContents);
// 			break;
// 		case VS_INSPECTION_OK:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_INSPECTION_OK"), m_strContents), Num);
// 			break;
// 		case VS_INSPECTION_RESULT:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_INSPECTION_RESULT"), m_strContents), Num);
// 			ParsingInspectionResult(Num, m_strContents);
// 			break;
// 		case VS_AUTO_CAM_SET_START:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_AUTO_CAM_SET_START"), m_strContents), Num);
// 			break;
// 		case VS_Z_MOVE_REQUEST:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_Z_MOVE_REQUEST"), m_strContents), Num);
// 			AutoFocusAxis(Num, 1, m_strContents);
// 			break;
// 		case VS_FOCUS_MOVE_REQUEST:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_FOCUS_MOVE_REQUEST"), m_strContents), Num);
// 			AutoFocusAxis(Num, 2, m_strContents);
// 			break;
// 		case VS_Z_SAVE_POS_REQUEST:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_Z_SAVE_POS_REQUEST"), m_strContents), Num);
// 			break;
// 		case VS_FOCUS_SAVE_POS_REQUEST:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_FOCUS_SAVE_POS_REQUEST"), m_strContents), Num);
// 			AutoFocusSave(Num);
// 			break;
// 		case VS_VISION_TEST:
// 			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_VISION_TEST"), m_strContents), Num);
// 			VisionInspectionMethod(0, 0);
// 			VisionInspectionMethod(1, 1);
// 			break;
// 		}
// 
// 	}
// }

// 已禁用 - 现在只使用 Lighting 协议 (5601端口)
// void CVisionThread::VisionFirstCheckMethod(int Num)
// {
// 	BOOL bModelCreate, bModelChange;
// 	CString strCommand = CStringSupport::FormatString(_T("%d,%d"), MC_ARE_YOU_THERE, theApp.m_VisionSocketManager[Num].m_iVisionSocketCheckCount);
// 	SocketSendto(Num, strCommand, MC_ARE_YOU_THERE);
// 	Delay(200, TRUE);
// 
// 	strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_PCTIME, GetDateString4());
// 	SocketSendto(Num, strCommand, MC_PCTIME);
// 	Delay(200, TRUE);
// 
// 	if (Num == PC1)
// 	{
// 		bModelCreate = theApp.m_CreateModelVision1;
// 		bModelChange = theApp.m_ChangeModelVision1;
// 	}
// 	else
// 	{
// 		bModelCreate = theApp.m_CreateModelVision2;
// 		bModelChange = theApp.m_ChangeModelVision2;
// 	}
// 	
// 	if (bModelCreate)
// 	{
// 		strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_MODEL_CREATE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
// 		SocketSendto(Num, strCommand, MC_MODEL_CREATE);
// 		LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), Num, MC_PacketNameTable[MC_MODEL_CREATE], strCommand), Num);
// 		Delay(200, TRUE);
// 	}
// 	
// 	if (bModelChange)
// 	{
// 		strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
// 		SocketSendto(Num, strCommand, MC_MODEL_CHANGE);
// 		LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), Num, MC_PacketNameTable[MC_MODEL_CHANGE], strCommand), Num);
// 		Delay(200, TRUE);
// 	}
// 
// }

// 已禁用 - 现在只使用 Lighting 协议 (5601端口)
// void CVisionThread::VisionCheckMethod(int Num)
// {
// 	CString strCommand = CStringSupport::FormatString(_T("%d,%d"), MC_ARE_YOU_THERE, theApp.m_VisionSocketManager[Num].m_iVisionSocketCheckCount);
// 	SocketSendto(Num, strCommand, MC_ARE_YOU_THERE);
// }

void CVisionThread::VisionPanelCheck()
{
	PanelData pPanelData; 
	FpcIDData pFpcData;
	CString strPanel, strFpcID;

	theApp.m_PlcLog->LOG_INFO(_T("[VisionThread] VisionPanelCheck started"));

	for (int ii = 0; ii < PanelMaxCount; ii++)
	{
		theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_VisionFpcID1 + ii, &pFpcData);
		strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));
		
		theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
			_T("[VisionThread] Received eWordType_VisionFpcID%d signal - FPC ID: %s"), 
			ii + 1, strFpcID));

		theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_VisionPanel1 + ii, &pPanelData);
		strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));
		
		theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
			_T("[VisionThread] Received eWordType_VisionPanel%d signal - Panel: %s"), 
			ii + 1, strPanel));

		if (strFpcID.GetLength() > 0)
		{
			theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
				_T("[VisionThread] FPC ID found for panel %d, setting VisionPcReceiver bit"), ii + 1));
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionPcReceiver, 0, TRUE);
			break;
		}
	}
}

void CVisionThread::ParsingModelRequest(int Num, CString strContents)
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_MODEL, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
	SocketSendto(Num, sendMsg, MC_MODEL);
	LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), Num, MC_PacketNameTable[MC_MODEL], sendMsg), Num);
}

void CVisionThread::ParsingPcTimeRequest(int Num, CString strContents)
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_PCTIME, GetDateString4());
	SocketSendto(Num, sendMsg, MC_PCTIME);
	LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), Num, MC_PacketNameTable[MC_PCTIME], sendMsg), Num);
}

void CVisionThread::VisionInspectionMethod(int Num, int panelNum)
{
	if (theApp.m_CurrentIndexZone < 0)
	{
		VisionPLCResult(Num, panelNum, _T("PLC Vision IndexZone Error"), m_codeResponseError , _T("NG"));
		return;
	}

	PanelData pPanelData;
	FpcIDData pFpcData;
	CString sendMsg = _T(""), strPanel = _T(""), strProcessID = _T(""), strFpcID = _T("");
	int iCurIndex = 0, indexPanelNum = 0;

	if (theApp.m_PanelTestStart)
	{
		strPanel.Format(_T("TEST%d%s"), panelNum, GetDateString4());
		strFpcID.Format(_T("TEST%d%s"), panelNum, GetDateString4());
	}
	else
	{
		theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_VisionPanel1 + panelNum, &pPanelData);
		strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

		theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_VisionFpcID1 + panelNum, &pFpcData);
		strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));
	}

	if (strPanel.IsEmpty())
		strPanel = strFpcID;

	if (strFpcID.IsEmpty())
	{
		VisionPLCResult(Num, panelNum, CStringSupport::FormatString(_T("PLC Vision Panel #%d ID Error"), panelNum), m_codePlcSendReceiverError, _T("NG"));
		return;
	}
	theApp.IndexCheck();
	iCurIndex = (theApp.m_CurrentIndexZone + (MaxZone - CZone)) % 4;
	indexPanelNum = theApp.m_indexList[iCurIndex].m_indexNum + panelNum;
	strProcessID = theApp.GetProcessID(strPanel);

	LogWrite(CStringSupport::FormatString(_T("%s Panel %d [%s][%s] Vision Grab Start"), PG_IndexName[iCurIndex], Num, strPanel, strFpcID), Num);

	BOOL bFlag = VisionVecAdd(strPanel, strFpcID, panelNum, indexPanelNum, Num, iCurIndex);
	if (bFlag)
	{
		sendMsg.Format(_T("%d,%s,%d,%s,%s"), MC_INSPECTION_START, strPanel, indexPanelNum, strProcessID, strFpcID);
		SocketSendto(Num, sendMsg, MC_INSPECTION_START);
	}
}

void CVisionThread::ParsingGrabEnd(int Num, CString strContents)
{
	CString sendMsg, strPanelID, strFpcID;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	strPanelID = responseTokens[0];
	strFpcID = responseTokens[1];
	strPanelID.Trim();
	strFpcID.Trim();

	// 已禁用 Vision PC Socket 发送 - 现在只使用 Lighting 协议
	// sendMsg.Format(_T("%d,%s,%s"), MC_GRAB_END_RECEIVE, strPanelID, strFpcID);
	// SocketSendto(Num, sendMsg, MC_GRAB_END_RECEIVE);
	
	for (auto &InspResult : theApp.m_lastInspResultVec)
	{
		if (!InspResult.m_cellId.CompareNoCase(strPanelID))
		{
			if (InspResult.m_bInspStart == TRUE)
			{
				LogWrite(CStringSupport::FormatString(_T("Panel [%s] Vision Grab End"), strPanelID), Num);
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionGrabEnd1 + InspResult.m_iPanelNum, OffSet_0, TRUE);
				InspResult.m_bGrabEnd = TRUE;
				break;
			}
		}
	}
}

void CVisionThread::ParsingInspectionResult(int Num, CString strContents)
{
	theApp.m_bVisionDeleteFlag = FALSE;
	CString sendMsg, strPanelID, strFpcID;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	strPanelID = responseTokens[0];
	strFpcID = responseTokens[1];
	strPanelID.Trim();
	strFpcID.Trim();

	// 已禁用 Vision PC Socket 发送 - 现在只使用 Lighting 协议
	// sendMsg.Format(_T("%d,%s,%s"), MC_INSPECTION_RESULT_RECEIVE, strPanelID, strFpcID);
	// SocketSendto(Num, sendMsg, MC_INSPECTION_RESULT_RECEIVE);

	int iokng = 0;
	for (auto &InspResult : theApp.m_lastInspResultVec)
	{
		if (!InspResult.m_cellId.CompareNoCase(strPanelID) || !InspResult.m_FpcID.CompareNoCase(strFpcID))
		{
			if (InspResult.m_bGrabEnd == TRUE)
			{
				InspResult.m_iResultValue = responseTokens[2] == _T("0") ? m_codeOk : m_codeFail;
				iokng = InspResult.m_iResultValue;
				if (InspResult.m_iResultValue == m_codeFail)
					theApp.m_pRankTread->AddRankCodeList(InspResult.m_cellId, InspResult.m_FpcID, InspResult.m_iPanelNum, InspResult.m_iCurIndex, RankAOI);

				VisionPLCResult(InspResult.m_iPCNum,
					InspResult.m_iPanelNum,
					PLC_ResultValue[InspResult.m_iResultValue],
					InspResult.m_iResultValue,
					InspResult.m_cellId);

				InspResult.time_check.StopTimer();
				InspResult.m_bResult = TRUE;
			}
			else
			{
				LogWrite(CStringSupport::FormatString(_T("Panel [%s] Vision Grab Error"), strPanelID), Num);
			}
			break;
		}
	}
	theApp.m_bVisionDeleteFlag = TRUE;
}

// 已禁用 - 现在只使用 Lighting 协议 (5601端口)
// void CVisionThread::OnEvent(UINT uEvent, LPVOID lpvData)
// {
// 	if (theApp.m_bExitFlag == FALSE)
// 		return;
// 
// 	SockAddrIn addrin;
// 	GetSockName(addrin);
// 	int VisionNum = ntohs(addrin.GetPort()) == _ttoi(VISION_PC1_PORT_NUM) ? PC1 : PC2;
// 	
// 	switch (uEvent)
// 	{
// 	case EVT_CONDROP:
// 		LogWrite(CStringSupport::FormatString(_T("Vision Connect Drop %d Ch"), VisionNum), VisionNum);
// 		break;
// 	case EVT_CONSUCCESS:
// 		LogWrite(CStringSupport::FormatString(_T("Vision Connect Success %d Ch"), VisionNum), VisionNum);
// 		break;
// 	case EVT_ZEROLENGTH:
// 		LogWrite(CStringSupport::FormatString(_T("Vision EVT_ZEROLENGTH %d Ch"), VisionNum), VisionNum);
// 		break;
// 	case EVT_CONFAILURE:
// 		LogWrite(CStringSupport::FormatString(_T("Vision EVT_CONFAILURE %d Ch"), VisionNum), VisionNum);
// 		break;
// 	default:
// 		LogWrite(CStringSupport::FormatString(_T("Vision Unknown Socket event %d Ch"), VisionNum), VisionNum);
// 		break;
// 	}
// }

// 已禁用 - 现在只使用 Lighting 协议 (5601端口)，不再处理 Vision PC 协议
void CVisionThread::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	// Vision PC 协议已禁用，现在只使用 Lighting 协议
}

void CVisionThread::OnEvent(UINT uEvent, LPVOID lpvData)
{
	// Vision PC 协议已禁用，现在只使用 Lighting 协议
}

BOOL CVisionThread::getConectCheck()
{
	SockAddrIn addrin;
	GetSockName(addrin);
	LONG  uAddr = addrin.GetIPAddr();
	if (uAddr == 0)
		return FALSE;	//���Ӿ���
	else
		return TRUE;	//������
}

void CVisionThread::RemoveClient()
{
	ShutdownConnection((SOCKET)m_hComm);
}

bool CVisionThread::SocketServerOpen(CString strServerPort)
{
	m_bMelsecSimulaion = true;
	SetSmartAddressing(false);
	SetServerState(true);
	bool ret = CreateSocket(strServerPort, AF_INET, SOCK_STREAM, 0);
	if (ret) return WatchComm();
	else return false;
}

UINT CVisionThread::VisionThreadProc(LPVOID pParam)
{
	CVisionThread* pThis = reinterpret_cast<CVisionThread*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;

}

BOOL CVisionThread::CreateTask(){
	BOOL bRet = TRUE;
	m_pThreadVision = ::AfxBeginThread(VisionThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadVision)
		bRet = FALSE;
	m_pThreadVision->m_bAutoDelete = FALSE;
	m_pThreadVision->ResumeThread();
	return bRet;
}

void CVisionThread::CloseTask()
{
	if (m_pThreadVision != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadVision->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadVision->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadVision->m_hThread, 1L);
				theApp.m_VisionLog->LOG_INFO(_T("Terminate Vision Thread"));
			}
		}
		delete m_pThreadVision;
		m_pThreadVision = NULL;
	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}

// 已禁用 - 现在只使用 Lighting 协议 (5601端口)
void CVisionThread::SocketSendto(int Num, CString strContents, int iCommand)
{
	// Vision PC Socket 发送已禁用，现在只使用 Lighting 协议
}

void CVisionThread::LogWrite(CString strContents,int Num)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	g_DlgMainView->m_VisionListBox[Num].InsertString(0, CStringSupport::FormatString(_T("[%s] %s"), GetNowSystemTimeMilliseconds(), strContents));
	theApp.m_VisionLog->LOG_INFO(strContents);
}

void CVisionThread::VisionPLCResult(int Num, int iPanelNum, CString ResultMsg, int ResultCode, CString strPanelID)
{
	if (theApp.m_AOIPassMode){
		ResultCode = m_codeOk;
		//int Rand = 0;
		////srand(time(NULL));
		//Rand = rand() % 10;
		//if (Rand > 1)
		//	ResultCode = m_codeOk;
		//else
		//	ResultCode = m_codeFail;
	}
		

	theApp.m_PlcLog->LOG_INFO(CStringSupport::FormatString(
		_T("[VisionThread] SetPlcWordData eWordType_VisionResult%d = %d (%s)"), 
		iPanelNum + 1, ResultCode, PLC_ResultValue[ResultCode]));
	theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_VisionResult1 + iPanelNum, &ResultCode);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionGrabEnd1 + iPanelNum, OffSet_0, TRUE);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionEnd1 + iPanelNum, OffSet_0, TRUE);

	LogWrite(CStringSupport::FormatString(_T("Panel %d %s Vision Result %s"), Num, strPanelID, ResultMsg), Num);
}

BOOL CVisionThread::VisionVecAdd(CString strPanel, CString strFpcID, int iPanelNum, int iIndexNum, int iPCNo, int iCurIndex)
{
	BOOL flag = TRUE;
	InspResult panelData;
	panelData.Reset();

	panelData.m_bInspStart = TRUE;
	if (theApp.m_iTimer[VisionGrabTimer] == 0)
		panelData.time_check.SetCheckTime(8000);
	else
		panelData.time_check.SetCheckTime(theApp.m_iTimer[VisionGrabTimer] * 1000);

	panelData.time_check.StartTimer();
	panelData.m_iIndexPanelNum = iIndexNum;
	panelData.m_iPanelNum = iPanelNum;
	panelData.m_iPCNum = iPCNo;
	panelData.m_cellId = strPanel;
	panelData.m_FpcID = strFpcID;
	panelData.m_iCurIndex = iCurIndex;

	if (panelData.m_cellId.IsEmpty())
		panelData.m_cellId = panelData.m_FpcID;

	for (auto &InspResult : theApp.m_lastInspResultVec)
	{
		if (!InspResult.m_cellId.CompareNoCase(strPanel))
		{
			flag = FALSE;
			VisionPLCResult(iPCNo, iPanelNum, _T("PLC ID Error"), m_codePlcPanelError, strPanel);
			break;
		}
	}

	for (auto &InspResult : theApp.m_lastInspResultVec)
	{
		if (InspResult.m_bInspStart == FALSE && flag == TRUE)
		{
			InspResult = panelData;
			break;
		}
			
	}

	return flag;
}

void CVisionThread::AutoFocusAxis(int Num, int iCommand, CString strContents)
{
	if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AutoFocusReady1 + Num, OffSet_0))
	{
		CStringArray responseTokens;
		CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);
		AutoFocusData pAutoFocusData;
		pAutoFocusData.m_MoterValue = _ttof(responseTokens[0]) * 10000;

		theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_AutoFocusMoter1 + Num, &iCommand);
		theApp.m_pEqIf->m_pMNetH->SetAutoFocusData(eWordType_AutoFocusValue1 + Num, &pAutoFocusData);
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AutoFocusStart1 + Num, OffSet_0, TRUE);
	}
	else
	{
		LogWrite(CStringSupport::FormatString(_T("Auto Focus Ready Error!!!!!")), Num);
	}
}

void CVisionThread::AutoFocusSave(int Num)
{
	if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AutoFocusReady1 + Num, OffSet_0))
	{
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AutoFocusSave1 + Num, OffSet_0, TRUE);
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AutoFocusStart1 + Num, OffSet_0, TRUE);
	}
	else
	{
		LogWrite(CStringSupport::FormatString(_T("Auto Focus Ready Error!!!!!")), Num);
	}
}

void CVisionThread::ParshingVisionData(int Num, CString strContents)
{
	CString strVisionModelName;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	if (responseTokens.GetSize() < 3)
		return;

	strVisionModelName = responseTokens[0];
	theApp.m_strOpvImageWidth = responseTokens[1];
	theApp.m_strOpvImageHeight = responseTokens[2];

	LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] ModelName[%s], ImageWidth[%s], ImageHeight[%s]"), 
		Num, strVisionModelName, theApp.m_strOpvImageWidth, theApp.m_strOpvImageHeight), Num);
}

#endif