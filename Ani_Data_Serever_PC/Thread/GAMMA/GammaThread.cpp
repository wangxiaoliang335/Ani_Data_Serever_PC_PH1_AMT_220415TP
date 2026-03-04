
#include "stdafx.h"

#if _SYSTEM_GAMMA_

#include "DlgGammaMain.h"
#include "GammaThread.h"

CGammaThread::CGammaThread(int iStageNum)
{
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_iPcNum = (iStageNum / 6) % 2;
	m_istageNum = iStageNum;
	m_iPanelNum = (iStageNum * 2) + 1;
	m_bStartFlag[iStageNum] = FALSE;
	m_bOperatorModeFlag[iStageNum] = FALSE;
	theApp.m_lastGammaVec[m_istageNum].resize(2);
	theApp.m_lastGammaVec[m_istageNum][PanelNum1].Reset();
	theApp.m_lastGammaVec[m_istageNum][PanelNum2].Reset();

	for (int ii = 0; ii < GammaPGStatusCount; ii++)
		m_bStart[iStageNum][ii] = FALSE;
}

CGammaThread::~CGammaThread()
{
}

void CGammaThread::ThreadRun()
{
	
	while (::WaitForSingleObject(m_hQuit, 50) != WAIT_OBJECT_0)
	{
		theApp.m_PgConectStatus[m_iPcNum] = theApp.m_PgSocketManager[m_iPcNum].getConectCheck();

		//if (theApp.m_bAllPassMode)
		//	continue;

		if (theApp.m_PgConectStatus)
		{
			if (theApp.m_PlcConectStatus == FALSE)
				continue;

			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaStage1PlcSend + m_istageNum, OffSet_0))
				GammaPanelCheck();
			else
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1PcReceiver + m_istageNum, OffSet_0, FALSE);

			m_bOperatorModeFlag[m_istageNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaStageOperatorModeStart1 + m_istageNum, OffSet_0);

			m_bStartFlag[m_istageNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Gamma_1MTPStart1 + m_istageNum, GammaStageMtp1);

			if (m_bStartFlag[m_istageNum] == FALSE)
			{
				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + m_istageNum, OffSet_0, &m_codeReset);
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd1 + m_istageNum, OffSet_0, FALSE);
			}

			if (m_bStart[m_istageNum][GammaStageMtp1] == !m_bStartFlag[m_istageNum])
			{
				m_bStart[m_istageNum][GammaStageMtp1] = m_bStartFlag[m_istageNum];
				theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[Stage%d] Panel %d Gamma Start Flag [%s]"), m_istageNum + 1, m_iPanelNum + 1, m_bStart[m_istageNum][GammaStageMtp1] == FALSE ? _T("FALSE") : _T("TRUE")));
				if (m_bStartFlag[m_istageNum] == TRUE)
				{
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + m_istageNum, OffSet_0, &m_codeReset);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd1 + m_istageNum, OffSet_0, FALSE);
					Delay(100);
					GammaMtpStart(GammaStageMtp1);
				}
			}

			m_bStartFlag[m_istageNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Gamma_1MTPStart1 + m_istageNum, GammaStageMtp2);

			if (m_bStartFlag[m_istageNum] == FALSE)
			{
				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult2 + m_istageNum, OffSet_0, &m_codeReset);
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd2 + m_istageNum, OffSet_0, FALSE);
			}

			if (m_bStart[m_istageNum][GammaStageMtp2] == !m_bStartFlag[m_istageNum])
			{
				m_bStart[m_istageNum][GammaStageMtp2] = m_bStartFlag[m_istageNum];
				theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[Stage%d] Panel %d Gamma Start Flag [%s]"), m_istageNum + 1, m_iPanelNum + 1, m_bStart[m_istageNum][GammaStageMtp2] == FALSE ? _T("FALSE") : _T("TRUE")));
				if (m_bStartFlag[m_istageNum] == TRUE)
				{
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult2 + m_istageNum, OffSet_0, &m_codeReset);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd2 + m_istageNum, OffSet_0, FALSE);
					Delay(100);
					GammaMtpStart(GammaStageMtp2);
				}
			}

			m_bStartFlag[m_istageNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaContactOnStart1 + m_istageNum, OffSet_0);

			if (m_bStartFlag[m_istageNum] == FALSE)
			{
				for (int ii = 0; ii < ChMaxCount; ii++)
				{
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + m_istageNum, ii, &m_codeReset);
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOn1stResult + m_istageNum, ii, &m_codeReset);
				}
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOnEnd1 + m_istageNum, OffSet_0, FALSE);
			}

			if (m_bStart[m_istageNum][GammaStageContactOn] == !m_bStartFlag[m_istageNum])
			{
				m_bStart[m_istageNum][GammaStageContactOn] = m_bStartFlag[m_istageNum];
				theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[Stage%d] Contact On Start Flag [%s]"), m_istageNum + 1, m_bStart[m_istageNum][GammaStageContactOn] == FALSE ? _T("FALSE") : _T("TRUE")));
				if (m_bStartFlag[m_istageNum] == TRUE)
				{
					for (int ii = 0; ii < ChMaxCount; ii++)
					{
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + m_istageNum, ii, &m_codeReset);
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOn1stResult + m_istageNum, ii, &m_codeReset);
					}
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOnEnd1 + m_istageNum, OffSet_0, FALSE);
					GammaContactOnStart();
				}
			}

			m_bStartFlag[m_istageNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaContactOffStart1 + m_istageNum, OffSet_0);

			if (m_bStartFlag[m_istageNum] == FALSE)
			{
				for (int ii = 0; ii < ChMaxCount; ii++)
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + m_istageNum, ii, &m_codeReset);

				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOffEnd1 + m_istageNum, OffSet_0, FALSE);
			}

			if (m_bStart[m_istageNum][GammaStageContactOff] == !m_bStartFlag[m_istageNum])
			{
				m_bStart[m_istageNum][GammaStageContactOff] = m_bStartFlag[m_istageNum];
				theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[Stage%d] Contact Off Start Flag [%s]"), m_istageNum + 1, m_bStart[m_istageNum][GammaStageContactOff] == FALSE ? _T("FALSE") : _T("TRUE")));
				if (m_bStartFlag[m_istageNum] == TRUE)
				{
					for (int ii = 0; ii < ChMaxCount; ii++)
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + m_istageNum, ii, &m_codeReset);

					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOffEnd1 + m_istageNum, OffSet_0, FALSE);
					GammaContactOffStart();
				}
			}

			m_bStartFlag[m_istageNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaContactNextStart1 + m_istageNum, OffSet_0);

			if (m_bStartFlag[m_istageNum] == FALSE)
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactNextEnd1 + m_istageNum, OffSet_0, FALSE);

			if (m_bStart[m_istageNum][GammaStageNext] == !m_bStartFlag[m_istageNum])
			{
				m_bStart[m_istageNum][GammaStageNext] = m_bStartFlag[m_istageNum];
				theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[Stage%d] Pattern Next Start Flag [%s]"), m_istageNum + 1, m_bStart[m_istageNum][GammaStageNext] == FALSE ? _T("FALSE") : _T("TRUE")));
				if (m_bStartFlag[m_istageNum] == TRUE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactNextEnd1 + m_istageNum, OffSet_0, FALSE);
					GammaNextStart();
				}
			}

			m_bStartFlag[m_istageNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaContactBackStart1 + m_istageNum, OffSet_0);

			if (m_bStartFlag[m_istageNum] == FALSE)
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactBackEnd1 + m_istageNum, OffSet_0, FALSE);

			if (m_bStart[m_istageNum][GammaStageBack] == !m_bStartFlag[m_istageNum])
			{
				m_bStart[m_istageNum][GammaStageBack] = m_bStartFlag[m_istageNum];
				theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[Stage%d] Pattern Back Start Flag [%s]"), m_istageNum + 1, m_bStart[m_istageNum][GammaStageBack] == FALSE ? _T("FALSE") : _T("TRUE")));
				if (m_bStartFlag[m_istageNum] == TRUE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactBackEnd1 + m_istageNum, OffSet_0, FALSE);
					GammaBackStart();
				}
			}
			
			m_bStartFlag[m_istageNum] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaStagePIDCheckStart1 + m_istageNum, OffSet_0);

			if (m_bStartFlag[m_istageNum] == FALSE)
			{
				for (int ii = 0; ii < ChMaxCount; ii++)
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1PIDCheckResult + m_istageNum, ii, &m_codeReset);

				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1PIDCheckEnd + m_istageNum, OffSet_0, FALSE);
			}

			if (m_bStart[m_istageNum][GammaStagePIDCheck] == !m_bStartFlag[m_istageNum])
			{
				m_bStart[m_istageNum][GammaStagePIDCheck] = m_bStartFlag[m_istageNum];
				theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("[Stage%d] PID Check Start Flag [%s]"), m_istageNum + 1, m_bStart[m_istageNum][GammaStagePIDCheck] == FALSE ? _T("FALSE") : _T("TRUE")));
				if (m_bStartFlag[m_istageNum] == TRUE)
				{
					for (int ii = 0; ii < ChMaxCount; ii++)
						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1PIDCheckResult + m_istageNum, ii, &m_codeReset);

					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1PIDCheckEnd + m_istageNum, OffSet_0, FALSE);
					GammaPIDCheckStart();
				}
			}

			for (int ii = 0; ii < theApp.m_lastGammaVec[m_istageNum].size(); ii++)
			{
				if (theApp.m_lastGammaVec[m_istageNum][ii].m_LastCheck == TRUE)
				{
					if (theApp.m_lastGammaVec[m_istageNum][ii].time_check.IsTimeOver())
					{
						switch (theApp.m_lastGammaVec[m_istageNum][ii].m_iStatus)
						{
						case PG_GAMMA:
							if (theApp.m_PgPassMode)
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + m_istageNum, theApp.m_lastGammaVec[m_istageNum][ii].m_iPanelNum, &m_codeOk);
							else
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + m_istageNum, theApp.m_lastGammaVec[m_istageNum][ii].m_iPanelNum, &m_codeTimeOut);

							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd1 + m_istageNum, theApp.m_lastGammaVec[m_istageNum][ii].m_iPanelNum, TRUE);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d PanelID [%s] Gamma Time Out"),
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum, theApp.m_lastGammaVec[m_istageNum][ii].m_cellId));
							break;
						case PG_CONTACT_ON:
							if (theApp.m_PgPassMode)
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + m_istageNum, theApp.m_lastGammaVec[m_istageNum][ii].m_iPanelNum, &m_codeOk);
							else
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + m_istageNum, theApp.m_lastGammaVec[m_istageNum][ii].m_iPanelNum, &m_codeTimeOut);

							Delay(10, TRUE);

							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOnEnd1 + m_istageNum, OffSet_0, TRUE);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d PanelID [%s] Contact On Time Out"), 
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum, theApp.m_lastGammaVec[m_istageNum][ii].m_cellId));
							break;
						case PG_CONTACT_OFF:
							if (theApp.m_PgPassMode)
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + m_istageNum, theApp.m_lastGammaVec[m_istageNum][ii].m_iPanelNum, &m_codeOk);
							else
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + m_istageNum, theApp.m_lastGammaVec[m_istageNum][ii].m_iPanelNum, &m_codeTimeOut);

							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOffEnd1 + m_istageNum, OffSet_0, TRUE);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Contact Off Time Out"), 
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum));
							break;
						case PG_PATTERN_NEXT:
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactNextEnd1 + m_istageNum, OffSet_0, TRUE);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Pattern Next Time Out"), 
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum));
							break;
						case PG_PATTERN_BACK:
							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactBackEnd1 + m_istageNum, OffSet_0, TRUE);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Pattern Back Time Out"), 
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum));
							break;
						case PG_PID_CHECK:
							if (theApp.m_PgPassMode)
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1PIDCheckResult + m_istageNum, theApp.m_lastGammaVec[m_istageNum][ii].m_iPanelNum, &m_codeOk);
							else
								theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1PIDCheckResult + m_istageNum, theApp.m_lastGammaVec[m_istageNum][ii].m_iPanelNum, &m_codeTimeOut);

							Delay(10, TRUE);

							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1PIDCheckEnd + m_istageNum, OffSet_0, TRUE);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d PID Check Time Out"),
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum));
							break;
						}

						theApp.m_lastGammaVec[m_istageNum][ii].m_bResult = TRUE;
					}
				}
				else if (theApp.m_lastGammaVec[m_istageNum][ii].m_bInspStart == TRUE)
				{
					if (theApp.m_lastGammaVec[m_istageNum][ii].time_check.IsTimeOver())
					{
						CString strPgSendMsg = _T("");
						switch (theApp.m_lastGammaVec[m_istageNum][ii].m_iStatus)
						{
						case PG_CONTACT_ON:
							strPgSendMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACT,%s"), theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum, theApp.m_lastGammaVec[m_istageNum][ii].m_cellId);
							theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strPgSendMsg, ii, m_istageNum);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d PanelID [%s] Contact On *ReStart*"), 
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum, theApp.m_lastGammaVec[m_istageNum][ii].m_cellId));
							break;
						case PG_CONTACT_OFF:
							if (_ttoi(theApp.m_strPGName) == PG_MuhanZC)
								strPgSendMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,RESET"), theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum);
							else 
								strPgSendMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACTOFF,%s"), theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum, theApp.m_lastGammaVec[m_istageNum][ii].m_cellId);
							
							theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strPgSendMsg, ii, m_istageNum);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d PanelID [%s] Contact Off *ReStart*"), 
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum, theApp.m_lastGammaVec[m_istageNum][ii].m_cellId));
							break;
						case PG_PATTERN_NEXT:
							strPgSendMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,NEXT"), theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum);
							theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strPgSendMsg, ii, m_istageNum);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Next *ReStart*"), 
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum));
							break;
						case PG_PATTERN_BACK:
							strPgSendMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,BACK"), theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum);
							theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strPgSendMsg, ii, m_istageNum);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Back *ReStart*"), 
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum));
							break;
						case PG_PID_CHECK:
							strPgSendMsg = CStringSupport::FormatString(_T("Ch,%d,PID,"), theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum);
							theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strPgSendMsg, ii, m_istageNum);

							theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d PID Check *ReStart*"), 
								m_istageNum + 1, theApp.m_lastGammaVec[m_istageNum][ii].m_iGammaRunChNum));
							break;
						}

						theApp.m_lastGammaVec[m_istageNum][ii].m_LastCheck = TRUE;
						theApp.m_lastGammaVec[m_istageNum][ii].time_check.SetCheckTime(theApp.m_iTimer[theApp.m_lastGammaVec[m_istageNum][ii].m_iStatus] - 1);
						theApp.m_lastGammaVec[m_istageNum][ii].time_check.StartTimer();
					}
				}

				if (theApp.m_lastGammaVec[m_istageNum][ii].m_bResult == TRUE)
					theApp.m_lastGammaVec[m_istageNum][ii].Reset();
			}
		}
	}
}

