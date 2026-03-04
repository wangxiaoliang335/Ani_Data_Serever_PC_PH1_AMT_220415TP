
#define _220309Version
#include "stdafx.h"
#if _SYSTEM_AMTAFT_
#include "DlgMainLog.h"
#include "ManualThread.h"
#include "DataInfo.h"

CManualThread::CManualThread()
{
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bOpvFirstStatus[0] = TRUE;
	m_bOpvFirstStatus[1] = TRUE;

	for (int ii = 0; ii < ManualStageMaxCount; ii++)
		theApp.m_VecManualStage[ii].resize(2);
}

CManualThread::~CManualThread()
{
}

void CManualThread::ThreadRun()
{	
	while (::WaitForSingleObject(m_hQuit, 50) != WAIT_OBJECT_0)
	{
		theApp.m_PgConectStatus[PgServer_2] = theApp.m_PgSocketManager[PgServer_2].getConectCheck();
		theApp.m_PgConectStatus[PgServer_3] = theApp.m_PgSocketManager[PgServer_3].getConectCheck();
		theApp.m_OpvConectStatus[CH_1] = theApp.m_OpvSocketManager[CH_1].getConectCheck();
		theApp.m_OpvConectStatus[CH_2] = theApp.m_OpvSocketManager[CH_2].getConectCheck();

		//if (theApp.m_bAllPassMode)
		//	continue;

		for (int ii = 0; ii < ChMaxCount; ii++)
		{
			if (theApp.m_OpvConectStatus[ii] == TRUE)
			{
				if (m_bOpvFirstStatus[ii] == TRUE)
				{
					m_bOpvFirstStatus[ii] = FALSE;
					time_check[ii].SetCheckTime(60000);
					time_check[ii].StartTimer();
					OpvCommCheckMethod(ii);
				}

				if (time_check[ii].IsTimeOver())
				{
					time_check[ii].StartTimer();
					OpvCommCheckMethod(ii);
					if (theApp.m_OpvSocketManager[ii].m_OpvCheckCount > 5)
						theApp.m_OpvSocketManager[ii].OpvLogMessage(CStringSupport::FormatString(_T("Opv PC%d Client Drop"), ii));
					
					theApp.m_OpvSocketManager[ii].m_OpvCheckCount++;
				}
			}
			
		}

		for (int ii = 0; ii < ChMaxCount; ii++)
		{
			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAContactPlcDataSend + ii, OffSet_0))
				ManualStagePanelCheck(ii, ContactPanelCheck);
			else
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactPcDataReceived + ii, OffSet_0, FALSE);


			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAPreGammaPlcDataSend + ii, OffSet_0))
				ManualStagePanelCheck(ii, PreGammaPanelCheck);
			else
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaPcDataReceived + ii, OffSet_0, FALSE);


			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageATouchPlcDataSend + ii, OffSet_0))
				ManualStagePanelCheck(ii, TouchPanelCheck);
			else
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchPcDataReceived+ ii, OffSet_0, FALSE);


			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageA_OperatorViewPlcDataSend + ii, OffSet_0))
				ManualStagePanelCheck(ii, OperatorViewPanelCheck);
			else
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OperatorViewPcDataReceived+ ii, OffSet_0, FALSE);

			if (theApp.m_PgConectStatus[PgServer_2] || theApp.m_PgConectStatus[PgServer_3] || theApp.m_PgPassMode)
			{
				m_bStart[ii] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAContactOnStart + ii, OffSet_0);
				if (m_bStart[ii] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOnEnd + ii, OffSet_0, FALSE);
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOnResult + ii, &m_codeReset);
				}

				if (m_bStart[ii] == !m_bStartFlag[ManualStageContactOn][ii])
				{
					m_bStartFlag[ManualStageContactOn][ii] = m_bStart[ii];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Manual Stage %d Contact On Start Flag [%s]"), ii + 1, m_bStartFlag[ManualStageContactOn] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bStartFlag[ManualStageContactOn][ii] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOnEnd + ii, OffSet_0, FALSE);
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOnResult + ii, &m_codeReset);
						ManualStageContactOnStart(ii, PgServer_2 + ii, PG_CH_17 + ii);
					}
				}

				m_bStart[ii] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAContactOffStart + ii, OffSet_0);

				if (m_bStart[ii] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOffEnd + ii, OffSet_0, FALSE);
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOffResult + ii, &m_codeReset);
				}

				if (m_bStart[ii] == !m_bStartFlag[ManualStageContactOff][ii])
				{
					m_bStartFlag[ManualStageContactOff][ii] = m_bStart[ii];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Manual Stage %d Contact Off Start Flag [%s]"), ii + 1, m_bStartFlag[ManualStageContactOff] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bStartFlag[ManualStageContactOff][ii] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOffEnd + ii, OffSet_0, FALSE);
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOffResult + ii, &m_codeReset);
						ManualStageContactOffStart(ii, PgServer_2 + ii, PG_CH_17 + ii);
					}
				}

				m_bStart[ii] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAContactNextStart + ii, OffSet_0);

				if (m_bStart[ii] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactNextEnd + ii, OffSet_0, FALSE);
				}

				if (m_bStart[ii] == !m_bStartFlag[ManualStageNext][ii])
				{
					m_bStartFlag[ManualStageNext][ii] = m_bStart[ii];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Manual Stage %d Pattern Next Start Flag [%s]"), ii + 1, m_bStartFlag[ManualStageNext] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bStartFlag[ManualStageNext][ii] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactNextEnd + ii, OffSet_0, FALSE);
						ManualStageNextStart(ii, PgServer_2 + ii, PG_CH_17 + ii);
					}
				}

				m_bStart[ii] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAContactBackStart + ii, OffSet_0);

				if (m_bStart[ii] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactBackEnd + ii, OffSet_0, FALSE);
				}

				if (m_bStart[ii] == !m_bStartFlag[ManualStageBack][ii])
				{
					m_bStartFlag[ManualStageBack][ii] = m_bStart[ii];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Manual Stage %d Pattern Back Start Flag [%s]"), ii + 1, m_bStartFlag[ManualStageBack] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bStartFlag[ManualStageBack][ii] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactBackEnd + ii, OffSet_0, FALSE);
						ManualStageBackStart(ii, PgServer_2 + ii, PG_CH_17 + ii);
					}
				}

				m_bStart[ii] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAPreGammaStart + ii, OffSet_0);

				if (m_bStart[ii] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaEnd + ii, OffSet_0, FALSE);
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAGammaResult + ii, &m_codeReset);
				}

				if (m_bStart[ii] == !m_bStartFlag[ManualStagePreGamma][ii])
				{
					m_bStartFlag[ManualStagePreGamma][ii] = m_bStart[ii];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Manual Stage %d Pre Gamma Start Flag [%s]"), ii + 1, m_bStartFlag[ManualStagePreGamma] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bStartFlag[ManualStagePreGamma][ii] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaEnd + ii, OffSet_0, FALSE);
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAGammaResult + ii, &m_codeReset);
						ManualStagePreGammaStart(ii, PgServer_2 + ii, PG_CH_17 + ii);
					}
				}
			}

			if (theApp.m_TpConectStatus || theApp.m_TpPassMode)
			{
				m_bStart[ii] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageATouchStart + ii, OffSet_0);

				if (m_bStart[ii] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchEnd + ii, OffSet_0, FALSE);
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageATouchResult + ii, &m_codeReset);
				}

				if (m_bStart[ii] == !m_bStartFlag[ManualStageTouch][ii])
				{
					m_bStartFlag[ManualStageTouch][ii] = m_bStart[ii];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Manual Stage %d Touch Start Flag [%s]"), ii + 1, m_bStartFlag[ManualStageTouch] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bStartFlag[ManualStageTouch][ii] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchEnd + ii, OffSet_0, FALSE);
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageATouchResult + ii, &m_codeReset);
						ManualStageTouchStart(ii, PG_CH_17 + ii);
					}
				}
			}

			if (theApp.m_OpvConectStatus[CH_1] || theApp.m_OpvConectStatus[CH_2] || theApp.m_OpvPassMode)
			{
				m_bStart[ii] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageA_OperatorViewStart + ii, OffSet_0);
				
				if (m_bStart[ii] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OperatorViewEnd + ii, OffSet_0, FALSE);
					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAOperatorViewResult + ii, &m_codeReset);
				}

				if (m_bStart[ii] == !m_bStartFlag[ManualStageOperatorView][ii])
				{
					m_bStartFlag[ManualStageOperatorView][ii] = m_bStart[ii];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Manual Stage %d Operator View Start Flag [%s]"), ii + 1, m_bStartFlag[ManualStageOperatorView] == FALSE ? _T("FALSE") : _T("TRUE")));

					if (m_bStartFlag[ManualStageOperatorView][ii] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OperatorViewEnd + ii, OffSet_0, FALSE);
						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAOperatorViewResult + ii, &m_codeReset);
						ManualStageOperatorViewStart(ii);
					}
				}
			}

			for (int ii = 0; ii < ManualStageMaxCount; ii++)
			{
				for (auto &InspResult : theApp.m_VecManualStage[ii])
				{
					if (InspResult.m_bInspStart == TRUE)
					{
						if (InspResult.time_check.IsTimeOver())
						{
							int iPgServerNum = InspResult.m_iPanelNum / 16;
							if (iPgServerNum == 1)
								iPgServerNum = abs(InspResult.m_iPanelNum - PG_CH_17);

							switch (InspResult.m_iStatus)
							{
							case ManualStageContactOn:
								if (theApp.m_PgPassMode)
									theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOnResult + iPgServerNum, &m_codeOk);
								else
									theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOnResult + iPgServerNum, &m_codeTimeOut);

								theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOnEnd + iPgServerNum, OffSet_0, TRUE);
								theApp.m_PgSocketManager[iPgServerNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Panel [%s] Contact On Time Out"), ULD_PG_IndexName[InspResult.m_iPanelNum], InspResult.m_cellId));
								theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] Panel [%s] Contact On Time out!!!"), ULD_PG_IndexName[InspResult.m_iPanelNum], InspResult.m_cellId));
								break;
							case ManualStageContactOff:
								if (theApp.m_PgPassMode)
									theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOffResult + iPgServerNum, &m_codeOk);
								else
									theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOffResult + iPgServerNum, &m_codeTimeOut);

								theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOffEnd + iPgServerNum, OffSet_0, TRUE);
								theApp.m_PgSocketManager[iPgServerNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Panel [%s] Contact Off Time Out"), ULD_PG_IndexName[InspResult.m_iPanelNum], InspResult.m_cellId));
								theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] Panel [%s] Contact Off Time out!!!"), ULD_PG_IndexName[InspResult.m_iPanelNum], InspResult.m_cellId));
								break;
							case ManualStageNext:
								theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactNextEnd + iPgServerNum, OffSet_0, TRUE);
								theApp.m_PgSocketManager[iPgServerNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Panel [%s] Pattern Next Time Out"), ULD_PG_IndexName[InspResult.m_iPanelNum], InspResult.m_cellId));
								theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] Panel [%s] Pattern Next Time out!!!"), ULD_PG_IndexName[InspResult.m_iPanelNum], InspResult.m_cellId));
								break;
							case ManualStageBack:
								theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactBackEnd + iPgServerNum, OffSet_0, TRUE);
								theApp.m_PgSocketManager[iPgServerNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Panel [%s] Pattern Back Time Out"), ULD_PG_IndexName[InspResult.m_iPanelNum], InspResult.m_cellId));
								theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] Panel [%s] Pattern Back Time out!!!"), ULD_PG_IndexName[InspResult.m_iPanelNum], InspResult.m_cellId));
								break;
							case ManualStagePreGamma:
								if (theApp.m_PgPassMode)
									theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAGammaResult + iPgServerNum, &m_codeOk);
								else
									theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAGammaResult + iPgServerNum, &m_codeTimeOut);

								theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaEnd + iPgServerNum, OffSet_0, TRUE);
								theApp.m_PgSocketManager[iPgServerNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Panel [%s] PreGamma Time Out"), ULD_PG_IndexName[InspResult.m_iPanelNum], InspResult.m_cellId));
								theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] Panel [%s] PreGamma Time out!!!"), ULD_PG_IndexName[InspResult.m_iPanelNum], InspResult.m_cellId));
								break;
							case ManualStageTouch:
								if (theApp.m_PgPassMode)
									theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageATouchResult + iPgServerNum, &m_codeOk);
								else
									theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageATouchResult + iPgServerNum, &m_codeTimeOut);

								theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchEnd + iPgServerNum, OffSet_0, TRUE);
								theApp.m_TpSocketManager.TpLogMessage(CStringSupport::FormatString(_T("[%s] Panel [%s] TP Time Out"), ULD_PG_IndexName[iPgServerNum], InspResult.m_cellId));
								theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] Panel [%s] TP Time out!!!"), ULD_PG_IndexName[iPgServerNum], InspResult.m_cellId));
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
}

