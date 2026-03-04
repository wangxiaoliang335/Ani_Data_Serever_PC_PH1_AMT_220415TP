
#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "DlgMainLog.h"
#else
#include "DlgGammaMain.h"
#endif
#include "AlignManager.h"

CAlignManager::CAlignManager(int iAlignType, int iAlignTypeNum, int iAlignNum)
{
	m_iAlignType = iAlignType;
	m_iAlignTypeNum = iAlignTypeNum;
	m_iAlignNum = iAlignNum;
}

CAlignManager::~CAlignManager()
{
}

BOOL CAlignManager::getConectCheck()
{
	SockAddrIn addrin;
	GetSockName(addrin);
	LONG  uAddr = addrin.GetIPAddr();
	if (uAddr == 0)
		return FALSE;
	else
		return TRUE;
}

bool CAlignManager::SocketServerOpen(CString strServerPort)
{
	m_bMelsecSimulaion = true;
	SetSmartAddressing(false);
	SetServerState(true);
	bool ret = CreateSocket(strServerPort, AF_INET, SOCK_STREAM, 0);
	if (ret) return WatchComm();
	else return false;
}

void CAlignManager::LogWrite(int iNum, CString strContents)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	g_MainLog->m_AlignListBox.InsertString(0, CStringSupport::FormatString(_T("[%s] %s"), GetNowSystemTimeMilliseconds(), strContents));
	theApp.m_AlignLog->LOG_INFO(strContents);
}

void CAlignManager::SocketSendto(int iNum, CString strContents, int iCommand)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	CString strCommand = CStringSupport::FormatString(_T("%c%s%c"), _STX, strContents, _ETX);
	char *lpCommand = StringToChar(strCommand);
	theApp.m_AlignSocketManager[iNum]->WriteComm((BYTE*)lpCommand, strlen(lpCommand), 100L);
	delete lpCommand;

	m_lastContent = strContents;
	theApp.m_pAlignSendReceiverLog[iNum]->LOG_INFO(CStringSupport::FormatString(_T("[MC -> VS] [Command : %s] ->%s"), MC_PacketNameTable[iCommand], strContents));
}

void CAlignManager::AlignLightOff(int iNum)
{
	CString sendMsg;
	for (int ii = 0; ii < 2; ii++)
	{
		sendMsg.Format(_T("%d,%d"), MC_ALIGN_LIGHT_OFF, ii);
		SocketSendto(iNum, sendMsg, MC_ALIGN_LIGHT_OFF);
	}
}

