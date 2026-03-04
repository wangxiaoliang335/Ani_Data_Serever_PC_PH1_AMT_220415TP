
#include "stdafx.h"
#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "DlgMainLog.h"
#else
#include "DlgGammaMain.h"
#endif
#include "PgManager.h"

CPgManager::CPgManager()
{
#if _SYSTEM_AMTAFT_
	m_lastContent.resize(18);
	m_lastResult.resize(18);
	m_lastCommand.resize(18);
	m_lastRequest.resize(18);
	m_lastGammaL.resize(18);
	m_lastGammaX.resize(18);
	m_lastGammaY.resize(18);
#endif
}

CPgManager::~CPgManager()
{
}

void CPgManager::SendPGMessage(CString strMsg, int iChNum, int iStageNum)
{
	m_csSocketSend.Lock();

	CString strAMsg, strSendMsg;

	strAMsg = strMsg;

	if (theApp.m_iMachineType == SetAMT)
	{
		CStringArray responseTokens;
		CStringSupport::GetTokenArray(strAMsg, _T(','), responseTokens);
		int iSize = responseTokens.GetSize();
		CString strPacket[5];
	
		for (int ii = 0; ii < iSize; ii++)
			strPacket[ii] = responseTokens[ii];
	
		//Ch,Number,PREGAMMA,START,CELLID 5
		//Ch,Number,PTRN,PatternNumber 4
		//Ch,Number,KEY,BACK 4
		//Ch,Number,KEY,NEXT 4
		//Ch,Number,CONTACTOFF,CELLID 4
		//Ch,Number,CONTACT,CELLID 4
		//Ch,Number,TURNOFF 3
		//Ch,Number,TURNON 3
		if (!strPacket[1].Compare(_T("18")))
		{
			strPacket[1] = _T("21");
			if (!strPacket[2].CompareNoCase(_T("PREGAMMA")))
				strAMsg.Format(_T("%s,%s,%s,%s,%s"), strPacket[0], strPacket[1], strPacket[2], strPacket[3], strPacket[4]);
			else if (!strPacket[2].CompareNoCase(_T("KEY")) || !strPacket[2].CompareNoCase(_T("PTRN")) || 
				!strPacket[2].CompareNoCase(_T("CONTACTOFF")) || !strPacket[2].CompareNoCase(_T("CONTACT")))
				strAMsg.Format(_T("%s,%s,%s,%s"), strPacket[0], strPacket[1], strPacket[2], strPacket[3]);
			else
				strAMsg.Format(_T("%s,%s,%s"), strPacket[0], strPacket[1], strPacket[2]);
		}
	}

	int iLen = strAMsg.GetLength();
	int iNum = iChNum - 1;
	strSendMsg.Format(_T("%c%c%04X%s%c"), _STX, _DEST, iLen, strAMsg, _ETX);

	char *lpCommand = StringToChar(strSendMsg);
	theApp.m_PgSocketManager[m_iPcNum].WriteComm((BYTE*)lpCommand, strlen(lpCommand), 100L);
#if _SYSTEM_AMTAFT_
	m_lastContent[iNum] = strMsg;
#else
	m_lastContent[iStageNum][iChNum] = strMsg;
#endif
	delete lpCommand;

	theApp.m_PgSendReceiverLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] [MC -> PG] %s"), GetNowSystemTimeMilliseconds(), strSendMsg));
	m_csSocketSend.Unlock();
}

void CPgManager::PgLogMessage(CString strContents)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	m_csSocketSend.Lock();

	g_MainLog->m_PgListBox.InsertString(0, CStringSupport::FormatString(_T("[%s] %s"), GetNowSystemTimeMilliseconds(), strContents));
	theApp.m_PgLog->LOG_INFO(strContents);

	m_csSocketSend.Unlock();
}

void CPgManager::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	CString strData;
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount, strData.GetBuffer(dwCount + 1), dwCount + 1);
	strData.ReleaseBuffer(dwCount);

	CStringArray responseTokens;
	CString m_strContents, m_strHeader, strParsing;
	CStringSupport::GetTokenArray(strData, _ETX, responseTokens);

	theApp.m_PgSendReceiverLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] [PG -> MC] %s"), GetNowSystemTimeMilliseconds(), strData));

	if (responseTokens.GetSize() == 1)
	{
		PgLogMessage(_T("ETX Message No !!!!"));
		return;
	}

	for (int ii = 0; ii < responseTokens.GetSize() - 1; ii++)
	{
		strParsing = responseTokens[ii];
		m_strHeader.Format(_T("%x"), strParsing.GetAt(0));
		UINT iHeader = (UINT)_ttoi(m_strHeader);
		if (iHeader != _STX || strParsing.GetAt(0) == ',')
		{
			PgLogMessage(_T("STX Message No !!!!"));
			continue;
		}

		int iFind = strParsing.Find(',') - 2;
		m_strContents = strParsing.Mid(iFind, strParsing.GetLength());

		m_csPgData.Lock();
#if _SYSTEM_AMTAFT_
		if (m_iPcNum == PgServer_1)
			AOIDataReceived(m_strContents);
		else
			ULDDataReceived(m_strContents);
#else
		GammaDataReceived(m_strContents);
#endif
		m_csPgData.Unlock();
	}

}

