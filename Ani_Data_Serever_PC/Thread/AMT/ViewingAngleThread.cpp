
#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "DlgMainView.h"
#include "DlgMainLog.h"
#include "ViewingAngleThread.h"
#include "DFSInfo.h"

CViewingAngleThread::CViewingAngleThread()
{
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bFirstStatus = TRUE;
	m_lastContent.resize(4);
	m_lastCommand.resize(4);
	m_lastRequest.resize(4);
	theApp.m_bViewingAngleDeleteFlag = TRUE;
	theApp.m_lastViewingAngleResultVec.resize(8);
}

CViewingAngleThread::~CViewingAngleThread()
{
}

void CViewingAngleThread::ThreadRun()
{
	for (auto &InspResult : theApp.m_lastViewingAngleResultVec)
		InspResult.Reset();

	CString strMsg;

	while (::WaitForSingleObject(m_hQuit, 50) != WAIT_OBJECT_0)
	{
		theApp.m_ViewingAngleConectStatus[0] = theApp.m_ViewingAngleSocketManager[0].getConectCheck();
		theApp.m_ViewingAngleConectStatus[1] = theApp.m_ViewingAngleSocketManager[1].getConectCheck();
		theApp.m_ViewingAngleConectStatus[2] = theApp.m_ViewingAngleSocketManager[2].getConectCheck();
		theApp.m_ViewingAngleConectStatus[3] = theApp.m_ViewingAngleSocketManager[3].getConectCheck();

		//if (theApp.m_bAllPassMode)
		//	continue;

		if ((theApp.m_ViewingAngleConectStatus[0] && theApp.m_ViewingAngleConectStatus[1] && theApp.m_ViewingAngleConectStatus[2] && theApp.m_ViewingAngleConectStatus[3]) || theApp.m_AnglePassMode)
		{
			if (m_bFirstStatus)
			{
				m_bFirstStatus = FALSE;
				time_check.SetCheckTime(60000);
				time_check.StartTimer();
				for (int ii = 0; ii < PanelMaxCount; ii++)
				{
					ViewingAngleFirstCheckMethod(ii);
					theApp.m_ViewingAnglePCStatus[ii] = TRUE;
					m_bStartViewingAngle[ii] = FALSE;
				}

			}

			//TEST Model 이 (TRUE) Model 변경도 안보고 그냥 계속 진행 합니다.
			//TEST Model 이 (FALSE) Model 변경 및 생성 계속 check 
			if (theApp.m_AnglePassMode == FALSE)
			{
				if (theApp.m_PlcConectStatus == FALSE || theApp.m_ChangeModelViewingAngle1 == TRUE || theApp.m_ChangeModelViewingAngle2 == TRUE
					|| theApp.m_ChangeModelViewingAngle3 == TRUE || theApp.m_ChangeModelViewingAngle4 == TRUE)
					continue;
			}

			if (time_check.IsTimeOver())
			{
				time_check.StartTimer();
				for (int ii = 0; ii < PanelMaxCount; ii++)
				{
					ViewingAngleCheckMethod(ii);
					if (theApp.m_ViewingAngleSocketManager[ii].m_ViewingAngleCheckCount > 5)
					{
						//LogWrite(CStringSupport::FormatString(_T("Viewing Angle PC %d Client Drop"), ii), ii);
					}
					theApp.m_ViewingAngleSocketManager[ii].m_ViewingAngleCheckCount++;
				}
			}

			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_ViewingAnglePlcSend, 0))
				ViewingAnglePnaleCheck();
			else
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAnglePcReceiver, 0, FALSE);

			for (int ii = 0; ii < PanelMaxCount; ii++)
			{
				m_bStartFlag[ii] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_ViewingAngleStart1 + ii, OffSet_0);

				if (m_bStartFlag[ii] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAngleEnd1 + ii, OffSet_0, FALSE);
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_ViewingAngleResult1 + ii, &m_codeReset);
				}

				if (m_bStartViewingAngle[ii] == !m_bStartFlag[ii])
				{
					m_bStartViewingAngle[ii] = m_bStartFlag[ii];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Viewing Angle %d Start Flag [%s]"), ii + 1, m_bStartViewingAngle[ii] == FALSE ? _T("FALSE") : _T("TRUE"), ii));

					if (m_bStartFlag[ii] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAngleEnd1 + ii, OffSet_0, FALSE);
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_ViewingAngleResult1 + ii, &m_codeReset);
						Delay(100);
						ViewingAngleMethod(PanelNum1 + ii);
					}
				}
			}

			for (auto &InspResult : theApp.m_lastViewingAngleResultVec)
			{
				if (InspResult.m_LastCheck == TRUE)
				{
					if (InspResult.time_check.IsTimeOver())
					{
						ViewingAnglePLCResult(InspResult.m_iPanelNum, PLC_ResultValue[m_codeTimeOut], m_codeTimeOut, InspResult.m_cellId);
						InspResult.m_bResult = TRUE;

						theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("Viewing Angle [%s] Time out"), InspResult.m_cellId));
					}
				}
				else if (InspResult.m_bInspStart == TRUE)
				{
					if (InspResult.time_check.IsTimeOver())
					{
						CString sendMsg;
						sendMsg.Format(_T("%d,%s,%d"), MC_INSPECTION_JUDGE_REQUEST, InspResult.m_cellId, InspResult.m_iIndexPanelNum);
						SocketSendto(InspResult.m_iPCNum, sendMsg, MC_INSPECTION_JUDGE_REQUEST);
						LogWrite(CStringSupport::FormatString(_T("Viewing Angle Inspection Last Request Start")),InspResult.m_iPanelNum);
						InspResult.m_LastCheck = TRUE;
						if (theApp.m_iTimer[ViewingAngleLastGrabTimer] == 0)
							InspResult.time_check.SetCheckTime(1000);
						else
							InspResult.time_check.SetCheckTime(theApp.m_iTimer[ViewingAngleLastGrabTimer] * 1000);

						InspResult.time_check.StartTimer();
						break;
					}
				}

				if (InspResult.m_bResult == TRUE && theApp.m_bViewingAngleDeleteFlag == TRUE)
					InspResult.Reset();

			}

		}
		else
		{
			theApp.m_ViewingAnglePCStatus[PanelNum1] = FALSE;
			theApp.m_ViewingAnglePCStatus[PanelNum2] = FALSE;
			theApp.m_ViewingAnglePCStatus[PanelNum3] = FALSE;
			theApp.m_ViewingAnglePCStatus[PanelNum4] = FALSE;
			m_bFirstStatus = TRUE;
		}
	}
}

