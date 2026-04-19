void CAni_Data_Serever_PCApp::GetLightingResultByBarcode(CString strBarcode, CString& strAOIResult, CString& strCodeAOI, CString& strGradeAOI, BOOL& bValid)
{
	strAOIResult = _T("");
	strCodeAOI = _T("");
	strGradeAOI = _T("");
	bValid = FALSE;

	// 获取当前线程ID用于调试
	DWORD threadId = GetCurrentThreadId();
	OutputDebugString(CStringSupport::FormatString(_T("[DBG] GetLightingResultByBarcode: ThreadID=%lu, Barcode=%s\n"), threadId, strBarcode));

	if (!theApp.m_bLightingDBConnected || theApp.m_pLightingConn == SQL_NULL_HANDLE)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pFTPLog->LOG_INFO(_T("GetLightingResultByBarcode: Database not connected"));
			return;
		}
	}

	// 先根据 Barcode 从 ivs_lcd_idmap 表找到 UniqueID
	CString strUniqueID;

	BOOL bRetry = FALSE;
	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret;
	try {
		CString strSQL;
		strSQL.Format(_T("SELECT UniqueID FROM ivs_lcd_idmap WHERE Barcode = '%s'"), strBarcode);
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] GetLightingResultByBarcode: ThreadID=%lu, SQL=%s\n"), threadId, strSQL));

		ret = SQLAllocHandle(SQL_HANDLE_STMT, theApp.m_pLightingConn, &stmt);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(theApp.m_pLightingConn, SQL_HANDLE_DBC);
			bRetry = TRUE;
			throw 1;
		}

		ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strSQL), SQL_NTS);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			bRetry = TRUE;
			throw 1;
		}

		ret = SQLFetch(stmt);
		if (SQL_SUCCEEDED(ret))
		{
			SQLCHAR uniqueIDBuf[256];
			SQLLEN len;
			SQLGetData(stmt, 1, SQL_C_CHAR, uniqueIDBuf, sizeof(uniqueIDBuf), &len);
			strUniqueID = CA2W((char*)uniqueIDBuf);
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] GetLightingResultByBarcode: ThreadID=%lu, Found UniqueID=%s for Barcode=%s\n"), threadId, strUniqueID, strBarcode));
			theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
				_T("GetLightingResultByBarcode: Found UniqueID=%s for Barcode=%s"), strUniqueID, strBarcode));
		}
		else
		{
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] GetLightingResultByBarcode: ThreadID=%lu, No UniqueID found for Barcode=%s\n"), threadId, strBarcode));
			theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
				_T("GetLightingResultByBarcode: No UniqueID found for Barcode=%s"), strBarcode));
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			return;
		}
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		stmt = SQL_NULL_HANDLE;
	}
	catch (...) {
		if (stmt != SQL_NULL_HANDLE) {
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
		}
		bRetry = TRUE;
	}

	if (bRetry) {
		theApp.m_bLightingDBConnected = FALSE;
		if (!ConnectLightingDatabase())
		{
			theApp.m_pFTPLog->LOG_INFO(_T("GetLightingResultByBarcode: Reconnect failed"));
			return;
		}
		try {
			CString strSQL;
			strSQL.Format(_T("SELECT UniqueID FROM ivs_lcd_idmap WHERE Barcode = '%s'"), strBarcode);

			ret = SQLAllocHandle(SQL_HANDLE_STMT, theApp.m_pLightingConn, &stmt);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(theApp.m_pLightingConn, SQL_HANDLE_DBC);
				throw 1;
			}

			ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strSQL), SQL_NTS);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(stmt, SQL_HANDLE_STMT);
				throw 1;
			}

			ret = SQLFetch(stmt);
			if (SQL_SUCCEEDED(ret))
			{
				SQLCHAR uniqueIDBuf[256];
				SQLLEN len;
				SQLGetData(stmt, 1, SQL_C_CHAR, uniqueIDBuf, sizeof(uniqueIDBuf), &len);
				strUniqueID = CA2W((char*)uniqueIDBuf);
				theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
					_T("GetLightingResultByBarcode: Retry success - Found UniqueID=%s for Barcode=%s"), strUniqueID, strBarcode));
			}
			else
			{
				theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
					_T("GetLightingResultByBarcode: Retry - No UniqueID found for Barcode=%s"), strBarcode));
				SQLFreeHandle(SQL_HANDLE_STMT, stmt);
				return;
			}
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
		}
		catch (...) {
			if (stmt != SQL_NULL_HANDLE) {
				SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			}
			theApp.m_pFTPLog->LOG_INFO(_T("GetLightingResultByBarcode: Retry SQL error"));
			return;
		}
	}

	// 再根据 UniqueID 查询 IVS_LCD_InspectionResult 表
	LightingInspectionResult result = QueryInspectionResult(strUniqueID);
	if (result.m_bValid)
	{
		strAOIResult = result.m_strAOIResult;
		strCodeAOI = result.m_strCodeAOI;
		strGradeAOI = result.m_strGradeAOI;
		bValid = TRUE;
	}
}