#if _SYSTEM_AMTAFT_
void CPgManager::AOIDataReceived(CString strContents)
{
	CStringArray responseTokens;
	int iPanelNum, iIndexNum, iPanelCheal, iChNum, iPgOrderNum = 0;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);
	CString strPanelID;
	CString strPreGammaResult;

	if (responseTokens[0].CompareNoCase(_T("ch")))
	{
		PgLogMessage(_T("PG  Response Error !!!!"));
		return;
	}

	iPanelCheal = _ttoi(responseTokens[1]);	//ÆÐ³ÎÀÇ ¹øÈ£			//1ch ~ 16 ch
	iChNum = iPanelCheal - 1;				//Ã¤³ÎÀÇ º¯¼ö ¹øÈ£   ch0~ ch15
	iPanelNum = iChNum % 4;		//ÆÐ³ÎÀÇ Ã¤³Î¹øÈ£		//ex)AZone 0ch , 1ch, 2ch 3ch

	if (iPanelNum == 3)
		iIndexNum = (iPanelCheal / MaxZone) - 1;	//ÆÐ³ÎÀÇ index ¹øÈ£ Azone = 0 , Bzone = 1 , CZone = 2 , DZone = 3
	else
		iIndexNum = (iPanelCheal / MaxZone);

	if (responseTokens[3] == _T("CONTACT")) // Ch,Number,DONE,KEY,RESET
		iPgOrderNum = PG_CONTACT_ON;
	else if (responseTokens[3] == _T("PREGAMMA"))
		iPgOrderNum = PG_PREGAMMA;
	else if (responseTokens[3] == _T("KEY"))
	{
		if (responseTokens[4] == _T("RESET"))
			iPgOrderNum = PG_CONTACT_OFF;
		//else if (responseTokens[4] == _T("NEXT"))
		//	iPgOrderNum = PG_PATTERN_NEXT;
		//if (responseTokens[4] == _T("BACK"))
		//	iPgOrderNum = PG_PATTERN_BACK;
	}
	else if (responseTokens[3] == _T("CONTACTOFF"))
		iPgOrderNum = PG_CONTACT_OFF;

	//Ch,2,CONTACT,END,GOOD
	CString strIndexName = PG_IndexName[iIndexNum];
	m_lastRequest[iChNum] = strContents;

	for (auto &InspResult : theApp.m_lastIndexPgVec[iIndexNum])
	{
		if (InspResult.m_iStatus == iPgOrderNum && InspResult.m_iIndexPanelNum == iPanelCheal) 
		{
			if (responseTokens[3] == _T("CONTACT"))
			{
				//Ch,Number,DONE,CONTACT,END,GOOD
				if (responseTokens[4] == _T("END"))
				{
					if (responseTokens[5] == _T("GOOD"))
					{
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOnResult + iIndexNum, iPanelNum, &m_codeOk);
						PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Contact On Success"), strIndexName, iPanelCheal, InspResult.m_cellId));
						InspResult.time_check.StopTimer();
						InspResult.m_bResult = TRUE;
						theApp.m_bContact[iChNum] = FALSE;
						theApp.m_strContactPanelID[iChNum] = _T("");
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOnEnd + iIndexNum, iPanelNum, TRUE);


						CString strSendMsg;
						strSendMsg.Format(_T("#FW*%d#"), iChNum + 1);
						int iLen = strSendMsg.GetLength();


						char *lpCommand = StringToChar(strSendMsg);
						theApp.m_TpSocketManager.WriteComm((BYTE*)lpCommand, iLen, 100L);

						m_lastContent[iChNum] = strSendMsg;
						delete lpCommand;
					}
					else
					{
						if (theApp.m_bContact[iChNum] == TRUE && _ttoi(theApp.m_strPGName) == PG_MuhanZC)
						{
							InspResult.time_check.StopTimer();
							InspResult.m_bResult = TRUE;
							theApp.m_PgInexThread[iIndexNum]->PgVecAdd(theApp.m_strContactPanelID[iChNum], theApp.m_strContactPanelID[iChNum], iPanelNum, iIndexNum, PG_CONTACT_OFF, PGContactOffTimer, iPanelCheal);
							PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Contact Off ReStart"), strIndexName, iPanelCheal, InspResult.m_cellId));
							CString strMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,RESET"), iPanelCheal);
							theApp.m_PgSocketManager[PgServer_1].SendPGMessage(strMsg, iPanelCheal);
							theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZone1stContactResult + iIndexNum, iPanelNum, &m_codeFail);
						}
						else
						{
							if (theApp.m_PgPassMode)
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOnResult + iIndexNum, iPanelNum, &m_codeOk);
							else
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOnResult + iIndexNum, iPanelNum, &m_codeFail);

							PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Contact On Fail"), strIndexName, iPanelCheal, InspResult.m_cellId));
							InspResult.time_check.StopTimer();
							InspResult.m_bResult = TRUE;
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOnEnd + iIndexNum, iPanelNum, TRUE);
						}
					}
					m_lastCommand[iChNum] = responseTokens[3];
					m_lastResult[iChNum] = responseTokens[5];

					if (responseTokens.GetSize() == 10)
					{
						PGDfsList PgList;
						PgList.strPanelID = InspResult.m_cellId;
						PgList.strFpcID = InspResult.m_FpcID;
						PgList.m_strVBIT = responseTokens[6];
						PgList.m_strVDDI = responseTokens[7];
						PgList.m_strVCI = responseTokens[8];
						PgList.m_strProgramVersion = responseTokens[9];
						theApp.PGDfsInfoSave(PgList);
					}
				}
			}
			else if (responseTokens[3] == _T("KEY"))
			{
				//Ch,Number,DONE,KEY,NEXT
				//Ch,Number,DONE,KEY,RESET,END,GOOD
				//Ch,Number,DONE,KEY,RESET,END,NG
				if (responseTokens[4] == _T("RESET") && _ttoi(theApp.m_strPGName) == PG_MuhanZC)
				{
					if (responseTokens[6] == _T("GOOD"))
					{
						if (theApp.m_bContact[iChNum] == TRUE)
						{
							InspResult.time_check.StopTimer();
							InspResult.m_bResult = TRUE;
							theApp.m_PgInexThread[iIndexNum]->PgVecAdd(theApp.m_strContactPanelID[iChNum], theApp.m_strContactPanelID[iChNum], iPanelNum, iIndexNum, PG_CONTACT_ON, PGContactOnTimer, iPanelCheal);
							CString strMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACT,%s"), iPanelCheal, theApp.m_strContactPanelID[iChNum]);
							theApp.m_PgSocketManager[PgServer_1].SendPGMessage(strMsg, iPanelCheal);
							PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Contact On ReStart"), strIndexName, iPanelCheal, theApp.m_strContactPanelID[iChNum]));
							theApp.m_bContact[iChNum] = FALSE;
							theApp.m_strContactPanelID[iChNum] = _T("");
						}
						else
						{
							theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOffResult + iIndexNum, iPanelNum, &m_codeOk);
							PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Contact Off Success"), strIndexName, iPanelCheal));
							InspResult.time_check.StopTimer();
							InspResult.m_bResult = TRUE;
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOffEnd + iIndexNum, iPanelNum, TRUE);
						}
					}
					else
					{
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOffResult + iIndexNum, iPanelNum, &m_codeFail);
						PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Contact Off Fail"), strIndexName, iPanelCheal));
						InspResult.time_check.StopTimer();
						InspResult.m_bResult = TRUE;
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOffEnd + iIndexNum, iPanelNum, TRUE);
					}
				
				}
				/*else if (responseTokens[4] == _T("NEXT"))
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZonePatternNextEnd + iIndexNum, IndexZone, TRUE);
					PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Pattern Next End"), strIndexName, iPanelCheal));
					InspResult.time_check.StopTimer();
					InspResult.m_bResult = TRUE;

				}
				else if (responseTokens[4] == _T("BACK"))
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZonePatternBackEnd + iIndexNum, IndexZone, TRUE);
					PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Pattern Back End"), strIndexName, iPanelCheal));
					InspResult.time_check.StopTimer();
					InspResult.m_bResult = TRUE;
				}*/

				m_lastCommand[iChNum] = responseTokens[3];
				m_lastResult[iChNum] = responseTokens[4];
			}
			else if (responseTokens[3] == _T("PREGAMMA"))
			{
				if(responseTokens[4] == _T("END"))
				{
					if (responseTokens.GetCount() == 7) // 진짜 씨부랄...
					{
						strPanelID = responseTokens[5];
						strPreGammaResult = responseTokens[6];
					}
					else
						strPreGammaResult = responseTokens[5];

					//Ch,Number,DONE,PREGAMMA,END,TEST0,GOOD
					//Ch,Number,DONE,PREGAMMA,END,TEST0,NG
					if (strPreGammaResult == _T("GOOD"))
					{                                    
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_PreGammaResult1, iPanelNum, &m_codeOk);
						PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Gamma Success"), strIndexName, iPanelCheal, InspResult.m_cellId));
						InspResult.time_check.StopTimer();
						InspResult.m_bResult = TRUE;
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaEnd1, iPanelNum, TRUE);
					}
					else
					{
						if (theApp.m_PgPassMode)
							theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_PreGammaResult1, iPanelNum, &m_codeOk);
						else
							theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_PreGammaResult1, iPanelNum, &m_codeFail);

						PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Gamma Fail"), strIndexName, iPanelCheal, InspResult.m_cellId));
						InspResult.time_check.StopTimer();
						InspResult.m_bResult = TRUE;
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaEnd1, iPanelNum, TRUE);
						
					}

					m_lastCommand[iChNum] = responseTokens[3];
					m_lastResult[iChNum] = strPreGammaResult;
				}
				CString strMsg = CStringSupport::FormatString(_T("Ch,%d,PTRN,3"), iChNum + 1);
				theApp.m_PgSocketManager[PgServer_1].SendPGMessage(strMsg, iChNum + 1);
			}
			else if (responseTokens[3] == _T("CONTACTOFF")) // Ch, Number, DONE, KEY, RESET, END, GOOD == > Ch, Number, DONE, CONTACT OFF, END, GOOD
			{
				if (responseTokens[4] == _T("END"))
				{
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOffResult + iIndexNum, iPanelNum, &m_codeOk);
					PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Contact Off Success"), strIndexName, iPanelCheal));
					InspResult.time_check.StopTimer();
					InspResult.m_bResult = TRUE;
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOffEnd + iIndexNum, iPanelNum, TRUE);
				}
				m_lastCommand[iChNum] = responseTokens[3];
				m_lastResult[iChNum] = responseTokens[5];
			}
			//>>210422 
			else if (responseTokens[3] == _T("GET_RECIPE")) // Ch, Number, DONE, GET_RECIPE, END, RECIPENAME
			{
				if (responseTokens[4] == _T("END"))
				{
					//>>0126 yjlim
					if (theApp.m_bPGCodeUsable == TRUE)
					{
						int iCheckErr(PGCode_NG); // 0: GOOD, 1: NGCode PG, Mes 안맞음, 2: TimeOver


						theApp.m_VecPGCode_PG[iChNum - 1].m_PGCode[0] = responseTokens[5]; //ex : Ch,1,DONE,GET_RECIPE,END,RECIPENAME

																						   //CTimerCheck cTime(1500);
																						   //cTime.StartTimer();
						while (1)
						{
							/*if (cTime.IsTimeOver())
							break;*/
							if (theApp.m_VecPGCode_Mes[iChNum - 1].time_check.IsTimeOver())
							{
								iCheckErr = PGCode_AckTimeout;
								break;
							}

							//if (theApp.m_VecPGCode_Mes[iChNum - 1].m_PGCode.size() > 0)
							if (theApp.m_VecPGCode_Mes[iChNum - 1].m_PGCode[0] != _T(""))
							{

								for (auto CodeList : theApp.m_VecPGCode_Mes[iChNum - 1].m_PGCode)
								{
									if (CodeList == theApp.m_VecPGCode_PG[iChNum - 1].m_PGCode[0])
										iCheckErr = PGCode_OK;
									else
										PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] MesPGCode : %s, PG PGCode: %s,"),
											PG_IndexName[iIndexNum], iChNum, InspResult.m_cellId,
											CodeList, theApp.m_VecPGCode_PG[iChNum - 1].m_PGCode[0]));
								}
								break;
							}
							else
							{
								CString sendMsg = CStringSupport::FormatString(_T("MES,%s,ProductSpec,PGCode"), InspResult.m_cellId);
								theApp.m_pComView->m_SocketClient[0]->SendMachineMessage(sendMsg, FALSE, FALSE);
								Delay(100, TRUE);
							}



							/*if (theApp.m_VecPGCode_Mes[iChNum - 1].m_PGCode.size() > 0
							&& theApp.m_VecPGCode_PG[iChNum - 1].m_PGCode != theApp.m_VecPGCode_Mes[iChNum - 1].m_PGCode)
							{

							PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] MesPGCode : %s, PG PGCode: %s,"),
							PG_IndexName[iStageNum], iChNum, theApp.InspResult.m_cellId,
							theApp.m_VecPGCode_Mes[iChNum - 1].m_PGCode, theApp.m_VecPGCode_PG[iChNum - 1].m_PGCode));
							iCheckErr = PGCode_NG;
							break;
							}
							else if (theApp.m_VecPGCode_PG[iChNum - 1].m_PGCode == theApp.m_VecPGCode_Mes[iChNum - 1].m_PGCode)
							break;*/

						}
						//>>여기 알람칠 내역 추가하고,, 
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_PGCodeCh1Result + iIndexNum, iPanelNum, &iCheckErr);
						theApp.m_VecPGCode_PG[iChNum - 1].Reset();
						theApp.m_VecPGCode_Mes[iChNum - 1].Reset();
						//<<
						/////////////////////////////////////////////////////////////

					}



				}
			}
			//<<
		}
	}
}

