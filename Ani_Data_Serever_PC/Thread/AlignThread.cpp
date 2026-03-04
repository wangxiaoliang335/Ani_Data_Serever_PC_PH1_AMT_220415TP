
#include "stdafx.h"
#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "DlgMainLog.h"
#else
#include "DlgGammaMain.h"
#endif
#include "AlignThread.h"

CAlignThread::CAlignThread(int iAlignType, int iAlignTypeNum, int iAlignNum)
{
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bFirstStatus = TRUE;
	m_iAlignType = iAlignType;
	m_iAlignTypeNum = iAlignTypeNum;
	m_iAlignNum = iAlignNum;
	m_positionResult = 0;
	m_AlignReverse = 0;

	theApp.m_lastAlignVec[m_iAlignNum].resize(2);

	for (int ii = 0; ii < 6; ii++)
	{
		m_bStartAlign[ii] = FALSE;
		m_bStartFlag[ii] = FALSE;
	}	
}

CAlignThread::~CAlignThread()
{
}

void CAlignThread::ThreadRun()
{	
	AlignResult alignResult;
	TrayCheckResult trayCheckResult;
	TrayLowerAlignResult trayLowerResult;

	while (::WaitForSingleObject(m_hQuit, 50) != WAIT_OBJECT_0)
	{
		theApp.m_AlignConectStatus[m_iAlignNum] = theApp.m_AlignSocketManager[m_iAlignNum]->getConectCheck();

		//if (theApp.m_bAllPassMode)
		//	continue;

		if (theApp.m_AlignConectStatus[m_iAlignNum])
		{
			if (m_bFirstStatus)
			{
				m_bFirstStatus = FALSE;
				time_check.SetCheckTime(60000);
				time_check.StartTimer();
				for (int ii = 0; ii < 6; ii++)
				{
					m_bStartAlign[ii] = FALSE;
					m_bStartFlag[ii] = FALSE;
				}
				AlignFirstCheckMethod();
			}

			if (theApp.m_PlcConectStatus == FALSE || theApp.m_ChangeModelAlign == TRUE)
				continue;

			if (time_check.IsTimeOver())
			{
				time_check.StartTimer();
				AlignCheckMethod();
				if (m_AlignCheckCount > 5)
				{
					theApp.m_AlignSocketManager[m_iAlignNum]->LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("Align PC %d Client Drop"), m_iAlignNum + 1));
				}
				m_AlignCheckCount++;
			}

			if (m_iAlignType == PatternAlign)
			{
				if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Align1DataPlcSend1 + m_iAlignTypeNum, OffSet_0))
					AlignPanelCheck(eWordType_Align1PanelID1, eBitType_Align1DataPcReceived1, m_iAlignTypeNum);
				else
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_Align1DataPcReceived1 + m_iAlignTypeNum, OffSet_0, FALSE);

				if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Align1DataPlcSend2 + m_iAlignTypeNum, OffSet_0))
					AlignPanelCheck(eWordType_Align1PanelID2, eBitType_Align1DataPcReceived2, m_iAlignTypeNum);
				else
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_Align1DataPcReceived2 + m_iAlignTypeNum, OffSet_0, FALSE);


				theApp.m_pEqIf->m_pMNetH->GetPlcWordData(eWordType_AlignPosition1 + m_iAlignTypeNum, &m_positionResult);
				theApp.m_pEqIf->m_pMNetH->GetPlcWordData(eWordType_AlignCount1 + m_iAlignTypeNum, &m_AlignCount);
#if _SYSTEM_GAMMA_
				theApp.m_pEqIf->m_pMNetH->GetPlcWordData(eWordType_AlignReverse, &m_AlignReverse);
