#pragma once
///////////////////////////////////////////////////////////////////////////////
// FILE : CDFSInfo.h
// Header file for CSOT DFS class
//		Version		Updated		 Author		 Note
//      -------     -------      ------      ----
//		   1.0      2018/09/12   Hacy		Create	: CSOT 수정
///////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <map>
#include <afx.h>

using namespace std;


struct SDFSHeaderInfo{
	CString strFile_Ver;
	CString strFile_CreateTime;
	CString strEQP_Type;
	CString strEQP_ID;
	CString strContent;
};

struct SDFAPanel_Data_Begin{
	CString strPanel_ID;
	CString strOwner_CODE;
	CString strOwner_Type;
	CString strProduct_ID;
	CString strProcess_ID;
	CString strProduct_Group;
	CString strLOT_ID;
	CString strCST_ID;
	CString strGroup_ID;
	CString strArray_Lot_ID;
	CString strArray_Glass_ID;
#if _SYSTEM_AMTAFT_
	CString strPre_Panel_Info;
	CString strPre_Panel_Array_Repair;
#endif
};

enum
{
	PanelData_Panel_iD,
	PanelData_Owner_Code,
	PanelData_Owner_Type,
	PanelData_Product_ID,
	PanelData_Process_ID,
	PanelData_Product_Group,
	PanelData_LOT_ID,
	PanelData_CST_ID,
	PanelData_Group_ID,
	PanelData_Array_Lot_ID,
	PanelData_Array_Glass_ID,
#if _SYSTEM_AMTAFT_
	PanelData_Pre_Panel_Info,
	PanelData_Pre_Panel_Array_Repair,
#endif
	MaxSDFSPanelDataBeginCount
};

#if _SYSTEM_AMTAFT_
struct SDFSEQPDataBegin{
	CString strRECIPE_NO;
	CString strAOI_RECIPE_NAME;
	CString strPLC_RECIPE_NAME;
	CString strPG_RECIPE_NAME;
	CString strTP_RECIPE_NAME;
	CString strSTART_TIME;
	CString strEND_TIME;
	CString strLOAD_STAGE_NO;
	CString strINSP_STAGE_NO;
	CString strUNLOAD_STAGE_NO;
	CString strPROBE_CONTACT_CNT;
	CString strINDEX_PANEL_GRADE;
	CString strINDEX_MAIN_CODE;
	CString strFINAL_PANEL_GRADE;
	CString strFINAL_MAIN_CODE;
	CString strOPERATOR_ID;
};

enum
{
	EQPData_Recipe_No,
	EQPData_Recipe_Name,
	EQPData_Start_Time,
	EQPData_End_Time,
	EQPData_Operator_ID,
	EQPData_Operator_Mode,
	EQPData_Load_Stage_No,
	EQPData_Insp_Stage_No,
	EQPData_Unload_Stage_No,
	EQPData_Probe_Contact_Cnt,
	EQPData_Tact_Time,
	EQPData_Insp_Tact_Time,
	EQPData_Pre_Gamma_Time,
	EQPData_Lumitop_Time,
	EQPData_TP_Time,
	MaxSDFSEQPDataBeginCount
};
#else
struct SDFSEQPDataBegin{
	CString strRecipe_No;
	CString strRecipe_Name;
	CString strStart_Time;
	CString strEnd_Time;
	CString strOperator_ID;
	CString strOperator_Mode;
	CString strUnit_ID;
	CString strStage_ID;
	CString strProbe_Contact_Cnt;
	CString strTact_Time;
	CString strLD_Time;
	CString strPre_Gamma_Time;
	CString strMainView_Time;
	CString strSideView_Time;
	CString strTP_Time;
	CString strUld_Time;
};