void CGammaThread::GammaPanelCheck()
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanelID, strFpcID;

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_GammaStage1PanelID1 + m_istageNum, &pPanelData);
	strPanelID = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_GammaStage1FpcID1 + m_istageNum, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strPanelID.IsEmpty() == FALSE)
	{
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1PcReceiver + m_istageNum, 0, TRUE);
		return;
	}

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_GammaStage1PanelID2 + m_istageNum, &pPanelData);
	strPanelID = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_GammaStage1FpcID2 + m_istageNum, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strPanelID.IsEmpty() == FALSE)
	{
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1PcReceiver + m_istageNum, 0, TRUE);
		return;
	}
}

void CGammaThread::GammaMtpStart(int iPanelNum)
{
	m_csPgStatus.Lock();

	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strSendMsg = _T("");
	
	if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaStage1PanelExist1 + m_istageNum, iPanelNum))
	{
		theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + m_istageNum, iPanelNum, &m_codeReset);

		int iPanelIDAddr = iPanelNum == PanelNum1 ? eWordType_GammaStage1PanelID1 : eWordType_GammaStage1PanelID2;
		int iFpcIDAddr = iPanelNum == PanelNum1 ? eWordType_GammaStage1FpcID1 : eWordType_GammaStage1FpcID2;

		theApp.m_pEqIf->m_pMNetH->GetPanelData(iPanelIDAddr + m_istageNum, &pPanelData);
		theApp.m_lastGammaVec[m_istageNum][iPanelNum].m_cellId = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));
		theApp.m_pEqIf->m_pMNetH->GetFpcIdData(iFpcIDAddr + m_istageNum, &pFpcData);
		theApp.m_lastGammaVec[m_istageNum][iPanelNum].m_FpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

		if (theApp.m_lastGammaVec[m_istageNum][iPanelNum].m_cellId.IsEmpty() == TRUE)
		{
			if (theApp.m_PanelTestStart)
			{
				CString strPanelID;
				strPanelID.Format(_T("TEST%d"), iPanelNum);
				theApp.m_lastGammaVec[m_istageNum][iPanelNum].m_FpcID = strPanelID;
				theApp.m_lastGammaVec[m_istageNum][iPanelNum].m_cellId = strPanelID;
			}
			else
			{
				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + m_istageNum, iPanelNum, &m_codePlcSendReceiverError);
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd1 + m_istageNum, iPanelNum, TRUE);
				theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Gamma PanelID Error"), m_istageNum + 1, m_iPanelNum + iPanelNum));
				return;
			}
		}

		//Ch,Number,GAMMA,START,CELLID
		strSendMsg = CStringSupport::FormatString(_T("Ch,%d,GAMMA,START,%s"), m_iPanelNum + iPanelNum, theApp.m_lastGammaVec[m_istageNum][iPanelNum].m_cellId);
		theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strSendMsg, iPanelNum, m_istageNum);

		theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d PanelID [%s] Gamma Start"),
			m_istageNum + 1, m_iPanelNum + iPanelNum, theApp.m_lastGammaVec[m_istageNum][iPanelNum].m_cellId));

		GammaVecAdd(theApp.m_lastGammaVec[m_istageNum][iPanelNum].m_cellId, theApp.m_lastGammaVec[m_istageNum][iPanelNum].m_FpcID, iPanelNum, m_istageNum, m_iPanelNum + iPanelNum, PG_GAMMA, PGGammaTimer);
		
	}
	else
		theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Gamma Panel Exist Error"), m_istageNum + 1, m_iPanelNum + iPanelNum));

	m_csPgStatus.Unlock();
}

