
#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "DlgMainLog.h"
#include "OpvManager.h"
#include "DlgDefectCount.h"

COpvManager::COpvManager()
{
	m_lastContent.resize(2);
	m_lastResult.resize(2);
	m_lastCommand.resize(2);
	m_lastRequest.resize(2);
}

COpvManager::~COpvManager()
{
}


void COpvManager::SendOpvMessage(CString strMsg, int iPanelNum, int iCommand)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	int iNum = iPanelNum;
	CString strCommand = CStringSupport::FormatString(_T("%c%s%c"), _STX, strMsg, _ETX);
	char *lpCommand = StringToChar(strCommand);
	theApp.m_OpvSocketManager[iNum].WriteComm((BYTE*)lpCommand, strlen(lpCommand), 100L);
	delete lpCommand;

	m_lastContent[iNum] = strMsg;

	if (iNum == CH_1)
		theApp.m_pOpvSendReceiver1Log->LOG_INFO(CStringSupport::FormatString(_T("[MC -> OPV] %s"), strMsg));
	else
		theApp.m_pOpvSendReceiver2Log->LOG_INFO(CStringSupport::FormatString(_T("[MC -> OPV] %s"), strMsg));
}

void COpvManager::OpvLogMessage(CString strContents)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	g_MainLog->m_TouchListBox.InsertString(0, CStringSupport::FormatString(_T("[%s] %s"), GetNowSystemTimeMilliseconds(), strContents));
	theApp.m_pOpvLog->LOG_INFO(strContents);
}

void COpvManager::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
	if (theApp.m_bExitFlag == FALSE)
		return;

	//烹脚吝 楷搬 登绢 甸绢棵版快俊绰 for 巩栏肺 ETX 扁霖栏肺 颇教秦辑 傈何 啊廉棵荐 乐档废 荐沥
	CString strData, m_strHeader, m_strCommand, m_strContents, strParsing;
	int iFind, iFindSTX;
	MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<LPCSTR>(lpBuffer), dwCount, strData.GetBuffer(dwCount + 1), dwCount + 1);
	strData.ReleaseBuffer(dwCount);

	SockAddrIn addrin;
	GetSockName(addrin);

	int Num = ntohs(addrin.GetPort()) == _ttoi(OPV1_PORT_NUM) ? CH_1 : CH_2;

	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strData, _ETX, responseTokens);

	if (responseTokens.GetSize() == 1)
	{
		OpvLogMessage(_T("ETX No Message!!!"));
		return;
	}

	for (int ii = 0; ii < responseTokens.GetSize() - 1; ii++)
	{
		strParsing = responseTokens[ii];

		m_strHeader.Format(_T("%x"), strParsing.GetAt(0));

		UINT iHeader = (UINT)_ttoi(m_strHeader);

		if (iHeader != _STX)
		{
			OpvLogMessage(_T("STX No Message!!!"));
			return;
		}

		iFind = strParsing.Find(',');
		m_strCommand = strParsing.Left(iFind);

		iFindSTX = strParsing.Find((char)_STX);
		m_strCommand = m_strCommand.Mid(iFindSTX + 1, m_strCommand.GetLength());

		int iCommand = _ttoi(m_strCommand);

		m_strContents = strParsing.Mid(iFind + 1, strParsing.GetLength());

		if (Num == CH_1)
			theApp.m_pOpvSendReceiver1Log->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), VS_PacketNameTable[iCommand], strData));
		else
			theApp.m_pOpvSendReceiver2Log->LOG_INFO(CStringSupport::FormatString(_T("[VS -> MC] [Command : %s] ->%s"), VS_PacketNameTable[iCommand], strData));

		

		switch (iCommand)
		{
		case VS_ARE_YOU_THERE:
			theApp.m_OpvSocketManager[Num].m_OpvCheckCount = 0;
			break;
		case VS_PCTIME_REQUEST:
			OpvLogMessage(CStringSupport::FormatString(_T("[VS -> MC] %s"), _T("RCV : VS_PCTIME_REQUEST")));
			OpvPcTimeRequest(Num);
			break;
		case VS_STATE:
			theApp.m_OpvConectStatus[Num] = m_strContents == _T("0") ? FALSE : TRUE;
			OpvLogMessage(CStringSupport::FormatString(_T("[VS -> MC] %s->%s"), _T("RCV : VS_STATE"), theApp.m_OpvConectStatus[Num] == TRUE ? _T("Start") : _T("Stop")));
			break;
		case VS_MODEL_REQUEST:
			OpvLogMessage(CStringSupport::FormatString(_T("[VS -> MC] %s"), _T("RCV : VS_MODEL_REQUEST")));
			OpvModelRequest(Num, theApp.m_CurrentModel.m_AlignPcCurrentModelName);
			break;
		case VS_INSPECTION_RESULT:
			OpvInspectionResult(Num, m_strContents);
			break;
		case VS_OPV_LOGIN_REQUEST:
			OpvLogMessage(CStringSupport::FormatString(_T("[Ch%d] [VS -> MC] %s"), Num + 1, _T("RCV : VS_OPV_LOGIN_REQUEST")));
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OPVLoginOut + Num, OffSet_0, TRUE);
			SendOpvMessage(CStringSupport::FormatString(_T("%d,1"), MC_OPV_LOGIN_DONE), Num, MC_OPV_LOGIN_DONE);
			GetOPID(Num, m_strContents);
			break;
		case VS_OPV_LOGOUT_REQUEST:
			OpvLogMessage(CStringSupport::FormatString(_T("[Ch%d] [VS -> MC] %s"), Num + 1, _T("RCV : VS_OPV_LOGOUT_REQUEST")));
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OPVLoginOut + Num, OffSet_0, FALSE);
			SendOpvMessage(CStringSupport::FormatString(_T("%d,1"), MC_OPV_LOGOUT_DONE), Num, MC_OPV_LOGOUT_DONE);
			break;
		}
	}
}

