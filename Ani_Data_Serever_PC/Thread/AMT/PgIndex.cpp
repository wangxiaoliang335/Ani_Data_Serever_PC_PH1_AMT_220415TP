
#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "DlgMainView.h"
#include "PgIndex.h"

CPgIndex::CPgIndex(int iZoneNum)
{
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_iIndexNum = theApp.m_indexList[iZoneNum].m_indexNum;
	m_iZoneNum = iZoneNum;
	m_iPanelNameAddr = iZoneNum * 4;

	m_bStartFlag[iZoneNum] = FALSE;

	for (int ii = 0; ii < 6; ii++)
		for (int jj = 0; jj < PanelMaxCount; jj++)
			m_bStart[iZoneNum][ii][jj] = FALSE;
	
	for (int ii = 0; ii < PanelMaxCount; ii++)
		theApp.m_lastIndexPgVec[ii].resize(12);

	m_strIndexName = PG_IndexName[iZoneNum];

	for (int ii = 0; ii < PG_MAX_CH; ii++)
	{
		theApp.m_strContactPanelID[ii] = _T("");
		theApp.m_bContact[ii] = FALSE;
	}
}

CPgIndex::~CPgIndex()
{
}

void CPgIndex::ThreadRun()
{
	int iChNum = 0;
	int iprogramChNum = 0;
	for (auto &InspResult : theApp.m_lastIndexPgVec[m_iZoneNum])
		InspResult.Reset();

	while (::WaitForSingleObject(m_hQuit, 50) != WAIT_OBJECT_0)
	{
		theApp.m_PgConectStatus[PgServer_1] = theApp.m_PgSocketManager[PgServer_1].getConectCheck();
		theApp.m_TpConectStatus = theApp.m_TpSocketManager.getConectCheck();

		//if (theApp.m_bAllPassMode)
		//	continue;

		if (theApp.m_PlcConectStatus == FALSE)
			continue;

		if (theApp.m_PgConectStatus[PgServer_1] || theApp.m_PgPassMode)
		{
			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZoneContactPlcSend + m_iZoneNum, OffSet_0))
				ZonePanelCheck(ContactPanelCheck);
			else
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactPcReceiver + m_iZoneNum, OffSet_0, FALSE);


			if (theApp.m_TpConectStatus || theApp.m_TpPassMode)
			{
				if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZoneTouchPlcSend + m_iZoneNum, OffSet_0))
					ZonePanelCheck(TouchPanelCheck);
				else
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchPcReceiver + m_iZoneNum, OffSet_0, FALSE);
			}

			if (theApp.m_iMachineType == SetAMT)
			{
				if (m_iZoneNum == BZone)
				{
					if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_PreGammaPlcSend, OffSet_0))
						ZonePanelCheck(PreGammaPanelCheck);
					else
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaPcReceiver, 0, FALSE);
				}
			}


			for (int ii = 0; ii < PanelMaxCount; ii++)
			{
				m_bStartFlag[m_iZoneNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZoneContactOnStart + m_iZoneNum, ii);

				if (m_bStartFlag[m_iZoneNum] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOnResult + m_iZoneNum, ii, &m_codeReset);
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZone1stContactResult + m_iZoneNum, ii, &m_codeReset);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOnEnd + m_iZoneNum, ii, FALSE);
				}


				if (m_bStart[m_iZoneNum][0][ii] == !m_bStartFlag[m_iZoneNum])
				{
					m_bStart[m_iZoneNum][0][ii] = m_bStartFlag[m_iZoneNum];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[%s] Contact On Panel %d Start Flag [%s]"), m_strIndexName, ii + 1, m_bStart[m_iZoneNum][0][ii] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bStartFlag[m_iZoneNum] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOnResult + m_iZoneNum, ii, &m_codeReset);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZone1stContactResult + m_iZoneNum, ii, &m_codeReset);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOnEnd + m_iZoneNum, ii, FALSE);
						Delay(100);
						ZoneContactOn(ii);
					}
				}
			}

			for (int ii = 0; ii < PanelMaxCount; ii++)
			{
				m_bStartFlag[m_iZoneNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZoneContactOffStart + m_iZoneNum, ii);

				if (m_bStartFlag[m_iZoneNum] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOffResult + m_iZoneNum, ii, &m_codeReset);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOffEnd + m_iZoneNum, ii, FALSE);
				}
				
				if (m_bStart[m_iZoneNum][1][ii] == !m_bStartFlag[m_iZoneNum])
				{
					m_bStart[m_iZoneNum][1][ii] = m_bStartFlag[m_iZoneNum];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[%s] Contact Off Panel %d Start Flag [%s]"), m_strIndexName, ii + 1, m_bStart[m_iZoneNum][1][ii] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[m_iZoneNum] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOffResult + m_iZoneNum, ii, &m_codeReset);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOffEnd + m_iZoneNum, ii, FALSE);
						Delay(100);
						ZoneContactOff(ii);
					}
				}
			}

			/*m_bStartFlag[m_iZoneNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZonePatternNext + m_iZoneNum, IndexZone);

			if (m_bStartFlag[m_iZoneNum] == FALSE)
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZonePatternNextEnd + m_iZoneNum, IndexZone, FALSE);
			
			if (m_bStart[m_iZoneNum][2][0] == !m_bStartFlag[m_iZoneNum])
			{
				m_bStart[m_iZoneNum][2][0] = m_bStartFlag[m_iZoneNum];
				theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[%s] Pattern Next Start Flag [%s]"), m_strIndexName, m_bStart[m_iZoneNum][2][0] == FALSE ? _T("FALSE") : _T("TRUE")));
				if (m_bStartFlag[m_iZoneNum] == TRUE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZonePatternNextEnd + m_iZoneNum, IndexZone, FALSE);
					for (int ii = 0; ii < PanelMaxCount; ii++)
					{
						if (theApp.m_lastIndexPgVec[m_iZoneNum][ii].m_bContactOn == TRUE)
							ZonePatternNext(ii);
					}
				}
			}

			m_bStartFlag[m_iZoneNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZonePatternBack + m_iZoneNum, IndexZone);

			if (m_bStartFlag[m_iZoneNum] == FALSE)
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZonePatternBackEnd + m_iZoneNum, IndexZone, FALSE);
			
			if (m_bStart[m_iZoneNum][3][0] == !m_bStartFlag[m_iZoneNum])
			{
				m_bStart[m_iZoneNum][3][0] = m_bStartFlag[m_iZoneNum];
				theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[%s] Pattern Back Start Flag [%s]"), m_strIndexName, m_bStart[m_iZoneNum][3][0] == FALSE ? _T("FALSE") : _T("TRUE")));
				if (m_bStartFlag[m_iZoneNum] == TRUE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZonePatternBackEnd + m_iZoneNum, IndexZone, FALSE);
					for (int ii = 0; ii < PanelMaxCount; ii++)
					{
						if (theApp.m_lastIndexPgVec[m_iZoneNum][ii].m_bContactOn == TRUE)
							ZonePatternBack(ii);
					}
				}
			}*/

			for (int ii = 0; ii < PanelMaxCount; ii++)
			{
				if (theApp.m_TpConectStatus == TRUE || theApp.m_TpPassMode)
				{
					m_bStartFlag[m_iZoneNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZoneTouchInspectionStart + m_iZoneNum, ii);

					if (m_bStartFlag[m_iZoneNum] == FALSE)
					{
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneTouchResult + m_iZoneNum, ii, &m_codeReset);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchInspectionEnd + m_iZoneNum, ii, FALSE);
					}


					if (m_bStart[m_iZoneNum][5][ii] == !m_bStartFlag[m_iZoneNum])
					{
						m_bStart[m_iZoneNum][5][ii] = m_bStartFlag[m_iZoneNum];
						theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[%s] Touch Inspection Panel %d Start Flag [%s]"), m_strIndexName, ii + 1, m_bStart[m_iZoneNum][5][ii] == FALSE ? _T("FALSE") : _T("TRUE")));

						if (m_bStartFlag[m_iZoneNum] == TRUE)
						{
							theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneTouchResult + m_iZoneNum, ii, &m_codeReset);
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchInspectionEnd + m_iZoneNum, ii, FALSE);
							Delay(100, TRUE);
							ZoneTouchInspection(ii);
						}
					}
				}
			}

			if (theApp.m_iMachineType == SetAMT)
			{
				if (m_iZoneNum == BZone)
				{
					for (int ii = 0; ii < PanelMaxCount; ii++)
					{
						m_bStartFlag[m_iZoneNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_PreGammaStart1, ii);

						if (m_bStartFlag[m_iZoneNum] == FALSE)
						{
							theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_PreGammaResult1, ii, &m_codeReset);
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaEnd1, ii, FALSE);
						}

						if (m_bStart[m_iZoneNum][4][ii] == !m_bStartFlag[m_iZoneNum])
						{
							m_bStart[m_iZoneNum][4][ii] = m_bStartFlag[m_iZoneNum];
							theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Pre Gamma Panel %d Start Flag [%s]"), ii + 1, m_bStart[m_iZoneNum][4][ii] == FALSE ? _T("FALSE") : _T("TRUE")));
							if (m_bStartFlag[m_iZoneNum] == TRUE)
							{
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_PreGammaResult1, ii, &m_codeReset);
								theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaEnd1, ii, FALSE);
								Delay(100);
								ZonePreGamma(ii);
							}
						}
					}
				}
			}

			for (auto &InspResult : theApp.m_lastIndexPgVec[m_iZoneNum])
			{
				if (InspResult.m_bInspStart == TRUE)
				{
					if (InspResult.time_check.IsTimeOver())
					{
						switch (InspResult.m_iStatus)
						{
						case PG_PREGAMMA:
							if (theApp.m_PgPassMode)
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_PreGammaResult1, InspResult.m_iPanelNum, &m_codeOk);
							else
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_PreGammaResult1, InspResult.m_iPanelNum, &m_codeTimeOut);

							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaEnd1, InspResult.m_iPanelNum, TRUE);
							theApp.m_PgSocketManager[PgServer_1].PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Pre Gamma Time Out"), InspResult.m_strZoneName, InspResult.m_iIndexPanelNum, InspResult.m_cellId));
							theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] Pre Gamma [%s] Time out"), InspResult.m_strZoneName, InspResult.m_cellId));

							theApp.m_PgSocketManager[PgServer_1].SendPGMessage(CStringSupport::FormatString(_T("Ch,%d,PTRN,3"), InspResult.m_iIndexPanelNum), InspResult.m_iIndexPanelNum);
							break;
						case PG_CONTACT_ON:
							//if (theApp.m_PgPassMode)
							//	theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOnResult + m_iZoneNum, InspResult.m_iPanelNum, &m_codeOk);
							//else
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOnResult + m_iZoneNum, InspResult.m_iPanelNum, &m_codeTimeOut);

							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOnEnd + m_iZoneNum, InspResult.m_iPanelNum, TRUE);
							theApp.m_PgSocketManager[PgServer_1].PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] Contact On Time Out"), m_strIndexName, InspResult.m_iIndexPanelNum, InspResult.m_cellId));
							theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] Contact On [%s] Time out"), m_strIndexName, InspResult.m_cellId));
							break;
						case PG_CONTACT_OFF:
							//if (theApp.m_PgPassMode)
							//	theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOffResult + m_iZoneNum, InspResult.m_iPanelNum, &m_codeOk);
							//else
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOffResult + m_iZoneNum, InspResult.m_iPanelNum, &m_codeTimeOut);

							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOffEnd + m_iZoneNum, InspResult.m_iPanelNum, TRUE);
							theApp.m_PgSocketManager[PgServer_1].PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Contact Off Time Out"), m_strIndexName, InspResult.m_iIndexPanelNum));
							theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] Contact Off [%s] Time out"), m_strIndexName, InspResult.m_cellId));
							break;
						/*case PG_PATTERN_NEXT:
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZonePatternNextEnd + m_iZoneNum, IndexZone, TRUE);
							theApp.m_PgSocketManager.PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Pattern Next Time Out"), m_strIndexName, InspResult.m_iIndexPanelNum));
							break;
						case PG_PATTERN_BACK:
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZonePatternBackEnd + m_iZoneNum, IndexZone, TRUE);
							theApp.m_PgSocketManager.PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Pattern Back Time Out"), m_strIndexName, InspResult.m_iIndexPanelNum));
							break;*/
						case PG_TOUCH:
							if (theApp.m_TpPassMode)
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneTouchResult + m_iZoneNum, InspResult.m_iPanelNum, &m_codeOk);
							else
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneTouchResult + m_iZoneNum, InspResult.m_iPanelNum, &m_codeTimeOut);

							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchInspectionEnd + m_iZoneNum, InspResult.m_iPanelNum, TRUE);
							theApp.m_TpSocketManager.TpLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d TP Result Time Out"), m_strIndexName, InspResult.m_iIndexPanelNum));
							
							break;
						}
						InspResult.m_bResult = TRUE;
					}
				}

				if (InspResult.m_bResult == TRUE)
					InspResult.Reset();

			}

		}
	}
}