void CViewingAngleThread::ViewingAngleFirstCheckMethod(int iPanelNum)
{
	//처음 보내주는것이 IO (MC_ARE_YOU_THERE) , PCTime(MC_PCTIME), 모델명(MC_MODEL)
	BOOL bModelCreate, bModelChange;
	CString strCommand = CStringSupport::FormatString(_T("%d,%d"), MC_ARE_YOU_THERE, theApp.m_ViewingAngleSocketManager[iPanelNum].m_ViewingAngleCheckCount);
	SocketSendto(iPanelNum, strCommand, MC_ARE_YOU_THERE);
	Delay(200, TRUE);

	strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_PCTIME, GetDateString4());
	SocketSendto(iPanelNum, strCommand, MC_PCTIME);
	Delay(200, TRUE);

	switch (iPanelNum)
	{
	case PanelNum1:
		bModelCreate = theApp.m_CreateModelViewingAngle1;
		bModelChange = theApp.m_ChangeModelViewingAngle1; 
		break;
	case PanelNum2:
		bModelCreate = theApp.m_CreateModelViewingAngle2;
		bModelChange = theApp.m_ChangeModelViewingAngle2;
		break;
	case PanelNum3:
		bModelCreate = theApp.m_CreateModelViewingAngle3;
		bModelChange = theApp.m_ChangeModelViewingAngle3;
		break;
	case PanelNum4:
		bModelCreate = theApp.m_CreateModelViewingAngle4;
		bModelChange = theApp.m_ChangeModelViewingAngle4;
		break;
	}

	if (bModelCreate)
	{
		strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_MODEL_CREATE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
		SocketSendto(iPanelNum, strCommand, MC_MODEL_CREATE);
		LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), iPanelNum, MC_PacketNameTable[MC_MODEL_CREATE], strCommand), iPanelNum);
		Delay(200, TRUE);
	}
	
	if (bModelChange)
	{
		strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
		SocketSendto(iPanelNum, strCommand, MC_MODEL_CHANGE);
		LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), iPanelNum, MC_PacketNameTable[MC_MODEL_CHANGE], strCommand), iPanelNum);
		Delay(200, TRUE);
	}

}