// DFS 模块调用：根据 UniqueID 查询点灯缺陷详情列表（AOI 缺陷详情）
BOOL CAni_Data_Serever_PCApp::QueryLightingDefectList(CString strUniqueID, std::vector<LUMITOP_SDFSDefectDataBegin>& vecDefects)
{
	vecDefects.clear();

	if (!theApp.m_bLightingDBConnected || theApp.m_pLightingConn == SQL_NULL_HANDLE)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pLightingLog->LOG_INFO(_T("QueryLightingDefectList: Database not connected"));
			return FALSE;
		}
	}

	BOOL bRetry = FALSE;
	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret;
	try {
		// 查询 IVS_LCD_AOIResult 表
		CString strSQL;
		strSQL.Format(_T("SELECT DefectIndex, Type, PatternID, PatternName, Pos_x, Pos_y, Pos_width, Pos_height, ")
			_T("TrueSize, GrayScale, GrayScale_BK, GrayScaleDiff, Code_AOI, Grade_AOI ")
			_T("FROM ivs_lcd_aoidefect WHERE GUID_IVS_LCD_InspectionResult = '%s' ORDER BY DefectIndex"),
			strUniqueID);

		ret = SQLAllocHandle(SQL_HANDLE_STMT, theApp.m_pLightingConn, &stmt);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(theApp.m_pLightingConn, SQL_HANDLE_DBC);
			bRetry = TRUE;
			throw 1;
		}

		ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strSQL), SQL_NTS);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			bRetry = TRUE;
			throw 1;
		}

		int iDefectCount = 0;
		while ((ret = SQLFetch(stmt)) == SQL_SUCCESS)
		{
			LUMITOP_SDFSDefectDataBegin defect;

			defect.strPANEL_ID = strUniqueID;
			
			SQLCHAR defectIndexBuf[51], patternIDBuf[101], patternNameBuf[101],
				posXBuf[51], posYBuf[51], grayScaleBuf[51];
			SQLLEN lenDefectIndex, lenPatternID, lenPatternName, lenPosX, lenPosY, lenGrayScale;
			
			SQLGetData(stmt, 1, SQL_C_CHAR, defectIndexBuf, sizeof(defectIndexBuf), &lenDefectIndex);
			defect.strPOINT = CA2W((char*)defectIndexBuf);
			
			SQLGetData(stmt, 3, SQL_C_CHAR, patternIDBuf, sizeof(patternIDBuf), &lenPatternID);
			CString strPatternID = CA2W((char*)patternIDBuf);
			
			SQLGetData(stmt, 4, SQL_C_CHAR, patternNameBuf, sizeof(patternNameBuf), &lenPatternName);
			CString strPatternName = CA2W((char*)patternNameBuf);
			
			defect.strLUMITOP_PTRN = strPatternName.IsEmpty() ? strPatternID : strPatternName;
			
			SQLGetData(stmt, 5, SQL_C_CHAR, posXBuf, sizeof(posXBuf), &lenPosX);
			defect.strX = CA2W((char*)posXBuf);
			
			SQLGetData(stmt, 6, SQL_C_CHAR, posYBuf, sizeof(posYBuf), &lenPosY);
			defect.strY = CA2W((char*)posYBuf);
			
			SQLGetData(stmt, 11, SQL_C_CHAR, grayScaleBuf, sizeof(grayScaleBuf), &lenGrayScale);
			defect.strLV = CA2W((char*)grayScaleBuf);

			// CIE 坐标暂时用 0 填充（如有相关字段可补充）
			defect.strCIE_X = _T("0");
			defect.strCIE_Y = _T("0");

			// CCT 相关字段暂时用 0 填充
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

		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
			_T("QueryLightingDefectList: Found %d defects for UniqueID=%s"),
			iDefectCount, strUniqueID));

		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		return TRUE;
	}
	catch (...) {
		if (stmt != SQL_NULL_HANDLE) {
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		}
		bRetry = TRUE;
	}

	if (bRetry) {
		theApp.m_bLightingDBConnected = FALSE;
		if (!ConnectLightingDatabase())
		{
			theApp.m_pLightingLog->LOG_INFO(_T("QueryLightingDefectList: Reconnect failed"));
			return FALSE;
		}
		try {
			CString strSQL;
			strSQL.Format(_T("SELECT DefectIndex, Type, PatternID, PatternName, Pos_x, Pos_y, Pos_width, Pos_height, ")
				_T("TrueSize, GrayScale, GrayScale_BK, GrayScaleDiff, Code_AOI, Grade_AOI ")
				_T("FROM IVS_LCD_AOIResult WHERE GUID_IVS_LCD_InspectionResult = '%s' ORDER BY DefectIndex"),
				strUniqueID);

			ret = SQLAllocHandle(SQL_HANDLE_STMT, theApp.m_pLightingConn, &stmt);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(theApp.m_pLightingConn, SQL_HANDLE_DBC);
				throw 1;
			}

			ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strSQL), SQL_NTS);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(stmt, SQL_HANDLE_STMT);
				throw 1;
			}

			int iDefectCount = 0;
			while ((ret = SQLFetch(stmt)) == SQL_SUCCESS)
			{
				LUMITOP_SDFSDefectDataBegin defect;

				defect.strPANEL_ID = strUniqueID;
				
				SQLCHAR defectIndexBuf[51], patternIDBuf[101], patternNameBuf[101],
					posXBuf[51], posYBuf[51], grayScaleBuf[51];
				SQLLEN lenDefectIndex, lenPatternID, lenPatternName, lenPosX, lenPosY, lenGrayScale;
				
				SQLGetData(stmt, 1, SQL_C_CHAR, defectIndexBuf, sizeof(defectIndexBuf), &lenDefectIndex);
				defect.strPOINT = CA2W((char*)defectIndexBuf);
				
				SQLGetData(stmt, 3, SQL_C_CHAR, patternIDBuf, sizeof(patternIDBuf), &lenPatternID);
				CString strPatternID = CA2W((char*)patternIDBuf);
				
				SQLGetData(stmt, 4, SQL_C_CHAR, patternNameBuf, sizeof(patternNameBuf), &lenPatternName);
				CString strPatternName = CA2W((char*)patternNameBuf);
				
				defect.strLUMITOP_PTRN = strPatternName.IsEmpty() ? strPatternID : strPatternName;
				
				SQLGetData(stmt, 5, SQL_C_CHAR, posXBuf, sizeof(posXBuf), &lenPosX);
				defect.strX = CA2W((char*)posXBuf);
				
				SQLGetData(stmt, 6, SQL_C_CHAR, posYBuf, sizeof(posYBuf), &lenPosY);
				defect.strY = CA2W((char*)posYBuf);
				
				SQLGetData(stmt, 11, SQL_C_CHAR, grayScaleBuf, sizeof(grayScaleBuf), &lenGrayScale);
				defect.strLV = CA2W((char*)grayScaleBuf);

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

			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("QueryLightingDefectList: Retry success - Found %d defects for UniqueID=%s"),
				iDefectCount, strUniqueID));

			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			return TRUE;
		}
		catch (...) {
			if (stmt != SQL_NULL_HANDLE) {
				SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			}
			theApp.m_pLightingLog->LOG_INFO(_T("QueryLightingDefectList: Retry SQL error"));
		}
	}

	return FALSE;
}