void CGammaThread::GammaContactOnStart()
{
	m_csPgStatus.Lock();

	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strSendMsg = _T("");

	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaStage1PanelExist1 + m_istageNum, ii))
		{
			theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + m_istageNum, ii, &m_codeReset);

			int iPanelIDAddr = ii == PanelNum1 ? eWordType_GammaStage1PanelID1 : eWordType_GammaStage1PanelID2;
			int iFpcIDAddr = ii == PanelNum1 ? eWordType_GammaStage1FpcID1 : eWordType_GammaStage1FpcID2;

			theApp.m_pEqIf->m_pMNetH->GetPanelData(iPanelIDAddr + m_istageNum, &pPanelData);
			theApp.m_lastGammaVec[m_istageNum][ii].m_cellId = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));
			theApp.m_pEqIf->m_pMNetH->GetFpcIdData(iFpcIDAddr + m_istageNum, &pFpcData);
			theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

			theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d PanelID [%s] Contact On Start"), 
				m_istageNum + 1, m_iPanelNum + ii, theApp.m_lastGammaVec[m_istageNum][ii].m_cellId));

			if (theApp.m_lastGammaVec[m_istageNum][ii].m_cellId.IsEmpty())
			{
				if (theApp.m_PanelTestStart)
				{
					CString strPanelID;
					strPanelID.Format(_T("TEST%d"), ii);
					theApp.m_lastGammaVec[m_istageNum][ii].m_cellId = strPanelID;
					theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID = strPanelID;
				}
				else
				{
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + m_istageNum, ii, &m_codePlcPanelError);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOnEnd1 + m_istageNum, OffSet_0, TRUE);

					theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Contact On PanelID Error"), m_istageNum + 1, m_iPanelNum + ii));
					return;
				}
			}

			//Ch,Number,CONTACT,CELLID
			strSendMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACT,%s"), m_iPanelNum + ii, theApp.m_lastGammaVec[m_istageNum][ii].m_cellId);
			theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strSendMsg, ii, m_istageNum);

			GammaVecAdd(theApp.m_lastGammaVec[m_istageNum][ii].m_cellId, theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID, ii, m_istageNum, m_iPanelNum + ii, PG_CONTACT_ON, PGContactOnTimer);

			theApp.m_bContact[m_iPanelNum + ii - 1] = TRUE;
			theApp.m_strContactPanelID[m_iPanelNum + ii - 1] = theApp.m_lastGammaVec[m_istageNum][ii].m_cellId;
		}
	}

	m_csPgStatus.Unlock();
}

