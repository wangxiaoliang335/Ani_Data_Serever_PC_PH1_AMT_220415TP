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

///////////////////////////////////////////////////////////////////////////////
// WriteOpvDefectCodeINI
// 功能：写入 OpvDefectCode INI 文件，格式参考 D:\ANI\OpvDefectCode
// 路径：D:\ANI\DataServer\Data\OpvDefectCode\{日期}\{PanelID}.ini
// 格式：
//   [Function]
//   Under,XIMXG1,R1=1
//   UnderKill=1
//
//   [Dot]
//   Under,XPOCPP,R1=2
//   UnderKill=2
//
// 新机器 AOI 检测完成后，OPV 还未复检，所以初始状态：
//   - Under = AOI 检出的缺陷数（作为初始值）
//   - OverKill = 0
//   - UnderKill = 总缺陷数
// OPV 复检后会更新这些值
///////////////////////////////////////////////////////////////////////////////
BOOL CDFSInfo::WriteOpvDefectCodeINI(LPCTSTR strPanelID, const CDefectInfoList& defectList, int nFixtureNo)
{
	if (strPanelID == NULL || _tcslen(strPanelID) == 0)
	{
		//theApp.m_pTestLog->Error(_T("[WriteOpvDefectCodeINI] PanelID 为空，跳过写入"));
		return FALSE;
	}

	// 路径：D:\ANI\DataServer\Data\OpvDefectCode\{日期}\{PanelID}.ini
	// 格式参考老机器：Section = Dot/Line/Mura 等，Key = Under,{Code},{Grade}=数量，Key = UnderKill=总数
	CString strDate = GetDateString2();  // 格式：2026-04-09
	CString strDirPath;
	strDirPath.Format(_T("D:\\ANI\\DataServer\\Data\\OpvDefectCode\\%s"), (LPCTSTR)strDate);

	// 创建目录（如果不存在）
	if (!PathIsDirectory(strDirPath))
	{
		if (!CreateDirectory(strDirPath, NULL))
		{
			//theApp.m_pTestLog->Error(_T("[WriteOpvDefectCodeINI] 创建目录失败: %s"), (LPCTSTR)strDirPath);
			return FALSE;
		}
	}

	CString strFilePath;
	strFilePath.Format(_T("%s\\%s.ini"), (LPCTSTR)strDirPath, strPanelID);

	TRACE(_T("[WriteOpvDefectCodeINI] 开始写入 INI: %s\n"), (LPCTSTR)strFilePath);
	//theApp.m_pTestLog->Info(_T("[WriteOpvDefectCodeINI] 开始写入 INI: %s, 缺陷数量: %d"),
	//	(LPCTSTR)strFilePath, (int)defectList.size());

	// 使用 EZIni 写入
	EZIni ini(strFilePath);

	// 按缺陷类型分组统计
	// Map: 缺陷类型 -> Map<"Code,Grade", 数量>
	CMapStringToString mapUnderCount;  // Key: "Code,Grade", Value: 数量
	CMapStringToString mapDefectType;  // Key: "Code,Grade", Value: 缺陷类型(Section)
	CStringArray arrSections;  // 记录所有缺陷类型(Section)

	for (int i = 0; i < (int)defectList.size(); i++)
	{
		const CDefectInfo& defect = defectList[i];

		// 确定缺陷类型 Section（老机器格式：Dot/Line/Mura/Function 等）
		CString strSection;
		CString strTypeUpper = defect.Type;
		strTypeUpper.MakeUpper();

		if (strTypeUpper.Find(_T("LINE")) >= 0)
			strSection = _T("Line");
		else if (strTypeUpper.Find(_T("MURA")) >= 0)
			strSection = _T("Mura");
		else if (strTypeUpper.Find(_T("DOT")) >= 0)
			strSection = _T("Dot");
		else if (strTypeUpper.Find(_T("BLOCK")) >= 0)
			strSection = _T("Block");
		else if (strTypeUpper.Find(_T("BM")) >= 0)
			strSection = _T("BM");
		else
			strSection = _T("Function");  // 其他类型默认 Function

		// 获取缺陷码和等级
		CString strCode = defect.Code_AOI;
		CString strGrade = defect.Grade_AOI;

		if (strCode.IsEmpty())
			strCode = _T("UNKNOWN");

		// 构建 Key: "Code,Grade"
		CString strKey;
		strKey.Format(_T("%s,%s"), (LPCTSTR)strCode, (LPCTSTR)strGrade);

		// 如果是新的 Section，记录下来
		BOOL bNewSection = TRUE;
		for (int j = 0; j < arrSections.GetSize(); j++)
		{
			if (arrSections[j] == strSection)
			{
				bNewSection = FALSE;
				break;
			}
		}
		if (bNewSection)
			arrSections.Add(strSection);

		// 记录 Code,Grade -> Section 的映射
		mapDefectType.SetAt(strKey, strSection);

		// 统计 Under 数量（初始 Under = AOI 检出数）
		CString strCount;
		if (mapUnderCount.Lookup(strKey, strCount))
		{
			int nCount = _ttoi(strCount) + 1;
			strCount.Format(_T("%d"), nCount);
			mapUnderCount.SetAt(strKey, strCount);
		}
		else
		{
			mapUnderCount.SetAt(strKey, _T("1"));
		}
	}

	// 写入 INI 文件
	// 按 Section 分组写入，格式参考老机器：
	//   [Dot]
	//   Under,XIMXG1,R1=1
	//   UnderKill=1
	for (int s = 0; s < arrSections.GetSize(); s++)
	{
		CString strSection = arrSections[s];
		CString strUnderCount;
		int nTotalUnder = 0;

		// 遍历 mapUnderCount，找到属于当前 Section 的所有缺陷
		POSITION pos = mapUnderCount.GetStartPosition();
		while (pos != NULL)
		{
			CString strKey, strCount;
			mapUnderCount.GetNextAssoc(pos, strKey, strCount);

			CString strDefectSection;
			if (!mapDefectType.Lookup(strKey, strDefectSection))
				continue;

			if (strDefectSection != strSection)
				continue;

			// 解析 Code 和 Grade
			CString strCode, strGrade;
			int nComma = strKey.Find(',');
			if (nComma >= 0)
			{
				strCode = strKey.Left(nComma);
				strGrade = strKey.Mid(nComma + 1);
			}
			else
			{
				strCode = strKey;
				strGrade = _T("");
			}

			int nCount = _ttoi(strCount);
			nTotalUnder += nCount;

			// 写入: Under,{Code},{Grade}={数量}
			CString strKeyName;
			strKeyName.Format(_T("Under,%s,%s"), (LPCTSTR)strCode, (LPCTSTR)strGrade);
			ini[strSection][strKeyName] = nCount;

			TRACE(_T("[WriteOpvDefectCodeINI] [%s] %s=%d\n"),
				(LPCTSTR)strSection, (LPCTSTR)strKeyName, nCount);
		}

		// 写入 UnderKill 总数
		ini[strSection][_T("UnderKill")] = nTotalUnder;

		//theApp.m_pTestLog->Info(_T("[WriteOpvDefectCodeINI] Section [%s] UnderKill=%d"),
		//	(LPCTSTR)strSection, nTotalUnder);
	}

	// 如果没有缺陷，写入 OK 标记（老机器格式）
	if (defectList.empty())
	{
		ini[_T("OK")][_T("Result")] = _T("1");
		//theApp.m_pTestLog->Info(_T("[WriteOpvDefectCodeINI] 无缺陷，写入 OK=1"));
	}

	//theApp.m_pTestLog->Info(_T("[WriteOpvDefectCodeINI] INI 写入完成: %s"), (LPCTSTR)strFilePath);
	TRACE(_T("[WriteOpvDefectCodeINI] INI 写入完成: %s\n"), (LPCTSTR)strFilePath);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// WriteAOICSVFile - 由 DataServer 程序写入 AOI 检测结果 CSV 文件
//   格式完全兼容旧版 Vision PC 生成的 DFS 文件，供 AMTAFTSavePanelDFS_SUM 读取
//
// 参数：
//   inspResult - ICW FN$ 处理得到的检测结果（包含 AOIResult/Code/Grade 等）
//   defectList - 缺陷列表（从数据库查询得到，包含 X/Y/Size/Type 等）
//   nFixtureNo - 治具号（1~4）
//
// 写入路径：DFS_SHARE_PATH + 日期 + \\ + PanelID + \\AOI\\ + PanelID + .csv
///////////////////////////////////////////////////////////////////////////////
BOOL CDFSInfo::WriteAOICSVFile(const CInspectionResult& inspResult, const CDefectInfoList& defectList, int nFixtureNo, LPCTSTR strFpcID)
{
	CStdioFile sFile;
	CString strPanelID = inspResult.UniqueID.IsEmpty() ? inspResult.ScreenID : inspResult.UniqueID;
	CString strDate = GetDateString2();

	TRACE(_T("[WriteAOICSVFile] ==== 开始写入AOI CSV ====\n"));
	//theApp.m_pTestLog->Info(_T("[WriteAOICSVFile] ==== 开始写入AOI CSV ===="));
	//theApp.m_pTestLog->Info(_T("  inspResult.UniqueID=[%s], inspResult.ScreenID=[%s], inspResult.GUID=[%s]"),
	//	(LPCTSTR)inspResult.UniqueID, (LPCTSTR)inspResult.ScreenID, (LPCTSTR)inspResult.GUID);
	//theApp.m_pTestLog->Info(_T("  defectList.size()=%d, nFixtureNo=%d, strFpcID=[%s]"),
	//	(int)defectList.size(), nFixtureNo, (LPCTSTR)(strFpcID ? strFpcID : _T("NULL")));
	TRACE(_T("  inspResult.UniqueID=[%s], inspResult.ScreenID=[%s], inspResult.GUID=[%s]\n"),
		(LPCTSTR)inspResult.UniqueID, (LPCTSTR)inspResult.ScreenID, (LPCTSTR)inspResult.GUID);
	TRACE(_T("  defectList.size()=%d, nFixtureNo=%d, strFpcID=[%s]\n"),
		(int)defectList.size(), nFixtureNo, (LPCTSTR)(strFpcID ? strFpcID : _T("NULL")));

	// 优先使用传入的 FpcID 作为目录/文件名（与 RankThread 读取路径保持一致）
	CString strCsvPanelID;
	if (strFpcID != NULL && _tcslen(strFpcID) > 0)
		strCsvPanelID = strFpcID;
	else
		strCsvPanelID = strPanelID;  // 兼容：未传入时退化为 UniqueID

	// 构造 AOI csv 文件路径（与 RankThread/RankSave 保持一致）
	CString strAOIPath = DFS_SHARE_PATH + strDate + _T("\\") + strCsvPanelID + _T("\\AOI\\") + strCsvPanelID + _T(".csv");

	TRACE(_T("  strCsvPanelID=[%s], strAOIPath=[%s]\n"), (LPCTSTR)strCsvPanelID, (LPCTSTR)strAOIPath);
	//theApp.m_pTestLog->Info(_T("  strCsvPanelID=[%s], strAOIPath=[%s]"), (LPCTSTR)strCsvPanelID, (LPCTSTR)strAOIPath);

	// 创建目录结构
	CString strAoiDir = DFS_SHARE_PATH + strDate + _T("\\") + strCsvPanelID + _T("\\AOI\\");
	CreateFolders(strAoiDir);

	// 打开文件（覆盖写入）
	if (sFile.Open(strAOIPath, CFile::modeCreate | CFile::modeWrite) == FALSE)
	{
		theApp.m_VisionLog->Info(_T("[WriteAOICSVFile] Failed to create file: %s"), strAOIPath);
		TRACE(_T("[WriteAOICSVFile] 文件打开失败: %s\n"), (LPCTSTR)strAOIPath);
		//theApp.m_pTestLog->Info(_T("[WriteAOICSVFile] 文件打开失败: %s"), (LPCTSTR)strAOIPath);
		return FALSE;
	}

	TRACE(_T("[WriteAOICSVFile] 文件打开成功: %s\n"), (LPCTSTR)strAOIPath);
	//theApp.m_pTestLog->Info(_T("[WriteAOICSVFile] 文件打开成功: %s"), (LPCTSTR)strAOIPath);

	// ================================================================
	// 填充 m_DefectDataList（缺陷列表）
	//   注意：DEFECT_TYPE 映射为 "Point"（与旧 Vision PC 一致）
	// ================================================================
	CString strAOIGrade = inspResult.AOIResult.CompareNoCase(_T("OK")) == 0 ? _T("OK") : inspResult.Grade_AOI;
	m_DefectDataList.clear();

	TRACE(_T("[WriteAOICSVFile] 开始遍历缺陷列表, defectList.size()=%d\n"), (int)defectList.size());
	//theApp.m_pTestLog->Info(_T("[WriteAOICSVFile] 开始遍历缺陷列表, defectList.size()=%d"), (int)defectList.size());
	for (int i = 0; i < (int)defectList.size(); i++)
	{
		const CDefectInfo& defect = defectList[i];

		TRACE(_T("  [Defect %d] Type=[%s], Pos_x=%d, Pos_y=%d, Pos_w=%d, Pos_h=%d, TrueSize=%.2f\n"),
			i, (LPCTSTR)defect.Type, defect.Pos_x, defect.Pos_y, defect.Pos_width, defect.Pos_height, defect.TrueSize);
		theApp.m_VisionLog->Info(_T("  [Defect %d] Type=[%s], Pos_x=%d, Pos_y=%d, Pos_w=%d, Pos_h=%d, TrueSize=%.2f"),
			i, (LPCTSTR)defect.Type, defect.Pos_x, defect.Pos_y, defect.Pos_width, defect.Pos_height, defect.TrueSize);

		SDFSDefectDataBegin item;
		item.strPANEL_ID = strCsvPanelID;
		item.strDEFECT_DATA_NUM = Int2String(i + 1);

		// DEFECT_TYPE：旧 Vision PC 使用 "Point"，非 "Dot"
		CString strType = defect.Type;
		strType.MakeUpper();
		if (strType.Find(_T("LINE")) >= 0)
			item.strDEFECT_TYPE = _T("Line");
		else if (strType.Find(_T("MURA")) >= 0)
			item.strDEFECT_TYPE = _T("Mura");
		else if (strType.Find(_T("DOT")) >= 0)
			item.strDEFECT_TYPE = _T("Point");   // Point = 旧 Vision PC 用法
		else
			item.strDEFECT_TYPE = _T("Other");

		item.strDEFECT_PTRN = defect.PatternName;
		item.strDEFECT_CODE = defect.Code_AOI.IsEmpty() ? _T("XIMXDE") : defect.Code_AOI;
		item.strDEFECT_GRADE = defect.Grade_AOI.IsEmpty() ? strAOIGrade : defect.Grade_AOI;

		// 图像数据（ImagePath → 取文件名记入）
		if (!defect.ImagePath.IsEmpty())
		{
			int nSlash = max(defect.ImagePath.ReverseFind('\\'), defect.ImagePath.ReverseFind('/'));
			if (nSlash >= 0)
				item.strIMAGE_DATA = defect.ImagePath.Mid(nSlash + 1);
			else
				item.strIMAGE_DATA = defect.ImagePath;
		}
		else
		{
			item.strIMAGE_DATA = _T("");
		}

		item.strX = defect.Pos_x > 0 ? Int2String(defect.Pos_x) : _T("");
		item.strY = defect.Pos_y > 0 ? Int2String(defect.Pos_y) : _T("");
		item.strSIZE = defect.TrueSize > 0 ? Int2String((int)defect.TrueSize) : _T("");

		TRACE(_T("  [Defect %d] CSV字段: X=[%s], Y=[%s], SIZE=[%s], CODE=[%s], GRADE=[%s]\n"),
			i, (LPCTSTR)item.strX, (LPCTSTR)item.strY, (LPCTSTR)item.strSIZE,
			(LPCTSTR)item.strDEFECT_CODE, (LPCTSTR)item.strDEFECT_GRADE);
		theApp.m_VisionLog->Info(_T("  [Defect %d] CSV字段: X=[%s], Y=[%s], SIZE=[%s], CODE=[%s], GRADE=[%s]"),
			i, (LPCTSTR)item.strX, (LPCTSTR)item.strY, (LPCTSTR)item.strSIZE,
			(LPCTSTR)item.strDEFECT_CODE, (LPCTSTR)item.strDEFECT_GRADE);

		// CAM_INSPECT=2（与旧 Vision PC 一致）
		item.strCAM_INSPECT = _T("2");

		// Zone = PlatformID（从 IVS_LCD_InspectionResult.PlatformID 获取）
		item.strZone = CStringSupport::FormatString(_T("%d"), inspResult.PlatformID);

		item.strInspName = _T("AOI");

		m_DefectDataList.push_back(item);
	}

	TRACE(_T("[WriteAOICSVFile] 缺陷列表填充完成, m_DefectDataList.size()=%d\n"), (int)m_DefectDataList.size());
	//theApp.m_pTestLog->Info(_T("[WriteAOICSVFile] 缺陷列表填充完成, m_DefectDataList.size()=%d"), (int)m_DefectDataList.size());

	// ================================================================
	// 补充汇总缺陷记录
	//   场景：AOI 检测到 NG（如 Line/Mura）但缺陷坐标异步写入 DB，
	//   导致 QueryDefectsByParentGUID 返回空列表。
	//   此时写入一条汇总记录，包含总等级码和总缺陷码，供 Rank 系统使用。
	// ================================================================
	if (m_DefectDataList.empty())
	{
		CString strAOIRes = inspResult.AOIResult;
		strAOIRes.MakeUpper();
		BOOL bIsNG = (strAOIRes != _T("OK")) && !strAOIRes.IsEmpty();

		if (bIsNG && !inspResult.Code_AOI.IsEmpty())
		{
			// 写入一条汇总缺陷记录（坐标为空，表示无具体坐标数据）
			SDFSDefectDataBegin summaryItem;
			summaryItem.strPANEL_ID = strCsvPanelID;
			summaryItem.strDEFECT_DATA_NUM = _T("1");
			// DEFECT_TYPE：从 AOIResult 推导
			if (strAOIRes.Find(_T("LINE")) >= 0)
				summaryItem.strDEFECT_TYPE = _T("Line");
			else if (strAOIRes.Find(_T("MURA")) >= 0)
				summaryItem.strDEFECT_TYPE = _T("Mura");
			else if (strAOIRes.Find(_T("DOT")) >= 0 || strAOIRes.Find(_T("POINT")) >= 0)
				summaryItem.strDEFECT_TYPE = _T("Point");
			else
				summaryItem.strDEFECT_TYPE = _T("Point");  // 默认 Point
			summaryItem.strDEFECT_PTRN = _T("");
			summaryItem.strDEFECT_CODE = inspResult.Code_AOI;
			summaryItem.strDEFECT_GRADE = inspResult.Grade_AOI.IsEmpty() ? strAOIGrade : inspResult.Grade_AOI;
			summaryItem.strIMAGE_DATA = _T("");
			summaryItem.strX = _T("0");
			summaryItem.strY = _T("0");
			summaryItem.strSIZE = _T("0");
			summaryItem.strCAM_INSPECT = _T("2");
			// Zone = PlatformID（从 IVS_LCD_InspectionResult.PlatformID 获取）
			summaryItem.strZone = CStringSupport::FormatString(_T("%d"), inspResult.PlatformID);
			summaryItem.strInspName = _T("AOI");
			m_DefectDataList.push_back(summaryItem);
		}
	}

	// ================================================================
	// 6. 开始写入 CSV（完全兼容旧版 Vision PC AOI csv 格式）
	//    真实格式（参考 D:\ANI\DataServer\AVX55CW02AA11\AOI\AVX55CW02AA11.csv）：
	//      1. EQP_PANEL_DATA（15列，无 PLC_RECIPE_NAME）
	//      2. DEFECT_DATA（12列，DEFECT_TYPE=Point，X/Y/SIZE 为浮点）
	//      3. OPV_DATA（19列，与 DEFECT_DATA 一一对应）
	// ================================================================

	// --- EQP_PANEL_DATA（15列：去掉 PLC_RECIPE_NAME，与旧 Vision PC 一致）---
	CString strAOIRecipe = theApp.m_CurrentModel.m_AlignPcCurrentModelName;
	CString strStartTime = inspResult.StartTime.GetStatus() == COleDateTime::valid
		? inspResult.StartTime.Format(_T("%Y%m%d%H%M%S")) : GetDateString6();
	CString strEndTime = inspResult.StopTime.GetStatus() == COleDateTime::valid
		? inspResult.StopTime.Format(_T("%Y%m%d%H%M%S")) : GetDateString6();

	sFile.WriteString(_T("EQP_PANEL_DATA_BEGIN\n"));
	sFile.WriteString(_T("RECIPE_NO,AOI_RECIPE_NAME,PG_RECIPE_NAME,TP_RECIPE_NAME,START_TIME,END_TIME,LOAD_STAGE_NO,INSP_STAGE_NO,UNLOAD_STAGE_NO,PROBE_CONTACT_CNT,INDEX_PANEL_GRADE,INDEX_MAIN_CODE,FINAL_PANEL_GRADE,FINAL_MAIN_CODE,OPERATOR_ID\n"));
	CString strEQPLine;
	strEQPLine.Format(_T(",%s,,,,,,%d,,,,%s,%s,%s,%s,\n"),
		(LPCTSTR)strAOIRecipe,
		nFixtureNo,
		(LPCTSTR)strAOIGrade,
		(LPCTSTR)inspResult.Code_AOI,
		(LPCTSTR)strAOIGrade,
		(LPCTSTR)inspResult.Code_AOI);
	sFile.WriteString(strEQPLine);
	sFile.WriteString(_T("EQP_PANEL_DATA_END\n"));

	// --- DEFECT_DATA ---
	sFile.WriteString(_T("\nDEFECT_DATA_BEGIN\n"));
	sFile.WriteString(_T("PANEL_ID,DEFECT_DATA_NUM,DEFECT_TYPE,DEFECT_PTRN,DEFECT_CODE,DEFECT_GRADE,IMAGE_DATA,X,Y,SIZE,CAM_INSPECT,Zone\n"));

	if (m_DefectDataList.empty())
	{
		sFile.WriteString(_T(",,,,,,,,,,,,\n"));  // 12列（含Zone）
	}
	else
	{
		for (size_t ii = 0; ii < m_DefectDataList.size(); ii++)
		{
			SDFSDefectDataBegin& item = m_DefectDataList[ii];

			// X/Y/SIZE：旧 Vision PC 使用浮点格式（如 16732.000000）
			// 从 CDefectInfo 的 Pos_x/Pos_y/TrueSize 转为浮点字符串（玻璃物理坐标）
			// 修复：添加边界检查，防止 defectList 大小小于 m_DefectDataList 时越界
			// 坐标转换：像素坐标 → 玻璃物理坐标
			double fX = 0.0, fY = 0.0, fSize = 0.0;
			if (ii < (size_t)defectList.size())
			{
				// 打印坐标转换参数
				theApp.m_VisionLog->LOG_INFO(CStringSupport::FormatString(
					_T("[WriteAOICSVFile] CoordConvert[%d]: Pixel(%d,%d) -> Physical(fX=%.2f, fY=%.2f) | ")
					_T("GridImage=(%d,%d), PanelPhysical=(%.2f,%.2f), Scale=(%.6f,%.6f)"),
					ii,
					defectList[ii].Pos_x, defectList[ii].Pos_y,
					fX, fY,
					inspResult.GridImageXLen, inspResult.GridImageYLen,
					inspResult.PanelPhysicalXLen, inspResult.PanelPhysicalYLen,
					(inspResult.GridImageXLen > 0) ? (inspResult.PanelPhysicalXLen / (double)inspResult.GridImageXLen) : 1.0,
					(inspResult.GridImageYLen > 0) ? (inspResult.PanelPhysicalYLen / (double)inspResult.GridImageYLen) : 1.0));

				// X 方向：像素坐标 → 玻璃物理坐标
				double fScaleX = (inspResult.GridImageXLen > 0)
					? (inspResult.PanelPhysicalXLen / (double)inspResult.GridImageXLen) : 1.0;
				//fX = (double)defectList[ii].Pos_x * fScaleX;
				fX = ((double)inspResult.GridImageXLen - (double)defectList[ii].Pos_x) * fScaleX;   //中心坐标从左上角换成右上角了

				// Y 方向：像素坐标 → 玻璃物理坐标
				double fScaleY = (inspResult.GridImageYLen > 0)
					? (inspResult.PanelPhysicalYLen / (double)inspResult.GridImageYLen) : 1.0;
				fY = (double)defectList[ii].Pos_y * fScaleY;

				fSize = defectList[ii].TrueSize > 0 ? defectList[ii].TrueSize
					: max((double)defectList[ii].Pos_width, (double)defectList[ii].Pos_height);
			}

			CString strDefectLine;
			strDefectLine.Format(_T("%s,%d,%s,%s,%s,%s,%s,%.2f,%.2f,%.6f,2,%s\n"),
				(LPCTSTR)item.strPANEL_ID,
				ii + 1,
				(LPCTSTR)item.strDEFECT_TYPE,
				(LPCTSTR)item.strDEFECT_PTRN,
				(LPCTSTR)item.strDEFECT_CODE,
				(LPCTSTR)item.strDEFECT_GRADE,
				(LPCTSTR)item.strIMAGE_DATA,
				fY * 1000, fX * 1000, fSize,
				(LPCTSTR)item.strZone);
			sFile.WriteString(strDefectLine);
		}
	}
	sFile.WriteString(_T("DEFECT_DATA_END\n"));

	// --- OPV_DATA（与 DEFECT_DATA 一一对应，19列，字段含义同 OPV 设备）---
	// 列：PANEL_ID,FPC_ID,DEFECT_GRADE,TP_FUNCTION,DEFECT_CODE,DEFECT_PTN,DATA_X1,GATE_Y1,DATA_X2,GATE_Y2,DATA_X3,GATE_Y3,IMAGE,GLASS_COORDINATE_X1,GLASS_COORDINATE_Y1,GLASS_COORDINATE_X2,GLASS_COORDINATE_Y2,GLASS_COORDINATE_X3,GLASS_COORDINATE_Y3（无末尾逗号）
	sFile.WriteString(_T("\nOPV_DATA_BEGIN\n"));
	sFile.WriteString(_T("PANEL_ID,FPC_ID,DEFECT_GRADE,TP_FUNCTION,DEFECT_CODE,DEFECT_PTN,DATA_X1,GATE_Y1,DATA_X2,GATE_Y2,DATA_X3,GATE_Y3,IMAGE,GLASS_COORDINATE_X1,GLASS_COORDINATE_Y1,GLASS_COORDINATE_X2,GLASS_COORDINATE_Y2,GLASS_COORDINATE_X3,GLASS_COORDINATE_Y3\n"));

	if (m_DefectDataList.empty())
	{
		// 19列（无末尾逗号）
		// 19列（无末尾逗号）：18个逗号
		sFile.WriteString(_T(",,,,,,,,,,,,,,,,\n"));
	}
	else
	{
		for (size_t ii = 0; ii < m_DefectDataList.size(); ii++)
		{
			SDFSDefectDataBegin& item = m_DefectDataList[ii];

			// 坐标：优先使用 defectList（真实缺陷坐标），否则使用 m_DefectDataList（汇总坐标 0）
			// DATA_X1/DATA_X2：缺陷起始/结束 X = Pos_x ± Pos_width/2（像素坐标）
			// GATE_Y1/GATE_Y2：缺陷起始/结束 Y = Pos_y ± Pos_height/2（像素坐标）
			// 注意：需要转换为玻璃物理坐标 = 像素坐标 × PanelPhysicalXLen / GridImageXLen
			double fX1 = 0.0, fY1 = 0.0, fX2 = 0.0, fY2 = 0.0;
			CString strImgFile;
			if (ii < (size_t)defectList.size())
			{
				// X 方向：像素坐标 → 玻璃物理坐标
				double fImageX = (double)defectList[ii].Pos_x;
				double fWidthX = (double)defectList[ii].Pos_width / 2.0;
				double fScaleX = (inspResult.GridImageXLen > 0)
					? (inspResult.PanelPhysicalXLen / (double)inspResult.GridImageXLen) : 1.0;
				fX1 = (fImageX - fWidthX) * fScaleX;
				fX2 = (fImageX + fWidthX) * fScaleX;

				// Y 方向：像素坐标 → 玻璃物理坐标
				double fImageY = (double)defectList[ii].Pos_y;
				double fHeightY = (double)defectList[ii].Pos_height / 2.0;
				double fScaleY = (inspResult.GridImageYLen > 0)
					? (inspResult.PanelPhysicalYLen / (double)inspResult.GridImageYLen) : 1.0;
				fY1 = (fImageY - fHeightY) * fScaleY;
				fY2 = (fImageY + fHeightY) * fScaleY;
				if (!defectList[ii].ImagePath.IsEmpty())
				{
					int nLastSlash = max(defectList[ii].ImagePath.ReverseFind('\\'),
						defectList[ii].ImagePath.ReverseFind('/'));
					if (nLastSlash >= 0)
						strImgFile = defectList[ii].ImagePath.Mid(nLastSlash + 1);
					else
						strImgFile = defectList[ii].ImagePath;
				}
			}
			else
			{
				// 汇总缺陷记录：坐标为空（数据库查询结果），使用 strX/strY（均为 "0"）
				// 注：汇总记录本身没有缺陷坐标，无需坐标转换
				if (!item.strX.IsEmpty()) fX1 = fX2 = _ttof(item.strX);
				if (!item.strY.IsEmpty()) fY1 = fY2 = _ttof(item.strY);
			}

			// FPC_ID：从 inspResult.ScreenID 获取
			CString strFpcID = inspResult.ScreenID;

			// 19列，无末尾逗号（与 GetLastExtractionMsg 用 ';' 解析配合）
			CString strOpvLine;
			strOpvLine.Format(_T("%s,%s,%s,***,%s,%s,%.6f,%.6f,%.6f,%.6f,***,***,%s,***,***,***,***,***,***\n"),
				(LPCTSTR)strPanelID,
				(LPCTSTR)strFpcID,
				(LPCTSTR)item.strDEFECT_GRADE,
				(LPCTSTR)item.strDEFECT_CODE,
				(LPCTSTR)item.strDEFECT_PTRN,
				fX1, fY1, fX2, fY2,
				(LPCTSTR)strImgFile);
			sFile.WriteString(strOpvLine);
		}
	}
	sFile.WriteString(_T("OPV_DATA_END\n"));

	sFile.Close();

	theApp.m_VisionLog->Info(_T("[WriteAOICSVFile] AOI CSV written: %s (DefectCount=%d)"),
		strAOIPath, (int)defectList.size());
	//theApp.m_pTestLog->Info(_T("[WriteAOICSVFile] ==== AOI CSV 写入完成 ===="));
	//theApp.m_pTestLog->Info(_T("  文件路径: %s, 缺陷数量: %d"), (LPCTSTR)strAOIPath, (int)defectList.size());
	TRACE(_T("[WriteAOICSVFile] ==== AOI CSV 写入完成 ====\n  文件路径: %s\n  缺陷数量: %d\n"), (LPCTSTR)strAOIPath, (int)defectList.size());

	return TRUE;
}