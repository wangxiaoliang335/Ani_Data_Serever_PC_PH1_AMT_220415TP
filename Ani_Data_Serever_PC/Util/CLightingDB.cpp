#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "Util/CLightingDB.h"
#include "Util/TLSConnection.h"
#include "Util/StringSupport.h"

using namespace std;

CLightingDB CLightingDB::s_instance;
CWinApp* CLightingDB::s_pApp = nullptr;

CLightingDB::CLightingDB()
	: m_bConnected(FALSE)
	, m_hConn(SQL_NULL_HANDLE)
	, m_hEnv(SQL_NULL_HANDLE)
	, m_strLastError(_T(""))
{
}

CLightingDB::~CLightingDB()
{
	Close();
}

CLightingDB& CLightingDB::Get()
{
	return s_instance;
}

void CLightingDB::SetAppPtr(CWinApp* pApp)
{
	s_pApp = pApp;
}

BOOL CLightingDB::IsConnected() const
{
	return m_bConnected;
}

BOOL CLightingDB::Connect()
{
	return ConnectLightingDatabase();
}

void CLightingDB::Close()
{
	CloseLightingDatabase();
}

void CLightingDB::PrintOdbcError(SQLHANDLE handle, SQLSMALLINT type)
{
	DWORD threadId = GetCurrentThreadId();
	SQLCHAR sqlState[6] = {0};
	SQLCHAR message[SQL_MAX_MESSAGE_LENGTH] = {0};
	SQLINTEGER nativeError = 0;
	SQLSMALLINT length = 0;

	CString strHeader;
	strHeader.Format(_T("[CLightingDB][ODBC] ThreadID=%lu, HandleType=%d"), threadId, type);
	OutputDebugString(strHeader + _T("\n"));

	SQLRETURN ret = SQLGetDiagRecA(type, handle, 1, sqlState, &nativeError, message, sizeof(message), &length);
	if (SQL_SUCCEEDED(ret)) {
		CString strErr;
		strErr.Format(_T("ODBC Error: %hs (SQL State: %hs, Native Error: %d)"),
			(char*)message, (char*)sqlState, nativeError);
		OutputDebugString(strErr + _T("\n"));
	}
}

std::string CLightingDB::UnicodeToMultiByte(const wchar_t* unicodeStr)
{
	if (unicodeStr == nullptr || wcslen(unicodeStr) == 0)
		return "";
	int bufferSize = WideCharToMultiByte(CP_ACP, 0, unicodeStr, -1, nullptr, 0, nullptr, nullptr);
	if (bufferSize <= 0)
		return "";
	vector<char> buffer(bufferSize);
	WideCharToMultiByte(CP_ACP, 0, unicodeStr, -1, buffer.data(), bufferSize, nullptr, nullptr);
	return string(buffer.data());
}

BOOL CLightingDB::ConnectLightingDatabase()
{
	DWORD threadId = GetCurrentThreadId();
	OutputDebugString(CStringSupport::FormatString(_T("[CLightingDB] ConnectLightingDatabase: ThreadID=%lu\n"), threadId));

	if (IsTlsLightingDBConnected() && GetTlsLightingConnPtr() != SQL_NULL_HANDLE) {
		OutputDebugString(CStringSupport::FormatString(_T("[CLightingDB] ConnectLightingDatabase: already connected (TLS)\n")));
		m_bConnected = TRUE;
		return TRUE;
	}

	// Use TLS connection instead of App-level members
	SQLHDBC conn = GetOrCreateConn();
	if (conn != SQL_NULL_HANDLE) {
		m_bConnected = TRUE;
		return TRUE;
	}

	OutputDebugString(CStringSupport::FormatString(_T("[CLightingDB] ConnectLightingDatabase: FAILED\n")));
	return FALSE;
}

void CLightingDB::CloseLightingDatabase()
{
	if (m_hConn != SQL_NULL_HANDLE) {
		SQLDisconnect(m_hConn);
		SQLFreeHandle(SQL_HANDLE_DBC, m_hConn);
		m_hConn = SQL_NULL_HANDLE;
	}
	m_bConnected = FALSE;
	OutputDebugString(_T("[CLightingDB] Lighting database connection closed\n"));
}

BOOL CLightingDB::ConnectDfsLightingDatabase()
{
	if (IsTlsLightingDBConnected() && GetTlsLightingConnPtr() != SQL_NULL_HANDLE)
		return TRUE;

	SQLHDBC conn = GetOrCreateConn();
	return (conn != SQL_NULL_HANDLE);
}

void CLightingDB::CloseDfsLightingDatabase()
{
	// TLS connections are managed globally, just update local state
	m_bConnected = FALSE;
	OutputDebugString(_T("[CLightingDB] CloseDfsLightingDatabase\n"));
}

SQLHDBC CLightingDB::GetDfsLightingConnection()
{
	if (IsTlsLightingDBConnected() && GetTlsLightingConnPtr() != SQL_NULL_HANDLE)
		return GetTlsLightingConnPtr();

	if (ConnectDfsLightingDatabase())
		return GetTlsLightingConnPtr();

	return SQL_NULL_HANDLE;
}

void CLightingDB::GetLightingResultByBarcode(CString strBarcode, CString& strAOIResult, CString& strCodeAOI, CString& strGradeAOI, BOOL& bValid)
{
	strAOIResult = _T("");
	strCodeAOI = _T("");
	strGradeAOI = _T("");
	bValid = FALSE;

	DWORD threadId = GetCurrentThreadId();
	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] GetLightingResultByBarcode: ThreadID=%lu, Barcode=%s\n"), threadId, strBarcode));

	SQLHDBC conn = GetOrCreateConn();
	if (conn == SQL_NULL_HANDLE) {
		OutputDebugString(_T("[CLightingDB] GetLightingResultByBarcode: no connection\n"));
		return;
	}

	CString strUniqueID;
	BOOL bRetry = FALSE;
	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(conn, SQL_HANDLE_DBC);
		return;
	}

	string sqlStr = "SELECT UniqueID FROM ivs_lcd_idmap WHERE Barcode = '" + UnicodeToMultiByte(strBarcode.GetString()) + "'";
	if (!ExecQueryWithRetry(_T("GetLightingResultByBarcode[SELECT_UniqueID]"), conn, sqlStr, stmt, &ret)) {
		if (stmt != SQL_NULL_HANDLE)
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return;
	}

	ret = SQLFetch(stmt);
	if (SQL_SUCCEEDED(ret)) {
		SQLCHAR buf[256] = {0};
		SQLLEN len = 0;
		SQLGetData(stmt, 1, SQL_C_CHAR, buf, sizeof(buf), &len);
		strUniqueID = CA2W((char*)buf);
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	if (strUniqueID.IsEmpty()) {
		OutputDebugString(CStringSupport::FormatString(
			_T("[CLightingDB] GetLightingResultByBarcode: No UniqueID for Barcode=%s\n"), strBarcode));
		return;
	}

	LightingInspectionResult result = QueryInspectionResult(strUniqueID);
	if (result.m_bValid) {
		strAOIResult = result.m_strAOIResult;
		strCodeAOI = result.m_strCodeAOI;
		strGradeAOI = result.m_strGradeAOI;
		bValid = TRUE;
	}
}