void CGammaThread::GammaContactOffStart()
{
	m_csPgStatus.Lock();

	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strSendMsg = _T("");

	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaStage1PanelExist1 + m_istageNum, ii))
		{
			theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + m_istageNum, ii, &m_codeReset);

			int iPanelIDAddr = ii == PanelNum1 ? eWordType_GammaStage1PanelID1 : eWordType_GammaStage1PanelID2;
			int iFpcIDAddr = ii == PanelNum1 ? eWordType_GammaStage1FpcID1 : eWordType_GammaStage1FpcID2;

			theApp.m_pEqIf->m_pMNetH->GetPanelData(iPanelIDAddr + m_istageNum, &pPanelData);
			theApp.m_lastGammaVec[m_istageNum][ii].m_cellId = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));
			theApp.m_pEqIf->m_pMNetH->GetFpcIdData(iFpcIDAddr + m_istageNum, &pFpcData);
			theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

			theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Contact Off Start"), m_istageNum + 1, m_iPanelNum + ii));

			if (theApp.m_lastGammaVec[m_istageNum][ii].m_cellId.IsEmpty())
			{
				if (theApp.m_PanelTestStart)
				{
					CString strPanelID;
					strPanelID.Format(_T("TEST%d"), ii);
					theApp.m_lastGammaVec[m_istageNum][ii].m_cellId = strPanelID;
					theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID = strPanelID;
				}
				else
				{
					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + m_istageNum, ii, &m_codePlcPanelError);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOffEnd1 + m_istageNum, OffSet_0, TRUE);

					theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Contact Off PnaelID Error"), m_istageNum + 1, m_iPanelNum + ii));
					return;
				}
			}

			if (_ttoi(theApp.m_strPGName) == PG_MuhanZC)
				strSendMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,RESET"), m_iPanelNum + ii);
			else 
				strSendMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACTOFF,%s"), m_iPanelNum + ii, theApp.m_lastGammaVec[m_istageNum][ii].m_cellId);
			theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strSendMsg, ii, m_istageNum);

			GammaVecAdd(theApp.m_lastGammaVec[m_istageNum][ii].m_cellId, theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID, ii, m_istageNum, m_iPanelNum + ii, PG_CONTACT_OFF, PGContactOffTimer);
		}
	}

	m_csPgStatus.Unlock();
}