void CPgIndex::ZonePanelCheck(int iNum)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel, strFpcID;

	for (int ii = 0; ii < PanelMaxCount; ii++)
	{
		if (iNum == PreGammaPanelCheck)
		{
			theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_PreGammaPanel1 + ii, &pPanelData);
			strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

			theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_PreGammaFpcID1 + ii, &pFpcData);
			strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_AZonePanel1 + m_iPanelNameAddr + ii, &pPanelData);
			strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

			theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_AZoneFpcID1 + m_iPanelNameAddr + ii, &pFpcData);
			strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));
		}

		if (strPanel.IsEmpty())
			strPanel = strFpcID;

		if (strFpcID.IsEmpty() == FALSE)
		{
			switch (iNum)
			{
			case ContactPanelCheck: theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactPcReceiver + m_iZoneNum, OffSet_0, TRUE); break;
			case TouchPanelCheck: theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchPcReceiver + m_iZoneNum, OffSet_0, TRUE); break;
			case PreGammaPanelCheck: theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaPcReceiver, OffSet_0, TRUE); break;
			}
			break;
		}
	}
}


void CPgIndex::ZoneContactOn(int Num)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel = _T(""), strFpcID = _T("");
	int iChNum = m_iIndexNum + Num;
	int iReadlChNum = iChNum - 1;

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_AZonePanel1 + m_iPanelNameAddr + Num, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_AZoneFpcID1 + m_iPanelNameAddr + Num, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));
	
	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), Num);
			strFpcID.Format(_T("TEST%d"), Num);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOnResult + m_iZoneNum, Num, &m_codePlcSendReceiverError);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOnEnd + m_iZoneNum, Num, TRUE);
			theApp.m_PgSocketManager[PgServer_1].PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Contact On No Panel ID"), m_strIndexName, iChNum));
			return;
		}
	}

	if (strPanel.IsEmpty())
		strPanel = strFpcID;


	theApp.m_PgSocketManager[PgServer_1].PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Contact On Start Panel [%s][%s]"), m_strIndexName, iChNum, strPanel, strFpcID));

	PgVecAdd(strPanel, strFpcID, Num, m_iZoneNum, PG_CONTACT_ON, PGContactOnTimer, iChNum);
	theApp.m_bContact[iReadlChNum] = TRUE;
	theApp.m_strContactPanelID[iReadlChNum] = strPanel;

	//>> 210422
	//if (theApp.m_bPGCodeUsable == TRUE)
	//{
	//	/////////////////////////////////////////////////////////////
	//	//>>210126 yjlim
	//	PGCoderesult PGCodeTemp;
	//	PGCodeTemp.iChNum = iChNum;
	//	PGCodeTemp.m_cellId = strPanel;
	//
	//	theApp.m_VecPGCode_PG[iChNum - 1] = PGCodeTemp;
	//	//theApp.m_VecPGCode_PG.push_back(PGCodeTemp);
	//	//if (ii == ChMaxCount - 1) // 간격 0.15초 줘야된다함.
	//	Delay(150, TRUE);
	//	CString sendMsg = CStringSupport::FormatString(_T("MES,%s,ProductSpec,PGCode"), strPanel);
	//	PGCodeTemp.time_check.SetCheckTime(theApp.m_iTimer[PGContactOnTimer] * 1000); //
	//	PGCodeTemp.time_check.StartTimer();
	//
	//	theApp.m_VecPGCode_Mes[iChNum - 1] = PGCodeTemp;
	//	//theApp.m_VecPGCode_Mes.push_back(PGCodeTemp);
	//
	//	theApp.m_pComView->m_SocketClient[0]->SendMachineMessage(sendMsg, FALSE, FALSE);
	//
	//	sendMsg = CStringSupport::FormatString(_T("Ch,%d,GET_RECIPE,%s"), iChNum, strPanel);
	//	theApp.m_PgSocketManager[PgServer_1].SendPGMessage(sendMsg, iChNum);
	//
	//
	//	//>>해당 부분에 TPCode 송신 부분 넣자
	//	PGCodeTemp.time_check.StopTimer();
	//	theApp.m_VecTPCode_TP[iChNum - 1] = PGCodeTemp;
	//
	//	Delay(150, TRUE);
	//	sendMsg = CStringSupport::FormatString(_T("MES,%s,ProductSpec,TPCode"), strPanel);
	//
	//	PGCodeTemp.time_check.SetCheckTime(theApp.m_iTimer[PGContactOnTimer] * 1000); //
	//	PGCodeTemp.time_check.StartTimer();
	//
	//	theApp.m_VecTPCode_Mes[iChNum - 1] = PGCodeTemp;
	//
	//	theApp.m_pComView->m_SocketClient[0]->SendMachineMessage(sendMsg, FALSE, FALSE);
	//
	//	theApp.m_TpSocketManager.SendTPMessage(iChNum, TP_CodeRequest);
	//
	//	sendMsg = CStringSupport::FormatString(_T("Ch,%d,GET_RECIPE,%s"), iChNum, strPanel);
	//
	//
	//	//<<
	//
	//	//<<
	//	/////////////////////////////////////////////////////////////
	//}

	//<< 210422

	CString strMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACT,%s"), iChNum, strPanel);
	theApp.m_PgSocketManager[PgServer_1].SendPGMessage(strMsg, iChNum);
}

