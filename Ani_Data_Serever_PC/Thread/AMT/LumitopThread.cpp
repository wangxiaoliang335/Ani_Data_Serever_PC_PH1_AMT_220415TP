
#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "DlgMainView.h"
#include "DlgMainLog.h"
#include "LumitopThread.h"
#include "DFSInfo.h"


CLumitopThread::CLumitopThread()
{
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bFirstStatus = TRUE;
	m_lastContent.resize(2);
	m_lastCommand.resize(2);
	m_lastRequest.resize(2);
	theApp.m_bLumitopDeleteFlag = TRUE;
	theApp.m_lastLumitopResultVec.resize(8);
}

CLumitopThread::~CLumitopThread()
{
}

void CLumitopThread::ThreadRun()
{
	for (auto &InspResult : theApp.m_lastLumitopResultVec)
		InspResult.Reset();

	while (::WaitForSingleObject(m_hQuit, 50) != WAIT_OBJECT_0)
	{
		theApp.m_LumitopConectStatus[0] = theApp.m_LumitopSocketManager[0].getConectCheck();
		theApp.m_LumitopConectStatus[1] = theApp.m_LumitopSocketManager[1].getConectCheck();

		//if (theApp.m_bAllPassMode)
		//	continue;

		if ((theApp.m_LumitopConectStatus[0] && theApp.m_LumitopConectStatus[1]) || theApp.m_LumitopPassMode)
		{
			if (m_bFirstStatus)
			{
				m_bFirstStatus = FALSE;
				time_check.SetCheckTime(60000);
				time_check.StartTimer();
				for (int ii = 0; ii < PCMaxCount; ii++)
				{
					LumitopFirstCheckMethod(ii);
					theApp.m_LumitopPCStatus[ii] = TRUE;
				}

				for (int jj = 0; jj < PanelMaxCount; jj++)
					m_bStartLumitop[jj] = FALSE;
			}

			//TEST Model 이 (TRUE) Model 변경도 안보고 그냥 계속 진행 합니다.
			//TEST Model 이 (FALSE) Model 변경 및 생성 계속 check 
			if (theApp.m_LumitopPassMode == FALSE)
			{
				if (theApp.m_PlcConectStatus == FALSE || theApp.m_ChangeModelLumitop1 == TRUE || theApp.m_ChangeModelLumitop2 == TRUE)
					continue;
			}

			if (time_check.IsTimeOver())
			{
				time_check.StartTimer();
				for (int ii = 0; ii < PCMaxCount; ii++)
				{
					LumitopCheckMethod(ii);
					if (theApp.m_LumitopSocketManager[ii].m_iLumitopSocketCheckCount > 5)
					{
						LogWrite(CStringSupport::FormatString(_T("Lumitop PC %d Client Drop"), ii), ii);
					}
					theApp.m_LumitopSocketManager[ii].m_iLumitopSocketCheckCount++;
				}
			}

			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_PreGammaPlcSend, 0))
				LumitopPanelCheck();
			else
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopPcReceiver, 0, FALSE);

			for (int ii = 0; ii < PanelMaxCount; ii++)
			{
				m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_PreGammaStart1, OffSet_0 + ii);

				if (m_bStartFlag == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_PreGammaResult1 + ii, &m_codeReset);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopGrabEnd1 + ii, OffSet_0, FALSE);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopEnd1 + ii, OffSet_0, FALSE);
				}

				if (m_bStartLumitop[ii] == !m_bStartFlag)
				{
					m_bStartLumitop[ii] = m_bStartFlag;
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Lumitop Panel %d Start Flag [%s]"), ii + 1, m_bStartLumitop[ii] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bStartFlag == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_PreGammaResult1 + ii, &m_codeReset);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopGrabEnd1 + ii, OffSet_0, FALSE);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopEnd1 + ii, OffSet_0, FALSE);
						//Delay(100);
						m_iPcNum = ii <= PanelNum2 ? PC1 : PC2;
						LumitopInspectionMethod(m_iPcNum, PanelNum1 + ii);
					}
				}
			}

			for (auto &InspResult : theApp.m_lastLumitopResultVec)
			{
				if (InspResult.m_LastCheck == TRUE)
				{
					if (InspResult.time_check.IsTimeOver())
					{
						LumitopPLCResult(InspResult.m_iPCNum,
							InspResult.m_iPanelNum,
							PLC_ResultValue[m_codeTimeOut],
							m_codeTimeOut,
							InspResult.m_cellId,
							InspResult.m_iIndexPanelNum);

						InspResult.m_bResult = TRUE;
						theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[PC : %d] Lumitop [%s] Time out"), InspResult.m_iPCNum, InspResult.m_cellId));
					}
				}
				else if (InspResult.m_bInspStart == TRUE)
				{
					if (InspResult.time_check.IsTimeOver())
					{
						CString sendMsg;
						sendMsg.Format(_T("%d,%s"), MC_INSPECTION_END, InspResult.m_cellId);
						SocketSendto(InspResult.m_iPCNum, sendMsg, MC_INSPECTION_END);
						LogWrite(CStringSupport::FormatString(_T("Lumitop %d Inspection Last Request Start")), InspResult.m_iPCNum);
						InspResult.m_LastCheck = TRUE;
						if (theApp.m_iTimer[LumitopLastGrabTimer] == 0)
							InspResult.time_check.SetCheckTime(1000);
						else
							InspResult.time_check.SetCheckTime(theApp.m_iTimer[LumitopLastGrabTimer] * 1000);

						InspResult.time_check.StartTimer();
					}
				}

				if (InspResult.m_bResult == TRUE && theApp.m_bLumitopDeleteFlag == TRUE)
					InspResult.Reset();

			}

		}
		else
		{
			theApp.m_LumitopPCStatus[0] = FALSE;
			theApp.m_LumitopPCStatus[1] = FALSE;
			m_bFirstStatus = TRUE;
		}
	}
}