void COpvManager::OnEvent(UINT uEvent, LPVOID lpvData)
{
	SockAddrIn addrin;
	GetSockName(addrin);

	int Num = ntohs(addrin.GetPort()) == _ttoi(OPV1_PORT_NUM) ? CH_1 : CH_2;

	switch (uEvent)
	{
	case EVT_CONDROP:
		OpvLogMessage(CStringSupport::FormatString(_T("PanelNum %d OPV Connect Drop"), m_iOpvNum + 1));
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OPVLoginOut + m_iOpvNum, OffSet_0, FALSE);
		break;
	case EVT_CONSUCCESS:
		OpvLogMessage(CStringSupport::FormatString(_T("PanelNum %d OPV Connect Success"), Num + 1));
		break;
	case EVT_ZEROLENGTH:
		OpvLogMessage(CStringSupport::FormatString(_T("PanelNum %d OPV EVT_ZEROLENGTH"), Num + 1));
		break;
	case EVT_CONFAILURE:
		OpvLogMessage(CStringSupport::FormatString(_T("PanelNum %d OPV EVT_CONFAILURE"), Num + 1));
		break;
	default:
		OpvLogMessage(CStringSupport::FormatString(_T("PanelNum %d Unknown Socket event"), Num + 1));
		break;
	}
}

BOOL COpvManager::getConectCheck()
{
	SockAddrIn addrin;
	GetSockName(addrin);
	LONG  uAddr = addrin.GetIPAddr();
	if (uAddr == 0)
		return FALSE;	//立加救窃
	else
		return TRUE;	//立加窃
}

bool COpvManager::SocketServerOpen(CString strServerPort)
{
	m_iOpvNum = _ttoi(strServerPort) == _ttoi(OPV1_PORT_NUM) ? CH_1 : CH_2;
	m_bMelsecSimulaion = true;
	SetSmartAddressing(false);
	SetServerState(true);
	bool ret = CreateSocket(strServerPort, AF_INET, SOCK_STREAM, 0);
	if (ret) return WatchComm();
	else return false;
}


int COpvManager::GetItemCount(CString strInfo)
{
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strInfo, _T(','), responseTokens);

	int aa = responseTokens.GetSize();
	int iCount = 0;

	for (int ii = 0; ii < responseTokens.GetSize(); ii++)
	{
		if (responseTokens[ii] == _T(""))
			break;
		else
			iCount++;
	}

	return iCount;
}

BOOL COpvManager::GetTitleCheck(CStdioFile& sFile, int iSize)
{
	CString strInfo;

	sFile.ReadString(strInfo);
	if (GetItemCount(strInfo) != iSize)
		return FALSE;

	return TRUE;
}

