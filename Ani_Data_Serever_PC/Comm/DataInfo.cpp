#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "DataInfo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDataInfo::CDataInfo()
{
	Clear();
}
CDataInfo::~CDataInfo(){
}

void CDataInfo::Clear()
{
	m_Panel_Info = {};
	m_Panel_Defect.clear();
}

BOOL CDataInfo::LoadDataInfo(CString strFilename)
{
	Clear();
	CString strInfo;
	CStdioFile sFile;
	if (sFile.Open(strFilename, CFile::modeRead) == FALSE)
		return FALSE;
	
	while (sFile.ReadString(strInfo))
	{	
		if (strInfo.Find(_T("<PANEL_INFO>")) != -1)
		{
			if (DFSInfoFinde(sFile))
			{
				sFile.ReadString(strInfo);
				if (!LoadPanelDataInfo(strInfo))
					return FALSE;

				sFile.ReadString(strInfo);
				if (strInfo.Find(_T("<RAW>")) != -1)
				{
					if (DFSInfoFinde(sFile))
					{
						sFile.ReadString(strInfo);
						while (strInfo.Find(_T("</RAW>")) == -1)
						{
							if (!LoadDefectDataInfo(strInfo))
								return FALSE;
							sFile.ReadString(strInfo);
						}
					}
				}
			}
		}
	}

	sFile.Close();
	return TRUE;
}

BOOL CDataInfo::LoadPanelDataInfo(CString strPanel)
{
	int a = GetItemCount(strPanel);
	if (GetItemCount(strPanel) != 5)
		return FALSE;

	m_Panel_Info.strTime = GetExtractionMsg(strPanel);
	m_Panel_Info.strPanel_ID = GetExtractionMsg(strPanel);
	m_Panel_Info.strFpc_ID = GetExtractionMsg(strPanel);
	m_Panel_Info.strDefect_Result = GetExtractionMsg(strPanel);
	m_Panel_Info.strPanel_Grade = GetExtractionMsg(strPanel);
	m_Panel_Info.strPanel_Width = GetExtractionMsg(strPanel);
	m_Panel_Info.strPanel_Hegiht = GetLastExtractionMsg(strPanel);
	
	return TRUE;
}