void CLumitopThread::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	SockAddrIn addrin;
	GetSockName(addrin);
	int Num = ntohs(addrin.GetPort()) == _ttoi(LUMITOP_PC1_PORT_NUM) ? PC1 : PC2;

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

		if (Num == PC1)
			theApp.m_pLumitopSendReceiver1Log->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), m_lastCommand[Num], strData));
		else
			theApp.m_pLumitopSendReceiver2Log->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), m_lastCommand[Num], strData));

		CString sendMsg;
		switch (iCommand)
		{
		case VS_ARE_YOU_THERE:
			theApp.m_LumitopSocketManager[Num].m_iLumitopSocketCheckCount = 0;
			break;
		case VS_PCTIME_REQUEST:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_PCTIME_REQUEST")), Num);
			ParsingPcTimeRequest(Num, m_strContents);
			break;
		case VS_STATE:
			theApp.m_LumitopPCStatus[Num] = m_strContents == _T("0") ? FALSE : TRUE;
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_STATE"), theApp.m_LumitopPCStatus[Num] == TRUE ? _T("Start") : _T("Stop")), Num);
			break;
		case VS_MODEL:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL")), Num);
			ParshingLumitopData(Num, m_strContents);
			break;
		case VS_MODEL_REQUEST:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL_REQUEST")), Num);
			ParsingModelRequest(Num, m_strContents);
			break;
		case VS_MODEL_CREATE:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL_CREATE")), Num);
			if (Num == PC1)
			{
				theApp.m_CreateModelLumitop1 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelCreate"), _T("Lumitop1"), theApp.m_CreateModelLumitop1);

				if (theApp.m_ChangeModelLumitop1)
				{
					sendMsg.Format(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
					theApp.m_LumitopSocketManager[PC1].SocketSendto(PC1, sendMsg, MC_MODEL_CHANGE);
				}
			}
			else
			{
				theApp.m_CreateModelLumitop2 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelCreate"), _T("Lumitop2"), theApp.m_CreateModelLumitop2);

				if (theApp.m_ChangeModelLumitop2)
				{
					sendMsg.Format(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
					theApp.m_LumitopSocketManager[PC2].SocketSendto(PC2, sendMsg, MC_MODEL_CHANGE);
				}
			}

			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Lumitop %d PC Model Create Success"), Num));
			break;
		case VS_MODEL_CHANGE:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s"), Num, _T("RCV : VS_MODEL_CHANGE")), Num);
			if (Num == PC1)
			{
				theApp.m_ChangeModelLumitop1 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelChange"), _T("Lumitop1"), theApp.m_ChangeModelLumitop1);
			}
			else
			{
				theApp.m_ChangeModelLumitop2 = FALSE;
				theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelChange"), _T("Lumitop2"), theApp.m_ChangeModelLumitop2);
			}

			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Lumitop %d PC Model Change Success"), Num));
			break;
		case VS_GRAB_END:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_GRAB_END"), m_strContents), Num);
			ParsingGrabEnd(Num, m_strContents);
			break;
		case VS_INSPECTION_OK:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_INSPECTION_OK"), m_strContents), Num);
			break;
		case VS_INSPECTION_RESULT:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_INSPECTION_RESULT"), m_strContents), Num);
			ParsingInspectionResult(Num, m_strContents);
			break;
		case VS_LUMITOP_TEST:
			LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] %s->%s"), Num, _T("RCV : VS_LUMITOP_TEST"), m_strContents), Num);
			LumitopInspectionMethod(0, 0);
			LumitopInspectionMethod(1, 1);
			break;
		}

	}
}

