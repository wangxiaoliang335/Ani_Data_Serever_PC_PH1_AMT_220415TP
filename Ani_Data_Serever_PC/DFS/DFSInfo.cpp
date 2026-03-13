#include "stdafx.h"
#include "DFSInfo.h"
#include "Main/Ani_Data_Serever_PC.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDFSInfo::CDFSInfo(){
	Clear();
}
CDFSInfo::~CDFSInfo(){
}

void CDFSInfo::Clear()
{
	ClearPanelInfo();
	m_mapPanelDefect.clear();
	m_mapDefectCodeList.clear();
}

void CDFSInfo::ClearPanelInfo()
{
	m_HeaderInfo = {};
	m_PanelDataBegin = {};
	m_EQPDataInfo = {};
	m_PanelSummaryInfo = {};
	m_PanelSummaryInfo_OP = {};
	m_DefectDataList.clear();
	m_ULD_DefectDataList.clear();
	m_LUMITOP_DefectDataList.clear();
}

#if _SYSTEM_AMTAFT_

BOOL CDFSInfo::AMTAFTSavePanelDFS_SUM(DfsDataValue& DfsInfo, CString strPanelID, CString strFpcID, CString strVisionPath, CString strViewingPath, CString strLumitopPath, CString strOpvPath, CString strSumPath)
{
	CStdioFile sFile;
	CDFSInfo AoiDfsData, ViewingDfsData, LumitopDfsData, OpvDfsData, PGDfsData;
	int iCount = 0;

	/*if (!OpvDfsData.LoadPanelDFSInfo(strOpvPath, OPVdfs))
	{
		theApp.m_pFTPLog->LOG_INFO(_T("Opv File Path Error : %s,"), strOpvPath);
	}*/

	//if (!AoiDfsData.LoadPanelDFSInfo(strVisionPath, AOIdfs))
	//{
	//	theApp.m_pFTPLog->LOG_INFO(_T("Vision File Path Error : %s,"), strVisionPath);
	//}

	if (!ViewingDfsData.LoadPanelDFSInfo(strViewingPath, VIEWdfs))
	{
		//theApp.m_pFTPLog->LOG_INFO(_T("Viewing Angle File Path Error"), strViewingPath);
	}

	// 点灯缺陷详情（AOI 缺陷）优先从 MySQL，否则从 CSV 读取
	if (AoiDfsData.m_DefectDataList.empty())
	{
		// MySQL 没有数据，从 CSV 读取 AOI 缺陷
		if (!AoiDfsData.LoadPanelDFSInfo(strVisionPath, AOIdfs))
		{
			theApp.m_pFTPLog->LOG_INFO(_T("Vision File Path Error : %s,"), strVisionPath);
		}
	}
	else
	{
		// MySQL 有数据，完全使用 MySQL
		theApp.m_pFTPLog->LOG_INFO(_T("AOI defects (lighting) loaded from MySQL, not reading CSV"));
	}

	if (theApp.m_iMachineType == SetAFT)
	{
		// LUMITOP 色度数据从 CSV 读取
		if (!LumitopDfsData.LoadPanelDFSInfo(strLumitopPath, LUMITOPdfs))
		{
			theApp.m_pFTPLog->LOG_INFO(_T("Lumitop File Path Error : %s,"), strLumitopPath);
		}
	}

	// 여기서 PaenlSummaryData를 add해줍니다.
	// PanelSummaryData 에서 Main 및 Sub 코드를 화면검사로 할지 작업자판정으로 할지는 알아보고 해야댐 우선 작업자껄로
	DFSDefectBeginLoad(strVisionPath, _T("AOI"), TRUE);
	DFSDefectBeginLoad_OP(strOpvPath, _T("OPV"), TRUE);
	m_PanelDataBegin.strPanel_ID = m_PanelSummaryInfo.strPanelID = strPanelID;

	AddTotalDefectListInfo(AoiDfsData.m_DefectDataList, ViewingDfsData.m_DefectDataList, LumitopDfsData.m_LUMITOP_DefectDataList, OpvDfsData.m_ULD_DefectDataList);

	if (sFile.Open(strSumPath, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		theApp.m_pFTPLog->LOG_INFO(_T("SumPath Open Error : %s,"), strSumPath);
		return FALSE;
	}
	
	// 어짜피 패널에대한 정보는 같으니깐 AOI껄로 하자 왜냐하면 GOOD이면 ULD에서는 DCR을 안찍기 때문에.
	SetBCServerData(strPanelID, Machine_AOI, OpvDfsData);
	//SetEQPDataInfo(CString strRECIPE_NO, CString strAOI_RECIPE_NAME, CString strPG_RECIPE_NAME, CString strTP_RECIPE_NAME, CString strSTART_TIME, CString strEND_TIME, CString strLOAD_STAGE_NO, CString strINSP_STAGE_NO, CString strUNLOAD_STAGE_NO, CString strPROBE_CONTACT_CNT, CString strINDEX_PANEL_GRADE, CString strINDEX_MAIN_CODE, CString strFINAL_PANEL_GRADE, CString strFINAL_MAIN_CODE, CString strOPERATOR_ID)
	SetEQPDataInfo(Int2String(theApp.m_CurrentModel.m_AlignPcCurrentModelNum), AoiDfsData.m_EQPDataInfo.strAOI_RECIPE_NAME,
		theApp.m_CurrentModel.m_AlignPcCurrentModelName, _T(""), _T(""), m_EQPDataInfo.strSTART_TIME, m_EQPDataInfo.strEND_TIME, m_EQPDataInfo.strLOAD_STAGE_NO, AoiDfsData.m_EQPDataInfo.strINSP_STAGE_NO, m_EQPDataInfo.strUNLOAD_STAGE_NO,
		m_EQPDataInfo.strPROBE_CONTACT_CNT, m_PanelSummaryInfo.strPanelGrade, m_PanelSummaryInfo.strMainDefectCode, m_PanelSummaryInfo_OP.strPanelGrade, m_PanelSummaryInfo_OP.strMainDefectCode, m_EQPDataInfo.strOPERATOR_ID);
		

	CString strPanelData = _T("PANEL_ID,OWNER_CODE,OWNER_TYPE,PRODUCT_ID,PROCESS_ID,PRODUCT_GROUP,LOT_ID,CST_ID,GROUP_ID,ARRAY_LOT_ID,ARRAY_GLASS_ID,PRE_PANEL_INFO,PRE_PANEL_ARRAY_REPAIR\n");
	CString strPanelEqpData = _T("RECIPE_NO,AOI_RECIPE_NAME,PLC_RECIPE_NAME,PG_RECIPE_NAME,TP_RECIPE_NAME,START_TIME,END_TIME,LOAD_STAGE_NO,INSP_STAGE_NO,UNLOAD_STAGE_NO,PROBE_CONTACT_CNT,INDEX_PANEL_GRADE,INDEX_MAIN_CODE,FINAL_PANEL_GRADE,FINAL_MAIN_CODE,OPERATOR_ID\n");
	CString strPanelSummary = _T("PANEL_ID,CONTACT_PANEL_GRADE,AOI_PANEL_GRADE,PRE_PANEL_GRADE,DOT_PANEL_GRADE,LUMITOP_PANEL_GRADE,OPV_PANEL_GRADE,OPERATOR_ID\n");
	CString strPanelDefectData = _T("PANEL_ID,DEFECT_DATA_NUM,DEFECT_TYPE,DEFECT_PTRN,DEFECT_CODE,DEFECT_GRADE,IMAGE_DATA,X,Y,SIZE,CAM_INSPECT,Zone");
	CString strLumitopDefectList = _T("PANEL_ID,POINT,X,Y,LUMITOP_PTRN,LV,CIE_X,CIE_Y,CCT,MPCD,MPCD_MIN,MPCD_MAX,MPCD_DIFF,MPCD_CENTER,CCT_CENTER");
	CString strPGValueList = _T("PANEL_ID,VBAT1,VDD1,VCI\n");

	
	m_HeaderInfo.strEQP_Type = theApp.m_strEqpId;
	m_HeaderInfo.strEQP_ID = theApp.m_strEqpId + theApp.m_strEqpNum;
	m_HeaderInfo.strContent = _T("PANEL_DATA/EQP_PANEL_DATA/PANEL_SUMMARY/DEFECT_DATA/SITE_DATA");
	
	m_HeaderInfo.strFile_Ver = _T("0.2");

	//
	//>> psh 0414
	if (m_PanelSummaryInfo.CONTACT_PAENL_GRADE == _T("NG"))
	{
		m_EQPDataInfo.strINDEX_MAIN_CODE = _T("XIMXNI");
	}
	if (m_PanelSummaryInfo.PRE_PAENL_GRADE == _T("NG"))
	{
		m_EQPDataInfo.strINDEX_MAIN_CODE = _T("XIMXPG");
	}
	if (m_PanelSummaryInfo.DOT_PAENL_GRADE == _T("NG"))
	{
		m_EQPDataInfo.strINDEX_MAIN_CODE = _T("XIMXDE");
	}

	sFile.WriteString(_T("HEADER_BEGIN\n"));
	sFile.WriteString(_T("FILE_VERSION:"));
	sFile.WriteString(m_HeaderInfo.strFile_Ver + _T("\n"));
	sFile.WriteString(_T("FILE_CREATED_TIME:"));
	sFile.WriteString(GetDateString6() + _T("\n"));
	sFile.WriteString(_T("EQP_TYPE:"));
	sFile.WriteString(m_HeaderInfo.strEQP_Type + _T("\n"));
	sFile.WriteString(_T("EQP_ID:"));
	sFile.WriteString(m_HeaderInfo.strEQP_ID + _T("\n"));
	sFile.WriteString(_T("CONTENT:"));
	sFile.WriteString(m_HeaderInfo.strContent + _T("\n"));
	sFile.WriteString(_T("HEADER_END\n"));

	sFile.WriteString(_T("\nPANEL_DATA_BEGIN\n"));
	sFile.WriteString(strPanelData);
	sFile.WriteString(GetPanelDatatBegin());
	sFile.WriteString(_T("\nPANEL_DATA_END\n"));

	sFile.WriteString(_T("\nEQP_PANEL_DATA_BEGIN\n"));
	sFile.WriteString(strPanelEqpData);
	sFile.WriteString(GetEQPDataInfo());
	sFile.WriteString(_T("\nEQP_PANEL_DATA_END\n"));

	sFile.WriteString(_T("\nPANEL_SUMMARY_BEGIN\n"));
	sFile.WriteString(strPanelSummary);
	sFile.WriteString(GetPanelSummaryInfo());
	sFile.WriteString(_T("\nPANEL_SUMMARY_END\n"));

	sFile.WriteString(_T("\nDEFECT_DATA_BEGIN\n"));
	sFile.WriteString(strPanelDefectData);

	// data가 없어도 무조건 ,는 남겨야 합니다
	iCount = m_DefectDataList.size(); 
	if (iCount == 0)
	{
		sFile.WriteString(GetDefectListInfo(0, AOIdfs));
	}
	else
	{
		for (int ii = 0; ii < m_DefectDataList.size(); ii++)
			sFile.WriteString(GetDefectListInfo(ii, AOIdfs)); // 시야각쓰면 처리하도록 합니다.
	}

	sFile.WriteString(_T("\nDEFECT_DATA_END\n"));

	sFile.WriteString(_T("\nSITE_DATA_BEGIN\n"));
	sFile.WriteString(strLumitopDefectList);

	// data가 없어도 무조건 ,는 남겨야 합니다
	iCount = m_LUMITOP_DefectDataList.size();
	if (iCount == 0)
	{
		sFile.WriteString(GetDefectListInfo(0, LUMITOPdfs));
	}
	else
	{
		for (int ii = 0; ii < m_LUMITOP_DefectDataList.size(); ii++)
			sFile.WriteString(GetDefectListInfo(ii, LUMITOPdfs));
	}
	
	sFile.WriteString(_T("\nSITE_DATA_END\n"));

	//>>
	sFile.WriteString(_T("\nPG_DATA_BEGIN\n"));
	sFile.WriteString(strPGValueList);
	sFile.WriteString(GetPGDatatBegin());	
	sFile.WriteString(_T("\nPG_DATA_END\n"));

	//<<
	
	sFile.Close();

	return TRUE;
}

BOOL CDFSInfo::DFSDefectBeginLoad(CString strFileName, CString strTypeName, BOOL bTotalDfs)
{
	int iRankCount = 0;
	int iTotalDotCnt(0), iTotalLineCnt(0), iTotalMuraCnt(0), iTotalColorShiftCnt(0);

	CString strInfo, strCode, strGrade;
	CStdioFile sFile;
	CStringArray responseTokens;
	map<CString, map<CString, CString>>::iterator iter;
	map<CString, int>::iterator Codeiter;
	map<CString, CString> mapCode;

	mapCode.clear();
	m_mapPanelDefect.clear();
	m_mapDefectCodeList.clear();

	CString strInspName = strTypeName == _T("AOI") ? _T("OPV") : _T("DEFECT");
	int iNum = strTypeName == _T("AOI") ? AOI : OPV;

	if (sFile.Open(strFileName, CFile::modeRead) == FALSE)
	{
		theApp.m_pTraceLog->LOG_DEBUG(_T("MapFile Path Error : %s,"), strFileName);
		return FALSE;
	}

	while (sFile.ReadString(strInfo))
	{
		if (strInfo.Find(CStringSupport::FormatString(_T("%s_DATA_BEGIN"), strInspName)) != -1)
		{
			sFile.ReadString(strInfo);
			if (strInfo.Find(_T("DEFECT_CODE")) != -1)
			{
				while (sFile.ReadString(strInfo))
				{
					if (strInfo == CStringSupport::FormatString(_T("%s_DATA_END"), strInspName))
						break;
					CStringSupport::GetTokenArray(strInfo, _T(','), responseTokens);
					strGrade = responseTokens[2];
					strCode = responseTokens[4];

					if (bTotalDfs == TRUE)
					{
						for (auto Rank : theApp.m_strVecRank[iNum])
						{
							if (!strCode.CompareNoCase(Rank.first))
							{
								switch (iRankCount)
								{
								case 0: m_PanelSummaryInfo.strMainDefectCode = strCode; m_PanelSummaryInfo.strPanelGrade = strGrade; break;
								case 1: m_PanelSummaryInfo.strSubDefectCode1 = strCode; break;
								case 2: m_PanelSummaryInfo.strSubDefectCode2 = strCode; break;
								case 3: m_PanelSummaryInfo.strSubDefectCode3 = strCode; break;
								case 4: m_PanelSummaryInfo.strSubDefectCode4 = strCode; break;
								case 5: m_PanelSummaryInfo.strSubDefectCode5 = strCode; break;
								}
								iRankCount++;
							}
						}

						for (int ii = 0; ii < DefectTitleMaxCount; ii++)
						{
							for (auto rank : theApp.m_VecDefectList[ii])
							{
								if (!rank.strDefectCode.CompareNoCase(strCode))
								{
									switch (ii)
									{
									case DefectTitleName_1: iTotalDotCnt++; break;
									case DefectTitleName_2: iTotalLineCnt++; break;
									case DefectTitleName_3: iTotalMuraCnt++; break;
									case DefectTitleName_4: iTotalColorShiftCnt++; break;
									}
								}
							}
						}

						m_PanelSummaryInfo.strTotalPointCnt = CStringSupport::FormatString(_T("%d"), iTotalDotCnt);
						m_PanelSummaryInfo.strTotalLineCnt = CStringSupport::FormatString(_T("%d"), iTotalLineCnt);
						m_PanelSummaryInfo.strTotalMuraCnt = CStringSupport::FormatString(_T("%d"), iTotalMuraCnt);
						m_PanelSummaryInfo.strColorShiftDefect = CStringSupport::FormatString(_T("%d"), iTotalColorShiftCnt);
					}
					else
					{
						iter = m_mapPanelDefect.find(strTypeName);
						if (iter != m_mapPanelDefect.end())
							iter->second.insert(make_pair(strCode, strGrade));
						else
						{
							mapCode.insert(make_pair(strCode, strGrade));
							m_mapPanelDefect.insert(make_pair(strTypeName, mapCode));
						}

						Codeiter = m_mapDefectCodeList.find(strCode);
						if (Codeiter != m_mapDefectCodeList.end())
							Codeiter->second++;
						else
							m_mapDefectCodeList.insert(make_pair(strCode, 1));
					}

					responseTokens.RemoveAll();
					mapCode.clear();
					strCode = strGrade = _T("");
				}
			}
		}
	}

	sFile.Close();

	return TRUE;
}


BOOL CDFSInfo::DFSDefectBeginLoad_OP(CString strFileName, CString strTypeName, BOOL bTotalDfs)
{
	int iRankCount = 0;
	int iTotalDotCnt(0), iTotalLineCnt(0), iTotalMuraCnt(0), iTotalColorShiftCnt(0);

	CString strInfo, strCode, strGrade;
	CStdioFile sFile;
	CStringArray responseTokens;
	map<CString, map<CString, CString>>::iterator iter;
	map<CString, int>::iterator Codeiter;
	map<CString, CString> mapCode;

	mapCode.clear();
	m_mapPanelDefect.clear();
	m_mapDefectCodeList.clear();

	CString strInspName = strTypeName == _T("AOI") ? _T("OPV") : _T("DEFECT");
	int iNum = strTypeName == _T("AOI") ? AOI : OPV;

	if (sFile.Open(strFileName, CFile::modeRead) == FALSE)
	{
		theApp.m_pTraceLog->LOG_DEBUG(_T("MapFile Path Error : %s,"), strFileName);
		return FALSE;
	}

	while (sFile.ReadString(strInfo))
	{
		if (strInfo.Find(CStringSupport::FormatString(_T("%s_DATA_BEGIN"), strInspName)) != -1)
		{
			sFile.ReadString(strInfo);
			if (strInfo.Find(_T("DEFECT_CODE")) != -1)
			{
				while (sFile.ReadString(strInfo))
				{
					if (strInfo == CStringSupport::FormatString(_T("%s_DATA_END"), strInspName))
						break;
					CStringSupport::GetTokenArray(strInfo, _T(','), responseTokens);
					strGrade = responseTokens[2];
					strCode = responseTokens[4];

					if (bTotalDfs == TRUE)
					{
						for (auto Rank : theApp.m_strVecRank[iNum])
						{
							if (!strCode.CompareNoCase(Rank.first))
							{
								switch (iRankCount)
								{
								case 0: m_PanelSummaryInfo_OP.strMainDefectCode = strCode; m_PanelSummaryInfo_OP.strPanelGrade = strGrade;  break;
								case 1: m_PanelSummaryInfo_OP.strSubDefectCode1 = strCode; break;
								case 2: m_PanelSummaryInfo_OP.strSubDefectCode2 = strCode; break;
								case 3: m_PanelSummaryInfo_OP.strSubDefectCode3 = strCode; break;
								case 4: m_PanelSummaryInfo_OP.strSubDefectCode4 = strCode; break;
								case 5: m_PanelSummaryInfo_OP.strSubDefectCode5 = strCode; break;
								}
								iRankCount++;
							}
						}

						for (int ii = 0; ii < DefectTitleMaxCount; ii++)
						{
							for (auto rank : theApp.m_VecDefectList[ii])
							{
								if (!rank.strDefectCode.CompareNoCase(strCode))
								{
									switch (ii)
									{
									case DefectTitleName_1: iTotalDotCnt++; break;
									case DefectTitleName_2: iTotalLineCnt++; break;
									case DefectTitleName_3: iTotalMuraCnt++; break;
									case DefectTitleName_4: iTotalColorShiftCnt++; break;
									}
								}
							}
						}

						m_PanelSummaryInfo_OP.strTotalPointCnt = CStringSupport::FormatString(_T("%d"), iTotalDotCnt);
						m_PanelSummaryInfo_OP.strTotalLineCnt = CStringSupport::FormatString(_T("%d"), iTotalLineCnt);
						m_PanelSummaryInfo_OP.strTotalMuraCnt = CStringSupport::FormatString(_T("%d"), iTotalMuraCnt);
						m_PanelSummaryInfo_OP.strColorShiftDefect = CStringSupport::FormatString(_T("%d"), iTotalColorShiftCnt);
					}
					else
					{
						iter = m_mapPanelDefect.find(strTypeName);
						if (iter != m_mapPanelDefect.end())
							iter->second.insert(make_pair(strCode, strGrade));
						else
						{
							mapCode.insert(make_pair(strCode, strGrade));
							m_mapPanelDefect.insert(make_pair(strTypeName, mapCode));
						}

						Codeiter = m_mapDefectCodeList.find(strCode);
						if (Codeiter != m_mapDefectCodeList.end())
							Codeiter->second++;
						else
							m_mapDefectCodeList.insert(make_pair(strCode, 1));
					}

					responseTokens.RemoveAll();
					mapCode.clear();
					strCode = strGrade = _T("");
				}
			}
		}
	}

	sFile.Close();

	return TRUE;
}

BOOL CDFSInfo::VisionLoadPanelDFSInfo(CString strPanelID, int iMachineNum)
{
	CString strInfo;
	CStdioFile sFile;

	CString strFilename = DFS_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\AOI\\") + strPanelID + _T(".csv");

	if (sFile.Open(strFilename, CFile::modeRead) == FALSE)
		return FALSE;

	while (sFile.ReadString(strInfo))
	{
		if (strInfo.Find(_T("OPV_DATA_BEGIN")) != -1)
		{
			if (GetTitleCheck(sFile, MaxSOpvDataListBeginCount))
			{
				sFile.ReadString(strInfo);
				while (strInfo.Find(_T("OPV_DATA_END")) == -1 && !strInfo.IsEmpty())
				{
					//sFile.ReadString(strInfo);
					if (!LoadOpvDataInfo(strInfo, iMachineNum))
						return FALSE;

					sFile.ReadString(strInfo);
				}
			}
		}
	}

	sFile.Close();

	return TRUE;
}

void CDFSInfo::AddDefectCodeResult(CString strPanelID, int iErrorNum, int iTpResult, int iType)
{
	if (iType == Machine_AOI)
	{
		SOpvDataBegin Defect;
		Defect.strPanel_ID = strPanelID;

		if (iErrorNum == m_dfsPreGammaNG)
		{
			Defect.strDefect_Grade = _T("R1");
			Defect.strDefect_code = _T("XIMXPG");
		}
		else if (iErrorNum == m_dfsContactNG)
		{
			Defect.strDefect_Grade = theApp.m_strContactNgGrade;
			Defect.strDefect_code = theApp.m_strContactNgCode;
		}

		if (iTpResult == m_dfsTpNG)
		{
			Defect.strDefect_Grade = _T("R1");
			Defect.strDefect_code = _T("XIMXDE");
		}

		//Defect.strFpc_ID;
		Defect.strTP_Function = _T("");
		Defect.strDefect_Ptn = _T("");
		Defect.strData_X1 = _T("");
		Defect.strGate_Y1 = _T("");
		Defect.strData_X2 = _T("");
		Defect.strGate_Y2 = _T("");
		Defect.strData_X3 = _T("");
		Defect.strGate_Y3 = _T("");
		Defect.strImage = _T("");
		Defect.strGlass_Coordinate_X1 = _T("");
		Defect.strGlass_Coordinate_Y1 = _T("");
		Defect.strGlass_Coordinate_X2 = _T("");
		Defect.strGlass_Coordinate_Y2 = _T("");
		Defect.strGlass_Coordinate_X3 = _T("");
		Defect.strGlass_Coordinate_Y3 = _T("");
		Defect.strInspName = _T("");
	
		m_OpvDataList[Machine_AOI].push_back(Defect);
	}
	else
	{
		SDFSDefectDataBegin Defect;
		Defect.strPANEL_ID = strPanelID;

		if (iErrorNum == m_dfsPreGammaNG)
		{
			Defect.strDEFECT_GRADE = _T("R1");
			Defect.strDEFECT_CODE = _T("XIMXPG");
			Defect.strDEFECT_PTRN = _T("Pregamma");
			Defect.strIMAGE_DATA = _T("999");
			Defect.strDEFECT_TYPE = _T("Pregamma");
			Defect.strSIZE = _T("999");
			Defect.strCAM_INSPECT = _T("9");
		}
		else if (iErrorNum == m_dfsContactNG)
		{
			Defect.strDEFECT_GRADE = theApp.m_strContactNgGrade;
			Defect.strDEFECT_CODE = theApp.m_strContactNgCode;
			Defect.strIMAGE_DATA = _T("999");
			Defect.strSIZE = _T("999");
		}

		if (iTpResult == m_dfsTpNG)
		{
			Defect.strDEFECT_GRADE = _T("R1");
			Defect.strDEFECT_CODE = _T("XIMXDE");
			Defect.strDEFECT_PTRN = _T("DOT");
			Defect.strIMAGE_DATA = _T("999");
			Defect.strDEFECT_TYPE = _T("DOT");
			Defect.strSIZE = _T("999");
			Defect.strCAM_INSPECT = _T("9");
		}

		Defect.strDEFECT_DATA_NUM = _T("");		
		Defect.strX = _T("");
		Defect.strY = _T("");		
		Defect.strZone = _T("");
		
		m_DefectDataList.push_back(Defect);
	}
}

void CDFSInfo::SetEQPDataInfo(CString strRECIPE_NO, CString strAOI_RECIPE_NAME, CString strPLC_RECIPE_NAME, CString strPG_RECIPE_NAME, CString strTP_RECIPE_NAME, CString strSTART_TIME, CString strEND_TIME, CString strLOAD_STAGE_NO, CString strINSP_STAGE_NO, CString strUNLOAD_STAGE_NO, CString strPROBE_CONTACT_CNT, CString strINDEX_PANEL_GRADE, CString strINDEX_MAIN_CODE, CString strFINAL_PANEL_GRADE, CString strFINAL_MAIN_CODE, CString strOPERATOR_ID)
{
	//주석 처리 한것은 PLC 에서 받아서 처리하고 있어서 따로 하지 않아요

	m_EQPDataInfo.strRECIPE_NO = strRECIPE_NO;
	m_EQPDataInfo.strAOI_RECIPE_NAME = strAOI_RECIPE_NAME;
	m_EQPDataInfo.strPLC_RECIPE_NAME = strPLC_RECIPE_NAME;
	m_EQPDataInfo.strPG_RECIPE_NAME = strPG_RECIPE_NAME;
	m_EQPDataInfo.strTP_RECIPE_NAME = strTP_RECIPE_NAME;
	//m_EQPDataInfo.strSTART_TIME = strSTART_TIME;
	//m_EQPDataInfo.strEND_TIME = strEND_TIME;
	m_EQPDataInfo.strLOAD_STAGE_NO = strLOAD_STAGE_NO;
	m_EQPDataInfo.strINSP_STAGE_NO = strINSP_STAGE_NO;
	m_EQPDataInfo.strUNLOAD_STAGE_NO = strUNLOAD_STAGE_NO;
	m_EQPDataInfo.strPROBE_CONTACT_CNT = strPROBE_CONTACT_CNT;
	m_EQPDataInfo.strINDEX_PANEL_GRADE = strINDEX_PANEL_GRADE;
	m_EQPDataInfo.strINDEX_MAIN_CODE = strINDEX_MAIN_CODE;
	m_EQPDataInfo.strFINAL_PANEL_GRADE = strFINAL_PANEL_GRADE;
	m_EQPDataInfo.strFINAL_MAIN_CODE = strFINAL_MAIN_CODE;
	m_EQPDataInfo.strOPERATOR_ID = strOPERATOR_ID;
}

BOOL CDFSInfo::LoadPanelDFSInfo(CString strFilename, int iInspNum)
{
	Delay(500, TRUE);
	CString strInfo, strInspName;
	CStdioFile sFile;
	BOOL bFlag = TRUE;
	int iCount = 0;

	if (iInspNum == AOIdfs || iInspNum == OPVdfs)
		strInspName = _T("DEFECT");
	else
		strInspName = _T("SITE");

	if (sFile.Open(strFilename, CFile::modeRead) == FALSE)
		return FALSE;

	while (sFile.ReadString(strInfo))
	{
		if (iInspNum == AOIdfs)
		{
			if (strInfo.Find(_T("EQP_PANEL_DATA_BEGIN")) != -1)
			{
				if (GetTitleCheck(sFile, MaxSDFSEQPDataBeginCount))
				{
					sFile.ReadString(strInfo);
					if (!LoadEQPDataInfo(strInfo))
						return FALSE;
				}
			}
		}
		if (strInfo.Find(_T("DEFECT_DATA_BEGIN")) != -1)
		{
			if (iInspNum == AOIdfs)
				iCount = MaxSDFSDefectListBeginCount;
			else if (iInspNum == LUMITOPdfs)
				iCount = MaxLUMITOPSDFSDefectListBeginCount;
			else if (iInspNum == OPVdfs)
				iCount = MaxULDSDFSDefectListBeginCount;

			if (GetTitleCheck(sFile, iCount))
			{
				sFile.ReadString(strInfo);
				while (strInfo.Find(_T("DEFECT_DATA_END")) == -1)
				{
					if (!LoadDefectListInfo(strInfo, iInspNum, iCount))
						return FALSE;

					sFile.ReadString(strInfo);
				}
			}
		}

		if (strInfo.Find(_T("SITE_DATA_BEGIN")) != -1)
		{
			if (iInspNum == AOIdfs)
				iCount = MaxSDFSDefectListBeginCount;
			else if (iInspNum == LUMITOPdfs)
				iCount = MaxLUMITOPSDFSDefectListBeginCount;
			else if (iInspNum == OPVdfs)
				iCount = MaxULDSDFSDefectListBeginCount;

			if (GetTitleCheck(sFile, iCount))
			{
				sFile.ReadString(strInfo);
				while (strInfo.Find(_T("SITE_DATA_END")) == -1)
				{
					if (!LoadSITEDATAListInfo(strInfo, iInspNum, iCount))
						return FALSE;

					sFile.ReadString(strInfo);
				}
			}
		}
		
		
	}

	sFile.Close();

	return TRUE;
}

CString CDFSInfo::GetPanelSummaryInfo()
{
	CString strTemp = _T("");

	strTemp += m_PanelSummaryInfo.strPanelID + _T(",");
	strTemp += m_PanelSummaryInfo.CONTACT_PAENL_GRADE + _T(",");
	strTemp += m_PanelSummaryInfo.AOI_PAENL_GRADE + _T(",");
	strTemp += m_PanelSummaryInfo.PRE_PAENL_GRADE + _T(",");
	strTemp += m_PanelSummaryInfo.DOT_PAENL_GRADE + _T(",");
	strTemp += m_PanelSummaryInfo.LUMITOP_PAENL_GRADE + _T(",");
	strTemp += m_PanelSummaryInfo.OPV_PAENL_GRADE + _T(",");
	strTemp += m_PanelSummaryInfo.OPERATOR_ID + _T(",");
	return strTemp;
}

BOOL CDFSInfo::LoadPanelSummaryInfo(CString strPanel)
{
	m_PanelSummaryInfo.strPanelID = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strPanelGrade = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strMainDefectCode = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strSubDefectCode1 = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strSubDefectCode2 = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strSubDefectCode3 = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strSubDefectCode4 = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strSubDefectCode5 = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strTotalPointCnt = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strTotalLineCnt = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strTotalMuraCnt = GetExtractionMsg(strPanel);
	m_PanelSummaryInfo.strColorShiftDefect = GetLastExtractionMsg(strPanel);

	return TRUE;
}

BOOL CDFSInfo::LoadOpvDataInfo(CString strPanel, int iMachineNum)
{
	SOpvDataBegin OpvData;

	OpvData.strPanel_ID = GetExtractionMsg(strPanel);
	OpvData.strFpc_ID = GetExtractionMsg(strPanel);
	OpvData.strDefect_Grade = GetExtractionMsg(strPanel);
	OpvData.strTP_Function = GetExtractionMsg(strPanel);
	OpvData.strDefect_code = GetExtractionMsg(strPanel);
	OpvData.strDefect_Ptn = GetExtractionMsg(strPanel);
	OpvData.strData_X1 = GetExtractionMsg(strPanel);
	OpvData.strGate_Y1 = GetExtractionMsg(strPanel);
	OpvData.strData_X2 = GetExtractionMsg(strPanel);
	OpvData.strGate_Y2 = GetExtractionMsg(strPanel);
	OpvData.strData_X3 = GetExtractionMsg(strPanel);
	OpvData.strGate_Y3 = GetExtractionMsg(strPanel);
	OpvData.strImage = GetExtractionMsg(strPanel);
	OpvData.strGlass_Coordinate_X1 = GetExtractionMsg(strPanel);
	OpvData.strGlass_Coordinate_Y1 = GetExtractionMsg(strPanel);
	OpvData.strGlass_Coordinate_X2 = GetExtractionMsg(strPanel);
	OpvData.strGlass_Coordinate_Y2 = GetExtractionMsg(strPanel);
	OpvData.strGlass_Coordinate_X3 = GetExtractionMsg(strPanel);
	OpvData.strGlass_Coordinate_Y3 = GetLastExtractionMsg(strPanel);

	m_OpvDataList[iMachineNum].push_back(OpvData);

	return TRUE;
}

void CDFSInfo::AddTotalDefectListInfo(vector <SDFSDefectDataBegin> vecAoiData, vector <SDFSDefectDataBegin> vecViewingData, vector <LUMITOP_SDFSDefectDataBegin> vecLumitopData, vector <ULD_SDFSDefectDataBegin> vecOpvData)
{
	for (SDFSDefectDataBegin Defect : vecAoiData)
		m_DefectDataList.push_back(Defect);

	for (SDFSDefectDataBegin Defect : vecViewingData)
		m_DefectDataList.push_back(Defect);

	for (LUMITOP_SDFSDefectDataBegin Defect : vecLumitopData)
		m_LUMITOP_DefectDataList.push_back(Defect);

	for (ULD_SDFSDefectDataBegin Defect : vecOpvData)
		m_ULD_DefectDataList.push_back(Defect);
}

CString CDFSInfo::GetDefectListInfo(int ii, int InspNum)
{
	CString strTemp = _T("\n");

	if (InspNum == AOIdfs)
	{
		if (m_DefectDataList.size() <= ii)
		{
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
		}
		else
		{
			strTemp += m_DefectDataList[ii].strPANEL_ID + _T(",");
			strTemp += Int2String(ii+1) + _T(",");
			strTemp += m_DefectDataList[ii].strDEFECT_TYPE + _T(",");
			strTemp += m_DefectDataList[ii].strDEFECT_PTRN + _T(",");
			strTemp += m_DefectDataList[ii].strDEFECT_CODE + _T(",");
			strTemp += m_DefectDataList[ii].strDEFECT_GRADE + _T(",");
			strTemp += m_DefectDataList[ii].strIMAGE_DATA + _T(",");
			strTemp += m_DefectDataList[ii].strX + _T(",");
			strTemp += m_DefectDataList[ii].strY + _T(",");
			strTemp += m_DefectDataList[ii].strSIZE + _T(",");
			strTemp += m_DefectDataList[ii].strCAM_INSPECT + _T(",");
			strTemp += m_DefectDataList[ii].strZone + _T(",");			
		}
	}
	else if (InspNum == LUMITOPdfs)
	{
		if (m_LUMITOP_DefectDataList.size() <= ii)
		{
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
			strTemp += _T(",");
		}

		else
		{

			strTemp += m_LUMITOP_DefectDataList[ii].strPANEL_ID + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strPOINT + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strX + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strY + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strLUMITOP_PTRN + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strLV + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strCIE_X + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strCIE_Y + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strCCT + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strMPCD + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strMPCD_MIN + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strMPCD_MAX + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strMPCD_DIFF + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strMPCD_CENTER + _T(",");
			strTemp += m_LUMITOP_DefectDataList[ii].strCCT_CENTER;
		}
	}
	else if (InspNum == OPVdfs)
	{
		if (m_ULD_DefectDataList.size() <= ii)
			return _T("");

		strTemp += m_ULD_DefectDataList[ii].strPanel_ID + _T(",");
		strTemp += m_ULD_DefectDataList[ii].strFpc_ID + _T(",");
		strTemp += m_ULD_DefectDataList[ii].strDefect_Grade + _T(",");
		strTemp += m_ULD_DefectDataList[ii].strTP_Function + _T(",");
		strTemp += m_ULD_DefectDataList[ii].strDefect_code + _T(",");
		strTemp += m_ULD_DefectDataList[ii].strDefect_Ptn;
	}

	return strTemp;
}

BOOL CDFSInfo::LoadDefectListInfo(CString strPanel, int InspNum, int iCount)
{
	//if (GetItemCount(strPanel) != MaxSDFSDefectListBeginCount) return FALSE;

	SDFSDefectDataBegin Defect;
	LUMITOP_SDFSDefectDataBegin Lumi_Defect;
	ULD_SDFSDefectDataBegin ULD_Defect;

	if (InspNum == AOIdfs)
	{
		Defect.strPANEL_ID = GetExtractionMsg(strPanel);
		Defect.strDEFECT_DATA_NUM = GetExtractionMsg(strPanel);
		Defect.strDEFECT_TYPE = GetExtractionMsg(strPanel);
		Defect.strDEFECT_PTRN = GetExtractionMsg(strPanel);
		Defect.strDEFECT_CODE = GetExtractionMsg(strPanel);
		Defect.strDEFECT_GRADE = GetExtractionMsg(strPanel);
		Defect.strIMAGE_DATA = GetExtractionMsg(strPanel);
		Defect.strX = GetExtractionMsg(strPanel);
		Defect.strY = GetExtractionMsg(strPanel);
		Defect.strSIZE = GetExtractionMsg(strPanel);
		Defect.strCAM_INSPECT = GetExtractionMsg(strPanel);
		Defect.strZone = GetExtractionMsg(strPanel);		
		Defect.strInspName = InspectName[InspNum]; // DFS 포맷이 아니라 OPV .txt 확인용

		m_DefectDataList.push_back(Defect);
	}
	else if (InspNum == LUMITOPdfs)
	{
		Defect.strPANEL_ID = GetExtractionMsg(strPanel);
		Defect.strDEFECT_DATA_NUM = GetExtractionMsg(strPanel);
		Defect.strDEFECT_TYPE = GetExtractionMsg(strPanel);
		Defect.strDEFECT_PTRN = GetExtractionMsg(strPanel);
		Defect.strDEFECT_CODE = GetExtractionMsg(strPanel);
		Defect.strDEFECT_GRADE = GetExtractionMsg(strPanel);
		Defect.strIMAGE_DATA = GetExtractionMsg(strPanel);
		Defect.strX = GetExtractionMsg(strPanel);
		Defect.strY = GetExtractionMsg(strPanel);
		Defect.strSIZE = GetExtractionMsg(strPanel);
		Defect.strCAM_INSPECT = GetExtractionMsg(strPanel);
		Defect.strZone = GetExtractionMsg(strPanel);

		m_DefectDataList.push_back(Defect);
	}
	else if (InspNum == OPVdfs)
	{
		ULD_Defect.strPanel_ID = GetExtractionMsg(strPanel);
		ULD_Defect.strFpc_ID = GetExtractionMsg(strPanel);
		ULD_Defect.strDefect_Grade = GetExtractionMsg(strPanel);
		ULD_Defect.strTP_Function = GetExtractionMsg(strPanel);
		ULD_Defect.strDefect_code = GetExtractionMsg(strPanel);
		ULD_Defect.strDefect_Ptn = GetLastExtractionMsg(strPanel);
		ULD_Defect.strProcessID = m_PanelDataBegin.strProcess_ID;

		m_ULD_DefectDataList.push_back(ULD_Defect);
	}

	return TRUE;
}



BOOL CDFSInfo::LoadSITEDATAListInfo(CString strPanel, int InspNum, int iCount)
{
	//if (GetItemCount(strPanel) != MaxSDFSDefectListBeginCount) return FALSE;

	SDFSDefectDataBegin Defect;
	LUMITOP_SDFSDefectDataBegin Lumi_Defect;
	ULD_SDFSDefectDataBegin ULD_Defect;

	if (InspNum == AOIdfs)
	{
		Defect.strPANEL_ID = GetExtractionMsg(strPanel);
		Defect.strDEFECT_DATA_NUM = GetExtractionMsg(strPanel);
		Defect.strDEFECT_TYPE = GetExtractionMsg(strPanel);
		Defect.strDEFECT_PTRN = GetExtractionMsg(strPanel);
		Defect.strDEFECT_CODE = GetExtractionMsg(strPanel);
		Defect.strDEFECT_GRADE = GetExtractionMsg(strPanel);
		Defect.strIMAGE_DATA = GetExtractionMsg(strPanel);
		Defect.strX = GetExtractionMsg(strPanel);
		Defect.strY = GetExtractionMsg(strPanel);
		Defect.strSIZE = GetExtractionMsg(strPanel);
		Defect.strCAM_INSPECT = GetExtractionMsg(strPanel);
		Defect.strZone = GetExtractionMsg(strPanel);
		Defect.strInspName = InspectName[InspNum]; // DFS 포맷이 아니라 OPV .txt 확인용

		m_DefectDataList.push_back(Defect);
	}
	else if (InspNum == LUMITOPdfs)
	{
		Lumi_Defect.strPANEL_ID = GetExtractionMsg(strPanel);
		Lumi_Defect.strPOINT = GetExtractionMsg(strPanel);
		Lumi_Defect.strX = GetExtractionMsg(strPanel);
		Lumi_Defect.strY = GetExtractionMsg(strPanel);
		Lumi_Defect.strLUMITOP_PTRN = GetExtractionMsg(strPanel);
		Lumi_Defect.strLV = GetExtractionMsg(strPanel);
		Lumi_Defect.strCIE_X = GetExtractionMsg(strPanel);
		Lumi_Defect.strCIE_Y = GetExtractionMsg(strPanel);
		Lumi_Defect.strCCT = GetExtractionMsg(strPanel);
		Lumi_Defect.strMPCD = GetExtractionMsg(strPanel);
		Lumi_Defect.strMPCD_MIN = GetExtractionMsg(strPanel);
		Lumi_Defect.strMPCD_MAX = GetExtractionMsg(strPanel);
		Lumi_Defect.strMPCD_DIFF = GetExtractionMsg(strPanel);
		Lumi_Defect.strMPCD_CENTER = GetExtractionMsg(strPanel);
		Lumi_Defect.strCCT_CENTER = GetExtractionMsg(strPanel);

		m_LUMITOP_DefectDataList.push_back(Lumi_Defect);
	}
	else if (InspNum == OPVdfs)
	{
		ULD_Defect.strPanel_ID = GetExtractionMsg(strPanel);
		ULD_Defect.strFpc_ID = GetExtractionMsg(strPanel);
		ULD_Defect.strDefect_Grade = GetExtractionMsg(strPanel);
		ULD_Defect.strTP_Function = GetExtractionMsg(strPanel);
		ULD_Defect.strDefect_code = GetExtractionMsg(strPanel);
		ULD_Defect.strDefect_Ptn = GetLastExtractionMsg(strPanel);
		ULD_Defect.strProcessID = m_PanelDataBegin.strProcess_ID;

		m_ULD_DefectDataList.push_back(ULD_Defect);
	}

	return TRUE;
}

void CDFSInfo::IndexZoneInspResultInfo(CString strPanelID)
{
	CStdioFile sFile;
	CString strPath;
	CString strFileNmae[3];

	strFileNmae[0] = DFS_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\AOI\\");
	strFileNmae[1] = DFS_VIEWING_ANGLE_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\VIEWING\\");
	strFileNmae[2] = DFS_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\LUMITOP\\");
	// TWICE 추가안되서 일단 화면검사만적용 
	for (int ii = 0; ii < 1; ii++)
	{
		if (sFile.Open(strPath = CStringSupport::FormatString(_T("%s%s"), strFileNmae[ii], _T("GOOD.txt")), CFile::modeRead | CFile::modeNoTruncate))
		{
			switch (ii)
			{
			case 0: m_strVisionResult = _T("G"); break;
			case 1: m_strViewingResult = _T("G"); break;
			case 2: m_strLumitopResult = _T("G"); break;
			}
		}
		else if (sFile.Open(strPath = CStringSupport::FormatString(_T("%s%s"), strFileNmae[ii], _T("NG.txt")), CFile::modeRead | CFile::modeNoTruncate))
		{
			switch (ii)
			{
			case 0: m_strVisionResult = _T("N"); break;
			case 1: m_strViewingResult = _T("N"); break;
			case 2: m_strLumitopResult = _T("N"); break;
			}
		}
		else
		{
			switch (ii)
			{
			case 0: m_strVisionResult = _T("UNKNOWN"); break;
			case 1: m_strViewingResult = _T("UNKNOWN"); break;
			case 2: m_strLumitopResult = _T("UNKNOWN"); break;
			}
		}
	}

	if (theApp.m_AOIPassMode)
		m_strVisionResult = _T("G");

	if (theApp.m_AnglePassMode)
		m_strViewingResult = _T("G");

	if (theApp.m_LumitopPassMode)
		m_strLumitopResult = _T("G");

	//sFile.Close();
}

BOOL CDFSInfo::PGDfsInfoLoad(CString strPanelID)
{
	CDFSInfo DfsInfo;
	CString strFilePath, strShift, strCode, strTemp;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strFilePath.Format(_T("%s\\%s_%s\\%s.ini"), DATA_PG_DFS_INFO_PATH, theApp.m_strCurrentToday, strShift, strPanelID);
	EZIni ini(strFilePath);

	if (!FileExists(strFilePath))
		return FALSE;

	m_PGDfsInfo.strPanelID = ini[_T("PG_DFS")][_T("PANEL_ID")];
	m_PGDfsInfo.strFpcID = ini[_T("PG_DFS")][_T("FPCB_ID")];
	m_PGDfsInfo.m_strVBIT = ini[_T("PG_DFS")][_T("VBAT")];
	m_PGDfsInfo.m_strVDDI = ini[_T("PG_DFS")][_T("VDDI")];
	m_PGDfsInfo.m_strVCI = ini[_T("PG_DFS")][_T("VCI")];
	m_PGDfsInfo.m_strProgramVersion = ini[_T("PG_DFS")][_T("PROGRAM_VERSION")];

	return TRUE;
}
#else
BOOL CDFSInfo::GammaSavePanelDFS_SUM(CString strPANEL_ID, CString strFpcID, CString strSumPath)
{
	CStdioFile sFile;
	CDFSInfo NullData;

	if (sFile.Open(strSumPath, CFile::modeCreate | CFile::modeWrite) == FALSE)
		return FALSE;

	if (!GammaDfsInfoLoad(strFpcID))
		theApp.m_pFTPLog->LOG_INFO2(_T("Not Exist Gamma Dfs file."));

	//SetPanelDataBegin(strPANEL_ID, _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""));
	//SetEQPDataInfo(Int2String(theApp.m_CurrentModel.m_AlignPcCurrentModelNum), theApp.m_CurrentModel.m_AlignPcCurrentModelName, _T(""), _T(""), _T(""),
	//	_T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""), _T(""));
	SetBCServerData(strPANEL_ID, Machine_GAMMA, NullData);

	CString strPanelData = _T("PANEL_ID,OWNER_CODE,OWNER_TYPE,PRODUCT_ID,PROCESS_ID,PRODUCT_GROUP,LOT_ID,CST_ID,GROUP_ID,ARRAY_LOT_ID,ARRAY_GLASS_ID,VBAT1,VDDI,VCI,PROGRAM_VERSION\n");
	CString strPanelEqpData = _T("RECIPE_NO,RECIPE_NAME,START_TIME,END_TIME,OPERATOR_ID,OPERATION_MODE,UNIT_ID,PROBE_CONTACT_CNT,TACT_TIME,LD_TIME,P_GAMMA_TIME,MAIN_VIEW_TIME,SIDE_VIEW_TIME,TP_TIME,ULD_TIME\n");
	CString strPanelDefectList = _T("PANEL_ID,DEFECT_GRADE,DEFECT_CODE");

	m_HeaderInfo.strEQP_Type = theApp.m_strEqpId;
	m_HeaderInfo.strEQP_ID = theApp.m_strEqpId + theApp.m_strEqpNum;
	m_HeaderInfo.strContent = _T("PANEL_DATA/EQP_PANEL_DATA/DEFECT_DATA");
	m_HeaderInfo.strFile_Ver = _T("1.2");

	sFile.WriteString(_T("HEADER_BEGIN\n"));
	sFile.WriteString(_T("FILE_VERSION:"));
	sFile.WriteString(m_HeaderInfo.strFile_Ver + _T("\n"));
	sFile.WriteString(_T("FILE_CREATED_TIME:"));
	sFile.WriteString(GetDateString6() + _T("\n"));
	sFile.WriteString(_T("EQP_TYPE:"));
	sFile.WriteString(m_HeaderInfo.strEQP_Type + _T("\n"));
	sFile.WriteString(_T("EQP_ID:"));
	sFile.WriteString(m_HeaderInfo.strEQP_ID + _T("\n"));
	sFile.WriteString(_T("CONTENT:"));
	sFile.WriteString(m_HeaderInfo.strContent + _T("\n"));
	sFile.WriteString(_T("HEADER_END\n"));

	sFile.WriteString(_T("\nPANEL_DATA_BEGIN\n"));
	sFile.WriteString(strPanelData);
	sFile.WriteString(GetPanelDatatBegin());
	sFile.WriteString(_T("\nPANEL_DATA_END\n"));

	sFile.WriteString(_T("\nEQP_PANEL_DATA_BEGIN\n"));
	sFile.WriteString(strPanelEqpData);
	sFile.WriteString(GetEQPDataInfo());
	sFile.WriteString(_T("\nEQP_PANEL_DATA_END\n"));

	sFile.WriteString(_T("\nDEFECT_DATA_BEGIN\n"));
	sFile.WriteString(strPanelDefectList);
	sFile.WriteString(_T("\nDEFECT_DATA_END\n"));

	sFile.Close();

	return TRUE;
}

BOOL CDFSInfo::GammaDfsInfoLoad(CString strPanelID)
{
	CDFSInfo DfsInfo;
	CString strFilePath, strShift, strCode, strTemp;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strFilePath.Format(_T("%s\\%s_%s\\%s.ini"), DATA_GAMMA_DFS_INFO_PATH, theApp.m_strCurrentToday, strShift, strPanelID);
	EZIni ini(strFilePath);

	if (!FileExists(strFilePath))
		return FALSE;

	m_GammaDfsInfo.m_strVBIT = ini[_T("GAMMA_DFS")][_T("VBAT")];
	m_GammaDfsInfo.m_strVDDI = ini[_T("GAMMA_DFS")][_T("VDDI")];
	m_GammaDfsInfo.m_strVCI = ini[_T("GAMMA_DFS")][_T("VCI")];
	m_GammaDfsInfo.m_strProgramVersion = ini[_T("GAMMA_DFS")][_T("PROGRAM_VERSION")];

	return TRUE;
}

void CDFSInfo::SetEQPDataInfo(CString strRecipe_No, CString strRecipe_Name, CString strStart_Time, CString strEnd_Time, CString strOperator_ID, CString strOperator_Mode, CString strUnit_ID, CString strStage_ID, CString strProbe_Contact_Cnt, CString strTact_Time, CString strLD_Time, CString strP_Gamma_Time, CString strMainView_Time, CString strSideView_Time, CString strTP_Time, CString strUld_Time)
{
	//주석 처리 한것은 PLC 에서 받아서 처리하고 있어서 따로 하지 않아요
	//m_EQPDataInfo.strRecipe_No = strRecipe_No;
	m_EQPDataInfo.strRecipe_Name = strRecipe_Name;
	//m_EQPDataInfo.strStart_Time = strStart_Time;
	//m_EQPDataInfo.strEnd_Time = strEnd_Time;
	m_EQPDataInfo.strOperator_ID = strOperator_ID;
	m_EQPDataInfo.strOperator_Mode = strOperator_Mode;
	//m_EQPDataInfo.strUnit_ID = strUnit_ID;
	m_EQPDataInfo.strProbe_Contact_Cnt = strProbe_Contact_Cnt;
	//m_EQPDataInfo.strTact_Time = strTact_Time;
	//m_EQPDataInfo.strLD_Time = strLD_Time;
	//m_EQPDataInfo.strP_Gamma_Time = strP_Gamma_Time;
	m_EQPDataInfo.strMainView_Time = strMainView_Time;
	m_EQPDataInfo.strSideView_Time = strSideView_Time;
	//m_EQPDataInfo.strTP_Time = strTP_Time;
	//m_EQPDataInfo.strUld_Time = strUld_Time;
}
#endif

BOOL CDFSInfo::GetTitleCheck(CStdioFile& sFile, int iSize)
{
	CString strInfo;

	sFile.ReadString(strInfo);
	if (GetItemCount(strInfo) != iSize)
		return FALSE;

	return TRUE;
}

CString CDFSInfo::WriteString(CString strWrite){
	if (strWrite == _T("")) return _T("***");
	else return strWrite;
}

int CDFSInfo::GetItemCount(CString strInfo)
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
CString CDFSInfo::GetExtractionMsg(CString& strMsg)
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

CString CDFSInfo::GetLastExtractionMsg(CString& strMsg)
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

BOOL CDFSInfo::CopyImage(CString strFilePath, CString strSendFilePath)
{
	CString strTempFilePath = strFilePath;
	GetPathOnly(strTempFilePath);
	strTempFilePath += _T("\\Image");

	CString strTempFilePath2 = strSendFilePath;
	GetPathOnly(strTempFilePath2);
	strTempFilePath2 += _T("\\Image");
	CreateFolders(strTempFilePath2);


	CFileFind aFile;
	BOOL IsExist = aFile.FindFile(strTempFilePath);
	CString msg, snedFilePath;
	CString strFile = strTempFilePath + _T("\\") + _T("*.*");
	IsExist = aFile.FindFile(strFile);
	if (IsExist)
	{
		IsExist = aFile.FindNextFile();
		while (IsExist)
		{
			if (!aFile.IsDots())
			{
				msg = aFile.GetFileName();
				strTempFilePath = aFile.GetFilePath();

				snedFilePath = strTempFilePath2 + _T("\\") + msg;
				if (!FileExists(strFile))
				{
					//theApp.m_pFTPLog->LOG_INFO2(_T("Not Exist image file."));
					return FALSE;
				}
				else
				{
					::MoveFile(strTempFilePath, snedFilePath);
				}
			}

			IsExist = aFile.FindNextFile();
			Delay(2, TRUE);  //<< 150613 JYLee
		}
		if (!aFile.IsDots())
		{
			msg = aFile.GetFileName();
			strTempFilePath = aFile.GetFilePath();

			snedFilePath = strTempFilePath2 + _T("\\") + msg;
			if (!FileExists(strFile))
			{
				//theApp.m_pFTPLog->LOG_INFO2(_T("Not Exist image file."));
				return FALSE;
			}
			else
			{
				::MoveFile(strTempFilePath, snedFilePath);
			}
		}
		return TRUE;

	}

	return FALSE;
}

BOOL CDFSInfo::CopyImage2(CString strFilePath, CString strSendFilePath)
{
	CString strTempFilePath = strFilePath;
	GetPathOnly(strTempFilePath);
	strTempFilePath += _T("Image");

	CString strTempFilePath2 = strSendFilePath;
	GetPathOnly(strTempFilePath2);	
	CreateFolders(strTempFilePath2);


	CFileFind aFile;
	BOOL IsExist = aFile.FindFile(strTempFilePath);
	CString msg, snedFilePath;
	CString strFile = strTempFilePath + _T("\\") + _T("*.*");
	IsExist = aFile.FindFile(strFile);
	if (IsExist)
	{
		IsExist = aFile.FindNextFile();
		while (IsExist)
		{
			if (!aFile.IsDots())
			{
				msg = aFile.GetFileName();
				strTempFilePath = aFile.GetFilePath();

				snedFilePath = strTempFilePath2 + msg;
				if (!FileExists(strFile))
				{
					//theApp.m_pFTPLog->LOG_INFO2(_T("Not Exist image file."));
					return FALSE;
				}
				else
				{
					::MoveFile(strTempFilePath, snedFilePath);
				}
			}

			IsExist = aFile.FindNextFile();
			Delay(2, TRUE);  //<< 150613 JYLee
		}
		if (!aFile.IsDots())
		{
			msg = aFile.GetFileName();
			strTempFilePath = aFile.GetFilePath();

			snedFilePath = strTempFilePath2 + _T("\\") + msg;
			if (!FileExists(strFile))
			{
				//theApp.m_pFTPLog->LOG_INFO2(_T("Not Exist image file."));
				return FALSE;
			}
			else
			{
				::MoveFile(strTempFilePath, snedFilePath);
			}
		}
		return TRUE;

	}

	return FALSE;
}

CString CDFSInfo::GetPanelDatatBegin()
{
	CString strTemp = _T("");

	strTemp += m_PanelDataBegin.strPanel_ID + _T(",");
	strTemp += m_PanelDataBegin.strOwner_CODE + _T(",");
	strTemp += m_PanelDataBegin.strOwner_Type + _T(",");
	strTemp += m_PanelDataBegin.strProduct_ID + _T(",");
	strTemp += m_PanelDataBegin.strProcess_ID + _T(",");
	strTemp += m_PanelDataBegin.strProduct_Group + _T(",");
	strTemp += m_PanelDataBegin.strLOT_ID + _T(",");
	strTemp += m_PanelDataBegin.strCST_ID + _T(",");
	strTemp += m_PanelDataBegin.strGroup_ID + _T(",");
	strTemp += m_PanelDataBegin.strArray_Lot_ID + _T(",");
#if _SYSTEM_AMTAFT_
	strTemp += m_PanelDataBegin.strArray_Glass_ID + _T(",");
	strTemp += m_PanelDataBegin.strPre_Panel_Info + _T(",");
	strTemp += m_PanelDataBegin.strPre_Panel_Array_Repair;
#else
	strTemp += m_PanelDataBegin.strArray_Glass_ID + _T(",");
	strTemp += m_GammaDfsInfo.m_strVBIT + _T(",");
	strTemp += m_GammaDfsInfo.m_strVDDI + _T(",");
	strTemp += m_GammaDfsInfo.m_strVCI + _T(",");
	strTemp += m_GammaDfsInfo.m_strProgramVersion;
#endif
	return strTemp;
}

BOOL CDFSInfo::LoadPanelDataBegin(CString strPanel)
{
	//if (GetItemCount(strPanel) != MaxSDFSPanelDataBeginCount) return FALSE;

	m_PanelDataBegin.strPanel_ID = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strOwner_CODE = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strOwner_Type = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strProduct_ID = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strProcess_ID = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strProduct_Group = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strLOT_ID = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strCST_ID = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strGroup_ID = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strArray_Lot_ID = GetExtractionMsg(strPanel);
#if _SYSTEM_AMTAFT_
	m_PanelDataBegin.strArray_Glass_ID = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strPre_Panel_Info = GetExtractionMsg(strPanel);
	m_PanelDataBegin.strPre_Panel_Array_Repair = GetLastExtractionMsg(strPanel);
#else
	m_PanelDataBegin.strArray_Glass_ID = GetLastExtractionMsg(strPanel);
#endif

	return TRUE;
}

void CDFSInfo::SetPanelDataBegin(CString strPanel_ID, CString strOwner_CODE, CString strOwner_Type, CString strProduct_ID, CString strProcess_ID, CString strProduct_Group, CString strLOT_ID, CString strCST_ID, CString strGroup_ID, CString strArray_Lot_ID, CString strArray_Glass_ID)
{
	m_PanelDataBegin.strPanel_ID = strPanel_ID;
	m_PanelDataBegin.strOwner_CODE = strOwner_CODE;
	m_PanelDataBegin.strOwner_Type = strOwner_Type;
	m_PanelDataBegin.strProduct_ID = strProduct_ID;
	m_PanelDataBegin.strProcess_ID = strProcess_ID;
	m_PanelDataBegin.strProduct_Group = strProduct_Group;
	m_PanelDataBegin.strLOT_ID = strLOT_ID;
	m_PanelDataBegin.strCST_ID = strCST_ID;
	m_PanelDataBegin.strGroup_ID = strGroup_ID;
#if _SYSTEM_AMTAFT_
	m_PanelDataBegin.strArray_Lot_ID = strArray_Lot_ID;
	m_PanelDataBegin.strArray_Glass_ID = strArray_Glass_ID;
#endif
}

CString CDFSInfo::GetEQPDataInfo()
{
	CString strTemp = _T("");

	theApp.m_pDataStatusLog->LOG_INFO(CStringSupport::FormatString(_T(" GetEQPDataInfo() Start time : %s,%s"), m_EQPDataInfo.strSTART_TIME,GetTimeString5(m_EQPDataInfo.strSTART_TIME)));
	theApp.m_pDataStatusLog->LOG_INFO(CStringSupport::FormatString(_T(" GetEQPDataInfo() End time : %s,%s"), m_EQPDataInfo.strEND_TIME,GetTimeString5(m_EQPDataInfo.strEND_TIME)));

#if _SYSTEM_AMTAFT_
	if (m_EQPDataInfo.strFINAL_PANEL_GRADE == _T(""))
		m_EQPDataInfo.strFINAL_PANEL_GRADE = _T("GOOD");
	if (m_EQPDataInfo.strINDEX_PANEL_GRADE == _T(""))
		m_EQPDataInfo.strINDEX_PANEL_GRADE = _T("GOOD");


	strTemp += m_EQPDataInfo.strRECIPE_NO + _T(",");
	strTemp += m_EQPDataInfo.strAOI_RECIPE_NAME + _T(",");
	//>> 201126 KMYOO
	strTemp += m_EQPDataInfo.strPLC_RECIPE_NAME + _T(",");//<<plc RECIPE NAME
	strTemp += m_PGDfsInfo.m_strProgramVersion + _T(",");//<<PG RECIPE NAME
	
	//strTemp += m_EQPDataInfo.strPG_RECIPE_NAME + _T(",");
	//<<
	strTemp += m_EQPDataInfo.strTP_RECIPE_NAME + _T(",");
	strTemp += GetTimeString5(m_EQPDataInfo.strSTART_TIME) + _T(",");
	strTemp += GetTimeString5(m_EQPDataInfo.strEND_TIME) + _T(",");
	strTemp += m_EQPDataInfo.strLOAD_STAGE_NO + _T(",");
	strTemp += m_EQPDataInfo.strINSP_STAGE_NO + _T(",");
	strTemp += m_EQPDataInfo.strUNLOAD_STAGE_NO + _T(",");
	strTemp += m_EQPDataInfo.strPROBE_CONTACT_CNT + _T(",");
	strTemp += m_EQPDataInfo.strINDEX_PANEL_GRADE + _T(",");
	strTemp += m_EQPDataInfo.strINDEX_MAIN_CODE + _T(",");
	strTemp += m_EQPDataInfo.strFINAL_PANEL_GRADE + _T(",");
	strTemp += m_EQPDataInfo.strFINAL_MAIN_CODE + _T(",");
	strTemp += m_EQPDataInfo.strOPERATOR_ID + _T(",");
#else
	strTemp += m_EQPDataInfo.strRecipe_No + _T(",");
	strTemp += m_EQPDataInfo.strRecipe_Name + _T(",");
	strTemp += GetTimeString5(m_EQPDataInfo.strStart_Time) + _T(",");
	strTemp += GetTimeString5(m_EQPDataInfo.strEnd_Time) + _T(",");
	strTemp += m_EQPDataInfo.strOperator_ID + _T(",");
	strTemp += m_EQPDataInfo.strOperator_Mode + _T(",");
	strTemp += m_EQPDataInfo.strUnit_ID + _T(",");
	strTemp += m_EQPDataInfo.strStage_ID + _T(",");

	strTemp += m_EQPDataInfo.strProbe_Contact_Cnt + _T(",");
	strTemp += m_EQPDataInfo.strTact_Time + _T(",");
	strTemp += GetTimeString5(m_EQPDataInfo.strLD_Time) + _T(",");
	strTemp += m_EQPDataInfo.strPre_Gamma_Time + _T(",");
	strTemp += m_EQPDataInfo.strMainView_Time + _T(",");
	strTemp += m_EQPDataInfo.strSideView_Time + _T(",");
	strTemp += m_EQPDataInfo.strTP_Time + _T(",");
	//strTemp += GetTimeString5(m_EQPDataInfo.strUld_Time);
#endif

	return strTemp;
}

CString CDFSInfo::GetPGDatatBegin()
{
	CString strTemp = _T("");

	strTemp += m_PGDfsInfo.strPanelID + _T(",");
	strTemp += m_PGDfsInfo.m_strVBIT + _T(",");
	strTemp += m_PGDfsInfo.m_strVDDI + _T(",");
	strTemp += m_PGDfsInfo.m_strVCI + _T(",");

	return strTemp;
}

BOOL CDFSInfo::LoadEQPDataInfo(CString strPanel)
{
	//if (GetItemCount(strPanel) != MaxSDFSEQPDataBeginCount) return FALSE;

#if _SYSTEM_AMTAFT_
	m_EQPDataInfo.strRECIPE_NO = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strAOI_RECIPE_NAME = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strPG_RECIPE_NAME = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strTP_RECIPE_NAME = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strSTART_TIME = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strEND_TIME = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strLOAD_STAGE_NO = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strINSP_STAGE_NO = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strUNLOAD_STAGE_NO = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strPROBE_CONTACT_CNT = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strINDEX_PANEL_GRADE = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strINDEX_MAIN_CODE = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strFINAL_PANEL_GRADE = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strFINAL_MAIN_CODE = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strOPERATOR_ID = GetExtractionMsg(strPanel);
#else
	m_EQPDataInfo.strRecipe_No = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strRecipe_Name = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strStart_Time = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strEnd_Time = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strOperator_ID = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strOperator_Mode = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strUnit_ID = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strStage_ID = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strProbe_Contact_Cnt = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strTact_Time = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strLD_Time = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strPre_Gamma_Time = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strMainView_Time = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strSideView_Time = GetExtractionMsg(strPanel);
	m_EQPDataInfo.strTP_Time = GetExtractionMsg(strPanel);
	//m_EQPDataInfo.strUld_Time = GetLastExtractionMsg(strPanel);
#endif

	return TRUE;
}

void CDFSInfo::SetBCServerData(CString strPanel, int iType, CDFSInfo DfsInfo)
{
	CString strTemp, strFilePath, strShift;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	if (iType == Machine_AOI)
		strTemp.Format(_T("%s\\%s\\%s_%s\\"), DATA_SYSTEM_DATA_SUM_PATH, _T("AOI"), theApp.m_strCurrentToday, strShift);
	else if (iType == Machine_ULD)
		strTemp.Format(_T("%s\\%s\\%s_%s\\"), DATA_SYSTEM_DATA_SUM_PATH, _T("ULD"), theApp.m_strCurrentToday, strShift);
	else
		strTemp.Format(_T("%s\\%s_%s\\"), DATA_SYSTEM_DATA_SUM_PATH, theApp.m_strCurrentToday, strShift);

	strFilePath.Format(_T("%s%s.ini"), strTemp, strPanel);
	EZIni ini(strFilePath);

	m_PanelDataBegin.strPanel_ID = strPanel;
	m_PanelDataBegin.strOwner_CODE = ini[_T("JOB_DATA")][_T("Owner_Code")];
	m_PanelDataBegin.strOwner_Type = ini[_T("JOB_DATA")][_T("Owner_Type")];
	m_PanelDataBegin.strProduct_ID = ini[_T("JOB_DATA")][_T("Product_ID")];
	m_PanelDataBegin.strProduct_Group = ini[_T("JOB_DATA")][_T("Product_Group")];
	if (m_PanelDataBegin.strProduct_ID == _T(""))
	{
		theApp.m_pSendDefectCodeLog->LOG_INFO(_T("1. ID : %s,strProduct_ID : %s, strProduct_ID_Before : %s"), m_PanelDataBegin.strPanel_ID, m_PanelDataBegin.strProduct_ID, strProduct_ID_Before);
		m_PanelDataBegin.strProduct_ID = strProduct_ID_Before;
	}
	else
	{
		strProduct_ID_Before = m_PanelDataBegin.strProduct_ID;
		theApp.m_pSendDefectCodeLog->LOG_INFO(_T("2. ID : %s,strProduct_ID : %s, strProduct_ID_Before : %s"), m_PanelDataBegin.strPanel_ID, m_PanelDataBegin.strProduct_ID, strProduct_ID_Before);
	}

	if (m_PanelDataBegin.strOwner_CODE == _T(""))
	{
		theApp.m_pSendDefectCodeLog->LOG_INFO(_T("1. ID : %s,strOwner_CODE : %s, strOwner_CODE_Before : %s"), m_PanelDataBegin.strPanel_ID, m_PanelDataBegin.strOwner_CODE, strOwner_CODE_Before);
		m_PanelDataBegin.strOwner_CODE = strOwner_CODE_Before;
	}
	else
	{
		strOwner_CODE_Before = m_PanelDataBegin.strOwner_CODE;
		theApp.m_pSendDefectCodeLog->LOG_INFO(_T("2. ID : %s,strOwner_CODE : %s, _Before : %s"), m_PanelDataBegin.strPanel_ID, m_PanelDataBegin.strOwner_CODE, strOwner_CODE_Before);
	}

	if (m_PanelDataBegin.strOwner_Type == _T(""))
	{
		theApp.m_pSendDefectCodeLog->LOG_INFO(_T("1. ID : %s,strOwer_Type : %s, strOwer_Type_Before : %s"), m_PanelDataBegin.strPanel_ID, m_PanelDataBegin.strOwner_Type, strOwner_Type_Before);
		m_PanelDataBegin.strOwner_Type = strOwner_Type_Before;
	}
	else
	{
		strOwner_Type_Before = m_PanelDataBegin.strOwner_Type;
		theApp.m_pSendDefectCodeLog->LOG_INFO(_T("2. ID : %s,strOwer_Type : %s, strOwer_Type_Before : %s"), m_PanelDataBegin.strPanel_ID, m_PanelDataBegin.strOwner_Type, strOwner_Type_Before);
	}

	if (iType == Machine_AOI)
	{
		/*if (DfsInfo.m_ULD_DefectDataList.size() > 0)
		{
			for (auto OpvDfs : DfsInfo.m_ULD_DefectDataList)
				m_PanelDataBegin.strProcess_ID = OpvDfs.strProcessID;
		}
		else*/
		{
			if (!theApp.m_strEqpId.CompareNoCase(_T("MFBAP")))
				m_PanelDataBegin.strProcess_ID = _T("1700");
			else if (!theApp.m_strEqpId.CompareNoCase(_T("MFGAP")))
				m_PanelDataBegin.strProcess_ID = _T("1L00");
		}
	}
	else if (iType == Machine_GAMMA)
		m_PanelDataBegin.strProcess_ID = _T("1J00");
}