// DFS 模块调用：根据 UniqueID 查询 AOI 缺陷详情列表（点灯缺陷）
BOOL CAni_Data_Serever_PCApp::QueryAOIDefectList(CString strUniqueID, std::vector<SDFSDefectDataBegin>& vecDefects, SQLHDBC pConn)
{
	CString strFilePath;
	vecDefects.clear();

	// 获取当前线程ID
	DWORD threadId = GetCurrentThreadId();
	OutputDebugString(_T("[DBG] QueryAOIDefectList: Entered function\n"));
	OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: ThreadID=%lu, strUniqueID=%s, pConn=%p\n"), threadId, strUniqueID, pConn));

	// 优先使用线程局部连接（TLS），避免多线程共享连接
	SQLHDBC pUseConn = pConn;
	
	// 如果传入了连接，优先使用传入的连接
	if (pUseConn != SQL_NULL_HANDLE)
	{
		OutputDebugString(_T("[DBG] QueryAOIDefectList: Using provided connection pConn\n"));
	}
	else
	{
		// 使用心跳检测并自动重连
		if (CheckAndReconnectTlsLighting(
			theApp.m_strLightingDBServer,
			theApp.m_strLightingDBName,
			theApp.m_strLightingDBUser,
			theApp.m_strLightingDBPassword,
			theApp.m_pLightingLog))
		{
			pUseConn = GetTlsLightingConnPtr();
			OutputDebugString(_T("[DBG] QueryAOIDefectList: TLS connection ready\n"));
		}
		else
		{
			// TLS 连接失败，使用全局连接作为后备
			OutputDebugString(_T("[DBG] QueryAOIDefectList: TLS connection failed, using global connection\n"));
			if (!theApp.m_bLightingDBConnected || theApp.m_pLightingConn == SQL_NULL_HANDLE)
			{
				OutputDebugString(_T("[DBG] QueryAOIDefectList: DB not connected, trying to connect...\n"));
				if (!ConnectLightingDatabase())
				{
					OutputDebugString(_T("[DBG] QueryAOIDefectList: ConnectLightingDatabase FAILED\n"));
					theApp.m_pLightingLog->LOG_INFO(_T("QueryAOIDefectList: Database not connected"));
					return FALSE;
				}
				OutputDebugString(_T("[DBG] QueryAOIDefectList: ConnectLightingDatabase SUCCESS\n"));
			}
			pUseConn = theApp.m_pLightingConn;
		}
	}

	BOOL bRetry = FALSE;
	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret;

	try {
		// 查询 IVS_LCD_AOIResult 表（包含 ImagePath）
		CString strSQL;
		strSQL.Format(_T("SELECT DefectIndex, Type, PatternID, PatternName, Pos_x, Pos_y, Pos_width, Pos_height, ")
			_T("TrueSize, GrayScale, GrayScale_BK, GrayScaleDiff, Code_AOI, Grade_AOI, ImagePath ")
			_T("FROM ivs_lcd_aoidefect WHERE GUID_IVS_LCD_InspectionResult = '%s' ORDER BY DefectIndex"),
			strUniqueID);

		OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: SQL=%s\n"), strSQL));

		OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: ThreadID=%lu, Creating Statement... pUseConn=%p, stmt=%p\n"), threadId, pUseConn, stmt));
		ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
			bRetry = TRUE;
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: ThreadID=%lu, SQLAllocHandle failed, ret=%d\n"), threadId, ret));
			throw 1;
		}
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: ThreadID=%lu, Creating Statement completed, stmt=%p\n"), threadId, stmt));

		// 诊断：检查 stmt 和 pUseConn 是否有效
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: stmt=%p, pUseConn=%p\n"), stmt, pUseConn));
		
		// 诊断：检查字符串转换
		CT2A strSQLA(strSQL);
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: SQL length=%d\n"), strlen((LPCSTR)strSQLA)));

		OutputDebugString(_T("[DBG] QueryAOIDefectList: Calling SQLExecDirectA...\n"));
		ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)strSQLA, SQL_NTS);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			bRetry = TRUE;
			throw 1;
		}
		OutputDebugString(_T("[DBG] QueryAOIDefectList: SQLExecDirectA completed, now fetching results...\n"));

		int iDefectCount = 0;
		ret = SQLFetch(stmt);
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: First SQLFetch ret=%d\n"), ret));
		
		while (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: Processing row %d...\n"), iDefectCount + 1));
			
			SDFSDefectDataBegin defect;
			SQLCHAR defectIndexBuf[51], typeBuf[51], patternIDBuf[101], patternNameBuf[101], 
				posXBuf[51], posYBuf[51], posWidthBuf[51], posHeightBuf[51], 
				trueSizeBuf[51], grayScaleBuf[51], grayScaleBKBuf[51], grayScaleDiffBuf[51], 
				codeAOIBuf[51], gradeAOIBuf[51], imagePathBuf[512];
			SQLLEN lenDefectIndex, lenType, lenPatternID, lenPatternName, 
				lenPosX, lenPosY, lenPosWidth, lenPosHeight, 
				lenTrueSize, lenGrayScale, lenGrayScaleBK, lenGrayScaleDiff, 
				lenCodeAOI, lenGradeAOI, lenImagePath;

			defect.strPANEL_ID = strUniqueID;
			
			SQLGetData(stmt, 1, SQL_C_CHAR, defectIndexBuf, sizeof(defectIndexBuf), &lenDefectIndex);
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: GetData col1 done, len=%d\n"), lenDefectIndex));
			SQLGetData(stmt, 2, SQL_C_CHAR, typeBuf, sizeof(typeBuf), &lenType);
			SQLGetData(stmt, 3, SQL_C_CHAR, patternIDBuf, sizeof(patternIDBuf), &lenPatternID);
			SQLGetData(stmt, 4, SQL_C_CHAR, patternNameBuf, sizeof(patternNameBuf), &lenPatternName);
			SQLGetData(stmt, 5, SQL_C_CHAR, posXBuf, sizeof(posXBuf), &lenPosX);
			SQLGetData(stmt, 6, SQL_C_CHAR, posYBuf, sizeof(posYBuf), &lenPosY);
			SQLGetData(stmt, 7, SQL_C_CHAR, posWidthBuf, sizeof(posWidthBuf), &lenPosWidth);
			SQLGetData(stmt, 8, SQL_C_CHAR, posHeightBuf, sizeof(posHeightBuf), &lenPosHeight);
			SQLGetData(stmt, 9, SQL_C_CHAR, trueSizeBuf, sizeof(trueSizeBuf), &lenTrueSize);
			SQLGetData(stmt, 10, SQL_C_CHAR, grayScaleBuf, sizeof(grayScaleBuf), &lenGrayScale);
			SQLGetData(stmt, 11, SQL_C_CHAR, grayScaleBKBuf, sizeof(grayScaleBKBuf), &lenGrayScaleBK);
			SQLGetData(stmt, 12, SQL_C_CHAR, grayScaleDiffBuf, sizeof(grayScaleDiffBuf), &lenGrayScaleDiff);
			SQLGetData(stmt, 13, SQL_C_CHAR, codeAOIBuf, sizeof(codeAOIBuf), &lenCodeAOI);
			SQLGetData(stmt, 14, SQL_C_CHAR, gradeAOIBuf, sizeof(gradeAOIBuf), &lenGradeAOI);
			SQLGetData(stmt, 15, SQL_C_CHAR, imagePathBuf, sizeof(imagePathBuf), &lenImagePath);
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: GetData all cols done, imagePathLen=%d\n"), lenImagePath));

			defect.strDEFECT_DATA_NUM = (char*)defectIndexBuf;
			// 如果 DefectIndex 从 0 开始，则 +1 使其从 1 开始（与参考CSV格式一致）
			if (!defect.strDEFECT_DATA_NUM.IsEmpty())
			{
				int nIndex = _ttoi(defect.strDEFECT_DATA_NUM);
				defect.strDEFECT_DATA_NUM.Format(_T("%d"), nIndex + 1);
			}
			defect.strDEFECT_TYPE = (char*)typeBuf;

			// PatternID 和 PatternName 组合
			CString strPatternID = CA2T((char*)patternIDBuf);
			CString strPatternName = CA2T((char*)patternNameBuf);
			defect.strDEFECT_PTRN = strPatternName.IsEmpty() ? strPatternID : strPatternName;

			defect.strDEFECT_CODE = (char*)codeAOIBuf;
			defect.strDEFECT_GRADE = (char*)gradeAOIBuf;
			defect.strX = (char*)posXBuf;
			defect.strY = (char*)posYBuf;
			defect.strSIZE = (char*)trueSizeBuf;

			// 从 IVS_LCD_AOIResult 表读取缺陷图像路径（绝对路径）
			defect.strIMAGE_DATA = (char*)imagePathBuf;

			// 调试日志：打印每条缺陷的详细信息
			OutputDebugString(CStringSupport::FormatString(
				_T("[DBG] QueryAOIDefectList: Row %d - DefectIndex=%s, Type=%s, Pattern=%s, Code=%s, Grade=%s, X=%s, Y=%s, Size=%s, ImagePath=%s\n"),
				iDefectCount + 1,
				defect.strDEFECT_DATA_NUM, 
				defect.strDEFECT_TYPE,
				defect.strDEFECT_PTRN,
				defect.strDEFECT_CODE,
				defect.strDEFECT_GRADE,
				defect.strX,
				defect.strY,
				defect.strSIZE,
				defect.strIMAGE_DATA));

			defect.strCAM_INSPECT = _T("");
			defect.strZone = _T("");
			defect.strInspName = _T("");

			vecDefects.push_back(defect);
			iDefectCount++;
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: push_back done, count=%d\n"), iDefectCount));
			
			// 获取下一行数据
			ret = SQLFetch(stmt);
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: Next SQLFetch ret=%d\n"), ret));
		}
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: Fetch loop ended, ret=%d\n"), ret));
		
		// Free statement before logging to avoid any potential issues
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		stmt = SQL_NULL_HANDLE;
		OutputDebugString(_T("[DBG] QueryAOIDefectList: SQLFreeHandle done\n"));

		CString strLog;
		strLog.Format(_T("QueryAOIDefectList: Found %d defects for UniqueID=%s"), iDefectCount, strUniqueID);
		theApp.m_pLightingLog->LOG_INFO(strLog);
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] %s\n"), strLog));

		return TRUE;
	}
	catch (...) {
		DWORD lastError = GetLastError();
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: EXCEPTION CAUGHT! LastError=%lu, ThreadID=%lu\n"), lastError, threadId));
		if (stmt != SQL_NULL_HANDLE) {
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
		}
		theApp.m_pLightingLog->LOG_INFO(_T("QueryAOIDefectList: SQL error, reconnecting..."));
		OutputDebugString(_T("[DBG] QueryAOIDefectList SQL EXCEPTION\n"));
		bRetry = TRUE;
	}

	if (bRetry) {
		// 诊断：检查为什么进入重试逻辑
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList: ThreadID=%lu, Entering retry logic, bRetry=%d, pConn=%p\n"), threadId, bRetry, pConn));
		
		// 即使传入了连接参数，如果 TLS 连接失败也需要重试
		// 先尝试重新建立 TLS 连接
		OutputDebugString(_T("[DBG] QueryAOIDefectList: Retrying with new TLS connection...\n"));
		
		// 强制重新创建 TLS 连接（不使用现有的可能已损坏的连接）
		CloseTlsLightingConnection();
		
		if (!GetTlsLightingConnection(
			theApp.m_strLightingDBServer,
			theApp.m_strLightingDBName,
			theApp.m_strLightingDBUser,
			theApp.m_strLightingDBPassword,
			theApp.m_pLightingLog))
		{
			OutputDebugString(_T("[DBG] QueryAOIDefectList: TLS reconnection FAILED\n"));
			theApp.m_pLightingLog->LOG_INFO(_T("QueryAOIDefectList: TLS reconnection failed"));
			return FALSE;
		}
		OutputDebugString(_T("[DBG] QueryAOIDefectList: TLS reconnection SUCCESS\n"));
		pUseConn = GetTlsLightingConnPtr();
		try {
			CString strSQL;
			strSQL.Format(_T("SELECT DefectIndex, Type, PatternID, PatternName, Pos_x, Pos_y, Pos_width, Pos_height, ")
				_T("TrueSize, GrayScale, GrayScale_BK, GrayScaleDiff, Code_AOI, Grade_AOI, ImagePath ")
				_T("FROM ivs_lcd_aoidefect WHERE GUID_IVS_LCD_InspectionResult = '%s' ORDER BY DefectIndex"),
				strUniqueID);

			OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList Retry: SQL=%s\n"), strSQL));

			ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
				throw 1;
			}
			
			// 使用与主逻辑相同的字符串转换方式
			CT2A strSQLA(strSQL);
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] QueryAOIDefectList Retry: SQL length=%d\n"), strlen((LPCSTR)strSQLA)));
			
			ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)strSQLA, SQL_NTS);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(stmt, SQL_HANDLE_STMT);
				throw 1;
			}

			int iDefectCount = 0;
			ret = SQLFetch(stmt);
			while (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			{
				SDFSDefectDataBegin defect;
				SQLCHAR defectIndexBuf[51], typeBuf[51], patternIDBuf[101], patternNameBuf[101], 
					posXBuf[51], posYBuf[51], trueSizeBuf[51], 
					codeAOIBuf[51], gradeAOIBuf[51], imagePathBuf[512];
				SQLLEN lenDefectIndex, lenType, lenPatternID, lenPatternName, 
					lenPosX, lenPosY, lenTrueSize, 
					lenCodeAOI, lenGradeAOI, lenImagePath;

				defect.strPANEL_ID = strUniqueID;
				
				SQLGetData(stmt, 1, SQL_C_CHAR, defectIndexBuf, sizeof(defectIndexBuf), &lenDefectIndex);
				SQLGetData(stmt, 2, SQL_C_CHAR, typeBuf, sizeof(typeBuf), &lenType);
				SQLGetData(stmt, 3, SQL_C_CHAR, patternIDBuf, sizeof(patternIDBuf), &lenPatternID);
				SQLGetData(stmt, 4, SQL_C_CHAR, patternNameBuf, sizeof(patternNameBuf), &lenPatternName);
				SQLGetData(stmt, 5, SQL_C_CHAR, posXBuf, sizeof(posXBuf), &lenPosX);
				SQLGetData(stmt, 6, SQL_C_CHAR, posYBuf, sizeof(posYBuf), &lenPosY);
				SQLGetData(stmt, 9, SQL_C_CHAR, trueSizeBuf, sizeof(trueSizeBuf), &lenTrueSize);
				SQLGetData(stmt, 13, SQL_C_CHAR, codeAOIBuf, sizeof(codeAOIBuf), &lenCodeAOI);
				SQLGetData(stmt, 14, SQL_C_CHAR, gradeAOIBuf, sizeof(gradeAOIBuf), &lenGradeAOI);
				SQLGetData(stmt, 15, SQL_C_CHAR, imagePathBuf, sizeof(imagePathBuf), &lenImagePath);

				defect.strDEFECT_DATA_NUM = (char*)defectIndexBuf;
				defect.strDEFECT_TYPE = (char*)typeBuf;

				CString strPatternID = CA2T((char*)patternIDBuf);
				CString strPatternName = CA2T((char*)patternNameBuf);
				defect.strDEFECT_PTRN = strPatternName.IsEmpty() ? strPatternID : strPatternName;

				defect.strDEFECT_CODE = (char*)codeAOIBuf;
				defect.strDEFECT_GRADE = (char*)gradeAOIBuf;
				defect.strX = (char*)posXBuf;
				defect.strY = (char*)posYBuf;
				defect.strSIZE = (char*)trueSizeBuf;

				defect.strIMAGE_DATA = (char*)imagePathBuf;

				defect.strCAM_INSPECT = _T("");
				defect.strZone = _T("");
				defect.strInspName = _T("");

				vecDefects.push_back(defect);
				iDefectCount++;
				
				// 获取下一行数据
				ret = SQLFetch(stmt);
			}

			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;

			CString strLog;
			strLog.Format(_T("QueryAOIDefectList: Retry success - Found %d defects for UniqueID=%s"), iDefectCount, strUniqueID);
			theApp.m_pLightingLog->LOG_INFO(strLog);
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] %s\n"), strLog));

			return TRUE;
		}
		catch (...) {
			if (stmt != SQL_NULL_HANDLE) {
				SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			}
			theApp.m_pLightingLog->LOG_INFO(_T("QueryAOIDefectList: Retry SQL error"));
			OutputDebugString(_T("[DBG] QueryAOIDefectList Retry SQL EXCEPTION\n"));
		}
	}

	return FALSE;
}