BOOL CLightingDB::QueryLightingDefectList(CString strUniqueID, std::vector<LUMITOP_SDFSDefectDataBegin>& vecDefects)
{
	vecDefects.clear();
	SQLHDBC conn = GetOrCreateConn();
	if (conn == SQL_NULL_HANDLE)
		return FALSE;

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(conn, SQL_HANDLE_DBC);
		return FALSE;
	}

	string sqlStr = "SELECT DefectIndex, Type, PatternID, PatternName, Pos_x, Pos_y, Pos_width, Pos_height, "
		"TrueSize, GrayScale, GrayScale_BK, GrayScaleDiff, Code_AOI, Grade_AOI "
		"FROM ivs_lcd_aoidefect WHERE GUID_IVS_LCD_InspectionResult = '" + UnicodeToMultiByte(strUniqueID.GetString()) + "' ORDER BY DefectIndex";

	ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlStr.c_str(), SQL_NTS);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(stmt, SQL_HANDLE_STMT);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);

		// SQL 失败，重连后重试一次
		OutputDebugString(_T("[CLightingDB] QueryLightingDefectList: SQL failed, retrying after reconnect...\n"));
		conn = EnsureTlsConnection();
		if (conn == SQL_NULL_HANDLE)
			conn = GetOrCreateConn();
		if (conn == SQL_NULL_HANDLE)
			return FALSE;

		ret = SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(conn, SQL_HANDLE_DBC);
			return FALSE;
		}

		ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlStr.c_str(), SQL_NTS);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			OutputDebugString(_T("[CLightingDB] QueryLightingDefectList: retry also failed\n"));
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			return FALSE;
		}
	}

	int iDefectCount = 0;
	while ((ret = SQLFetch(stmt)) == SQL_SUCCESS) {
		LUMITOP_SDFSDefectDataBegin defect;
		defect.strPANEL_ID = strUniqueID;

		SQLCHAR buf[256] = {0};
		SQLLEN len = 0;

		SQLGetData(stmt, 1, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strPOINT = CA2W((char*)buf);

		SQLGetData(stmt, 3, SQL_C_CHAR, buf, sizeof(buf), &len);
		CString strPatternID = CA2W((char*)buf);

		SQLGetData(stmt, 4, SQL_C_CHAR, buf, sizeof(buf), &len);
		CString strPatternName = CA2W((char*)buf);

		defect.strLUMITOP_PTRN = strPatternName.IsEmpty() ? strPatternID : strPatternName;

		SQLGetData(stmt, 5, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strX = CA2W((char*)buf);

		SQLGetData(stmt, 6, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strY = CA2W((char*)buf);

		SQLGetData(stmt, 11, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strLV = CA2W((char*)buf);

		defect.strCIE_X = _T("0");
		defect.strCIE_Y = _T("0");
		defect.strCCT = _T("0");
		defect.strMPCD = _T("0");
		defect.strMPCD_MIN = _T("0");
		defect.strMPCD_MAX = _T("0");
		defect.strMPCD_DIFF = _T("0");
		defect.strMPCD_CENTER = _T("0");
		defect.strCCT_CENTER = _T("0");

		vecDefects.push_back(defect);
		iDefectCount++;
	}

	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] QueryLightingDefectList: Found %d defects for UniqueID=%s\n"),
		iDefectCount, strUniqueID));

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return TRUE;
}

