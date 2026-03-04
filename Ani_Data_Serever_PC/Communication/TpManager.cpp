
#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "DlgMainLog.h"
#include "TpManager.h"

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

void CTpManager::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	m_strDummyContents.Format(_T(""));
	m_strDummyContents.ReleaseBuffer();

	if (theApp.m_bExitFlag == FALSE)
		return;

	CString strData;
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount, strData.GetBuffer(dwCount + 1), dwCount + 1);
	strData.ReleaseBuffer(dwCount);

	theApp.m_pTpSendReceiverLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] [TP -> MC] %s"), GetNowSystemTimeMilliseconds(), strData));

	CString strHeader = _T(""), strContent = _T("");
	strHeader.Format(_T("#"));
	// Packet이 연달아 붙어서 올 경우 처리 할 수 있도록
	int iFind = 0;
	if (strHeader == strData.Left(1))
	{
		for (int ii = 2; ii < strData.GetLength() + 1; ii++)
		{
			CString Msg = strData.Left(ii).Right(1);
			if (strHeader == Msg)
			{
				strContent = strData.Left(ii);
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
	int iLen = strData.GetLength();
	if (iFind < iLen - 1)
	{
		m_strDummyContents = strData.Mid(iFind, strData.GetLength());
	}

	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContent, _T('*'), responseTokens);

	CString strIndexName = _T("");
	int iZoneNum = 0;
	int iPanelNum = 0;
	int iChNum = _ttoi(responseTokens[1]) - 1;
	BOOL bMachine = FALSE;

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