#endif

				m_bStartFlag[0] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Align1TStart1+ m_iAlignTypeNum, OffSet_0);
				m_bStartFlag[1] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Align1TStart2+ m_iAlignTypeNum, OffSet_0);
				m_bStartFlag[2] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Align1XyStart1+ m_iAlignTypeNum, OffSet_0);
				m_bStartFlag[3] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Align1XyStart2+ m_iAlignTypeNum, OffSet_0);
				m_bStartFlag[4] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Align1LightOn1+ m_iAlignTypeNum, OffSet_0);
				m_bStartFlag[5] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Align1LightOn2+ m_iAlignTypeNum, OffSet_0);


				if (m_bStartAlign[4] == !m_bStartFlag[4])
				{
					m_bStartAlign[4] = m_bStartFlag[4];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Light On1 Flag [%s]"), AlignTypeName[m_iAlignType], m_bStartFlag[4] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[4] == TRUE)
					{
						AlignLightControl(PanelNum1);
					}
				}

				if (m_bStartAlign[5] == !m_bStartFlag[5])
				{
					m_bStartAlign[5] = m_bStartFlag[5];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Light On2 Flag [%s]"), AlignTypeName[m_iAlignType], m_bStartFlag[5] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[5] == TRUE)
					{
						AlignLightControl(PanelNum2);
					}
				}

				if (m_bStartFlag[0] == FALSE && m_bStartFlag[2] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_Align1Result1 + m_iAlignTypeNum, &alignResult);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_Align1End1 + m_iAlignTypeNum, OffSet_0, FALSE);
				}

				if (m_bStartFlag[1] == FALSE && m_bStartFlag[3] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_Align1Result2 + m_iAlignTypeNum, &alignResult);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_Align1End2 + m_iAlignTypeNum, OffSet_0, FALSE);
				}

				if (m_bStartAlign[0] == !m_bStartFlag[0])
				{
					m_bStartAlign[0] = m_bStartFlag[0];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Panel 1 Position T Start Flag [%s]"), AlignTypeName[m_iAlignType], m_bStartFlag[0] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[0] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_Align1Result1 + m_iAlignTypeNum, &alignResult);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_Align1End1 + m_iAlignTypeNum, OffSet_0, FALSE);
						AlignGrabMethod(PanelNum1, Align_Start_T, m_iAlignTypeNum);
					}
				}

				if (m_bStartAlign[1] == !m_bStartFlag[1])
				{
					m_bStartAlign[1] = m_bStartFlag[1];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Panel 2 Position T Start Flag [%s]"), AlignTypeName[m_iAlignType], m_bStartFlag[1] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[1] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_Align1Result2 + m_iAlignTypeNum, &alignResult);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_Align1End2 + m_iAlignTypeNum, OffSet_0, FALSE);
						AlignGrabMethod(PanelNum2, Align_Start_T, m_iAlignTypeNum);
					}
				}

				if (m_bStartAlign[2] == !m_bStartFlag[2])
				{
					m_bStartAlign[2] = m_bStartFlag[2];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Panel 1 Position XY Start Flag [%s]"), AlignTypeName[m_iAlignType], m_bStartFlag[2] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[2] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_Align1Result1 + m_iAlignTypeNum, &alignResult);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_Align1End1 + m_iAlignTypeNum, OffSet_0, FALSE);
						AlignGrabMethod(PanelNum1, Align_Start_XY, m_iAlignTypeNum);
					}
				}

				if (m_bStartAlign[3] == !m_bStartFlag[3])
				{
					m_bStartAlign[3] = m_bStartFlag[3];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Panel 2 Position XY Start Flag [%s]"), AlignTypeName[m_iAlignType], m_bStartFlag[3] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[3] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_Align1Result2 + m_iAlignTypeNum, &alignResult);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_Align1End2 + m_iAlignTypeNum, OffSet_0, FALSE);
						AlignGrabMethod(PanelNum2, Align_Start_XY, m_iAlignTypeNum);
					}
				}
						
			}
			else if (m_iAlignType == TrayCheck)
			{
				m_bStartFlag[0] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_TrayCheckStart1 + m_iAlignTypeNum, OffSet_0);
				m_bStartFlag[1] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_TrayCheckLightOn1 + m_iAlignTypeNum, OffSet_0);

				if (m_bStartAlign[1] == !m_bStartFlag[1])
				{
					m_bStartAlign[1] = m_bStartFlag[1];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Light On1 Flag [%s]"), AlignTypeName[m_iAlignType] ,m_bStartFlag[4] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[1] == TRUE)
					{
						AlignLightControl(PanelNum1);
					}
				}

				if (m_bStartFlag[0] == FALSE)
				{
					//theApp.m_pEqIf->m_pMNetH->SetTrayCheckResult(eWordType_TrayCheckResult1 + m_iAlignTypeNum, &trayCheckResult);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayCheckEnd1 + m_iAlignTypeNum, OffSet_0, FALSE);
				}

				if (m_bStartAlign[0] == !m_bStartFlag[0])
				{
					m_bStartAlign[0] = m_bStartFlag[0];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Start Flag [%s]"), AlignTypeName[m_iAlignType], m_bStartFlag[0] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[0] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetTrayCheckResult(eWordType_TrayCheckResult1 + m_iAlignTypeNum, &trayCheckResult);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayCheckEnd1 + m_iAlignTypeNum, OffSet_0, FALSE);
						TrayCheckNTrayAlignGrabMethod(PanelNum1);
					}
				}
			}
			else if (m_iAlignType == TrayAlign)
			{
				m_bStartFlag[0] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_TrayAlignStart1 + m_iAlignTypeNum, OffSet_0);
				m_bStartFlag[1] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_TrayAlignLightNo1 + m_iAlignTypeNum, OffSet_0);

				if (m_bStartAlign[1] == !m_bStartFlag[1])
				{
					m_bStartAlign[1] = m_bStartFlag[1];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Light On1 Flag [%s]"), AlignTypeName[m_iAlignType], m_bStartFlag[4] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[1] == TRUE)
					{
						AlignLightControl(PanelNum1);
					}
				}

				if (m_bStartFlag[0] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_TrayAlignResult1 + m_iAlignTypeNum, &alignResult);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayAlignEnd1 + m_iAlignTypeNum, OffSet_0, FALSE);
				}

				if (m_bStartAlign[0] == !m_bStartFlag[0])
				{
					m_bStartAlign[0] = m_bStartFlag[0];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Start Flag [%s]"), AlignTypeName[m_iAlignType], m_bStartFlag[0] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[0] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_TrayAlignResult1 + m_iAlignTypeNum, &alignResult);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayAlignEnd1 + m_iAlignTypeNum, OffSet_0, FALSE);
						TrayCheckNTrayAlignGrabMethod(PanelNum1);
					}
				}
			}
			else if (m_iAlignType == TrayLowerAlign)
			{
				if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_TrayLowerAlignDataSend1 + m_iAlignTypeNum, OffSet_0))
					AlignPanelCheck(eWordType_TrayLower1AlignPanelID1, eBitType_TrayLowerAlignDataReceived1, m_iAlignTypeNum);
				else
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayLowerAlignDataReceived1 + m_iAlignTypeNum, OffSet_0, FALSE);

				if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_TrayLowerAlignDataSend2 + m_iAlignTypeNum, OffSet_0))
					AlignPanelCheck(eWordType_TrayLower1AlignPanelID2, eBitType_TrayLowerAlignDataReceived2, m_iAlignTypeNum);
				else
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayLowerAlignDataReceived2 + m_iAlignTypeNum, OffSet_0, FALSE);

				theApp.m_pEqIf->m_pMNetH->GetPlcWordData(eWordType_TrayLowerAlignPosition1 + m_iAlignTypeNum, &m_positionResult);		// 1 =Tray , 2 = Lower
				theApp.m_pEqIf->m_pMNetH->GetPlcWordData(eWordType_TrayLowerAlignCount1 + m_iAlignTypeNum, &m_PanelCount);				// 1 = 왼 , 2 = 오 , 3 = 양

				m_bStartFlag[0] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_TrayLowerAlignStart1 + m_iAlignTypeNum, OffSet_0);

				if (m_bStartFlag[0] == FALSE)
				{
					theApp.m_pEqIf->m_pMNetH->SetTrayLowerAlignResult(eWordType_TrayLowerAlignResult1 + m_iAlignTypeNum, &trayLowerResult);
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayLowerAlignEnd1 + m_iAlignTypeNum, OffSet_0, FALSE);
				}

				if (m_bStartAlign[0] == !m_bStartFlag[0])
				{
					m_bStartAlign[0] = m_bStartFlag[0];
					theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("%s Start Flag [%s]"), AlignTypeName[m_iAlignType], m_bStartFlag[0] == FALSE ? _T("FALSE") : _T("TRUE")));
					if (m_bStartFlag[0] == TRUE)
					{
						theApp.m_pEqIf->m_pMNetH->SetTrayLowerAlignResult(eWordType_TrayLowerAlignResult1 + m_iAlignTypeNum, &trayLowerResult);
						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayLowerAlignEnd1 + m_iAlignTypeNum, OffSet_0, FALSE);
						TrayLowerAlignGrabMethod(PanelNum1, m_iAlignTypeNum);
					}
				}
			}


			for (int ii = 0; ii < theApp.m_lastAlignVec[m_iAlignNum].size(); ii++)
			{
				if (theApp.m_lastAlignVec[m_iAlignNum][ii].m_bInspStart == TRUE)
				{
					if (theApp.m_lastAlignVec[m_iAlignNum][ii].time_check.IsTimeOver())
					{
						AlignPLCResult(ii % 2, m_iAlignType, m_iAlignTypeNum, m_codeTimeOut, PLC_ResultValue[m_codeTimeOut], theApp.m_lastAlignVec[m_iAlignNum][ii].m_cellId);
						theApp.m_TimeOutLog->LOG_INFO(CStringSupport::FormatString(_T("[%s] Panel %d Align Time out"), theApp.m_lastAlignVec[m_iAlignNum][ii].m_strZoneName, theApp.m_lastAlignVec[m_iAlignNum][ii].m_iPanelNum));
						theApp.m_lastAlignVec[m_iAlignNum][ii].Reset();
					}
				}
			}

		}
		else
		{
			theApp.m_AlignPCStatus[m_iAlignNum] = FALSE;
			m_bFirstStatus = TRUE;
		}

	}
}