void CGammaThread::GammaNextStart()
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strSendMsg = _T("");
	
	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaStage1PanelExist1 + m_istageNum, ii))
		{
			int iPanelIDAddr = ii == PanelNum1 ? eWordType_GammaStage1PanelID1 : eWordType_GammaStage1PanelID2;
			int iFpcIDAddr = ii == PanelNum1 ? eWordType_GammaStage1FpcID1 : eWordType_GammaStage1FpcID2;

			theApp.m_pEqIf->m_pMNetH->GetPanelData(iPanelIDAddr + m_istageNum, &pPanelData);
			theApp.m_lastGammaVec[m_istageNum][ii].m_cellId = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));
			theApp.m_pEqIf->m_pMNetH->GetFpcIdData(iFpcIDAddr + m_istageNum, &pFpcData);
			theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

			//Ch,Number,KEY,NEXT
			strSendMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,NEXT"), m_iPanelNum + ii);
			theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strSendMsg, ii, m_istageNum);

			theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Next Start"), m_istageNum + 1, m_iPanelNum + ii));

			GammaVecAdd(theApp.m_lastGammaVec[m_istageNum][ii].m_cellId, theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID, ii, m_istageNum, m_iPanelNum + ii, PG_PATTERN_NEXT, PGNextTimer);
		}
	}
}