BOOL CLightingDB::QueryAOIDefectList(CString strUniqueID, std::vector<SDFSDefectDataBegin>& vecDefects, SQLHDBC pConn)
{
	vecDefects.clear();
	DWORD threadId = GetCurrentThreadId();
	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] QueryAOIDefectList: ThreadID=%lu, UniqueID=%s\n"), threadId, strUniqueID));

	SQLHDBC pUseConn = pConn;
	if (pUseConn == SQL_NULL_HANDLE) {
		pUseConn = EnsureTlsConnection();
		if (pUseConn == SQL_NULL_HANDLE)
			pUseConn = GetOrCreateConn();
	}

	if (pUseConn == SQL_NULL_HANDLE)
		return FALSE;

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
		return FALSE;
	}

	string sqlStr = "SELECT DefectIndex, Type, PatternID, PatternName, Pos_x, Pos_y, Pos_width, Pos_height, "
		"TrueSize, GrayScale, GrayScale_BK, GrayScaleDiff, Code_AOI, Grade_AOI, ImagePath "
		"FROM ivs_lcd_aoidefect WHERE GUID_IVS_LCD_InspectionResult = '" + UnicodeToMultiByte(strUniqueID.GetString()) + "' ORDER BY DefectIndex";

	if (!ExecQueryWithRetry(_T("QueryAOIDefectList"), pUseConn, sqlStr, stmt, &ret))
		return FALSE;

	int iDefectCount = 0;
	while ((ret = SQLFetch(stmt)) == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
		SDFSDefectDataBegin defect;
		defect.strPANEL_ID = strUniqueID;

		SQLCHAR buf[512] = {0};
		SQLLEN len = 0;

		SQLGetData(stmt, 1, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strDEFECT_DATA_NUM = CA2T((char*)buf);
		if (!defect.strDEFECT_DATA_NUM.IsEmpty()) {
			int nIndex = _ttoi(defect.strDEFECT_DATA_NUM);
			defect.strDEFECT_DATA_NUM.Format(_T("%d"), nIndex + 1);
		}

		SQLGetData(stmt, 2, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strDEFECT_TYPE = CA2T((char*)buf);

		SQLGetData(stmt, 3, SQL_C_CHAR, buf, sizeof(buf), &len);
		CString strPatternID = CA2T((char*)buf);

		SQLGetData(stmt, 4, SQL_C_CHAR, buf, sizeof(buf), &len);
		CString strPatternName = CA2T((char*)buf);
		defect.strDEFECT_PTRN = strPatternName.IsEmpty() ? strPatternID : strPatternName;

		SQLGetData(stmt, 13, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strDEFECT_CODE = CA2T((char*)buf);

		SQLGetData(stmt, 14, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strDEFECT_GRADE = CA2T((char*)buf);

		SQLGetData(stmt, 5, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strX = CA2T((char*)buf);

		SQLGetData(stmt, 6, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strY = CA2T((char*)buf);

		SQLGetData(stmt, 9, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strSIZE = CA2T((char*)buf);

		SQLGetData(stmt, 15, SQL_C_CHAR, buf, sizeof(buf), &len);
		defect.strIMAGE_DATA = CA2T((char*)buf);

		defect.strCAM_INSPECT = _T("");
		defect.strZone = _T("");
		defect.strInspName = _T("");

		vecDefects.push_back(defect);
		iDefectCount++;
	}

	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] QueryAOIDefectList: Found %d defects for UniqueID=%s\n"),
		iDefectCount, strUniqueID));

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return TRUE;
}

BOOL CLightingDB::QueryAOIDefectListThreadSafe(CString strUniqueID, std::vector<SDFSDefectDataBegin>& vecDefects, SQLHDBC pConn)
{
	return QueryAOIDefectList(strUniqueID, vecDefects, pConn);
}

BOOL CLightingDB::QueryAOIDefectListThreadSafe(CString strUniqueID, CDefectInfoList& vecDefects, SQLHDBC pConn)
{
	vecDefects.clear();
	std::vector<SDFSDefectDataBegin> vecRaw;
	if (!QueryAOIDefectList(strUniqueID, vecRaw, pConn))
		return FALSE;
	for (const auto& raw : vecRaw) {
		CDefectInfo info;
		info.DefectIndex = _ttoi(raw.strDEFECT_DATA_NUM);
		info.Type = raw.strDEFECT_TYPE;
		info.PatternID = _ttoi(raw.strDEFECT_PTRN);
		info.PatternName = raw.strDEFECT_PTRN;
		info.Pos_x = _ttoi(raw.strX);
		info.Pos_y = _ttoi(raw.strY);
		info.Code_AOI = raw.strDEFECT_CODE;
		info.Grade_AOI = raw.strDEFECT_GRADE;
		info.ImagePath = raw.strIMAGE_DATA;
		info.GUID_Parent = strUniqueID;
		vecDefects.push_back(info);
	}
	return TRUE;
}

CString CLightingDB::GetLightingUniqueIDByBarcode(CString strBarcode)
{
	CString strUniqueID = _T("");
	SQLHDBC conn = GetOrCreateConn();
	if (conn == SQL_NULL_HANDLE)
		return strUniqueID;

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(conn, SQL_HANDLE_DBC);
		return strUniqueID;
	}

	string sqlStr = "SELECT UniqueID FROM ivs_lcd_idmap WHERE Barcode = '" + UnicodeToMultiByte(strBarcode.GetString()) + "'";
	if (!ExecQueryWithRetry(_T("GetLightingUniqueIDByBarcode"), conn, sqlStr, stmt, &ret)) {
		if (stmt != SQL_NULL_HANDLE)
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return strUniqueID;
	}

	ret = SQLFetch(stmt);
	if (SQL_SUCCEEDED(ret)) {
		SQLCHAR buf[101] = {0};
		SQLLEN len = 0;
		SQLGetData(stmt, 1, SQL_C_CHAR, buf, sizeof(buf), &len);
		strUniqueID = CA2W((char*)buf);
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return strUniqueID;
}

BOOL CLightingDB::QueryIdMapByFixtureNo(int fixtureNo, CString& uniqueID, CString& screenID, CString& markID)
{
	uniqueID = _T("");
	screenID = _T("");
	markID = _T("");
	SQLHDBC conn = GetOrCreateConn();
	if (conn == SQL_NULL_HANDLE)
		return FALSE;

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(conn, SQL_HANDLE_DBC);
		return FALSE;
	}

	char sqlBuf[512];
	sprintf_s(sqlBuf, sizeof(sqlBuf), "SELECT UniqueID, Barcode, MainAoiFixID FROM ivs_lcd_idmap WHERE MainAoiFixID = '%d'", fixtureNo);
	ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlBuf, SQL_NTS);
	BOOL ok = FALSE;
	if (SQL_SUCCEEDED(ret)) {
		ret = SQLFetch(stmt);
		if (SQL_SUCCEEDED(ret)) {
			SQLCHAR uidBuf[101] = {0}, screenBuf[101] = {0};
			SQLLEN lenUid = 0, lenScreen = 0;
			int mainAoiFixID = 0;
			SQLGetData(stmt, 1, SQL_C_CHAR, uidBuf, sizeof(uidBuf), &lenUid);
			SQLGetData(stmt, 2, SQL_C_CHAR, screenBuf, sizeof(screenBuf), &lenScreen);
			SQLGetData(stmt, 3, SQL_C_SLONG, &mainAoiFixID, 0, NULL);
			uniqueID = CA2W((char*)uidBuf);
			screenID = CA2W((char*)screenBuf);
			markID.Format(_T("%02d"), mainAoiFixID);
			OutputDebugString(CStringSupport::FormatString(
				_T("[CLightingDB] QueryIdMapByFixtureNo: FixtureNo=%d, UniqueID=%s, ScreenID=%s, MarkID=%s\n"),
				fixtureNo, uniqueID, screenID, markID));
			ok = TRUE;
		}
	}
	else {
		PrintOdbcError(stmt, SQL_HANDLE_STMT);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);

		// SQL 失败，重连后重试一次
		OutputDebugString(_T("[CLightingDB] QueryIdMapByFixtureNo: SQL failed, retrying after reconnect...\n"));
		conn = EnsureTlsConnection();
		if (conn == SQL_NULL_HANDLE)
			conn = GetOrCreateConn();
		if (conn == SQL_NULL_HANDLE)
			return FALSE;

		ret = SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(conn, SQL_HANDLE_DBC);
			return FALSE;
		}

		sprintf_s(sqlBuf, sizeof(sqlBuf), "SELECT UniqueID, Barcode, MainAoiFixID FROM ivs_lcd_idmap WHERE MainAoiFixID = '%d'", fixtureNo);
		ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlBuf, SQL_NTS);
		if (SQL_SUCCEEDED(ret)) {
			ret = SQLFetch(stmt);
			if (SQL_SUCCEEDED(ret)) {
				SQLCHAR uidBuf[101] = {0}, screenBuf[101] = {0};
				SQLLEN lenUid = 0, lenScreen = 0;
				int mainAoiFixID = 0;
				SQLGetData(stmt, 1, SQL_C_CHAR, uidBuf, sizeof(uidBuf), &lenUid);
				SQLGetData(stmt, 2, SQL_C_CHAR, screenBuf, sizeof(screenBuf), &lenScreen);
				SQLGetData(stmt, 3, SQL_C_SLONG, &mainAoiFixID, 0, NULL);
				uniqueID = CA2W((char*)uidBuf);
				screenID = CA2W((char*)screenBuf);
				markID.Format(_T("%02d"), mainAoiFixID);
				OutputDebugString(CStringSupport::FormatString(
					_T("[CLightingDB] QueryIdMapByFixtureNo: retry success, FixtureNo=%d, UniqueID=%s, ScreenID=%s, MarkID=%s\n"),
					fixtureNo, uniqueID, screenID, markID));
				ok = TRUE;
			}
		}
		else {
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			OutputDebugString(_T("[CLightingDB] QueryIdMapByFixtureNo: retry also failed\n"));
		}
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return ok;
}