CString COpvManager::GetExtractionMsg(CString& strMsg)
{
	int count = strMsg.Find(',');
	CString m = strMsg.SpanExcluding(_T(","));
	if (count == -1)
	{
		strMsg = _T("");
		return m;
	}
	strMsg = strMsg.Mid(count + 1);
	return m;
}

CString COpvManager::GetLastExtractionMsg(CString& strMsg)
{
	int count = strMsg.Find(';');
	CString m = strMsg.SpanExcluding(_T(";"));
	if (count == -1)
	{
		strMsg = _T("");
		return m;
	}
	strMsg = strMsg.Mid(count + 1);
	return m;
}
BOOL COpvManager::LoadDefectListInfo(CString strPanel, int iCount)
{
	
	
	ULD_SDFSDefectDataBegin2 ULD_Defect;

	ULD_Defect.strPanel_ID = GetExtractionMsg(strPanel);
	ULD_Defect.strFpc_ID = GetExtractionMsg(strPanel);
	ULD_Defect.strDefect_Grade = GetExtractionMsg(strPanel);
	ULD_Defect.strTP_Function = GetExtractionMsg(strPanel);
	ULD_Defect.strDefect_code = GetExtractionMsg(strPanel);
	ULD_Defect.strDefect_Ptn = GetLastExtractionMsg(strPanel);

	m_ULD_DefectDataList2.push_back(ULD_Defect);

	return TRUE;
}
void COpvManager::GetOPID(int Num, CString strContents)
{
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);
	m_strOPID = responseTokens[0];
}
void COpvManager::OpvInspectionResult(int Num, CString strContents)
{
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strContents, _T(','), responseTokens);

	CString strPanelID = responseTokens[0];
	int iResult = _ttoi(responseTokens[1]) == 1 ? m_codeOk : m_codeFail;
	CString strOperatorID = responseTokens[2];

	if (theApp.m_OpvPassMode)
		theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAOperatorViewResult + Num, &m_codeOk);
	else
	{
		if (responseTokens[1] == m_codeDfsFileError)
			iResult = m_codeDfsFileError;

		theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_MStageAOperatorViewResult + Num, &iResult);
	}

	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_MStageA_OperatorViewEnd + Num, OffSet_0, TRUE);
	OpvLogMessage(CStringSupport::FormatString(_T("[%s] Ch %d Panel [%s] OPV Result %s"), ULD_PG_IndexName[Num], Num + 1, strPanelID, PLC_ResultValue[iResult]));


	//Test 炼陛 秦焊绊 巩力 乐栏搁 官肺 Thread 窍唱 秦具且波鞍篮单... 瘤陛且鳖...酒 疤唱蓖满篮单 捞扒...
	//lock 栏肺 茄锅 秦焊绊 磐瘤搁 .... 弊嫐 Thread 窍唱 眠啊窍磊 
	m_csData.Lock();
	CString strFilePath, strValue;
	int iDefectTitle;
	DefectSumCountData DefectSumData;
	DefectCountData DefectData;
	DefectSumData.Reset();
	DefectData.Reset();
	map<CString, vector<DefectList>> mapCurHistory;
	map<CString, map<CString, int>>::iterator iterTitle;
	map<CString, vector<DefectList>>::iterator iterHistory;
	map<CString, int> ::iterator iterName;
	map<CString, int> mapInsert;
	vector<DefectList> vceHistory;
	DefectList defectHistory;
	BOOL bFlag = FALSE, bFlag2 = FALSE;
	CStringArray responseTokens2;

	strFilePath = CStringSupport::FormatString(_T("%s\\%s\\%s.ini"), DATA_OPV_DEFECT_CODE_PATH, theApp.m_strCurrentToday, strPanelID);
	EZIni ini(strFilePath);
	std::vector<CString> listOfSectionNames;
	ini.EnumSectionNames(listOfSectionNames);

	for (auto title : listOfSectionNames)
	{
		bFlag = FALSE;
		bFlag2 = FALSE;
		std::vector<CString> listOfKeyNames;
		ini[title].EnumKeyNames(listOfKeyNames);
		vceHistory.clear();
		for (auto name : listOfKeyNames)
		{
			responseTokens2.RemoveAll();
			strValue = ini[title][name];
			for (int ii = 0; ii < DefectTitleMaxCount; ii++)
				if (theApp.m_strDefectTitleName[ii] == title)
				{
					iDefectTitle = ii;
					break;
				}

			iterTitle = theApp.m_mapOpvDefectList[theApp.m_lastShiftIndex].find(title);
			if (iterTitle != theApp.m_mapOpvDefectList[theApp.m_lastShiftIndex].end())
			{
				iterName = iterTitle->second.find(name);
				if (iterName != iterTitle->second.end())
				{
					iterName->second += _ttoi(strValue);

					if (!name.CompareNoCase(_T("OverKill")))
					{
						theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OverKillDefectTotalSum[iDefectTitle] = iterName->second;
						DefectData.m_OverKillDefectTotalSum[iDefectTitle] = _ttoi(strValue);
					}
					else if (!name.CompareNoCase(_T("UnderKill")))
					{
						theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_UnderKillDefectTotalSum[iDefectTitle] = iterName->second;
						DefectData.m_OverKillDefectTotalSum[iDefectTitle] = _ttoi(strValue);
					}
					else if (!name.CompareNoCase(_T("Match")))
					{
						theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_MatchDefectTotalSum[iDefectTitle] = iterName->second;
						DefectData.m_MatchDefectTotalSum[iDefectTitle] = _ttoi(strValue);
					}

				}
				else
				{
					iterTitle->second.insert(make_pair(name, _ttoi(strValue)));

					if (!name.CompareNoCase(_T("OverKill")))
						theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OverKillDefectTotalSum[iDefectTitle] = DefectData.m_OverKillDefectTotalSum[iDefectTitle] = _ttoi(strValue);
					else if (!name.CompareNoCase(_T("UnderKill")))
						theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_UnderKillDefectTotalSum[iDefectTitle] = DefectData.m_UnderKillDefectTotalSum[iDefectTitle] = _ttoi(strValue);
					else if (!name.CompareNoCase(_T("Match")))
						theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_MatchDefectTotalSum[iDefectTitle] = DefectData.m_MatchDefectTotalSum[iDefectTitle] = _ttoi(strValue);

				}
			}
			else
			{
				mapInsert.insert(make_pair(name, _ttoi(strValue)));
				bFlag = TRUE;

				if (!name.CompareNoCase(_T("OverKill")))
					DefectData.m_OverKillDefectTotalSum[iDefectTitle] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OverKillDefectTotalSum[iDefectTitle] = _ttoi(strValue);
				else if (!name.CompareNoCase(_T("UnderKill")))
					DefectData.m_UnderKillDefectTotalSum[iDefectTitle] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_UnderKillDefectTotalSum[iDefectTitle] = _ttoi(strValue);
				else if (!name.CompareNoCase(_T("Match")))
					DefectData.m_MatchDefectTotalSum[iDefectTitle] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_MatchDefectTotalSum[iDefectTitle] = _ttoi(strValue);

			}

			theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_AoiDefectTotalSum[iDefectTitle] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OverKillDefectTotalSum[iDefectTitle] + theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_MatchDefectTotalSum[iDefectTitle];
			theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvDefectTotalSum[iDefectTitle] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OverKillDefectTotalSum[iDefectTitle] + theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_UnderKillDefectTotalSum[iDefectTitle];
			theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_TotalDefectSum[iDefectTitle] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_AoiDefectTotalSum[iDefectTitle] + theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_UnderKillDefectTotalSum[iDefectTitle];

			DefectData.m_AoiDefectTotalSum[iDefectTitle] = DefectData.m_OverKillDefectTotalSum[iDefectTitle] + DefectData.m_MatchDefectTotalSum[iDefectTitle];
			DefectData.m_OpvDefectTotalSum[iDefectTitle] = DefectData.m_OverKillDefectTotalSum[iDefectTitle] + DefectData.m_UnderKillDefectTotalSum[iDefectTitle];
			DefectData.m_TotalDefectSum[iDefectTitle] = DefectData.m_AoiDefectTotalSum[iDefectTitle] + DefectData.m_UnderKillDefectTotalSum[iDefectTitle];

			CStringSupport::GetTokenArray(name, _T(','), responseTokens2);
			if (responseTokens2.GetSize() > 1)
			{
				iterHistory = theApp.m_mapOpvDefectHistory[theApp.m_lastShiftIndex].find(title);
				if (iterHistory != theApp.m_mapOpvDefectHistory[theApp.m_lastShiftIndex].end())
				{
					for (int ii = 0; ii < _ttoi(strValue); ii++)
					{
						defectHistory.strTime = GetDateString6();
						defectHistory.strPanelID = strPanelID;
						defectHistory.strChNum = Num + 1;
						defectHistory.strDefectOpvResult = responseTokens2[0];
						defectHistory.strDefectCode = responseTokens2[1];
						defectHistory.strDefectGrade = responseTokens2[2];
						defectHistory.strDefectDesctiption = theApp.ParsingDefectDesctiption(responseTokens2[1]);
						theApp.m_pTraceLog->LOG_DEBUG(_T("Test description %s, "), defectHistory.strDefectDesctiption);
						defectHistory.strUserID = strOperatorID;
						iterHistory->second.push_back(defectHistory);
						vceHistory.push_back(defectHistory);
					}
				}
				else
				{
					for (int ii = 0; ii < _ttoi(strValue); ii++)
					{
						defectHistory.strTime = GetDateString6();
						defectHistory.strPanelID = strPanelID;
						defectHistory.strChNum = Num + 1;
						defectHistory.strDefectOpvResult = responseTokens2[0];
						defectHistory.strDefectCode = responseTokens2[1];
						defectHistory.strDefectGrade = responseTokens2[2];
						defectHistory.strDefectDesctiption = theApp.ParsingDefectDesctiption(responseTokens2[1]);
						theApp.m_pTraceLog->LOG_DEBUG(_T("Test description %s, "), defectHistory.strDefectDesctiption);
						defectHistory.strUserID = strOperatorID;
						vceHistory.push_back(defectHistory);
						bFlag2 = TRUE;
					}
				}
			}
		}
		if (bFlag == TRUE)
			theApp.m_mapOpvDefectList[theApp.m_lastShiftIndex].insert(make_pair(title, mapInsert));

		if (bFlag2 == TRUE)
			theApp.m_mapOpvDefectHistory[theApp.m_lastShiftIndex].insert(make_pair(title, vceHistory));

		mapCurHistory.insert(make_pair(title, vceHistory));
	}

	if (iResult == m_codeOk)
		theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvOkSum++;
	else
		theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvNgSum++;

	theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvTotalNgSum = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvOkSum + theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvNgSum;

	theApp.InspctionDefectDataSum(DefectData, theApp.m_lastShiftIndex, DefectSumData);
	DefectData.m_strTime = GetDateString6();
	DefectData.m_strPanelID = strPanelID;
	DefectData.m_strOperationID = strOperatorID;
	DefectData.m_strOpvResult = OpvResultName[iResult];
	DefectData.m_iChNum = Num + 1;
	DefectData.m_iTotalMatch = DefectSumData.m_MatchDefectTotalSum;
	DefectData.m_iTotalOverKill = DefectSumData.m_OverKillDefectTotalSum;
	DefectData.m_iTotalUnderKill = DefectSumData.m_UnderKillDefectTotalSum;
	theApp.m_VecDefectHistory[theApp.m_lastShiftIndex].push_back(DefectData);

	OpvDefectCountSave();


	CString FileName, strShift, strTitle, strName;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	FileName.Format(_T("%s%s_Opv_DefectList_%s.csv"), DATA_OPV_DEFECT_LIST_PATH, theApp.m_strCurrentToday, strShift);

	strTitle.Format(_T("Time,CH,PanelID,DefectResult,TotalMatch,TotalOverKill,TotalUnderKill,DotMath,DotOverKill,DotUnderKill, MuraMath, MuraOverKill, MuraUnderKill,ListMath, ListOverKill, ListUnderKill, AppearanceMath, AppearanceOverKill, AppearanceUnderKill, FunctionMath, FunctionOverKill, FunctionUnderKill, OperatorID"));
	strName.Format(_T("%s,%d,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s"),
		DefectData.m_strTime,
		Num + 1,
		strPanelID,
		OpvResultName[iResult],
		DefectData.m_iTotalMatch,
		DefectData.m_iTotalOverKill,
		DefectData.m_iTotalUnderKill,
		DefectData.m_MatchDefectTotalSum[0],
		DefectData.m_OverKillDefectTotalSum[0],
		DefectData.m_UnderKillDefectTotalSum[0],
		DefectData.m_MatchDefectTotalSum[1],
		DefectData.m_OverKillDefectTotalSum[1],
		DefectData.m_UnderKillDefectTotalSum[1],
		DefectData.m_MatchDefectTotalSum[2],
		DefectData.m_OverKillDefectTotalSum[2],
		DefectData.m_UnderKillDefectTotalSum[2],
		DefectData.m_MatchDefectTotalSum[3],
		DefectData.m_OverKillDefectTotalSum[3],
		DefectData.m_UnderKillDefectTotalSum[3],
		DefectData.m_MatchDefectTotalSum[4],
		DefectData.m_OverKillDefectTotalSum[4],
		DefectData.m_UnderKillDefectTotalSum[4],
		strOperatorID);

	theApp.ExcelFileSave(FileName, strTitle, strName);

	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	FileName.Format(_T("%s%s_Opv_DefectHisTory_%s.csv"), DATA_OPV_DEFECT_HISTORY_PATH, theApp.m_strCurrentToday, strShift);
	strTitle.Format(_T("Time,CH,PanelID,DefectTitle,DefectCode,DefectGrade,DefectDesctiption,OpvResult,OperatorID"));
	for (auto DefectTitle : mapCurHistory)
	{
		for (auto Defect : DefectTitle.second)
		{
			strName.Format(_T("%s,%d,%s,%s,%s,%s,%s,%s,%s"),
				Defect.strTime,
				Defect.strChNum,
				Defect.strPanelID,
				DefectTitle.first,
				Defect.strDefectCode,
				Defect.strDefectGrade,
				Defect.strDefectDesctiption,
				Defect.strDefectOpvResult,
				Defect.strUserID);

			theApp.ExcelFileSave(FileName, strTitle, strName);
		}
	}
	m_csData.Unlock();
}