void CViewingAngleThread::ViewingAngleCheckMethod(int iPcNum)
{
	CString strCommand = CStringSupport::FormatString(_T("%d,%d"), MC_ARE_YOU_THERE, theApp.m_ViewingAngleSocketManager[iPcNum].m_ViewingAngleCheckCount);
	SocketSendto(iPcNum, strCommand, MC_ARE_YOU_THERE);
}

void CViewingAngleThread::ViewingAnglePnaleCheck()
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel, strFpcID;


	for (int ii = 0; ii < PanelMaxCount; ii++)
	{
		theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_ViewingAnglePanel1 + ii, &pPanelData);
		strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

		theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_ViewingAngleFpcID1 + ii, &pFpcData);
		strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

		if (strFpcID.GetLength() > 0)
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAnglePcReceiver, 0, TRUE);
			break;
		}
			
	}
	

}

void CViewingAngleThread::ViewingAngleMethod(int iPanelNum)
{
	BOOL bFlag = TRUE;
	if (theApp.m_CurrentIndexZone < 0)
	{
		ViewingAnglePLCError(_T("PLC IndexZone Error"), m_codeResponseError, iPanelNum);
		return;
	}
	
	CString sendMsg, strPanel = _T(""), strProcessID = _T(""), strFpcID = _T("");
	PanelData pPanelData;
	FpcIDData pFpcData;
	int iIndexPanelNum = 0, iCurIndex = 0;	

	//if (theApp.m_AnglePassMode)
	//{
	//	theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_ViewingAngleResult1 + iPanelNum, &m_codeOk);
	//	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAngleEnd1, iPanelNum, TRUE);
	//	LogWrite(CStringSupport::FormatString(_T("Panel %d Viewing Angle Result %s"), iPanelNum + 1, _T("OK")), iPanelNum);
	//	return;
	//}

	if (theApp.m_PanelTestStart)
	{
		strPanel = _T("TEST1") + GetDateString4();
		strFpcID = _T("TEST1") + GetDateString4();
	}
	else
	{
		theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_ViewingAnglePanel1 + iPanelNum, &pPanelData);
		strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

		theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_ViewingAngleFpcID1 + iPanelNum, &pFpcData);
		strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));
	}

	if (strPanel.IsEmpty())
		strPanel = strFpcID;

	if (strFpcID.IsEmpty())
	{
		ViewingAnglePLCError(_T("Viewing Angle PLC FPC ID Error"), m_codePlcSendReceiverError, iPanelNum);
		return;
	}
	theApp.IndexCheck();
	iCurIndex = (theApp.m_CurrentIndexZone + (MaxZone - BZone)) % 4;
	iIndexPanelNum = theApp.m_indexList[iCurIndex].m_indexNum + iPanelNum;
	strProcessID = theApp.GetProcessID(strPanel);

	LogWrite(CStringSupport::FormatString(_T("%s Panel %d Viewing Angle [%s][%s] Grab Start"), PG_IndexName[iCurIndex], iPanelNum, strPanel, strFpcID), iPanelNum);
	
	bFlag = ViewingAngleVecAdd(strPanel, strFpcID, iPanelNum, iIndexPanelNum, iCurIndex);
	if (bFlag == TRUE)
	{
		sendMsg.Format(_T("%d,%s,%d,%s,%s"), MC_INSPECTION_START, strPanel, iIndexPanelNum, strProcessID, strFpcID);
		SocketSendto(iPanelNum, sendMsg, MC_INSPECTION_START);
	}

}