BOOL CLightingDB::QueryIdMapByFixtureNoThreadSafe(int fixtureNo, CString& uniqueID, CString& screenID, CString& markID, SQLHDBC pConn)
{
	uniqueID = _T("");
	screenID = _T("");
	markID = _T("");
	SQLHDBC pUseConn = pConn;
	if (pUseConn == SQL_NULL_HANDLE) {
		pUseConn = EnsureTlsConnection();
		if (pUseConn == SQL_NULL_HANDLE)
			pUseConn = GetOrCreateConn();
	}
	if (pUseConn == SQL_NULL_HANDLE)
		return FALSE;

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
		return FALSE;
	}

	char sqlBuf[512];
	sprintf_s(sqlBuf, sizeof(sqlBuf), "SELECT UniqueID, Barcode, MainAoiFixID FROM ivs_lcd_idmap WHERE MainAoiFixID = '%d'", fixtureNo);
	ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlBuf, SQL_NTS);
	BOOL ok = FALSE;
	if (SQL_SUCCEEDED(ret)) {
		ret = SQLFetch(stmt);
		if (SQL_SUCCEEDED(ret)) {
			SQLCHAR uidBuf[101] = {0}, screenBuf[101] = {0};
			SQLLEN lenUid = 0, lenScreen = 0;
			int mainAoiFixID = 0;
			SQLGetData(stmt, 1, SQL_C_CHAR, uidBuf, sizeof(uidBuf), &lenUid);
			SQLGetData(stmt, 2, SQL_C_CHAR, screenBuf, sizeof(screenBuf), &lenScreen);
			SQLGetData(stmt, 3, SQL_C_SLONG, &mainAoiFixID, 0, NULL);
			uniqueID = CA2W((char*)uidBuf);
			screenID = CA2W((char*)screenBuf);
			markID.Format(_T("%02d"), mainAoiFixID);
			ok = TRUE;
		}
	}
	else {
		PrintOdbcError(stmt, SQL_HANDLE_STMT);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);

		// SQL 失败，重连后重试一次
		OutputDebugString(_T("[CLightingDB] QueryIdMapByFixtureNoThreadSafe: SQL failed, retrying after reconnect...\n"));
		pUseConn = EnsureTlsConnection();
		if (pUseConn == SQL_NULL_HANDLE)
			pUseConn = GetOrCreateConn();
		if (pUseConn == SQL_NULL_HANDLE)
			return FALSE;

		ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
			return FALSE;
		}

		sprintf_s(sqlBuf, sizeof(sqlBuf), "SELECT UniqueID, Barcode, MainAoiFixID FROM ivs_lcd_idmap WHERE MainAoiFixID = '%d'", fixtureNo);
		ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlBuf, SQL_NTS);
		if (SQL_SUCCEEDED(ret)) {
			ret = SQLFetch(stmt);
			if (SQL_SUCCEEDED(ret)) {
				SQLCHAR uidBuf[101] = {0}, screenBuf[101] = {0};
				SQLLEN lenUid = 0, lenScreen = 0;
				int mainAoiFixID = 0;
				SQLGetData(stmt, 1, SQL_C_CHAR, uidBuf, sizeof(uidBuf), &lenUid);
				SQLGetData(stmt, 2, SQL_C_CHAR, screenBuf, sizeof(screenBuf), &lenScreen);
				SQLGetData(stmt, 3, SQL_C_SLONG, &mainAoiFixID, 0, NULL);
				uniqueID = CA2W((char*)uidBuf);
				screenID = CA2W((char*)screenBuf);
				markID.Format(_T("%02d"), mainAoiFixID);
				OutputDebugString(CStringSupport::FormatString(
					_T("[CLightingDB] QueryIdMapByFixtureNoThreadSafe: retry success, FixtureNo=%d, UniqueID=%s, ScreenID=%s, MarkID=%s\n"),
					fixtureNo, uniqueID, screenID, markID));
				ok = TRUE;
			}
		}
		else {
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			OutputDebugString(_T("[CLightingDB] QueryIdMapByFixtureNoThreadSafe: retry also failed\n"));
		}
	}
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return ok;
}

LightingInspectionResult CLightingDB::QueryInspectionResult(CString uniqueID)
{
	SQLHDBC conn = EnsureTlsConnection();
	if (conn == SQL_NULL_HANDLE)
		return LightingInspectionResult();
	return QueryInspectionResultThreadSafe(uniqueID, conn);
}