void CManualThread::ManualStagePanelCheck(int iChNum, int iOrderNum)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel, strFpcID;

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_MStageAPanel + iChNum, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_MStageAFpcID + iChNum, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strFpcID.IsEmpty() == FALSE)
	{
		switch (iOrderNum)
		{
		case ContactPanelCheck: theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactPcDataReceived + iChNum, OffSet_0, TRUE); break;
		case TouchPanelCheck: theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchPcDataReceived + iChNum, OffSet_0, TRUE); break;
		case PreGammaPanelCheck: theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaPcDataReceived + iChNum, OffSet_0, TRUE); break;
		case OperatorViewPanelCheck: theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OperatorViewPcDataReceived + iChNum, OffSet_0, TRUE); break;
		}
	}
}

void CManualThread::ManualStageContactOnStart(int iNum, int iPcNum, int iChNum)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel = _T(""), strFpcID = _T("");

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_MStageAPanel + iNum, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_MStageAFpcID + iNum, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), iChNum);
			strFpcID.Format(_T("TEST%d"), iChNum);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOnResult + iNum, &m_codePlcSendReceiverError);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOnEnd + iNum, OffSet_0, TRUE);
			theApp.m_PgSocketManager[iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Contact On No Panel ID"), ULD_PG_IndexName[iNum]));
			return;
		}
	}
	if (strPanel.IsEmpty())
		strPanel = strFpcID;

	theApp.m_PgSocketManager[iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Contact On Start Panel [%s][%s]"), ULD_PG_IndexName[iNum], strPanel, strFpcID));

	ManualStageVecAdd(strPanel, strFpcID, iChNum, ManualStageContactOn, PGContactOnTimer, iNum);
	theApp.m_bContact[iChNum] = TRUE;
	theApp.m_strContactPanelID[iChNum] = strPanel;

	CString strMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACT,%s"), iChNum + 1, strPanel);
	theApp.m_PgSocketManager[iPcNum].SendPGMessage(strMsg, iChNum + 1);

	CDataInfo dataInfo;
	CString strPath;
	CString strMsg2;
	strPath = CStringSupport::FormatString(_T("%s%s\\%s\\%s.txt"), DFS_SHARE_OPV_PATH, GetDateString2(), strPanel, strPanel);

	dataInfo.LoadDataInfo(strPath);

	static USHORT m_codeReset = 0;
	static USHORT m_codeOk = 1;
	static USHORT m_codeFail = 2;
	static USHORT m_codeContactFail = 3;
	static USHORT m_codeResponseError = 4;
	static USHORT m_codeDfsFileError = 8;

	//if (_ttoi(dataInfo.m_Panel_Info.strPreGammaContactStatus) == m_codeContactFail)
	//{
	//	strMsg2 = CStringSupport::FormatString(_T("%d,%d"), MC_NG_PANEL, _CONTACT);
	//	theApp.m_OpvSocketManager[iChNum].SendOpvMessage(strMsg, iChNum, MC_NG_PANEL);
	//
	//}
	//else if (_ttoi(dataInfo.m_Panel_Info.strTpResult) == m_codeFail)
	//{
	//	strMsg2 = CStringSupport::FormatString(_T("%d,%d"), MC_NG_PANEL, _DOT);
	//	theApp.m_OpvSocketManager[iChNum].SendOpvMessage(strMsg, iChNum, MC_NG_PANEL);
	//
	//}
	//else if (_ttoi(dataInfo.m_Panel_Info.strPreGammaContactStatus) == m_codeFail)
	//{
	//	strMsg2 = CStringSupport::FormatString(_T("%d,%d"), MC_NG_PANEL, _PREGAMMA);
	//	theApp.m_OpvSocketManager[iChNum].SendOpvMessage(strMsg, iChNum, MC_NG_PANEL);
	//
	//}
	//else if (dataInfo.m_Panel_Info.strVisionResult == _T("N") && dataInfo.m_Panel_Info.strViewingResult == _T("N"))
	//{
	//	strMsg2 = CStringSupport::FormatString(_T("%d,%d"), MC_NG_PANEL, _AOI);
	//	theApp.m_OpvSocketManager[iChNum].SendOpvMessage(strMsg, iChNum, MC_NG_PANEL);
	//
	//}
	//else if (dataInfo.m_Panel_Info.strVisionResult == _T("N"))
	//{
	//	strMsg2 = CStringSupport::FormatString(_T("%d,%d"), MC_NG_PANEL, _AOI);
	//	theApp.m_OpvSocketManager[iChNum].SendOpvMessage(strMsg, iChNum, MC_NG_PANEL);
	//
	//}
	//else if (dataInfo.m_Panel_Info.strViewingResult == _T("N"))
	//{
	//	strMsg2 = CStringSupport::FormatString(_T("%d,%d"), MC_NG_PANEL, _AOI);
	//	theApp.m_OpvSocketManager[iChNum].SendOpvMessage(strMsg, iChNum, MC_NG_PANEL);
	//
	//}
	//else
	//{
	//	strMsg2 = CStringSupport::FormatString(_T("%d,%d"), MC_NG_PANEL, _AOI);
	//	theApp.m_OpvSocketManager[iChNum].SendOpvMessage(strMsg, iChNum, MC_NG_PANEL);
	//
	//}
}