void CLumitopThread::LumitopFirstCheckMethod(int Num)
{
	BOOL bModelCreate, bModelChange;
	//처음 보내주는것이 IO (MC_ARE_YOU_THERE) , PCTime(MC_PCTIME), 모델명(MC_MODEL)
	CString strCommand = CStringSupport::FormatString(_T("%d,%d"), MC_ARE_YOU_THERE, theApp.m_LumitopSocketManager[Num].m_iLumitopSocketCheckCount);
	SocketSendto(Num, strCommand, MC_ARE_YOU_THERE);
	Delay(200, TRUE);

	strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_PCTIME, GetDateString4());
	SocketSendto(Num, strCommand, MC_PCTIME);
	Delay(200, TRUE);

	if (Num == PC1)
	{
		bModelCreate = theApp.m_CreateModelLumitop1;
		bModelChange = theApp.m_ChangeModelLumitop1;
	}
	else
	{
		bModelCreate = theApp.m_CreateModelLumitop2;
		bModelChange = theApp.m_ChangeModelLumitop2;
	}

	if (bModelCreate)
	{
		strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_MODEL_CREATE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
		SocketSendto(Num, strCommand, MC_MODEL_CREATE);
		LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), Num, MC_PacketNameTable[MC_MODEL_CREATE], strCommand), Num);
		Delay(200, TRUE);
	}

	if (bModelChange)
	{
		strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
		SocketSendto(Num, strCommand, MC_MODEL_CHANGE);
		LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), Num, MC_PacketNameTable[MC_MODEL_CHANGE], strCommand), Num);
		Delay(200, TRUE);
	}

}

void CLumitopThread::LumitopCheckMethod(int Num)
{
	CString strCommand = CStringSupport::FormatString(_T("%d,%d"), MC_ARE_YOU_THERE, theApp.m_LumitopSocketManager[Num].m_iLumitopSocketCheckCount);
	SocketSendto(Num, strCommand, MC_ARE_YOU_THERE);
}

void CLumitopThread::LumitopPanelCheck()
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel, strFpcID;

	for (int ii = 0; ii < PanelMaxCount; ii++)
	{
		theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_PreGammaFpcID1 + ii, &pFpcData);
		strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

		theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_PreGammaPanel1 + ii, &pPanelData);
		strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

		if (strFpcID.GetLength() > 0)
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopPcReceiver, 0, TRUE);
			break;
		}
	}
}

void CLumitopThread::ParsingModelRequest(int Num, CString strContents)
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_MODEL, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
	SocketSendto(Num, sendMsg, MC_MODEL);
	LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), Num, MC_PacketNameTable[MC_MODEL], sendMsg), Num);
}