void CAlignThread::AlignLightControl(int iPnaulNum)
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%d"), MC_ALIGN_LIGHT_CONTROL, iPnaulNum);
	theApp.m_AlignSocketManager[m_iAlignNum]->SocketSendto(m_iAlignNum, sendMsg, MC_ALIGN_LIGHT_CONTROL);
}

void CAlignThread::AlignPanelCheck(int iPanelAddr, int iReceivedAddr, int iAlignNum)
{
	FpcIDData pFpcData;
	CString strPanel, strFpcID;

	theApp.m_pEqIf->m_pMNetH->GetFpcIdData(iPanelAddr + iAlignNum, &pFpcData);
	
	strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

	if (theApp.m_PanelTestStart == TRUE)
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(iReceivedAddr + iAlignNum, 0, TRUE);
	else if (strFpcID.GetLength() > 0)
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(iReceivedAddr + iAlignNum, 0, TRUE);
}

void CAlignThread::AlignFirstCheckMethod()
{
	CString strCommand = CStringSupport::FormatString(_T("%d,%d"), MC_ARE_YOU_THERE, m_AlignCheckCount);
	theApp.m_AlignSocketManager[m_iAlignNum]->SocketSendto(m_iAlignNum, strCommand, MC_ARE_YOU_THERE);
	Delay(200, TRUE);
	
	if (theApp.m_CreateModelAlign)
	{
		strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_MODEL_CREATE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
		theApp.m_AlignSocketManager[m_iAlignNum]->SocketSendto(m_iAlignNum, strCommand, MC_MODEL_CREATE);
		theApp.m_AlignSocketManager[m_iAlignNum]->LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[MC -> VS] %s->%s"), MC_PacketNameTable[MC_MODEL_CREATE], strCommand));
		Delay(200, TRUE);
	}
	
	if (theApp.m_ChangeModelAlign)
	{
		strCommand = CStringSupport::FormatString(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
		theApp.m_AlignSocketManager[m_iAlignNum]->SocketSendto(m_iAlignNum, strCommand, MC_MODEL_CHANGE);
		theApp.m_AlignSocketManager[m_iAlignNum]->LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[MC -> VS] %s->%s"), MC_PacketNameTable[MC_MODEL_CHANGE], strCommand));
		Delay(200, TRUE);
	}
}

void CAlignThread::AlignCheckMethod()
{
	CString strCommand = CStringSupport::FormatString(_T("%d,%d"), MC_ARE_YOU_THERE, m_AlignCheckCount);
	theApp.m_AlignSocketManager[m_iAlignNum]->SocketSendto(m_iAlignNum, strCommand, MC_ARE_YOU_THERE);
}

void CAlignThread::AlignGrabMethod(int iPanelNum, int iPositionTXY, int iAlignNum)
{
	PanelData pPanelData;
	FpcIDData pFpcData;
	CString strPanelID, sendMsg;

	if (m_positionResult == 0 && theApp.m_AlignSocketManager[m_iAlignNum]->m_bstart == FALSE)
	{
		AlignPLCResult(iPanelNum, m_iAlignType, iAlignNum, m_codePositionError, PLC_ResultValue[m_codePositionError], _T("NG"));
		return;
	}

	if (m_AlignCount == 0 && theApp.m_AlignSocketManager[m_iAlignNum]->m_bstart == FALSE)
	{
		AlignPLCResult(iPanelNum, m_iAlignType, iAlignNum, m_codeAlignCount, PLC_ResultValue[m_codeAlignCount], _T("NG"));
		return;
	}

	if (theApp.m_PanelTestStart)
		strPanelID.Format(_T("TEST_%d_%s"), iPanelNum, GetDateString4());
	else
	{
		if (iPanelNum == PanelNum1)
			theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_Align1PanelID1 + iAlignNum, &pFpcData);
		else
			theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_Align1PanelID2 + iAlignNum, &pFpcData);

		strPanelID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));

		if (strPanelID.IsEmpty())
		{
			AlignPLCResult(iPanelNum, m_iAlignType, iAlignNum, m_codePlcPanelError, PLC_ResultValue[m_codePlcPanelError], _T("NG"));
			return;
		}

	}

	int iPositionPanelNum = (m_positionResult * 2) - 1;
	int iPanelCheal = (iPositionPanelNum - 1) % 4;