void CPgIndex::ZoneContactOff(int Num)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel = _T(""), strFpcID = _T("");
	int iChNum = m_iIndexNum + Num;

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_AZonePanel1 + m_iPanelNameAddr + Num, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_AZoneFpcID1 + m_iPanelNameAddr + Num, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), Num);
			strFpcID.Format(_T("TEST%d"), Num);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOnResult + m_iZoneNum, Num, &m_codePlcSendReceiverError);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOffEnd + m_iZoneNum, Num, TRUE);
			theApp.m_PgSocketManager[PgServer_1].PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Contact Off No Panel ID"), m_strIndexName, iChNum));
			return;
		}
	}

	if (strPanel.IsEmpty())
		strPanel = strFpcID;

	theApp.m_PgSocketManager[PgServer_1].PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel Contact Off Start"), m_strIndexName, iChNum));

	PgVecAdd(strPanel, strFpcID, Num, m_iZoneNum, PG_CONTACT_OFF, PGContactOffTimer, iChNum);


	CString strMsg;
	if (_ttoi(theApp.m_strPGName) == PG_MuhanZC)
		strMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,RESET"), iChNum);
	else
		strMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACTOFF,%s"), iChNum, strPanel);
	
	theApp.m_PgSocketManager[PgServer_1].SendPGMessage(strMsg, iChNum);
}