void CAlignManager::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	CString strData, m_strHeader, m_strCommand, m_strContents, strParsing;
	int iFind, iFindSTX;
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount, strData.GetBuffer(dwCount + 1), dwCount + 1);
	strData.ReleaseBuffer(dwCount);

	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strData, _ETX, responseTokens);

	//theApp.m_pAlignSendReceiverLog[m_iAlignNum]->LOG_INFO(strData);		//TWICE

	if (responseTokens.GetSize() == 1)
	{
		LogWrite(m_iAlignNum,_T("ETX No Message!!!"));
		return;
	}

	for (int ii = 0; ii < responseTokens.GetSize() - 1; ii++)
	{
		strParsing = responseTokens[ii];

		m_strHeader.Format(_T("%x"), strParsing.GetAt(0));

		UINT iHeader = (UINT)_ttoi(m_strHeader);

		if (iHeader != _STX)
		{
			LogWrite(m_iAlignNum,_T("STX No Message!!!"));
			return;
		}

		iFind = strParsing.Find(',');
		m_strCommand = strParsing.Left(iFind);

		iFindSTX = strParsing.Find((char)_STX);
		m_strCommand = m_strCommand.Mid(iFindSTX + 1, m_strCommand.GetLength());

		int iCommand = _ttoi(m_strCommand);

		m_strContents = strParsing.Mid(iFind + 1, strParsing.GetLength());

		m_lastCommand = VS_PacketNameTable[iCommand];
		m_lastRequest = m_strContents;

		theApp.m_pAlignSendReceiverLog[m_iAlignNum]->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), m_lastCommand, strData));

		CString sendMsg;
		switch (iCommand)
		{
		case VS_ARE_YOU_THERE:
			theApp.m_AlignThread[m_iAlignNum]->m_AlignCheckCount = 0;
			break;
		case VS_PCTIME_REQUEST:
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[VS -> MC] %s"), _T("RCV : VS_PCTIME_REQUEST")));
			AlignPcTimeRequest();
			break;
		case VS_STATE:
			theApp.m_AlignPCStatus[m_iAlignNum] = m_strContents == _T("0") ? FALSE : TRUE;
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[VS_%d -> MC] %s->%s"), m_iAlignNum,  _T("RCV : VS_STATE"), theApp.m_AlignPCStatus[m_iAlignNum] == TRUE ? _T("Start") : _T("Stop")));
			break;
		case VS_MODEL_REQUEST:
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[VS -> MC] %s"), _T("RCV : VS_MODEL_REQUEST")));
			AlignModelRequest();
			break;
		case VS_MODEL_CREATE:
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[VS -> MC] %s"), _T("RCV : VS_MODEL_CREATE")));
			theApp.m_CreateModelAlign = FALSE;
			theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelCreate"), _T("Align"), theApp.m_CreateModelAlign);
			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Align Model Create Success")));

			if (theApp.m_ChangeModelAlign)
			{
				sendMsg.Format(_T("%d,%s"), MC_MODEL_CHANGE, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
				theApp.m_AlignSocketManager[m_iAlignNum]->SocketSendto(m_iAlignNum, sendMsg, MC_MODEL_CHANGE);
			}
			break;
		case VS_MODEL_CHANGE:
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[VS -> MC] %s"), _T("RCV : VS_MODEL_CHANGE")));
			theApp.m_ChangeModelAlign = FALSE;
			theApp.m_PlcThread->ModelCreateChangeModify(_T("ModelChange"), _T("Align"), theApp.m_ChangeModelAlign);
			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("Align Model Change Success")));
			break;
		case VS_GRAB_END:
			LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[VS -> MC] %s->%s"), _T("RCV : VS_GRAB_END"), m_strContents));
	
			if (m_iAlignType == PatternAlign)
				AlignGrabEnd(m_strContents);
			else if (m_iAlignType == TrayCheck)
				AlignTrayCheckGrabEnd(m_strContents);
			else if (m_iAlignType == TrayLowerAlign)
				AlignTrayLowerAlignGrabEnd(m_strContents); 
			else if (m_iAlignType == TrayAlign)
				AlignTrayAlignGrabEnd(m_strContents);
		
			break;
		}
	}
}

void CAlignManager::AlignModelRequest()
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_MODEL, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
	SocketSendto(m_iAlignNum, sendMsg, MC_MODEL);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[MC -> VS] %s->%s"), MC_PacketNameTable[MC_MODEL], sendMsg));
}

void CAlignManager::AlignPcTimeRequest()
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_PCTIME, GetDateString4());
	SocketSendto(m_iAlignNum, sendMsg, MC_PCTIME);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[MC -> VS] %s->%s"), MC_PacketNameTable[MC_PCTIME], sendMsg));
}