#if _SYSTEM_AMTAFT_ 
	int iIndexNum = (iPositionPanelNum / MaxZone);
	CString strIndexNum = iAlignNum == 0 ? PG_IndexName[iIndexNum] : ULD_PG_IndexName[iIndexNum];
#else
	int iIndexNum;
	if (theApp.m_AlignSocketManager[m_iAlignNum]->m_bstart == FALSE)
		iIndexNum = m_positionResult - 1;
	else
		iIndexNum = m_positionResult;

	CString strIndexNum = PG_IndexName[iIndexNum];
#endif

	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].m_strZoneName = PG_IndexName[iIndexNum];
	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].m_iPanelNum = iPanelCheal + iPanelNum;
	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].m_bInspStart = TRUE;
	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].m_cellId = strPanelID;


	if (theApp.m_iTimer[AlignGrabTimer] == 0)
		theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].time_check.SetCheckTime(8000);
	else
		theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].time_check.SetCheckTime(theApp.m_iTimer[AlignGrabTimer] * 1000);

	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].time_check.StartTimer();

	if (theApp.m_AlignSocketManager[m_iAlignType]->m_bstart)
		sendMsg.Format(_T("%d,%d,%d,%d,%d,%s,%d"), MC_GRAB_READY_REQUEST, 1, iPositionTXY, 1 + iPanelNum, 2, strPanelID, m_AlignReverse);
	else
		sendMsg.Format(_T("%d,%d,%d,%d,%d,%s,%d"), MC_GRAB_READY_REQUEST, m_positionResult, iPositionTXY, iPositionPanelNum + iPanelNum, m_AlignCount, strPanelID, m_AlignReverse);
	
	theApp.m_AlignSocketManager[m_iAlignNum]->SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_READY_REQUEST);
	theApp.m_AlignSocketManager[m_iAlignNum]->LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[MC -> VS] [%s] Panel %d Grab Start -> %s"), strIndexNum, iPanelCheal + iPanelNum, sendMsg));
}