void CLumitopThread::ParsingPcTimeRequest(int Num, CString strContents)
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_PCTIME, GetDateString4());
	SocketSendto(Num, sendMsg, MC_PCTIME);
	LogWrite(CStringSupport::FormatString(_T("[MC -> VS %d] %s->%s"), Num, MC_PacketNameTable[MC_PCTIME], sendMsg), Num);
}

void CLumitopThread::LumitopInspectionMethod(int Num, int panelNum)
{
	if (theApp.m_CurrentIndexZone < 0)
	{
		LumitopPLCResult(Num, panelNum, _T("PLC Lumitop IndexZone Error"), m_codeResponseError, _T("NG"), 0);
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
		theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_PreGammaPanel1 + panelNum, &pPanelData);
		strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

		theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_PreGammaFpcID1 + panelNum, &pFpcData);
		strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));
	}

	if (strFpcID.IsEmpty())
	{
		LumitopPLCResult(Num, panelNum, CStringSupport::FormatString(_T("PLC Lumitop Panel #%d ID Error"), panelNum), m_codePlcSendReceiverError, _T("NG"), 0);
		return;
	}

	if (strPanel.IsEmpty())
		strPanel = strFpcID;
	theApp.IndexCheck();
	iCurIndex = (theApp.m_CurrentIndexZone + (MaxZone - DZone)) % 4;
	indexPanelNum = theApp.m_indexList[iCurIndex].m_indexNum + panelNum;
	strProcessID = theApp.GetProcessID(strPanel);

	LogWrite(CStringSupport::FormatString(_T("%s Panel %d [%s][%s] Lumitop Grab Start"), PG_IndexName[iCurIndex], Num, strPanel, strFpcID), Num);

	BOOL bFlag = LumitopVecAdd(strPanel, strFpcID, panelNum, indexPanelNum, Num, iCurIndex);
	if (bFlag)
	{
		sendMsg.Format(_T("%d,%s,%d,%s,%s"), MC_INSPECTION_START, strPanel, indexPanelNum, strProcessID, strFpcID);
		SocketSendto(Num, sendMsg, MC_INSPECTION_START);
	}
}

void CLumitopThread::ParsingGrabEnd(int Num, CString strContents)
{
	CString sendMsg, strPanelID;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	strPanelID = responseTokens[0];
	strPanelID.Trim();

	sendMsg.Format(_T("%d,%s"), MC_GRAB_END_RECEIVE, strPanelID);
	SocketSendto(Num, sendMsg, MC_GRAB_END_RECEIVE);

	for (auto &InspResult : theApp.m_lastLumitopResultVec)
	{
		if (!InspResult.m_cellId.CompareNoCase(strPanelID))
		{
			if (InspResult.m_bInspStart == TRUE)
			{
				LogWrite(CStringSupport::FormatString(_T("Panel [%s] Lumitop Grab End"), strPanelID), Num);
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopGrabEnd1 + InspResult.m_iPanelNum, OffSet_0, TRUE);
				InspResult.m_bGrabEnd = TRUE;
				break;
			}
		}
	}

}

void CLumitopThread::ParsingInspectionResult(int Num, CString strContents)
{
	theApp.m_bLumitopDeleteFlag = FALSE;
	CString sendMsg, strPanelID, strFpcID;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	strPanelID = responseTokens[0];
	strPanelID.Trim();
	strFpcID = responseTokens[1];
	strFpcID.Trim();

	sendMsg.Format(_T("%d,%s,%s"), MC_INSPECTION_RESULT_RECEIVE, strPanelID, strFpcID);
	SocketSendto(Num, sendMsg, MC_INSPECTION_RESULT_RECEIVE);

	int iokng = 0;
	for (auto &InspResult : theApp.m_lastLumitopResultVec)
	{
		if (!InspResult.m_cellId.CompareNoCase(strPanelID) || !InspResult.m_FpcID.CompareNoCase(strFpcID))
		{
			InspResult.m_iResultValue = responseTokens[2] == _T("0") ? m_codeOk : m_codeFail;
			iokng = InspResult.m_iResultValue;
			//if (InspResult.m_iResultValue == m_codeFail)
			//	theApp.m_pRankTread->AddRankCodeList(InspResult.m_cellId, InspResult.m_FpcID, InspResult.m_iPanelNum, InspResult.m_iCurIndex, RankAOI);

			LumitopPLCResult(InspResult.m_iPCNum,
				InspResult.m_iPanelNum,
				PLC_ResultValue[InspResult.m_iResultValue],
				InspResult.m_iResultValue,
				InspResult.m_cellId,
				InspResult.m_iIndexPanelNum);
			

			InspResult.time_check.StopTimer();
			InspResult.m_bResult = TRUE;
			break;
		}
	}
	theApp.m_bLumitopDeleteFlag = TRUE;
		
}

