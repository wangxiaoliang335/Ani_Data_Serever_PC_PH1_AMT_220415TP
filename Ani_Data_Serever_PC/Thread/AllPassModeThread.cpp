#include "stdafx.h"
#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "DlgMainLog.h"
#else
#include "DlgGammaMain.h"
#endif
#include "AllPassModeThread.h"

CAllPassModeThread::CAllPassModeThread()
{
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_bFisrtStartFlag = FALSE;
	m_bStartFlag = FALSE;
	m_bVisionStart = FALSE;

	m_bAlarmStart = FALSE;
	m_AxisStart = FALSE;
	m_bModelStart = FALSE;
	m_bModelFirstCheck = FALSE;
	m_OperateTimeStart = FALSE;
	

	for (int ii = 0; ii < PanelMaxCount; ii++)
	{
		m_bStartAlign[ii] = FALSE;
		m_bAlignStartFlag[ii] = FALSE;
		m_bStartVision[ii] = FALSE;
		m_bStartViewingAngle[ii] = FALSE;
		m_bViewingStartFlag[ii] = FALSE;
		m_bDataFlag[ii] = FALSE;
		m_bPreGammaFlag[ii] = FALSE;
		m_bTouchFlag[ii] = FALSE;
		m_bContactOnFlag[ii] = FALSE;
		m_bContactOffFlag[ii] = FALSE;
	}
}

CAllPassModeThread::~CAllPassModeThread()
{
	
}