LightingInspectionResult CLightingDB::QueryInspectionResultThreadSafe(CString uniqueID, SQLHDBC pConn)
{
	LightingInspectionResult result;
	result.m_bValid = FALSE;
	SQLHDBC pUseConn = pConn;
	if (pUseConn == SQL_NULL_HANDLE) {
		pUseConn = EnsureTlsConnection();
		if (pUseConn == SQL_NULL_HANDLE)
			pUseConn = GetOrCreateConn();
	}
	if (pUseConn == SQL_NULL_HANDLE)
		return result;

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
		return result;
	}

	string sqlStr = "SELECT GUID, ScreenID, AOIResult, Code_AOI, Grade_AOI, StartTime, StopTime, LocalIP "
		"FROM ivs_lcd_inspectionresult WHERE UniqueID = '" + UnicodeToMultiByte(uniqueID.GetString()) + "'";

	// 重试循环：SQLExecDirect 失败或 SQLFetch 失败都重试一次（断线重连后重试）
	for (int nRetry = 0; nRetry < 2; ++nRetry) {
		if (nRetry > 0) {
			// 重新获取连接并重建语句句柄
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
			pUseConn = EnsureTlsConnection();
			if (pUseConn == SQL_NULL_HANDLE)
				pUseConn = GetOrCreateConn();
			if (pUseConn == SQL_NULL_HANDLE)
				break;
			ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
				break;
			}
			OutputDebugString(CStringSupport::FormatString(
				_T("[CLightingDB] QueryInspectionResultThreadSafe: retry %d for UniqueID=%s\n"), nRetry, uniqueID));
		}

		ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlStr.c_str(), SQL_NTS);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			continue;  // 重试
		}

		ret = SQLFetch(stmt);
		if (SQL_SUCCEEDED(ret))
			break;  // 成功，退出重试循环
		if (ret == SQL_NO_DATA)
			break;  // 无数据，不需要重试
		// SQLFetch 失败（如连接断开），继续重试
		PrintOdbcError(stmt, SQL_HANDLE_STMT);
		// 不 break，让循环自然重试
	}

	BOOL bNoData = (ret == SQL_NO_DATA);
	if (!SQL_SUCCEEDED(ret)) {
		if (!bNoData) {
			// SQL_ERROR: 两次尝试都失败
			OutputDebugString(CStringSupport::FormatString(
				_T("[CLightingDB] QueryInspectionResultThreadSafe: both attempts failed for UniqueID=%s\n"), uniqueID));
		} else {
			// SQL_NO_DATA: 存在记录但 Fetch 失败（理论上不太可能）
			OutputDebugString(CStringSupport::FormatString(
				_T("[CLightingDB] QueryInspectionResultThreadSafe: SQL_NO_DATA for UniqueID=%s\n"), uniqueID));
		}
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return result;
	}

	SQLCHAR guidBuf[101] = {0}, screenIDBuf[101] = {0}, aoiResultBuf[51] = {0},
		codeAOIBuf[51] = {0}, gradeAOIBuf[51] = {0}, startTimeBuf[51] = {0}, stopTimeBuf[51] = {0}, localIPBuf[101] = {0};
	SQLLEN lenGUID = 0, lenScreenID = 0, lenAOIResult = 0, lenCodeAOI = 0, lenGradeAOI = 0, lenStartTime = 0, lenStopTime = 0, lenLocalIP = 0;

	SQLGetData(stmt, 1, SQL_C_CHAR, guidBuf, sizeof(guidBuf), &lenGUID);
	SQLGetData(stmt, 2, SQL_C_CHAR, screenIDBuf, sizeof(screenIDBuf), &lenScreenID);
	SQLGetData(stmt, 3, SQL_C_CHAR, aoiResultBuf, sizeof(aoiResultBuf), &lenAOIResult);
	SQLGetData(stmt, 4, SQL_C_CHAR, codeAOIBuf, sizeof(codeAOIBuf), &lenCodeAOI);
	SQLGetData(stmt, 5, SQL_C_CHAR, gradeAOIBuf, sizeof(gradeAOIBuf), &lenGradeAOI);
	SQLGetData(stmt, 6, SQL_C_CHAR, startTimeBuf, sizeof(startTimeBuf), &lenStartTime);
	SQLGetData(stmt, 7, SQL_C_CHAR, stopTimeBuf, sizeof(stopTimeBuf), &lenStopTime);
	SQLGetData(stmt, 8, SQL_C_CHAR, localIPBuf, sizeof(localIPBuf), &lenLocalIP);  // 8: LocalIP

	result.m_strGUID = CA2W((char*)guidBuf);
	result.m_strScreenID = CA2W((char*)screenIDBuf);
	result.m_strUniqueID = uniqueID;
	result.m_strAOIResult = CA2W((char*)aoiResultBuf);
	result.m_strCodeAOI = CA2W((char*)codeAOIBuf);
	result.m_strGradeAOI = CA2W((char*)gradeAOIBuf);
	result.m_strStartTime = CA2W((char*)startTimeBuf);
	result.m_strStopTime = CA2W((char*)stopTimeBuf);
	result.m_strLocalIP = CA2W((char*)localIPBuf);
	result.m_bValid = TRUE;

	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] QueryInspectionResultThreadSafe: UniqueID=%s, AOIResult=%s, Code=%s, Grade=%s\n"),
		uniqueID, result.m_strAOIResult, result.m_strCodeAOI, result.m_strGradeAOI));

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return result;
}

CString CLightingDB::GetSelectLatestByUniqueIDSQL(const CString& strUniqueID) const
{
	CString strSQL;
	strSQL.Format(
		_T("SELECT SysID, GUID, ScreenID, PlatformID, AOIResult, UniqueID, Code_AOI, Grade_AOI, ")
		_T("StartTime, StopTime, GridImageXLen, GridImageYLen, PanelPhysicalXLen, PanelPhysicalYLen ")
		_T("FROM IVS_LCD_InspectionResult WHERE UniqueID = '%s' ORDER BY SysID DESC LIMIT 1"),
		strUniqueID);
	return strSQL;
}