enum
{
	EQPData_Recipe_No,
	EQPData_Recipe_Name,
	EQPData_Start_Time,
	EQPData_End_Time,
	EQPData_Operator_ID,
	EQPData_Operator_Mode,
	EQPData_Unit_ID,
	EQPData_Stage_ID,
	EQPData_Probe_Contact_Cnt,
	EQPData_Tact_Time,
	EQPData_LD_Time,
	EQPData_P_Gamma_Time,
	EQPData_MainView_Time,
	EQPData_SideView_Time,
	EQPData_TP_Time,
	//EQPData_ULD_Time,
	MaxSDFSEQPDataBeginCount
};
#endif

struct SDFSPanelSummaryDataVBegin{
	CString PAENL_ID;
	CString CONTACT_PAENL_GRADE;
	CString	AOI_PAENL_GRADE;
	CString	PRE_PAENL_GRADE;
	CString	DOT_PAENL_GRADE;
	CString	LUMITOP_PAENL_GRADE;
	CString	OPV_PAENL_GRADE;
	CString	OPERATOR_ID;


	CString strPanelID;
	CString strPanelGrade;
	CString strMainDefectCode;
	CString strSubDefectCode1;
	CString strSubDefectCode2;
	CString strSubDefectCode3;
	CString strSubDefectCode4;
	CString strSubDefectCode5;
	CString strTotalPointCnt;
	CString strTotalLineCnt;
	CString strTotalMuraCnt;
	CString strColorShiftDefect;
};

enum
{
	PanelSummaryData_PanelID,
	PanelSummaryData_PanelGrade,
	PanelSummaryData_MainDefectCode,
	PanelSummaryData_SubDefectCode1,
	PanelSummaryData_SubDefectCode2,
	PanelSummaryData_SubDefectCode3,
	PanelSummaryData_SubDefectCode4,
	PanelSummaryData_SubDefectCode5,
	PanelSummaryData_TotalPointCnt,
	PanelSummaryData_TotalLineCnt,
	PanelSummaryData_TotalMuraCnt,
	PanelSummaryData_ColorShiftDefect,
	MaxPanelSummaryListBeginCount
};

struct SDFSDefectDataBegin{
	CString strPANEL_ID;
	CString strDEFECT_DATA_NUM;
	CString strDEFECT_TYPE;
	CString strDEFECT_PTRN;
	CString strDEFECT_CODE;
	CString strDEFECT_GRADE;
	CString strIMAGE_DATA;
	CString strX;
	CString strY;
	CString strSIZE;
	CString strCAM_INSPECT;
	CString strZone;
	CString strInspName;
};

enum
{
	DefectData_PANEL_ID	,
	DefectData_DEFECT_DATA_NUM	,
	DefectData_DEFECT_TYPE	,
	DefectData_DEFECT_PTRN	,
	DefectData_DEFECT_CODE	,
	DefectData_DEFECT_GRADE	,
	DefectData_IMAGE_DATA	,
	DefectData_X	,
	DefectData_Y,
	DefectData_SIZE	,
	DefectData_CAM_INSPECT	,
	DefectData_Zone	,
	MaxSDFSDefectListBeginCount
};

struct ULD_SDFSDefectDataBegin{
	CString strPanel_ID;
	CString strFpc_ID;
	CString strDefect_Grade;
	CString strTP_Function;
	CString strDefect_code;
	CString strDefect_Ptn;
	CString strProcessID; // DFS항목아니고 OPV ProcessID 확인용
};

enum
{
	ULD_DefectData_Panel_ID,
	ULD_DefectData_Fpc_ID,
	ULD_DefectData_Defect_Grade,
	ULD_DefectData_TP_Function,
	ULD_DefectData_Defect_Code,
	ULD_DefectData_Defect_Ptn,
	MaxULDSDFSDefectListBeginCount
};

struct LUMITOP_SDFSDefectDataBegin{
	CString strPANEL_ID;
	CString strPOINT;
	CString strX;
	CString strY;
	CString strLUMITOP_PTRN;
	CString strLV;
	CString strCIE_X;
	CString strCIE_Y;
	CString strCCT;
	CString strMPCD;
	CString strMPCD_MIN;
	CString strMPCD_MAX;
	CString strMPCD_DIFF;
	CString strMPCD_CENTER;
	CString strCCT_CENTER;

};