void CPgIndex::ZonePreGamma(int Num)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel = _T(""), strFpcID = _T("");
	theApp.IndexCheck();
	int indexNum = (theApp.m_CurrentIndexZone + (MaxZone - DZone)) % 4;
	int iChNum = theApp.m_indexList[indexNum].m_indexNum + Num;
	
	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_PreGammaPanel1 + Num, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_PreGammaFpcID1 + Num, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));
	
	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), Num);
			strFpcID.Format(_T("TEST%d"), Num);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_PreGammaResult1, Num, &m_codePlcSendReceiverError);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaEnd1, Num, TRUE);
			theApp.m_PgSocketManager[PgServer_1].PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d PreGamma No Panel ID"), PG_IndexName[indexNum], iChNum));

			return;
		}
	}

	if (strPanel.IsEmpty())
		strPanel = strFpcID;

	theApp.m_PgSocketManager[PgServer_1].PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d PreGamma Start Panel [%s][%s]"), PG_IndexName[indexNum], iChNum, strPanel, strFpcID));

	PgVecAdd(strPanel, strFpcID, Num, indexNum, PG_PREGAMMA, PGGammaTimer, iChNum);

	CString strMsg = CStringSupport::FormatString(_T("Ch,%d,PREGAMMA,START,%s"), iChNum, strPanel);
	theApp.m_PgSocketManager[PgServer_1].SendPGMessage(strMsg, iChNum);
}