///////////////////////////////////////////////////////////////////////////////
// Query by UniqueID
///////////////////////////////////////////////////////////////////////////////
BOOL CLightingDB::QueryByUniqueID(const CString& strUniqueID, CInspectionResult& result)
{
	result.Reset();

	SQLHDBC conn = GetOrCreateConn();
	if (conn == SQL_NULL_HANDLE)
		return FALSE;

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(conn, SQL_HANDLE_DBC);
		return FALSE;
	}

	string sqlStr = "SELECT SysID, GUID, ScreenID, PlatformID, AOIResult, UniqueID, Code_AOI, Grade_AOI, "
		"StartTime, StopTime, GridImageXLen, GridImageYLen, PanelPhysicalXLen, PanelPhysicalYLen, LocalIP "
		"FROM IVS_LCD_InspectionResult WHERE UniqueID = '" + UnicodeToMultiByte(strUniqueID.GetString()) + "' "
		"ORDER BY SysID DESC LIMIT 1";

	// 重试循环：SQLExecDirect 失败或 SQLFetch 失败都重试一次（断线重连后重试）
	for (int nRetry = 0; nRetry < 2; ++nRetry) {
		if (nRetry > 0) {
			// 重新获取连接并重建语句句柄
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
			conn = EnsureTlsConnection();
			if (conn == SQL_NULL_HANDLE)
				conn = GetOrCreateConn();
			if (conn == SQL_NULL_HANDLE)
				break;
			ret = SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(conn, SQL_HANDLE_DBC);
				break;
			}
			OutputDebugString(CStringSupport::FormatString(
				_T("[CLightingDB] QueryByUniqueID: retry %d for UniqueID=%s\n"), nRetry, strUniqueID));
		}

		ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlStr.c_str(), SQL_NTS);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			continue;  // 重试
		}

		ret = SQLFetch(stmt);
		if (SQL_SUCCEEDED(ret))
			break;  // 成功，退出重试循环
		if (ret == SQL_NO_DATA)
			break;  // 无数据，不需要重试
		// SQLFetch 失败（如连接断开），继续重试
		PrintOdbcError(stmt, SQL_HANDLE_STMT);
	}

	if (!SQL_SUCCEEDED(ret)) {
		if (ret == SQL_NO_DATA)
			OutputDebugString(CStringSupport::FormatString(
				_T("[CLightingDB] QueryByUniqueID: No result for UniqueID=%s\n"), strUniqueID));
		else
			OutputDebugString(CStringSupport::FormatString(
				_T("[CLightingDB] QueryByUniqueID: both attempts failed for UniqueID=%s\n"), strUniqueID));
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return FALSE;
	}

	SQLCHAR buf[256] = {0};
	SQLLEN len = 0;

	SQLGetData(stmt, 1, SQL_C_SLONG, &result.SysID, 0, NULL);              // 1: SysID
	SQLGetData(stmt, 2, SQL_C_CHAR, buf, sizeof(buf), &len);                // 2: GUID
	result.GUID = CA2W((char*)buf);

	SQLGetData(stmt, 3, SQL_C_CHAR, buf, sizeof(buf), &len);                // 3: ScreenID
	result.ScreenID = CA2W((char*)buf);

	SQLGetData(stmt, 4, SQL_C_SLONG, &result.PlatformID, 0, NULL);           // 4: PlatformID

	SQLGetData(stmt, 5, SQL_C_CHAR, buf, sizeof(buf), &len);                // 5: AOIResult
	result.AOIResult = CA2W((char*)buf);

	SQLGetData(stmt, 6, SQL_C_CHAR, buf, sizeof(buf), &len);                // 6: UniqueID
	result.UniqueID = CA2W((char*)buf);

	SQLGetData(stmt, 7, SQL_C_CHAR, buf, sizeof(buf), &len);                // 7: Code_AOI
	result.Code_AOI = CA2W((char*)buf);

	SQLGetData(stmt, 8, SQL_C_CHAR, buf, sizeof(buf), &len);                // 8: Grade_AOI
	result.Grade_AOI = CA2W((char*)buf);

	// 9: StartTime, 10: StopTime
	SQLGetData(stmt, 9, SQL_C_CHAR, buf, sizeof(buf), &len);
	result.StartTime.ParseDateTime(CA2W((char*)buf));

	SQLGetData(stmt, 10, SQL_C_CHAR, buf, sizeof(buf), &len);
	result.StopTime.ParseDateTime(CA2W((char*)buf));

	SQLGetData(stmt, 11, SQL_C_SLONG, &result.GridImageXLen, 0, NULL);     // 11: GridImageXLen
	SQLGetData(stmt, 12, SQL_C_SLONG, &result.GridImageYLen, 0, NULL);     // 12: GridImageYLen
	SQLGetData(stmt, 13, SQL_C_DOUBLE, &result.PanelPhysicalXLen, 0, NULL); // 13: PanelPhysicalXLen
	SQLGetData(stmt, 14, SQL_C_DOUBLE, &result.PanelPhysicalYLen, 0, NULL);  // 14: PanelPhysicalYLen

	// 15: LocalIP（AOI设备IP地址，用于策略3拼图路径）
	SQLGetData(stmt, 15, SQL_C_CHAR, buf, sizeof(buf), &len);
	result.LocalIP = CA2W((char*)buf);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	//OutputDebugString(CStringSupport::FormatString(
	//	_T("[CLightingDB] QueryByUniqueID: UniqueID=%s, AOIResult=%s, Code=%s, Grade=%s\n"),
	//	result.UniqueID, result.AOIResult, result.Code_AOI, result.Grade_AOI));

	return TRUE;
}

LightingInspectionResult CLightingDB::GetLightingResultByUniqueID(CString uniqueID)
{
	return QueryInspectionResult(uniqueID);
}

BOOL CLightingDB::UpdateLightingInspectionResult(CString uniqueID)
{
	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] UpdateLightingInspectionResult: UniqueID=%s\n"), uniqueID));
	return TRUE;
}