void CAlignThread::TrayCheckNTrayAlignGrabMethod(int iPanelNum)
{
	CString sendMsg;

	if (theApp.m_iTimer[AlignGrabTimer] == 0)
		theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].time_check.SetCheckTime(8000);
	else
		theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].time_check.SetCheckTime(theApp.m_iTimer[AlignGrabTimer] * 1000);

	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].time_check.StartTimer();
	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].m_bInspStart = TRUE;

	sendMsg.Format(_T("%d,%s,%s,%s,%s,%s,%s"), MC_GRAB_READY_REQUEST, _T(""), _T(""), _T(""), _T(""), _T(""), _T(""));

	theApp.m_AlignSocketManager[m_iAlignNum]->SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_READY_REQUEST);
	theApp.m_AlignSocketManager[m_iAlignNum]->LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[MC -> VS] %s Grab Start -> %s"), AlignTypeName[m_iAlignType], sendMsg));
}

void CAlignThread::TrayLowerAlignGrabMethod(int iPanelNum, int iAlignNum)
{
	FpcIDData pFpcData[ChMaxCount];
	CString strPanelID[ChMaxCount] = { _T(""), _T("") }, sendMsg;

	if (m_positionResult == 0 && theApp.m_AlignSocketManager[m_iAlignNum]->m_bstart == FALSE)
	{
		AlignPLCResult(iPanelNum, m_iAlignType, iAlignNum, m_codePositionError, PLC_ResultValue[m_codePositionError], _T("NG"));
		return;
	}

	if (m_PanelCount == 0 && theApp.m_AlignSocketManager[m_iAlignNum]->m_bstart == FALSE)
	{
		AlignPLCResult(iPanelNum, m_iAlignType, iAlignNum, m_codeAlignCount, PLC_ResultValue[m_codeAlignCount], _T("NG"));
		return;
	}

	if (theApp.m_PanelTestStart)
	{
		strPanelID[0].Format(_T("TEST_%d_%s"), PanelNum1 + 1, GetDateString4());
		strPanelID[1].Format(_T("TEST_%d_%s"), PanelNum2 + 1, GetDateString4());
	}
	else
	{
		theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_TrayLower1AlignPanelID1 + iAlignNum, &pFpcData[PanelNum1]);
		theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_TrayLower1AlignPanelID2 + iAlignNum, &pFpcData[PanelNum2]);

		strPanelID[PanelNum1] = CStringSupport::ToWString(pFpcData[PanelNum1].m_FpcIDData, sizeof(pFpcData[PanelNum1].m_FpcIDData));
		strPanelID[PanelNum2] = CStringSupport::ToWString(pFpcData[PanelNum2].m_FpcIDData, sizeof(pFpcData[PanelNum2].m_FpcIDData));

		if (strPanelID[0].IsEmpty() && strPanelID[1].IsEmpty())
		{
			AlignPLCResult(iPanelNum, m_iAlignType, iAlignNum, m_codePlcPanelError, PLC_ResultValue[m_codePlcPanelError], _T("NG"));
			return;
		}
	}

	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].m_iPanelNum = iPanelNum;
	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].m_bInspStart = TRUE;
	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].m_strcellId[PanelNum1] = strPanelID[PanelNum1];
	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].m_strcellId[PanelNum2] = strPanelID[PanelNum2];


	if (theApp.m_iTimer[AlignGrabTimer] == 0)
		theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].time_check.SetCheckTime(8000);
	else
		theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].time_check.SetCheckTime(theApp.m_iTimer[AlignGrabTimer] * 1000);

	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].time_check.StartTimer();

	if (theApp.m_AlignSocketManager[m_iAlignNum]->m_bstart)
		sendMsg.Format(_T("%d,%d,%s,%d,%s,%s,%s"), MC_GRAB_READY_REQUEST, 1, _T(""), 1, _T(""), strPanelID[CH_1], strPanelID[CH_2]);
	else
		sendMsg.Format(_T("%d,%d,%s,%d,%s,%s,%s"), MC_GRAB_READY_REQUEST, m_positionResult, _T(""), m_PanelCount, _T(""), strPanelID[CH_1], strPanelID[CH_2]);

	theApp.m_AlignSocketManager[m_iAlignNum]->SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_READY_REQUEST);
	theApp.m_AlignSocketManager[m_iAlignNum]->LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[MC -> VS] TrayLowerAlign Grab Start -> %s"), sendMsg));
}