// 线程安全的 AOI 缺陷列表查询（包装 QueryAOIDefectList）
BOOL CAni_Data_Serever_PCApp::QueryAOIDefectListThreadSafe(CString strUniqueID, std::vector<SDFSDefectDataBegin>& vecDefects, SQLHDBC pConn)
{
	return QueryAOIDefectList(strUniqueID, vecDefects, pConn);
}

BOOL CAni_Data_Serever_PCApp::QueryAOIDefectListThreadSafe(CString strUniqueID, CDefectInfoList& vecDefects, SQLHDBC pConn)
{
	vecDefects.clear();

	std::vector<SDFSDefectDataBegin> vecRaw;
	if (!QueryAOIDefectList(strUniqueID, vecRaw, pConn))
		return FALSE;

	for (const auto& raw : vecRaw)
	{
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

// DFS 模块调用：根据 Barcode 查询 UniqueID
CString CAni_Data_Serever_PCApp::GetLightingUniqueIDByBarcode(CString strBarcode)
{
	CString strUniqueID = _T("");

	// 获取当前线程ID用于调试
	DWORD threadId = GetCurrentThreadId();
	OutputDebugString(CStringSupport::FormatString(_T("[DBG] GetLightingUniqueIDByBarcode: ThreadID=%lu, Barcode=%s\n"), threadId, strBarcode));

	if (!theApp.m_bLightingDBConnected || theApp.m_pLightingConn == SQL_NULL_HANDLE)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pFTPLog->LOG_INFO(_T("GetLightingUniqueIDByBarcode: Database not connected"));
			return strUniqueID;
		}
	}

	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret;
	BOOL bRetry = FALSE;

	try {
		CString strSQL;
		strSQL.Format(_T("SELECT UniqueID FROM ivs_lcd_idmap WHERE Barcode = '%s'"), strBarcode);
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] GetLightingUniqueIDByBarcode: ThreadID=%lu, SQL=%s\n"), threadId, strSQL));

		ret = SQLAllocHandle(SQL_HANDLE_STMT, theApp.m_pLightingConn, &stmt);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(theApp.m_pLightingConn, SQL_HANDLE_DBC);
			bRetry = TRUE;
			throw 1;
		}

		ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strSQL), SQL_NTS);
		if (SQL_SUCCEEDED(ret)) {
			ret = SQLFetch(stmt);
			if (SQL_SUCCEEDED(ret)) {
				SQLCHAR uniqueIDBuf[101];
				SQLLEN lenUniqueID;
				SQLGetData(stmt, 1, SQL_C_CHAR, uniqueIDBuf, sizeof(uniqueIDBuf), &lenUniqueID);
				strUniqueID = (char*)uniqueIDBuf;
				OutputDebugString(CStringSupport::FormatString(_T("[DBG] GetLightingUniqueIDByBarcode: ThreadID=%lu, Found UniqueID=%s\n"), threadId, strUniqueID));
			} else {
				OutputDebugString(CStringSupport::FormatString(_T("[DBG] GetLightingUniqueIDByBarcode: ThreadID=%lu, No UniqueID found for Barcode=%s\n"), threadId, strBarcode));
			}
		}
		else {
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			bRetry = TRUE;
			throw 1;
		}
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		stmt = SQL_NULL_HANDLE;
	}
	catch (...) {
		if (stmt != SQL_NULL_HANDLE) {
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
		}
		bRetry = TRUE;
	}

	if (bRetry) {
		theApp.m_pFTPLog->LOG_INFO(_T("GetLightingUniqueIDByBarcode: SQL error, reconnecting..."));
		theApp.m_bLightingDBConnected = FALSE;
		if (!ConnectLightingDatabase())
		{
			return strUniqueID;
		}
		try {
			CString strSQL;
			strSQL.Format(_T("SELECT UniqueID FROM ivs_lcd_idmap WHERE Barcode = '%s'"), strBarcode);

			ret = SQLAllocHandle(SQL_HANDLE_STMT, theApp.m_pLightingConn, &stmt);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(theApp.m_pLightingConn, SQL_HANDLE_DBC);
				throw 1;
			}

			ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strSQL), SQL_NTS);
			if (SQL_SUCCEEDED(ret)) {
				ret = SQLFetch(stmt);
				if (SQL_SUCCEEDED(ret)) {
					SQLCHAR uniqueIDBuf[101];
					SQLLEN lenUniqueID;
					SQLGetData(stmt, 1, SQL_C_CHAR, uniqueIDBuf, sizeof(uniqueIDBuf), &lenUniqueID);
					strUniqueID = (char*)uniqueIDBuf;
				}
			}
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
		}
		catch (...) {
			if (stmt != SQL_NULL_HANDLE) {
				SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			}
			theApp.m_pFTPLog->LOG_INFO(_T("GetLightingUniqueIDByBarcode: Retry SQL error"));
		}
	}

	OutputDebugString(CStringSupport::FormatString(_T("[DBG] GetLightingUniqueIDByBarcode: ThreadID=%lu, Returning UniqueID=%s\n"), threadId, strUniqueID));
	return strUniqueID;
}

