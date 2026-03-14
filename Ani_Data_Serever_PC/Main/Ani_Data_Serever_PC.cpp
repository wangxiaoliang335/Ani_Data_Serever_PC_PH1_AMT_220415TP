
// Ani_Data_Serever_PCApp.cpp
//

#include "stdafx.h"
#include <memory>
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Ani_Data_Serever_PC.h"
#include "MainFrm.h"

#include "Ani_Data_Serever_PCDoc.h"
#include "Ani_Data_Serever_PCView.h"
#include "MsgBox.h"
#include <locale.h>

// ODBC for MySQL Database

#ifdef _DEBUGUNLOADER_MAIN_TOP_VIEW
#define new DEBUG_NEW
#endif

// CAni_Data_Serever_PCApp

BEGIN_MESSAGE_MAP(CAni_Data_Serever_PCApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CAni_Data_Serever_PCApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CAni_Data_Serever_PCApp

CAni_Data_Serever_PCApp::CAni_Data_Serever_PCApp() :m_pEqIf(NULL), m_pComView(NULL), m_lastShiftIndex(99), m_bLightingCycleInProgress(FALSE), m_bLightingRunning(FALSE), m_bLightingSnapDone(FALSE), m_dwLightingStartTick(0), m_dwLightingTimeoutMs(60000), m_bLightingDBConnected(FALSE)
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

	// Initialize arrays
	memset(m_bLightingActiveSlot, 0, sizeof(m_bLightingActiveSlot));
	memset(m_LightingInspResult, 0, sizeof(m_LightingInspResult));
	m_pLightingConn = NULL;
#ifdef _MANAGED
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO
	// CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Ani_Data_Serever_PC.AppID.NoVersion"));


	// TODO: 
	// InitInstance
}

// CAni_Data_Serever_PCApp

CAni_Data_Serever_PCApp theApp;


// CAni_Data_Serever_PCApp

BOOL CAni_Data_Serever_PCApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	m_hApp = CreateMutex(NULL, FALSE, _T("Ani_Data_Serever_PC.exe"));

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AfxMessageBox(_T("Ani_Data_Serever_PC Program is already existed"));

		CloseHandle(m_hApp);
		return FALSE;
	}

	/*if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}*/

	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	SetRegistryKey(_T("ANI"));
	LoadStdProfileSettings(4);
	
	WSADATA data;
	::WSAStartup(MAKEWORD(2, 2), &data);

	MakeDefaultDir();

	EZIni ini(DATA_SYSTEM_PATH + _T("modelData.ini"));
	theApp.m_CurrentModel.m_AlignPcCurrentModelName = ini[_T("MODEL")][_T("LAST_MODEL")];

	theApp.GetSystemData();
	theApp.GetTactParameter();
	theApp.LoadSetTimer();
	theApp.AlarmDataLoad();
	theApp.IDCardReaderUserHistory();

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAni_Data_Serever_PCDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CAni_Data_Serever_PCView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	//indexNum
	m_indexList.resize(4);
	for (int ii = 0; ii < MaxZone; ii++)
	{
		switch (ii)
		{
		case AZone: m_indexList[ii].m_indexNum = 1, m_indexList[ii].m_indexProgramNum = 0; break;
		case BZone: m_indexList[ii].m_indexNum = 5, m_indexList[ii].m_indexProgramNum = 4; break;
		case CZone: m_indexList[ii].m_indexNum = 9, m_indexList[ii].m_indexProgramNum = 8; break;
		case DZone: m_indexList[ii].m_indexNum = 13, m_indexList[ii].m_indexProgramNum = 12; break;
		}
	}
	theApp.m_bExitFlag = TRUE;
	m_iUserClass = USER_OPERATOR;
	theApp.m_iTotalCompareCount = 0;

	CString strPath;

	strPath.Format(_T("%sPlcLog.log"), LOG_PLC_LOG_PATH);
	m_PlcLog = new CLogger(_T("PlcLog"), strPath, FALSE);
	theApp.m_PlcLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sPlcHeartBitLog.log"), LOG_PlcHeartBit_LOG_PATH);
	m_PlcHeartBitLog = new CLogger(_T("PlcHeartBitLog"), strPath, FALSE);
	theApp.m_PlcHeartBitLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sAlignLog.log"), LOG_ALIGN_LOG_PATH);
	m_AlignLog = new CLogger(_T("AlignLog"), strPath, FALSE);
	theApp.m_AlignLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	for (int ii = 0; ii < 10; ii++)
	{
		strPath.Format(_T("%s%s%dLog\\AlignSendReceiver%dLog.log"), LOG_PATH , _T("AlignSendReceiver"), ii + 1, ii + 1);
		m_pAlignSendReceiverLog[ii] = new CLogger(CStringSupport::FormatString(_T("AlignSendReceiver%dLog"), ii + 1), strPath, FALSE);
		theApp.m_pAlignSendReceiverLog[ii]->LOG_INFO(_T("************************ SYSTEM START ************************"));
	}

	strPath.Format(_T("%sTimeOutLog.log"), LOG_TIME_OUT_LOG_PATH);
	m_TimeOutLog = new CLogger(_T("TimeOutLog"), strPath, FALSE);
	theApp.m_TimeOutLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sPgLog.log"), LOG_PG_LOG_PATH);
	m_PgLog = new CLogger(_T("PgLog"), strPath, FALSE);
	theApp.m_PgLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sPgSendReceiverLog.log"), LOG_PG_SEND_RECEIVER_LOG_PATH);
	m_PgSendReceiverLog = new CLogger(_T("PgSendReceiverLog"), strPath, FALSE);
	theApp.m_PgSendReceiverLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sFtpLog.log"), LOG_FTP_LOG_PATH);
	m_pFTPLog = new CLogger(_T("FtpLog"), strPath, FALSE);
	theApp.m_pFTPLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sDataStatusLog.log"), LOG_DATA_STATUS_PATH);
	m_pDataStatusLog = new CLogger(_T("DataStatusLog"), strPath, FALSE);
	theApp.m_pDataStatusLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sTactTimeLog.log"), LOG_TACT_TIME_PATH);
	m_pTactTimeLog = new CLogger(_T("TactTimeLog"), strPath, FALSE);
	theApp.m_pTactTimeLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sTraceLog.log"), LOG_TRACE_PATH);
	m_pTraceLog = new CLogger(_T("TraceLog"), strPath, FALSE);
	theApp.m_pTraceLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sAxisLog.log"), LOG_AXIS_PATH);
	m_pAxisLog = new CLogger(_T("AxisLog"), strPath, FALSE);
	theApp.m_pAxisLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sOperateTimeLog.log"), LOG_OPERATE_TIME_PATH);
	m_pOperateTimeLog = new CLogger(_T("OperateTimeLog"), strPath, FALSE);
	theApp.m_pOperateTimeLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sTpLog.log"), LOG_TP_PATH);
	m_pTpLog = new CLogger(_T("TpLog"), strPath, FALSE);
	theApp.m_pTpLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sTpSendReceiverLog.log"), LOG_TP_SEND_RECIEVER_PATH);
	m_pTpSendReceiverLog = new CLogger(_T("TpSendReceiverLog"), strPath, FALSE);
	theApp.m_pTpSendReceiverLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sUserLoginOutLog.log"), LOG_USER_LOGIN_OUT_PATH);
	m_pUserLoginOutLog = new CLogger(_T("UserLoginOutLog"), strPath, FALSE);
	theApp.m_pUserLoginOutLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sUserLog.log"), LOG_USER_PATH);
	m_pUserLog = new CLogger(_T("UserLog"), strPath, FALSE);
	theApp.m_pUserLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sFFUSendReceiverLog.log"), LOG_FFU_SEND_RECIEVER_PATH);
	m_pFFUSendReceiverLog = new CLogger(_T("FFUSendReceiverLog"), strPath, FALSE);
	theApp.m_pFFUSendReceiverLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sSendDefectCodeLog.log"), LOG_SEND_DEFECT_CODE_PATH);
	m_pSendDefectCodeLog = new CLogger(_T("SendDefectCodeLog"), strPath, FALSE);
	theApp.m_pSendDefectCodeLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sARSSendReceiverLog.log"), LOG_ARS_SEND_RECIEVER_PATH);
	m_pARSSendReceiverLog = new CLogger(_T("ARSSendReceiverLog"), strPath, FALSE);
	theApp.m_pARSSendReceiverLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

#if _SYSTEM_AMTAFT_
	strPath.Format(_T("%sViewingAngleLog.log"), LOG_VIEWING_ANGLE_LOG_PATH);
	m_ViewingAngleLog = new CLogger(_T("ViewingAngleLog"), strPath, FALSE);
	theApp.m_ViewingAngleLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sVisionLog.log"), LOG_VISION_LOG_PATH);
	m_VisionLog = new CLogger(_T("VisionLog"), strPath, FALSE);
	theApp.m_VisionLog->LOG_INFO(_T("*****z******************* SYSTEM START ************************"));

	strPath.Format(_T("%sLumitopLog.log"), LOG_LUMITOP_LOG_PATH);
	m_LumitopLog = new CLogger(_T("LumitopLog"), strPath, FALSE);
	theApp.m_LumitopLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sVisionSendReceiver1Log.log"), LOG_VISION_SEND_RECIEVER_LOG_1);
	m_pVisionSendReceiver1Log = new CLogger(_T("VisionSendReceiver1Log"), strPath, FALSE);
	theApp.m_pVisionSendReceiver1Log->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sVisionSendReceiver2Log.log"), LOG_VISION_SEND_RECIEVER_LOG_2);
	m_pVisionSendReceiver2Log = new CLogger(_T("VisionSendReceiver2Log"), strPath, FALSE);
	theApp.m_pVisionSendReceiver2Log->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sViewingAngleSendReceiver1Log.log"), LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_1);
	m_pViewingAngleSendReceiver1Log = new CLogger(_T("ViewingAngleSendReceiver1Log"), strPath, FALSE);
	theApp.m_pViewingAngleSendReceiver1Log->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sViewingAngleSendReceiver2Log.log"), LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_2);
	m_pViewingAngleSendReceiver2Log = new CLogger(_T("ViewingAngleSendReceiver2Log"), strPath, FALSE);
	theApp.m_pViewingAngleSendReceiver2Log->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sViewingAngleSendReceiver3Log.log"), LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_3);
	m_pViewingAngleSendReceiver3Log = new CLogger(_T("ViewingAngleSendReceiver3Log"), strPath, FALSE);
	theApp.m_pViewingAngleSendReceiver3Log->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sViewingAngleSendReceiver4Log.log"), LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_4);
	m_pViewingAngleSendReceiver4Log = new CLogger(_T("ViewingAngleSendReceiver4Log"), strPath, FALSE);
	theApp.m_pViewingAngleSendReceiver4Log->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sLumitopSendReceiver1Log.log"), LOG_LUMITOP_SEND_RECIEVER_LOG_1);
	m_pLumitopSendReceiver1Log = new CLogger(_T("LumitopSendReceiver1Log"), strPath, FALSE);
	theApp.m_pLumitopSendReceiver1Log->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sLumitopSendReceiver2Log.log"), LOG_LUMITOP_SEND_RECIEVER_LOG_2);
	m_pLumitopSendReceiver2Log = new CLogger(_T("LumitopSendReceiver2Log"), strPath, FALSE);
	theApp.m_pLumitopSendReceiver2Log->LOG_INFO(_T("************************ SYSTEM START ************************"));
	
	strPath.Format(_T("%sOpvLog.log"), LOG_OPV_PATH);
	m_pOpvLog = new CLogger(_T("OpvLog"), strPath, FALSE);
	theApp.m_pOpvLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sOpvSendReceiver1Log.log"), LOG_OPV_SEND_RECIEVER1_PATH);
	m_pOpvSendReceiver1Log = new CLogger(_T("OpvSendReceiver1Log"), strPath, FALSE);
	theApp.m_pOpvSendReceiver1Log->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sOpvSendReceiver2Log.log"), LOG_OPV_SEND_RECIEVER2_PATH);
	m_pOpvSendReceiver2Log = new CLogger(_T("OpvSendReceiver2Log"), strPath, FALSE);
	theApp.m_pOpvSendReceiver2Log->LOG_INFO(_T("************************ SYSTEM START ************************"));

	strPath.Format(_T("%sLightingLog.log"), LOG_LIGHTING_PATH);
	m_pLightingLog = new CLogger(_T("LightingLog"), strPath, FALSE);
	theApp.m_pLightingLog->LOG_INFO(_T("************************ SYSTEM START ************************"));

	// 初始化 Lighting 数据库连接
	InitLightingDatabase();

	strPath.Format(_T("%sLightingSendReceiverLog.log"), LOG_LIGHTING_SEND_RECEIVER_PATH);
	m_pLightingSendReceiverLog = new CLogger(_T("LightingSendReceiverLog"), strPath, FALSE);
	theApp.m_pLightingSendReceiverLog->LOG_INFO(_T("************************ SYSTEM START ************************"));
#endif

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	CAni_Data_Serever_PCApp* pApp = (CAni_Data_Serever_PCApp*)::AfxGetApp();
	CMainFrame* pMainFrame = (CMainFrame*)pApp->GetMainWnd();
	theApp.m_pComView->ShowWindow(SW_HIDE);
	pMainFrame->m_pAddrView->ShowWindow(SW_HIDE);
#if _SYSTEM_AMTAFT_
	pMainFrame->m_pMainUnloaderView->ShowWindow(SW_HIDE);
#endif

	theApp.LanguageChange();
	m_pMainWnd->MoveWindow(0, 0, 1280, 1024);
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	theApp.m_bBCTestMode = TRUE;

	return TRUE;
}

int CAni_Data_Serever_PCApp::ExitInstance()
{
	//TODO:
	AfxOleTerm(FALSE);

	for (int ii = 0; ii < theApp.m_AlignThread.size(); ii++)
	{
		if (theApp.m_AlignThread[ii])
		{
			theApp.m_AlignThread[ii]->CloseTask();
			delete theApp.m_AlignThread[ii];
		}
		theApp.m_AlignThread[ii] = NULL;
	}

	for (int ii = 0; ii < theApp.m_AlignSocketManager.size(); ii++)
	{
		if (theApp.m_AlignSocketManager[ii])
		{
			theApp.m_AlignSocketManager[ii]->CloseComm();
			delete theApp.m_AlignSocketManager[ii];
		}
		theApp.m_AlignSocketManager[ii] = NULL;
	}

	if (theApp.m_FFUSerialCom)
	{
		theApp.m_FFUSerialCom->CloseTask();
		theApp.m_FFUSerialCom->ClosePort();
		delete theApp.m_FFUSerialCom;
	}
	theApp.m_FFUSerialCom = NULL;

#if _SYSTEM_AMTAFT_
	if (theApp.m_ViewingAngleThread){
		theApp.m_ViewingAngleThread->CloseTask();
		delete theApp.m_ViewingAngleThread;
	}
	theApp.m_ViewingAngleThread = NULL;

	if (theApp.m_VisionThread){
		theApp.m_VisionThread->CloseTask();
		delete theApp.m_VisionThread;
	}
	theApp.m_VisionThread = NULL;

	for (int ii = 0; ii < MaxZone; ii++)
	{
		if (theApp.m_PgInexThread[ii]){
			theApp.m_PgInexThread[ii]->CloseTask();
			delete theApp.m_PgInexThread[ii];
		}
		theApp.m_PgInexThread[ii] = NULL;
	}

	if (theApp.m_pRankTread)
	{
		theApp.m_pRankTread->CloseTask();
		delete theApp.m_pRankTread;
	}
	theApp.m_pRankTread = NULL;

	if (theApp.m_ManualThread){
		theApp.m_ManualThread->CloseTask();
		delete m_ManualThread;
	}
	theApp.m_ManualThread = NULL;

	if (theApp.m_TpThread){
		theApp.m_TpThread->CloseTask();
		delete theApp.m_TpThread;
	}
	theApp.m_TpThread = NULL;
#else
	for(int ii = 0; ii < MaxGammaStage; ii++)
	{
		if (theApp.m_GammaThread[ii]){
			theApp.m_GammaThread[ii]->CloseTask();
			delete m_GammaThread[ii];
		}
		theApp.m_GammaThread[ii] = NULL;
	}
#endif

	if (theApp.m_PlcThread){
		theApp.m_PlcThread->CloseTask();
		theApp.m_PlcThread->TactCloseTask();
		delete theApp.m_PlcThread;
	}
	theApp.m_PlcThread = NULL;

	if (theApp.m_AllPassModeThread){
		theApp.m_AllPassModeThread->CloseTask();
		delete theApp.m_AllPassModeThread;
	}
	theApp.m_AllPassModeThread = NULL;

	if (theApp.m_pFTP){
#if _SYSTEM_AMTAFT_
		theApp.m_pFTP->CloseTask();
#endif
		theApp.m_pFTP->CloseDfsTask();
		delete theApp.m_pFTP;
	}
	theApp.m_pFTP = NULL;

	if (theApp.m_pFS){
		theApp.m_pFS->CloseTask();
		delete theApp.m_pFS;
	}
	theApp.m_pFS = NULL;

	if (theApp.m_pEqIf){
		theApp.m_pEqIf->CloseEqIf();
		delete theApp.m_pEqIf;
	}
	theApp.m_pEqIf = NULL;

	for (int ii = 0; ii < 10; ii++)
	{
		if (m_pAlignSendReceiverLog[ii] != NULL)
			delete m_pAlignSendReceiverLog[ii];
	}
	if (m_PlcLog != NULL)
		delete m_PlcLog;
	if (m_ViewingAngleLog != NULL)
		delete m_ViewingAngleLog;
	if (m_VisionLog != NULL)
		delete m_VisionLog;
	if (m_LumitopLog != NULL)
		delete m_LumitopLog;
	if (m_PlcHeartBitLog != NULL)
		delete m_PlcHeartBitLog;
	if (m_AlignLog != NULL)
		delete m_AlignLog;
	if (m_TimeOutLog != NULL)
		delete m_TimeOutLog;
	if (m_PgLog != NULL)
		delete m_PgLog;
	if (m_PgSendReceiverLog != NULL)
		delete m_PgSendReceiverLog;
	if (m_pFTPLog != NULL)
		delete m_pFTPLog;
	if (m_pVisionSendReceiver1Log != NULL)
		delete m_pVisionSendReceiver1Log;
	if (m_pVisionSendReceiver2Log != NULL)
		delete m_pVisionSendReceiver2Log;
	if (m_pViewingAngleSendReceiver1Log != NULL)
		delete m_pViewingAngleSendReceiver1Log;
	if (m_pViewingAngleSendReceiver2Log != NULL)
		delete m_pViewingAngleSendReceiver2Log;
	if (m_pViewingAngleSendReceiver3Log != NULL)
		delete m_pViewingAngleSendReceiver3Log;
	if (m_pViewingAngleSendReceiver4Log != NULL)
		delete m_pViewingAngleSendReceiver4Log;
	if (m_pDataStatusLog != NULL)
		delete m_pDataStatusLog;
	if (m_pTactTimeLog != NULL)
		delete m_pTactTimeLog;
	if (m_pTraceLog != NULL)
		delete m_pTraceLog;
	if (m_pAxisLog != NULL)
		delete m_pAxisLog;
	if (m_pOperateTimeLog != NULL)
		delete m_pOperateTimeLog;
	if (m_pTpLog != NULL)
		delete m_pTpLog;
	if (m_pTpSendReceiverLog != NULL)
		delete m_pTpSendReceiverLog;
	if (m_pOpvLog != NULL)
		delete m_pOpvLog;
	if (m_pOpvSendReceiver1Log != NULL)
		delete m_pOpvSendReceiver1Log;
	if (m_pOpvSendReceiver2Log != NULL)
		delete m_pOpvSendReceiver2Log;
	if (m_pUserLoginOutLog != NULL)
		delete m_pUserLoginOutLog;
	if (m_pUserLog != NULL)
		delete m_pUserLog;
	if (m_pFFUSendReceiverLog != NULL)
		delete m_pFFUSendReceiverLog;
	if (m_pSendDefectCodeLog != NULL)
		delete m_pSendDefectCodeLog;
	if (m_pARSSendReceiverLog != NULL)
		delete m_pARSSendReceiverLog;

	if (m_pMsgBox != NULL)
		delete m_pMsgBox;

	if (m_pMsgBoxAlarm != NULL)
		delete m_pMsgBoxAlarm;

	// 关闭 Lighting 数据库连接
	CloseLightingDatabase();

	return CWinApp::ExitInstance();
}

LightingInspectionResult CAni_Data_Serever_PCApp::QueryInspectionResult(CString uniqueID)
{
	LightingInspectionResult result;
	result.m_bValid = FALSE;

	if (!m_bLightingDBConnected)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pLightingLog->LOG_INFO(_T("QueryInspectionResult: Database not connected"));
			return result;
		}
	}

	try {
		CString strSQL;
		strSQL.Format(_T("SELECT GUID, ScreenID, AOIResult, Code_AOI, Grade_AOI, StartTime, StopTime FROM IVS_LCD_InspectionResult WHERE UniqueID = '%s'"), uniqueID);

		std::auto_ptr<sql::Statement> stmt(m_pLightingConn->createStatement());
		std::auto_ptr<sql::ResultSet> res(stmt->executeQuery((std::string)CT2A(strSQL)));

		if (res->next())
		{
			result.m_strGUID = res->getString("GUID").c_str();
			result.m_strScreenID = res->getString("ScreenID").c_str();
			result.m_strUniqueID = uniqueID;
			result.m_strAOIResult = res->getString("AOIResult").c_str();
			result.m_strCodeAOI = res->getString("Code_AOI").c_str();
			result.m_strGradeAOI = res->getString("Grade_AOI").c_str();
			result.m_strStartTime = res->getString("StartTime").c_str();
			result.m_strStopTime = res->getString("StopTime").c_str();
			result.m_bValid = TRUE;

			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("QueryInspectionResult: Found result for UniqueID=%s, AOIResult=%s, Code=%s, Grade=%s"),
				uniqueID, result.m_strAOIResult, result.m_strCodeAOI, result.m_strGradeAOI));
		}
		else
		{
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("QueryInspectionResult: No result found for UniqueID=%s"), uniqueID));
		}
	}
	catch (sql::SQLException& e) {
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
			_T("QueryInspectionResult: SQL error: %s"), CString(e.what())));
	}

	return result;
}