void CAllPassModeThread::ThreadRun()
{
	//int ms;
	ModelNameData pModelName;
	AlignResult m_AlignResult;
	while (::WaitForSingleObject(m_hQuit, 50) != WAIT_OBJECT_0)
	{
//		theApp.m_bAllPassMode = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AllPassModeStart, OffSet_0);
//
//		if (theApp.m_bAllPassMode == TRUE)
//		{
//#if _SYSTEM_AMTAFT_
//			// Vision
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionReady, OffSet_0, TRUE);
//
//			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_VisionPlcSend, 0))
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionPcReceiver, 0, TRUE);
//
//			for (int ii = 0; ii < PanelMaxCount; ii++)
//			{
//				m_bVisionStart = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_VisionStart1, OffSet_0 + ii);
//
//				if (m_bVisionStart == FALSE)
//				{
//					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_VisionResult1 + ii, &m_codeReset);
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionGrabEnd1 + ii, OffSet_0, FALSE);
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionEnd1 + ii, OffSet_0, FALSE);
//				}
//
//				if (m_bStartVision[ii] == !m_bVisionStart)
//				{
//					m_bStartVision[ii] = m_bVisionStart;
//		
//					if (m_bVisionStart == TRUE)
//					{
//						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_VisionResult1 + ii, &m_codeOk);
//						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionGrabEnd1 + ii, OffSet_0, TRUE);
//						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionEnd1 + ii, OffSet_0, TRUE);
//						
//					}
//				}
//			}
//
//			// Viewing Angle
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAngleReady, OffSet_0, TRUE);
//			
//			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_ViewingAnglePlcSend, OffSet_0))
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAnglePcReceiver, OffSet_0, TRUE);
//
//			for (int ii = 0; ii < PanelMaxCount; ii++)
//			{
//				m_bViewingStartFlag[ii] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_ViewingAngleStart1 + ii, OffSet_0);
//
//				if (m_bViewingStartFlag[ii] == FALSE)
//				{
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAngleEnd1 + ii, OffSet_0, FALSE);
//					theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_ViewingAngleResult1 + ii, &m_codeReset);
//				}
//
//				if (m_bStartViewingAngle[ii] == !m_bViewingStartFlag[ii])
//				{
//					m_bStartViewingAngle[ii] = m_bViewingStartFlag[ii];
//	
//					if (m_bViewingStartFlag[ii] == TRUE)
//					{
//						//ms = rand() % 2 == 0 ? m_codeOk : m_codeFail;
//						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ViewingAngleEnd1 + ii, OffSet_0, TRUE);
//						theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_ViewingAngleResult1 + ii, &m_codeOk);
//					}
//
//				}
//			}
//
//			for (int ii = 0; ii < PanelMaxCount; ii++)
//			{
//				m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_DataReportStart1 + ii, OffSet_0);
//
//				if (m_bStartFlag == FALSE)
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_DataReportEnd1 + ii, OffSet_0, FALSE);
//
//				if (m_bDataFlag[ii] == !m_bStartFlag)
//				{
//					m_bDataFlag[ii] = m_bStartFlag;
//
//					if (m_bDataFlag[ii] == TRUE)
//					{
//						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_DataReportEnd1 + ii, OffSet_0, TRUE);
//					}
//				}
//			}
//
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaReady, OffSet_0, TRUE);
//
//			if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_PreGammaPlcSend, OffSet_0))
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaPcReceiver, 0, TRUE);
//
//			for (int ii = 0; ii < PanelMaxCount; ii++)
//			{
//				m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_PreGammaStart1 + ii, OffSet_0);
//
//				if (m_bStartFlag == FALSE)
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaEnd1 + ii, OffSet_0, FALSE);
//
//				if (m_bPreGammaFlag[ii] == !m_bStartFlag)
//				{
//					m_bPreGammaFlag[ii] = m_bStartFlag;
//
//					if (m_bPreGammaFlag[ii] == TRUE)
//					{
//						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_PreGammaEnd1 + ii, OffSet_0, TRUE);
//						theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_PreGammaResult1 + ii, OffSet_0, &m_codeOk);
//						
//					}
//				}
//			}
//
//			for (int ii = 0; ii < PanelMaxCount; ii++)
//			{
//				if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZoneContactPlcSend + ii, OffSet_0))
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactPcReceiver + ii, OffSet_0, TRUE);
//
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchReady + ii, OffSet_0, TRUE);
//
//				if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZoneTouchPlcSend + ii, OffSet_0))
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchPcReceiver + ii, OffSet_0, TRUE);
//			}
//
//			for (int jj = 0; jj < MaxZone; jj++)
//			{
//				for (int ii = 0; ii < PanelMaxCount; ii++)
//				{
//					m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZoneContactOnStart + jj, ii);
//
//					if (m_bStartFlag == FALSE)
//						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOnEnd + jj, ii, FALSE);
//
//					if (m_bContactOnFlag[ii] == !m_bStartFlag)
//					{
//						m_bContactOnFlag[ii] = m_bStartFlag;
//
//						if (m_bContactOnFlag[ii] == TRUE)
//						{
//							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOnEnd + jj, ii, TRUE);
//							theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOnResult + jj, ii, &m_codeOk);
//						}
//					}
//				}
//
//				for (int ii = 0; ii < PanelMaxCount; ii++)
//				{
//					m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZoneContactOffStart + jj, ii);
//
//					if (m_bStartFlag == FALSE)
//						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOffEnd + jj, ii, FALSE);
//
//					if (m_bContactOffFlag[ii] == !m_bStartFlag)
//					{
//						m_bContactOffFlag[ii] = m_bStartFlag;
//
//						if (m_bContactOffFlag[ii] == TRUE)
//						{
//							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneContactOffEnd + jj, ii, TRUE);
//							theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneContactOffResult + jj, ii, &m_codeOk);
//						}
//					}
//				}
//
//				for (int ii = 0; ii < PanelMaxCount; ii++)
//				{
//					m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AZoneTouchInspectionStart + jj, ii);
//
//					if (m_bStartFlag == FALSE)
//						theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchInspectionEnd + jj, ii, FALSE);
//
//					if (m_bTouchFlag[ii] == !m_bStartFlag)
//					{
//						m_bTouchFlag[ii] = m_bStartFlag;
//
//						if (m_bTouchFlag[ii] == TRUE)
//						{
//							theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AZoneTouchInspectionEnd + jj, ii, TRUE);
//							theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_AZoneTouchResult + jj, ii, &m_codeOk);
//						}
//					}
//				}
//			}
//#elif _SYSTEM_GAMMA_
//	for (int ii = 0; ii < MaxGammaStage; ii++)
//	{
//		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1Ready + ii, OffSet_0, TRUE);
//	
//		if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaStage1PlcSend + ii, OffSet_0))
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1PcReceiver + ii, OffSet_0, TRUE);
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Gamma_1MTPStart1 + ii, GammaStageMtp1);
//
//		if (m_bStartFlag == FALSE)
//		{
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd1 + ii, OffSet_0, FALSE);
//			theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + ii, OffSet_0, &m_codeReset);
//		}
//
//		if (m_bStartFlag == !m_bGammaFlag[GammaStageMtp1][PanelNum1])
//		{
//			m_bGammaFlag[GammaStageMtp1][PanelNum1] = m_bStartFlag;
//
//			if (m_bGammaFlag[GammaStageMtp1][PanelNum1] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd1 + ii, OffSet_0, TRUE);
//				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult1 + ii, OffSet_0, &m_codeOk);
//			}
//		}
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_Gamma_1MTPStart1 + ii, GammaStageMtp2);
//
//		if (m_bStartFlag == FALSE)
//		{
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd2 + ii, OffSet_0, FALSE);
//			theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult2 + ii, OffSet_0, &m_codeReset);
//		}
//
//		if (m_bStartFlag == !m_bGammaFlag[GammaStageMtp2][PanelNum2])
//		{
//			m_bGammaFlag[GammaStageMtp2][PanelNum2] = m_bStartFlag;
//
//			if (m_bGammaFlag[GammaStageMtp2][PanelNum2] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaStage1MTPEnd2 + ii, OffSet_0, TRUE);
//				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1MTPResult2 + ii, OffSet_0, &m_codeOk);
//			}
//		}
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaContactOnStart1 + ii, OffSet_0);
//	
//		for (int jj = 0; jj < ChMaxCount; jj++)
//		{
//			if (m_bStartFlag == FALSE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOnEnd1 + ii, OffSet_0, FALSE);
//				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + ii, jj, &m_codeReset);
//			}
//	
//	
//			if (m_bStartFlag == !m_bGammaFlag[GammaStageContactOn][jj])
//			{
//				m_bGammaFlag[GammaStageContactOn][jj] = m_bStartFlag;
//	
//				if (m_bGammaFlag[GammaStageContactOn][jj] == TRUE)
//				{
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOnEnd1 + ii, OffSet_0, TRUE);
//					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOnResult + ii, jj, &m_codeOk);
//				}
//			}
//		}
//	
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaContactOffStart1 + ii, OffSet_0);
//	
//		for (int jj = 0; jj < ChMaxCount; jj++)
//		{
//			if (m_bStartFlag == FALSE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOffEnd1 + ii, OffSet_0, FALSE);
//				theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + ii, jj, &m_codeReset);
//			}
//	
//	
//			if (m_bStartFlag == !m_bGammaFlag[GammaStageContactOff][jj])
//			{
//				m_bGammaFlag[GammaStageContactOff][jj] = m_bStartFlag;
//	
//				if (m_bGammaFlag[GammaStageContactOff][jj] == TRUE)
//				{
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactOffEnd1 + ii, OffSet_0, TRUE);
//					theApp.m_pEqIf->m_pMNetH->SetWordResultOffSet(eWordType_GammaStage1ContactOffResult + ii, jj, &m_codeOk);
//				}
//			}
//		}
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaContactNextStart1 + ii, OffSet_0);
//	
//		if (m_bStartFlag == FALSE)
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactNextEnd1 + ii, OffSet_0, FALSE);
//	
//		for (int jj = 0; jj < ChMaxCount; jj++)
//		{
//			if (m_bStartFlag == !m_bGammaFlag[GammaStageNext][jj])
//			{
//				m_bGammaFlag[GammaStageNext][jj] = m_bStartFlag;
//	
//				if (m_bGammaFlag[GammaStageNext][jj] == TRUE)
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactNextEnd1 + ii, OffSet_0, TRUE);
//			}
//		}
//	
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_GammaContactBackStart1 + ii, OffSet_0);
//	
//		if (m_bStartFlag == FALSE)
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactBackEnd1 + ii, OffSet_0, FALSE);
//	
//		for (int jj = 0; jj < ChMaxCount; jj++)
//		{
//			if (m_bStartFlag == !m_bGammaFlag[GammaStageBack][jj])
//			{
//				m_bGammaFlag[GammaStageBack][jj] = m_bStartFlag;
//	
//				if (m_bGammaFlag[GammaStageBack][jj] == TRUE)
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_GammaContactBackEnd1 + ii, OffSet_0, TRUE);
//			}
//		}
//	}
//	
//	for (int ii = 0; ii < ChMaxCount; ii++)
//	{
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_DataReportStart1 + ii, OffSet_0);
//	
//		if (m_bStartFlag == FALSE)
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_DataReportEnd1 + ii, OffSet_0, FALSE);
//	
//		if (m_bDataFlag[ii] == !m_bStartFlag)
//		{
//			m_bDataFlag[ii] = m_bStartFlag;
//
//			if (m_bDataFlag[ii] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_DataReportEnd1 + ii, OffSet_0, TRUE);
//			}
//		}
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_NgReportStart1 + ii, OffSet_0);
//
//		if (m_bStartFlag == FALSE)
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_NgReportEnd1 + ii, OffSet_0, FALSE);
//
//		if (m_bDataFlag[ii] == !m_bStartFlag)
//		{
//			m_bDataFlag[ii] = m_bStartFlag;
//
//			if (m_bDataFlag[ii] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_NgReportEnd1 + ii, OffSet_0, TRUE);
//			}
//		}
//	}
//
//#else
//	for (int ii = 0; ii < ChMaxCount; ii++)
//	{
//		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaReady + ii, OffSet_0, TRUE);
//		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchReady + ii, OffSet_0, TRUE);
//		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OperatorViewReady + ii, OffSet_0, TRUE);
//
//		//Gamma , PG
//		if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAContactPlcDataSend + ii, OffSet_0))
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactPcDataReceived + ii, OffSet_0, TRUE);
//
//
//		if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAPreGammaPlcDataSend + ii, OffSet_0))
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaPcDataReceived + ii, OffSet_0, TRUE);
//
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAContactOnStart + ii, OffSet_0);
//
//		if(m_bStartFlag == FALSE)
//		{
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOnEnd + ii, OffSet_0, FALSE);
//			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOnResult + ii, &m_codeReset);
//		}
//
//		if (m_bStartFlag == !m_bUnloaderFlag[ManualStageContactOn][ii])
//		{
//			m_bUnloaderFlag[ManualStageContactOn][ii] = m_bStartFlag;
//
//			if (m_bUnloaderFlag[ManualStageContactOn][ii] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOnEnd + ii, OffSet_0, TRUE);
//				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOnResult + ii, &m_codeOk);
//			}
//		}
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAContactOffStart + ii, OffSet_0);
//
//		if(m_bStartFlag == FALSE)
//		{
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOffEnd + ii, OffSet_0, FALSE);
//			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOffResult + ii, &m_codeReset);
//		}
//
//		if (m_bStartFlag == !m_bUnloaderFlag[ManualStageContactOff][ii])
//		{
//			m_bUnloaderFlag[ManualStageContactOff][ii] = m_bStartFlag;
//
//			if (m_bUnloaderFlag[ManualStageContactOff][ii] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactOffEnd + ii, OffSet_0, TRUE);
//				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAContactOffResult + ii, &m_codeOk);
//			}
//		}
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAContactNextStart + ii, OffSet_0);
//
//		if (m_bStartFlag == FALSE)
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactNextEnd + ii, OffSet_0, FALSE);
//
//		if (m_bStartFlag == !m_bUnloaderFlag[ManualStageNext][ii])
//		{
//			m_bUnloaderFlag[ManualStageNext][ii] = m_bStartFlag;
//
//			if (m_bUnloaderFlag[ManualStageNext][ii] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactNextEnd + ii, OffSet_0, TRUE);
//			}
//		}
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAContactBackStart + ii, OffSet_0);
//
//		if (m_bStartFlag == FALSE)
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactBackEnd + ii, OffSet_0, FALSE);
//
//		if (m_bStartFlag == !m_bUnloaderFlag[ManualStageBack][ii])
//		{
//			m_bUnloaderFlag[ManualStageBack][ii] = m_bStartFlag;
//
//			if (m_bUnloaderFlag[ManualStageBack][ii] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAContactBackEnd + ii, OffSet_0, TRUE);
//			}
//		}
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageAPreGammaStart + ii, OffSet_0);
//
//		if (m_bStartFlag == FALSE)
//		{
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaEnd + ii, OffSet_0, FALSE);
//			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAGammaResult + ii, &m_codeReset);
//		}
//
//		if (m_bStartFlag == !m_bUnloaderFlag[ManualStagePreGamma][ii])
//		{
//			m_bUnloaderFlag[ManualStagePreGamma][ii] = m_bStartFlag;
//
//			if (m_bUnloaderFlag[ManualStagePreGamma][ii] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageAPreGammaEnd + ii, OffSet_0, TRUE);
//				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAGammaResult + ii, &m_codeOk);
//			}
//		}
//
//		//TP
//		if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageATouchPlcDataSend + ii, OffSet_0))
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchPcDataReceived + ii, OffSet_0, TRUE);
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageATouchStart + ii, OffSet_0);
//
//		if (m_bStartFlag == FALSE)
//		{
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchEnd + ii, OffSet_0, FALSE);
//			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageATouchResult + ii, &m_codeReset);
//		}
//
//		if (m_bStartFlag == !m_bUnloaderFlag[ManualStageTouch][ii])
//		{
//			m_bUnloaderFlag[ManualStageTouch][ii] = m_bStartFlag;
//
//			if (m_bUnloaderFlag[ManualStageTouch][ii] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageATouchEnd + ii, OffSet_0, TRUE);
//				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageATouchResult + ii, &m_codeOk);
//			}
//		}
//
//		//OPV
//		if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageA_OperatorViewPlcDataSend + ii, OffSet_0))
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OperatorViewPcDataReceived + ii, OffSet_0, TRUE);
//
//		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_MStageA_OperatorViewStart + ii, OffSet_0);
//
//		if (m_bStartFlag == FALSE)
//		{
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OperatorViewEnd + ii, OffSet_0, FALSE);
//			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAOperatorViewResult + ii, &m_codeReset);
//		}
//
//		if (m_bStartFlag == !m_bUnloaderFlag[ManualStageOperatorView][ii])
//		{
//			m_bUnloaderFlag[ManualStageOperatorView][ii] = m_bStartFlag;
//
//			if (m_bUnloaderFlag[ManualStageOperatorView][ii] == TRUE)
//			{
//				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OperatorViewEnd + ii, OffSet_0, TRUE);
//				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAOperatorViewResult + ii, &m_codeOk);
//			}
//		}
//	}
//#endif
//
//			// PLC
//			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_DataServerPcHeartBit, OffSet_0, theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_PlcHearbit, OffSet_0));
//
//			m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_ModelStart, OffSet_0);
//
//			if (m_bModelStart == !m_bStartFlag)
//			{
//				m_bModelStart = m_bStartFlag;
//				if (m_bModelStart == TRUE)
//				{
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_ModelEnd, OffSet_0, TRUE);
//				}
//			}
//
//			m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AlarmStart, OffSet_0);
//
//			if (m_bAlarmStart == !m_bStartFlag)
//			{
//				m_bAlarmStart = m_bStartFlag;
//				if (m_bAlarmStart == TRUE)
//				{
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AlarmEnd, OffSet_0, TRUE);
//				}
//			}
//
//
//			m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AxisStart, OffSet_0);
//
//			if (m_AxisStart == !m_bStartFlag)
//			{
//				m_AxisStart = m_bStartFlag;
//
//				if (m_AxisStart == TRUE)
//				{
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AxisEnd, OffSet_0, TRUE);
//				}
//			}
//
//			m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_OperateStart, OffSet_0);
//
//			if (m_OperateTimeStart == !m_bStartFlag)
//			{
//				m_OperateTimeStart = m_bStartFlag;
//				if (m_OperateTimeStart == TRUE)
//				{
//					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_OperateEnd, OffSet_0, TRUE);
//				}
//			}
//
//			//TWICE				AllPassMode 추후 추가 하자
//			//theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AlignReady1, OffSet_0, TRUE);
//			//theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AlignReady2, OffSet_0, TRUE);
//
//			//// Align
//			//for (int ii = 0; ii < 2; ii++)
//			//{
//			//	if (theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AlignDataPlcSend1 + ii, OffSet_0))
//			//	{
//			//		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AlignDataPcReceived1 + ii, OffSet_0, TRUE);
//			//	}
//			//}
//
//			//m_bAlignStartFlag[0] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AlignTStart1, OffSet_0);
//			//m_bAlignStartFlag[1] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AlignTStart2, OffSet_0);
//			//m_bAlignStartFlag[2] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AlignXyStart1, OffSet_0);
//			//m_bAlignStartFlag[3] = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_AlignXyStart2, OffSet_0);
//
//			//m_AlignResult.resultValue = m_codeReset;
//
//			//if (m_bAlignStartFlag[0] == FALSE && m_bAlignStartFlag[2] == FALSE)
//			//{
//			//	theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_AlignResult1, &m_AlignResult);
//			//	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AlignEnd1, OffSet_0, FALSE);
//			//}
//
//			//if (m_bAlignStartFlag[1] == FALSE && m_bAlignStartFlag[3] == FALSE)
//			//{
//			//	theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_AlignResult2, &m_AlignResult);
//			//	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AlignEnd2, OffSet_0, FALSE);
//			//}
//
//			//m_AlignResult.resultValue = m_codeOk;
//
//			//if (m_bStartAlign[0] == !m_bAlignStartFlag[0])
//			//{
//			//	m_bStartAlign[0] = m_bAlignStartFlag[0];
//
//			//	if (m_bAlignStartFlag[0] == TRUE)
//			//	{
//			//		theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_AlignResult1, &m_AlignResult);
//			//		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AlignEnd1, OffSet_0, TRUE);
//			//	
//			//	}
//			//}
//
//			//if (m_bStartAlign[1] == !m_bAlignStartFlag[1])
//			//{
//			//	m_bStartAlign[1] = m_bAlignStartFlag[1];
//
//			//	if (m_bAlignStartFlag[1] == TRUE)
//			//	{
//			//		theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_AlignResult2, &m_AlignResult);
//			//		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AlignEnd2, OffSet_0, TRUE);
//
//			//	}
//			//}
//
//			//if (m_bStartAlign[2] == !m_bAlignStartFlag[2])
//			//{
//			//	m_bStartAlign[2] = m_bAlignStartFlag[2];
//
//			//	if (m_bAlignStartFlag[2] == TRUE)
//			//	{
//			//		theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_AlignResult1, &m_AlignResult);
//			//		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AlignEnd1, OffSet_0, TRUE);
//
//			//	}
//			//}
//
//			//if (m_bStartAlign[3] == !m_bAlignStartFlag[3])
//			//{
//			//	m_bStartAlign[3] = m_bAlignStartFlag[3];
//
//			//	if (m_bAlignStartFlag[3] == TRUE)
//			//	{
//			//		theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_AlignResult2, &m_AlignResult);
//			//		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_AlignEnd2, OffSet_0, TRUE);
//
//			//	}
//			//}
//		}
//		else
//		{
//			m_bAlarmStart = FALSE;
//			m_AxisStart = FALSE;
//			m_bModelStart = FALSE;
//			m_bModelFirstCheck = FALSE;
//			m_OperateTimeStart = FALSE;
//		}
	}
}

UINT CAllPassModeThread::AllPassModeThreadProc(LPVOID pParam)
{
	CAllPassModeThread* pThis = reinterpret_cast<CAllPassModeThread*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;

}

BOOL CAllPassModeThread::CreateTask()
{
	BOOL bRet = TRUE;
	m_pThreadAllPassMode = ::AfxBeginThread(AllPassModeThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadAllPassMode)
		bRet = FALSE;
	m_pThreadAllPassMode->m_bAutoDelete = FALSE;
	m_pThreadAllPassMode->ResumeThread();
	return bRet;
}

void CAllPassModeThread::CloseTask()
{
	if (m_pThreadAllPassMode != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadAllPassMode->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadAllPassMode->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadAllPassMode->m_hThread, 1L);
				theApp.m_PlcLog->LOG_INFO(_T("Terminate PLC Thread"));
			}
		}
		delete m_pThreadAllPassMode;
		m_pThreadAllPassMode = NULL;

	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}