BOOL CLightingDB::InsertLightingIdMap(int fixtureNo, CString uniqueID, CString screenID, CString markID)
{
	DWORD threadId = GetCurrentThreadId();
	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] InsertLightingIdMap: ThreadID=%lu, fixtureNo=%d, uniqueID=%s, screenID=%s, markID=%s\n"),
		threadId, fixtureNo, uniqueID, screenID, markID));

	SQLHDBC pUseConn = EnsureTlsConnection();
	if (pUseConn == SQL_NULL_HANDLE)
		pUseConn = GetOrCreateConn();
	if (pUseConn == SQL_NULL_HANDLE)
		return FALSE;

	CString strUID = uniqueID;
	strUID.Replace(_T("'"), _T("''"));
	CString strBarcode = screenID;
	strBarcode.Replace(_T("'"), _T("''"));
	CString strMark = markID;
	strMark.Replace(_T("'"), _T("''"));

	// 直接 INSERT，不检查是否存在
	char sqlBuf[1024];
	sprintf_s(sqlBuf, sizeof(sqlBuf), "INSERT INTO ivs_lcd_idmap (MarkID, UniqueID, Barcode, MainAoiFixID) VALUES ('%s', '%s', '%s', '%d')",
		UnicodeToMultiByte(strMark.GetString()).c_str(),
		UnicodeToMultiByte(strUID.GetString()).c_str(),
		UnicodeToMultiByte(strBarcode.GetString()).c_str(),
		fixtureNo);

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
		return FALSE;
	}

	ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlBuf, SQL_NTS);
	if (SQL_SUCCEEDED(ret)) {
		SQLLEN affected = 0;
		SQLRowCount(stmt, &affected);
		OutputDebugString(CStringSupport::FormatString(
			_T("[CLightingDB] InsertLightingIdMap: affected=%d, fixtureNo=%d\n"), (int)affected, fixtureNo));
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return TRUE;
	}

	// SQL 失败，可能是连接断开，重连后重试一次
	PrintOdbcError(stmt, SQL_HANDLE_STMT);
	OutputDebugString(_T("[CLightingDB] InsertLightingIdMap: SQL failed, retrying after reconnect...\n"));
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	// 重新获取连接
	pUseConn = EnsureTlsConnection();
	if (pUseConn == SQL_NULL_HANDLE)
		pUseConn = GetOrCreateConn();
	if (pUseConn == SQL_NULL_HANDLE)
		return FALSE;

	stmt = SQL_NULL_HANDLE;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
		return FALSE;
	}

	ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlBuf, SQL_NTS);
	if (SQL_SUCCEEDED(ret)) {
		SQLLEN affected = 0;
		SQLRowCount(stmt, &affected);
		OutputDebugString(CStringSupport::FormatString(
			_T("[CLightingDB] InsertLightingIdMap: retry success, affected=%d, fixtureNo=%d\n"), (int)affected, fixtureNo));
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return TRUE;
	}

	PrintOdbcError(stmt, SQL_HANDLE_STMT);
	OutputDebugString(_T("[CLightingDB] InsertLightingIdMap: retry also failed\n"));
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return FALSE;
}

// 保留旧函数名以兼容，但内部改为直接 INSERT
BOOL CLightingDB::UpdateLightingIdMap(int fixtureNo, CString uniqueID, CString screenID, CString markID)
{
	return InsertLightingIdMap(fixtureNo, uniqueID, screenID, markID);
}

// 清空 ivs_lcd_idmap 表
BOOL CLightingDB::ClearLightingIdMap()
{
	OutputDebugString(_T("[CLightingDB] ClearLightingIdMap: Clearing ivs_lcd_idmap table\n"));

	SQLHDBC pUseConn = EnsureTlsConnection();
	if (pUseConn == SQL_NULL_HANDLE)
		pUseConn = GetOrCreateConn();
	if (pUseConn == SQL_NULL_HANDLE)
		return FALSE;

	char sqlBuf[256];
	sprintf_s(sqlBuf, sizeof(sqlBuf), "TRUNCATE TABLE ivs_lcd_idmap");

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
		return FALSE;
	}

	ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlBuf, SQL_NTS);
	if (SQL_SUCCEEDED(ret)) {
		OutputDebugString(_T("[CLightingDB] ClearLightingIdMap: Table truncated successfully\n"));
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return TRUE;
	}

	// TRUNCATE 失败时尝试 DELETE（需要先重建 stmt 句柄）
	PrintOdbcError(stmt, SQL_HANDLE_STMT);
	OutputDebugString(_T("[CLightingDB] ClearLightingIdMap: TRUNCATE failed, trying DELETE after rebuild...\n"));
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	stmt = SQL_NULL_HANDLE;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
		// 重建 stmt 也失败，尝试重连后再试
		OutputDebugString(_T("[CLightingDB] ClearLightingIdMap: stmt rebuild failed, reconnecting...\n"));
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		stmt = SQL_NULL_HANDLE;
		pUseConn = EnsureTlsConnection();
		if (pUseConn == SQL_NULL_HANDLE)
			pUseConn = GetOrCreateConn();
		if (pUseConn == SQL_NULL_HANDLE)
			return FALSE;
		ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
			return FALSE;
		}
	}
	// 重建 stmt 成功后，执行 DELETE
	sprintf_s(sqlBuf, sizeof(sqlBuf), "DELETE FROM ivs_lcd_idmap");
	ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlBuf, SQL_NTS);
	if (SQL_SUCCEEDED(ret)) {
		OutputDebugString(_T("[CLightingDB] ClearLightingIdMap: Table deleted successfully\n"));
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return TRUE;
	}

	// DELETE 也失败，可能是连接断开，重连后重试一次
	PrintOdbcError(stmt, SQL_HANDLE_STMT);
	OutputDebugString(_T("[CLightingDB] ClearLightingIdMap: DELETE failed, retrying after reconnect...\n"));
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	// 重新获取连接
	pUseConn = EnsureTlsConnection();
	if (pUseConn == SQL_NULL_HANDLE)
		pUseConn = GetOrCreateConn();
	if (pUseConn == SQL_NULL_HANDLE)
		return FALSE;

	// 重试 TRUNCATE
	ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
		return FALSE;
	}

	sprintf_s(sqlBuf, sizeof(sqlBuf), "TRUNCATE TABLE ivs_lcd_idmap");
	ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlBuf, SQL_NTS);
	if (SQL_SUCCEEDED(ret)) {
		OutputDebugString(_T("[CLightingDB] ClearLightingIdMap: retry success (TRUNCATE)\n"));
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return TRUE;
	}

	// 重试 DELETE
	sprintf_s(sqlBuf, sizeof(sqlBuf), "DELETE FROM ivs_lcd_idmap");
	ret = SQLExecDirectA(stmt, (SQLCHAR*)sqlBuf, SQL_NTS);
	if (SQL_SUCCEEDED(ret)) {
		OutputDebugString(_T("[CLightingDB] ClearLightingIdMap: retry success (DELETE)\n"));
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return TRUE;
	}

	PrintOdbcError(stmt, SQL_HANDLE_STMT);
	OutputDebugString(_T("[CLightingDB] ClearLightingIdMap: retry also failed\n"));
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return FALSE;
}