// 根据治具号查询 ID 映射
BOOL CAni_Data_Serever_PCApp::QueryIdMapByFixtureNo(int fixtureNo, CString& uniqueID, CString& screenID, CString& markID)
{
	if (!theApp.m_bLightingDBConnected || theApp.m_pLightingConn == SQL_NULL_HANDLE)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pLightingLog->LOG_INFO(_T("QueryIdMapByFixtureNo: Database not connected"));
			return FALSE;
		}
	}

	BOOL bRetry = FALSE;
	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret;

	try {
		// 查询 ivs_lcd_idmap 表：MainAoiFixID=治具号, Barcode=产品码
		CString strSQL;
		strSQL.Format(_T("SELECT UniqueID, Barcode, MainAoiFixID FROM ivs_lcd_idmap WHERE MainAoiFixID = '%d'"), fixtureNo);

		ret = SQLAllocHandle(SQL_HANDLE_STMT, theApp.m_pLightingConn, &stmt);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(theApp.m_pLightingConn, SQL_HANDLE_DBC);
			bRetry = TRUE;
			throw 1;
		}

		ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strSQL), SQL_NTS);
		if (SQL_SUCCEEDED(ret)) {
			ret = SQLFetch(stmt);
			if (SQL_SUCCEEDED(ret)) {
				SQLCHAR uniqueIDBuf[101], screenIDBuf[101];
				SQLLEN lenUniqueID, lenScreenID;
				int mainAoiFixID = 0;

				SQLGetData(stmt, 1, SQL_C_CHAR, uniqueIDBuf, sizeof(uniqueIDBuf), &lenUniqueID);
				SQLGetData(stmt, 2, SQL_C_CHAR, screenIDBuf, sizeof(screenIDBuf), &lenScreenID);
				SQLGetData(stmt, 3, SQL_C_SLONG, &mainAoiFixID, 0, NULL);

				uniqueID = (char*)uniqueIDBuf;
				screenID = (char*)screenIDBuf;
				markID.Format(_T("%02d"), mainAoiFixID);

				theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
					_T("QueryIdMapByFixtureNo: FixtureNo=%d, UniqueID=%s, ScreenID=%s, MarkID=%s"),
					fixtureNo, uniqueID, screenID, markID));

				SQLFreeHandle(SQL_HANDLE_STMT, stmt);
				stmt = SQL_NULL_HANDLE;
				return TRUE;
			}
		}
		else {
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			bRetry = TRUE;
			throw 1;
		}
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		stmt = SQL_NULL_HANDLE;
	}
	catch (...) {
		if (stmt != SQL_NULL_HANDLE) {
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
		}
		theApp.m_pLightingLog->LOG_INFO(_T("[DBG] QueryIdMapByFixtureNo SQL error, reconnecting..."));
		OutputDebugString(_T("[DBG] QueryIdMapByFixtureNo SQL error\n"));
		bRetry = TRUE;
	}

	if (bRetry) {
		theApp.m_bLightingDBConnected = FALSE;
		if (!ConnectLightingDatabase())
		{
			theApp.m_pLightingLog->LOG_INFO(_T("QueryIdMapByFixtureNo: Reconnect failed"));
			return FALSE;
		}
		try {
			CString strSQL;
			strSQL.Format(_T("SELECT UniqueID, Barcode, MainAoiFixID FROM ivs_lcd_idmap WHERE MainAoiFixID = '%d'"), fixtureNo);

			ret = SQLAllocHandle(SQL_HANDLE_STMT, theApp.m_pLightingConn, &stmt);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(theApp.m_pLightingConn, SQL_HANDLE_DBC);
				throw 1;
			}

			ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strSQL), SQL_NTS);
			if (SQL_SUCCEEDED(ret)) {
				ret = SQLFetch(stmt);
				if (SQL_SUCCEEDED(ret)) {
					SQLCHAR uniqueIDBuf[101], screenIDBuf[101];
					SQLLEN lenUniqueID, lenScreenID;
					int mainAoiFixID = 0;

					SQLGetData(stmt, 1, SQL_C_CHAR, uniqueIDBuf, sizeof(uniqueIDBuf), &lenUniqueID);
					SQLGetData(stmt, 2, SQL_C_CHAR, screenIDBuf, sizeof(screenIDBuf), &lenScreenID);
					SQLGetData(stmt, 3, SQL_C_SLONG, &mainAoiFixID, 0, NULL);

					uniqueID = (char*)uniqueIDBuf;
					screenID = (char*)screenIDBuf;
					markID.Format(_T("%02d"), mainAoiFixID);

					theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
						_T("QueryIdMapByFixtureNo: Retry success - FixtureNo=%d, UniqueID=%s, ScreenID=%s, MarkID=%s"),
						fixtureNo, uniqueID, screenID, markID));

					SQLFreeHandle(SQL_HANDLE_STMT, stmt);
					stmt = SQL_NULL_HANDLE;
					return TRUE;
				}
			}
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
		}
		catch (...) {
			if (stmt != SQL_NULL_HANDLE) {
				SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			}
			theApp.m_pLightingLog->LOG_INFO(_T("[DBG] QueryIdMapByFixtureNo Retry SQL error"));
			OutputDebugString(_T("[DBG] QueryIdMapByFixtureNo Retry SQL error\n"));
		}
	}

	return FALSE;
}