//void CPgIndex::ZonePatternNext(int Num)
//{
//	PanelData pPanelData;
//	CString strPanel = _T("");
//	int iChNum = m_iIndexNum + Num;
//
//	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_AzonePanel1 + m_iPanelNameAddr + Num, &pPanelData);
//	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));
//
//	theApp.m_PgSocketManager.PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel Pattern Next Start"), m_strIndexName, iChNum));
//
//	PgVecAdd(strPanel, Num, m_iZoneNum, PG_PATTERN_NEXT, PGNextTimer, iChNum);
//
//	CString strMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,NEXT"), iChNum);
//	theApp.m_PgSocketManager.SendPGMessage(strMsg, iChNum);
//}
//
//void CPgIndex::ZonePatternBack(int Num)
//{
//	PanelData pPanelData;
//	CString strPanel = _T("");
//	int iChNum = m_iIndexNum + Num;
//
//	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_AzonePanel1 + m_iPanelNameAddr + Num, &pPanelData);
//	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));
//
//	theApp.m_PgSocketManager.PgLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel Pattern Back Start"), m_strIndexName, iChNum));
//
//	PgVecAdd(strPanel, Num, m_iZoneNum, PG_PATTERN_BACK, PGBackTimer, iChNum);
//
//	CString strMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,BACK"), iChNum);
//	theApp.m_PgSocketManager.SendPGMessage(strMsg, iChNum);
//}