BOOL CViewingAngleThread::ViewingAngleVecAdd(CString strPanel, CString strFpcID, int iPanelNum, int iIndexNum, int iCurIndex)
{
	BOOL flag = TRUE;
	InspResult panelData;
	panelData.Reset();
	
	panelData.m_iPCNum = iPanelNum;
	panelData.m_bInspStart = TRUE;
	panelData.m_iPanelNum = iPanelNum;
	panelData.m_iIndexPanelNum = iIndexNum;
	panelData.m_cellId = strPanel;
	panelData.m_FpcID = strFpcID;
	panelData.m_iCurIndex = iCurIndex;

	if (panelData.m_cellId.IsEmpty())
		panelData.m_cellId = panelData.m_FpcID;

	for (auto &InspResult : theApp.m_lastViewingAngleResultVec)
	{
		if (!InspResult.m_cellId.CompareNoCase(strPanel))
		{
			flag = FALSE;
			ViewingAnglePLCResult(iPanelNum, _T("PLC ID Error"), m_codePlcPanelError, strPanel);
			break;
		}
	}

	if (theApp.m_iTimer[ViewingAngleGrabTimer] == 0)
		panelData.time_check.SetCheckTime(15000);
	else
		panelData.time_check.SetCheckTime(theApp.m_iTimer[ViewingAngleGrabTimer] * 1000);

	panelData.time_check.StartTimer();

	for (auto &InspResult : theApp.m_lastViewingAngleResultVec)
	{
		if (InspResult.m_bInspStart == FALSE && flag == TRUE)
		{
			InspResult = panelData;
			break;
		}
	}

	return flag;
}