BOOL CAni_Data_Serever_PCApp::ConnectLightingDatabase()
{
	// 获取当前线程ID
	DWORD threadId = GetCurrentThreadId();
	OutputDebugString(CStringSupport::FormatString(_T("[DBG] ConnectLightingDatabase: ThreadID=%lu, called\n"), threadId));
	
	// 如果已连接，直接返回
	if (theApp.m_bLightingDBConnected && theApp.m_pLightingConn != SQL_NULL_HANDLE) {
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] ConnectLightingDatabase: ThreadID=%lu, already connected\n"), threadId));
		return TRUE;
	}

	// 线程同步：避免在其他线程使用连接时关闭连接
	CSingleLock lock(&theApp.m_csLightingFlow, TRUE);
	
	// 再次检查连接状态（双重检查）
	if (theApp.m_bLightingDBConnected && theApp.m_pLightingConn != SQL_NULL_HANDLE) {
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] ConnectLightingDatabase: ThreadID=%lu, already connected (after lock)\n"), threadId));
		return TRUE;
	}

	// 先关闭旧连接（如果有）
	if (theApp.m_pLightingConn != SQL_NULL_HANDLE)
	{
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] ConnectLightingDatabase: ThreadID=%lu, Closing old connection\n"), threadId));
		SQLDisconnect(theApp.m_pLightingConn);
		SQLFreeHandle(SQL_HANDLE_DBC, theApp.m_pLightingConn);
		theApp.m_pLightingConn = SQL_NULL_HANDLE;
		theApp.m_bLightingDBConnected = FALSE;
	}

	// 初始化ODBC环境（如果未初始化）
	if (theApp.m_odbcEnv == SQL_NULL_HANDLE) {
		SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &theApp.m_odbcEnv);
		if (!SQL_SUCCEEDED(ret)) {
			theApp.m_pLightingLog->LOG_INFO(_T("Failed to allocate ODBC environment handle"));
			return FALSE;
		}
		SQLSetEnvAttr(theApp.m_odbcEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	}

	// 多驱动尝试连接
	const char* driverNames[] = {
		"MySQL ODBC 5.3 ANSI Driver",
		"MySQL ODBC 5.3 Unicode Driver",
		"MySQL ODBC 5.3 Driver"
	};

	SQLHDBC conn = SQL_NULL_HANDLE;
	SQLRETURN ret;
	BOOL connected = FALSE;

	for (int i = 0; i < sizeof(driverNames)/sizeof(driverNames[0]); i++) {
		if (conn != SQL_NULL_HANDLE) {
			SQLDisconnect(conn);
			SQLFreeHandle(SQL_HANDLE_DBC, conn);
		}

		ret = SQLAllocHandle(SQL_HANDLE_DBC, theApp.m_odbcEnv, &conn);
		if (!SQL_SUCCEEDED(ret)) {
			theApp.m_pLightingLog->LOG_INFO(_T("Failed to allocate ODBC connection handle"));
			continue;
		}

		//char* pLightingDBServer = CT2A(theApp.m_strLightingDBServer);

		// 转换
		string pLightingDBServer = UnicodeToMultiByte(theApp.m_strLightingDBServer.GetString()).c_str();
		string pLightingDBName = UnicodeToMultiByte(theApp.m_strLightingDBName.GetString()).c_str();
		string pLightingDBUser = UnicodeToMultiByte(theApp.m_strLightingDBUser.GetString()).c_str();
		string pLightingDBPassword = UnicodeToMultiByte(theApp.m_strLightingDBPassword.GetString()).c_str();
		//char* pLightingDBName = CT2A(theApp.m_strLightingDBName);
		//char* pLightingDBUser = CT2A(theApp.m_strLightingDBUser);
		//char* pLightingDBPassword = CT2A(theApp.m_strLightingDBPassword);

		char connStr[512];
		sprintf_s(connStr, sizeof(connStr), 
				"DRIVER={%s};SERVER=%s;PORT=%d;DATABASE=%s;UID=%s;PWD=%s;OPTION=3;",
				driverNames[i], pLightingDBServer.c_str(), 3306,
				pLightingDBName.c_str(),
				pLightingDBUser.c_str(),
				pLightingDBPassword.c_str());

		CString dbgMsg;
		dbgMsg.Format(_T("[DBG] ConnectLightingDatabase: ThreadID=%lu, Connecting with driver: %s"), threadId, CString(driverNames[i]));
		theApp.m_pLightingLog->LOG_INFO(dbgMsg);
		OutputDebugString(dbgMsg + _T("\n"));

		ret = SQLDriverConnectA(conn, NULL, (SQLCHAR*)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

		if (SQL_SUCCEEDED(ret)) {
			OutputDebugString(CStringSupport::FormatString(_T("[DBG] ConnectLightingDatabase: ThreadID=%lu, Successfully connected with driver: %s\n"), threadId, CString(driverNames[i])));
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("[DBG] Successfully connected with driver: %s"), CString(driverNames[i])));
			connected = TRUE;
			break;
		} else {
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("[DBG] Driver %s failed, trying next..."), CString(driverNames[i])));
			PrintOdbcError(conn, SQL_HANDLE_DBC);
		}
	}

	if (!connected) {
		OutputDebugString(CStringSupport::FormatString(_T("[DBG] ConnectLightingDatabase: ThreadID=%lu, Failed to connect to MySQL database with all drivers\n"), threadId));
		theApp.m_pLightingLog->LOG_INFO(_T("Failed to connect to MySQL database with all drivers"));
		if (conn != SQL_NULL_HANDLE) {
			SQLFreeHandle(SQL_HANDLE_DBC, conn);
		}
		return FALSE;
	}

	theApp.m_pLightingConn = conn;
	theApp.m_bLightingDBConnected = TRUE;
	OutputDebugString(CStringSupport::FormatString(_T("[DBG] ConnectLightingDatabase: ThreadID=%lu, MySQL connect SUCCESS\n"), threadId));
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
		_T("[DBG] MySQL connect SUCCESS: Server=%s, DB=%s, User=%s"),
		theApp.m_strLightingDBServer, theApp.m_strLightingDBName, theApp.m_strLightingDBUser));
	return TRUE;
}

// 关闭数据库连接
void CAni_Data_Serever_PCApp::CloseLightingDatabase()
{
	if (theApp.m_pLightingConn != SQL_NULL_HANDLE)
	{
		SQLDisconnect(theApp.m_pLightingConn);
		SQLFreeHandle(SQL_HANDLE_DBC, theApp.m_pLightingConn);
		theApp.m_pLightingConn = SQL_NULL_HANDLE;
	}

	theApp.m_bLightingDBConnected = FALSE;
	theApp.m_pLightingLog->LOG_INFO(_T("Lighting database connection closed"));
}

// DFS 模块专用的数据库连接
BOOL CAni_Data_Serever_PCApp::ConnectDfsLightingDatabase()
{
	// 如果已连接，直接返回
	if (theApp.m_bDfsLightingDBConnected && theApp.m_pDfsLightingConn != SQL_NULL_HANDLE)
		return TRUE;

	// 先关闭旧连接（如果有）
	if (theApp.m_pDfsLightingConn != SQL_NULL_HANDLE)
	{
		SQLDisconnect(theApp.m_pDfsLightingConn);
		SQLFreeHandle(SQL_HANDLE_DBC, theApp.m_pDfsLightingConn);
		theApp.m_pDfsLightingConn = SQL_NULL_HANDLE;
		theApp.m_bDfsLightingDBConnected = FALSE;
	}

	// 初始化ODBC环境（如果未初始化）
	if (theApp.m_odbcEnv == SQL_NULL_HANDLE) {
		SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &theApp.m_odbcEnv);
		if (!SQL_SUCCEEDED(ret)) {
			theApp.m_pFTPLog->LOG_INFO(_T("ConnectDfsLightingDatabase: Failed to allocate ODBC environment handle"));
			return FALSE;
		}
		SQLSetEnvAttr(theApp.m_odbcEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	}

	// 多驱动尝试连接
	const char* driverNames[] = {
		"MySQL ODBC 5.3 ANSI Driver",
		"MySQL ODBC 5.3 Unicode Driver",
		"MySQL ODBC 5.3 Driver"
	};

	SQLHDBC conn = SQL_NULL_HANDLE;
	SQLRETURN ret;
	BOOL connected = FALSE;

	for (int i = 0; i < sizeof(driverNames)/sizeof(driverNames[0]); i++) {
		if (conn != SQL_NULL_HANDLE) {
			SQLDisconnect(conn);
			SQLFreeHandle(SQL_HANDLE_DBC, conn);
		}

		ret = SQLAllocHandle(SQL_HANDLE_DBC, theApp.m_odbcEnv, &conn);
		if (!SQL_SUCCEEDED(ret)) {
			theApp.m_pFTPLog->LOG_INFO(_T("ConnectDfsLightingDatabase: Failed to allocate ODBC connection handle"));
			continue;
		}

		//char* pLightingDBServer = CT2A(theApp.m_strLightingDBServer);
		//char* pLightingDBName = CT2A(theApp.m_strLightingDBName);
		//char* pLightingDBUser = CT2A(theApp.m_strLightingDBUser);
		//char* pLightingDBPassword = CT2A(theApp.m_strLightingDBPassword);

		string pLightingDBServer = UnicodeToMultiByte(theApp.m_strLightingDBServer.GetString()).c_str();
		string pLightingDBName = UnicodeToMultiByte(theApp.m_strLightingDBName.GetString()).c_str();
		string pLightingDBUser = UnicodeToMultiByte(theApp.m_strLightingDBUser.GetString()).c_str();
		string pLightingDBPassword = UnicodeToMultiByte(theApp.m_strLightingDBPassword.GetString()).c_str();

		char connStr[512];
		sprintf_s(connStr, sizeof(connStr), 
				"DRIVER={%s};SERVER=%s;PORT=%d;DATABASE=%s;UID=%s;PWD=%s;OPTION=3;",
				driverNames[i], pLightingDBServer.c_str(), 3306,
			pLightingDBName.c_str(),
			pLightingDBUser.c_str(),
			pLightingDBPassword.c_str());

		CString dbgMsg;
		dbgMsg.Format(_T("[DFS] Connecting with driver: %s"), CString(driverNames[i]));
		theApp.m_pFTPLog->LOG_INFO(dbgMsg);
		OutputDebugString(dbgMsg + _T("\n"));

		ret = SQLDriverConnectA(conn, NULL, (SQLCHAR*)connStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);

		if (SQL_SUCCEEDED(ret)) {
			theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
				_T("ConnectDfsLightingDatabase: Successfully connected with driver: %s"), CString(driverNames[i])));
			connected = TRUE;
			break;
		} else {
			theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
				_T("[DFS] Driver %s failed, trying next..."), CString(driverNames[i])));
			PrintOdbcError(conn, SQL_HANDLE_DBC);
		}
	}

	if (!connected) {
		theApp.m_pFTPLog->LOG_INFO(_T("ConnectDfsLightingDatabase: Failed to connect to MySQL database with all drivers"));
		if (conn != SQL_NULL_HANDLE) {
			SQLFreeHandle(SQL_HANDLE_DBC, conn);
		}
		return FALSE;
	}

	theApp.m_pDfsLightingConn = conn;
	theApp.m_bDfsLightingDBConnected = TRUE;
	theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
		_T("ConnectDfsLightingDatabase: Connected to %s/%s"), theApp.m_strLightingDBServer, theApp.m_strLightingDBName));
	return TRUE;
}