void CManualThread::ManualStageContactOffStart(int iNum, int iPcNum, int iChNum)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel = _T(""), strFpcID = _T("");

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_MStageAPanel + iNum, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_MStageAFpcID + iNum, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), iChNum);
			strFpcID.Format(_T("TEST%d"), iChNum);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOffResult + iNum, &m_codePlcSendReceiverError);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOffEnd + iNum, OffSet_0, TRUE);
			theApp.m_PgSocketManager[iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Contact Off No Panel ID"), ULD_PG_IndexName[iNum]));
			return;
		}
	}
	if (strPanel.IsEmpty())
		strPanel = strFpcID;

	theApp.m_PgSocketManager[iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Contact Off Start"), ULD_PG_IndexName[iNum]));

	ManualStageVecAdd(strPanel, strFpcID, iChNum, ManualStageContactOff, PGContactOffTimer, iNum);


	CString strMsg;
	if (_ttoi(theApp.m_strPGName) == PG_MuhanZC)
		strMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,RESET"), iChNum + 1);
	else
		strMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACTOFF,%s"), iChNum + 1, strPanel);

	theApp.m_PgSocketManager[iPcNum].SendPGMessage(strMsg, iChNum + 1);
}

void CManualThread::ManualStageNextStart(int iNum, int iPcNum, int iChNum)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel = _T(""), strFpcID = _T("");

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_MStageAPanel + iNum, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_MStageAFpcID + iNum, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), iChNum);
			strFpcID.Format(_T("TEST%d"), iChNum);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactNextEnd + iNum, OffSet_0, TRUE);
			theApp.m_PgSocketManager[iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Pattern Next No Panel ID"), ULD_PG_IndexName[iNum]));
			return;
		}
	}
	if (strPanel.IsEmpty())
		strPanel = strFpcID;

	theApp.m_PgSocketManager[iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Pattern Next Start Panel [%s][%s]"), ULD_PG_IndexName[iNum], strPanel, strFpcID));

	ManualStageVecAdd(strPanel, strFpcID, iChNum, ManualStageNext, PGNextTimer, iNum);

	CString strMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,NEXT"), iChNum +1);
	theApp.m_PgSocketManager[iPcNum].SendPGMessage(strMsg, iChNum + 1);
}