enum
{
	LUMITOP_DefectData_PANEL_ID	,
	LUMITOP_DefectData_POINT	,
	LUMITOP_DefectData_X	,
	LUMITOP_DefectData_Y	,
	LUMITOP_DefectData_LUMITOP_PTRN	,
	LUMITOP_DefectData_LV	,
	LUMITOP_DefectData_CIE_X	,
	LUMITOP_DefectData_CIE_Y	,
	LUMITOP_DefectData_CCT, 
	LUMITOP_DefectData_MPCD, 
	LUMITOP_DefectData_MPCD_MIN, 
	LUMITOP_DefectData_MPCD_MAX, 
	LUMITOP_DefectData_MPCD_DIFF, 
	LUMITOP_DefectData_MPCD_CENTER, 
	LUMITOP_DefectData_CCT_CENTER,
	MaxLUMITOPSDFSDefectListBeginCount
};

struct SOpvDataBegin{
	CString strPanel_ID;
	CString strFpc_ID;
	CString strDefect_Grade;
	CString strTP_Function;
	CString strDefect_code;
	CString strDefect_Ptn;
	CString strData_X1;
	CString strGate_Y1;
	CString strData_X2;
	CString strGate_Y2;
	CString strData_X3;
	CString strGate_Y3;
	CString strImage;
	CString strGlass_Coordinate_X1;
	CString strGlass_Coordinate_Y1;
	CString strGlass_Coordinate_X2;
	CString strGlass_Coordinate_Y2;
	CString strGlass_Coordinate_X3;
	CString strGlass_Coordinate_Y3;
	CString strInspName;
	CString strChNum;
};

enum
{
	OpvData_Panel_ID,
	OpvData_Fpc_ID,
	OpvData_Defect_Grade,
	OpvData_TP_Function,
	OpvData_Defect_Code,
	OpvData_Defect_Ptn,
	OpvData_Data_X1,
	OpvData_Gate_Y1,
	OpvData_Data_X2,
	OpvData_Gate_Y2,
	OpvData_Data_X3,
	OpvData_Gate_Y3,
	OpvData_Image,
	OpvData_Glass_Coordinate_X1,
	OpvData_Glass_Coordinate_Y1,
	OpvData_Glass_Coordinate_X2,
	OpvData_Glass_Coordinate_Y2,
	OpvData_Glass_Coordinate_X3,
	OpvData_Glass_Coordinate_Y3,
	MaxSOpvDataListBeginCount
};

class CDFSInfo{
public:
#if _SYSTEM_AMTAFT_
	BOOL AMTAFTSavePanelDFS_SUM(DfsDataValue& DfsInfo, CString strPanelID, CString strFpcID, CString strVisionPath, CString strViewingPath, CString strLumitopPath, CString strOpvPath, CString strSumPath);

	BOOL DFSDefectBeginLoad(CString strFileName, CString strTypeName, BOOL bTotalDfs);
	BOOL DFSDefectBeginLoad_OP(CString strFileName, CString strTypeName, BOOL bTotalDfs);
	
	BOOL VisionLoadPanelDFSInfo(CString strPanelID, int iMachineNum);
	BOOL LoadOpvDataInfo(CString strPanel, int iMachineNum);
	BOOL LoadPanelDFSInfo(CString strFilename, int iInspNum);
	BOOL LoadDefectListInfo(CString strPanel, int InspNum, int iCount);
	BOOL LoadSITEDATAListInfo(CString strPanel, int InspNum, int iCount);
	void AddDefectCodeResult(CString strPanelID, int iErrorNum, int iTpResult, int iType);
	void SetEQPDataInfo(CString strRECIPE_NO, CString strAOI_RECIPE_NAME, CString strPLC_RECIPE_NAME, CString strPG_RECIPE_NAME, CString strTP_RECIPE_NAME, CString strSTART_TIME, CString strEND_TIME, CString strLOAD_STAGE_NO, CString strINSP_STAGE_NO, CString strUNLOAD_STAGE_NO, CString strPROBE_CONTACT_CNT, CString strINDEX_PANEL_GRADE, CString strINDEX_MAIN_CODE, CString strFINAL_PANEL_GRADE, CString strFINAL_MAIN_CODE, CString strOPERATOR_ID);
	void IndexZoneInspResultInfo(CString strPanelID);

