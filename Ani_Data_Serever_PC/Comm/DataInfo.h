#pragma once
///////////////////////////////////////////////////////////////////////////////
// FILE : CDataInfo.h
// Header file for CSOT OPV class
//		Version		Updated		 Author		 Note
//      -------     -------      ------      ----
//		   1.0      2018/08/14   hacy		Create	: OPV Data 처리 설계 및 작업
///////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <map>
using namespace std;

class CDataInfo{
public:
	SDataPanelInfo m_Panel_Info;
	vector <SDataDefectInfo> m_Panel_Defect;

	SDataModuleShop m_FS_ModuleData;
	SJobDataShop m_JobData;

	CDataInfo();
	~CDataInfo();

	void Clear();

	BOOL LoadDataInfo(CString strFilename);
	BOOL LoadPanelDataInfo(CString strPanel);
	BOOL LoadDefectDataInfo(CString strPanel);

	BOOL DFSInfoFinde(CStdioFile& sFile);
	int GetItemCount(CString strInfo);
	CString GetExtractionMsg(CString& strMsg);
	CString GetLastExtractionMsg(CString& strMsg);

	BOOL SetSaveFile(CString strFIleName);
	DfsDataValue SetLoadFile(CString strPanelID);
	CString GetPanelInfo();
	CString GetPanelDefectInfo(int i);
};