void CManualThread::ManualStageBackStart(int iNum, int iPcNum, int iChNum)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel = _T(""), strFpcID = _T("");

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_MStageAPanel + iNum, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_MStageAFpcID + iNum, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), iChNum);
			strFpcID.Format(_T("TEST%d"), iChNum);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactBackEnd + iNum, OffSet_0, TRUE);
			theApp.m_PgSocketManager[iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Pattern Back No Panel ID"), ULD_PG_IndexName[iNum]));
			return;
		}
	}
	if (strPanel.IsEmpty())
		strPanel = strFpcID;

	theApp.m_PgSocketManager[iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] Pattern Back Start Panel [%s][%s]"), ULD_PG_IndexName[iNum], strPanel, strFpcID));

	ManualStageVecAdd(strPanel, strFpcID, iChNum, ManualStageBack, PGBackTimer, iNum);

	CString strMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,BACK"), iChNum + 1);
	theApp.m_PgSocketManager[iPcNum].SendPGMessage(strMsg, iChNum + 1);
}

void CManualThread::ManualStagePreGammaStart(int iNum, int iPcNum, int iChNum)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel = _T(""), strFpcID = _T("");

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_MStageAPanel + iNum, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_MStageAFpcID + iNum, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), iChNum);
			strFpcID.Format(_T("TEST%d"), iChNum);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAGammaResult + iNum, &m_codePlcSendReceiverError);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaEnd + iNum, OffSet_0, TRUE);
			theApp.m_PgSocketManager[iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] PreGamma No Panel ID"), ULD_PG_IndexName[iNum]));
			return;
		}
	}
	if (strPanel.IsEmpty())
		strPanel = strFpcID;

	theApp.m_PgSocketManager[iPcNum].PgLogMessage(CStringSupport::FormatString(_T("[%s] PreGamma Start Panel [%s][%s]"), ULD_PG_IndexName[iNum], strPanel, strFpcID));

	ManualStageVecAdd(strPanel, strFpcID, iChNum, ManualStagePreGamma, PGGammaTimer, iNum);

	CString strMsg = CStringSupport::FormatString(_T("Ch,%d,PREGAMMA,START,%s"), iChNum + 1, strPanel);
	theApp.m_PgSocketManager[iPcNum].SendPGMessage(strMsg, iChNum + 1);
}