void CPgIndex::ZoneTouchInspection(int Num)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel = _T(""), strFpcID = _T(""), sendMsg = _T("");
	int iChNum = m_iIndexNum + Num - 1;

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_AZonePanel1 + m_iPanelNameAddr + Num, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_AZoneFpcID1 + m_iPanelNameAddr + Num, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), Num);
			strFpcID.Format(_T("TEST%d"), Num);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneTouchResult + m_iZoneNum, Num, &m_codePlcSendReceiverError);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchInspectionEnd + m_iZoneNum, Num, TRUE);
			theApp.m_TpSocketManager.TpLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d TP No Panel ID"), m_strIndexName, iChNum + 1));
			return;
		}
	}

	if (strPanel.IsEmpty())
		strPanel = strFpcID;
	
	theApp.m_TpSocketManager.TpLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d TP Start Panel [%s][%s]"), m_strIndexName, iChNum + 1, strPanel, strFpcID));
	PgVecAdd(strPanel, strFpcID, Num, m_iZoneNum, PG_TOUCH, PGTPTimer, iChNum + 1);

	theApp.m_TpSocketManager.SendTPMessage(iChNum, TP_SendPanelID, strPanel);
	Delay(20);
	theApp.m_TpSocketManager.SendTPMessage(iChNum, TP_InspStart);
}

void CPgIndex::PgVecAdd(CString strPanel, CString strFpcID, int iPanelNum, int iZoneNum, int iPgOrderNum, int iPgTimerNum, int iChNum)
{
	m_csVecData.Lock();

	InspResult panelData;
	panelData.Reset();

	panelData.m_bInspStart = TRUE;
	panelData.m_iStatus = iPgOrderNum;
	panelData.m_cellId = strPanel;
	panelData.m_FpcID = strFpcID;
	panelData.m_iPanelNum = iPanelNum;
	panelData.m_iIndexPanelNum = iChNum;
	panelData.m_strZoneName = PG_IndexName[iZoneNum];

	if (theApp.m_iTimer[iPgTimerNum] == 0)
		panelData.time_check.SetCheckTime(15000);
	else
		panelData.time_check.SetCheckTime(theApp.m_iTimer[iPgTimerNum] * 1000);

	panelData.time_check.StartTimer();

	for (auto &InspResult : theApp.m_lastIndexPgVec[iZoneNum])
	{
		if (InspResult.m_bInspStart == FALSE)
		{
			InspResult = panelData;
			break;
		}
	}

	m_csVecData.Unlock();
}


UINT CPgIndex::PgIndexProc(LPVOID pParam)
{
	CPgIndex* pThis = reinterpret_cast<CPgIndex*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;

}

BOOL CPgIndex::CreateTask(){
	BOOL bRet = TRUE;
	m_pPgIndex = ::AfxBeginThread(PgIndexProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pPgIndex)
		bRet = FALSE;
	m_pPgIndex->m_bAutoDelete = FALSE;
	m_pPgIndex->ResumeThread();
	return bRet;
}

void CPgIndex::CloseTask()
{
	if (m_pPgIndex != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pPgIndex->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pPgIndex->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pPgIndex->m_hThread, 1L);
				theApp.m_PgLog->LOG_INFO(_T("Terminate IndexZone Thread"));
			}
		}
		delete m_pPgIndex;
		m_pPgIndex = NULL;

	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}


#endif