void CAni_Data_Serever_PCApp::MakeDefaultDir()
{
	CString strPath;
	//Log	
	strPath = LOG_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_PLC_LOG_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_ALIGN_LOG_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_TIME_OUT_LOG_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_PG_LOG_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_PG_SEND_RECEIVER_LOG_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_FTP_LOG_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_DATA_STATUS_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_AXIS_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_OPERATE_TIME_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_TP_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_TP_SEND_RECIEVER_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_USER_LOGIN_OUT_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_USER_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_USER_HISTORY_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_FFU_SEND_RECIEVER_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_ARS_SEND_RECIEVER_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_SEND_DEFECT_CODE_PATH;

#if _SYSTEM_AMTAFT_
	strPath = LOG_VIEWING_ANGLE_LOG_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_VISION_LOG_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_LUMITOP_LOG_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_VISION_SEND_RECIEVER_LOG_1;
	CreateDirectory(strPath, NULL);
	strPath = LOG_VISION_SEND_RECIEVER_LOG_2;
	CreateDirectory(strPath, NULL);
	strPath = LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_1;
	CreateDirectory(strPath, NULL);
	strPath = LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_2;
	CreateDirectory(strPath, NULL);
	strPath = LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_3;
	CreateDirectory(strPath, NULL);
	strPath = LOG_VIEWING_ANGLE_SEND_RECIEVER_LOG_4;
	CreateDirectory(strPath, NULL);
	strPath = LOG_LUMITOP_SEND_RECIEVER_LOG_1;
	CreateDirectory(strPath, NULL);
	strPath = LOG_LUMITOP_SEND_RECIEVER_LOG_2;
	CreateDirectory(strPath, NULL);
	for (int ii = 0; ii < NameCount; ii++)
	{
		strPath = DATA_INSPECT_RESULT_CODE_PATH + InspectName[ii];
		CreateDirectory(strPath, NULL);
	}

	strPath = LOG_OPV_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_OPV_SEND_RECIEVER1_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_OPV_SEND_RECIEVER2_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_LIGHTING_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_LIGHTING_SEND_RECEIVER_PATH;
	CreateDirectory(strPath, NULL);

	strPath = DATA_INSPECT_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_INSPECT_PATHTIME;
	CreateDirectory(strPath, NULL);
	strPath = DATA_INSPECT_CSV_PATH;
	CreateDirectory(strPath, NULL);
	strPath = AOI_DATA_INSPECT_PATH;
	CreateDirectory(strPath, NULL);
	strPath = AOI_DATA_INSPECT_PATHTIME;
	CreateDirectory(strPath, NULL);
	strPath = AOI_DATA_INSPECT_CSV_PATH;
	CreateDirectory(strPath, NULL);
	strPath = AOI_DATA_ALIGN_PATH;
	CreateDirectory(strPath, NULL);
	strPath = AOI_DATA_ALIGN_PATHTIME;
	CreateDirectory(strPath, NULL);
	strPath = ULD_DATA_INSPECT_PATH;
	CreateDirectory(strPath, NULL);
	strPath = ULD_DATA_INSPECT_PATHTIME;
	CreateDirectory(strPath, NULL);
	strPath = ULD_DATA_INSPECT_CSV_PATH;
	CreateDirectory(strPath, NULL);
	strPath = ULD_DATA_ALIGN_PATH;
	CreateDirectory(strPath, NULL);
	strPath = ULD_DATA_ALIGN_PATHTIME;
	CreateDirectory(strPath, NULL);

	strPath = DATA_TP_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_TP_PATHTIME;
	CreateDirectory(strPath, NULL);
	strPath = DATA_OPV_DEFECT_LIST_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_OPV_DEFECT_CODE_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_OPV_SUM_DEFECT_CODE_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_OPV_DEFECT_HISTORY_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_PREGAMMA_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_PREGAMMA_PATHTIME;
	CreateDirectory(strPath, NULL);
#else
	strPath = DATA_MTP_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_MTP_PATHTIME;
	CreateDirectory(strPath, NULL);
	strPath = DATA_GAMMA_DFS_INFO_PATH;
	CreateDirectory(strPath, NULL);
#endif

	//System
	strPath = DATA_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_SYSTEM_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_ALIGN_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_INSPECT_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_ALARM_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_ALARM_HISTORY_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_ALARM_COUNT_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_INSPECT_CSV_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_CONTACT_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_TACT_TIME_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_TACT_TIME_PATH;
	CreateDirectory(strPath, NULL);
	strPath = LOG_TRACE_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_ALIGN_PATHTIME;
	CreateDirectory(strPath, NULL);
	strPath = DATA_CONTACT_PATHTIME;
	CreateDirectory(strPath, NULL);
	strPath = DATA_TACT_TIME_UNIT_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_INSPECT_LOGIN_OUT_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_DEFECT_CODE_PATH;
	CreateDirectory(strPath, NULL);
	strPath = DATA_TOTAL_CHECK_PATH;
	CreateDirectory(strPath, NULL);
}

void CAni_Data_Serever_PCApp::LanguageChange()
{
	CAni_Data_Serever_PCApp* pApp = (CAni_Data_Serever_PCApp*)::AfxGetApp();
	CMainFrame* pMainFrame = (CMainFrame*)pApp->GetMainWnd();
	pMainFrame->m_cViewCtrl.StringChanged();
	theApp.m_pComView->StringChanged();
	pMainFrame->m_pAddrView->StringChanged();
	g_MainLog->StringChanged();

#if _SYSTEM_AMTAFT_
	g_DlgMainView->StringChanged();
#endif
}

// CAni_Data_Serever_PCApp


// CAboutDlg

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	// ±¸ÇöÀÔ´Ï´Ù.
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 
void CAni_Data_Serever_PCApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CAni_Data_Serever_PCApp


void CAboutDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

void CAni_Data_Serever_PCApp::getMsgBox(int style, CString strKorMsg, CString strEngMsg, CString strChoMsg)
{
	CString msg;

	switch (m_iLanguageSelect)
	{
	case KOR: msg = strKorMsg; break;
	case ENG: msg = strEngMsg; break;
	case CHI: msg = strChoMsg; break;
	}

	CMsgBox dlg(style, msg);
	dlg.DoModal();
}

void CAni_Data_Serever_PCApp::getMsgBox2(int style, CString strKorMsg, CString strEngMsg, CString strChoMsg)
{
	CString msg;

	switch (m_iLanguageSelect)
	{
	case KOR: msg = strKorMsg; break;
	case ENG: msg = strEngMsg; break;
	case CHI: msg = strChoMsg; break;
	}

	CMsgBox dlg(style, msg);
	//dlg.DoModal();
	dlg.ShowWindow(SW_SHOW);
}

int CAni_Data_Serever_PCApp::YesNoMsgBox(int style, CString strKorMsg, CString strEngMsg, CString strChoMsg)
{
	CString msg;

	switch (m_iLanguageSelect)
	{
	case KOR: msg = strKorMsg; break;
	case ENG: msg = strEngMsg; break;
	case CHI: msg = strChoMsg; break;
	}

	CMsgBox dlg(style, msg);
	int rval = dlg.DoModal();
	return rval;
}

void CAni_Data_Serever_PCApp::ModelCheck(BOOL ModelCreateChangeFlag, CBtnEnh *ModelParm)
{
	if (ModelCreateChangeFlag == TRUE)
	{
		if (ModelParm->GetBackColorInterior() != TRED)
		{
			ModelParm->SetBackColorInterior(TRED);
			ModelParm->SetBackColorMouseOver(TRED);
		}
	}
	else
	{
		if (ModelParm->GetBackColorInterior() != TGREEN)
		{
			ModelParm->SetBackColorInterior(TGREEN);
			ModelParm->SetBackColorMouseOver(TGREEN);
		}
	}
}

void CAni_Data_Serever_PCApp::LoadSetTimer()
{
	CString strTitle, strValue;
	EZIni ini(DATA_SYSTEM_DATA_PATH);
	CStringArray responseTokens;
	std::vector<CString> listOfKeyNames;

	int ii = 0;
	ini[_T("TIMER")].EnumKeyNames(listOfKeyNames);

	for (auto list = listOfKeyNames.begin(); list != listOfKeyNames.end(); ++list)
	{
		theApp.m_iTimer[ii] = ini[_T("TIMER")][*list];
		ii++;
	}

	theApp.m_iShiftTime[Shift_Start] = ini[_T("SHIFT")][_T("DY")];
	theApp.m_iShiftTime[Shift_End] = ini[_T("SHIFT")][_T("NT")];

	theApp.m_iDataResetTime[Shift_DY][Shift_Start] = ini[_T("DY_RESET_TIME")][_T("START")];
	theApp.m_iDataResetTime[Shift_DY][Shift_End] = ini[_T("DY_RESET_TIME")][_T("END")];

	theApp.m_iDataResetTime[Shift_NT][Shift_Start] = ini[_T("NT_RESET_TIME")][_T("START")];
	theApp.m_iDataResetTime[Shift_NT][Shift_End] = ini[_T("NT_RESET_TIME")][_T("END")];

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strTitle.Format(_T("%d"), ii);
		strValue = ini[_T("SHIFT_TIME_DY")][strTitle];
		responseTokens.RemoveAll();
		CStringSupport::GetTokenArray(strValue, _T('^'), responseTokens);

		if (responseTokens.GetSize() > 1)
		{
			theApp.m_stuTimeInspect[Shift_DY][ii].m_iTimeNum = ii;
			theApp.m_stuTimeInspect[Shift_DY][ii].m_iShiftTimeStart = _ttoi(responseTokens[0]);
			theApp.m_stuTimeInspect[Shift_DY][ii].m_iShiftTimeEnd = _ttoi(responseTokens[1]);
		}

		strValue = ini[_T("SHIFT_TIME_NT")][strTitle];
		responseTokens.RemoveAll();
		CStringSupport::GetTokenArray(strValue, _T('^'), responseTokens);

		if (responseTokens.GetSize() > 1)
		{
			theApp.m_stuTimeInspect[Shift_NT][ii].m_iTimeNum = ii;
			theApp.m_stuTimeInspect[Shift_NT][ii].m_iShiftTimeStart = _ttoi(responseTokens[0]);
			theApp.m_stuTimeInspect[Shift_NT][ii].m_iShiftTimeEnd = _ttoi(responseTokens[1]);
		}
	}
}
void CAni_Data_Serever_PCApp::SaveSetTimer()
{
	CString strTitle, strValue,strMsg;
	int iShiftStart, iShiftEnd;
	EZIni ini(DATA_SYSTEM_DATA_PATH);

	for (int ii = 0; ii < MaxTimerCount; ii++)
	{
		strMsg.Format(_T("%d"), ii);
		ini[_T("TIMER")][strMsg] = theApp.m_iTimer[ii];
	}
	
	ini[_T("SHIFT")][_T("DY")] = theApp.m_iShiftTime[Shift_Start];
	ini[_T("SHIFT")][_T("NT")] = theApp.m_iShiftTime[Shift_End];

	theApp.m_iDataResetTime[Shift_DY][Shift_Start] = theApp.m_iShiftTime[Shift_Start] - 2;
	theApp.m_iDataResetTime[Shift_DY][Shift_End] = theApp.m_iShiftTime[Shift_Start] - 1;

	ini[_T("DY_RESET_TIME")][_T("START")] = theApp.m_iDataResetTime[Shift_DY][Shift_Start];
	ini[_T("DY_RESET_TIME")][_T("END")] = theApp.m_iDataResetTime[Shift_DY][Shift_End];

	theApp.m_iDataResetTime[Shift_NT][Shift_Start] = theApp.m_iShiftTime[Shift_End] - 2;
	theApp.m_iDataResetTime[Shift_NT][Shift_End] = theApp.m_iShiftTime[Shift_End] - 1;

	ini[_T("NT_RESET_TIME")][_T("START")] = theApp.m_iDataResetTime[Shift_NT][Shift_Start];
	ini[_T("NT_RESET_TIME")][_T("END")] = theApp.m_iDataResetTime[Shift_NT][Shift_End];

	iShiftStart = theApp.m_iShiftTime[Shift_Start];
	iShiftEnd = theApp.m_iShiftTime[Shift_Start];
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		iShiftEnd += 100;
		strTitle.Format(_T("%d"), ii);
		strValue.Format(_T("%d^%d"), iShiftStart, iShiftEnd);
		ini[_T("SHIFT_TIME_DY")][strTitle] = strValue;
		theApp.m_stuTimeInspect[Shift_DY][ii].m_iTimeNum = ii;
		theApp.m_stuTimeInspect[Shift_DY][ii].m_iShiftTimeStart = iShiftStart;
		theApp.m_stuTimeInspect[Shift_DY][ii].m_iShiftTimeEnd = iShiftEnd;
		iShiftStart = iShiftEnd;
	}

	iShiftStart = theApp.m_iShiftTime[Shift_End];
	iShiftEnd = theApp.m_iShiftTime[Shift_End];
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		iShiftEnd += 100;

		if (iShiftStart > 2300)
		{
			//
			strMsg.Format(_T("%d"), iShiftStart);
			iShiftEnd = _ttoi(strMsg.Right(2));
		}
		strTitle.Format(_T("%d"), ii);
		strValue.Format(_T("%d^%d"), iShiftStart, iShiftEnd);
		theApp.m_stuTimeInspect[Shift_NT][ii].m_iTimeNum = ii;
		theApp.m_stuTimeInspect[Shift_NT][ii].m_iShiftTimeStart = iShiftStart;
		theApp.m_stuTimeInspect[Shift_NT][ii].m_iShiftTimeEnd = iShiftEnd;
		ini[_T("SHIFT_TIME_NT")][strTitle] = strValue;
		iShiftStart = iShiftEnd;
	}

}

int CAni_Data_Serever_PCApp::GetShift(int nTime)
{
	if (nTime >= theApp.m_iShiftTime[Shift_Start] && nTime < theApp.m_iShiftTime[Shift_End])
		return Shift_DY;
		
	return Shift_NT;
}

void CAni_Data_Serever_PCApp::GetShiftTime(int nTime, int nShiftTime)
{
	CString strShift;
	int nShiftEndTime;

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		if (theApp.m_stuTimeInspect[theApp.m_lastShiftIndex][ii].m_iShiftTimeEnd >= 0 &&
			theApp.m_stuTimeInspect[theApp.m_lastShiftIndex][ii].m_iShiftTimeEnd < 100)
		{
			strShift.Format(_T("24%d"), theApp.m_stuTimeInspect[theApp.m_lastShiftIndex][ii].m_iShiftTimeEnd);
			nShiftEndTime = _ttoi(strShift);
			if (nTime < 2300)
				nTime += 2400;
		}
		else
		{
			if (nTime >= 2400)
				nTime -= 2400;

			nShiftEndTime = theApp.m_stuTimeInspect[theApp.m_lastShiftIndex][ii].m_iShiftTimeEnd;
		}

		if (nTime >= theApp.m_stuTimeInspect[theApp.m_lastShiftIndex][ii].m_iShiftTimeStart && nTime < nShiftEndTime)
		{
			theApp.m_iTimeInspectNum = theApp.m_stuTimeInspect[theApp.m_lastShiftIndex][ii].m_iTimeNum;
			break;
		}
	}
}

void CAni_Data_Serever_PCApp::SetSaveResultCode(CString strPanelID, CString strFpcID, CString strTypeName, PLCSendDefect defectinfo, int iType)
{
	CString strPath, strFilePath,strCodeCount, strShift, strCodeGrade;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	if (iType == Machine_AOI)
		strPath.Format(_T("%s\\%s\\%s_%s"), DATA_DEFECT_CODE_PATH, _T("AOI"), theApp.m_strCurrentToday, strShift);
	else
		strPath.Format(_T("%s\\%s\\%s_%s"), DATA_DEFECT_CODE_PATH, _T("ULD"), theApp.m_strCurrentToday, strShift);

	CreateFolders(strPath);
	strFilePath.Format(_T("%s\\%s.ini"), strPath, strFpcID);
	EZIni ini(strFilePath);

	strCodeCount.Format(_T("%d"), defectinfo.m_iCount);
	strCodeGrade.Format(_T("%s^%s"), defectinfo.m_strCode, defectinfo.m_strGrade);

	ini[strTypeName][strCodeCount] = strCodeGrade;
}

void CAni_Data_Serever_PCApp::SetLoadResultCode(CString strPanelID, CString strFpcID)
{
	CString strFilePath, strShift, strCode, strGrade, strCodeGrade;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strFilePath.Format(_T("%s\\%s\\%s_%s\\%s.ini"), DATA_DEFECT_CODE_PATH, _T("AOI"), theApp.m_strCurrentToday, strShift, strFpcID);
	EZIni ini(strFilePath);

	std::vector<CString> listOfKeyNames;

	ini["AOI"].EnumKeyNames(listOfKeyNames);
	for (auto code : listOfKeyNames)
	{
		strCodeGrade = ini[_T("AOI")][code];
		CStringArray responseTokens;
		CStringSupport::GetTokenArray(strCodeGrade, _T('^'), responseTokens);

		strCode = responseTokens[0];
		strCodeGrade = responseTokens[1];

		m_Send_Result_Code_Map.insert(make_pair(strCode, strCodeGrade));

		responseTokens.RemoveAll();
		strCodeGrade = strCode = strGrade = _T("");
	}
	listOfKeyNames.clear();

	ini["Viewing"].EnumKeyNames(listOfKeyNames);
	for (auto code : listOfKeyNames)
	{
		strCodeGrade = ini[_T("Viewing")][code];
		CStringArray responseTokens;
		CStringSupport::GetTokenArray(strCodeGrade, _T('^'), responseTokens);

		strCode = responseTokens[0];
		strCodeGrade = responseTokens[1];

		m_Send_Result_Code_Map.insert(make_pair(strCode, strCodeGrade));

		responseTokens.RemoveAll();
		strCodeGrade = strCode = strGrade = _T("");
	}
}

CString CAni_Data_Serever_PCApp::SetTotalLoadResultCode(CString strPanelID, CString strFpcID, int iTypeNum)
{
	CString strFilePath, strShift, strCodeGrade;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");

	if (iTypeNum == Machine_AOI)
		strFilePath.Format(_T("%s\\%s\\%s_%s\\%s.ini"), DATA_DEFECT_CODE_PATH, _T("AOI"), theApp.m_strCurrentToday, strShift, strFpcID);
	else
		strFilePath.Format(_T("%s\\%s\\%s_%s\\%s.ini"), DATA_DEFECT_CODE_PATH, _T("ULD"), theApp.m_strCurrentToday, strShift, strFpcID);

	EZIni ini(strFilePath);

	if (theApp.m_iNumberSendToPlc > 0)
	{
		for (int ii = 1; ii < theApp.m_iNumberSendToPlc + 1; ii++)
			strCodeGrade = ini[_T("TotalDefectCode")][CStringSupport::FormatString(_T("%d"), ii)];
	}

	return strCodeGrade;
}

void CAni_Data_Serever_PCApp::InspDataShiftReset(int iShiftTime)
{
#if _SYSTEM_AMTAFT_
	for (int ii = 0; ii < MaxZone; ii++)
	{
		theApp.m_shiftProduction[ii].Reset(iShiftTime);
		theApp.m_UiShiftProduction[ii].Reset(iShiftTime);

		if (ii < ChMaxCount)
		{
			theApp.m_ULDshiftProduction[ii].Reset(iShiftTime);
			theApp.m_ULDUiShiftProduction[ii].Reset(iShiftTime);
		}
	}
#else
	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		theApp.m_shiftProduction[ii].Reset(iShiftTime);
		theApp.m_UiShiftProduction[ii].Reset(iShiftTime);
	}
#endif

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		theApp.m_shift_TimeProduction[ii].Reset(iShiftTime);
		theApp.m_UiShift_TimeProduction[ii].Reset(iShiftTime);
#if _SYSTEM_AMTAFT_
		theApp.m_ULDshift_TimeProduction[ii].Reset(iShiftTime);
		theApp.m_ULDUiShift_TimeProduction[ii].Reset(iShiftTime);
#endif
	}
}