void CAlignManager::AlignGrabEnd(CString strContents)
{
	
	AlignResult AlignResult;
	CString sendMsg;
	int plcResultMap, plcEndMap;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	int iPanelNum = (_ttoi(responseTokens[2]) - 1) % 2;
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_0"), iPanelNum));
	sendMsg.Format(_T("%d,"), MC_GRAB_END_RECEIVE);
	SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_END_RECEIVE);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_1"), iPanelNum));
	switch (iPanelNum)
	{
	case 0: plcEndMap = eBitType_Align1End1 + m_iAlignTypeNum, plcResultMap = eWordType_Align1Result1 + m_iAlignTypeNum; break;
	case 1: plcEndMap = eBitType_Align1End2 + m_iAlignTypeNum, plcResultMap = eWordType_Align1Result2 + m_iAlignTypeNum; break;
	}

	AlignResult.resultValue = responseTokens[3] == _T("0") ? m_codeOk : m_codeFail;
	AlignResult.resultX = _ttof(responseTokens[4]) * 10000;
	AlignResult.resultY = _ttof(responseTokens[5]) * 10000;
	AlignResult.resultT = _ttof(responseTokens[6]) * 10000;
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_2"), iPanelNum));
#if _SYSTEM_AMT_AFT_
	AlignDataSum(_ttoi(responseTokens[2]), AlignResult.resultValue, m_iAlignTypeNum);
#else
	//AlignDataSum(_ttoi(responseTokens[2]), AlignResult.resultValue, Machine_GAMMA);
#endif
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_3"), iPanelNum));
	theApp.m_pEqIf->m_pMNetH->SetAlignResult(plcResultMap, &AlignResult); 
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("%s Panel %d Result : %s[%d]"), AlignTypeName[m_iAlignType], iPanelNum, PLC_ResultValue[AlignResult.resultValue], AlignResult.resultValue));
	Delay(10, TRUE);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(plcEndMap, OffSet_0, TRUE);
	theApp.m_lastAlignVec[m_iAlignNum][iPanelNum].Reset();
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_4"), iPanelNum));
	//hdm test
	if (m_bstart && m_testT[iPanelNum])
	{
		m_testT[iPanelNum] = FALSE;
		theApp.m_AlignThread[m_iAlignType]->AlignGrabMethod(iPanelNum, Align_Start_XY, m_iAlignTypeNum);
	}
	else if (m_bstart)
	{
		m_testT[iPanelNum] = TRUE;
		theApp.m_AlignThread[m_iAlignType]->AlignGrabMethod(iPanelNum, Align_Start_T, m_iAlignTypeNum);
	}
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("!!!!!!!!!!!Panel %d_5"), iPanelNum));
}

void CAlignManager::AlignTrayCheckGrabEnd(CString strContents)
{
	TrayCheckResult AlignResult;
	CString sendMsg;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	sendMsg.Format(_T("%d,"), MC_GRAB_END_RECEIVE);
	SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_END_RECEIVE);

	//종류			Comment				결과			    1번		2번
	//Tray Check	11(VS_GRAB_END)		1 = ok, 2 = ng	11100	11000

	int iResult = responseTokens[0] == _T("0") ? m_codeOk : m_codeFail;
	AlignResult.result[0] = _ttoi(responseTokens[1]); 
	AlignResult.result[1] = _ttoi(responseTokens[2]); 
	AlignResult.result[2] = _ttoi(responseTokens[3]);
	AlignResult.result[3] = _ttoi(responseTokens[4]);

	theApp.m_pEqIf->m_pMNetH->SetTrayCheckResult(eWordType_TrayCheckResult1 + m_iAlignTypeNum, &AlignResult);
	CString strMsg = m_iAlignNum == 0 ? _T("AOI") : _T("UNLOADER");
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("[%s] %s Result : %s[%d]"), strMsg, AlignTypeName[m_iAlignType], PLC_ResultValue[iResult], iResult));
	Delay(10, TRUE);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayCheckEnd1 + m_iAlignTypeNum, OffSet_0, TRUE);
	theApp.m_lastAlignVec[m_iAlignNum][PanelNum1].Reset();

	//hdm test
	if (m_bstart)
		theApp.m_AlignThread[m_iAlignType]->TrayCheckNTrayAlignGrabMethod(PanelNum1);
}