void CAlignThread::AlignPLCResult(int iPanelNum, int iAlignType, int iAlignNum, int iResultCode, CString strResultMsg, CString strPanelID)
{
	if (iAlignType == PatternAlign)
	{
		AlignResult alignResult;
		alignResult.resultValue = iResultCode;
		if (iPanelNum == PanelNum1)
		{
			theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_Align1Result1 + iAlignNum, &alignResult);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_Align1End1 + iAlignNum, OffSet_0, TRUE);
		}
		else
		{
			theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_Align1Result2 + iAlignNum, &alignResult);
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_Align1End2 + iAlignNum, OffSet_0, TRUE);
		}
	}
	else if (iAlignType == TrayCheck)
	{
		TrayCheckResult trayCheckResult;
		//trayCheckResult.resultValue = iResultCode;
		theApp.m_pEqIf->m_pMNetH->SetTrayCheckResult(eWordType_TrayCheckResult1 + iAlignNum, &trayCheckResult);
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayCheckEnd1 + iAlignNum, OffSet_0, TRUE);
	}
	else if (iAlignType == TrayAlign)
	{
		AlignResult alignResult;
		alignResult.resultValue = iResultCode;
		theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_TrayAlignResult1 + iAlignNum, &alignResult);
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayAlignEnd1 + iAlignNum, OffSet_0, TRUE);
	}
	else if (iAlignType == TrayLowerAlign)
	{
		TrayLowerAlignResult trayLowerResult;
		trayLowerResult.resultValue = iResultCode;
		theApp.m_pEqIf->m_pMNetH->SetTrayLowerAlignResult(eWordType_TrayLowerAlignResult1 + iAlignNum, &trayLowerResult);
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayLowerAlignEnd1 + iAlignNum, OffSet_0, TRUE);
	}

	theApp.m_AlignSocketManager[m_iAlignNum]->LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("%s Panel_%d %s Result : %s"), AlignTypeName[m_iAlignType], iPanelNum + 1, strPanelID, strResultMsg));
}

UINT CAlignThread::AlignThreadProc(LPVOID pParam)
{
	CAlignThread* pThis = reinterpret_cast<CAlignThread*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;

} // end AlignThreadProc

BOOL CAlignThread::CreateTask(){
	BOOL bRet = TRUE;
	m_pThreadAlign = ::AfxBeginThread(AlignThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadAlign)
		bRet = FALSE;
	m_pThreadAlign->m_bAutoDelete = FALSE;	/// ¾²·¹µå Á¾·á½Ã WaitForSingleObject Àû¿ëÀ§ÇØ...
	m_pThreadAlign->ResumeThread();
	return bRet;
}

void CAlignThread::CloseTask()
{
	if (m_pThreadAlign != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadAlign->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadAlign->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadAlign->m_hThread, 1L);
				theApp.m_AlignLog->LOG_INFO(_T("Terminate Align Thread"));
			}
		}
		delete m_pThreadAlign;
		m_pThreadAlign = NULL;

	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}


}