void COpvManager::OpvDefectCountSave()
{
	CString strFIlePath, strShift;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strFIlePath.Format(_T("%s%s_Opv_SumDefectCode_%s.ini"), DATA_OPV_SUM_DEFECT_CODE_PATH, theApp.m_strCurrentToday, strShift);
	EZIni ini(strFIlePath);
	for (int ii = 0; ii < DefectTitleMaxCount; ii++)
	{
		ini[theApp.m_strDefectTitleName[ii]][_T("AOI")] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_AoiDefectTotalSum[ii];
		ini[theApp.m_strDefectTitleName[ii]][_T("OPV")] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvDefectTotalSum[ii];
		ini[theApp.m_strDefectTitleName[ii]][_T("MATH")] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_MatchDefectTotalSum[ii];
		ini[theApp.m_strDefectTitleName[ii]][_T("OVER")] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OverKillDefectTotalSum[ii];
		ini[theApp.m_strDefectTitleName[ii]][_T("UNDER")] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_UnderKillDefectTotalSum[ii];
		ini[theApp.m_strDefectTitleName[ii]][_T("DEFECTSUM")] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_TotalDefectSum[ii];
	}
	ini[_T("TOTALNG")][_T("TOTALSUM")] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvTotalNgSum;
	ini[_T("OK")][_T("TOTALSUM")] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvOkSum;
	ini[_T("NG")][_T("TOTALSUM")] = theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvNgSum;
}

void COpvManager::OpvPcTimeRequest(int Num)
{
	CString sendMsg;
	sendMsg.Format(_T("%d,%s"), MC_PCTIME, GetDateString4());
	SendOpvMessage(sendMsg, Num, MC_PCTIME);
	OpvLogMessage(CStringSupport::FormatString(_T("[MC -> VS] %s->%s"), MC_PacketNameTable[MC_PCTIME], sendMsg));
}

void COpvManager::OpvModelRequest(int Num, CString strModelID)
{
	CString strMsg;
	strMsg.Format(_T("%d,%s"), MC_MODEL, strModelID);
	SendOpvMessage(strMsg, Num, MC_MODEL);
	OpvLogMessage(CStringSupport::FormatString(_T("[MC -> VS] %s->%s"), MC_PacketNameTable[MC_MODEL], strMsg));
}
#endif