void CViewingAngleThread::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	SockAddrIn addrin;
	GetSockName(addrin);
	int Num = ntohs(addrin.GetPort()) == _ttoi(VIEWING_ANGLE_PANEL1_PORT_NUM) ? PanelNum1 : ntohs(addrin.GetPort()) == _ttoi(VIEWING_ANGLE_PANEL2_PORT_NUM) ? 
		PanelNum2 : ntohs(addrin.GetPort()) == _ttoi(VIEWING_ANGLE_PANEL3_PORT_NUM) ? PanelNum3 : PanelNum4;

	//통신중 연결 되어 들어올경우에는 for 문으로 ETX 기준으로 파싱해서 전부 가져올수 있도록 수정
	CString strData, m_strHeader, m_strCommand, m_strContents, strParsing;
	int iFind, iFindSTX;
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount, strData.GetBuffer(dwCount + 1), dwCount + 1);
	strData.ReleaseBuffer(dwCount);

	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strData, _ETX, responseTokens);

	if (responseTokens.GetSize() == 1)
	{
		LogWrite(_T("ETX No Message!!!"), Num);
		return;
	}

	for (int ii = 0; ii < responseTokens.GetSize() - 1; ii++)
	{
		strParsing = responseTokens[ii];

		m_strHeader.Format(_T("%x"), strParsing.GetAt(0));

		UINT iHeader = (UINT)_ttoi(m_strHeader);

		if (iHeader != _STX)
		{
			LogWrite(_T("STX No Message!!!"), Num);
			return;
		}

		iFind = strParsing.Find(',');
		m_strCommand = strParsing.Left(iFind);

		iFindSTX = strParsing.Find((char)_STX);
		m_strCommand = m_strCommand.Mid(iFindSTX + 1, m_strCommand.GetLength());

		int iCommand = _ttoi(m_strCommand);

		m_strContents = strParsing.Mid(iFind + 1, strParsing.GetLength());

		m_lastCommand[Num] = VS_PacketNameTable[iCommand];
		m_lastRequest[Num] = m_strContents;

		switch (Num)
		{
		case PanelNum1: theApp.m_pViewingAngleSendReceiver1Log->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), m_lastCommand[Num], strData)); break;
		case PanelNum2: theApp.m_pViewingAngleSendReceiver2Log->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), m_lastCommand[Num], strData)); break;
		case PanelNum3: theApp.m_pViewingAngleSendReceiver3Log->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), m_lastCommand[Num], strData)); break;
		case PanelNum4: theApp.m_pViewingAngleSendReceiver4Log->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), m_lastCommand[Num], strData)); break;
		}

		CString sendMsg;
		switch (iCommand)
		{
		case VS_ARE_YOU_THERE:
			theApp.m_ViewingAngleSocketManager[Num].m_ViewingAngleCheckCount = 0;
			break;
		case VS_PCTIME_REQUEST:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_PCTIME_REQUEST")), Num);
			ViewingAnglePcTimeRequest(Num);
			break;
		case VS_STATE:
			theApp.m_ViewingAnglePCStatus[Num] = m_strContents == _T("0") ? FALSE : TRUE;
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_STATE"), theApp.m_ViewingAnglePCStatus[Num] == TRUE ? _T("Start") : _T("Stop")), Num);
			break;
		case VS_MODEL_REQUEST:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL_REQUEST")), Num);
			ViewingAngleModelRequest(Num);
			break;
		case VS_MODEL_CREATE:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL_CREATE")), Num);
			if (Num == PanelNum1)
			{
				theApp.m_CreateModelViewingAngle1 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelCreate"), _T("ViewingAngle1"), theApp.m_CreateModelViewingAngle1);

				if (theApp.m_ChangeModelViewingAngle1)
				{
					sendMsg.Format(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
					theApp.m_ViewingAngleSocketManager[PanelNum1].SocketSendto(PanelNum1, sendMsg, MC_MODEL_CHANGE);
				}
			}
			else if (Num == PanelNum2)
			{
				theApp.m_CreateModelViewingAngle2 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelCreate"), _T("ViewingAngle2"), theApp.m_CreateModelViewingAngle2);

				if (theApp.m_ChangeModelViewingAngle2)
				{
					sendMsg.Format(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
					theApp.m_ViewingAngleSocketManager[PanelNum2].SocketSendto(PanelNum2, sendMsg, MC_MODEL_CHANGE);
				}
			}
			else if (Num == PanelNum3)
			{
				theApp.m_CreateModelViewingAngle3 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelCreate"), _T("ViewingAngle3"), theApp.m_CreateModelViewingAngle3);

				if (theApp.m_ChangeModelViewingAngle3)
				{
					sendMsg.Format(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
					theApp.m_ViewingAngleSocketManager[PanelNum3].SocketSendto(PanelNum3, sendMsg, MC_MODEL_CHANGE);
				}
			}
			else if (Num == PanelNum4)
			{
				theApp.m_CreateModelViewingAngle4 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelCreate"), _T("ViewingAngle4"), theApp.m_CreateModelViewingAngle4);

				if (theApp.m_ChangeModelViewingAngle4)
				{
					sendMsg.Format(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
					theApp.m_ViewingAngleSocketManager[PanelNum4].SocketSendto(PanelNum4, sendMsg, MC_MODEL_CHANGE);
				}
			}
			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Viewing Angle %d PC Model Create Success"), Num));
			break;
		case VS_MODEL_CHANGE:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL_CHANGE")), Num);
			if (Num == PanelNum1)
			{
				theApp.m_ChangeModelViewingAngle1 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelChange"), _T("ViewingAngle1"), theApp.m_ChangeModelViewingAngle1);
			}
			else if (Num == PanelNum2)
			{
				theApp.m_ChangeModelViewingAngle2 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelChange"), _T("ViewingAngle2"), theApp.m_ChangeModelViewingAngle2);
			}
			else if (Num == PanelNum3)
			{
				theApp.m_ChangeModelViewingAngle3 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelChange"), _T("ViewingAngle3"), theApp.m_ChangeModelViewingAngle3);
			}
			else if (Num == PanelNum4)
			{
				theApp.m_ChangeModelViewingAngle4 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelChange"), _T("ViewingAngle4"), theApp.m_ChangeModelViewingAngle4);
			}

			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Viewing Angle %d PC Model Change Success"), Num));
			break;
		case VS_INSPECTION_OK:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_INSPECTION_OK"), m_strContents), Num);
			break;
		case VS_INSPECTION_RESULT:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_INSPECTION_RESULT"), m_strContents), Num);
			ViewingAngleInspectionResult(Num, m_strContents);
			break;
		case VS_VIEWING_TEST:
			ViewingAngleMethod(PanelNum1);
			ViewingAngleMethod(PanelNum2);
			ViewingAngleMethod(PanelNum3);
			ViewingAngleMethod(PanelNum4);
			break;
		}
	}

}

void CViewingAngleThread::ViewingAngleModelRequest(int Num)
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_MODEL, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
	SocketSendto(Num, sendMsg, MC_MODEL);
	LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), Num, MC_PacketNameTable[MC_MODEL], sendMsg), Num);
}