void CManualThread::ManualStageTouchStart(int iNum, int iChNum)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanel = _T(""), strFpcID = _T("");

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_MStageAPanel + iNum, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_MStageAFpcID + iNum, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), iChNum);
			strFpcID.Format(_T("TEST%d"), iChNum);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageATouchResult + iNum, &m_codePlcSendReceiverError);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchEnd + iNum, OffSet_0, TRUE);
			theApp.m_TpSocketManager.TpLogMessage(CStringSupport::FormatString(_T("[%s] TP No Panel ID"), ULD_PG_IndexName[iNum]));
			return;
		}
	}
	if (strPanel.IsEmpty())
		strPanel = strFpcID;

#ifndef _220309Version
	//if (theApp.m_iMachineType == SetAMT)
	//{
	//	int iPgServerNum = iChNum == PG_CH_17 ? PgServer_2 : PgServer_3;
	//	CString strMsg = CStringSupport::FormatString(_T("Ch,%d,TURNOFF"), iChNum + 1);
	//	theApp.m_PgSocketManager[iPgServerNum].SendPGMessage(strMsg, iChNum + 1);
	//}
#endif

	theApp.m_TpSocketManager.TpLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d TP Start Panel [%s][%s]"), ULD_PG_IndexName[iNum], iChNum + 1, strPanel, strFpcID));

	ManualStageVecAdd(strPanel, strFpcID, iChNum, ManualStageTouch, PGTPTimer, iNum);

	theApp.m_TpSocketManager.SendTPMessage(iChNum, TP_SendPanelID, strPanel);
	Delay(20);
	theApp.m_TpSocketManager.SendTPMessage(iChNum, TP_InspStart);
}