	CString GetDefectListInfo(int ii, int InspNum);

	CString GetPanelSummaryInfo();
	BOOL LoadPanelSummaryInfo(CString strPanel);
	void SetPanelSummaryInfo();

	void AddTotalDefectListInfo(vector <SDFSDefectDataBegin> vecAoiData, vector <SDFSDefectDataBegin> vecViewingData, vector <LUMITOP_SDFSDefectDataBegin> vecLumitopData, vector <ULD_SDFSDefectDataBegin> vecOpvData);

	BOOL PGDfsInfoLoad(CString strPanelID);
#else
	BOOL GammaSavePanelDFS_SUM(CString strPANEL_ID, CString strFpcID, CString strSumPath);

	BOOL GammaDfsInfoLoad(CString strPanelID);
	void SetEQPDataInfo(CString strRecipe_No, CString strRecipe_Name, CString strStart_Time, CString strEnd_Time, CString strOperator_ID, CString strOperator_Mode, CString strUnit_ID, CString strStage_ID, CString strProbe_Contact_Cnt, CString strTact_Time, CString strLD_Time, CString strP_Gamma_Time, CString strMainView_Time, CString strSideView_Time, CString strTP_Time, CString strUld_Time);
#endif

	map<CString, map<CString, CString>> m_mapPanelDefect;
	map<CString, int> m_mapDefectCodeList;

	vector <SDFSDefectDataBegin> m_DefectDataList;
	vector <ULD_SDFSDefectDataBegin> m_ULD_DefectDataList;
	vector <LUMITOP_SDFSDefectDataBegin> m_LUMITOP_DefectDataList;
	vector <SOpvDataBegin> m_OpvDataList[2];

	SDFSHeaderInfo m_HeaderInfo;
	SDFAPanel_Data_Begin m_PanelDataBegin;
	SDFSEQPDataBegin m_EQPDataInfo;
	SDFSPanelSummaryDataVBegin m_PanelSummaryInfo;
	SDFSPanelSummaryDataVBegin m_PanelSummaryInfo_OP;

	PGDfsList m_GammaDfsInfo;
	PGDfsList m_PGDfsInfo;
	
	CString m_strVisionResult;
	CString m_strViewingResult;
	CString m_strLumitopResult;
	
	CDFSInfo();
	~CDFSInfo();

	void Clear();
	BOOL CopyImage(CString strFilePath, CString strSendFilePath);
	BOOL CopyImage2(CString strFilePath, CString strSendFilePath);

	CString strProduct_ID_Before;
	CString strOwner_CODE_Before;
	CString strOwner_Type_Before;

private:
	// AMT AFT GAMMA 공통함수

	void ClearPanelInfo();

	int GetItemCount(CString strInfo);
	CString GetExtractionMsg(CString& strMsg);
	CString GetLastExtractionMsg(CString& strMsg);
	CString WriteString(CString strWrite);

	CString GetPanelDatatBegin();
	BOOL LoadPanelDataBegin(CString strPanel);
	void SetPanelDataBegin(CString strPanel_ID, CString strOwner_CODE, CString strOwner_Type, CString strProduct_ID, CString strProcess_ID, CString strProduct_Group, CString strLOT_ID, CString strCST_ID, CString strGroup_ID, CString strArray_Lot_ID, CString strArray_Glass_ID);
	
	CString GetPGDatatBegin();//
	CString GetEQPDataInfo();
	BOOL LoadEQPDataInfo(CString strPanel);
	
	
	BOOL GetTitleCheck(CStdioFile& sFile, int iSize);

	void SetBCServerData(CString strPanel, int iType, CDFSInfo DfsInfo);
};