// 关闭 DFS 数据库连接
void CAni_Data_Serever_PCApp::CloseDfsLightingDatabase()
{
	if (theApp.m_pDfsLightingConn != SQL_NULL_HANDLE)
	{
		SQLDisconnect(theApp.m_pDfsLightingConn);
		SQLFreeHandle(SQL_HANDLE_DBC, theApp.m_pDfsLightingConn);
		theApp.m_pDfsLightingConn = SQL_NULL_HANDLE;
	}

	theApp.m_bDfsLightingDBConnected = FALSE;
	theApp.m_pFTPLog->LOG_INFO(_T("CloseDfsLightingDatabase: DFS Lighting database connection closed"));
}

// 获取 DFS 模块专用的数据库连接（如果未连接则先连接）
SQLHDBC CAni_Data_Serever_PCApp::GetDfsLightingConnection()
{
	if (!theApp.m_bDfsLightingDBConnected || theApp.m_pDfsLightingConn == SQL_NULL_HANDLE)
	{
		if (!ConnectDfsLightingDatabase())
		{
			theApp.m_pFTPLog->LOG_INFO(_T("GetDfsLightingConnection: Failed to connect"));
			return SQL_NULL_HANDLE;
		}
	}
	return theApp.m_pDfsLightingConn;
}

LightingInspectionResult CAni_Data_Serever_PCApp::GetLightingResultByUniqueID(CString uniqueID)
{
	return QueryInspectionResult(uniqueID);
}

BOOL CAni_Data_Serever_PCApp::UpdateLightingInspectionResult(CString uniqueID)
{
	// 更新检测结果的状态（如果需要）
	// 根据业务需求决定是否需要更新数据库
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
		_T("UpdateLightingInspectionResult: UniqueID=%s"), uniqueID));
	return TRUE;
}