void CAlignManager::AlignTrayAlignGrabEnd(CString strContents)
{
	AlignResult AlignResult;
	CString sendMsg;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	sendMsg.Format(_T("%d,"), MC_GRAB_END_RECEIVE);
	SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_END_RECEIVE);

	//종류			Comment			결과			X	Y	T
	//Tray Align	11(VS_GRAB_END)	1 = ok, 2 = ng	값	값	값

	AlignResult.resultValue = responseTokens[0] == _T("0") ? m_codeOk : m_codeFail;
	AlignResult.resultX = _ttof(responseTokens[1]) * 10000;
	AlignResult.resultY = _ttof(responseTokens[2]) * 10000;
	AlignResult.resultT = _ttof(responseTokens[3]) * 10000;

	theApp.m_pEqIf->m_pMNetH->SetAlignResult(eWordType_TrayAlignResult1 + m_iAlignTypeNum, &AlignResult);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("%s Result : %s[%d]"), AlignTypeName[m_iAlignType], PLC_ResultValue[AlignResult.resultValue], AlignResult.resultValue));
	Delay(10, TRUE);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayAlignEnd1 + m_iAlignTypeNum, OffSet_0, TRUE);
	theApp.m_lastAlignVec[m_iAlignNum][PanelNum1].Reset();

	//hdm test
	if (m_bstart)
		theApp.m_AlignThread[m_iAlignType]->TrayCheckNTrayAlignGrabMethod(PanelNum1);
}

void CAlignManager::AlignTrayLowerAlignGrabEnd(CString strContents)
{
	TrayLowerAlignResult AlignResult;
	CString sendMsg;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	sendMsg.Format(_T("%d,"), MC_GRAB_END_RECEIVE);
	SocketSendto(m_iAlignNum, sendMsg, MC_GRAB_END_RECEIVE);

	//종류				Comment				PanelID		PanelID		Result			X	Y	T	X	Y	T
	//Tray Lower Align	11(VS_GRAB_END)		PanelID_1	PanelID_2	1 = ok, 2 = ng	값	값	값	값	값	값

	AlignResult.resultValue = responseTokens[2] == _T("0") ? m_codeOk : m_codeFail;
	AlignResult.resultX1 = _ttof(responseTokens[3]) * 10000;
	AlignResult.resultY1 = _ttof(responseTokens[4]) * 10000;
	AlignResult.resultT1 = _ttof(responseTokens[5]) * 10000;
	AlignResult.resultX2 = _ttof(responseTokens[6]) * 10000;
	AlignResult.resultY2 = _ttof(responseTokens[7]) * 10000;
	AlignResult.resultT2 = _ttof(responseTokens[8]) * 10000;

	theApp.m_pEqIf->m_pMNetH->SetTrayLowerAlignResult(eWordType_TrayLowerAlignResult1 + m_iAlignTypeNum, &AlignResult);
	LogWrite(m_iAlignNum, CStringSupport::FormatString(_T("%s Result : %s[%d]"), AlignTypeName[m_iAlignType], PLC_ResultValue[AlignResult.resultValue], AlignResult.resultValue));
	Delay(10, TRUE);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_TrayLowerAlignEnd1 + m_iAlignTypeNum, OffSet_0, TRUE);
	theApp.m_lastAlignVec[m_iAlignNum][PanelNum1].Reset();


	//hdm test
	if (m_bstart)
		theApp.m_AlignThread[m_iAlignNum]->TrayLowerAlignGrabMethod(PanelNum1, m_iAlignTypeNum);
}