void CGammaThread::GammaBackStart()
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strSendMsg = _T("");

	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaStage1PanelExist1 + m_istageNum, ii))
		{
			int iPanelIDAddr = ii == PanelNum1 ? eWordType_GammaStage1PanelID1 : eWordType_GammaStage1PanelID2;
			int iFpcIDAddr = ii == PanelNum1 ? eWordType_GammaStage1FpcID1 : eWordType_GammaStage1FpcID2;

			theApp.m_pEqIf->m_pMNetH->GetPanelData(iPanelIDAddr + m_istageNum, &pPanelData);
			theApp.m_lastGammaVec[m_istageNum][ii].m_cellId = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));
			theApp.m_pEqIf->m_pMNetH->GetFpcIdData(iFpcIDAddr + m_istageNum, &pFpcData);
			theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

			//Ch,Number,KEY,BACK
			strSendMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,BACK"), m_iPanelNum + ii);
			theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strSendMsg, ii, m_istageNum);

			theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d Back Start"), m_istageNum + 1, m_iPanelNum + ii));

			GammaVecAdd(theApp.m_lastGammaVec[m_istageNum][ii].m_cellId, theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID, ii, m_istageNum, m_iPanelNum + ii, PG_PATTERN_BACK, PGBackTimer);
		}
	}
}