void CManualThread::ULDInspectDataParser(int iPanelNum, int iCommand, CString strPanelID, CString strFpcID)
{
	theApp.m_PlcThread->m_csData.Lock();
	UnloaderDataStatus pDataStatus;
	DataStatusItem dataItem;
	CString strCell_ID, strFpc_ID;
	int iData = iCommand * 2;
	int iChNum = iPanelNum;
	dataItem.Reset();


	
	strCell_ID = strPanelID;
	strFpc_ID = strFpcID;


	
	if (iCommand == Data_BufferTrayIn)
	{
		theApp.m_ULDshiftProduction[iChNum].m_BufferTrayResult[theApp.m_lastShiftIndex]--;
		theApp.m_ULDUiShiftProduction[iChNum].m_BufferTrayResult[theApp.m_lastShiftIndex]--;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_BufferTrayResult[theApp.m_lastShiftIndex]--;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_BufferTrayResult[theApp.m_lastShiftIndex]--;
		dataItem.DataOutStatus = _T("Buffer Tray");
		dataItem.DataDefect = _T("BYPASS");
	}

	//버퍼 나가는것은 총카운터 집계 못하도록
	if (iCommand == Data_GoodOut || iCommand == Data_SampleOut)
	{
		theApp.m_ULDshiftProduction[iChNum].m_InspectionTotal[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShiftProduction[iChNum].m_InspectionTotal[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_InspectionTotal[theApp.m_lastShiftIndex]++;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_InspectionTotal[theApp.m_lastShiftIndex]++;
	}
	else if (iCommand == Data_NgOut)
	{
		theApp.m_ULDshiftProduction[iChNum].m_InspectionTotal[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShiftProduction[iChNum].m_InspectionTotal[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_InspectionTotal[theApp.m_lastShiftIndex]++;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_InspectionTotal[theApp.m_lastShiftIndex]++;
	}

	if (pDataStatus.m_ContactStatus == m_codeFail)
	{
		theApp.m_ULDshiftProduction[iChNum].m_ContactResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShiftProduction[iChNum].m_ContactResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_ContactResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_ContactResult[theApp.m_lastShiftIndex]++;

		dataItem.DataContactStatus = _T("NG");
	}
	else if (pDataStatus.m_ContactStatus == m_codeOk)
	{
		dataItem.DataContactStatus = _T("GOOD");
	}

	if (pDataStatus.m_FirstContactStatus == m_codeFail)
	{
		theApp.m_ULDshiftProduction[iChNum].m_FirstContactResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShiftProduction[iChNum].m_FirstContactResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_FirstContactResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_FirstContactResult[theApp.m_lastShiftIndex]++;

		dataItem.DataFirstContactStatus = _T("NG");
	}

	if (pDataStatus.m_ManualContactStatus == m_codeFail)
	{
		theApp.m_ULDshiftProduction[iChNum].m_ManualContactResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShiftProduction[iChNum].m_ManualContactResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_ManualContactResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_ManualContactResult[theApp.m_lastShiftIndex]++;
		dataItem.DataManualContactStatus = _T("NG");
	}
	else if (pDataStatus.m_ManualContactStatus == m_codeOk)
	{
		dataItem.DataManualContactStatus = _T("GOOD");
	}

	if (pDataStatus.m_PreGammaStatus == m_codeFail)
	{
		theApp.m_ULDshiftProduction[iChNum].m_GammaResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShiftProduction[iChNum].m_GammaResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_GammaResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_GammaResult[theApp.m_lastShiftIndex]++;
		dataItem.DataGammaStatus = _T("NG");
	}
	else if (pDataStatus.m_PreGammaStatus == m_codeOk)
	{
		dataItem.DataGammaStatus = _T("GOOD");
	}

	if (pDataStatus.m_TpStatus == m_codeFail)
	{
		theApp.m_ULDshiftProduction[iChNum].m_TouchResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShiftProduction[iChNum].m_TouchResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_TouchResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_TouchResult[theApp.m_lastShiftIndex]++;
		dataItem.DataTpStatus = _T("NG");
	}
	else if (pDataStatus.m_TpStatus == m_codeOk)
	{
		dataItem.DataTpStatus = _T("GOOD");
	}

	if (pDataStatus.m_OpvStatus == m_codeFail)
	{
		theApp.m_ULDshiftProduction[iChNum].m_OpvResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShiftProduction[iChNum].m_OpvResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_OpvResult[theApp.m_lastShiftIndex]++;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_OpvResult[theApp.m_lastShiftIndex]++;
		dataItem.DataOpvStatus = _T("NG");
	}
	else if (pDataStatus.m_OpvStatus == m_codeOk)
	{
		dataItem.DataOpvStatus = _T("GOOD");
	}

	if (pDataStatus.m_TryInsertStatus == m_TrayInsert)
	{
		theApp.m_ULDshiftProduction[iChNum].m_TrayInsertstatus[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShiftProduction[iChNum].m_TrayInsertstatus[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_TrayInsertstatus[theApp.m_lastShiftIndex]++;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_TrayInsertstatus[theApp.m_lastShiftIndex]++;
		dataItem.DataInStatus = _T("Tray");
	}
	else
	{
		theApp.m_ULDshiftProduction[iChNum].m_UpperInsertstatus[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShiftProduction[iChNum].m_UpperInsertstatus[theApp.m_lastShiftIndex]++;
		theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_UpperInsertstatus[theApp.m_lastShiftIndex]++;
		theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_UpperInsertstatus[theApp.m_lastShiftIndex]++;
		dataItem.DataInStatus = _T("Upper");
	}


	CFile   File;
	CString FileName, strString, strTemp, strShift;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	CString strEQPID = CStringSupport::FormatString(_T("%s%s"), theApp.m_strEqpId, theApp.m_strEqpNum);
	FileName.Format(_T("%s\\%s_%s_Cell_Log_%s.csv"), ULD_DATA_INSPECT_CSV_PATH, theApp.m_strCurrentToday, strEQPID, strShift);

	BOOL bOpen = FALSE;
	if (!File.Open(FileName, CFile::modeReadWrite | CFile::shareDenyNone))
	{
		if (File.Open(FileName, CFile::modeCreate | CFile::modeWrite))
		{
			bOpen = TRUE;

			strString.Format(_T("TIME,EQP_ID,CH,STAGE,CELLID,FPCID,DEFECT,1ST_CONTACT,2ND_CONTACT,MANUALCONTACT,PREGAMMA,TP,OPV,DATAIN,DATAOUT"));
			strString += "\r\n";
			File.Write(strString.GetBuffer(), strString.GetLength() * 2);
			strString.ReleaseBuffer();
		}

	}
	else bOpen = TRUE;

	if (bOpen) {
		CString strTime, strDefectString = _T("");
		CTime cTime;

		cTime = CTime::GetCurrentTime();
		File.SeekToEnd();

		strString.Format(_T("%s,%s,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s"),
			GetDateString6(),
			strEQPID,
			iChNum + 1,
			dataItem.DataStageNum,
			strCell_ID,
			strFpc_ID,
			dataItem.DataDefect,
			dataItem.DataFirstContactStatus,
			dataItem.DataContactStatus,
			dataItem.DataManualContactStatus,
			dataItem.DataGammaStatus,
			dataItem.DataTpStatus,
			dataItem.DataOpvStatus,
			dataItem.DataInStatus,
			dataItem.DataOutStatus);


		strString += "\r\n";

		int iLen = strString.GetLength();
		File.Write(strString.GetBuffer(), iLen * 2);
		strString.ReleaseBuffer();
		File.Close();
	}



	theApp.m_pDataStatusLog->LOG_INFO(CStringSupport::FormatString(_T("PANEL : %d STAGE : %s CELLID : %s FPCID : %s DEFECT: %s CONTACT: %d Manual 1ST_CONTACT :%d 2ND_CONTACT: %d PreGamma: %d TP: %d OPV: %d DATAIN : %d DATAOUT : %d"),
		iChNum + 1,
		dataItem.DataStageNum,
		strCell_ID,
		strFpc_ID,
		dataItem.DataDefect,
		pDataStatus.m_FirstContactStatus,
		pDataStatus.m_ContactStatus,
		pDataStatus.m_ManualContactStatus,
		pDataStatus.m_PreGammaStatus,
		pDataStatus.m_TpStatus,
		pDataStatus.m_OpvStatus,
		pDataStatus.m_TryInsertStatus,
		iCommand));

	theApp.ULDInspectionDataSave(theApp.m_lastShiftIndex);
	theApp.ULDInspectionTimeDataSave(theApp.m_lastShiftIndex);

	//theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GoodReportEnd + iCommand, iPanelNum, TRUE);

	theApp.m_PlcThread->m_csData.Unlock();
}

void CManualThread::ManualStageOperatorViewStart(int iChNum)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CDataInfo OpvInfo;
	CString strPanel = _T(""), strFpcID = _T("");
	BOOL bClear = FALSE;
	long bBufferTrayFlag = 0;

	theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_MStageAPanel + iChNum, &pPanelData);
	strPanel = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_MStageAFpcID + iChNum, &pFpcData);
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	theApp.m_pEqIf->m_pMNetH->GetPlcWordData(eWordType_MStageABufferTrayINFlag + iChNum, &bBufferTrayFlag);
	

	if (strFpcID.IsEmpty())
	{
		if (theApp.m_PanelTestStart)
		{
			strPanel.Format(_T("TEST%d"), iChNum);
			strFpcID.Format(_T("TEST%d"), iChNum);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAOperatorViewResult + iChNum, &m_codePlcSendReceiverError);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OperatorViewEnd + iChNum, OffSet_0, TRUE);
			theApp.m_OpvSocketManager[iChNum].OpvLogMessage(CStringSupport::FormatString(_T("[Ch%d] OPV No Panel ID"), iChNum + 1));
			return;
		}
	}
	if (strPanel.IsEmpty())
		strPanel = strFpcID;

	theApp.m_OpvSocketManager[iChNum].OpvLogMessage(CStringSupport::FormatString(_T("[%s] OPV Start Panel [%s][%s]"), ULD_PG_IndexName[iChNum], strPanel, strFpcID));
	CString strMsg;
	strMsg = CStringSupport::FormatString(_T("%d,%s,%s,%d"), MC_INSPECTION_START, strPanel, strFpcID, FALSE);

	theApp.m_OpvSocketManager[iChNum].SendOpvMessage(strMsg, iChNum, MC_INSPECTION_START);
	if(bBufferTrayFlag)
		ULDInspectDataParser(iChNum, 4, strPanel, strFpcID);
	
}

void CManualThread::ManualStageVecAdd(CString strPanel, CString strFpcID, int iChNum, int iMStageOrder, int iTimerNum, int iNum)
{

	InspResult panelData;
	panelData.Reset();

	panelData.m_bInspStart = TRUE;
	panelData.m_iStatus = iMStageOrder;
	panelData.m_cellId = strPanel;
	panelData.m_FpcID = strFpcID;
	panelData.m_iPanelNum = iChNum;

	if (iTimerNum != MaxTimerCount)
	{
		if (theApp.m_iTimer[iTimerNum] == 0)
			panelData.time_check.SetCheckTime(15000);
		else
			panelData.time_check.SetCheckTime(theApp.m_iTimer[iTimerNum] * 1000);

		panelData.time_check.StartTimer();
	}

	theApp.m_VecManualStage[iMStageOrder][iNum] = panelData;

}

UINT CManualThread::ManualThreadProc(LPVOID pParam)
{
	CManualThread* pThis = reinterpret_cast<CManualThread*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;

}

BOOL CManualThread::CreateTask(){
	BOOL bRet = TRUE;
	m_pThreadManual = ::AfxBeginThread(ManualThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadManual)
		bRet = FALSE;
	m_pThreadManual->m_bAutoDelete = FALSE;	/// ¾²·¹µå Á¾·á½Ã WaitForSingleObject Àû¿ëÀ§ÇØ...
	m_pThreadManual->ResumeThread();
	return bRet;
}

void CManualThread::CloseTask()
{
	if (m_pThreadManual != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadManual->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadManual->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadManual->m_hThread, 1L);
				theApp.m_pTraceLog->LOG_INFO(_T("Terminate Manual Thread"));
			}
		}
		delete m_pThreadManual;
		m_pThreadManual = NULL;

	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}


}

void CManualThread::OpvCommCheckMethod(int iNum)
{
	CString strCommand = CStringSupport::FormatString(_T("%d,%d"), MC_ARE_YOU_THERE, theApp.m_OpvSocketManager[iNum].m_OpvCheckCount);
	theApp.m_OpvSocketManager[iNum].SendOpvMessage(strCommand, iNum, MC_ARE_YOU_THERE);
}
#endif