void CLumitopThread::OnEvent(UINT uEvent, LPVOID lpvData)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	SockAddrIn addrin;
	GetSockName(addrin);
	int LumitopNum = ntohs(addrin.GetPort()) == _ttoi(LUMITOP_PC1_PORT_NUM) ? PC1 : PC2;

	switch (uEvent)
	{
	case EVT_CONDROP:
		LogWrite(CStringSupport::FormatString(_T("Lumitop Connect Drop %d Ch"), LumitopNum), LumitopNum);
		break;
	case EVT_CONSUCCESS:
		LogWrite(CStringSupport::FormatString(_T("Lumitop Connect Success %d Ch"), LumitopNum), LumitopNum);
		break;
	case EVT_ZEROLENGTH:
		LogWrite(CStringSupport::FormatString(_T("Lumitop EVT_ZEROLENGTH %d Ch"), LumitopNum), LumitopNum);
		break;
	case EVT_CONFAILURE:
		LogWrite(CStringSupport::FormatString(_T("Lumitop EVT_CONFAILURE %d Ch"), LumitopNum), LumitopNum);
		break;
	default:
		LogWrite(CStringSupport::FormatString(_T("Lumitop Unknown Socket event %d Ch"), LumitopNum), LumitopNum);
		break;
	}
}

BOOL CLumitopThread::getConectCheck()
{
	SockAddrIn addrin;
	GetSockName(addrin);
	LONG  uAddr = addrin.GetIPAddr();
	if (uAddr == 0)
		return FALSE;	//접속안함
	else
		return TRUE;	//접속함
}

void CLumitopThread::RemoveClient()
{
	ShutdownConnection((SOCKET)m_hComm);
}

bool CLumitopThread::SocketServerOpen(CString strServerPort)
{
	m_bMelsecSimulaion = true;
	SetSmartAddressing(false);
	SetServerState(true);
	bool ret = CreateSocket(strServerPort, AF_INET, SOCK_STREAM, 0);
	if (ret) return WatchComm();
	else return false;
}

UINT CLumitopThread::LumitopThreadProc(LPVOID pParam)
{
	CLumitopThread* pThis = reinterpret_cast<CLumitopThread*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;

}

BOOL CLumitopThread::CreateTask(){
	BOOL bRet = TRUE;
	m_pThreadLumitop = ::AfxBeginThread(LumitopThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadLumitop)
		bRet = FALSE;
	m_pThreadLumitop->m_bAutoDelete = FALSE;
	m_pThreadLumitop->ResumeThread();
	return bRet;
}

void CLumitopThread::CloseTask()
{
	if (m_pThreadLumitop != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadLumitop->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadLumitop->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadLumitop->m_hThread, 1L);
				theApp.m_PgLog->LOG_INFO(_T("Terminate Lumitop Thread"));
			}
		}
		delete m_pThreadLumitop;
		m_pThreadLumitop = NULL;
	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}