// 点灯检数据库操作函数 - 使用 ODBC
BOOL CAni_Data_Serever_PCApp::UpdateLightingIdMap(int fixtureNo, CString uniqueID, CString screenID, CString markID)
{
	// 获取当前线程ID
	DWORD threadId = GetCurrentThreadId();
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
		_T("[DBG] UpdateLightingIdMap ENTER: ThreadID=%lu, fixtureNo=%d, uniqueID=%s, screenID=%s, markID=%s"),
		threadId, fixtureNo, uniqueID, screenID, markID));

	// 优先使用线程局部连接（TLS），避免多线程共享连接
	// 使用心跳检测确保连接仍然有效，自动重连如果断开
	SQLHDBC pUseConn = SQL_NULL_HANDLE;

	if (CheckAndReconnectTlsLighting(
		theApp.m_strLightingDBServer,
		theApp.m_strLightingDBName,
		theApp.m_strLightingDBUser,
		theApp.m_strLightingDBPassword,
		theApp.m_pLightingLog))
	{
		pUseConn = GetTlsLightingConnPtr();
		theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: TLS connection ready (heartbeat OK)"));
	}
	else
	{
		// TLS 连接失败，使用全局连接作为后备
		theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: TLS connection failed, using global connection"));
		if (!theApp.m_bLightingDBConnected || theApp.m_pLightingConn == SQL_NULL_HANDLE)
		{
			theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: DB not connected, trying to connect..."));
			if (!ConnectLightingDatabase())
			{
				theApp.m_pLightingLog->LOG_INFO(_T("UpdateLightingIdMap: Database not connected"));
				return FALSE;
			}
			theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: Database connected successfully"));
		}
		pUseConn = theApp.m_pLightingConn;
	}

	if (pUseConn == SQL_NULL_HANDLE)
	{
		theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: No valid connection available"));
		return FALSE;
	}

	// 确保 Barcode 列存在（兼容旧数据库 - 不支持 ADD COLUMN IF NOT EXISTS）
	theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: Checking/adding Barcode column..."));
	SQLHSTMT stmt = SQL_NULL_HANDLE;
	SQLRETURN ret;

	theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: Using existing table structure"));

	// ivs_lcd_idmap 表结构：MainAoiFixID=治具号(1,2,3,4), UniqueID=唯一ID, Barcode=产品码
	// 转义 SQL 字符串中的单引号（避免注入与语法错误）
	CString strUniqueID = uniqueID;
	strUniqueID.Replace(_T("'"), _T("''"));
	CString strBarcode = screenID;
	strBarcode.Replace(_T("'"), _T("''"));
	CString strMarkID = markID;
	strMarkID.Replace(_T("'"), _T("''"));

	// 先查询记录是否存在
	CString strCheckSQL;
	strCheckSQL.Format(_T("SELECT COUNT(*) FROM ivs_lcd_idmap WHERE MainAoiFixID='%d'"), fixtureNo);
	SQLHSTMT checkStmt = SQL_NULL_HANDLE;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &checkStmt);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
		SQLFreeHandle(SQL_HANDLE_STMT, checkStmt);
		throw 1;
	}
	
	ret = SQLExecDirectA(checkStmt, (SQLCHAR*)(LPCSTR)CT2A(strCheckSQL), SQL_NTS);
	if (!SQL_SUCCEEDED(ret)) {
		PrintOdbcError(checkStmt, SQL_HANDLE_STMT);
		SQLFreeHandle(SQL_HANDLE_STMT, checkStmt);
		throw 1;
	}
	
	ret = SQLFetch(checkStmt);
	int count = 0;
	if (SQL_SUCCEEDED(ret)) {
		SQLGetData(checkStmt, 1, SQL_C_SLONG, &count, 0, NULL);
	}
	SQLFreeHandle(SQL_HANDLE_STMT, checkStmt);
	checkStmt = SQL_NULL_HANDLE;
	
	CString strSQL;
	if (count > 0) {
		// 记录存在，执行更新
		strSQL.Format(_T("UPDATE ivs_lcd_idmap SET UniqueID='%s', Barcode='%s' WHERE MainAoiFixID='%d'"),
			strUniqueID, strBarcode, fixtureNo);
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("[DBG] UpdateLightingIdMap: Record exists, executing UPDATE")));
	} else {
		// 记录不存在，执行插入
		strSQL.Format(_T("INSERT INTO ivs_lcd_idmap (MarkID, UniqueID, Barcode, MainAoiFixID) VALUES ('%s', '%s', '%s', '%d')"),
			strMarkID, strUniqueID, strBarcode, fixtureNo);
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("[DBG] UpdateLightingIdMap: Record does not exist, executing INSERT")));
	}
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
		_T("[DBG] UpdateLightingIdMap: SQL=%s"), strSQL));

	BOOL bRetry = FALSE;
	theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: Creating statement for UPDATE..."));
	try {
		// 按 MainAoiFixID 更新对应治具号记录，供检测软件使用
		ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
		if (!SQL_SUCCEEDED(ret)) {
			PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
			throw 1;
		}

		theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: Executing UPDATE statement..."));
		ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strSQL), SQL_NTS);
		if (SQL_SUCCEEDED(ret)) {
			SQLLEN affected = 0;
			ret = SQLRowCount(stmt, &affected);
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("[DBG] UpdateLightingIdMap: SQLRowCount returned affected=%d"), (int)affected));

			if (affected >= 0)
			{
				if (affected > 0)
				{
					theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
						_T("SUCCESS: Updated %d rows in ivs_lcd_idmap - MainAoiFixID=%d, UniqueID=%s, ScreenID=%s"),
						(int)affected, fixtureNo, uniqueID, screenID));
				}
				else
				{
					theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
						_T("WARNING: No rows affected when updating ivs_lcd_idmap (MainAoiFixID=%d). Check if record exists."), 
						fixtureNo));
				}
				SQLFreeHandle(SQL_HANDLE_STMT, stmt);
				return TRUE;
			}
			else
			{
				theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
					_T("FAILED: SQLRowCount returned error for ivs_lcd_idmap update (MainAoiFixID=%d)"), fixtureNo));
				SQLFreeHandle(SQL_HANDLE_STMT, stmt);
				return FALSE;
			}
		}
		else {
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("[DBG] UpdateLightingIdMap: SQLExecDirectA failed with SQL=%s"), strSQL));
			PrintOdbcError(stmt, SQL_HANDLE_STMT);
			throw 1;
		}
		SQLFreeHandle(SQL_HANDLE_STMT, stmt);
		stmt = SQL_NULL_HANDLE;
	}
	catch (...) {
		if (stmt != SQL_NULL_HANDLE) {
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
		}
		theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: SQL error, retrying..."));
		bRetry = TRUE;
	}

	if (bRetry) {
		theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: Starting retry logic with heartbeat check..."));

		// 使用心跳检测并自动重连
		if (!CheckAndReconnectTlsLighting(
			theApp.m_strLightingDBServer,
			theApp.m_strLightingDBName,
			theApp.m_strLightingDBUser,
			theApp.m_strLightingDBPassword,
			theApp.m_pLightingLog))
		{
			theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: TLS reconnection failed"));
			return FALSE;
		}
		pUseConn = GetTlsLightingConnPtr();
		theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap: TLS reconnected successfully"));

		try {
			// 重新连接后，确保 Barcode 列存在（兼容旧数据库）
			theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap Retry: Checking if Barcode column exists..."));
			CString strCheckSQL;
			strCheckSQL.Format(_T("SELECT COUNT(*) FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'ivs_lcd_idmap' AND COLUMN_NAME = 'Barcode'"));
			
			ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
				throw 1;
			}

			ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strCheckSQL), SQL_NTS);
			if (SQL_SUCCEEDED(ret)) {
				ret = SQLFetch(stmt);
				if (SQL_SUCCEEDED(ret)) {
					int count = 0;
					SQLGetData(stmt, 1, SQL_C_SLONG, &count, 0, NULL);
					if (count > 0) {
						theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap Retry: Barcode column already exists"));
					} else {
						theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap Retry: Adding Barcode column..."));
						SQLFreeHandle(SQL_HANDLE_STMT, stmt);
						stmt = SQL_NULL_HANDLE;

						ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
						if (!SQL_SUCCEEDED(ret)) {
							PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
							throw 1;
						}

						ret = SQLExecDirectA(stmt, (SQLCHAR*)"ALTER TABLE ivs_lcd_idmap ADD COLUMN Barcode VARCHAR(100)", SQL_NTS);
						if (SQL_SUCCEEDED(ret)) {
							theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap Retry: ALTER TABLE executed"));
						}
					}
				}
			}
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;

			// 先查询记录是否存在
			CString strCheckSQLRetry;
			strCheckSQLRetry.Format(_T("SELECT COUNT(*) FROM ivs_lcd_idmap WHERE MainAoiFixID='%d'"), fixtureNo);
			SQLHSTMT checkStmt = SQL_NULL_HANDLE;
			ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &checkStmt);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
				SQLFreeHandle(SQL_HANDLE_STMT, checkStmt);
				throw 1;
			}
			
			ret = SQLExecDirectA(checkStmt, (SQLCHAR*)(LPCSTR)CT2A(strCheckSQLRetry), SQL_NTS);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(checkStmt, SQL_HANDLE_STMT);
				SQLFreeHandle(SQL_HANDLE_STMT, checkStmt);
				throw 1;
			}
			
			ret = SQLFetch(checkStmt);
			int count = 0;
			if (SQL_SUCCEEDED(ret)) {
				SQLGetData(checkStmt, 1, SQL_C_SLONG, &count, 0, NULL);
			}
			SQLFreeHandle(SQL_HANDLE_STMT, checkStmt);
			checkStmt = SQL_NULL_HANDLE;
			
			CString strSQL;
			if (count > 0) {
				// 记录存在，执行更新
				strSQL.Format(_T("UPDATE ivs_lcd_idmap SET UniqueID='%s', Barcode='%s' WHERE MainAoiFixID='%d'"),
					strUniqueID, strBarcode, fixtureNo);
				theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("[DBG] UpdateLightingIdMap Retry: Record exists, executing UPDATE")));
			} else {
				// 记录不存在，执行插入
				strSQL.Format(_T("INSERT INTO ivs_lcd_idmap (MarkID, UniqueID, Barcode, MainAoiFixID) VALUES ('%s', '%s', '%s', '%d')"),
					strMarkID, strUniqueID, strBarcode, fixtureNo);
				theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(_T("[DBG] UpdateLightingIdMap Retry: Record does not exist, executing INSERT")));
			}

			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("[DBG] UpdateLightingIdMap Retry: SQL=%s"), strSQL));
			theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap Retry: Creating statement for UPDATE..."));

			ret = SQLAllocHandle(SQL_HANDLE_STMT, pUseConn, &stmt);
			if (!SQL_SUCCEEDED(ret)) {
				PrintOdbcError(pUseConn, SQL_HANDLE_DBC);
				throw 1;
			}

			theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap Retry: Executing UPDATE..."));
			ret = SQLExecDirectA(stmt, (SQLCHAR*)(LPCSTR)CT2A(strSQL), SQL_NTS);
			if (SQL_SUCCEEDED(ret)) {
				SQLLEN affected = 0;
				ret = SQLRowCount(stmt, &affected);
				theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
					_T("[DBG] UpdateLightingIdMap Retry: SQLRowCount returned affected=%d"), (int)affected));

				if (affected >= 0)
				{
					if (affected > 0)
					{
						theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
							_T("RETRY SUCCESS: Updated %d rows in ivs_lcd_idmap - MainAoiFixID=%d, UniqueID=%s, ScreenID=%s"),
							(int)affected, fixtureNo, uniqueID, screenID));
					}
					else
					{
						theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
							_T("RETRY WARNING: No rows affected (MainAoiFixID=%d). Record may not exist in table."), 
							fixtureNo));
					}
					SQLFreeHandle(SQL_HANDLE_STMT, stmt);
					return TRUE;
				}
			}
			SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			stmt = SQL_NULL_HANDLE;
		}
		catch (...) {
			if (stmt != SQL_NULL_HANDLE) {
				SQLFreeHandle(SQL_HANDLE_STMT, stmt);
			}
			theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap Retry: SQL error"));
		}
	}

	theApp.m_pLightingLog->LOG_INFO(_T("[DBG] UpdateLightingIdMap EXIT: returning FALSE"));
	return FALSE;
}