void CPgManager::ULDDataReceived(CString strContents)
{
	CStringArray responseTokens;
	int iPanelCheal, iChNum = 0, iPgOrderNum = 0;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);
	CString strPanelID;
	CString strPreGammaResult;

	if (responseTokens[0].CompareNoCase(_T("ch")))
	{
		PgLogMessage(_T("PG  Response Error !!!!"));
		return;
	}

	if (theApp.m_iMachineType == SetAMT)
	{
		if (_ttoi(responseTokens[1]) == 21)
			responseTokens[1] = _T("18");
	}

	iPanelCheal = _ttoi(responseTokens[1]);	// ch17~18
	iChNum = iPanelCheal - 1;				

	int iPanelNum = (iChNum) % 2;

	if (responseTokens[3] == _T("CONTACT")) // Ch,Number,DONE,KEY,RESET
		iPgOrderNum = ManualStageContactOn;
	else if (responseTokens[3] == _T("PREGAMMA"))
		iPgOrderNum = ManualStagePreGamma;
	else if (responseTokens[3] == _T("KEY"))
	{
		if (responseTokens[4] == _T("RESET"))
			iPgOrderNum = ManualStageContactOff;
		if (responseTokens[4] == _T("NEXT"))
			iPgOrderNum = ManualStageNext;
		if (responseTokens[4] == _T("BACK"))
			iPgOrderNum = ManualStageBack;
	}
	else if (responseTokens[3] == _T("CONTACTOFF"))
		iPgOrderNum = ManualStageContactOff;

	//Ch,2,CONTACT,END,GOOD
	m_lastRequest[iChNum] = strContents;

	if (responseTokens[3] == _T("CONTACT"))
	{
		//Ch,Number,DONE,CONTACT,END,GOOD
		if (responseTokens[4] == _T("END"))
		{
			if (responseTokens[5] == _T("GOOD"))
			{
				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOnResult + iPanelNum, &m_codeOk);
				PgLogMessage(CStringSupport::FormatString(_T("Ch %d Panel [%s] Contact On Success"), iPanelCheal, theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_cellId));
				theApp.m_VecManualStage[iPgOrderNum][iPanelNum].time_check.StopTimer();
				theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_bResult = TRUE;
				theApp.m_bContact[iChNum] = FALSE;
				theApp.m_strContactPanelID[iChNum] = _T("");
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOnEnd + iPanelNum, OffSet_0, TRUE);
			}
			else
			{
				if (theApp.m_bContact[iChNum] == TRUE && _ttoi(theApp.m_strPGName) == PG_MuhanZC)
				{
					theApp.m_VecManualStage[iPgOrderNum][iChNum].time_check.StopTimer();
					theApp.m_VecManualStage[iPgOrderNum][iChNum].m_bResult = TRUE;
					theApp.m_ManualThread->ManualStageVecAdd(theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_cellId, theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_cellId, iChNum, ManualStageContactOff, PGContactOffTimer, m_iPcNum - 1);
					PgLogMessage(CStringSupport::FormatString(_T("Ch %d Panel [%s] Contact Off ReStart"), iPanelCheal, theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_cellId));
					CString strMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,RESET"), iPanelCheal);
					theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strMsg, iPanelCheal);
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_MStageAContactResetResult + iChNum, OffSet_0, &m_codeFail);
				}
				else
				{
					if (theApp.m_PgPassMode)
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOnResult + iPanelNum, &m_codeOk);
					else
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOnResult + iPanelNum, &m_codeFail);

					PgLogMessage(CStringSupport::FormatString(_T("Ch %d Panel [%s] Contact On Fail"), iPanelCheal, theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_cellId));

					theApp.m_VecManualStage[iPgOrderNum][iPanelNum].time_check.StopTimer();
					theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_bResult = TRUE;
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOnEnd + iPanelNum, OffSet_0, TRUE);
				}
			}
			m_lastCommand[iChNum] = responseTokens[3];
			m_lastResult[iChNum] = responseTokens[5];
		}
	}
	else if (responseTokens[3] == _T("KEY"))
	{
		//Ch,Number,DONE,KEY,NEXT,END
		if (responseTokens[4] == _T("RESET") && _ttoi(theApp.m_strPGName) == PG_MuhanZC)
		{
			if (responseTokens[6] == _T("GOOD"))
			{
				if (theApp.m_bContact[iChNum] == TRUE)
				{
					theApp.m_VecManualStage[iPgOrderNum][iPanelNum].time_check.StopTimer();
					theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_bResult = TRUE;
					theApp.m_ManualThread->ManualStageVecAdd(theApp.m_strContactPanelID[iChNum], theApp.m_strContactPanelID[iChNum], iChNum, ManualStageContactOn, PGContactOnTimer, m_iPcNum - 1);
					CString strMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACT,%s"), iPanelCheal, theApp.m_strContactPanelID[iChNum]);
					theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strMsg, iPanelCheal);
					PgLogMessage(CStringSupport::FormatString(_T("Ch %d Panel [%s] Contact On ReStart"), iPanelCheal, theApp.m_strContactPanelID[iChNum]));
					theApp.m_bContact[iChNum] = FALSE;
					theApp.m_strContactPanelID[iChNum] = _T("");
				}
				else
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOffResult + iChNum, &m_codeOk);
					PgLogMessage(CStringSupport::FormatString(_T("Ch %d Contact Off Success"), iPanelCheal));
					theApp.m_VecManualStage[iPgOrderNum][iPanelNum].time_check.StopTimer();
					theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_bResult = TRUE;
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOffEnd + iChNum, OffSet_0, TRUE);
				}
			}
			else
			{
				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOffResult + iChNum, &m_codeFail);
				PgLogMessage(CStringSupport::FormatString(_T("Ch %d Contact Off Fail"), iPanelCheal));
				theApp.m_VecManualStage[iPgOrderNum][iPanelNum].time_check.StopTimer();
				theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_bResult = TRUE;
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOffEnd + iChNum, OffSet_0, TRUE);
		
			}
		
		}
		else if (responseTokens[4] == _T("NEXT"))
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactNextEnd + iPanelNum, OffSet_0, TRUE);
			PgLogMessage(CStringSupport::FormatString(_T("Ch %d Pattern Next End"), iPanelCheal));
			theApp.m_VecManualStage[iPgOrderNum][iPanelNum].time_check.StopTimer();
			theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_bResult = TRUE;

		}
		else if (responseTokens[4] == _T("BACK"))
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactBackEnd + iPanelNum, OffSet_0, TRUE);
			PgLogMessage(CStringSupport::FormatString(_T("Ch %d Pattern Back End"), iPanelCheal));
			theApp.m_VecManualStage[iPgOrderNum][iPanelNum].time_check.StopTimer();
			theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_bResult = TRUE;
		}

		m_lastCommand[iChNum] = responseTokens[4];
		m_lastResult[iChNum] = responseTokens[5];
	}
	else if (responseTokens[3] == _T("PREGAMMA"))
	{
		if (responseTokens[4] == _T("END"))	
		{
			if (responseTokens.GetCount() == 7) // 진짜 씨부랄...
			{
				strPanelID = responseTokens[5];
				strPreGammaResult = responseTokens[6];
			}
			else
				strPreGammaResult = responseTokens[5];
			//Ch,Number,DONE,PREGAMMA,END,TEST0,GOOD
			//Ch,Number,DONE,PREGAMMA,END,TEST0,NG,
			//Ch,Number,DONE,PREGAMMA,START,END,TEST0,NG,NG	이건 어디 프로토콜이야
			//if (responseTokens[6] == _T("GOOD"))
			if (strPreGammaResult == _T("GOOD"))
			{
				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAGammaResult + iPanelNum, &m_codeOk);
				PgLogMessage(CStringSupport::FormatString(_T("Ch %d Panel [%s] Gamma Success"), iPanelCheal, theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_cellId));
				theApp.m_VecManualStage[iPgOrderNum][iPanelNum].time_check.StopTimer();
				theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_bResult = TRUE;
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaEnd + iPanelNum, OffSet_0, TRUE);
			}
			else
			{
				if (theApp.m_PgPassMode)
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAGammaResult + iPanelNum, &m_codeOk);
				else
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAGammaResult + iPanelNum, &m_codeFail);

				PgLogMessage(CStringSupport::FormatString(_T("Ch %d Panel [%s] Gamma Fail"), iPanelCheal, theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_cellId));
				theApp.m_VecManualStage[iPgOrderNum][iPanelNum].time_check.StopTimer();
				theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_bResult = TRUE;
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaEnd + iPanelNum, OffSet_0, TRUE);
			}

			m_lastCommand[iChNum] = responseTokens[3];
			m_lastResult[iChNum] = strPreGammaResult;
		}
	}
	else if (responseTokens[3] == _T("CONTACTOFF"))
	{
		if (responseTokens[4] == _T("END"))
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOffResult + iPanelNum, &m_codeOk);

			PgLogMessage(CStringSupport::FormatString(_T("Ch %d Contact Off Success"), iPanelCheal));
			theApp.m_VecManualStage[iPgOrderNum][iPanelNum].time_check.StopTimer();
			theApp.m_VecManualStage[iPgOrderNum][iPanelNum].m_bResult = TRUE;
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOffEnd + iPanelNum, OffSet_0, TRUE);
		}
		m_lastCommand[iChNum] = responseTokens[3];
		m_lastResult[iChNum] = responseTokens[5];
	}
}
#else
void CPgManager::GammaDataReceived(CString strContents)
{
	CStringArray responseTokens;
	int iPanelNum, iChNum, iStageNum;
	int iPgOrderNum = 0;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	if (responseTokens[0].CompareNoCase(_T("ch")))
	{
		PgLogMessage(_T("PG  Response Error !!!!"));
		return;
	}

	iChNum = _ttoi(responseTokens[1]); // Ch 1 ~ 24

	iPanelNum = (iChNum - 1) % 2; // 존 별 채널

	if (iPanelNum == PanelNum1)
		iStageNum = iChNum / 2;
	else
		iStageNum = (iChNum / 2) - 1;

	if (responseTokens[3] == _T("CONTACT"))
		iPgOrderNum = PG_CONTACT_ON;
	else if (responseTokens[3] == _T("GAMMA"))
		iPgOrderNum = PG_GAMMA;
	else if (responseTokens[3] == _T("KEY"))
	{
		if (responseTokens[4] == _T("RESET")) 
			iPgOrderNum = PG_CONTACT_OFF;
		if (responseTokens[4] == _T("NEXT"))
			iPgOrderNum = PG_PATTERN_NEXT;
		if (responseTokens[4] == _T("BACK"))
			iPgOrderNum = PG_PATTERN_BACK;
	}
	else if (responseTokens[3] == _T("CONTACTOFF")) 
		iPgOrderNum = PG_CONTACT_OFF;
	else if (responseTokens[3] == _T("PID"))
		iPgOrderNum = PG_PID_CHECK;

	m_lastRequest[iStageNum][iPanelNum] = strContents;

	if (responseTokens[3] == _T("CONTACT")) // CH,NUM,DONE,CONTACT,END,RESULT
	{
		if (responseTokens[4] == _T("END"))
		{
			if (responseTokens[5] == _T("GOOD"))
			{
				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + iStageNum, iPanelNum, &m_codeOk);

				PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Contact On Success"), PG_IndexName[iStageNum], iChNum, theApp.m_lastGammaVec[iStageNum][iPanelNum].m_cellId));
				theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
				theApp.m_bContact[iChNum - 1] = FALSE;
				theApp.m_strContactPanelID[iChNum - 1] = _T("");
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOnEnd1 + iStageNum, OffSet_0, TRUE);

				if (responseTokens.GetSize() == 10)
				{
					PGDfsList PgList;
					PgList.strPanelID = theApp.m_lastGammaVec[iStageNum][iPanelNum].m_cellId;
					PgList.strFpcID = theApp.m_lastGammaVec[iStageNum][iPanelNum].m_FpcID;
					PgList.m_strVBIT = responseTokens[6];
					PgList.m_strVDDI = responseTokens[7];
					PgList.m_strVCI = responseTokens[8];
					PgList.m_strProgramVersion = responseTokens[9];
					theApp.GammaDfsInfoSave(PgList);

					m_csPgData.Lock();
					m_lastGammaVBAT[iStageNum][iPanelNum] = responseTokens[6];
					m_lastGammaVDDI[iStageNum][iPanelNum] = responseTokens[7];
					m_lastGammaVCI[iStageNum][iPanelNum] = responseTokens[8];
					m_lastGammaPGCODE[iStageNum][iPanelNum] = responseTokens[9];
					m_csPgData.Unlock();
				}
				else
					theApp.m_pTraceLog->LOG_INFO(_T("**************** PanelID [%s] Contact On Protocol Error ****************"), theApp.m_lastGammaVec[iStageNum][iPanelNum].m_cellId);

				theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
			}
			else
			{
				if (theApp.m_bContact[iChNum - 1] == TRUE && _ttoi(theApp.m_strPGName) == PG_MuhanZC)
				{
					theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
					theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
					theApp.m_GammaThread[iStageNum]->GammaVecAdd(theApp.m_strContactPanelID[iChNum - 1], theApp.m_strContactPanelID[iChNum - 1], iPanelNum, iStageNum, iChNum, PG_CONTACT_OFF, PGContactOffTimer);
					PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Contact Off ReStart"), PG_IndexName[iStageNum], iPanelNum, theApp.m_strContactPanelID[iChNum - 1]));
				
					CString strMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,RESET"), iChNum);
					theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strMsg, iPanelNum, iStageNum);
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOn1stResult + iStageNum, iPanelNum, &m_codeFail);
				}
				else
				{
					if (theApp.m_PgPassMode)
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + iStageNum, iPanelNum, &m_codeOk);
					else
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + iStageNum, iPanelNum, &m_codeFail);


					PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Contact On Fail"), PG_IndexName[iStageNum], iChNum, theApp.m_lastGammaVec[iStageNum][iPanelNum].m_cellId));
					theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
					theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOnEnd1 + iStageNum, OffSet_0, TRUE);

					CString strPanelID = theApp.m_lastGammaVec[iStageNum][iPanelNum].m_cellId;
					CString strFpcID = theApp.m_lastGammaVec[iStageNum][iPanelNum].m_FpcID;
					theApp.GammaDefectInfoSave(strPanelID, strFpcID, theApp.m_strContactNgCode, theApp.m_strContactNgGrade);
				}
			}

			m_lastCommand[iStageNum][iPanelNum] = responseTokens[3];
			m_lastResult[iStageNum][iPanelNum] = responseTokens[5];
		}
	}
	else if (responseTokens[3] == _T("KEY"))  
	{
		if (responseTokens[4] == _T("RESET") && theApp.m_GammaThread[iStageNum]->m_bOperatorModeFlag[iStageNum] == TRUE)
		{
			//if (responseTokens[6] == _T("GOOD"))
			//{
				if (theApp.m_bContact[iChNum - 1] == TRUE && _ttoi(theApp.m_strPGName) == PG_MuhanZC)
				{
					theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
					theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
					theApp.m_GammaThread[iStageNum]->GammaVecAdd(theApp.m_strContactPanelID[iChNum - 1], theApp.m_strContactPanelID[iChNum - 1], iPanelNum, iStageNum, iChNum, PG_CONTACT_ON, PGContactOnTimer);
					PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Contact On ReStart"), PG_IndexName[iStageNum], iPanelNum, theApp.m_strContactPanelID[iChNum - 1]));
			
					CString strMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACT,%s"), iChNum, theApp.m_strContactPanelID[iChNum - 1]);
					theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strMsg, iPanelNum, iStageNum);
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + iStageNum, iPanelNum, &m_codeFail);
				}
				else
				{
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + iStageNum, iPanelNum, &m_codeOk);
			
					PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Contact Off Success"), PG_IndexName[iStageNum], iChNum));
					theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
					theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOffEnd1 + iStageNum, OffSet_0, TRUE);
				}
			//}
			//else
			//{
			//	theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + iStageNum, iPanelNum, &m_codeFail);
			//
			//	PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Contact Off Fail"), PG_IndexName[iStageNum], iChNum));
			//	theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
			//	theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
			//	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOffEnd1 + iStageNum, OffSet_0, TRUE);
			//}
		}
		else if (responseTokens[4] == _T("NEXT"))
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactNextEnd1 + iStageNum, OffSet_0, TRUE);

			PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Pattern Next End"), PG_IndexName[iStageNum], iChNum));
			theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
			theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;

		}
		else if (responseTokens[4] == _T("BACK"))
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactBackEnd1 + iStageNum, OffSet_0, TRUE);
			PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Pattern Back End"), PG_IndexName[iStageNum], iChNum));
			theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
			theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
		}

		m_lastCommand[iStageNum][iPanelNum] = responseTokens[3];
		m_lastResult[iStageNum][iPanelNum] = responseTokens[4];
	}
	else if (responseTokens[3] == _T("GAMMA")) // TWICE => CH,NUM,DONE,GAMMA,END,NG,CODE,GRADE
	{
		if (responseTokens[4] == _T("END"))
		{
			if (responseTokens[5] == _T("GOOD"))
			{
				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + iStageNum, iPanelNum, &m_codeOk);

				PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Gamma Success"), PG_IndexName[iStageNum], iChNum, theApp.m_lastGammaVec[iStageNum][iPanelNum].m_cellId));
				theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
				theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd1 + iStageNum, iPanelNum, TRUE);
			}			
			else
			{
				if (theApp.m_PgPassMode)
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + iStageNum, iPanelNum, &m_codeOk);
				else
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + iStageNum, iPanelNum, &m_codeFail);

				CString strPanelID = theApp.m_lastGammaVec[iStageNum][iPanelNum].m_cellId;
				CString strFpcID = theApp.m_lastGammaVec[iStageNum][iPanelNum].m_FpcID;
				CString strCode = responseTokens[6];
				CString strGrade = responseTokens[7];
				
				theApp.GammaDefectInfoSave(strPanelID, strFpcID, strCode, strGrade);

				PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Gamma Fail"), PG_IndexName[iStageNum], iChNum, theApp.m_lastGammaVec[iStageNum][iPanelNum].m_cellId));
				theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
				theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd1 + iStageNum, iPanelNum, TRUE);
			}

			m_lastCommand[iStageNum][iPanelNum] = responseTokens[3];
			m_lastResult[iStageNum][iPanelNum] = responseTokens[5];
		}
	}
	else if (responseTokens[3] == _T("CONTACTOFF")) // Ch, Number, DONE, KEY, RESET, END, GOOD == > Ch, Number, DONE, CONTACT OFF, END, GOOD
	{
		if (responseTokens[4] == _T("END"))
		{
			theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + iStageNum, iPanelNum, &m_codeOk);

			PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Contact Off Success"), PG_IndexName[iStageNum], iChNum));
			theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
			theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOffEnd1 + iStageNum, OffSet_0, TRUE);
		}
		m_lastCommand[iStageNum][iPanelNum] = responseTokens[3];
		m_lastResult[iStageNum][iPanelNum] = responseTokens[5];
	}
	else if (responseTokens[3] == _T("PID"))
	{
		if (responseTokens[4] == _T("END"))
		{
			if (responseTokens[5] == _T("GOOD"))
			{
				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1PIDCheckResult + iStageNum, iPanelNum, &m_codeOk);

				PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] PID Check Success"), PG_IndexName[iStageNum], iChNum, theApp.m_lastGammaVec[iStageNum][iPanelNum].m_cellId));
				theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
				theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1PIDCheckEnd + iStageNum, iPanelNum, TRUE);
			}
			else
			{
				if (theApp.m_PgPassMode)
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1PIDCheckResult + iStageNum, iPanelNum, &m_codeOk);
				else
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1PIDCheckResult + iStageNum, iPanelNum, &m_codeFail);

				PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] PID Check Fail"), PG_IndexName[iStageNum], iChNum, theApp.m_lastGammaVec[iStageNum][iPanelNum].m_cellId));
				theApp.m_lastGammaVec[iStageNum][iPanelNum].time_check.StopTimer();
				theApp.m_lastGammaVec[iStageNum][iPanelNum].m_bResult = TRUE;
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1PIDCheckEnd + iStageNum, iPanelNum, TRUE);
			}
		}
	}
}
#endif