void CLumitopThread::SocketSendto(int Num, CString strContents, int iCommand)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	m_csSocketSend.Lock();
	CString strCommand = CStringSupport::FormatString(_T("%c%s,%c"), _STX, strContents, _ETX);
	char *lpCommand = StringToChar(strCommand);
	theApp.m_LumitopSocketManager[Num].WriteComm((BYTE*)lpCommand, strlen(lpCommand), 100L);
	delete lpCommand;

	m_lastContent[Num] = strContents;

	if (Num == PC1)
		theApp.m_pLumitopSendReceiver1Log->LOG_INFO(CStringSupport::FormatString(_T("[MC -> VS] [Command : %s] ->%s"), MC_PacketNameTable[iCommand], strContents));
	else
		theApp.m_pLumitopSendReceiver2Log->LOG_INFO(CStringSupport::FormatString(_T("[MC -> VS] [Command : %s] ->%s"), MC_PacketNameTable[iCommand], strContents));

	m_csSocketSend.Unlock();
}

void CLumitopThread::LogWrite(CString strContents, int Num)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	g_DlgMainView->m_ViewingAngleListBox[Num].InsertString(0, CStringSupport::FormatString(_T("[%s] %s"), GetNowSystemTimeMilliseconds(), strContents));
	//g_MainLog->m_PgListBox.InsertString(0, CStringSupport::FormatString(_T("[%s] %s"), GetNowSystemTimeMilliseconds(), strContents));
	theApp.m_LumitopLog->LOG_INFO(strContents);
}

void CLumitopThread::LumitopPLCResult(int Num, int iPanelNum, CString ResultMsg, int ResultCode, CString strPanelID, int iIndexPanelNum)
{
	if (theApp.m_LumitopPassMode)
		ResultCode = m_codeOk;

	theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_PreGammaResult1 + iPanelNum, &ResultCode);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopGrabEnd1 + iPanelNum, OffSet_0, TRUE);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopEnd1 + iPanelNum, OffSet_0, TRUE);

	if (iIndexPanelNum != 0)
	{
		CString strMsg = CStringSupport::FormatString(_T("Ch,%d,PTRN,3"), iIndexPanelNum);
		theApp.m_PgSocketManager[PgServer_1].SendPGMessage(strMsg, iIndexPanelNum);
	}

	LogWrite(CStringSupport::FormatString(_T("Panel %d %s Lumitop Result %s"), Num, strPanelID, ResultMsg), Num);
}

BOOL CLumitopThread::LumitopVecAdd(CString strPanel, CString strFpcID, int iPanelNum, int iIndexNum, int iPCNo, int iCurIndex)
{
	BOOL flag = TRUE;
	InspResult panelData;
	panelData.Reset();

	panelData.m_bInspStart = TRUE;
	if (theApp.m_iTimer[LumitopGrabTimer] == 0)
		panelData.time_check.SetCheckTime(8000);
	else
		panelData.time_check.SetCheckTime(theApp.m_iTimer[LumitopGrabTimer] * 1000);

	panelData.time_check.StartTimer();
	panelData.m_iIndexPanelNum = iIndexNum;
	panelData.m_iPanelNum = iPanelNum;
	panelData.m_iPCNum = iPCNo;
	panelData.m_cellId = strPanel;
	panelData.m_FpcID = strFpcID;
	panelData.m_iCurIndex = iCurIndex;

	if (panelData.m_cellId.IsEmpty())
		panelData.m_cellId = panelData.m_FpcID;

	for (auto &InspResult : theApp.m_lastLumitopResultVec)
	{
		if (!InspResult.m_cellId.CompareNoCase(strPanel))
		{
			flag = FALSE;
			LumitopPLCResult(iPCNo, iPanelNum, _T("PLC ID Error"), m_codePlcPanelError, strPanel, iIndexNum);
			break;
		}
	}

	for (auto &InspResult : theApp.m_lastLumitopResultVec)
	{
		if (InspResult.m_bInspStart == FALSE && flag == TRUE)
		{
			InspResult = panelData;
			break;
		}

	}

	return flag;
}

void CLumitopThread::ParshingLumitopData(int Num, CString strContents)
{
	CString strLumitopModelName;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	if (responseTokens.GetSize() < 3)
		return;

	strLumitopModelName = responseTokens[0];

	LogWrite(CStringSupport::FormatString(_T("[VS %d -> MC] ModelName[%s]"),
		Num, strLumitopModelName), Num);
}

#endif