void CGammaThread::GammaPIDCheckStart()
{
	m_csPgStatus.Lock();

	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strSendMsg = _T("");

	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1PIDCheckResult + m_istageNum, ii, &m_codeReset);

		int iPanelIDAddr = ii == PanelNum1 ? eWordType_GammaStage1PanelID1 : eWordType_GammaStage1PanelID2;
		int iFpcIDAddr = ii == PanelNum1 ? eWordType_GammaStage1FpcID1 : eWordType_GammaStage1FpcID2;

		theApp.m_pEqIf->m_pMNetH->GetPanelData(iPanelIDAddr + m_istageNum, &pPanelData);
		theApp.m_lastGammaVec[m_istageNum][ii].m_cellId = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));
		theApp.m_pEqIf->m_pMNetH->GetFpcIdData(iFpcIDAddr + m_istageNum, &pFpcData);
		theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

		theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d PanelID [%s] PID Check Start"),
			m_istageNum + 1, m_iPanelNum + ii, theApp.m_lastGammaVec[m_istageNum][ii].m_cellId));

		if (theApp.m_lastGammaVec[m_istageNum][ii].m_cellId.IsEmpty())
		{
			if (theApp.m_PanelTestStart)
			{
				CString strPanelID;
				strPanelID.Format(_T("TEST%d"), ii);
				theApp.m_lastGammaVec[m_istageNum][ii].m_cellId = strPanelID;
				theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID = strPanelID;
			}
			else
			{
				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1PIDCheckResult + m_istageNum, ii, &m_codePlcPanelError);
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1PIDCheckEnd + m_istageNum, OffSet_0, TRUE);

				theApp.m_PgSocketManager[m_iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[Stage%d] Ch %d PID Check PanelID Error"), m_istageNum + 1, m_iPanelNum + ii));
				return;
			}
		}
		CString strProjec_ID = theApp.GetProjectID(theApp.m_lastGammaVec[m_istageNum][ii].m_cellId);

		//Ch,Number,PID,PID CODE
		strSendMsg = CStringSupport::FormatString(_T("Ch,%d,PID,%s"), m_iPanelNum + ii, strProjec_ID);
		theApp.m_PgSocketManager[m_iPcNum].SendPGMessage(strSendMsg, ii, m_istageNum);

		GammaVecAdd(theApp.m_lastGammaVec[m_istageNum][ii].m_cellId, theApp.m_lastGammaVec[m_istageNum][ii].m_FpcID, ii, m_istageNum, m_iPanelNum + ii, PG_PID_CHECK, PGContactOnTimer);
	}

	m_csPgStatus.Unlock();
}

void CGammaThread::GammaVecAdd(CString strPanelID, CString strFpcID, int iPanelNum, int iStageNum, int iChNum, int iGammaOrder, int iTimerNum)
{
	m_csVecData.Lock();

	InspResult panelData;
	panelData.Reset();

	panelData.m_bInspStart = TRUE;
	panelData.m_iStatus = iGammaOrder;
	panelData.m_cellId = strPanelID;
	panelData.m_FpcID = strFpcID;
	panelData.m_iPanelNum = iPanelNum;
	panelData.m_iGammaRunChNum = iChNum;

	if (iTimerNum != MaxTimerCount)
	{
		if (theApp.m_iTimer[iTimerNum] == 0)
			panelData.time_check.SetCheckTime(15000);
		else
			panelData.time_check.SetCheckTime(theApp.m_iTimer[iTimerNum] * 1000);

		panelData.time_check.StartTimer();
	}

	theApp.m_lastGammaVec[iStageNum][iPanelNum] = panelData;

	m_csVecData.Unlock();
}

UINT CGammaThread::GammaThreadProc(LPVOID pParam)
{
	CGammaThread* pThis = reinterpret_cast<CGammaThread*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;
}

BOOL CGammaThread::CreateTask(){
	BOOL bRet = TRUE;
	m_pThreadGamma = ::AfxBeginThread(GammaThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadGamma)
		bRet = FALSE;
	m_pThreadGamma->m_bAutoDelete = FALSE;
	m_pThreadGamma->ResumeThread();
	return bRet;
}

void CGammaThread::CloseTask()
{
	if (m_pThreadGamma != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadGamma->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadGamma->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadGamma->m_hThread, 1L);
				theApp.m_PgLog->LOG_INFO(_T("Terminate Gamma Thread"));
			}
		}
		delete m_pThreadGamma;
		m_pThreadGamma = NULL;

	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}
#endif