#if _SYSTEM_AMTAFT_
void CAni_Data_Serever_PCApp::AOIInspectionDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(AOI_DATA_INSPECT_PATH + strTemp);

	for (int ii = 0; ii < MaxZone; ii++)
	{
		strTemp.Format(_T("VISION_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_VisionResult[nShift];

		strTemp.Format(_T("VIEWING_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_ViewingResult[nShift];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("TP_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_TpResult[nShift];

		strTemp.Format(_T("PREGAMMA_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_PreGammaResult[nShift];

		strTemp.Format(_T("GOOD_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("A_GRADE_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_GoodAGradeResult[nShift];

		strTemp.Format(_T("B_GRADE_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_GoodBGradeResult[nShift];

		strTemp.Format(_T("C_GRADE_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_GoodCGradeResult[nShift];

		strTemp.Format(_T("BAD_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("UNKNOW_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_UnKnowResult[nShift];

		strTemp.Format(_T("TRAY_INSERT_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_TrayInsertstatus[nShift];

		strTemp.Format(_T("LOWER_OUT_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_LowerDataOut[nShift];

		strTemp.Format(_T("TRAY_OUT_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_TrayDataOut[nShift];

		//<<UI DATA
		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("UI_VISION_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_VisionResult[nShift];

		strTemp.Format(_T("UI_VIEWING_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_ViewingResult[nShift];

		strTemp.Format(_T("UI_1ST_CONTACT_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("UI_2ND_CONTACT_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("UI_TP_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_TpResult[nShift];

		strTemp.Format(_T("UI_PREGAMMA_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_PreGammaResult[nShift];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("UI_A_GRADE_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_GoodAGradeResult[nShift];

		strTemp.Format(_T("UI_B_GRADE_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_GoodBGradeResult[nShift];

		strTemp.Format(_T("UI_C_GRADE_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_GoodCGradeResult[nShift];

		strTemp.Format(_T("UI_BAD_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("UI_UNKNOW_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_UnKnowResult[nShift];

		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("TRAY_INSERT_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_TrayInsertstatus[nShift];

		strTemp.Format(_T("LOWER_OUT_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_LowerDataOut[nShift];

		strTemp.Format(_T("TRAY_OUT_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_TrayDataOut[nShift];

	}
}

void CAni_Data_Serever_PCApp::AOIInspectionDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(AOI_DATA_INSPECT_PATH + strTemp);

	for (int ii = 0; ii < MaxZone; ii++)
	{
		strTemp.Format(_T("VISION_%d"), ii);
		m_shiftProduction[ii].m_VisionResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("VIEWING_%d"), ii);
		m_shiftProduction[ii].m_ViewingResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		m_shiftProduction[ii].m_FirstContactResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		m_shiftProduction[ii].m_ContactResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("TP_%d"), ii);
		m_shiftProduction[ii].m_TpResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("PREGAMMA_%d"), ii);
		m_shiftProduction[ii].m_PreGammaResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("GOOD_%d"), ii);
		m_shiftProduction[ii].m_GoodResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("A_GRADE_%d"), ii);
		m_shiftProduction[ii].m_GoodAGradeResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("B_GRADE_%d"), ii);
		m_shiftProduction[ii].m_GoodBGradeResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("C_GRADE_%d"), ii);
		m_shiftProduction[ii].m_GoodCGradeResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("BAD_%d"), ii);
		m_shiftProduction[ii].m_BadResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("UNKNOW_%d"), ii);
		m_shiftProduction[ii].m_UnKnowResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("TOTAL_%d"), ii);
		m_shiftProduction[ii].m_InspectionTotal[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("TRAY_INSERT_%d"), ii);
		m_shiftProduction[ii].m_TrayInsertstatus[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("LOWER_OUT_%d"), ii);
		m_shiftProduction[ii].m_LowerDataOut[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("TRAY_OUT_%d"), ii);
		m_shiftProduction[ii].m_TrayDataOut[nShift] = ini[PG_IndexName[ii]][strTemp];

		//<<UI
		strTemp.Format(_T("UI_VISION_%d"), ii);
		m_UiShiftProduction[ii].m_VisionResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_VIEWING_%d"), ii);
		m_UiShiftProduction[ii].m_ViewingResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_1ST_CONTACT_%d"), ii);
		m_UiShiftProduction[ii].m_FirstContactResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_2ND_CONTACT_%d"), ii);
		m_UiShiftProduction[ii].m_ContactResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_TP_%d"), ii);
		m_UiShiftProduction[ii].m_TpResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_PREGAMMA_%d"), ii);
		m_UiShiftProduction[ii].m_PreGammaResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		m_UiShiftProduction[ii].m_GoodResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_A_GRADE_%d"), ii);
		m_UiShiftProduction[ii].m_GoodAGradeResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_B_GRADE_%d"), ii);
		m_UiShiftProduction[ii].m_GoodBGradeResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_C_GRADE_%d"), ii);
		m_UiShiftProduction[ii].m_GoodCGradeResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_BAD_%d"), ii);
		m_UiShiftProduction[ii].m_BadResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_UNKNOW_%d"), ii);
		m_UiShiftProduction[ii].m_UnKnowResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		m_UiShiftProduction[ii].m_InspectionTotal[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("TRAY_INSERT_%d"), ii);
		m_UiShiftProduction[ii].m_TrayInsertstatus[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("LOWER_OUT_%d"), ii);
		m_UiShiftProduction[ii].m_LowerDataOut[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("TRAY_OUT_%d"), ii);
		m_UiShiftProduction[ii].m_TrayDataOut[nShift] = ini[PG_UIIndexName[ii]][strTemp];
	}
}

void CAni_Data_Serever_PCApp::AOIInspectionTimeDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(AOI_DATA_INSPECT_PATHTIME + strTemp);

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s") , GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		strTemp.Format(_T("VISION_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_VisionResult[nShift];

		strTemp.Format(_T("VIEWING_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_ViewingResult[nShift];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("TP_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_TpResult[nShift];

		strTemp.Format(_T("PREGAMMA_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_PreGammaResult[nShift];

		strTemp.Format(_T("GOOD_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("BAD_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("UNKNOW_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_UnKnowResult[nShift];

		//<<UI DATA
		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("UI_VISION_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_VisionResult[nShift];

		strTemp.Format(_T("UI_VIEWING_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_ViewingResult[nShift];

		strTemp.Format(_T("UI_1ST_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("UI_2ND_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("UI_TP_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_TpResult[nShift];

		strTemp.Format(_T("UI_PREGAMMA_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_PreGammaResult[nShift];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("UI_BAD_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("UI_UNKNOW_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_UnKnowResult[nShift];

		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_InspectionTotal[nShift];

	}
}

void CAni_Data_Serever_PCApp::AOIInspectionTimeDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(AOI_DATA_INSPECT_PATHTIME + strTemp);

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("VISION_%d"), ii);
		m_shift_TimeProduction[ii].m_VisionResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("VIEWING_%d"), ii);
		m_shift_TimeProduction[ii].m_ViewingResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		m_shift_TimeProduction[ii].m_FirstContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		m_shift_TimeProduction[ii].m_ContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("TP_%d"), ii);
		m_shift_TimeProduction[ii].m_TpResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("PREGAMMA_%d"), ii);
		m_shift_TimeProduction[ii].m_PreGammaResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("GOOD_%d"), ii);
		m_shift_TimeProduction[ii].m_GoodResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("BAD_%d"), ii);
		m_shift_TimeProduction[ii].m_BadResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UNKNOW_%d"), ii);
		m_shift_TimeProduction[ii].m_UnKnowResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("TOTAL_%d"), ii);
		m_shift_TimeProduction[ii].m_InspectionTotal[nShift] = ini[strShiftTime][strTemp];

		//<<UI
		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("UI_VISION_%d"), ii);
		m_UiShift_TimeProduction[ii].m_VisionResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_VIEWING_%d"), ii);
		m_UiShift_TimeProduction[ii].m_ViewingResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_1ST_CONTACT_%d"), ii);
		m_UiShift_TimeProduction[ii].m_FirstContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_2ND_CONTACT_%d"), ii);
		m_UiShift_TimeProduction[ii].m_ContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_TP_%d"), ii);
		m_UiShift_TimeProduction[ii].m_TpResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_PREGAMMA_%d"), ii);
		m_UiShift_TimeProduction[ii].m_PreGammaResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		m_UiShift_TimeProduction[ii].m_GoodResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_BAD_%d"), ii);
		m_UiShift_TimeProduction[ii].m_BadResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_UNKNOW_%d"), ii);
		m_UiShift_TimeProduction[ii].m_UnKnowResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		m_UiShift_TimeProduction[ii].m_InspectionTotal[nShift] = ini[strShiftTime][strTemp];
	}
}

void CAni_Data_Serever_PCApp::AOIInspctionDataSum(AOIProductionData productionData[MaxZone], int nShift, AOIProductionData &SumProductionData)
{
	for (int ii = 0; ii < MaxZone; ii++)
	{
		SumProductionData.m_AlignResult[nShift] += productionData[ii].m_AlignResult[nShift];
		SumProductionData.m_VisionResult[nShift] += productionData[ii].m_VisionResult[nShift];
		SumProductionData.m_ViewingResult[nShift] += productionData[ii].m_ViewingResult[nShift];
		SumProductionData.m_ContactResult[nShift] += productionData[ii].m_ContactResult[nShift];
		SumProductionData.m_FirstContactResult[nShift] += productionData[ii].m_FirstContactResult[nShift];
		SumProductionData.m_TpResult[nShift] += productionData[ii].m_TpResult[nShift];
		SumProductionData.m_PreGammaResult[nShift] += productionData[ii].m_PreGammaResult[nShift];
		SumProductionData.m_GoodResult[nShift] += productionData[ii].m_GoodResult[nShift];
		SumProductionData.m_BadResult[nShift] += productionData[ii].m_BadResult[nShift];
		SumProductionData.m_UnKnowResult[nShift] += productionData[ii].m_UnKnowResult[nShift];
		SumProductionData.m_InspectionTotal[nShift] += productionData[ii].m_InspectionTotal[nShift];
		SumProductionData.m_TrayDataOut[nShift] += productionData[ii].m_TrayDataOut[nShift];
	}
}

void CAni_Data_Serever_PCApp::AOIInspctionTimeDataSum(AOIProductionData productionData[InspectTimeTotalCount], int nShift, AOIProductionData &SumProductionData)
{
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		SumProductionData.m_AlignResult[nShift] += productionData[ii].m_AlignResult[nShift];
		SumProductionData.m_VisionResult[nShift] += productionData[ii].m_VisionResult[nShift];
		SumProductionData.m_ViewingResult[nShift] += productionData[ii].m_ViewingResult[nShift];
		SumProductionData.m_ContactResult[nShift] += productionData[ii].m_ContactResult[nShift];
		SumProductionData.m_FirstContactResult[nShift] += productionData[ii].m_FirstContactResult[nShift];
		SumProductionData.m_TpResult[nShift] += productionData[ii].m_TpResult[nShift];
		SumProductionData.m_PreGammaResult[nShift] += productionData[ii].m_PreGammaResult[nShift];
		SumProductionData.m_GoodResult[nShift] += productionData[ii].m_GoodResult[nShift];
		SumProductionData.m_BadResult[nShift] += productionData[ii].m_BadResult[nShift];
		SumProductionData.m_UnKnowResult[nShift] += productionData[ii].m_UnKnowResult[nShift];
		SumProductionData.m_InspectionTotal[nShift] += productionData[ii].m_InspectionTotal[nShift];
		SumProductionData.m_TrayDataOut[nShift] += productionData[ii].m_TrayDataOut[nShift];
	}
}

void CAni_Data_Serever_PCApp::AlignDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_ALIGN_PATH + strTemp);

	for (int ii = 0; ii < MaxZone; ii++)
	{
		theApp.m_shiftProduction[ii].m_AlignResult[nShift] = ini[PG_IndexName[ii]][_T("TOTALNG")];
		theApp.m_UiShiftProduction[ii].m_AlignResult[nShift] = ini[PG_UIIndexName[ii]][_T("UI_TOTALNG")];
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), jj + 1);
			m_shiftProduction[ii].m_AlignShiftGood[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("NG_%d"), jj + 1);
			m_shiftProduction[ii].m_AlignShiftNg[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			m_UiShiftProduction[ii].m_AlignShiftGood[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			m_UiShiftProduction[ii].m_AlignShiftNg[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.txt"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_ALIGN_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		theApp.m_shift_TimeProduction[ii].m_AlignResult[nShift] = ini2[strShiftTime][_T("TOTALNG")];

		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		theApp.m_UiShift_TimeProduction[ii].m_AlignResult[nShift] = ini2[strShiftTime][_T("UI_TOTALNG")];
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("GOOD_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_AlignShiftGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("NG_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_AlignShiftNg[nShift][jj] = ini2[strShiftTime][strTemp];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_AlignShiftGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_AlignShiftNg[nShift][jj] = ini2[strShiftTime][strTemp];
		}
	}

}

void CAni_Data_Serever_PCApp::AlignDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_ALIGN_PATH + strTemp);

	for (int ii = 0; ii < MaxZone; ii++)
	{
		ini[PG_IndexName[ii]][_T("TOTALNG")] = theApp.m_shiftProduction[ii].m_AlignResult[nShift];
		ini[PG_UIIndexName[ii]][_T("UI_TOTALNG")] = theApp.m_UiShiftProduction[ii].m_AlignResult[nShift];
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_AlignShiftGood[nShift][jj];

			strTemp.Format(_T("NG_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_AlignShiftNg[nShift][jj];

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_AlignShiftGood[nShift][jj];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_AlignShiftNg[nShift][jj];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.txt"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_ALIGN_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		ini2[strShiftTime][_T("TOTALNG")] = theApp.m_shift_TimeProduction[ii].m_AlignResult[nShift];

		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		ini2[strShiftTime][_T("UI_TOTALNG")] = theApp.m_UiShift_TimeProduction[ii].m_AlignResult[nShift];
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_AlignShiftGood[nShift][jj];

			strTemp.Format(_T("NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_AlignShiftNg[nShift][jj];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_AlignShiftGood[nShift][jj];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_AlignShiftNg[nShift][jj];
		}
	}
}

void CAni_Data_Serever_PCApp::ULDAlignDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(ULD_DATA_ALIGN_PATH + strTemp);

	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		theApp.m_ULDshiftProduction[ii].m_AlignResult[nShift] = ini[ULD_PG_IndexName[ii]][_T("TOTALNG")];
		theApp.m_ULDUiShiftProduction[ii].m_AlignResult[nShift] = ini[ULD_PG_UIIndexName[ii]][_T("UI_TOTALNG")];

		strTemp.Format(_T("GOOD_%d"), ii + 1);
		m_ULDshiftProduction[ii].m_AlignShiftGood[nShift] = ini[ULD_PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("NG_%d"), ii + 1);
		m_ULDshiftProduction[ii].m_AlignShiftNg[nShift] = ini[ULD_PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("UI_GOOD_%d"), ii + 1);
		m_ULDUiShiftProduction[ii].m_AlignShiftGood[nShift] = ini[ULD_PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_NG_%d"), ii + 1);
		m_ULDUiShiftProduction[ii].m_AlignShiftNg[nShift] = ini[ULD_PG_UIIndexName[ii]][strTemp];
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.txt"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(ULD_DATA_ALIGN_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		theApp.m_ULDshift_TimeProduction[ii].m_AlignResult[nShift] = ini2[strShiftTime][_T("TOTALNG")];

		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		theApp.m_ULDUiShift_TimeProduction[ii].m_AlignResult[nShift] = ini2[strShiftTime][_T("UI_TOTALNG")];

		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("GOOD_%d"), ii + 1);
		m_ULDshift_TimeProduction[ii].m_AlignShiftGood[nShift] = ini2[strShiftTime][strTemp];

		strTemp.Format(_T("NG_%d"), ii + 1);
		m_ULDshift_TimeProduction[ii].m_AlignShiftNg[nShift] = ini2[strShiftTime][strTemp];

		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("UI_GOOD_%d"), ii + 1);
		m_ULDUiShift_TimeProduction[ii].m_AlignShiftGood[nShift] = ini2[strShiftTime][strTemp];

		strTemp.Format(_T("UI_NG_%d"), ii + 1);
		m_ULDUiShift_TimeProduction[ii].m_AlignShiftNg[nShift] = ini2[strShiftTime][strTemp];
	}
}

void CAni_Data_Serever_PCApp::ULDAlignDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(ULD_DATA_ALIGN_PATH + strTemp);

	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		ini[ULD_PG_IndexName[ii]][_T("TOTALNG")] = theApp.m_ULDshiftProduction[ii].m_AlignResult[nShift];
		ini[PG_UIIndexName[ii]][_T("UI_TOTALNG")] = theApp.m_ULDUiShiftProduction[ii].m_AlignResult[nShift];
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), ii + 1);
			ini[ULD_PG_IndexName[ii]][strTemp] = theApp.m_ULDshiftProduction[ii].m_AlignShiftGood[nShift];

			strTemp.Format(_T("NG_%d"), ii + 1);
			ini[ULD_PG_IndexName[ii]][strTemp] = theApp.m_ULDshiftProduction[ii].m_AlignShiftNg[nShift];

			strTemp.Format(_T("UI_GOOD_%d"), ii + 1);
			ini[ULD_PG_UIIndexName[ii]][strTemp] = theApp.m_ULDUiShiftProduction[ii].m_AlignShiftGood[nShift];

			strTemp.Format(_T("UI_NG_%d"), ii + 1);
			ini[ULD_PG_UIIndexName[ii]][strTemp] = theApp.m_ULDUiShiftProduction[ii].m_AlignShiftNg[nShift];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.txt"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(ULD_DATA_ALIGN_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		ini2[strShiftTime][_T("TOTALNG")] = theApp.m_ULDshift_TimeProduction[ii].m_AlignResult[nShift];

		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		ini2[strShiftTime][_T("UI_TOTALNG")] = theApp.m_ULDUiShift_TimeProduction[ii].m_AlignResult[nShift];

		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("GOOD_%d"), ii + 1);
		ini2[strShiftTime][strTemp] = theApp.m_ULDshift_TimeProduction[ii].m_AlignShiftGood[nShift];

		strTemp.Format(_T("NG_%d"), ii + 1);
		ini2[strShiftTime][strTemp] = theApp.m_ULDshift_TimeProduction[ii].m_AlignShiftNg[nShift];

		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("UI_GOOD_%d"), ii + 1);
		ini2[strShiftTime][strTemp] = theApp.m_ULDUiShift_TimeProduction[ii].m_AlignShiftGood[nShift];

		strTemp.Format(_T("UI_NG_%d"), ii + 1);
		ini2[strShiftTime][strTemp] = theApp.m_ULDUiShift_TimeProduction[ii].m_AlignShiftNg[nShift];
	}
}

void CAni_Data_Serever_PCApp::ContactDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_CONTACT.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_CONTACT_PATH + strTemp);

	for (int ii = 0; ii < MaxZone; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strTemp.Format(_T("1ST_GOOD_%d"), jj + 1);
			m_shiftProduction[ii].m_ContactGood[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("1ST_NG_%d"), jj + 1);
			m_shiftProduction[ii].m_FirstContactNG[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("2ND_NG_%d"), jj + 1);
			m_shiftProduction[ii].m_ContactNg[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("UI_1ST_GOOD_%d"), jj + 1);
			m_UiShiftProduction[ii].m_ContactGood[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];

			strTemp.Format(_T("UI_1ST_NG_%d"), jj + 1);
			m_UiShiftProduction[ii].m_FirstContactNG[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];

			strTemp.Format(_T("UI_2ND_NG_%d"), jj + 1);
			m_UiShiftProduction[ii].m_ContactNg[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_CONTACT.txt"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_CONTACT_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("1ST_GOOD_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_ContactGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("1ST_NG_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_FirstContactNG[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("2ND_NG_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_ContactNg[nShift][jj] = ini2[strShiftTime][strTemp];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_1ST_GOOD_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_ContactGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("UI_1ST_NG_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_FirstContactNG[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("UI_2ND_NG_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_ContactNg[nShift][jj] = ini2[strShiftTime][strTemp];
		}
	}
}

void CAni_Data_Serever_PCApp::ContactDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_CONTACT.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_CONTACT_PATH + strTemp);

	for (int ii = 0; ii < MaxZone; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strTemp.Format(_T("1ST_GOOD_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_ContactGood[nShift][jj];

			strTemp.Format(_T("1ST_NG_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_FirstContactNG[nShift][jj];

			strTemp.Format(_T("2ND_NG_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_ContactNg[nShift][jj];

			strTemp.Format(_T("UI_1ST_GOOD_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_ContactGood[nShift][jj];

			strTemp.Format(_T("UI_1ST_NG_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_FirstContactNG[nShift][jj];

			strTemp.Format(_T("UI_2ND_NG_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_ContactNg[nShift][jj];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_CONTACT.txt"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_CONTACT_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("1ST_GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_ContactGood[nShift][jj];

			strTemp.Format(_T("1ST_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_FirstContactNG[nShift][jj];

			strTemp.Format(_T("2ND_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_ContactNg[nShift][jj];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_1ST_GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_ContactGood[nShift][jj];

			strTemp.Format(_T("UI_1ST_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_FirstContactNG[nShift][jj];

			strTemp.Format(_T("UI_2ND_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_ContactNg[nShift][jj];
		}
	}
}
void CAni_Data_Serever_PCApp::IndexCheck()
{
	m_csIndexCheck.Lock();
	for (int ii = 0; ii < MaxZone; ii++)
	{
		if (theApp.m_pEqIf->m_pMNetH->GetCurrentIndexZone(ii))
			theApp.m_CurrentIndexZone = ii;
	}
	m_csIndexCheck.Unlock();
}
void CAni_Data_Serever_PCApp::TpDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_TP.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_TP_PATH + strTemp);

	for (int ii = 0; ii < MaxZone; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), jj + 1);
			m_shiftProduction[ii].m_TpGood[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("NG_%d"), jj + 1);
			m_shiftProduction[ii].m_TpNg[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			m_UiShiftProduction[ii].m_TpGood[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			m_UiShiftProduction[ii].m_TpNg[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_TP.txt"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_TP_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("GOOD_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_TpGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("NG_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_TpNg[nShift][jj] = ini2[strShiftTime][strTemp];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_TpGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_TpNg[nShift][jj] = ini2[strShiftTime][strTemp];
		}
	}
}

void CAni_Data_Serever_PCApp::TpDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_TP.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_TP_PATH + strTemp);

	for (int ii = 0; ii < MaxZone; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_TpGood[nShift][jj];

			strTemp.Format(_T("NG_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_TpNg[nShift][jj];

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_TpGood[nShift][jj];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_TpNg[nShift][jj];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_TP.txt"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_TP_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_TpGood[nShift][jj];

			strTemp.Format(_T("NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_TpNg[nShift][jj];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_TpGood[nShift][jj];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_TpNg[nShift][jj];
		}
	}
}

void CAni_Data_Serever_PCApp::PreGammaDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_PREGAMMA.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_PREGAMMA_PATH + strTemp);

	for (int ii = 0; ii < MaxZone; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), jj + 1);
			m_shiftProduction[ii].m_PreGammaGood[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("NG_%d"), jj + 1);
			m_shiftProduction[ii].m_PreGammaNg[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			m_UiShiftProduction[ii].m_PreGammaGood[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			m_UiShiftProduction[ii].m_PreGammaNg[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_PREGAMMA.txt"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_PREGAMMA_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("GOOD_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_PreGammaGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("NG_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_PreGammaNg[nShift][jj] = ini2[strShiftTime][strTemp];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_PreGammaGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_PreGammaNg[nShift][jj] = ini2[strShiftTime][strTemp];
		}
	}
}

void CAni_Data_Serever_PCApp::PreGammaDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_PREGAMMA.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_PREGAMMA_PATH + strTemp);

	for (int ii = 0; ii < MaxZone; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_PreGammaGood[nShift][jj];

			strTemp.Format(_T("NG_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_PreGammaNg[nShift][jj];

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_PreGammaGood[nShift][jj];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_PreGammaNg[nShift][jj];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_PREGAMMA.txt"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_PREGAMMA_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_PreGammaGood[nShift][jj];

			strTemp.Format(_T("NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_PreGammaNg[nShift][jj];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_PreGammaGood[nShift][jj];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_PreGammaNg[nShift][jj];
		}
	}
}

void CAni_Data_Serever_PCApp::SetSaveHistoryCode(CString strPanelID, CString strInspName, ResultCodeRank code, BOOL bFlag)
{
	CStdioFile sFile;
	CString strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	CString strPath = _T("");
	strPath = DATA_INSPECT_RESULT_CODE_PATH + strInspName + _T("\\") + GetDateString2() + _T("\\");
	CreateFolders(strPath);
	
	if (!FileExists(strPath + _T("History") + strShift))
		bFlag = TRUE;

	if (bFlag == TRUE)
	{
		if (sFile.Open(strPath + _T("History") + strShift, CFile::modeCreate | CFile::modeWrite) == FALSE)
			return;
	}
	else
	{
		if (sFile.Open(strPath + _T("History") + strShift, CFile::modeWrite) == FALSE)
			return;
	}

	sFile.SeekToEnd();

	CString msg;
	msg.Format(_T("%s,%d,%s,%s"), code.m_strZone, code.m_iCh, strPanelID, code.m_strResultCode);
	sFile.WriteString(msg);

	msg = _T("\n");
	sFile.WriteString(msg);

	sFile.Close();
}

void CAni_Data_Serever_PCApp::SetLoadHistoryCode(int nShift)
{
	CStdioFile sFile;
	CString strTemp, strShift, strPanelID;
	ResultCodeRank RankData;
	vector<pair<CString, ResultCodeRank>> vecRankData;
	CStringArray responseTokens;
	strShift = nShift == 0 ? _T("DY") : _T("NT");

	for (int ii = 0; ii < NameCount; ii++)
	{
		vecRankData.clear();
		if (sFile.Open(DATA_INSPECT_RESULT_CODE_PATH + InspectName[ii] + _T("\\") + GetDateString2() + _T("\\") + _T("History") + strShift, CFile::modeNoInherit | CFile::modeRead))
		{
			while (sFile.ReadString(strTemp))
			{
				responseTokens.RemoveAll();
				CStringSupport::GetTokenArray(strTemp, _T(','), responseTokens);
				if (responseTokens.GetSize() == 4)
				{
					RankData.m_strZone = responseTokens[0];
					RankData.m_iCh = _ttoi(responseTokens[1]);
					strPanelID = responseTokens[2];
					RankData.m_strResultCode = responseTokens[3];

					vecRankData.push_back(make_pair(strPanelID, RankData));
				}
			}

			theApp.m_mapRankTotalList[nShift].insert(make_pair(InspectName[ii], vecRankData));
			sFile.Close();
		}
		else
			theApp.m_mapRankTotalList[nShift].insert(make_pair(InspectName[ii], vecRankData));
	}
}

void CAni_Data_Serever_PCApp::SetSaveRankCode(CString strInspName, CString strKey, int iValue, BOOL bFlag)
{
	CStdioFile sFile;
	CString strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	CString strPath = _T("");
	CStringArray responseTokens;
	ResultCodeRank RankData;
	strPath = DATA_INSPECT_RESULT_CODE_PATH + strInspName + _T("\\") + GetDateString2() + _T("\\");
	CreateFolders(strPath);

	if (!FileExists(strPath + _T("Rank") + strShift))
		bFlag = TRUE;

	if (bFlag == TRUE)
	{
		if (sFile.Open(strPath + _T("Rank") + strShift, CFile::modeCreate | CFile::modeWrite) == FALSE)
			return;
	}
	else
	{
		if (sFile.Open(strPath  + _T("Rank") + strShift, CFile::modeWrite) == FALSE)
			return;
	}

	sFile.SeekToEnd();

	CStringSupport::GetTokenArray(strKey, _T('^'), responseTokens);

	RankData.m_strZone = responseTokens[0];
	RankData.m_iCh = _ttoi(responseTokens[1]);
	RankData.m_strResultCode = responseTokens[2];
	RankData.m_iResultCodeCount = iValue;

	CString msg;
	msg.Format(_T("%s,%d,%s,%d"), RankData.m_strZone, RankData.m_iCh, RankData.m_strResultCode, RankData.m_iResultCodeCount);
	sFile.WriteString(msg);

	msg = _T("\n");
	sFile.WriteString(msg);

	sFile.Close();
}

void CAni_Data_Serever_PCApp::SetLoadRankCode(int nShift)
{
	CStdioFile sFile;
	CString strTemp, strShift, strKey;
	ResultCodeRank RankData;
	CStringArray responseTokens;
	map<CString, int> mapRankData;
	strShift = nShift == 0 ? _T("DY") : _T("NT");

	for (int ii = 0; ii < NameCount; ii++)
	{
		mapRankData.clear();
		if (sFile.Open(DATA_INSPECT_RESULT_CODE_PATH + InspectName[ii] + _T("\\") + GetDateString2() + _T("\\") + _T("Rank") + strShift, CFile::modeNoInherit | CFile::modeRead))
		{
			while (sFile.ReadString(strTemp))
			{
				responseTokens.RemoveAll();
				CStringSupport::GetTokenArray(strTemp, _T(','), responseTokens);
				if (responseTokens.GetSize() == 4)
				{
					RankData.m_strZone = responseTokens[0];
					RankData.m_iCh = _ttoi(responseTokens[1]);
					RankData.m_strResultCode = responseTokens[2];
					RankData.m_iResultCodeCount = _ttoi(responseTokens[3]);

					strKey = _T("");
					strKey.Format(_T("%s"), CStringSupport::FormatString(_T("%s^%d^%s"), RankData.m_strZone, RankData.m_iCh, RankData.m_strResultCode));
					mapRankData.insert(make_pair(strKey, RankData.m_iResultCodeCount));
				}
			}

			theApp.m_mapRankCodeCount[nShift].insert(make_pair(InspectName[ii], mapRankData));
			sFile.Close();
		}
		else
			theApp.m_mapRankCodeCount[nShift].insert(make_pair(InspectName[ii], mapRankData));
	}
}

void CAni_Data_Serever_PCApp::ULDInspctionDataSum(ULDProductionData productionData[ChMaxCount], int nShift, ULDProductionData &SumProductionData)
{
	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		SumProductionData.m_InspectionTotal[nShift] += productionData[ii].m_InspectionTotal[nShift];
		SumProductionData.m_GoodResult[nShift] += productionData[ii].m_GoodResult[nShift];
		SumProductionData.m_BadResult[nShift] += productionData[ii].m_BadResult[nShift];
		SumProductionData.m_AlignResult[nShift] += productionData[ii].m_AlignResult[nShift];
		SumProductionData.m_ContactResult[nShift] += productionData[ii].m_ContactResult[nShift];
		SumProductionData.m_GammaResult[nShift] += productionData[ii].m_GammaResult[nShift];
		SumProductionData.m_TouchResult[nShift] += productionData[ii].m_TouchResult[nShift];
		SumProductionData.m_OpvResult[nShift] += productionData[ii].m_OpvResult[nShift];
		SumProductionData.m_BufferTrayResult[nShift] += productionData[ii].m_BufferTrayResult[nShift];
		SumProductionData.m_ManualContactResult[nShift] += productionData[ii].m_ManualContactResult[nShift];
		SumProductionData.m_TrayInsertstatus[nShift] += productionData[ii].m_TrayInsertstatus[nShift];
		SumProductionData.m_SampleResult[nShift] += productionData[ii].m_SampleResult[nShift];
	}
}

void CAni_Data_Serever_PCApp::ULDInspctionTimeDataSum(ULDProductionData productionData[InspectTimeTotalCount], int nShift, ULDProductionData &SumProductionData)
{
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		SumProductionData.m_InspectionTotal[nShift] += productionData[ii].m_InspectionTotal[nShift];
		SumProductionData.m_GoodResult[nShift] += productionData[ii].m_GoodResult[nShift];
		SumProductionData.m_BadResult[nShift] += productionData[ii].m_BadResult[nShift];
		SumProductionData.m_AlignResult[nShift] += productionData[ii].m_AlignResult[nShift];
		SumProductionData.m_ContactResult[nShift] += productionData[ii].m_ContactResult[nShift];
		SumProductionData.m_GammaResult[nShift] += productionData[ii].m_GammaResult[nShift];
		SumProductionData.m_TouchResult[nShift] += productionData[ii].m_TouchResult[nShift];
		SumProductionData.m_OpvResult[nShift] += productionData[ii].m_OpvResult[nShift];
		SumProductionData.m_BufferTrayResult[nShift] += productionData[ii].m_BufferTrayResult[nShift];
		SumProductionData.m_ManualContactResult[nShift] += productionData[ii].m_ManualContactResult[nShift];
		SumProductionData.m_TrayInsertstatus[nShift] += productionData[ii].m_TrayInsertstatus[nShift];
		SumProductionData.m_SampleResult[nShift] += productionData[ii].m_SampleResult[nShift];
	}
}

void CAni_Data_Serever_PCApp::InspctionDefectDataSum(DefectCountData productionData, int nShift, DefectSumCountData &SumProductionData)
{
	for (int ii = 0; ii < DefectTitleMaxCount; ii++)
	{
		SumProductionData.m_AoiDefectTotalSum += productionData.m_AoiDefectTotalSum[ii];
		SumProductionData.m_OpvDefectTotalSum += productionData.m_OpvDefectTotalSum[ii];
		SumProductionData.m_MatchDefectTotalSum += productionData.m_MatchDefectTotalSum[ii];
		SumProductionData.m_OverKillDefectTotalSum += productionData.m_OverKillDefectTotalSum[ii];
		SumProductionData.m_UnderKillDefectTotalSum += productionData.m_UnderKillDefectTotalSum[ii];
		SumProductionData.m_TotalDefectSum += productionData.m_TotalDefectSum[ii];
	}
	SumProductionData.m_OpvOkSum = productionData.m_OpvOkSum;
	SumProductionData.m_OpvNgSum = productionData.m_OpvNgSum;
	SumProductionData.m_OpvTotalNgSum = productionData.m_OpvTotalNgSum;
}

void CAni_Data_Serever_PCApp::OpvDefectHistoryLosd()
{
	
	CStdioFile sFile;
	CString FileName, strShift, strInfo, strCurTitle = _T(""), strBackTitle = _T("");
	CStringArray responseTokens;
	DefectList defectHistory;
	vector<DefectList> vceHistory;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	BOOL bFlag = TRUE;
	FileName.Format(_T("%s%s_Opv_DefectHisTory_%s.csv"), DATA_OPV_DEFECT_HISTORY_PATH, theApp.m_strCurrentToday, strShift);

	if (sFile.Open(FileName, CFile::modeRead | CFile::typeUnicode) == FALSE)
		return;

	while (sFile.ReadString(strInfo))
	{
		if (strInfo.Find(_T("OperatorID")) != -1)
		{
			while (sFile.ReadString(strInfo))
			{
				responseTokens.RemoveAll();
				CStringSupport::GetTokenArray(strInfo, _T(','), responseTokens);
				defectHistory.strTime = responseTokens[0];
				defectHistory.strChNum = _ttoi(responseTokens[1]);
				defectHistory.strPanelID = responseTokens[2];
				strCurTitle = responseTokens[3];
				defectHistory.strDefectCode = responseTokens[4];
				defectHistory.strDefectGrade = responseTokens[5];
				defectHistory.strDefectDesctiption = responseTokens[6];
				defectHistory.strDefectOpvResult = responseTokens[7];
				defectHistory.strUserID = responseTokens[8];
				vceHistory.push_back(defectHistory);

				if (bFlag == TRUE)
				{
					strBackTitle = strCurTitle;
					bFlag = FALSE;
				}

				if (strBackTitle.CompareNoCase(strCurTitle))
					theApp.m_mapOpvDefectHistory[theApp.m_lastShiftIndex].insert(make_pair(strCurTitle, vceHistory));
			}
			theApp.m_mapOpvDefectHistory[theApp.m_lastShiftIndex].insert(make_pair(strCurTitle, vceHistory));
		}
	}
}

void CAni_Data_Serever_PCApp::OpvDefectPanelHistoryLosd()
{
	CStdioFile sFile;
	CString FileName, strShift, strInfo, strCurTitle = _T(""), strBackTitle = _T("");
	CStringArray responseTokens;
	DefectCountData panelDefect;
	//m_VecDefectHistory
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	BOOL bFlag = TRUE;
	FileName.Format(_T("%s%s_Opv_DefectList_%s.csv"), DATA_OPV_DEFECT_LIST_PATH, theApp.m_strCurrentToday, strShift);

	if (sFile.Open(FileName, CFile::modeRead | CFile::typeUnicode) == FALSE)
		return;

	while (sFile.ReadString(strInfo))
	{
		if (strInfo.Find(_T("OperatorID")) != -1)
		{
			while (sFile.ReadString(strInfo))
			{
				responseTokens.RemoveAll();
				CStringSupport::GetTokenArray(strInfo, _T(','), responseTokens);
				panelDefect.m_strTime = responseTokens[0];
				panelDefect.m_iChNum = _ttoi(responseTokens[1]);
				panelDefect.m_strPanelID = responseTokens[2];
				panelDefect.m_strOpvResult = responseTokens[3];
				panelDefect.m_iTotalMatch = _ttoi(responseTokens[4]);
				panelDefect.m_iTotalOverKill = _ttoi(responseTokens[5]);
				panelDefect.m_iTotalUnderKill = _ttoi(responseTokens[6]);
				panelDefect.m_MatchDefectTotalSum[0] = _ttoi(responseTokens[7]);
				panelDefect.m_OverKillDefectTotalSum[0] = _ttoi(responseTokens[8]);
				panelDefect.m_UnderKillDefectTotalSum[0] = _ttoi(responseTokens[9]);
				panelDefect.m_MatchDefectTotalSum[1] = _ttoi(responseTokens[10]);
				panelDefect.m_OverKillDefectTotalSum[1] = _ttoi(responseTokens[11]);
				panelDefect.m_UnderKillDefectTotalSum[1] = _ttoi(responseTokens[12]);
				panelDefect.m_MatchDefectTotalSum[2] = _ttoi(responseTokens[13]);
				panelDefect.m_OverKillDefectTotalSum[2] = _ttoi(responseTokens[14]);
				panelDefect.m_UnderKillDefectTotalSum[2] = _ttoi(responseTokens[15]);
				panelDefect.m_MatchDefectTotalSum[3] = _ttoi(responseTokens[16]);
				panelDefect.m_OverKillDefectTotalSum[3] = _ttoi(responseTokens[17]);
				panelDefect.m_UnderKillDefectTotalSum[3] = _ttoi(responseTokens[18]);
				panelDefect.m_MatchDefectTotalSum[4] = _ttoi(responseTokens[19]);
				panelDefect.m_OverKillDefectTotalSum[4] = _ttoi(responseTokens[20]);
				panelDefect.m_UnderKillDefectTotalSum[4] = _ttoi(responseTokens[21]);
				panelDefect.m_strOperationID = responseTokens[22];

				m_VecDefectHistory[theApp.m_lastShiftIndex].push_back(panelDefect);
			}
		}
	}
}

void CAni_Data_Serever_PCApp::OpvDefectSumCount()
{
	CString strFIlePath, strShift;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strFIlePath.Format(_T("%s%s_Opv_SumDefectCode_%s.ini"), DATA_OPV_SUM_DEFECT_CODE_PATH, theApp.m_strCurrentToday, strShift);
	EZIni ini(strFIlePath);
	for (int ii = 0; ii < DefectTitleMaxCount; ii++)
	{
		theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_AoiDefectTotalSum[ii] = ini[theApp.m_strDefectTitleName[ii]][_T("AOI")];
		theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvDefectTotalSum[ii] = ini[theApp.m_strDefectTitleName[ii]][_T("OPV")];
		theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_MatchDefectTotalSum[ii] = ini[theApp.m_strDefectTitleName[ii]][_T("MATH")];
		theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OverKillDefectTotalSum[ii] = ini[theApp.m_strDefectTitleName[ii]][_T("OVER")];
		theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_UnderKillDefectTotalSum[ii] = ini[theApp.m_strDefectTitleName[ii]][_T("UNDER")];
		theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_TotalDefectSum[ii] = ini[theApp.m_strDefectTitleName[ii]][_T("DEFECTSUM")];
	}
	theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvTotalNgSum = ini[_T("TOTALNG")][_T("TOTALSUM")];
	theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvOkSum = ini[_T("OK")][_T("TOTALSUM")];
	theApp.m_SumDefectCountData[theApp.m_lastShiftIndex].m_OpvNgSum = ini[_T("NG")][_T("TOTALSUM")];
}

void CAni_Data_Serever_PCApp::ULDInspectionDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(ULD_DATA_INSPECT_PATH + strTemp);

	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		strTemp.Format(_T("TOTAL_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("GOOD_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("BAD_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("ALIGN_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_AlignResult[nShift];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("GAMMA_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_GammaResult[nShift];

		strTemp.Format(_T("TOUCH_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_TouchResult[nShift];

		strTemp.Format(_T("OPV_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_OpvResult[nShift];

		strTemp.Format(_T("BUFFER_TRAY_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_BufferTrayResult[nShift];

		strTemp.Format(_T("SAMPLE_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_SampleResult[nShift];

		strTemp.Format(_T("TRAY_INSERTS_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_TrayInsertstatus[nShift];

		strTemp.Format(_T("MANUAL_CONTACT_%d"), ii);
		ini[ChName[ii]][strTemp] = m_ULDshiftProduction[ii].m_ManualContactResult[nShift];

		//<<UI DATA
		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("UI_BAB_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("UI_ALIGN_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_AlignResult[nShift];

		strTemp.Format(_T("UI_1ST_CONTACT_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("UI_2ND_CONTACT_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("UI_GAMMA_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_GammaResult[nShift];

		strTemp.Format(_T("UI_TOUCH_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_TouchResult[nShift];

		strTemp.Format(_T("UI_OPV_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_OpvResult[nShift];

		strTemp.Format(_T("UI_BUFFER_TRAY_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_BufferTrayResult[nShift];

		strTemp.Format(_T("UI_SAMPLE_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_SampleResult[nShift];

		strTemp.Format(_T("UI_TRAY_INSERTS_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_TrayInsertstatus[nShift];

		strTemp.Format(_T("UI_MANUAL_CONTACT_%d"), ii);
		ini[UI_ChName[ii]][strTemp] = m_ULDUiShiftProduction[ii].m_ManualContactResult[nShift];
	}
}

void CAni_Data_Serever_PCApp::ULDInspectionDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(ULD_DATA_INSPECT_PATH + strTemp);

	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		strTemp.Format(_T("TOTAL_%d"), ii);
		m_ULDshiftProduction[ii].m_InspectionTotal[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("GOOD_%d"), ii);
		m_ULDshiftProduction[ii].m_GoodResult[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("BAD_%d"), ii);
		m_ULDshiftProduction[ii].m_BadResult[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("ALIGN_%d"), ii);
		m_ULDshiftProduction[ii].m_AlignResult[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		m_ULDshiftProduction[ii].m_FirstContactResult[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		m_ULDshiftProduction[ii].m_ContactResult[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("GAMMA_%d"), ii);
		m_ULDshiftProduction[ii].m_GammaResult[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("TOUCH_%d"), ii);
		m_ULDshiftProduction[ii].m_TouchResult[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("OPV_%d"), ii);
		m_ULDshiftProduction[ii].m_OpvResult[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("BUFFER_TRAY_%d"), ii);
		m_ULDshiftProduction[ii].m_BufferTrayResult[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("SAMPLE_%d"), ii);
		m_ULDshiftProduction[ii].m_SampleResult[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("TRAY_INSERTS_%d"), ii);
		m_ULDshiftProduction[ii].m_TrayInsertstatus[nShift] = ini[ChName[ii]][strTemp];

		strTemp.Format(_T("MANUAL_CONTACT_%d"), ii);
		m_ULDshiftProduction[ii].m_ManualContactResult[nShift] = ini[ChName[ii]][strTemp];

		//<<UI DATA
		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		m_ULDUiShiftProduction[ii].m_InspectionTotal[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		m_ULDUiShiftProduction[ii].m_GoodResult[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_BAB_%d"), ii);
		m_ULDUiShiftProduction[ii].m_BadResult[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_ALIGN_%d"), ii);
		m_ULDUiShiftProduction[ii].m_AlignResult[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_1ST_CONTACT_%d"), ii);
		m_ULDUiShiftProduction[ii].m_FirstContactResult[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_2ND_CONTACT_%d"), ii);
		m_ULDUiShiftProduction[ii].m_ContactResult[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_GAMMA_%d"), ii);
		m_ULDUiShiftProduction[ii].m_GammaResult[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_TOUCH_%d"), ii);
		m_ULDUiShiftProduction[ii].m_TouchResult[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_OPV_%d"), ii);
		m_ULDUiShiftProduction[ii].m_OpvResult[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_BUFFER_TRAY_%d"), ii);
		m_ULDUiShiftProduction[ii].m_BufferTrayResult[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_SAMPLE_%d"), ii);
		m_ULDUiShiftProduction[ii].m_SampleResult[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_TRAY_INSERTS_%d"), ii);
		m_ULDUiShiftProduction[ii].m_TrayInsertstatus[nShift] = ini[UI_ChName[ii]][strTemp];

		strTemp.Format(_T("UI_MANUAL_CONTACT_%d"), ii);
		m_ULDUiShiftProduction[ii].m_ManualContactResult[nShift] = ini[UI_ChName[ii]][strTemp];
	}
}

void CAni_Data_Serever_PCApp::ULDInspectionTimeDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(ULD_DATA_INSPECT_PATHTIME + strTemp);

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		strTemp.Format(_T("TOTAL_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDshift_TimeProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("GOOD_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDshift_TimeProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("BAD_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDshift_TimeProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("ALIGN_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDshift_TimeProduction[ii].m_AlignResult[nShift];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDshift_TimeProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDshift_TimeProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("GAMMA_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDshift_TimeProduction[ii].m_GammaResult[nShift];

		strTemp.Format(_T("TOUCH_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDshift_TimeProduction[ii].m_TouchResult[nShift];

		strTemp.Format(_T("OPV_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDshift_TimeProduction[ii].m_OpvResult[nShift];

		strTemp.Format(_T("BUFFER_TRAY_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDshift_TimeProduction[ii].m_BufferTrayResult[nShift];

		//<<UI DATA
		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDUiShift_TimeProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDUiShift_TimeProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("UI_BAB_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDUiShift_TimeProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("UI_ALIGN_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDUiShift_TimeProduction[ii].m_AlignResult[nShift];

		strTemp.Format(_T("UI_1ST_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDUiShift_TimeProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("UI_2ND_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDUiShift_TimeProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("UI_GAMMA_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDUiShift_TimeProduction[ii].m_GammaResult[nShift];

		strTemp.Format(_T("UI_TOUCH_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDUiShift_TimeProduction[ii].m_TouchResult[nShift];

		strTemp.Format(_T("UI_OPV_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDUiShift_TimeProduction[ii].m_OpvResult[nShift];

		strTemp.Format(_T("UI_BUFFER_TRAY_%d"), ii);
		ini[strShiftTime][strTemp] = m_ULDUiShift_TimeProduction[ii].m_BufferTrayResult[nShift];

	}
}

void CAni_Data_Serever_PCApp::ULDInspectionTimeDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(ULD_DATA_INSPECT_PATHTIME + strTemp);

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("TOTAL_%d"), ii);
		m_ULDshift_TimeProduction[ii].m_InspectionTotal[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("GOOD_%d"), ii);
		m_ULDshift_TimeProduction[ii].m_GoodResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("BAD_%d"), ii);
		m_ULDshift_TimeProduction[ii].m_BadResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("ALIGN_%d"), ii);
		m_ULDshift_TimeProduction[ii].m_AlignResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		m_ULDshift_TimeProduction[ii].m_FirstContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		m_ULDshift_TimeProduction[ii].m_ContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("GAMMA_%d"), ii);
		m_ULDshift_TimeProduction[ii].m_GammaResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("TOUCH_%d"), ii);
		m_ULDshift_TimeProduction[ii].m_TouchResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("OPV_%d"), ii);
		m_ULDshift_TimeProduction[ii].m_OpvResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("BUFFER_TRAY_%d"), ii);
		m_ULDshift_TimeProduction[ii].m_BufferTrayResult[nShift] = ini[strShiftTime][strTemp];

		//<<UI DATA
		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		m_ULDUiShift_TimeProduction[ii].m_InspectionTotal[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		m_ULDUiShift_TimeProduction[ii].m_GoodResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_BAB_%d"), ii);
		m_ULDUiShift_TimeProduction[ii].m_BadResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_ALIGN_%d"), ii);
		m_ULDUiShift_TimeProduction[ii].m_AlignResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_1ST_CONTACT_%d"), ii);
		m_ULDUiShift_TimeProduction[ii].m_FirstContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_2ND_CONTACT_%d"), ii);
		m_ULDUiShift_TimeProduction[ii].m_ContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_GAMMA_%d"), ii);
		m_ULDUiShift_TimeProduction[ii].m_GammaResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_TOUCH_%d"), ii);
		m_ULDUiShift_TimeProduction[ii].m_TouchResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_OPV_%d"), ii);
		m_ULDUiShift_TimeProduction[ii].m_OpvResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_BUFFER_TRAY_%d"), ii);
		m_ULDUiShift_TimeProduction[ii].m_BufferTrayResult[nShift] = ini[strShiftTime][strTemp];

	}
}

CString CAni_Data_Serever_PCApp::SetLoadOpvResultCode(CString strPanelID)
{
	map<CString, map<CString, CString>>::iterator iter;
	map<CString, CString>::iterator iter2;
	map<CString, CString> mapCode;
	CDFSInfo DfsInfo;
	DefectCodeRank pDefectCodeRank;
	CString strPath, strTypeName;
	CString strSendCodeGrade, strSendCode, strSendGrade;
	int iCount = 0; 

	strPath = DFS_SHARE_OPVDFS_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\") + strPanelID + _T(".csv");
	strTypeName = _T("OPV");
	DfsInfo.DFSDefectBeginLoad(strPath, strTypeName, FALSE);

	if (DfsInfo.m_mapPanelDefect.size() > 0)
	{
		for (auto Rank : theApp.m_strVecRank[OPV])
		{
			if (iCount == theApp.m_iNumberSendToPlc)
				break;

			iter = DfsInfo.m_mapPanelDefect.find(strTypeName);
			if (iter != DfsInfo.m_mapPanelDefect.end())
			{
				iter2 = iter->second.find(Rank.first);
				if (iter2 != iter->second.end())
				{
					iCount++;
					if (iCount == 1)
						strSendGrade = iter2->second;

					strSendCode.AppendFormat(_T("%s"), iter2->first);
				}
			}
		}
	}
	if (strSendCode.IsEmpty() == FALSE && strSendGrade.IsEmpty() == FALSE)
		strSendCodeGrade.Format(_T("%s^%s"), strSendCode, strSendGrade);
	else
	{
		strSendCodeGrade = _T("");
	}

	return strSendCodeGrade;
}

CString CAni_Data_Serever_PCApp::ParsingDefectDesctiption(CString strCode)
{
	CStringArray responseTokens;
	BOOL bFlag = FALSE;
	for (int ii = 0; ii < DefectTitleMaxCount; ii++)
	{
		for (auto defect : theApp.m_VecDefectList[ii])
		{
			if (!defect.strDefectCode.CompareNoCase(strCode))
			{
				bFlag = TRUE;
				CStringSupport::GetTokenArray(defect.strDefectDesctiption, _T(':'), responseTokens);
				if (responseTokens.GetSize() == 0)
					return defect.strDefectDesctiption;
				else
					return responseTokens[0];
			}
		}
	}

	return _T("");
}

void CAni_Data_Serever_PCApp::DefectCodeListLoad()
{
	setlocale(LC_ALL, "Chinese");
	CStringArray responseTokens;
	CStdioFile sFile;
	CString strFilename = DATA_DEFECTLIST_1_PATH + theApp.m_CurrentModel.m_AlignPcCurrentModelName + _T("\\DefectList.CSV");
	CString strInfo;
	CString strCode;
	DefectList list;

	if (theApp.m_CurrentModel.m_AlignPcCurrentModelName == _T(""))
	{
		EZIni ini(DATA_SYSTEM_PATH + _T("modelData.ini"));
		CString strModelName;

		strModelName = ini[_T("MODEL")][_T("LAST_MODEL")];
		strFilename = DATA_DEFECTLIST_1_PATH + strModelName + _T("\\DefectList.CSV");
	}

	if (sFile.Open(strFilename, CFile::modeRead) == FALSE)
		return;

	int ii = 0;
	while (sFile.ReadString(strInfo))
	{
		if (strInfo.Find(_T("Pattern")) != -1)
			break;

		if (strInfo.Find(theApp.m_strDefectTitleName[ii]) != -1)
		{
			theApp.m_VecDefectList[ii].clear();
			while (sFile.ReadString(strInfo))
			{
				responseTokens.RemoveAll();
				CStringSupport::GetTokenArray(strInfo, _T(','), responseTokens);

				if (responseTokens[0] == _T(""))
				{
					ii++;
					break;
				}

				if (responseTokens.GetSize() >= 2 && strInfo.Find(_T("Defect_Name")) == -1)
				{
					list.strDefectCode = responseTokens[0];
					list.strDefectGrade = responseTokens[1];
					list.strDefectDesctiption = responseTokens[2];
					theApp.m_VecDefectList[ii].push_back(list);
				}
			}
		}
	}

	sFile.Close();
}

void CAni_Data_Serever_PCApp::OpvLoadTitleName()
{
	EZIni ini(DATA_OPV_SYSTEM_SET_SYSTEM_DATA_1_PATH + _T("SetSystemData_1.ini"));

	theApp.m_strDefectTitleName[DefectTitleName_1] = ini[_T("SYSTEM")][_T("Defect_Title_1")];
	theApp.m_strDefectTitleName[DefectTitleName_2] = ini[_T("SYSTEM")][_T("Defect_Title_2")];
	theApp.m_strDefectTitleName[DefectTitleName_3] = ini[_T("SYSTEM")][_T("Defect_Title_3")];
	theApp.m_strDefectTitleName[DefectTitleName_4] = ini[_T("SYSTEM")][_T("Defect_Title_4")];
	theApp.m_strDefectTitleName[DefectTitleName_5] = ini[_T("SYSTEM")][_T("Defect_Title_5")];
}
#else
void CAni_Data_Serever_PCApp::InspctionDataSum(ProductionData productionData[MaxGammaStage], int nShift, ProductionData &SumProductionData)
{
	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		SumProductionData.m_InspectionTotal[nShift] += productionData[ii].m_InspectionTotal[nShift];
		SumProductionData.m_GoodResult[nShift] += productionData[ii].m_GoodResult[nShift];
		SumProductionData.m_BadResult[nShift] += productionData[ii].m_BadResult[nShift];
		SumProductionData.m_ContactResult[nShift] += productionData[ii].m_ContactResult[nShift];
		SumProductionData.m_FirstContactResult[nShift] += productionData[ii].m_FirstContactResult[nShift];
		SumProductionData.m_ManualContactResult[nShift] += productionData[ii].m_ManualContactResult[nShift];
		SumProductionData.m_MtpResult[nShift] += productionData[ii].m_MtpResult[nShift];
		SumProductionData.m_AlignResult[nShift] += productionData[ii].m_AlignResult[nShift];
	}
}

void CAni_Data_Serever_PCApp::InspctionTimeDataSum(ProductionData productionData[InspectTimeTotalCount], int nShift, ProductionData &SumProductionData)
{
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		SumProductionData.m_InspectionTotal[nShift] += productionData[ii].m_InspectionTotal[nShift];
		SumProductionData.m_GoodResult[nShift] += productionData[ii].m_GoodResult[nShift];
		SumProductionData.m_BadResult[nShift] += productionData[ii].m_BadResult[nShift];
		SumProductionData.m_ContactResult[nShift] += productionData[ii].m_ContactResult[nShift];
		SumProductionData.m_FirstContactResult[nShift] += productionData[ii].m_FirstContactResult[nShift];
		SumProductionData.m_ManualContactResult[nShift] += productionData[ii].m_ManualContactResult[nShift];
		SumProductionData.m_MtpResult[nShift] += productionData[ii].m_MtpResult[nShift];
		SumProductionData.m_AlignResult[nShift] += productionData[ii].m_AlignResult[nShift];
	}
}

void CAni_Data_Serever_PCApp::InspectionDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_INSPECT_PATH + strTemp);

	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		strTemp.Format(_T("TOTAL_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("GOOD_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("BAD_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("MANUL_CONTACT_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_ManualContactResult[nShift];

		strTemp.Format(_T("MTP_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_MtpResult[nShift];

		strTemp.Format(_T("ALIGN_%d"), ii);
		ini[PG_IndexName[ii]][strTemp] = m_shiftProduction[ii].m_AlignResult[nShift];

		//<<UI DATA
		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("UI_BAB_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("UI_CONTACT_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("UI_MANUAL_CONTACT_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_ManualContactResult[nShift];

		strTemp.Format(_T("UI_MTP_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_MtpResult[nShift];

		strTemp.Format(_T("UI_ALIGN_%d"), ii);
		ini[PG_UIIndexName[ii]][strTemp] = m_UiShiftProduction[ii].m_AlignResult[nShift];
	}
}

void CAni_Data_Serever_PCApp::InspectionDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_INSPECT_PATH + strTemp);

	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		strTemp.Format(_T("TOTAL_%d"), ii);
		m_shiftProduction[ii].m_InspectionTotal[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("GOOD_%d"), ii);
		m_shiftProduction[ii].m_GoodResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("BAD_%d"), ii);
		m_shiftProduction[ii].m_BadResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		m_shiftProduction[ii].m_FirstContactResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		m_shiftProduction[ii].m_ContactResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("MANUAL_CONTACT_%d"), ii);
		m_shiftProduction[ii].m_ManualContactResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("MTP_%d"), ii);
		m_shiftProduction[ii].m_MtpResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		strTemp.Format(_T("ALIGN_%d"), ii);
		m_shiftProduction[ii].m_AlignResult[nShift] = ini[PG_IndexName[ii]][strTemp];

		//<<UI DATA
		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		m_UiShiftProduction[ii].m_InspectionTotal[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		m_UiShiftProduction[ii].m_GoodResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_BAB_%d"), ii);
		m_UiShiftProduction[ii].m_BadResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_CONTACT_%d"), ii);
		m_UiShiftProduction[ii].m_ContactResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_MANUAL_CONTACT_%d"), ii);
		m_UiShiftProduction[ii].m_ManualContactResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_MTP_%d"), ii);
		m_UiShiftProduction[ii].m_MtpResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];

		strTemp.Format(_T("UI_ALIGN_%d"), ii);
		m_UiShiftProduction[ii].m_AlignResult[nShift] = ini[PG_UIIndexName[ii]][strTemp];
	}
}

void CAni_Data_Serever_PCApp::InspectionTimeDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_INSPECT_PATHTIME + strTemp);

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		strTemp.Format(_T("TOTAL_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("GOOD_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("BAD_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("MANUAL_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_ManualContactResult[nShift];

		strTemp.Format(_T("MTP_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_MtpResult[nShift];

		strTemp.Format(_T("ALIGN_%d"), ii);
		ini[strShiftTime][strTemp] = m_shift_TimeProduction[ii].m_AlignResult[nShift];

		//<<UI DATA
		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_InspectionTotal[nShift];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_GoodResult[nShift];

		strTemp.Format(_T("UI_BAB_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_BadResult[nShift];

		strTemp.Format(_T("UI_1ST_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_FirstContactResult[nShift];

		strTemp.Format(_T("UI_2ND_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_ContactResult[nShift];

		strTemp.Format(_T("UI_MANUAL_CONTACT_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_ManualContactResult[nShift];

		strTemp.Format(_T("UI_MTP_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_MtpResult[nShift];

		strTemp.Format(_T("UI_ALIGN_%d"), ii);
		ini[strShiftTime][strTemp] = m_UiShift_TimeProduction[ii].m_AlignResult[nShift];
	}
}

void CAni_Data_Serever_PCApp::InspectionTimeDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s.txt"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_INSPECT_PATHTIME + strTemp);

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("TOTAL_%d"), ii);
		m_shift_TimeProduction[ii].m_InspectionTotal[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("GOOD_%d"), ii);
		m_shift_TimeProduction[ii].m_GoodResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("BAD_%d"), ii);
		m_shift_TimeProduction[ii].m_BadResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("1ST_CONTACT_%d"), ii);
		m_shift_TimeProduction[ii].m_FirstContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("2ND_CONTACT_%d"), ii);
		m_shift_TimeProduction[ii].m_ContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("MANUAL_CONTACT_%d"), ii);
		m_shift_TimeProduction[ii].m_ManualContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("MTP_%d"), ii);
		m_shift_TimeProduction[ii].m_MtpResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("ALIGN_%d"), ii);
		m_shift_TimeProduction[ii].m_AlignResult[nShift] = ini[strShiftTime][strTemp];

		//<<UI DATA
		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

		strTemp.Format(_T("UI_TOTAL_%d"), ii);
		m_UiShift_TimeProduction[ii].m_InspectionTotal[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_GOOD_%d"), ii);
		m_UiShift_TimeProduction[ii].m_GoodResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_BAB_%d"), ii);
		m_UiShift_TimeProduction[ii].m_BadResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_1ST_CONTACT_%d"), ii);
		m_UiShift_TimeProduction[ii].m_FirstContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_2ND_CONTACT_%d"), ii);
		m_UiShift_TimeProduction[ii].m_ContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_MANUAL_CONTACT_%d"), ii);
		m_UiShift_TimeProduction[ii].m_ManualContactResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_MTP_%d"), ii);
		m_UiShift_TimeProduction[ii].m_MtpResult[nShift] = ini[strShiftTime][strTemp];

		strTemp.Format(_T("UI_ALIGN_%d"), ii);
		m_UiShift_TimeProduction[ii].m_AlignResult[nShift] = ini[strShiftTime][strTemp];
	}
}

void CAni_Data_Serever_PCApp::AlignDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.ini"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_ALIGN_PATH + strTemp);

	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		theApp.m_shiftProduction[ii].m_AlignResult[nShift] = ini[PG_IndexName[ii]][_T("TOTALNG")];
		theApp.m_UiShiftProduction[ii].m_AlignResult[nShift] = ini[PG_UIIndexName[ii]][_T("UI_TOTALNG")];
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), jj + 1);
			m_shiftProduction[ii].m_AlignShiftGood[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("NG_%d"), jj + 1);
			m_shiftProduction[ii].m_AlignShiftNg[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			m_UiShiftProduction[ii].m_AlignShiftGood[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			m_UiShiftProduction[ii].m_AlignShiftNg[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];
		}

	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.ini"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_ALIGN_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		theApp.m_shift_TimeProduction[ii].m_AlignResult[nShift] = ini2[strShiftTime][_T("TOTALNG")];

		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		theApp.m_UiShift_TimeProduction[ii].m_AlignResult[nShift] = ini2[strShiftTime][_T("UI_TOTALNG")];
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("GOOD_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_AlignShiftGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("NG_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_AlignShiftNg[nShift][jj] = ini2[strShiftTime][strTemp];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_AlignShiftGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_AlignShiftNg[nShift][jj] = ini2[strShiftTime][strTemp];
		}
	}

}

void CAni_Data_Serever_PCApp::AlignDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.ini"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_ALIGN_PATH + strTemp);

	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		ini[PG_IndexName[ii]][_T("TOTALNG")] = theApp.m_shiftProduction[ii].m_AlignResult[nShift];
		ini[PG_UIIndexName[ii]][_T("UI_TOTALNG")] = theApp.m_UiShiftProduction[ii].m_AlignResult[nShift];
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_AlignShiftGood[nShift][jj];

			strTemp.Format(_T("NG_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_AlignShiftNg[nShift][jj];

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_AlignShiftGood[nShift][jj];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_AlignShiftNg[nShift][jj];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_ALIGN.ini"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_ALIGN_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		ini2[strShiftTime][_T("TOTALNG")] = theApp.m_shift_TimeProduction[ii].m_AlignResult[nShift];

		strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));
		ini2[strShiftTime][_T("UI_TOTALNG")] = theApp.m_UiShift_TimeProduction[ii].m_AlignResult[nShift];
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_AlignShiftGood[nShift][jj];

			strTemp.Format(_T("NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_AlignShiftNg[nShift][jj];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_AlignShiftGood[nShift][jj];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_AlignShiftNg[nShift][jj];
		}
	}
}

void CAni_Data_Serever_PCApp::ContactDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_CONTACT.ini"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_CONTACT_PATH + strTemp);

	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strTemp.Format(_T("1ST_GOOD_%d"), jj + 1);
			m_shiftProduction[ii].m_ContactGood[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("1ST_NG_%d"), jj + 1);
			m_shiftProduction[ii].m_FirstContactNG[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("2ND_NG_%d"), jj + 1);
			m_shiftProduction[ii].m_ContactNg[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("UI_1ST_GOOD_%d"), jj + 1);
			m_UiShiftProduction[ii].m_ContactGood[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];

			strTemp.Format(_T("UI_1ST_NG_%d"), jj + 1);
			m_UiShiftProduction[ii].m_FirstContactNG[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];

			strTemp.Format(_T("UI_2ND_NG_%d"), jj + 1);
			m_UiShiftProduction[ii].m_ContactNg[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_CONTACT.ini"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_CONTACT_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("1ST_GOOD_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_ContactGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("1ST_NG_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_FirstContactNG[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("2ND_NG_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_ContactNg[nShift][jj] = ini2[strShiftTime][strTemp];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_1ST_GOOD_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_ContactGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("UI_1ST_NG_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_FirstContactNG[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("UI_2ND_NG_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_ContactNg[nShift][jj] = ini2[strShiftTime][strTemp];
		}
	}
}

void CAni_Data_Serever_PCApp::ContactDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_CONTACT.ini"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_CONTACT_PATH + strTemp);

	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strTemp.Format(_T("1ST_GOOD_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_ContactGood[nShift][jj];

			strTemp.Format(_T("1ST_NG_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_FirstContactNG[nShift][jj];

			strTemp.Format(_T("2ND_NG_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_ContactNg[nShift][jj];

			strTemp.Format(_T("UI_1ST_GOOD_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_ContactGood[nShift][jj];

			strTemp.Format(_T("UI_1ST_NG_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_FirstContactNG[nShift][jj];

			strTemp.Format(_T("UI_2ND_NG_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_ContactNg[nShift][jj];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_CONTACT.ini"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_CONTACT_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("1ST_GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_ContactGood[nShift][jj];

			strTemp.Format(_T("1ST_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_FirstContactNG[nShift][jj];

			strTemp.Format(_T("2ND_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_ContactNg[nShift][jj];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_1ST_GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_ContactGood[nShift][jj];

			strTemp.Format(_T("UI_1ST_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_FirstContactNG[nShift][jj];

			strTemp.Format(_T("UI_2ND_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_ContactNg[nShift][jj];
		}
	}
}

void CAni_Data_Serever_PCApp::MtpDataLoad(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_MTP.ini"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_MTP_PATH + strTemp);

	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), jj + 1);
			m_shiftProduction[ii].m_MtpGood[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("NG_%d"), jj + 1);
			m_shiftProduction[ii].m_MtpNg[nShift][jj] = ini[PG_IndexName[ii]][strTemp];

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			m_UiShiftProduction[ii].m_MtpGood[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			m_UiShiftProduction[ii].m_MtpNg[nShift][jj] = ini[PG_UIIndexName[ii]][strTemp];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_MTP.ini"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_MTP_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("GOOD_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_MtpGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("NG_%d"), jj + 1);
			m_shift_TimeProduction[ii].m_MtpNg[nShift][jj] = ini2[strShiftTime][strTemp];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_MtpGood[nShift][jj] = ini2[strShiftTime][strTemp];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			m_UiShift_TimeProduction[ii].m_MtpNg[nShift][jj] = ini2[strShiftTime][strTemp];
		}
	}
}

void CAni_Data_Serever_PCApp::MtpDataSave(int nShift)
{
	if (theApp.m_lastShiftIndex > 2)
		return;

	CString strTemp, strShift, strShiftTime;
	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_MTP.ini"), theApp.m_strCurrentToday, strShift);
	//strShift
	EZIni ini(DATA_MTP_PATH + strTemp);

	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strTemp.Format(_T("GOOD_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_MtpGood[nShift][jj];

			strTemp.Format(_T("NG_%d"), jj + 1);
			ini[PG_IndexName[ii]][strTemp] = theApp.m_shiftProduction[ii].m_MtpNg[nShift][jj];

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_MtpGood[nShift][jj];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			ini[PG_UIIndexName[ii]][strTemp] = theApp.m_UiShiftProduction[ii].m_MtpNg[nShift][jj];
		}
	}

	strShift = nShift == 0 ? _T("DY") : _T("NT");
	strTemp.Format(_T("%s_%s_MTP.ini"), theApp.m_strCurrentToday, strShift);
	EZIni ini2(DATA_MTP_PATHTIME + strTemp);
	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		for (int jj = 0; jj < ChMaxCount; jj++)
		{
			strShiftTime.Format(_T("%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_MtpGood[nShift][jj];

			strTemp.Format(_T("NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_shift_TimeProduction[ii].m_MtpNg[nShift][jj];

			strShiftTime.Format(_T("UI_%s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart));

			strTemp.Format(_T("UI_GOOD_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_MtpGood[nShift][jj];

			strTemp.Format(_T("UI_NG_%d"), jj + 1);
			ini2[strShiftTime][strTemp] = theApp.m_UiShift_TimeProduction[ii].m_MtpNg[nShift][jj];
		}
	}
}

void CAni_Data_Serever_PCApp::GammaDefectInfoSave(CString strPanelID, CString strFpcID, CString strCode, CString strGrade)
{
	CString strPath, strFilePath, strShift, strCodeGrade;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strPath.Format(_T("%s\\%s_%s"), DATA_DEFECT_CODE_PATH, theApp.m_strCurrentToday, strShift);
	CreateFolders(strPath);

	if (theApp.m_PanelTestStart)
		strFpcID.Format(_T("TEST"));

	strFilePath.Format(_T("%s\\%s.ini"), strPath, strFpcID);
	EZIni ini(strFilePath);

	ini[_T("GAMMA_DEFECT")][_T("CODE")] = strCode;
	ini[_T("GAMMA_DEFECT")][_T("GRADE")] = strGrade;
}

CString CAni_Data_Serever_PCApp::GammaDefectInfoLoad(CString strPanelID, CString strFpcID)
{
	CString strFilePath, strShift, strCode, strGrade, strCodeGrade;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strFilePath.Format(_T("%s\\%s_%s\\%s.ini"), DATA_DEFECT_CODE_PATH, theApp.m_strCurrentToday, strShift, strFpcID);
	EZIni ini(strFilePath);

	strCode = ini[_T("GAMMA_DEFECT")][_T("CODE")];
	strGrade = ini[_T("GAMMA_DEFECT")][_T("GRADE")];

	strCodeGrade = CStringSupport::FormatString(_T("%s^%s"), strCode, strGrade);

	return strCodeGrade;
}

#endif

void CAni_Data_Serever_PCApp::AlarmDataSave(vector<AlarmDataItem> alarmData, BOOL bFlag)
{
	CStdioFile sFile;
	if (!FileExists(LOG_ALARM_HISTORY_PATH + _T("History")))
		bFlag = TRUE;

	if (bFlag == TRUE)
	{
		if (sFile.Open(LOG_ALARM_HISTORY_PATH + _T("History"), CFile::modeCreate | CFile::modeWrite) == FALSE)
			return;
	}
	else
	{
		if (sFile.Open(LOG_ALARM_HISTORY_PATH + _T("History"), CFile::modeWrite) == FALSE)
			return;
	}

	sFile.SeekToEnd();

	CString msg;
	for (auto Alarm : alarmData)
	{
		if (!Alarm.m_alarmStartTime.IsEmpty())
		{
			Alarm.m_alarmClearTime = AlarmTimeParsing(GetTimeString(), Alarm.m_alarmStartTime);
			msg.Format(_T("%s,%s,%s,%s,%s,%s"), Alarm.m_strTime, Alarm.m_alarmStartTime, GetTimeString(), Alarm.m_alarmCode, Alarm.m_alarmMsg, Alarm.m_alarmClearTime);
			sFile.WriteString(msg);

			msg = _T("\n");
			sFile.WriteString(msg);
		}
	}
	sFile.Close();

	CFile   File;
	CString FileName, strString, strTemp, strShift;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	FileName.Format(_T("%s\\%s_AlarmLog_%s.csv"), DATA_ALARM_PATH, theApp.m_strCurrentToday, strShift);

	BOOL bOpen = FALSE;
	if (!File.Open(FileName, CFile::modeReadWrite | CFile::shareDenyNone))
	{
		if (File.Open(FileName, CFile::modeCreate | CFile::modeWrite))
		{
			bOpen = TRUE;

			strString.Format(_T("Date, Start Time, End Time, Alarm Code, Alarm Msg, Clear Time"));
			strString += "\r\n";
			File.Write(strString.GetBuffer(), strString.GetLength() * 2);
			strString.ReleaseBuffer();
		}

	}
	else bOpen = TRUE;

	if (bOpen){
		CString strTime, strDefectString = _T("");
		CTime cTime;

		cTime = CTime::GetCurrentTime();
		File.SeekToEnd();
		for (auto Alarm : alarmData)
		{
			if (!Alarm.m_alarmStartTime.IsEmpty())
			{
				Alarm.m_alarmClearTime = AlarmTimeParsing(GetTimeString(), Alarm.m_alarmStartTime);
				strString.Format(_T("%s,%s,%s,%s,%s,%s"), Alarm.m_strTime, Alarm.m_alarmStartTime, GetTimeString(), Alarm.m_alarmCode, Alarm.m_alarmMsg, Alarm.m_alarmClearTime);
				strString += "\r\n";

				int iLen = strString.GetLength();
				File.Write(strString.GetBuffer(), iLen * 2);
				strString.ReleaseBuffer();
			}
		}
		File.Close();
	}
}

void CAni_Data_Serever_PCApp::AlarmDataLoad()
{
	CStdioFile sFile;
	CString strTemp, strCurPos, strClear;
	if (sFile.Open(LOG_ALARM_HISTORY_PATH + _T("History"), CFile::modeNoInherit | CFile::modeRead))
	{
		while (sFile.ReadString(strTemp))
		{
			AlarmDataItem alarmData;
			CStringArray responseTokens;
			CStringSupport::GetTokenArray(strTemp, _T(','), responseTokens);
			alarmData.m_strTime = responseTokens[0];
			alarmData.m_alarmStartTime = responseTokens[1];
			alarmData.m_alarmEndTime = responseTokens[2];
			alarmData.m_alarmCode = responseTokens[3];
			alarmData.m_alarmMsg = responseTokens[4];
			alarmData.m_alarmClearTime = responseTokens[5];

			theApp.m_AlarmDataList.insert(theApp.m_AlarmDataList.begin(), alarmData);

		}

		sFile.Close();
	}

	theApp.AlarmMaxCount = theApp.m_AlarmDataList.size();
}

void CAni_Data_Serever_PCApp::TactTimeDataSave(int TactTimeUnit)
{
	CString strTemp;
	strTemp.Format(_T("%s_TactTime.ini"), theApp.m_strCurrentToday);

	//strShift
	EZIni ini(DATA_TACT_TIME_PATH + strTemp);

	strTemp.Format(_T("%s_AVG"), theApp.m_vecTactName[TactTimeUnit].m_strTactTimeName);
	ini[_T("TOTAL")][strTemp] = theApp.m_pTactTimeList[TactTimeUnit].m_iSumTimeValue;
	strTemp.Format(_T("%s_COUNT"), theApp.m_vecTactName[TactTimeUnit].m_strTactTimeName);
	ini[_T("TOTAL")][strTemp] = theApp.m_pTactTimeList[TactTimeUnit].m_iTactTimeCount;
}

void CAni_Data_Serever_PCApp::TactTimeTotalDataSave(int TactTimeUnit, DWORD dwTime, BOOL bTotalFlag)
{
	m_csFileSave.Lock();
	CString strTemp, strFilePath, strValue;
	BOOL bFlag = FALSE;
	int TactTIme = dwTime;
	if (bTotalFlag == TRUE)
		strValue.Format(_T("Total : %d"), TactTIme);
	else
		strValue.Format(_T("%d"), TactTIme);

	strTemp.Format(_T("%s_TactTime.ini"), theApp.m_strCurrentToday);

	strTemp.Format(_T("%s\\%s\\"), DATA_TACT_TIME_UNIT_PATH, theApp.m_strCurrentToday);
	CreateFolders(strTemp);
	strFilePath.Format(_T("%s%s_%s.txt"), strTemp, theApp.m_vecTactName[TactTimeUnit].m_strTactTimeName, theApp.m_strCurrentToday);

	CStdioFile sFile;
	
	if (sFile.Open(strFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite))
	{
		bFlag = TRUE;
		sFile.SeekToEnd();
		sFile.WriteString(strValue + _T("\n"));
	}

	if (bFlag == TRUE)
		sFile.Close();

	m_csFileSave.Unlock();
}

void CAni_Data_Serever_PCApp::TotalTactTimeLoad()
{
	CString strTemp ;
	strTemp.Format(_T("%s_TactTime.ini"), theApp.m_strCurrentToday);
	EZIni ini(DATA_TACT_TIME_PATH + strTemp);

	for (int ii = 0; ii < theApp.m_vecTactName.size(); ii++)
	{
		strTemp.Format(_T("%s_AVG"), theApp.m_vecTactName[ii].m_strTactTimeName);
		theApp.m_pTactTimeList[ii].m_iSumTimeValue = ini[_T("TOTAL")][strTemp];
		strTemp.Format(_T("%s_COUNT"), theApp.m_vecTactName[ii].m_strTactTimeName);
		theApp.m_pTactTimeList[ii].m_iTactTimeCount = ini[_T("TOTAL")][strTemp];
	}
}

void CAni_Data_Serever_PCApp::GetSystemData()
{
	EZIni ini(DATA_SYSTEM_DATA_PATH);
	theApp.m_iLanguageSelect = ini[_T("DATA")][_T("LANGUAGAE")];
	theApp.m_strCurrentToday = ini[_T("DATA")][_T("CURRENT_TOADY")];

	theApp.m_strMachineType = ini[_T("DATA")][_T("MACHINE_TYPE")];
	theApp.m_strEqpId = ini[_T("EQP")][_T("ID")];
	theApp.m_strEqpNum = ini[_T("EQP")][_T("EQP_NUM")];
	theApp.m_strFileServerID = ini[_T("EQP")][_T("FILESERVER_ID")];
	theApp.m_strCompanyLine = ini[_T("DATA")][_T("COMPANY_LINE")];
	theApp.m_strOpvImageWidth = ini[_T("DATA")][_T("OPV_IMAGE_WIDTH")];
	theApp.m_strOpvImageHeight = ini[_T("DATA")][_T("OPV_IMAGE_HEIGHT")];
	theApp.m_strOkGrade = ini[_T("DATA")][_T("OK_GRADE")];
	theApp.m_strContactNgGrade  = ini[_T("DATA")][_T("CONTACT_NG_GRADE")];
	theApp.m_strContactNgCode = ini[_T("DATA")][_T("CONTACT_NG_CODE")];
	theApp.m_strAlignCount = ini[_T("DATA")][_T("ALIGN_COUNT")];
	theApp.m_iMachineType = ini[_T("DATA")][_T("MACHINE_NAME")];
	theApp.m_strOKProcessID = ini[_T("DATA")][_T("OK_PROCESSID")];
	theApp.m_strNGProcessID = ini[_T("DATA")][_T("NG_PROCESSID")];
	theApp.m_strFFUPortNum = ini[_T("DATA")][_T("FFU_PORT")];
	theApp.m_strARSPortNum = ini[_T("DATA")][_T("ARS_PORT")];
	theApp.m_strFFUEndPoint = ini[_T("DATA")][_T("FFU_END")];
	theApp.m_strPGName = ini[_T("PG")][_T("PGNAME")];
	theApp.m_strLightingIP = ini[_T("LIGHTING")][_T("IP")];
	theApp.m_strLightingPort = ini[_T("LIGHTING")][_T("PORT")];
	if (theApp.m_strLightingPort.IsEmpty())
		theApp.m_strLightingPort = _T("6501");  // 默认端口

	//>>210422 
	theApp.m_bPGCodeUsable = ini[_T("PG")][_T("PGCODE_USABLE")];
	theApp.m_strMesAdapterPort = ini[_T("DATA")][_T("MESADAPTER_PORT")] << _T("51000");
	//<<

	theApp.m_strSameDefectCode = ini[_T("VISION")][_T("DEFECT_CODE")];
	theApp.m_strSameDefectMaxCount = ini[_T("VISION")][_T("MAX_COUNT")];
	theApp.m_strSameDefectAlarmMaxCount = ini[_T("VISION")][_T("ALARM_COUNT")];
	theApp.m_bSameDefectChCheckMode = ini[_T("VISION")][_T("CH_CHECK_MODE")];
	theApp.m_bSameDefectMode = ini[_T("VISION")][_T("SAME_DEFECT_MODE")];

	for (int ii = 0; ii < _ttoi(theApp.m_strAlignCount); ii++)
		theApp.m_iAlignInspectType[ii] = ini[_T("DATA")][CStringSupport::FormatString(_T("ALIGN_TYPE_%d"), ii + 1)];

	theApp.m_CurrentLoginUser.m_strLogintTime = ini[_T("UserLogin")][_T("LogintTime")];
	theApp.m_CurrentLoginUser.m_strLevel = ini[_T("UserLogin")][_T("Level")];
	theApp.m_CurrentLoginUser.m_strUserID = ini[_T("UserLogin")][_T("UserID")];
	theApp.m_CurrentLoginUser.m_strUserPassWord = ini[_T("UserLogin")][_T("UserPassWord")];
	theApp.m_CurrentLoginUser.m_strIDCardNo = ini[_T("UserLogin")][_T("IDCardNo")];
	theApp.m_CurrentLoginUser.m_strDivision = ini[_T("UserLogin")][_T("Division")];
	theApp.m_CurrentLoginUser.m_strUserName = ini[_T("UserLogin")][_T("UserName")];
}

void CAni_Data_Serever_PCApp::IDCardReaderUserHistory()
{
	CStdioFile sFile;
	CString strTemp, strCurPos, strClear;
	int iCount = 0;
	if (sFile.Open(LOG_USER_HISTORY_PATH + _T("History"), CFile::modeNoInherit | CFile::modeRead))
	{
		while (sFile.ReadString(strTemp))
		{
			IDCardReader UserData;
			CStringArray responseTokens;
			CStringSupport::GetTokenArray(strTemp, _T(','), responseTokens);

			UserData.m_strLogintTime = responseTokens[0];
			UserData.m_strLevel = responseTokens[1];
			UserData.m_strIDCardNo = responseTokens[2];
			UserData.m_strUserID = responseTokens[3];
			UserData.m_strUserName = responseTokens[4];
			UserData.m_strDivision = responseTokens[5];
			UserData.m_strLoginOut = responseTokens[6];
			UserData.m_iNum = iCount;

			theApp.m_LoginOutData.insert(theApp.m_LoginOutData.begin(), UserData);
			iCount++;
		}

		sFile.Close();
	}
}

void CAni_Data_Serever_PCApp::GetTactParameter()
{
	if (theApp.m_strMachineType == _T(""))
		return;

	CString strTemp, tactName;
	TactTimeName tactTiemNm;
	std::vector<CString> listOfKeyNames;
	EZIni ini(DATA_SYSTEM_TACT_NAME);
	ini[theApp.m_strMachineType].EnumKeyNames(listOfKeyNames);

	for (auto list : listOfKeyNames)
	{
		tactTiemNm.m_strTactTimeName = ini[theApp.m_strMachineType][list];
		tactTiemNm.m_iTactTimeNum = _ttoi(list);
		theApp.m_vecTactName.push_back(tactTiemNm);
	}
}

void CAni_Data_Serever_PCApp::LoginCheckMethod()
{
	CAni_Data_Serever_PCApp* pApp = (CAni_Data_Serever_PCApp*)::AfxGetApp();
	CMainFrame* pMainFrame = (CMainFrame*)pApp->GetMainWnd();
	pMainFrame->m_cViewCtrl.SetTestButtonVisible();
}

void CAni_Data_Serever_PCApp::GetAlarmCount()
{
	CString strTemp, strTemp2;
	AlarmDataItem alarmData;
	CStringArray responseTokens;

	std::vector<CString> listOfKeyNames;
	for (int i = 0; i < eNumShift; i++)
	{
		strTemp.Format(_T("%s_%s_AlarmCount.ini"), theApp.m_strCurrentToday, ShiftDY_NT[i]);
		EZIni ini(DATA_ALARM_COUNT_PATH + strTemp);
		ini[_T("ALARM_COUNT")].EnumKeyNames(listOfKeyNames);

		for (auto list : listOfKeyNames )
		{
			strTemp2 = ini[_T("ALARM_COUNT")][list];
			
			CStringSupport::GetTokenArray(strTemp2, _T('^'), responseTokens);
			alarmData.m_alarmCode = list;
			alarmData.m_alarmMsg = responseTokens[0];
			alarmData.m_alarmCount = _ttoi(responseTokens[1]);
			theApp.m_AlarmRankCount[i].insert(make_pair(alarmData.m_alarmCode, alarmData));
			responseTokens.RemoveAll();
		}
		listOfKeyNames.clear();
	}
}

void CAni_Data_Serever_PCApp::IDCardReaderLoad()
{
	CStringArray responseTokens;
	IDCardReader cardReader;
	CStdioFile sFile;
	CString strFilename = DATA_SYSTEM_PATH + _T("AUTH.CSV");
	CString strInfo;
	int iCount = 0;

	if (sFile.Open(strFilename, CFile::modeRead) == FALSE)
		return;


	while (sFile.ReadString(strInfo))
	{
		if (strInfo.Find(_T("Name")) != -1)
		{
			while (sFile.ReadString(strInfo))
			{
				responseTokens.RemoveAll();
				cardReader.Reset();
				CStringSupport::GetTokenArray(strInfo, _T(','), responseTokens);
				cardReader.m_strLevel = responseTokens[0];
				cardReader.m_strUserID = responseTokens[1];
				cardReader.m_strUserPassWord = responseTokens[2];
				cardReader.m_strIDCardNo = responseTokens[3];
				cardReader.m_strDivision = responseTokens[4];
				cardReader.m_strUserName = responseTokens[5];
				cardReader.m_iNum = iCount;
				m_VecIDCardReader.push_back(cardReader);
				iCount++;
			}
		}
	}

	sFile.Close();
}

void CAni_Data_Serever_PCApp::CurrenrUserSave()
{
	EZIni ini(DATA_SYSTEM_DATA_PATH);

	ini[_T("UserLogin")][_T("LogintTime")] = theApp.m_CurrentLoginUser.m_strLogintTime;
	ini[_T("UserLogin")][_T("Level")] = theApp.m_CurrentLoginUser.m_strLevel;
	ini[_T("UserLogin")][_T("UserID")] = theApp.m_CurrentLoginUser.m_strUserID;
	ini[_T("UserLogin")][_T("UserPassWord")] = theApp.m_CurrentLoginUser.m_strUserPassWord;
	ini[_T("UserLogin")][_T("IDCardNo")] = theApp.m_CurrentLoginUser.m_strIDCardNo;
	ini[_T("UserLogin")][_T("Division")] = theApp.m_CurrentLoginUser.m_strDivision;
	ini[_T("UserLogin")][_T("UserName")] = theApp.m_CurrentLoginUser.m_strUserName;
	
}

CString CAni_Data_Serever_PCApp::GetProcessID(CString strPanel)
{
	CString strProcessID = _T(""), strFilePath = _T(""), strShift;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strFilePath.Format(_T("%s\\%s\\%s_%s\\%s.ini"), DATA_SYSTEM_DATA_SUM_PATH, _T("AOI"), theApp.m_strCurrentToday, strShift, strPanel);
	EZIni ini(strFilePath);
	strProcessID = ini[_T("JOB_DATA")][_T("Process_ID")];

	if (strProcessID.IsEmpty())
		strProcessID = _T("1L00");

	return strProcessID;
}

CString CAni_Data_Serever_PCApp::GetProjectID(CString strPanelID)
{
	CString strProjectID = _T(""), strFilePath = _T(""), strShift;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strFilePath.Format(_T("%s\\%s_%s\\%s.ini"), DATA_SYSTEM_DATA_SUM_PATH, theApp.m_strCurrentToday, strShift, strPanelID);
	EZIni ini(strFilePath);
	strProjectID = ini[_T("JOB_DATA")][_T("Product_ID")];

	if (strProjectID.IsEmpty())
		strProjectID = _T("");

	return strProjectID;
}

CString CAni_Data_Serever_PCApp::AlarmTimeParsing(CString strEndTime, CString strStartTime)
{
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strEndTime, _T(':'), responseTokens);

	int iEndTemp1, iEndTemp2, iEndTemp3, iEndSum;
	iEndTemp1 = _ttoi(responseTokens[0]) * 3600;
	iEndTemp2 = _ttoi(responseTokens[1]) * 60;
	iEndTemp3 = _ttoi(responseTokens[2]);
	iEndSum = iEndTemp1 + iEndTemp2 + iEndTemp3;

	responseTokens.RemoveAll();

	CStringSupport::GetTokenArray(strStartTime, _T(':'), responseTokens);

	int iStartTemp1, iStartTemp2, iStartTemp3, iStartSum;
	iStartTemp1 = _ttoi(responseTokens[0]) * 3600;
	iStartTemp2 = _ttoi(responseTokens[1]) * 60;
	iStartTemp3 = _ttoi(responseTokens[2]);
	iStartSum = iStartTemp1 + iStartTemp2 + iStartTemp3;

	if (iStartSum > iEndSum)
		iStartSum -= 86400; // 3600 * 24

	CString strEnd = CStringSupport::FormatString(_T("%d"), iEndSum);
	CString	strStart = CStringSupport::FormatString(_T("%d"), iStartSum);

	int iTime, iHour, iMinute, iSecond;
	iTime = _ttoi(CStringSupport::FormatString(_T("%d"), (_ttoi(strEnd) - _ttoi(strStart))));
	iHour = iTime / 3600;
	iMinute = (iTime % 3600) / 60;
	iSecond = (iTime % 3600) % 60;

	CString strTime = CStringSupport::FormatString(_T("%d:%d:%d"), iHour, iMinute, iSecond);

	return strTime;
}

void CAni_Data_Serever_PCApp::PmModeIDCardReaderLoad()
{
	EZIni ini(DATA_SYSTEM_PM_MOCDE_USER_DATA_PATH);

	for (int ii = 0; ii < 5; ii++)
	{
		theApp.m_PmModeLoginUser[ii].m_strLogintTime = ini[_T("UserLogin")][CStringSupport::FormatString(_T("LogintTime_%d"), ii)];
		theApp.m_PmModeLoginUser[ii].m_strLevel = ini[_T("UserLogin")][CStringSupport::FormatString(_T("Level_%d"), ii)];
		theApp.m_PmModeLoginUser[ii].m_strUserID = ini[_T("UserLogin")][CStringSupport::FormatString(_T("UserID_%d"), ii)];
		theApp.m_PmModeLoginUser[ii].m_strUserPassWord = ini[_T("UserLogin")][CStringSupport::FormatString(_T("UserPassWord_%d"), ii)];
		theApp.m_PmModeLoginUser[ii].m_strIDCardNo = ini[_T("UserLogin")][CStringSupport::FormatString(_T("IDCardNo_%d"), ii)];
		theApp.m_PmModeLoginUser[ii].m_strDivision = ini[_T("UserLogin")][CStringSupport::FormatString(_T("Division_%d"), ii)];
		theApp.m_PmModeLoginUser[ii].m_strUserName = ini[_T("UserLogin")][CStringSupport::FormatString(_T("UserName_%d"), ii)];
		theApp.m_PmModeLoginUser[ii].m_bLoginFlag = ini[_T("UserLogin")][CStringSupport::FormatString(_T("LoginFlag_%d"), ii)];
		theApp.m_PmModeLoginUser[ii].m_bIdSerarchFlag = ini[_T("UserLogin")][CStringSupport::FormatString(_T("IdSerarchFlag_%d"), ii)];
	}
}

void CAni_Data_Serever_PCApp::PmModeIDCardReaderSave()
{
	EZIni ini(DATA_SYSTEM_PM_MOCDE_USER_DATA_PATH);

	for (int ii = 0; ii < 5; ii++)
	{
		ini[_T("UserLogin")][CStringSupport::FormatString(_T("LogintTime_%d"), ii)] = theApp.m_PmModeLoginUser[ii].m_strLogintTime;
		ini[_T("UserLogin")][CStringSupport::FormatString(_T("Level_%d"), ii)] = theApp.m_PmModeLoginUser[ii].m_strLevel;
		ini[_T("UserLogin")][CStringSupport::FormatString(_T("UserID_%d"), ii)] = theApp.m_PmModeLoginUser[ii].m_strUserID;
		ini[_T("UserLogin")][CStringSupport::FormatString(_T("UserPassWord_%d"), ii)] = theApp.m_PmModeLoginUser[ii].m_strUserPassWord;
		ini[_T("UserLogin")][CStringSupport::FormatString(_T("IDCardNo_%d"), ii)] = theApp.m_PmModeLoginUser[ii].m_strIDCardNo;
		ini[_T("UserLogin")][CStringSupport::FormatString(_T("Division_%d"), ii)] = theApp.m_PmModeLoginUser[ii].m_strDivision;
		ini[_T("UserLogin")][CStringSupport::FormatString(_T("UserName_%d"), ii)] = theApp.m_PmModeLoginUser[ii].m_strUserName;
		ini[_T("UserLogin")][CStringSupport::FormatString(_T("LoginFlag_%d"), ii)] = theApp.m_PmModeLoginUser[ii].m_bLoginFlag;
		ini[_T("UserLogin")][CStringSupport::FormatString(_T("IdSerarchFlag_%d"), ii)] = theApp.m_PmModeLoginUser[ii].m_bIdSerarchFlag;
	}
}
#if _SYSTEM_AMTAFT_
void CAni_Data_Serever_PCApp::LoadRank()
{
	CStringArray responseTokens;
	CString strCodeInfo, strCode, strDescribe;
	EZIni ini(DATA_SYSTEM_SET_NGRANK_PATH);
	std::vector<CString> listOfKeyNames;

	for (int ii = 0; ii < RankListCount; ii++)
	{
		ini[RankIniTital[ii]].EnumKeyNames(listOfKeyNames);

		for (auto list : listOfKeyNames)
		{
			strCodeInfo = ini[RankIniTital[ii]][list];
			if (strCodeInfo.IsEmpty() == FALSE)
			{
				if (strCodeInfo.Find(_T("^")) != -1)
				{
					responseTokens.RemoveAll();
					CStringSupport::GetTokenArray(strCodeInfo, _T('^'), responseTokens);
					strCode = responseTokens[0];
					strDescribe = responseTokens[1];

					theApp.m_strVecRank[ii].push_back(make_pair(strCode, strDescribe));
				}
				else
					theApp.m_strVecRank[ii].push_back(make_pair(strCodeInfo, _T("")));
			}
		}
	}

	theApp.m_iNumberSendToPlc = ini[_T("SYSTEM")][_T("PlcSendNumber")];
}

void CAni_Data_Serever_PCApp::PGDfsInfoSave(PGDfsList PgList)
{
	CString strPath, strFilePath, strShift, strCodeGrade;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strPath.Format(_T("%s\\%s_%s"), DATA_PG_DFS_INFO_PATH, theApp.m_strCurrentToday, strShift);
	CreateFolders(strPath);

	strFilePath.Format(_T("%s\\%s.ini"), strPath, PgList.strPanelID);//CelliD
	EZIni ini(strFilePath);

	ini[_T("PG_DFS")][_T("PANEL_ID")] = PgList.strPanelID;
	ini[_T("PG_DFS")][_T("FPCB_ID")] = PgList.strFpcID;
	ini[_T("PG_DFS")][_T("VBAT")] = PgList.m_strVBIT;
	ini[_T("PG_DFS")][_T("VDDI")] = PgList.m_strVDDI;
	ini[_T("PG_DFS")][_T("VCI")] = PgList.m_strVCI;
	ini[_T("PG_DFS")][_T("PROGRAM_VERSION")] = PgList.m_strProgramVersion;
}
#else
void CAni_Data_Serever_PCApp::GammaDfsInfoSave(PGDfsList PgList)
{
	CString strPath, strFilePath, strShift, strCodeGrade;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	strPath.Format(_T("%s\\%s_%s"), DATA_GAMMA_DFS_INFO_PATH, theApp.m_strCurrentToday, strShift);
	CreateFolders(strPath);

	strFilePath.Format(_T("%s\\%s.ini"), strPath, PgList.strFpcID);
	EZIni ini(strFilePath);

	ini[_T("GAMMA_DFS")][_T("VBAT")] = PgList.m_strVBIT;
	ini[_T("GAMMA_DFS")][_T("VDDI")] = PgList.m_strVDDI;
	ini[_T("GAMMA_DFS")][_T("VCI")] = PgList.m_strVCI;
	ini[_T("GAMMA_DFS")][_T("PROGRAM_VERSION")] = PgList.m_strProgramVersion;
}
#endif

void CAni_Data_Serever_PCApp::ExcelFileSave(CString strFileName, CString strTitle, CString strValue)
{
	setlocale(LC_ALL, "Chinese");
	CFile   File;
	CString strString;
	BOOL bOpen = FALSE;
	if (!File.Open(strFileName, CFile::modeReadWrite | CFile::shareDenyNone))
	{
		if (File.Open(strFileName, CFile::modeCreate | CFile::modeWrite))
		{
			bOpen = TRUE;

			strTitle += "\r\n";
			File.Write(strTitle.GetBuffer(), strTitle.GetLength() * 2);
			strTitle.ReleaseBuffer();
		}

	}
	else bOpen = TRUE;

	if (bOpen){
		CString strTime, strDefectString = _T("");
		CTime cTime;

		cTime = CTime::GetCurrentTime();
		File.SeekToEnd();
		strValue += "\r\n";

		int iLen = strValue.GetLength();
		File.Write(strValue.GetBuffer(), iLen * 2);
		strValue.ReleaseBuffer();
		File.Close();
	}
}

void CAni_Data_Serever_PCApp::ThreadCreateDelete(BOOL bdelete, int OldAlignCnt)
{
	int iAlignTypeNum[MaxAlignInspectType] = { 0, 0, 0, 0 };
	if (bdelete){
		for (int ii = 0; ii < OldAlignCnt; ii++)
		{
			theApp.m_AlignThread[ii]->CloseTask();
			delete theApp.m_AlignThread[ii];
			theApp.m_AlignSocketManager[ii]->CloseComm();
			delete theApp.m_AlignSocketManager[ii];
		}
		theApp.m_AlignThread.clear();
		theApp.m_AlignSocketManager.clear();
	}

	theApp.m_AlignThread.resize(_ttoi(theApp.m_strAlignCount));
	theApp.m_AlignSocketManager.resize(_ttoi(theApp.m_strAlignCount));
	theApp.m_AlignPCStatus.resize(_ttoi(theApp.m_strAlignCount));
	for (int i = 0; i < _ttoi(theApp.m_strAlignCount); i++)
	{
		theApp.m_AlignPCStatus[i] = FALSE;
		CAlignThread* pAlignThread;
		CAlignManager* pAlignManager;
		switch (theApp.m_iAlignInspectType[i])
		{
		case PatternAlign:
			pAlignThread = new CAlignThread(PatternAlign, iAlignTypeNum[PatternAlign], i);
			pAlignManager = new CAlignManager(PatternAlign, iAlignTypeNum[PatternAlign], i);
			pAlignThread->CreateTask();
			iAlignTypeNum[PatternAlign]++;
			break;
		case TrayCheck:
			pAlignThread = new CAlignThread(TrayCheck, iAlignTypeNum[TrayCheck], i);
			pAlignManager = new CAlignManager(TrayCheck, iAlignTypeNum[TrayCheck], i);
			pAlignThread->CreateTask();
			iAlignTypeNum[TrayCheck]++;
			break;
		case TrayAlign:
			pAlignThread = new CAlignThread(TrayAlign, iAlignTypeNum[TrayAlign], i);
			pAlignManager = new CAlignManager(TrayAlign, iAlignTypeNum[TrayAlign], i);
			pAlignThread->CreateTask();
			iAlignTypeNum[TrayAlign]++;
			break;
		case TrayLowerAlign:
			pAlignThread = new CAlignThread(TrayLowerAlign, iAlignTypeNum[TrayLowerAlign], i);
			pAlignManager = new CAlignManager(TrayLowerAlign, iAlignTypeNum[TrayLowerAlign], i);
			pAlignThread->CreateTask();
			iAlignTypeNum[TrayLowerAlign]++;
			break;
		}
		memmove(&theApp.m_AlignThread[i], &pAlignThread, sizeof(pAlignThread));
		memmove(&theApp.m_AlignSocketManager[i], &pAlignManager, sizeof(pAlignManager));
		if (bdelete)
			theApp.m_AlignSocketManager[i]->SocketServerOpen(ALIGN_PORT_NUM[AlignCount_1 + i]);
	}
}

// ILightingEventHandler 接口实现
void CAni_Data_Serever_PCApp::OnLightingRunning()
{
	// 点灯检开始 Running
	theApp.m_pLightingLog->LOG_INFO(_T("Lighting inspection started (Running)"));
	m_csLightingFlow.Lock();
	m_bLightingRunning = TRUE;
	m_csLightingFlow.Unlock();
}

void CAni_Data_Serever_PCApp::OnLightingSnapFN()
{
	// 点灯采图完成 SnapFN，产品可以移动
	theApp.m_pLightingLog->LOG_INFO(_T("Lighting snap completed (SnapFN), product can move"));

	m_csLightingFlow.Lock();
	m_bLightingSnapDone = TRUE;
	BOOL active[4] = { m_bLightingActiveSlot[0], m_bLightingActiveSlot[1], m_bLightingActiveSlot[2], m_bLightingActiveSlot[3] };
	m_csLightingFlow.Unlock();

	// SnapFN -> GrabEnd：对当前周期内的治具位写 GrabEnd
	for (int i = 0; i < 4; ++i)
	{
		if (active[i])
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopGrabEnd1 + i, OffSet_0, TRUE);
	}
}

void CAni_Data_Serever_PCApp::OnLightingResult(const int resultCode[4])
{
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
		_T("Lighting inspection completed, results: [%02d][%02d][%02d][%02d]"),
		resultCode[0], resultCode[1], resultCode[2], resultCode[3]));

	// TODO: 根据resultCode和治具号，从数据库读取检测结果
	// 并更新 IVS_LCD_InspectionResult 和 IVS_LCD_AOIDefect 表
	// 同时写入PLC和DFS文件
	// 先补齐 PLC 流程闭环：FN$...@ 到达时，置 End，并写入一个“临时默认结果”
	// - FN$ 的数字为“完成的治具号”，不是 OK/NG；OK/NG 需要查 DB（后续可接入）
	// - 为保证 PLC 不被卡住：这里默认写 OK，并打日志提示（如果要更严谨，可改成 Timeout/NG 并报警）
	BOOL active[4] = { FALSE, FALSE, FALSE, FALSE };
	m_csLightingFlow.Lock();
	for (int i = 0; i < 4; ++i) active[i] = m_bLightingActiveSlot[i];
	m_csLightingFlow.Unlock();

	for (int i = 0; i < 4; ++i)
	{
		const int fixtureNo = resultCode[i]; // 1..4, empty = 0
		if (fixtureNo <= 0)
			continue;

		const int slotIdx = fixtureNo - 1;
		if (slotIdx < 0 || slotIdx >= 4)
			continue;

		if (!active[slotIdx])
		{
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("Lighting result fixtureNo=%d but slot not active (ignored)"), fixtureNo));
			continue;
		}
		CString uniqueID, screenID, markID;
		if (QueryIdMapByFixtureNo(fixtureNo, uniqueID, screenID, markID))
		{
			LightingInspectionResult inspResult = QueryInspectionResult(uniqueID);
			if (inspResult.m_bValid)
			{
				// 保存检测结果到缓存
				m_LightingInspResult[slotIdx] = inspResult;
				m_LightingInspResult[slotIdx].m_strUniqueID = uniqueID;

				// 根据检测结果写入 PLC
				// AOIResult: OK=良品, NG=异常, BrightDot/BlackDot/Line/Mura/Block/BM=各类缺陷
				USHORT plcResult = 0;
				if (inspResult.m_strAOIResult.CompareNoCase(_T("OK")) == 0)
				{
					plcResult = m_codeOk;  // OK
				}
				else
				{
					plcResult = m_codeNg;  // NG
				}

				// 写入检测结果到 PLC
				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_PreGammaResult1 + slotIdx, &plcResult);

				// ========== DFS 数据整合与上传 ==========
				DfsDataValue dfsData;
				dfsData.Reset();
				dfsData.m_FpcID = screenID;         // Barcode（产品码）
				dfsData.m_PanelID = screenID;       // PanelID 同 Barcode
				dfsData.m_StartTime = inspResult.m_strStartTime;
				dfsData.m_EndTime = inspResult.m_strStopTime;
				dfsData.m_ModelID = _T("");          // 配方名（如有需要可从 PLC 读取）
				dfsData.m_IndexNum = markID;         // 治具号 "01"~"04"
				dfsData.m_ChNum.Format(_T("%d"), slotIdx);

				// 点灯结果：OK/NG/BrightDot/BlackDot/Line/Mura/Block/BM
				// 转换为 DFS 格式：OK=OK, 其他=NG
				CString strLumitopResult = inspResult.m_strAOIResult;
				if (strLumitopResult.CompareNoCase(_T("OK")) == 0)
					dfsData.m_Lumitop = _T("OK");
				else
					dfsData.m_Lumitop = _T("NG");
				dfsData.m_TypeNum = Machine_Lumitop;
				dfsData.m_StageNum = slotIdx + 1;
				theApp.m_pFTP->DfsAddTransferFile(dfsData);
				theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
					_T("Writing to PLC: Slot=%d, FixtureNo=%d, UniqueID=%s, Result=%s (Code=%s, Grade=%s), DFS uploaded"),
					slotIdx, fixtureNo, uniqueID, inspResult.m_strAOIResult, inspResult.m_strCodeAOI, inspResult.m_strGradeAOI));
			}
			else
			{
				theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
					_T("Failed to query inspection result for UniqueID=%s, writing default result"), uniqueID));

				USHORT tmpResult = m_codeOk;  // 默认写 OK，避免 PLC 卡住
				theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_PreGammaResult1 + slotIdx, &tmpResult);
			}
		}
		else
		{
			// 查询不到映射关系，写入默认值
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("Failed to query ID map for FixtureNo=%d, writing default result"), fixtureNo));
			USHORT tmpResult = m_codeOk;  // 默认写 OK，避免 PLC 卡住
			theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_PreGammaResult1 + slotIdx, &tmpResult);
		}
	}

	m_csLightingFlow.Lock();
	m_bLightingCycleInProgress = FALSE;
	m_bLightingRunning = FALSE;
	m_bLightingSnapDone = FALSE;
	for (int i = 0; i < 4; ++i) m_bLightingActiveSlot[i] = FALSE;
	m_dwLightingStartTick = 0;
	m_csLightingFlow.Unlock();
}

BOOL CAni_Data_Serever_PCApp::TryStartLightingFromPlc(const BOOL startFlags[4])
{
	if (!m_LightingThreadOpenFlag || !m_LightingConectStatus)
		return FALSE;

	m_csLightingFlow.Lock();
	if (m_bLightingCycleInProgress)
	{
		m_csLightingFlow.Unlock();
		theApp.m_pLightingLog->LOG_INFO(_T("Lighting Start requested but previous cycle still in progress (ignored)"));
		return FALSE;
	}

	BOOL any = FALSE;
	for (int i = 0; i < 4; ++i)
	{
		m_bLightingActiveSlot[i] = (startFlags[i] == TRUE);
		if (m_bLightingActiveSlot[i]) any = TRUE;
	}

	if (!any)
	{
		m_csLightingFlow.Unlock();
		return FALSE;
	}

	m_bLightingCycleInProgress = TRUE;
	m_bLightingRunning = FALSE;
	m_bLightingSnapDone = FALSE;
	m_dwLightingStartTick = ::GetTickCount();
	m_csLightingFlow.Unlock();

	// 发送开始检测前，更新 ivs_lcd_idmap 对应治具号记录，供检测软件使用
	if (m_bLightingDBConnected || ConnectLightingDatabase())
	{
		PanelData pPanelData;
		FpcIDData pFpcData;

		for (int i = 0; i < 4; ++i)
		{
			if (!startFlags[i])
				continue;

			int fixtureNo = i + 1;  // 治具号 1~4
			theApp.m_pEqIf->m_pMNetH->GetPanelData(eWordType_PreGammaPanel1 + i, &pPanelData);
			theApp.m_pEqIf->m_pMNetH->GetFpcIdData(eWordType_PreGammaFpcID1 + i, &pFpcData);

			CString strPanelID = CStringSupport::ToWString(pPanelData.m_PanelData, sizeof(pPanelData.m_PanelData));
			CString strFpcID = CStringSupport::ToWString(pFpcData.m_FpcIDData, sizeof(pFpcData.m_FpcIDData));
			strPanelID.Trim();
			strFpcID.Trim();
			if (strPanelID.IsEmpty())
				strPanelID = strFpcID;
			if (strFpcID.IsEmpty())
				strFpcID = strPanelID;

			// UniqueID 使用 GUID 保证全局唯一性
			CString strUniqueID;
			CStringSupport::GetGuid(strUniqueID);

			// Barcode=产品码（使用 FpcID 或 PanelID），MarkID=治具号 "01"~"04"
			CString strMarkID;
			strMarkID.Format(_T("%02d"), fixtureNo);
			UpdateLightingIdMap(fixtureNo, strUniqueID, strFpcID.IsEmpty() ? strPanelID : strFpcID, strMarkID);
		}
	}
	else
	{
		theApp.m_pLightingLog->LOG_INFO(_T("TryStartLightingFromPlc: DB not connected, skip idmap update"));
	}

	// Start$xxxxxxxx$xxxxxxxx@
	int usedSlots[4] = { 0,0,0,0 };
	int maxSlots[4] = { 1,2,3,4 };
	for (int i = 0; i < 4; ++i)
		usedSlots[i] = startFlags[i] ? (i + 1) : 0;

	theApp.m_LightingSocketManager.SendStart(usedSlots, maxSlots);
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
		_T("Lighting Start sent (slots=%02d%02d%02d%02d)"),
		usedSlots[0], usedSlots[1], usedSlots[2], usedSlots[3]));

	return TRUE;
}

void CAni_Data_Serever_PCApp::LightingFlowTimeoutCheck()
{
	if (!m_LightingThreadOpenFlag || !m_LightingConectStatus)
		return;

	BOOL inProgress = FALSE;
	DWORD startTick = 0;
	DWORD timeoutMs = 0;
	BOOL active[4] = { FALSE, FALSE, FALSE, FALSE };

	m_csLightingFlow.Lock();
	inProgress = m_bLightingCycleInProgress;
	startTick = m_dwLightingStartTick;
	timeoutMs = m_dwLightingTimeoutMs;
	for (int i = 0; i < 4; ++i) active[i] = m_bLightingActiveSlot[i];
	m_csLightingFlow.Unlock();

	if (!inProgress || startTick == 0 || timeoutMs == 0)
		return;

	const DWORD now = ::GetTickCount();
	if (now - startTick < timeoutMs)
		return;

	theApp.m_pLightingLog->LOG_INFO(_T("Lighting flow TIMEOUT: forcing PLC End/Result to avoid hang"));

	for (int i = 0; i < 4; ++i)
	{
		if (!active[i])
			continue;
		USHORT tmpResult = m_codeTimeOut;
		theApp.m_pEqIf->m_pMNetH->SetPlcWordData(eWordType_PreGammaResult1 + i, &tmpResult);
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopGrabEnd1 + i, OffSet_0, TRUE);
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_LumitopEnd1 + i, OffSet_0, TRUE);
	}

	m_csLightingFlow.Lock();
	m_bLightingCycleInProgress = FALSE;
	m_bLightingRunning = FALSE;
	m_bLightingSnapDone = FALSE;
	for (int i = 0; i < 4; ++i) m_bLightingActiveSlot[i] = FALSE;
	m_dwLightingStartTick = 0;
	m_csLightingFlow.Unlock();
}

// 点灯检数据库操作函数 - 使用 MySQL Connector/C++
BOOL CAni_Data_Serever_PCApp::UpdateLightingIdMap(int fixtureNo, CString uniqueID, CString screenID, CString markID)
{
	if (!m_bLightingDBConnected)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pLightingLog->LOG_INFO(_T("UpdateLightingIdMap: Database not connected"));
			return FALSE;
		}
	}

	// ivs_lcd_idmap 表结构：MarkID/MainAoiFixID=治具号('01'~'04'), UniqueID=唯一ID, Barcode=产品码
	// screenID 参数对应表字段 Barcode（产品码）, markID 对应治具号字符串如 "01"
	CString strMarkID;
	strMarkID.Format(_T("%02d"), fixtureNo);  // 1->"01", 2->"02", 3->"03", 4->"04"

	// 转义 SQL 字符串中的单引号（避免注入与语法错误）
	CString strUniqueID = uniqueID;
	strUniqueID.Replace(_T("'"), _T("''"));
	CString strBarcode = screenID;
	strBarcode.Replace(_T("'"), _T("''"));

	try {
		// 按 MarkID 更新对应治具号记录，供检测软件使用
		CString strSQL;
		strSQL.Format(_T("UPDATE ivs_lcd_idmap SET UniqueID='%s', Barcode='%s', MainAoiFixID='%s' WHERE MarkID='%s'"),
			strUniqueID, strBarcode, strMarkID, strMarkID);

		std::auto_ptr<sql::Statement> stmt(m_pLightingConn->createStatement());
		int affected = stmt->execute((std::string)CT2A(strSQL));

		if (affected >= 0)
		{
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("Updated ivs_lcd_idmap: MarkID=%s, UniqueID=%s, Barcode=%s (for detection software)"),
				strMarkID, uniqueID, screenID));
			return TRUE;
		}
		else
		{
			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("Failed to update ivs_lcd_idmap (MarkID=%s): No rows affected"), strMarkID));
			return FALSE;
		}
	}
	catch (sql::SQLException& e) {
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
			_T("Failed to update ivs_lcd_idmap (MarkID=%s): %s"), strMarkID, CString(e.what())));
		return FALSE;
	}
}

BOOL CAni_Data_Serever_PCApp::LoadLightingInspectionResult(CString uniqueID)
{
	if (!m_bLightingDBConnected)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pLightingLog->LOG_INFO(_T("LoadLightingInspectionResult: Database not connected"));
			return FALSE;
		}
	}

	LightingInspectionResult result = QueryInspectionResult(uniqueID);

	if (result.m_bValid)
	{
		// 找到对应的治具号并保存结果
		// 这里需要根据 uniqueID 找到对应的治具号
		// 暂时遍历所有治具进行匹配
		for (int i = 0; i < 4; i++)
		{
			if (m_LightingInspResult[i].m_strUniqueID == uniqueID)
			{
				m_LightingInspResult[i] = result;
				theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
					_T("Loaded inspection result: UniqueID=%s, Result=%s, Code=%s, Grade=%s"),
					uniqueID, result.m_strAOIResult, result.m_strCodeAOI, result.m_strGradeAOI));
				return TRUE;
			}
		}
		// 如果没找到对应的治具，保存到第一个空槽位
		for (int i = 0; i < 4; i++)
		{
			if (!m_LightingInspResult[i].m_bValid)
			{
				m_LightingInspResult[i] = result;
				m_LightingInspResult[i].m_strUniqueID = uniqueID;
				theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
					_T("Loaded inspection result to slot %d: UniqueID=%s, Result=%s, Code=%s, Grade=%s"),
					i, uniqueID, result.m_strAOIResult, result.m_strCodeAOI, result.m_strGradeAOI));
				return TRUE;
			}
		}
	}
	else
	{
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
			_T("No inspection result found for UniqueID=%s"), uniqueID));
	}

	return FALSE;
}

BOOL CAni_Data_Serever_PCApp::UpdateLightingInspectionResult(CString uniqueID)
{
	// 更新检测结果的状态（如果需要）
	// 根据业务需求决定是否需要更新数据库
	theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
		_T("UpdateLightingInspectionResult: UniqueID=%s"), uniqueID));
	return TRUE;
}

// MySQL 数据库初始化
BOOL CAni_Data_Serever_PCApp::InitLightingDatabase()
{
	// 从配置文件读取数据库连接信息
	// 配置文件路径: D:\ServerInfo.ini
	EZIni ini(_T("D:\\ServerInfo.ini"));

	// 读取数据库配置（根据实际配置文件调整）
	CString sTemp;
	m_strLightingDBServer = ini[_T("LightingDB")][_T("Server")]; sTemp = _T("127.0.0.1"); m_strLightingDBServer = (m_strLightingDBServer.IsEmpty()) ? sTemp : m_strLightingDBServer;
	m_strLightingDBName = ini[_T("LightingDB")][_T("Database")]; sTemp = _T("IVS_LCD"); m_strLightingDBName = (m_strLightingDBName.IsEmpty()) ? sTemp : m_strLightingDBName;
	m_strLightingDBUser = ini[_T("LightingDB")][_T("User")]; sTemp = _T("root"); m_strLightingDBUser = (m_strLightingDBUser.IsEmpty()) ? sTemp : m_strLightingDBUser;
	m_strLightingDBPassword = ini[_T("LightingDB")][_T("Password")]; sTemp = _T("password"); m_strLightingDBPassword = (m_strLightingDBPassword.IsEmpty()) ? sTemp : m_strLightingDBPassword;

	m_bLightingDBConnected = FALSE;
	m_pLightingConn = NULL;

	theApp.m_pLightingLog->LOG_INFO(_T("Lighting database environment initialized"));

	// 尝试连接数据库，失败时不阻止程序启动
	try {
		return ConnectLightingDatabase();
	}
	catch (sql::SQLException& e) {
		CString errorMsg = CStringSupport::FormatString(
			_T("Lighting database connection failed: %s"), CString(e.what()));
		theApp.m_pLightingLog->LOG_INFO(errorMsg);
		OutputDebugString(errorMsg + _T("\n"));
		return FALSE;
	}
	catch (std::exception& e) {
		CString errorMsg = CStringSupport::FormatString(
			_T("Lighting database connection failed: %s"), CString(e.what()));
		theApp.m_pLightingLog->LOG_INFO(errorMsg);
		OutputDebugString(errorMsg + _T("\n"));
		return FALSE;
	}
	catch (...) {
		theApp.m_pLightingLog->LOG_INFO(_T("Lighting database connection failed: unknown error"));
		return FALSE;
	}
}

BOOL CAni_Data_Serever_PCApp::ConnectLightingDatabase()
{
	if (m_bLightingDBConnected && m_pLightingConn != NULL)
		return TRUE;

	try {
		// 创建 MySQL Connector/C++ 连接对象
		sql::Driver* driver = get_driver_instance();
		if (!driver) {
			theApp.m_pLightingLog->LOG_INFO(_T("Failed to get MySQL driver"));
			return FALSE;
		}

		// 构建连接 URL
		CString strUrl;
		strUrl.Format(_T("tcp://%s:3306"), m_strLightingDBServer);

		sql::SQLString sql_str1("tcp://114.67.86.96:3306");
		sql::SQLString user("root");
		sql::SQLString password("Root@123456");
		// 连接数据库
		//m_pLightingConn = driver->connect((std::string)CT2A(strUrl), (std::string)CT2A(m_strLightingDBUser), (std::string)CT2A(m_strLightingDBPassword));
		m_pLightingConn = driver->connect(sql_str1, user, password);
		if (!m_pLightingConn) {
			theApp.m_pLightingLog->LOG_INFO(_T("Failed to connect to MySQL database"));
			return FALSE;
		}

		// 选择数据库
		m_pLightingConn->setSchema((std::string)CT2A(m_strLightingDBName));

		m_bLightingDBConnected = TRUE;
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
			_T("Connected to Lighting database: %s/%s"), m_strLightingDBServer, m_strLightingDBName));
		return TRUE;
	}
	catch (sql::SQLException& e) {
		CString strMsg = CStringSupport::FormatString(
			_T("Failed to connect to database: %s"), CString(e.what()));
		theApp.m_pLightingLog->LOG_INFO(strMsg);
		OutputDebugString(strMsg + _T("\n"));
		return FALSE;
	}
}

// 关闭数据库连接
void CAni_Data_Serever_PCApp::CloseLightingDatabase()
{
	if (m_pLightingConn)
	{
		delete m_pLightingConn;
		m_pLightingConn = NULL;
	}

	m_bLightingDBConnected = FALSE;
	theApp.m_pLightingLog->LOG_INFO(_T("Lighting database connection closed"));
}

LightingInspectionResult CAni_Data_Serever_PCApp::GetLightingResultByUniqueID(CString uniqueID)
{
	return QueryInspectionResult(uniqueID);
}

void CAni_Data_Serever_PCApp::GetLightingResultByBarcode(CString strBarcode, CString& strAOIResult, CString& strCodeAOI, CString& strGradeAOI, BOOL& bValid)
{
	strAOIResult = _T("");
	strCodeAOI = _T("");
	strGradeAOI = _T("");
	bValid = FALSE;

	if (!m_bLightingDBConnected)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pFTPLog->LOG_INFO(_T("GetLightingResultByBarcode: Database not connected"));
			return;
		}
	}

	// 先根据 Barcode 从 ivs_lcd_idmap 表找到 UniqueID
	CString strUniqueID;

	try {
		CString strSQL;
		strSQL.Format(_T("SELECT UniqueID FROM ivs_lcd_idmap WHERE Barcode = '%s'"), strBarcode);

		std::auto_ptr<sql::Statement> stmt(m_pLightingConn->createStatement());
		std::auto_ptr<sql::ResultSet> res(stmt->executeQuery((std::string)CT2A(strSQL)));

		if (res->next())
		{
			strUniqueID = res->getString("UniqueID").c_str();
			theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
				_T("GetLightingResultByBarcode: Found UniqueID=%s for Barcode=%s"), strUniqueID, strBarcode));
		}
		else
		{
			theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
				_T("GetLightingResultByBarcode: No UniqueID found for Barcode=%s"), strBarcode));
			return;
		}
	}
	catch (sql::SQLException& e) {
		theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
			_T("GetLightingResultByBarcode: SQL error: %s"), CString(e.what())));
		return;
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

	if (!m_bLightingDBConnected)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pLightingLog->LOG_INFO(_T("QueryLightingDefectList: Database not connected"));
			return FALSE;
		}
	}

	try {
		// 查询 IVS_LCD_AOIDefect 表
		CString strSQL;
		strSQL.Format(_T("SELECT DefectIndex, Type, PatternID, PatternName, Pos_x, Pos_y, Pos_width, Pos_height, ")
			_T("TrueSize, GrayScale, GrayScale_BK, GrayScaleDiff, Code_AOI, Grade_AOI ")
			_T("FROM IVS_LCD_AOIDefect WHERE GUID_IVS_LCD_InspectionResult = '%s' ORDER BY DefectIndex"),
			strUniqueID);

		std::auto_ptr<sql::Statement> stmt(m_pLightingConn->createStatement());
		std::auto_ptr<sql::ResultSet> res(stmt->executeQuery((std::string)CT2A(strSQL)));

		int iDefectCount = 0;
		while (res->next())
		{
			LUMITOP_SDFSDefectDataBegin defect;

			defect.strPANEL_ID = strUniqueID;
			defect.strPOINT = res->getString("DefectIndex").c_str();
			defect.strX = res->getString("Pos_x").c_str();
			defect.strY = res->getString("Pos_y").c_str();

			// PatternID 和 PatternName 组合为画面信息
			CString strPatternID = CA2W(res->getString("PatternID").c_str());
			CString strPatternName = CA2W(res->getString("PatternName").c_str());
			defect.strLUMITOP_PTRN = strPatternName.IsEmpty() ? strPatternID : strPatternName;

			// 亮度使用 GrayScale
			defect.strLV = res->getString("GrayScale").c_str();

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

		return TRUE;
	}
	catch (sql::SQLException& e) {
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
			_T("QueryLightingDefectList: SQL error: %s"), CString(e.what())));
		return FALSE;
	}
}

// DFS 模块调用：根据 UniqueID 查询 AOI 缺陷详情列表（点灯缺陷）
BOOL CAni_Data_Serever_PCApp::QueryAOIDefectList(CString strUniqueID, std::vector<SDFSDefectDataBegin>& vecDefects)
{
	vecDefects.clear();

	if (!m_bLightingDBConnected)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pLightingLog->LOG_INFO(_T("QueryAOIDefectList: Database not connected"));
			return FALSE;
		}
	}

	try {
		// 查询 IVS_LCD_AOIDefect 表
		CString strSQL;
		strSQL.Format(_T("SELECT DefectIndex, Type, PatternID, PatternName, Pos_x, Pos_y, Pos_width, Pos_height, ")
			_T("TrueSize, GrayScale, GrayScale_BK, GrayScaleDiff, Code_AOI, Grade_AOI ")
			_T("FROM IVS_LCD_AOIDefect WHERE GUID_IVS_LCD_InspectionResult = '%s' ORDER BY DefectIndex"),
			strUniqueID);

		std::auto_ptr<sql::Statement> stmt(m_pLightingConn->createStatement());
		std::auto_ptr<sql::ResultSet> res(stmt->executeQuery((std::string)CT2A(strSQL)));

		int iDefectCount = 0;
		while (res->next())
		{
			SDFSDefectDataBegin defect;

			defect.strPANEL_ID = strUniqueID;
			defect.strDEFECT_DATA_NUM = res->getString("DefectIndex").c_str();
			defect.strDEFECT_TYPE = res->getString("Type").c_str();

			// PatternID 和 PatternName 组合
			CString strPatternID = CA2W(res->getString("PatternID").c_str());
			CString strPatternName = CA2W(res->getString("PatternName").c_str());
			defect.strDEFECT_PTRN = strPatternName.IsEmpty() ? strPatternID : strPatternName;

			defect.strDEFECT_CODE = res->getString("Code_AOI").c_str();
			defect.strDEFECT_GRADE = res->getString("Grade_AOI").c_str();
			defect.strX = res->getString("Pos_x").c_str();
			defect.strY = res->getString("Pos_y").c_str();
			defect.strSIZE = res->getString("TrueSize").c_str();

			// 图像数据相关字段为空
			defect.strIMAGE_DATA = _T("");
			defect.strCAM_INSPECT = _T("");
			defect.strZone = _T("");
			defect.strInspName = _T("");

			vecDefects.push_back(defect);
			iDefectCount++;
		}

		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
			_T("QueryAOIDefectList: Found %d defects for UniqueID=%s"),
			iDefectCount, strUniqueID));

		return TRUE;
	}
	catch (sql::SQLException& e) {
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
			_T("QueryAOIDefectList: SQL error: %s"), CString(e.what())));
		return FALSE;
	}
}

// DFS 模块调用：根据 Barcode 查询 UniqueID
CString CAni_Data_Serever_PCApp::GetLightingUniqueIDByBarcode(CString strBarcode)
{
	CString strUniqueID = _T("");

	if (!m_bLightingDBConnected)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pFTPLog->LOG_INFO(_T("GetLightingUniqueIDByBarcode: Database not connected"));
			return strUniqueID;
		}
	}

	try {
		CString strSQL;
		strSQL.Format(_T("SELECT UniqueID FROM ivs_lcd_idmap WHERE Barcode = '%s'"), strBarcode);

		std::auto_ptr<sql::Statement> stmt(m_pLightingConn->createStatement());
		std::auto_ptr<sql::ResultSet> res(stmt->executeQuery((std::string)CT2A(strSQL)));

		if (res->next())
		{
			strUniqueID = res->getString("UniqueID").c_str();
		}
	}
	catch (sql::SQLException& e) {
		theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
			_T("GetLightingUniqueIDByBarcode: SQL error: %s"), CString(e.what())));
	}

	return strUniqueID;
}

// 根据治具号查询 ID 映射
BOOL CAni_Data_Serever_PCApp::QueryIdMapByFixtureNo(int fixtureNo, CString& uniqueID, CString& screenID, CString& markID)
{
	if (!m_bLightingDBConnected)
	{
		if (!ConnectLightingDatabase())
		{
			theApp.m_pLightingLog->LOG_INFO(_T("QueryIdMapByFixtureNo: Database not connected"));
			return FALSE;
		}
	}

	try {
		// 查询 ivs_lcd_idmap 表：MarkID=治具号('01'~'04'), Barcode=产品码
		CString strMarkID;
		strMarkID.Format(_T("%02d"), fixtureNo);

		CString strSQL;
		strSQL.Format(_T("SELECT UniqueID, Barcode, MarkID FROM ivs_lcd_idmap WHERE MarkID = '%s'"), strMarkID);

		std::auto_ptr<sql::Statement> stmt(m_pLightingConn->createStatement());
		std::auto_ptr<sql::ResultSet> res(stmt->executeQuery((std::string)CT2A(strSQL)));

		if (res->next())
		{
			uniqueID = res->getString("UniqueID").c_str();
			screenID = res->getString("Barcode").c_str();  // 实际为 Barcode 产品码
			markID = res->getString("MarkID").c_str();

			theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
				_T("QueryIdMapByFixtureNo: FixtureNo=%d, UniqueID=%s, Barcode=%s, MarkID=%s"),
				fixtureNo, uniqueID, screenID, markID));

			return TRUE;
		}
	}
	catch (sql::SQLException& e) {
		theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
			_T("QueryIdMapByFixtureNo: SQL error: %s"), CString(e.what())));
	}

	return FALSE;
}