void CViewingAngleThread::ViewingAnglePcTimeRequest(int Num)
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_PCTIME, GetDateString4());
	SocketSendto(Num, sendMsg, MC_PCTIME);
	LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), Num, MC_PacketNameTable[MC_PCTIME], sendMsg), Num);
}

void CViewingAngleThread::ViewingAngleInspectionResult(int Num, CString strContents)
{
	theApp.m_bViewingAngleDeleteFlag = FALSE;
	CString sendMsg ,strPanel;
	CStringArray responseTokens;
	CString strAOTUpperPath, strAOTLowerPath, strAOIPath, strViewingPath;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	strPanel = responseTokens[0];
	strPanel.Trim();

	sendMsg.Format(_T("%d,%s"), MC_INSPECTION_RESULT_RECEIVE, strPanel);
	SocketSendto(Num, sendMsg, MC_INSPECTION_RESULT_RECEIVE);

	for (auto &InspResult : theApp.m_lastViewingAngleResultVec)
	{
		if (!InspResult.m_cellId.CompareNoCase(strPanel))
		{
			if (InspResult.m_bResult == FALSE)
			{
				InspResult.time_check.StopTimer();
				InspResult.m_iResultValue = responseTokens[1] == _T("0") ? m_codeOk : m_codeFail;

				if (InspResult.m_iResultValue == m_codeFail)
					theApp.m_pRankTread->AddRankCodeList(InspResult.m_cellId, InspResult.m_FpcID, InspResult.m_iPanelNum, InspResult.m_iCurIndex, RankViewing);

				ViewingAnglePLCResult(InspResult.m_iPanelNum,
					PLC_ResultValue[InspResult.m_iResultValue],
					InspResult.m_iResultValue,
					InspResult.m_cellId);

				InspResult.m_bResult = TRUE;
				break;
			}
		}
	}
	theApp.m_bViewingAngleDeleteFlag = TRUE;
}

void CViewingAngleThread::OnEvent(UINT uEvent, LPVOID lpvData)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	SockAddrIn addrin;
	GetSockName(addrin);
	int iPanelNum = ntohs(addrin.GetPort()) == _ttoi(VIEWING_ANGLE_PANEL1_PORT_NUM) ? PanelNum1 : ntohs(addrin.GetPort()) == _ttoi(VIEWING_ANGLE_PANEL2_PORT_NUM) ?
	PanelNum2 : ntohs(addrin.GetPort()) == _ttoi(VIEWING_ANGLE_PANEL3_PORT_NUM) ? PanelNum3 : PanelNum4;

	switch (uEvent)
	{
	case EVT_CONDROP:
		LogWrite(CStringSupport::FormatString(_T("Viewing Angle Drop %d Ch"), iPanelNum), iPanelNum);
		break;
	case EVT_CONSUCCESS:
		LogWrite(CStringSupport::FormatString(_T("Viewing Angle Connect Success %d Ch"), iPanelNum), iPanelNum);
		break;
	case EVT_ZEROLENGTH:
		LogWrite(CStringSupport::FormatString(_T("Viewing Angle EVT_ZEROLENGTH %d Ch"), iPanelNum), iPanelNum);
		break;
	case EVT_CONFAILURE:
		LogWrite(CStringSupport::FormatString(_T("Viewing Angle EVT_CONFAILURE %d Ch"), iPanelNum), iPanelNum);
		break;
	default:
		LogWrite(CStringSupport::FormatString(_T("Unknown Socket event %d Ch"), iPanelNum), iPanelNum);
		break;
	}
}

BOOL CViewingAngleThread::getConectCheck()
{
	SockAddrIn addrin;
	GetSockName(addrin);
	LONG  uAddr = addrin.GetIPAddr();
	if (uAddr == 0)
		return FALSE;	//접속안함
	else
		return TRUE;	//접속함
}

bool CViewingAngleThread::SocketServerOpen(CString strServerPort)
{
	m_bMelsecSimulaion = true;
	SetSmartAddressing(false);
	SetServerState(true);
	bool ret = CreateSocket(strServerPort, AF_INET, SOCK_STREAM, 0);
	if (ret) return WatchComm();
	else return false;
}

void CViewingAngleThread::RemoveClient()
{
	ShutdownConnection((SOCKET)m_hComm);
}