void CAlignManager::AlignDataSum(int iChNum, int iResultValue, int iInspSection)
{
	if (m_iAlignTypeNum == 0)
	{
#if _SYSTEM_AMT_AFT_
		int iIndexZone = (iChNum - 1) / 4;
		int iPanelNum = (iChNum - 1) % MaxZone;
#else
		int iPanelNum, iIndexZone;

		iPanelNum = (iChNum - 1) % 2;

		if (iPanelNum == PanelNum1)
			iIndexZone = iChNum / 2;
		else
			iIndexZone = (iChNum / 2) - 1;
#endif
		if (iInspSection == Machine_AOI || iInspSection == Machine_GAMMA)
		{
			if (iResultValue == m_codeFail)
			{
				theApp.m_shiftProduction[iIndexZone].m_AlignResult[theApp.m_lastShiftIndex]++;
				theApp.m_UiShiftProduction[iIndexZone].m_AlignResult[theApp.m_lastShiftIndex]++;

				theApp.m_shift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignResult[theApp.m_lastShiftIndex]++;
				theApp.m_UiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignResult[theApp.m_lastShiftIndex]++;

				theApp.m_shiftProduction[iIndexZone].m_AlignShiftNg[theApp.m_lastShiftIndex][iPanelNum]++;
				theApp.m_UiShiftProduction[iIndexZone].m_AlignShiftNg[theApp.m_lastShiftIndex][iPanelNum]++;

				theApp.m_shift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftNg[theApp.m_lastShiftIndex][iPanelNum]++;
				theApp.m_UiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftNg[theApp.m_lastShiftIndex][iPanelNum]++;
			}
			else
			{
				theApp.m_shiftProduction[iIndexZone].m_AlignShiftGood[theApp.m_lastShiftIndex][iPanelNum]++;
				theApp.m_UiShiftProduction[iIndexZone].m_AlignShiftGood[theApp.m_lastShiftIndex][iPanelNum]++;

				theApp.m_shift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftGood[theApp.m_lastShiftIndex][iPanelNum]++;
				theApp.m_UiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftGood[theApp.m_lastShiftIndex][iPanelNum]++;
			}

			theApp.AlignDataSave(theApp.m_lastShiftIndex);
		}
		else
		{
#if _SYSTEM_AMT_AFT_
			int iPanelNum = (iChNum - 1) % 2;

			if (iResultValue == m_codeFail)
			{
				theApp.m_ULDshiftProduction[iPanelNum].m_AlignResult[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShiftProduction[iPanelNum].m_AlignResult[theApp.m_lastShiftIndex]++;

				theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignResult[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignResult[theApp.m_lastShiftIndex]++;

				theApp.m_ULDshiftProduction[iPanelNum].m_AlignShiftNg[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShiftProduction[iPanelNum].m_AlignShiftNg[theApp.m_lastShiftIndex]++;

				theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftNg[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftNg[theApp.m_lastShiftIndex]++;
			}
			else
			{
				theApp.m_ULDshiftProduction[iPanelNum].m_AlignShiftGood[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShiftProduction[iPanelNum].m_AlignShiftGood[theApp.m_lastShiftIndex]++;

				theApp.m_ULDshift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftGood[theApp.m_lastShiftIndex]++;
				theApp.m_ULDUiShift_TimeProduction[theApp.m_iTimeInspectNum].m_AlignShiftGood[theApp.m_lastShiftIndex]++;
			}

			theApp.ULDAlignDataSave(theApp.m_lastShiftIndex);
#endif
		}
	}
}

void CAlignManager::OnEvent(UINT uEvent, LPVOID lpvData)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	switch (uEvent)
	{
	case EVT_CONDROP:
		LogWrite(m_iAlignNum, _T("Align PC Connect Drop"));
		break;
 	case EVT_CONSUCCESS:
		LogWrite(m_iAlignNum, _T("Align PC Connect Success"));
		break;
	case EVT_ZEROLENGTH:
		LogWrite(m_iAlignNum, _T("Align PC EVT_ZEROLENGTH"));
		break;
	case EVT_CONFAILURE:
		LogWrite(m_iAlignNum, _T("Align PC EVT_CONFAILURE"));
		break;
	default:
		LogWrite(m_iAlignNum, _T("Unknown Socket event"));
		break;
	}
}