BOOL CDataInfo::DFSInfoFinde(CStdioFile& sFile)
{
	CString strInfo;

	sFile.ReadString(strInfo);
	if (strInfo.Find(_T(":")) != -1)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CDataInfo::LoadDefectDataInfo(CString strPanel)
{
	int a = GetItemCount(strPanel);
	if (GetItemCount(strPanel) != 7)
		return FALSE;

	SDataDefectInfo defectInfo;
	BOOL bFlag = FALSE;
	defectInfo.strNo = GetExtractionMsg(strPanel);
	defectInfo.strDefect_Code = GetExtractionMsg(strPanel);
	defectInfo.strDefect_Name = GetExtractionMsg(strPanel);
	defectInfo.strDefect_StartX = GetExtractionMsg(strPanel);
	defectInfo.strDefect_StartY = GetExtractionMsg(strPanel);
	defectInfo.strDefect_EndX = GetExtractionMsg(strPanel);
	defectInfo.strDefect_EndY = GetExtractionMsg(strPanel);
	defectInfo.strDefect_Pattern = GetExtractionMsg(strPanel);
	defectInfo.strDefect_Grade = GetLastExtractionMsg(strPanel);

	m_Panel_Defect.push_back(defectInfo);

	return TRUE;
}

int CDataInfo::GetItemCount(CString strInfo){
	int iPos = 0, iCount = 0;
	while ((iPos = strInfo.Find(_T("^"), iPos)) != -1){
		iPos++; iCount++;
	}
	return iCount;
}
CString CDataInfo::GetExtractionMsg(CString& strMsg)
{
	int count = strMsg.Find('^');
	CString m = strMsg.SpanExcluding(_T("^"));
	if (count == -1)
	{
		strMsg = _T("");
		return m;
	}
	strMsg = strMsg.Mid(count + 1);
	return m;
}

CString CDataInfo::GetLastExtractionMsg(CString& strMsg)
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

CString CDataInfo::GetPanelInfo(){
	CString strTemp = _T("");

	strTemp += m_Panel_Info.strTime + _T("^");
	strTemp += m_Panel_Info.strPanel_ID + _T("^");
	strTemp += m_Panel_Info.strFpc_ID + _T("^");
	strTemp += m_Panel_Info.strDefect_Result + _T("^");
	strTemp += m_Panel_Info.strPanel_Grade + _T("^");
	strTemp += m_Panel_Info.strPanel_Width + _T("^");
	strTemp += m_Panel_Info.strPanel_Hegiht + _T("^");
	strTemp += m_Panel_Info.strPreGammaContactStatus + _T("^");
	strTemp += m_Panel_Info.strModel_ID + _T("^");
	strTemp += m_Panel_Info.strIndexNum + _T("^");
	strTemp += m_Panel_Info.strChNum + _T("^");
	strTemp += m_Panel_Info.strVisionResult + _T("^");
	strTemp += m_Panel_Info.strViewingResult + _T("^");
	strTemp += m_Panel_Info.strTpResult + _T(";");

	return strTemp;
}

CString CDataInfo::GetPanelDefectInfo(int i)
{
	if (m_Panel_Defect.size() <= i)
		return _T("");

	CString strTemp = _T("\n");
	
	strTemp += m_Panel_Defect[i].strNo + _T("^");
	strTemp += m_Panel_Defect[i].strInspName + _T("^");
	strTemp += m_Panel_Defect[i].strDefect_Code + _T("^");
	strTemp += m_Panel_Defect[i].strDefect_Name + _T("^");
	strTemp += m_Panel_Defect[i].strDefect_StartX + _T("^");
	strTemp += m_Panel_Defect[i].strDefect_StartY + _T("^");
	strTemp += m_Panel_Defect[i].strDefect_EndX + _T("^");
	strTemp += m_Panel_Defect[i].strDefect_EndY + _T("^");
	strTemp += m_Panel_Defect[i].strDefect_Pattern + _T("^");
	strTemp += m_Panel_Defect[i].strDefect_Grade + _T(";");

	return strTemp;
}

BOOL CDataInfo::SetSaveFile(CString strFIleName)
{
	CStdioFile sFile;
	if (sFile.Open(strFIleName, CFile::modeCreate | CFile::modeWrite) == FALSE)
		return FALSE;

	CString strPanelInfoHeader = _T("TIME^PANEL_ID^FPC_ID^DEFECT_RESULT^PANEL_GRADE^PANEL_WIDTH^PANEL_HEIGHT^PREGAMMA_STATUS^MODEL_ID^INDEX_NUM^CH_NUM^VISION^VIEWING^TP_RESULT:\n");
	CString strRAWInfoHeader = _T("NO^INSPNAME^DEFECT_CODE^DEFECT_NAME^StartX^StartY^EndX^EndY^PATTERN^DEFECT_GRADE:");

	sFile.WriteString(_T("<PANEL_INFO>\n"));
	sFile.WriteString(strPanelInfoHeader);
	sFile.WriteString(GetPanelInfo());
	sFile.WriteString(_T("\n<RAW>\n"));
	sFile.WriteString(strRAWInfoHeader);
	for (int ii = 0; ii < m_Panel_Defect.size(); ii++)
	{
		sFile.WriteString(GetPanelDefectInfo(ii));
	}

	sFile.WriteString(_T("\n</RAW>\n"));
	sFile.WriteString(_T("</PANEL_INFO>\n"));
	sFile.Close();

	return TRUE;
}

DfsDataValue CDataInfo::SetLoadFile(CString strPanelID)
{
	DfsDataValue ResultData;
	CStdioFile sFile;
	CString strFileName = DFS_SHARE_OPV_PATH + strPanelID + _T("\\") + strPanelID + _T(".txt");
	CString strInfo, strResult, strPreGammaContactResult, strTpResult;
	int iPreGammaContactResultPos(0), iTpResultPos(0), iInedexNumPos(0), iChNumPos(0);
	BOOL bCheckFlag1(FALSE), bCheckFlag2(FALSE), bCheckFlag3(FALSE), bCheckFlag4(FALSE);

	BOOL bFlag = TRUE;

	if (sFile.Open(strFileName, CFile::modeRead | CFile::modeNoTruncate) == FALSE)
		return ResultData;

	while (sFile.ReadString(strInfo))
	{
		if (strInfo.Find(_T("<PANEL_INFO>")) != -1)
		{
			sFile.ReadString(strInfo);
			CStringArray responseTokens;
			CStringSupport::GetTokenArray(strInfo, _T('^'), responseTokens);

			int ii = 0;
			while (bFlag)
			{
				if (!responseTokens[ii].CompareNoCase(_T("PREGAMMA_STATUS")))
				{
					bCheckFlag1 = TRUE;
					iPreGammaContactResultPos = ii;
					ii++;
				}
				else if (!responseTokens[ii].CompareNoCase(_T("INDEX_NUM")))
				{
					bCheckFlag2 = TRUE;
					iInedexNumPos = ii;
					ii++;
				}
				else if (!responseTokens[ii].CompareNoCase(_T("CH_NUM")))
				{
					bCheckFlag3 = TRUE;
					iChNumPos = ii;
					ii++;
				}
				// 마지막이라 : 있고 결과값에는 ; 이게있음..당장 좋은게 안떠올라 이렇게함..나중에수정예정 하..Replace로 하자
				else if (!responseTokens[ii].CompareNoCase(_T("TP_RESULT:")))
				{
					bCheckFlag4 = TRUE;
					iTpResultPos = ii;
				}
				else
					ii++;

				if (bCheckFlag1 && bCheckFlag2 && bCheckFlag3 && bCheckFlag4)
					bFlag = FALSE;
			}
			sFile.ReadString(strInfo);

			CStringArray responseTokens2;
			CStringSupport::GetTokenArray(strInfo, _T('^'), responseTokens2);
			
			ResultData.m_PreGammaContactStatus = responseTokens2[iPreGammaContactResultPos];
			ResultData.m_IndexNum = responseTokens2[iInedexNumPos];
			ResultData.m_ChNum = responseTokens2[iChNumPos];
			ResultData.m_TpResult = responseTokens2[iTpResultPos].Left(1);
		}	
	}

	sFile.Close();

	return ResultData;
}