SQLHDBC CLightingDB::EnsureTlsConnection()
{
	if (IsTlsLightingDBConnected() && GetTlsLightingConnPtr() != SQL_NULL_HANDLE)
		return GetTlsLightingConnPtr();

	CAni_Data_Serever_PCApp* pApp = (CAni_Data_Serever_PCApp*)AfxGetApp();
	CString strServer = pApp ? pApp->m_strLightingDBServer : _T("localhost");
	CString strDBName = pApp ? pApp->m_strLightingDBName : _T("ivs_lcd");
	CString strUser   = pApp ? pApp->m_strLightingDBUser : _T("root");
	CString strPassword = pApp ? pApp->m_strLightingDBPassword : _T("");

	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] EnsureTlsConnection: Server=%s, DB=%s, User=%s, Pwd=%s\n"),
		strServer, strDBName, strUser,
		strPassword.IsEmpty() ? _T("(empty)") : _T("******")));

	CheckAndReconnectTlsLighting(strServer, strDBName, strUser, strPassword, theApp.m_pTestLog);

	SQLHDBC hConn = GetTlsLightingConnPtr();
	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] EnsureTlsConnection: connPtr=0x%p, g_bTlsLightingDBConnected=%d\n"),
		(void*)hConn, g_bTlsLightingDBConnected));

	return hConn;
}

SQLHDBC CLightingDB::GetOrCreateConn()
{
	if (IsTlsLightingDBConnected() && GetTlsLightingConnPtr() != SQL_NULL_HANDLE)
		return GetTlsLightingConnPtr();

	return EnsureTlsConnection();
}

BOOL CLightingDB::ExecQueryWithRetry(LPCWSTR strFuncName, SQLHDBC& connRef, const std::string& sqlQuery,
	SQLHSTMT& stmtRef, SQLRETURN* outRet)
{
	SQLRETURN ret = SQLExecDirectA(stmtRef, (SQLCHAR*)sqlQuery.c_str(), SQL_NTS);
	if (SQL_SUCCEEDED(ret)) {
		if (outRet) *outRet = ret;
		return TRUE;
	}

	// SQL 执行失败，打印错误并尝试断线重连后重试
	PrintOdbcError(stmtRef, SQL_HANDLE_STMT);
	SQLFreeHandle(SQL_HANDLE_STMT, stmtRef);

	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] %s: SQL failed, retrying after reconnect...\n"), strFuncName));

	// 重新获取连接
	connRef = EnsureTlsConnection();
	if (connRef == SQL_NULL_HANDLE)
		connRef = GetOrCreateConn();
	if (connRef == SQL_NULL_HANDLE) {
		stmtRef = SQL_NULL_HANDLE;
		if (outRet) *outRet = SQL_ERROR;
		return FALSE;
	}

	// 重建语句句柄
	ret = SQLAllocHandle(SQL_HANDLE_STMT, connRef, &stmtRef);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(connRef, SQL_HANDLE_DBC);
		stmtRef = SQL_NULL_HANDLE;
		if (outRet) *outRet = ret;
		return FALSE;
	}

	// 重试 SQL 执行
	ret = SQLExecDirectA(stmtRef, (SQLCHAR*)sqlQuery.c_str(), SQL_NTS);
	if (SQL_SUCCEEDED(ret)) {
		OutputDebugString(CStringSupport::FormatString(
			_T("[CLightingDB] %s: retry succeeded\n"), strFuncName));
		if (outRet) *outRet = ret;
		return TRUE;
	}

	PrintOdbcError(stmtRef, SQL_HANDLE_STMT);
	OutputDebugString(CStringSupport::FormatString(
		_T("[CLightingDB] %s: retry also failed\n"), strFuncName));
	SQLFreeHandle(SQL_HANDLE_STMT, stmtRef);
	stmtRef = SQL_NULL_HANDLE;
	if (outRet) *outRet = ret;
	return FALSE;
}

CLightingDB& GetDBInterface()
{
	return CLightingDB::Get();
}

BOOL CLightingDB::QueryDefectCodeByBarcode(CString strBarcode, CString& strCode, CString& strGrade)
{
	strCode = _T("");
	strGrade = _T("");
	m_strLastError = _T("");

	SQLHDBC conn = GetOrCreateConn();
	if (conn == SQL_NULL_HANDLE) {
		m_strLastError = _T("No database connection");
		return FALSE;
	}

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(conn, SQL_HANDLE_DBC);
		m_strLastError = _T("Failed to allocate statement");
		return FALSE;
	}

	string sqlStr = "SELECT Code_AOI, Grade_AOI FROM ivs_lcd_inspectionresult WHERE Barcode = '" + UnicodeToMultiByte(strBarcode.GetString()) + "'";
	if (!ExecQueryWithRetry(_T("QueryDefectCodeByBarcode"), conn, sqlStr, stmt, &ret)) {
		PrintOdbcError(stmt, SQL_HANDLE_STMT);
		m_strLastError = _T("Query failed");
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return FALSE;
	}

	ret = SQLFetch(stmt);
	if (SQL_SUCCEEDED(ret)) {
		SQLCHAR buf[101] = {0};
		SQLLEN len = 0;
		SQLGetData(stmt, 1, SQL_C_CHAR, buf, sizeof(buf), &len);
		strCode = CA2W((char*)buf);
		SQLGetData(stmt, 2, SQL_C_CHAR, buf, sizeof(buf), &len);
		strGrade = CA2W((char*)buf);
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return TRUE;
	}
	if (ret == SQL_NO_DATA)
		m_strLastError = _T("Query returned no results");
	else
		m_strLastError = _T("Fetch failed");
	PrintOdbcError(stmt, SQL_HANDLE_STMT);
	SQLFreeHandle(SQL_HANDLE_STMT, stmt);
	return FALSE;
}