UINT CViewingAngleThread::ViewingAngleThreadProc(LPVOID pParam)
{
	CViewingAngleThread* pThis = reinterpret_cast<CViewingAngleThread*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;

}

BOOL CViewingAngleThread::CreateTask(){
	BOOL bRet = TRUE;
	m_pThreadViewingAngle = ::AfxBeginThread(ViewingAngleThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadViewingAngle)
		bRet = FALSE;
	m_pThreadViewingAngle->m_bAutoDelete = FALSE;	/// 쓰레드 종료시 WaitForSingleObject 적용위해...
	m_pThreadViewingAngle->ResumeThread();
	return TRUE;
}

void CViewingAngleThread::CloseTask()
{
	if (m_pThreadViewingAngle != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadViewingAngle->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadViewingAngle->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadViewingAngle->m_hThread, 1L);
				theApp.m_ViewingAngleLog->LOG_INFO(_T("Terminate ViewingAngle Thread"));
			}
		}
		delete m_pThreadViewingAngle;
		m_pThreadViewingAngle = NULL;

	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}

void CViewingAngleThread::SocketSendto(int Num ,CString strContents, int iCommand)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	m_csSocketSend.Lock();
	CString strCommand = CStringSupport::FormatString(_T("%c%s,%c"), _STX, strContents, _ETX);
	char *lpCommand = StringToChar(strCommand);
	theApp.m_ViewingAngleSocketManager[Num].WriteComm((BYTE*)lpCommand, strlen(lpCommand), 100L);
	delete lpCommand;

	m_lastContent[Num] = strContents;

	switch (Num)
	{
	case PanelNum1: theApp.m_pViewingAngleSendReceiver1Log->LOG_INFO(CStringSupport::FormatString(_T("[MC -> VS] [Command : %s] ->%s"), MC_PacketNameTable[iCommand], strContents)); break;
	case PanelNum2: theApp.m_pViewingAngleSendReceiver2Log->LOG_INFO(CStringSupport::FormatString(_T("[MC -> VS] [Command : %s] ->%s"), MC_PacketNameTable[iCommand], strContents)); break;
	case PanelNum3: theApp.m_pViewingAngleSendReceiver3Log->LOG_INFO(CStringSupport::FormatString(_T("[MC -> VS] [Command : %s] ->%s"), MC_PacketNameTable[iCommand], strContents)); break;
	case PanelNum4: theApp.m_pViewingAngleSendReceiver4Log->LOG_INFO(CStringSupport::FormatString(_T("[MC -> VS] [Command : %s] ->%s"), MC_PacketNameTable[iCommand], strContents)); break;
	}

	m_csSocketSend.Unlock();
}

void CViewingAngleThread::LogWrite(CString strContents, int iPanelNum)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	switch (iPanelNum)
	{
	case PanelNum1:
	case PanelNum2: g_DlgMainView->m_ViewingAngleListBox[0].InsertString(0, CStringSupport::FormatString(_T("[%s] %s"), GetNowSystemTimeMilliseconds(), strContents)); break;
	case PanelNum3:
	case PanelNum4: g_DlgMainView->m_ViewingAngleListBox[1].InsertString(0, CStringSupport::FormatString(_T("[%s] %s"), GetNowSystemTimeMilliseconds(), strContents)); break;
	}
	
	theApp.m_ViewingAngleLog->LOG_INFO(strContents);
}

void CViewingAngleThread::ViewingAnglePLCError(CString ErrorMsg, int ErrorCode, int iPanelNum)
{
	theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_ViewingAngleResult1 + iPanelNum, &ErrorCode);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAngleEnd1, iPanelNum, TRUE);

	LogWrite(ErrorMsg, iPanelNum);
}

void CViewingAngleThread::ViewingAnglePLCResult(int iPanelNum, CString ResultMsg, int ResultCode, CString strPanelID)
{
	if (theApp.m_AnglePassMode)
		ResultCode = m_codeOk;

	theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_ViewingAngleResult1 + iPanelNum, &ResultCode);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAngleEnd1, iPanelNum, TRUE);
	LogWrite(CStringSupport::FormatString(_T("Panel %s Viewing Angle [CH_%d] Result %s"), strPanelID, iPanelNum + 1, ResultMsg), iPanelNum);
}

#endif