void CPgManager::OnEvent(UINT uEvent, LPVOID lpvData)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	switch (uEvent)
	{
	case EVT_CONDROP:
		PgLogMessage(CStringSupport::FormatString(_T("PG Connect Drop")));
		break;
	case EVT_CONSUCCESS:
		PgLogMessage(CStringSupport::FormatString(_T("PG Connect Success")));
		break;
	case EVT_ZEROLENGTH:
		PgLogMessage(CStringSupport::FormatString(_T("PG EVT_ZEROLENGTH")));
		break;
	case EVT_CONFAILURE:
		PgLogMessage(CStringSupport::FormatString(_T("PG EVT_CONFAILURE")));
		break;
	default:
		PgLogMessage(CStringSupport::FormatString(_T("Unknown Socket event")));
		break;
	}
}

BOOL CPgManager::getConectCheck()
{
	SockAddrIn addrin;
	GetSockName(addrin);
	LONG  uAddr = addrin.GetIPAddr();
	if (uAddr == 0)
		return FALSE;	//Á¢¼Ó¾ÈÇÔ
	else
		return TRUE;	//Á¢¼ÓÇÔ
}

bool CPgManager::SocketServerOpen(CString strServerPort, int iPcNum)
{
	m_bMelsecSimulaion = true;
	SetSmartAddressing(false);
	SetServerState(true);
	m_iPcNum = iPcNum;
	bool ret = CreateSocket(strServerPort, AF_INET, SOCK_STREAM, 0);
	if (ret) return WatchComm();
	else return false;
}

void CPgManager::RemoveClient()
{
	ShutdownConnection((SOCKET)m_hComm);
}
