#pragma once

#include <vector>
#include <string>
#include <sql.h>
#include <sqlext.h>

// Pull in full struct definitions needed by CLightingDB
// DataModels.h defines CDefectInfo and CDefectInfoList
// DFSInfo.h defines LUMITOP_SDFSDefectDataBegin and SDFSDefectDataBegin
// LightingInspectionResult is forward-declared below (full definition in Ani_Data_Serever_PC.h)
#include "DataModels.h"
#include "DFSInfo.h"

struct LightingInspectionResult;

class CWinApp;
class CAni_Data_Serever_PCApp;

// DB interface accessor - returns reference to the singleton
class CLightingDB;
CLightingDB& GetDBInterface();

class CLightingDB
{
public:
	static CLightingDB& Get();
	static void SetAppPtr(CWinApp* pApp);

	BOOL Connect();
	void Close();
	BOOL IsConnected() const;

	BOOL ConnectLightingDatabase();
	void CloseLightingDatabase();
	BOOL ConnectDfsLightingDatabase();
	void CloseDfsLightingDatabase();
	SQLHDBC GetDfsLightingConnection();

	CString GetSelectLatestByUniqueIDSQL(const CString& strUniqueID) const;
	BOOL QueryByUniqueID(const CString& strUniqueID, CInspectionResult& result);

	LightingInspectionResult QueryInspectionResult(CString uniqueID);
	LightingInspectionResult QueryInspectionResultThreadSafe(CString uniqueID, SQLHDBC pConn);
	BOOL QueryAOIDefectList(CString strUniqueID, std::vector<SDFSDefectDataBegin>& vecDefects, SQLHDBC pConn);
	BOOL QueryAOIDefectListThreadSafe(CString strUniqueID, std::vector<SDFSDefectDataBegin>& vecDefects, SQLHDBC pConn);
	BOOL QueryAOIDefectListThreadSafe(CString strUniqueID, CDefectInfoList& vecDefects, SQLHDBC pConn);
	BOOL QueryLightingDefectList(CString strUniqueID, std::vector<LUMITOP_SDFSDefectDataBegin>& vecDefects);
	CString GetLightingUniqueIDByBarcode(CString strBarcode);
	BOOL QueryIdMapByFixtureNo(int fixtureNo, CString& uniqueID, CString& screenID, CString& markID);
	BOOL QueryIdMapByFixtureNoThreadSafe(int fixtureNo, CString& uniqueID, CString& screenID, CString& markID, SQLHDBC pConn);

	BOOL UpdateLightingIdMap(int fixtureNo, CString uniqueID, CString screenID, CString markID);
	BOOL UpdateLightingInspectionResult(CString uniqueID);

	void GetLightingResultByBarcode(CString strBarcode, CString& strAOIResult, CString& strCodeAOI, CString& strGradeAOI, BOOL& bValid);
	LightingInspectionResult GetLightingResultByUniqueID(CString uniqueID);

	// Query defect code by barcode (used by SetLoadResultCode)
	BOOL QueryDefectCodeByBarcode(CString strBarcode, CString& strCode, CString& strGrade);

	// Get last error message
	CString GetLastError() const { return m_strLastError; }

	// Returns a valid SQLHDBC on success, SQL_NULL_HANDLE on failure.
	// This avoids a round-trip: caller gets the handle directly instead of
	// having to call EnsureTlsConnection() then GetTlsLightingConnPtr().
	SQLHDBC EnsureTlsConnection();

protected:
	CLightingDB();
	~CLightingDB();

private:
	CAni_Data_Serever_PCApp* App() const;
	SQLHDBC GetOrCreateConn();
	void PrintOdbcError(SQLHANDLE handle, SQLSMALLINT type);
	std::string UnicodeToMultiByte(const wchar_t* unicodeStr);

	static CWinApp* s_pApp;
	static CLightingDB s_instance;

	BOOL m_bConnected;
	SQLHDBC m_hConn;
	SQLHENV m_hEnv;
	CString m_strLastError;
};
