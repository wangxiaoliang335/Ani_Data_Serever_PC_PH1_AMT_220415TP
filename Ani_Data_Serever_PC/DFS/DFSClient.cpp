// FTPClient.cpp: implementation of the CDFSClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DFSClient.h"
#include "DfsInfo.h"
#include "DFSInfo.h"
#include "DataInfo.h"
#include "Ani_Data_Serever_PC.h"
#include "Main\Migration.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDFSClient::CDFSClient()
{
	//CFile File;
	//File.Open(DFS_LOCAL+_T("dummy.dmy"), CFile::modeCreate);
	//File.Close();


	m_bConnectState = FALSE;
	m_pFtpConnection = NULL;
	m_pDFSSession = NULL;

	m_strCurrentDirectory = "/";
	m_strRootDirectory = "/";

	// 从配置文件读取 FTP 设置，如果读取失败则使用默认值
	EZIni ini(DATA_SYSTEM_DATA_PATH);
	CString strFtpIP = ini[_T("FTP")][_T("DFS_IP")];
	CString strFtpPort = ini[_T("FTP")][_T("DFS_PORT")];

	m_strAddress = strFtpIP.IsEmpty() ? DFS_FTP_DEFAULT_IP : strFtpIP;
	m_nPort = strFtpPort.IsEmpty() ? DFS_FTP_DEFAULT_PORT : _ttoi(strFtpPort);

	// 日志记录 FTP 配置信息
	CString strLogMsg;
	strLogMsg.Format(_T("[DFS FTP Config] ConfigFile=%s, [FTP]DFS_IP=%s (default=%s), [FTP]DFS_PORT=%s (default=%d)"),
		DATA_SYSTEM_DATA_PATH,
		strFtpIP.IsEmpty() ? _T("(empty)") : strFtpIP,
		DFS_FTP_DEFAULT_IP,
		strFtpPort.IsEmpty() ? _T("(empty)") : strFtpPort,
		DFS_FTP_DEFAULT_PORT);
	theApp.m_pFTPLog->LOG_INFO(strLogMsg);

	m_bUsePASVMode = FALSE;
	m_nRetries = 1;//3;	//0828Y13.JJH.ModifyFTP

	m_strDescription = "";
	m_strLocalPath = "";
	m_nRetryDelay = 2;//10;	//0828Y13.JJH.ModifyFTP
	m_strUserName = "";
	m_strLogin = "";
	m_strName = "";
	m_strPassword = "";
	m_strRemotePath = "";
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CDFSClient::~CDFSClient()
{
	Disconnect();
}

void CDFSClient::Connect(LPCTSTR lpszSiteName, LPCTSTR lpszUserName, LPCTSTR lpszPassword)
{
	//Default Setting Param
	// clear name!
	m_ftpSite.m_strName.Empty();

	m_strUserName = lpszUserName;
	m_strLogin = lpszUserName;
	m_strName = lpszSiteName;
	m_strPassword = lpszPassword;

	// if no site name is specified, show connect dialog
	m_ftpSite.m_bUsePASVMode = m_bUsePASVMode;
	m_ftpSite.m_nRetries = m_nRetries;
	m_ftpSite.m_nPort = m_nPort;
	m_ftpSite.m_nRetryDelay = m_nRetryDelay;
	m_ftpSite.m_strAddress = m_strAddress;
	m_ftpSite.m_strDescription = m_strDescription;
	m_ftpSite.m_strLocalPath = m_strLocalPath;
	m_ftpSite.m_strLogin = m_strLogin;
	m_ftpSite.m_strName = m_strName;
	m_ftpSite.m_strPassword = m_strPassword;
	m_ftpSite.m_strRemotePath = m_strRemotePath;

	// quick connect ?
	if (m_ftpSite.m_strName.IsEmpty())
	{
		m_ftpSite.m_bUsePASVMode = AfxGetApp()->GetProfileInt(_T("Settings"), _T("DefaultUsePASVMode"), 0);
		m_ftpSite.m_nRetries = AfxGetApp()->GetProfileInt(_T("Settings"), _T("DefaultRetries"), 3);
		m_ftpSite.m_nPort = 21;

		m_ftpSite.m_nRetryDelay = AfxGetApp()->GetProfileInt(_T("Settings"), _T("DefaultRetryDelay"), 2);
		m_ftpSite.m_strAddress = lpszSiteName;
		m_ftpSite.m_strDescription = _T("");
		m_ftpSite.m_strLocalPath = AfxGetApp()->GetProfileString(_T("Settings"), _T("DefaultLocalPath"), _T(""));
		m_ftpSite.m_strLogin = lpszUserName;
		m_ftpSite.m_strName = _T("");
		m_ftpSite.m_strPassword = lpszPassword;
		m_ftpSite.m_strRemotePath = _T("");
	}

	// fix paths the ways we like it...
	m_ftpSite.m_strLocalPath.TrimRight(_T("\\"));

	if (!m_ftpSite.m_strRemotePath.IsEmpty())
	{
		m_ftpSite.m_strRemotePath.Replace(_T("\\"), _T("/"));
		m_ftpSite.m_strRemotePath.TrimLeft('/');
		m_ftpSite.m_strAddress += _T("/");
		m_ftpSite.m_strAddress += m_ftpSite.m_strRemotePath;
	}

	CString strObject;
	INTERNET_PORT nPort;
	DWORD dwServiceType;

	// if there's already a connection open, close it first
	if (m_pFtpConnection != NULL)
	{
		m_pFtpConnection->Close();

		delete m_pFtpConnection;
		m_pFtpConnection = NULL;
	}

	if (m_pDFSSession != NULL)
	{
		m_pDFSSession->Close();
		delete m_pDFSSession;

		m_pDFSSession = NULL;
	}

	CString str;
	if (!str.LoadString(AFX_IDS_APP_TITLE))
		str = "AppUnknown";

	m_pDFSSession = new CDFSSession(str, 1, PRE_CONFIG_INTERNET_ACCESS);

	// Alert the user if the internet session could
	// not be started and close app
	if (!m_pDFSSession)
	{
		return;
	}

	m_pDFSSession->m_pMainWnd = NULL; //¾²Áö ¾ÊÀ¸¸®...
	m_pDFSSession->EnableStatusCallback();
	// check to see if this is a reasonable URL --
	// ie "ftp://servername/dirs" or just "servername/dirs"


	if (!AfxParseURL(m_ftpSite.m_strAddress, dwServiceType, m_strServerName, strObject, nPort))
	{
		// try adding the "ftp://" protocol
		CString strFtpURL = _T("ftp://");
		strFtpURL += m_ftpSite.m_strAddress;

		if (!AfxParseURL(strFtpURL, dwServiceType, m_strServerName, strObject, nPort))
		{
			AfxMessageBox(_T("URL IS NOT GOOD!"), MB_OK);
			return;
		}
	}

	//CWaitCursor cursor; // this will automatically display a wait cursor

	// Now open a FTP connection to the server
	if ((dwServiceType == INTERNET_SERVICE_FTP) && !m_strServerName.IsEmpty())
	{
		int nRetries = m_ftpSite.m_nRetries;
		while (nRetries > 0)
		{
			Delay(5, TRUE); //170205 KM
			nRetries--;
			try
			{
				int nTimeout = AfxGetApp()->GetProfileInt(_T("Settings"), _T("ConnectionTimeout"), 3);
				m_pDFSSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeout * 1000);
				m_pDFSSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, nTimeout * 1000);
				m_pDFSSession->SetOption(INTERNET_OPTION_SEND_TIMEOUT, nTimeout * 1000);

				m_pFtpConnection = m_pDFSSession->GetFtpConnection(m_strServerName, m_ftpSite.m_strLogin, m_ftpSite.m_strPassword, m_ftpSite.m_nPort, m_ftpSite.m_bUsePASVMode);
				nRetries = 0;
			}
			catch (CInternetException* pEx)
			{
				DWORD dwLength = 255;//, dwError;

				CString strLastResponce;
				//				InternetGetLastResponseInfo(&dwError, strLastResponce.GetBuffer(dwLength), &dwLength);
				strLastResponce.ReleaseBuffer();
				strLastResponce.Remove('\n');
				strLastResponce.Remove('\r');

				// catch errors from WinINet
				dwLength = 255;
				CString strInfo;
				if (pEx->GetErrorMessage(strInfo.GetBuffer(dwLength), dwLength))
				{
					strInfo.ReleaseBuffer();
					strInfo.Remove('\n');
					strInfo.Remove('\r');

					// show wait dialog
					if (nRetries > 0)
					{
						Delay(m_ftpSite.m_nRetryDelay, TRUE); //<<150818 JYLee
					}
					else
					{
						if (!strLastResponce.IsEmpty())
						{
							strLastResponce += _T("\r\n");
						}

						strLastResponce += strInfo;
						//AfxMessageBox(strLastResponce, MB_OK);
					}
				}
				else
				{
					nRetries = 0;
					//AfxMessageBox(_T("Connect Fail!"), MB_OK);
				}
				pEx->Delete();
				pEx = NULL;

				delete m_pFtpConnection;
				m_pFtpConnection = NULL;
				delete m_pDFSSession;
				m_pDFSSession = NULL;
			}
		}
	}
	else
	{
		AfxMessageBox(_T("INVALID_URL"), MB_OK);
	}


	// PopulateTree() will display an error if the FTP connection
	// could not be made, otherwise, it grabs the root listing
	// and expands any folder indicated by the site name

	if (m_pFtpConnection != NULL)
	{
		// try to set remote directory
		if (m_pFtpConnection->SetCurrentDirectory(strObject))
		{
			m_pFtpConnection->GetCurrentDirectory(m_strRootDirectory);
		}

		//if (!m_ftpSite.m_strName.IsEmpty())
		//	AfxGetApp()->AddToRecentFileList(m_ftpSite.m_strName);

		m_bConnectState = TRUE;
	}
	else
	{
		m_bConnectState = FALSE;
		// 		GetTraceView()->AddTraceLine(3, _T("[%d] FTP connection could not be made."), AfxGetThread()->m_nThreadID);
		// 		PopulateTree();
	}

	if (m_pFtpConnection){
		m_pFtpConnection->Close();
		delete m_pFtpConnection;
		m_pFtpConnection = NULL;
	}
}

void CDFSClient::UploadFile(CString &source, CString &dest, BOOL &bSend)
{
	
	bSend = FALSE;
	if (!m_pDFSSession) return;
	BOOL bCreateDirectory = FALSE;

	// Create new ftp connection to retrieve file
	int nRetries = m_ftpSite.m_nRetries;
	while (nRetries > 0)
	{
		Delay(5, TRUE); //170205 KM
		nRetries--;
		try
		{
			int nTimeout = AfxGetApp()->GetProfileInt(_T("Settings"), _T("ConnectionTimeout"), 3);
			m_pDFSSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeout * 1000);
			m_pDFSSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, nTimeout * 1000);
			m_pDFSSession->SetOption(INTERNET_OPTION_SEND_TIMEOUT, nTimeout * 1000);

			m_pFtpConnection = m_pDFSSession->GetFtpConnection(m_strServerName, m_ftpSite.m_strLogin, m_ftpSite.m_strPassword, m_ftpSite.m_nPort, m_ftpSite.m_bUsePASVMode);			
			nRetries = 0;
		}
		catch (CInternetException* pEx)
		{
			DWORD dwLength = 255;//, dwError;

			CString strLastResponce;
			//				InternetGetLastResponseInfo(&dwError, strLastResponce.GetBuffer(dwLength), &dwLength);
			strLastResponce.ReleaseBuffer();
			strLastResponce.Remove('\n');
			strLastResponce.Remove('\r');

			// catch errors from WinINet
			dwLength = 255;
			CString strInfo;
			if (pEx->GetErrorMessage(strInfo.GetBuffer(dwLength), dwLength))
			{
				strInfo.ReleaseBuffer();
				strInfo.Remove('\n');
				strInfo.Remove('\r');

				// show wait dialog
				if (nRetries > 0)
				{
					Delay(m_ftpSite.m_nRetryDelay, TRUE); //<<150818 JYLee
				}
				else
				{
					if (!strLastResponce.IsEmpty())
					{
						strLastResponce += _T("\r\n");
					}

					strLastResponce += strInfo;
					//AfxMessageBox(strLastResponce, MB_OK);
				}
			}
			else
			{
				nRetries = 0;
				//AfxMessageBox(_T("Connect Fail!"), MB_OK);
			}
			pEx->Delete();
			pEx = NULL;

			delete m_pFtpConnection;
			m_pFtpConnection = NULL;
			delete m_pDFSSession;
			m_pDFSSession = NULL;
			return;
		}
	}

	// 	if (!m_pFtpConnection){
	// 		delete m_pFtpConnection;
	// 		m_pFtpConnection = NULL;
	// 		delete m_pDFSSession;
	// 		m_pDFSSession= NULL;
	// 		return;
	// 	}
	//>> 140607 JSLee
	CString strDir;
	m_pFtpConnection->GetCurrentDirectory(strDir);
	m_strCurrentDirectory = strDir + _T("/") + m_strCurrentDirectory;
	// set current directory
	if (!m_pFtpConnection->SetCurrentDirectory(m_strCurrentDirectory)) //
	{
		if (m_pFtpConnection->CreateDirectory(m_strCurrentDirectory))
		{
			if (!m_pFtpConnection->SetCurrentDirectory(m_strCurrentDirectory)) {
				delete m_pFtpConnection;
				m_pFtpConnection = NULL;
				delete m_pDFSSession;
				m_pDFSSession = NULL;
				return;
			}
		}
	}
	m_pFtpConnection->SetCurrentDirectory(_T("/"));
	
	theApp.m_pFTPLog->LOG_INFO2(_T("==================FTP connection Upload================="));
	if (!m_pFtpConnection->PutFile(source, dest)){
		delete m_pFtpConnection;
		m_pFtpConnection = NULL;
		delete m_pDFSSession;
		m_pDFSSession = NULL;
		return;
	}
	m_pFtpConnection->Close();
	delete m_pFtpConnection;
	m_pFtpConnection = NULL;
	bSend = TRUE;
	theApp.m_pFTPLog->LOG_INFO2(_T("====================FTP connection Close========================="));
	// close FTP connection
}

/********************************************************************/
/*																	*/
/* Function name : WaitWithMessageLoop								*/
/* Description   : Pump messages while waiting for event			*/
/*																	*/
/********************************************************************/
BOOL CDFSClient::WaitWithMessageLoop(HANDLE hEvent, int nTimeout)
{
	DWORD dwRet;

	DWORD dwMaxTick = GetTickCount() + nTimeout;

	while (1)
	{
		// wait for event or message, if it's a message, process it and return to waiting state
		dwRet = MsgWaitForMultipleObjects(1, &hEvent, FALSE, dwMaxTick - GetTickCount(), QS_ALLINPUT);
		if (dwRet == WAIT_OBJECT_0)
		{
			TRACE0("WaitWithMessageLoop() event triggered.\n");
			return TRUE;
		}
		else
			if (dwRet == WAIT_OBJECT_0 + 1)
			{
				// process window messages
				DoEvents();
			}
			else
			{
				// timed out !
				return FALSE;
			}
		Delay(2, TRUE);  //<< 150613 JYLee
	}
}

BOOL CDFSClient::CheckDirectory(CString strPath)
{
	if (!m_pDFSSession) return FALSE;

	// Create new ftp connection to retrieve file
	int nRetries = m_ftpSite.m_nRetries;
	while (nRetries > 0)
	{
		nRetries--;
		try
		{
			int nTimeout = AfxGetApp()->GetProfileInt(_T("Settings"), _T("ConnectionTimeout"), 3);
			m_pDFSSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeout * 1000);
			m_pDFSSession->SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, nTimeout * 1000);
			m_pDFSSession->SetOption(INTERNET_OPTION_SEND_TIMEOUT, nTimeout * 1000);

			m_pFtpConnection = m_pDFSSession->GetFtpConnection(m_strServerName, m_ftpSite.m_strLogin, m_ftpSite.m_strPassword, m_ftpSite.m_nPort, m_ftpSite.m_bUsePASVMode);
			nRetries = 0;
		}
		catch (CInternetException* pEx)
		{
			DWORD dwLength = 255;//, dwError;

			CString strLastResponce;
			//				InternetGetLastResponseInfo(&dwError, strLastResponce.GetBuffer(dwLength), &dwLength);
			strLastResponce.ReleaseBuffer();
			strLastResponce.Remove('\n');
			strLastResponce.Remove('\r');

			// catch errors from WinINet
			dwLength = 255;
			CString strInfo;
			if (pEx->GetErrorMessage(strInfo.GetBuffer(dwLength), dwLength))
			{
				strInfo.ReleaseBuffer();
				strInfo.Remove('\n');
				strInfo.Remove('\r');

				// show wait dialog
				if (nRetries > 0)
				{
					Delay(m_ftpSite.m_nRetryDelay, TRUE); //<<150818 JYLee
				}
				else
				{
					if (!strLastResponce.IsEmpty())
					{
						strLastResponce += _T("\r\n");
					}

					strLastResponce += strInfo;
					//AfxMessageBox(strLastResponce, MB_OK);
				}
			}
			else
			{
				nRetries = 0;
				//AfxMessageBox(_T("Connect Fail!"), MB_OK);
			}
			pEx->Delete();
			pEx = NULL;

			delete m_pFtpConnection;
			m_pFtpConnection = NULL;
			delete m_pDFSSession;
			m_pDFSSession = NULL;
			return FALSE;
		}
	}
	// 	if (!m_pFtpConnection){
	// 		delete m_pFtpConnection;
	// 		m_pFtpConnection = NULL;
	// 		delete m_pDFSSession;
	// 		m_pDFSSession= NULL;
	// 		return FALSE;
	// 	}
	CString strFilePath, strDir;
	int temp(0);
	m_pFtpConnection->GetCurrentDirectory(strFilePath);
	//strFilePath = strFilePath + _T("/") + strPath;
	strFilePath = strPath;
	BOOL bRet = TRUE;
	
	if (m_pFtpConnection->SetCurrentDirectory(strPath))
		bRet = TRUE;
	else
	{
		m_pFtpConnection->SetCurrentDirectory(_T("/"));
		while (temp != -1)
		{
			temp = strPath.Find(_T("/"));
			if (temp == -1)
			{
				strDir = strPath;
			}
			else
			{
				strDir = strPath.Left(temp);
				strPath = strPath.Mid(temp + 1);
			}
			m_pFtpConnection->GetCurrentDirectory(strFilePath);
			strFilePath = strFilePath + "/" + strDir;

			if (!m_pFtpConnection->SetCurrentDirectory(strFilePath))
			{
				if (m_pFtpConnection->CreateDirectory(strFilePath))
					m_pFtpConnection->SetCurrentDirectory(strFilePath);
				else
					bRet = FALSE;
			}
		}
	}
	if (m_pFtpConnection){
		m_pFtpConnection->Close();
		delete m_pFtpConnection;
		m_pFtpConnection = NULL;
	}
	return bRet;
}

//<<
/*--- END OF FTPClient.cpp ---*/
void CDFSClient::Disconnect()
{
	if (m_pDFSSession){
		m_pDFSSession->Close();
		delete m_pDFSSession;
		m_pDFSSession= NULL;
	}
	if (m_pFtpConnection){
		m_pFtpConnection->Close();
		delete m_pFtpConnection;
		m_pFtpConnection = NULL;
	}
}

void CDFSClient::DfsAddTransferFile(DfsDataValue strTransferFile)
{
	if (strTransferFile.m_PanelID.IsEmpty())
		theApp.m_pFTPLog->LOG_INFO(_T("Sum Dfs PanelID Error"));

	m_csDfsUploadLock.Lock();
	m_DfsUploadtransferFileList.push(strTransferFile);
	m_csDfsUploadLock.Unlock();
}

void CDFSClient::RunDfsUploadThread()
{
	CDFSInfo DfsInfo;
	CDataInfo OpvInfo, DataInfo;
	DfsDataValue dfsData;

	CString strFpcID, strPanelID, strFilePath, strCsvFilePath, strImageFilePath, strIndexFile, strIndexTempFilePath, strLinkFilePath;
	CString strAOIPath, strViewingPath, strLumitopPath, strSumPath, strSumImagePath, strTemp1, strAoiImagePath, strViewingImagePath, strIndexFilePath, strOpvFilPath;

	while (::WaitForSingleObject(m_hDfsUploadQuit, 1000) != WAIT_OBJECT_0)
	{
		if (!m_DfsUploadtransferFileList.empty())
		{
			while (!m_DfsUploadtransferFileList.empty())
			{
				m_csDfsUploadLock.Lock();

				dfsData.Reset();
				DfsInfo.Clear();
				DataInfo.Clear();
				OpvInfo.Clear();
				m_vecIndexValue.clear();
				DfsInfo.m_OpvDataList[Machine_ULD].clear();
			
				dfsData = m_DfsUploadtransferFileList.front();
				strPanelID = dfsData.m_PanelID;
				strFpcID = dfsData.m_FpcID;
				theApp.m_pFTPLog->LOG_DEBUG(_T("Sum DFS START PanelID : %s, FPCID : %s,"), strPanelID, strFpcID);
				m_csDfsUploadLock.Unlock();
#if _SYSTEM_AMTAFT_
				if (strPanelID.IsEmpty() == FALSE)
				{
					strAOIPath = DFS_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\AOI\\") + strPanelID + _T(".csv");
					strViewingPath = DFS_VIEWING_ANGLE_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\VIEWING\\") + strPanelID + _T(".csv");
					strLumitopPath = DFS_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\LUMITOP\\") + strPanelID + _T(".csv");
					strOpvFilPath = DFS_SHARE_OPVDFS_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\") + strPanelID + _T(".csv");

					strTemp1 = DFS_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\SUM\\");
					CreateFolders(strTemp1);

					strSumImagePath = strTemp1 + _T("Image");
					CreateFolders(strSumImagePath);

					strAoiImagePath = DFS_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\AOI\\Image");
					strViewingImagePath = DFS_VIEWING_ANGLE_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\VIEWING\\Image");

					DfsInfo.CopyImage(strAoiImagePath, strSumImagePath);
					//DfsInfo.CopyImage(strViewingImagePath, strSumImagePath);

					strSumPath = strTemp1 + strPanelID + _T(".csv");
					//strSumImagePath = strTemp1 + _T("Image");

					DfsDataValue result;
					result = DataInfo.SetLoadFile(strPanelID);

					// 从 MySQL 数据库查询点灯检测结果（AOI 点灯检结果，直接从数据库读取）
					CString strAOIResult, strCodeAOI, strGradeAOI;
					BOOL bValid = FALSE;
					CString strUniqueID = theApp.GetLightingUniqueIDByBarcode(strPanelID);
					theApp.GetLightingResultByBarcode(strPanelID, strAOIResult, strCodeAOI, strGradeAOI, bValid);
					if (bValid)
					{
						// 将点灯结果填充到 result 结构
						// m_Lumitop 字段用于存储点灯结果（OK/NG）
						if (strAOIResult.CompareNoCase(_T("OK")) == 0)
							result.m_Lumitop = _T("OK");
						else
							result.m_Lumitop = _T("NG");

						// 将点灯检测结果填充到 DFS 数据中
						DfsInfo.m_PanelSummaryInfo.LUMITOP_PAENL_GRADE = strAOIResult;
						DfsInfo.m_strLumitopResult = strAOIResult;

						// 如果有点灯缺陷，更新 MainDefectCode
						if (!strCodeAOI.IsEmpty() && strAOIResult.CompareNoCase(_T("OK")) != 0)
						{
							DfsInfo.m_PanelSummaryInfo.strMainDefectCode = strCodeAOI;
						}

						theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
							_T("DFS: Lighting result from MySQL for PanelID=%s: AOIResult=%s, Code=%s, Grade=%s"),
							strPanelID, strAOIResult, strCodeAOI, strGradeAOI));

						// 从 MySQL 数据库查询点灯缺陷详情列表（AOI 缺陷详情）
						if (!strUniqueID.IsEmpty())
						{
							std::vector<SDFSDefectDataBegin> vecAOIDefects;
							sql::Connection* pDfsConn = theApp.GetDfsLightingConnection();
							if (theApp.QueryAOIDefectList(strUniqueID, vecAOIDefects, pDfsConn))
							{
								// 将缺陷详情添加到 DfsInfo（点灯缺陷填充到 m_DefectDataList）
								for (size_t i = 0; i < vecAOIDefects.size(); i++)
								{
									DfsInfo.m_DefectDataList.push_back(vecAOIDefects[i]);
								}
								theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
									_T("DFS: Loaded %d lighting defects from MySQL for PanelID=%s"),
									vecAOIDefects.size(), strPanelID));
							}
						}
					}
					else
					{
						theApp.m_pFTPLog->LOG_INFO(CStringSupport::FormatString(
							_T("DFS: No lighting result found in MySQL for PanelID=%s"), strPanelID));
					}

					if (_ttoi(result.m_ChNum) > 2)
					{
						if (result.m_ChNum == _T("3"))
							result.m_ChNum = _T("1");
						else
							result.m_ChNum = _T("2");
					}
					
					/*
					DfsInfo.m_EQPDataInfo.strRecipe_No = dfsData.m_ModelID;
					DfsInfo.m_EQPDataInfo.strLoad_Stage_No = result.m_ChNum;
					DfsInfo.m_EQPDataInfo.strUnload_Stage_No = CStringSupport::FormatString(_T("%d"), dfsData.m_StageNum);
					DfsInfo.m_EQPDataInfo.strStart_Time = dfsData.m_StartTime;
					DfsInfo.m_EQPDataInfo.strEnd_Time = dfsData.m_EndTime;
					DfsInfo.m_EQPDataInfo.strPre_Gamma_Time = dfsData.m_PreGammaTime;
					DfsInfo.m_EQPDataInfo.strTP_Time = dfsData.m_TpTime;
					DfsInfo.m_EQPDataInfo.strTact_Time = dfsData.m_TactTime;
					DfsInfo.m_EQPDataInfo.strContact = dfsData.m_Contact;
					DfsInfo.m_EQPDataInfo.strPreGamma= dfsData.m_PreGamma;
					DfsInfo.m_EQPDataInfo.strAOIInpsect = dfsData.m_AOIInpsect;
					DfsInfo.m_EQPDataInfo.strTpResult = dfsData.m_TpResult2;
					DfsInfo.m_EQPDataInfo.strLumitop =  dfsData.m_Lumitop;
					DfsInfo.m_EQPDataInfo.strmura =  dfsData.m_mura;
					*/

					//DfsInfo.m_EQPDataInfo.strAOI_RECIPE_NAME = dfsData.m_ModelID;
					//DfsInfo.m_EQPDataInfo.strPG_RECIPE_NAME = result.m_ChNum;
					//DfsInfo.m_EQPDataInfo.strTP_RECIPE_NAME = CStringSupport::FormatString(_T("%d"), dfsData.m_StageNum);
					theApp.m_pDataStatusLog->LOG_INFO(CStringSupport::FormatString(_T(" GetEQPDataInfo()_DFS Start time dfsData.m_StartTime : %s"), dfsData.m_StartTime));
					DfsInfo.m_EQPDataInfo.strSTART_TIME = dfsData.m_StartTime;
					DfsInfo.m_EQPDataInfo.strEND_TIME = dfsData.m_EndTime;
					DfsInfo.m_EQPDataInfo.strLOAD_STAGE_NO = dfsData.m_LoadeHandlerNUM;
					//DfsInfo.m_EQPDataInfo.strINSP_STAGE_NO = dfsData.m_TpTime;
					DfsInfo.m_EQPDataInfo.strUNLOAD_STAGE_NO = dfsData.m_UnLoadeHandlerNUM;	
					if (DfsInfo.m_EQPDataInfo.strUNLOAD_STAGE_NO == _T("0"))
						DfsInfo.m_EQPDataInfo.strUNLOAD_STAGE_NO = _T("1");
					DfsInfo.m_EQPDataInfo.strPROBE_CONTACT_CNT = dfsData.m_ContactCount;
					//DfsInfo.m_EQPDataInfo.strINDEX_PANEL_GRADE = dfsData.m_PreGamma;
					//DfsInfo.m_EQPDataInfo.strINDEX_MAIN_CODE = dfsData.m_AOIInpsect;
					//DfsInfo.m_EQPDataInfo.strFINAL_PANEL_GRADE = dfsData.m_TpResult2;
					//DfsInfo.m_EQPDataInfo.strFINAL_MAIN_CODE = dfsData.m_Lumitop;
					DfsInfo.m_EQPDataInfo.strOPERATOR_ID = theApp.m_OpvSocketManager[_ttoi(DfsInfo.m_EQPDataInfo.strUNLOAD_STAGE_NO)-1].m_strOPID;

					DfsInfo.m_PanelSummaryInfo.CONTACT_PAENL_GRADE = dfsData.m_Contact;
					// 点灯检代替 AOI：若 MySQL 点灯结果有效，则用其填充 AOI 结果
					if (bValid)
						DfsInfo.m_PanelSummaryInfo.AOI_PAENL_GRADE = strAOIResult;
					else
						DfsInfo.m_PanelSummaryInfo.AOI_PAENL_GRADE = dfsData.m_AOIInpsect;
					DfsInfo.m_PanelSummaryInfo.PRE_PAENL_GRADE = dfsData.m_PreGamma;
					DfsInfo.m_PanelSummaryInfo.DOT_PAENL_GRADE = dfsData.m_TpResult2;
					// 点灯结果：优先用 MySQL 点灯结果，否则用 PLC 值
					if (bValid)
						DfsInfo.m_PanelSummaryInfo.LUMITOP_PAENL_GRADE = strAOIResult;
					else
						DfsInfo.m_PanelSummaryInfo.LUMITOP_PAENL_GRADE = dfsData.m_Lumitop;
					DfsInfo.m_PanelSummaryInfo.OPV_PAENL_GRADE = dfsData.m_opViewResult;
					DfsInfo.m_PanelSummaryInfo.OPERATOR_ID = theApp.m_OpvSocketManager[_ttoi(DfsInfo.m_EQPDataInfo.strUNLOAD_STAGE_NO) - 1].m_strOPID;
	
					if (_ttoi(result.m_PreGammaContactStatus) == m_dfsPreGammaNG || _ttoi(result.m_PreGammaContactStatus) == m_dfsContactNG || _ttoi(result.m_TpResult) == m_dfsTpNG)
						DfsInfo.AddDefectCodeResult(strPanelID, _ttoi(result.m_PreGammaContactStatus), _ttoi(result.m_TpResult), Machine_ULD);
					//>>PG DFS Load
					DfsInfo.PGDfsInfoLoad(strPanelID);
					//<<

					DfsInfo.AMTAFTSavePanelDFS_SUM(result, strPanelID, strFpcID, strAOIPath, strViewingPath, strLumitopPath, strOpvFilPath, strSumPath);

					BOOL bTransfer = TRUE;

					if (!FileExists(strSumPath)){
						theApp.m_pFTPLog->LOG_INFO2(CStringSupport::FormatString(_T("[%s] Inspect Not exist csv File"), strPanelID));
					}
					else
					{
						CString strUploadEQPID = CStringSupport::FormatString(_T("%s%s"), theApp.m_strEqpId, theApp.m_strEqpNum);

						if (strPanelID.GetLength() >= DFS_CHECK_PANEL_SIZE)
						{
							if (theApp.m_bDFSTestMode == TRUE)
								strCsvFilePath = _T("D:\\TEST");
							else
								strCsvFilePath = _T("\\\\172.18.3.110\\module");

							SetFilePath(&strCsvFilePath, theApp.m_strEqpId);
							SetFilePath(&strCsvFilePath, GetDateString2());
							SetFilePath(&strCsvFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID.Left(5));
							SetFilePath(&strCsvFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID.Left(8));
							SetFilePath(&strCsvFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID);

							strImageFilePath = strCsvFilePath;
							SetFilePath(&strImageFilePath, _T("Image"));
							CreateFolders(strImageFilePath);

							SetFilePath(&strCsvFilePath, _T("Data"));
							CreateFolders(strCsvFilePath);

							if (theApp.m_bDFSTestMode == TRUE)
								strLinkFilePath = strIndexFilePath = _T("D:\\TEST");
							else
								strLinkFilePath = strIndexFilePath = _T("\\\\172.18.3.110\\module");

							// Index 파일 생성
							SetFilePath(&strIndexFilePath, _T("INDEX"));
							SetFilePath(&strIndexFilePath, theApp.m_strEqpId);
							CreateFolders(strIndexFilePath);

							// Link 파일 생성
							SetFilePath(&strLinkFilePath, _T("LINK"));
							SetFilePath(&strLinkFilePath, theApp.m_strEqpId);
							SetFilePath(&strLinkFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID.Left(5));
							SetFilePath(&strLinkFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID.Left(8));
							SetFilePath(&strLinkFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID);
							CreateFolders(strLinkFilePath);

							// ProcessID는 설비별로 따로 있고 GOOD,NG 일때 또 따로 있는데 
							// 담당자가 DFS파일 이름은 설비 ProcessID로 하라고 해서 이렇게 했음
							// 만약에 결과값에 따라 바뀐다고 하면 strProcessID -> DfsInfo.m_PanelDataBegin.strProcess_ID 이걸로 하면됩니다.
							CString strProcessID = _T("");
							if (!DfsInfo.m_HeaderInfo.strEQP_Type.CompareNoCase(_T("MFBAP")))
								strProcessID = _T("1700");
							else if (!DfsInfo.m_HeaderInfo.strEQP_Type.CompareNoCase(_T("MFGAP")))
								strProcessID = _T("1L00");

							strLinkFilePath = strLinkFilePath + _T("\\") + strProcessID + _T("_") + DfsInfo.m_PanelDataBegin.strPanel_ID + _T("_") + GetNowSystemTimeMillisecondsSirius() + _T(".csv");
							strCsvFilePath = strCsvFilePath + _T("\\") + strProcessID + _T("_") + DfsInfo.m_PanelDataBegin.strPanel_ID + _T("_") + GetNowSystemTimeMillisecondsSirius() + _T(".csv");

							::CopyFile(strSumPath, strLinkFilePath, FALSE);		//Link 파일 업로드
							::MoveFile(strSumPath, strCsvFilePath);		//csv 파일 업로드

							if (theApp.m_bDFSTestMode == TRUE)
								strCsvFilePath.Replace(_T("D:\\TEST\\"), _T("/MODULE/"));
							else
								strCsvFilePath.Replace(_T("\\\\172.18.3.110\\module"), _T("/MODULE"));

							strCsvFilePath.Replace(_T("\\"), _T("/"));
							m_vecIndexValue.push_back(strCsvFilePath);

							if (!DfsInfo.VisionLoadPanelDFSInfo(strPanelID, Machine_ULD))
								theApp.m_pFTPLog->LOG_INFO(_T("OPV Vision Dfs File Path Error : %s,"), strPanelID);

							for (int i = 0; i < DfsInfo.m_OpvDataList[Machine_ULD].size() && bTransfer; i++)
							{

								strSrc = strSumImagePath + _T("\\") + DfsInfo.m_OpvDataList[Machine_ULD].at(i).strImage;
								strDest = strImageFilePath + _T("\\") + DfsInfo.m_OpvDataList[Machine_ULD].at(i).strImage;

								if (FileExists(strSrc))
								{
									::MoveFile(strSrc, strDest);		// 화면검사 image 업로드
									if (theApp.m_bDFSTestMode == TRUE)
										strDest.Replace(_T("D:\\TEST\\"), _T("/MODULE/"));
									else
										strDest.Replace(_T("\\\\172.18.3.110\\module"), _T("/MODULE"));

									//strDest.Replace(theApp.m_strEqpId, strUploadEQPID);

									strDest.Replace(_T("\\"), _T("/"));
									m_vecIndexValue.push_back(strDest);
								}
								else
									theApp.m_pFTPLog->LOG_INFO2(_T("Vision Not exist image file"));
							}

							//전체Image File Name 항상 통일
							strSrc = strSumImagePath + _T("\\") + _T("AddsrcImageADD.jpg");
							strDest = strImageFilePath + _T("\\") + strProcessID + _T("_") + DfsInfo.m_PanelDataBegin.strPanel_ID + GetNowSystemTimeMillisecondsSirius4() + _T(".jpg");

							//CString strDest2 = strSumImagePath + _T("\\") + strProcessID + _T("_") + DfsInfo.m_PanelDataBegin.strPanel_ID + GetNowSystemTimeMillisecondsSirius4() + _T(".jpg");
							COleDateTime now;
							now = COleDateTime::GetCurrentTime();
							CString strDest2;
							strDest2.Format(_T("%s\\%s_%s_%s_Layout_%s_%02d%02d%02d.jpg"), strImageFilePath, strProcessID, DfsInfo.m_PanelDataBegin.strPanel_ID, theApp.m_strEqpId + theApp.m_strEqpNum, GetDateString2(), now.GetHour(), now.GetMinute(), now.GetSecond());
							if (FileExists(strSrc))
							{
								//::CopyFile(strSrc, strDest, FALSE);		// 전체 image 업로드
								::MoveFile(strSrc, strDest2);		// 전체 image 업로드
								

								strSrc = strSumImagePath;
								//strDest = strImageFilePath;

								DfsInfo.CopyImage2(strSrc, strDest);
								if (theApp.m_bDFSTestMode == TRUE)
									strDest2.Replace(_T("D:\\TEST\\"), _T("/MODULE/"));
								else
									strDest2.Replace(_T("\\\\172.18.3.110\\module"), _T("/MODULE"));

								//strDest2.Replace(theApp.m_strEqpId, strUploadEQPID);

								strDest2.Replace(_T("\\"), _T("/"));
								m_vecIndexValue.push_back(strDest2);
							}
							else
								theApp.m_pFTPLog->LOG_INFO2(_T("Not exist image file"));

							DfsIDXFileCreate(strUploadEQPID, &strIndexFile);
							strDest = strIndexFilePath + _T("\\") + GetDateString2() + _T("_") + strUploadEQPID + _T(".csv");

							::CopyFile(strIndexFile, strDest, FALSE);			//index 파일 업로드
						}
						else
						{
							theApp.m_pFTPLog->LOG_INFO2(_T("Panel ID length is short."));
						}

					}
				}

				Delay(10, TRUE);

				m_csDfsUploadLock.Lock();
				m_DfsUploadtransferFileList.pop();
				m_csDfsUploadLock.Unlock();
#else
				if (strPanelID.IsEmpty() == FALSE)
				{
					DfsInfo.m_EQPDataInfo.strRecipe_No = Int2String(theApp.m_CurrentModel.m_AlignPcCurrentModelNum);
					DfsInfo.m_EQPDataInfo.strRecipe_Name = theApp.m_CurrentModel.m_AlignPcCurrentModelName;
					DfsInfo.m_EQPDataInfo.strStart_Time = dfsData.m_StartTime;
					DfsInfo.m_EQPDataInfo.strEnd_Time = dfsData.m_EndTime;
					//DfsInfo.m_EQPDataInfo.strLD_Time = dfsData.m_LoadHandlerTime;
					//DfsInfo.m_EQPDataInfo.strUld_Time = dfsData.m_UnloadHandlerTime;
					DfsInfo.m_EQPDataInfo.strPre_Gamma_Time = dfsData.m_PreGammaTime;
					DfsInfo.m_EQPDataInfo.strTact_Time = dfsData.m_TactTime;
					DfsInfo.m_EQPDataInfo.strOperator_ID = theApp.m_strEqpId + theApp.m_strEqpNum;
					DfsInfo.m_EQPDataInfo.strUnit_ID = dfsData.m_IndexNum;
					DfsInfo.m_EQPDataInfo.strStage_ID = dfsData.m_ChNum; // 20200401 kty
					DfsInfo.m_EQPDataInfo.strProbe_Contact_Cnt = _T("1");
				
					strTemp1 = DFS_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID;
					CreateFolders(strTemp1);
					strSumPath = strTemp1 + _T("\\") + strPanelID + _T(".csv");
					DfsInfo.GammaSavePanelDFS_SUM(strPanelID, strFpcID, strSumPath);
				
					BOOL bTransfer = TRUE;
				
					if (!FileExists(strSumPath)){
						theApp.m_pFTPLog->LOG_INFO2(CStringSupport::FormatString(_T("[%s][%s] Inspect Not exist csv File"), strPanelID, strFpcID));
					}
					else
					{
						if (strPanelID.GetLength() >= DFS_CHECK_PANEL_SIZE)
						{
							if (theApp.m_bDFSTestMode == TRUE)
								strCsvFilePath = _T("D:\\TEST");
							else
								strCsvFilePath = _T("\\\\172.18.3.110\\module");
				
							SetFilePath(&strCsvFilePath, theApp.m_strEqpId);
							SetFilePath(&strCsvFilePath, GetDateString2());
							SetFilePath(&strCsvFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID.Left(5));
							SetFilePath(&strCsvFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID.Left(8));
							SetFilePath(&strCsvFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID);
							SetFilePath(&strCsvFilePath, _T("Data"));
							CreateFolders(strCsvFilePath);
				
							if (theApp.m_bDFSTestMode == TRUE)
								strLinkFilePath = strIndexFilePath = _T("D:\\TEST");
							else
								strLinkFilePath = strIndexFilePath = _T("\\\\172.18.3.110\\module");
							
							SetFilePath(&strIndexFilePath, _T("INDEX"));
							SetFilePath(&strIndexFilePath, theApp.m_strEqpId);
							
							SetFilePath(&strLinkFilePath, _T("LINK"));
							SetFilePath(&strLinkFilePath, theApp.m_strEqpId);
							SetFilePath(&strLinkFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID.Left(5));
							SetFilePath(&strLinkFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID.Left(8));
							SetFilePath(&strLinkFilePath, DfsInfo.m_PanelDataBegin.strPanel_ID);
							
							CreateFolders(strLinkFilePath);
							CreateFolders(strIndexFilePath);
				
							strLinkFilePath = strLinkFilePath + _T("\\") + DfsInfo.m_PanelDataBegin.strProcess_ID + _T("_") + DfsInfo.m_PanelDataBegin.strPanel_ID + _T("_") + GetNowSystemTimeMillisecondsSirius() + _T(".csv");
							strCsvFilePath = strCsvFilePath + _T("\\") + DfsInfo.m_PanelDataBegin.strProcess_ID + _T("_") + DfsInfo.m_PanelDataBegin.strPanel_ID + _T("_") + GetNowSystemTimeMillisecondsSirius() + _T(".csv");
				
							::CopyFile(strSumPath, strLinkFilePath, FALSE);		//Link 파일 업로드
							::CopyFile(strSumPath, strCsvFilePath, FALSE);		//csv 파일 업로드
				
							if (theApp.m_bDFSTestMode == TRUE)
								strDest.Replace(_T("D:\\TEST\\"), _T("/MODULE/"));
							else
								strDest.Replace(_T("\\\\172.18.3.110\\module"), _T("/MODULE"));
							
							strCsvFilePath.Replace(_T("\\"), _T("/"));
							m_vecIndexValue.push_back(strCsvFilePath);
							
							DfsIDXFileCreate(DfsInfo.m_HeaderInfo.strEQP_ID, &strIndexFile);
							strDest = strIndexFilePath + _T("\\") + GetDateString2() + _T("_") + DfsInfo.m_HeaderInfo.strEQP_ID + _T(".csv");
				
							::CopyFile(strIndexFile, strDest, FALSE);			//index파일 업로드
						}
						else
							theApp.m_pFTPLog->LOG_INFO2(_T("Panel ID length is short."));
					}
				}
				Delay(10, TRUE);
				
				m_csDfsUploadLock.Lock();
				m_DfsUploadtransferFileList.pop();
				m_csDfsUploadLock.Unlock();
#endif
			}
		}
	}
}

#if _SYSTEM_AMTAFT_
void CDFSClient::AddTransferFile(DfsDataValue strTransferFile)
{
	if (strTransferFile.m_PanelID.IsEmpty())
		theApp.m_pFTPLog->LOG_INFO(_T("PanelID Error"));

	m_csLock.Lock();
	m_transferFileList.push(strTransferFile);
	m_csLock.Unlock();
}

void CDFSClient::RunFtpUploadThread()
{
	CDFSInfo DfsInfo;
	CDataInfo OpvInfo, DataInfo;
	DfsDataValue dfsData;

	CString strFpcID, strPanelID;
	CString strSumImagePath, strTemp1;

	while (::WaitForSingleObject(m_hQuit, 1000) != WAIT_OBJECT_0)
	{
		if (!m_transferFileList.empty())
		{
			while (!m_transferFileList.empty())
			{
				m_csLock.Lock();

				dfsData.Reset();
				OpvInfo.Clear();
				DfsInfo.m_strVisionResult = "";
				DfsInfo.m_strViewingResult = "";
				DfsInfo.m_strLumitopResult = "";
				DfsInfo.m_OpvDataList[Machine_AOI].clear();

				dfsData = m_transferFileList.front();
				strPanelID = dfsData.m_PanelID;
				strFpcID = dfsData.m_FpcID;
				theApp.m_pFTPLog->LOG_DEBUG(_T("DFS START PanelID : %s, FPCID : %s,"), strPanelID, strFpcID);
				m_csLock.Unlock();

				if (strPanelID.IsEmpty() == FALSE)
				{
					DfsInfo.IndexZoneInspResultInfo(strPanelID);
					if (!DfsInfo.VisionLoadPanelDFSInfo(strPanelID, Machine_AOI)) // 이건 OPV .txt 파일 용입니다.
						theApp.m_pFTPLog->LOG_INFO(_T("OPV Vision Dfs File Path Error : %s,"), strPanelID);

					if (theApp.m_bSameDefectMode == TRUE)
					{
						for (auto AoiDefect : DfsInfo.m_OpvDataList[Machine_AOI])
						{
							VisionSameDefect DefectInfo;
							DefectInfo.m_strDefectCode = AoiDefect.strDefect_code;
							DefectInfo.m_strPanelID = AoiDefect.strPanel_ID;
							DefectInfo.m_strChNum = dfsData.m_ChNum;
							DefectInfo.m_strFpcID = AoiDefect.strFpc_ID;

							if (!CheckSameAOIDefect(DefectInfo.m_strChNum, DefectInfo))
								theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionSameDefectAlarmStart, OffSet_0, TRUE);
						}
					}

					OpvInfo.m_Panel_Info.strTime = GetDateString4();
					OpvInfo.m_Panel_Info.strPanel_ID = DfsInfo.m_PanelDataBegin.strPanel_ID;
					OpvInfo.m_Panel_Info.strFpc_ID = strFpcID;
					OpvInfo.m_Panel_Info.strPanel_Width = theApp.m_strOpvImageWidth;
					OpvInfo.m_Panel_Info.strPanel_Hegiht = theApp.m_strOpvImageHeight;
					OpvInfo.m_Panel_Info.strPreGammaContactStatus = dfsData.m_PreGammaContactStatus;
					OpvInfo.m_Panel_Info.strModel_ID = dfsData.m_ModelID;
					OpvInfo.m_Panel_Info.strIndexNum = dfsData.m_IndexNum;
					OpvInfo.m_Panel_Info.strChNum = dfsData.m_ChNum;
					OpvInfo.m_Panel_Info.strVisionResult = DfsInfo.m_strVisionResult;
					OpvInfo.m_Panel_Info.strViewingResult = DfsInfo.m_strViewingResult;
					OpvInfo.m_Panel_Info.strTpResult = dfsData.m_TpResult;
					OpvInfo.m_Panel_Info.strLumitopResult = DfsInfo.m_strLumitopResult;

					if (theApp.m_iMachineType == SetAMT)
					{
						if (_ttoi(OpvInfo.m_Panel_Info.strPreGammaContactStatus) == m_dfsContactNG)
						{
							OpvInfo.m_Panel_Info.strDefect_Result = _T("N");
						}
						else if (_ttoi(OpvInfo.m_Panel_Info.strPreGammaContactStatus) == m_dfsPreGammaNG
							|| OpvInfo.m_Panel_Info.strVisionResult == _T("N")
							|| OpvInfo.m_Panel_Info.strViewingResult == _T("N"))
						{
							OpvInfo.m_Panel_Info.strDefect_Result = _T("N");
						}
						else
							OpvInfo.m_Panel_Info.strDefect_Result = _T("G");
					}
					else
					{
						if (_ttoi(OpvInfo.m_Panel_Info.strPreGammaContactStatus) == m_dfsContactNG)
						{
							OpvInfo.m_Panel_Info.strDefect_Result = _T("N");
						}
						else if (OpvInfo.m_Panel_Info.strVisionResult == _T("N") ||
							OpvInfo.m_Panel_Info.strViewingResult == _T("N") ||
							OpvInfo.m_Panel_Info.strLumitopResult == _T("N"))
						{
							OpvInfo.m_Panel_Info.strDefect_Result = _T("N");
						}
						else
							OpvInfo.m_Panel_Info.strDefect_Result = _T("G");
					}

					if (_ttoi(OpvInfo.m_Panel_Info.strPreGammaContactStatus) == m_dfsPreGammaNG || _ttoi(OpvInfo.m_Panel_Info.strPreGammaContactStatus) == m_dfsContactNG || _ttoi(OpvInfo.m_Panel_Info.strTpResult) == m_dfsTpNG)
						DfsInfo.AddDefectCodeResult(strPanelID, _ttoi(OpvInfo.m_Panel_Info.strPreGammaContactStatus), _ttoi(OpvInfo.m_Panel_Info.strTpResult), Machine_AOI);

					int ii = 1;
					for (auto defect : DfsInfo.m_OpvDataList[Machine_AOI])
					{
						SDataDefectInfo defectInfo;
						defectInfo.strNo = CStringSupport::FormatString(_T("%d"), ii);
						defectInfo.strInspName = defect.strInspName;
						defectInfo.strDefect_Code = defect.strDefect_code;
						defectInfo.strDefect_Pattern = defect.strDefect_Ptn;
						defectInfo.strDefect_StartX = defect.strData_X1;
						defectInfo.strDefect_StartY = defect.strGate_Y1;
						defectInfo.strDefect_EndX = defect.strData_X2;
						defectInfo.strDefect_EndY = defect.strGate_Y2;
						defectInfo.strDefect_Grade = defect.strDefect_Grade;
						ii++;
						OpvInfo.m_Panel_Defect.push_back(defectInfo);
					}

					strTemp1 = DFS_SHARE_OPV_PATH + GetDateString2() + _T("\\") + strPanelID;
					//strTemp1 = DFS_SHARE_OPV_PATH + GetDateString2() + strPanelID;
					CreateFolders(strTemp1);
					strSumImagePath = DFS_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\AOI\\Image");
					strOpvSrc = strSumImagePath + _T("\\") + _T("AddsrcImageADD.jpg");
					strOpvDest = strTemp1 + _T("\\") + _T("AddsrcImageADD.jpg");

					strTemp1 = strTemp1 + _T("\\") + strPanelID + _T(".txt");
					OpvInfo.SetSaveFile(strTemp1);

					::CopyFile(strOpvSrc, strOpvDest, FALSE); //image 업로드
					theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionSameDefectAlarmStart, OffSet_0, FALSE);
				}

				Delay(10, TRUE);

				m_csLock.Lock();
				m_transferFileList.pop();
				m_csLock.Unlock();
			}
		}
	}
}

UINT CDFSClient::FtpUploadTask(LPVOID pParam)
{
	//>> 161201 jwan
	CDFSClient* pThis = reinterpret_cast<CDFSClient*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->RunFtpUploadThread();
	return 1L;
}


BOOL CDFSClient::CreateTask() {
	//>> 161201 jwan
	BOOL bRet = TRUE;
	m_pThreadFtpUpload = ::AfxBeginThread(FtpUploadTask, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadFtpUpload)
		bRet = FALSE;
	m_pThreadFtpUpload->m_bAutoDelete = FALSE;	/// ¾²·¹µå Á¾·á½Ã WaitForSingleObject Àû¿ëÀ§ÇØ...
	m_pThreadFtpUpload->ResumeThread();
	return bRet;
}

void CDFSClient::CloseTask()
{
	//>> 161201 jwan
	if (m_pThreadFtpUpload != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadFtpUpload->m_hThread, 6000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadFtpUpload->m_hThread, 10000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadFtpUpload->m_hThread, 1L);
				TRACE(_T("Terminate FTP Upload Thread\n"));
			}
		}
		delete m_pThreadFtpUpload;
		m_pThreadFtpUpload = NULL;
	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}
#endif
BOOL CDFSClient::CheckSameAOIDefect(CString strChNum, VisionSameDefect defectList)
{
	BOOL bOverDefectCount(TRUE);
	map<CString, VisionSameDefect>::iterator iter;

	CString strKey = CStringSupport::FormatString(_T("%s^%s"), defectList.m_strChNum, defectList.m_strDefectCode);
	CString strCode, strCh;
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(strKey, _T('^'), responseTokens);

	strCh = responseTokens[0];
	strCode = responseTokens[1];

	if (!strCode.IsEmpty())
	{
		theApp.m_iTotalCompareCount++;

		iter = m_mapSameDefect.find(strKey);
		if (iter != m_mapSameDefect.end())
		{
			if (theApp.m_bSameDefectChCheckMode == TRUE)
			{
				if (!iter->second.m_strChNum.CompareNoCase(strCh) && !theApp.m_strSameDefectCode.CompareNoCase(strCode) &&
					iter->second.m_strPanelID.CompareNoCase(defectList.m_strPanelID))
				{
					iter->second.m_iSameDefectCount++;
					m_mapSameDefect.insert(make_pair(strKey, iter->second));

					if (iter->second.m_iSameDefectCount >= _ttoi(theApp.m_strSameDefectAlarmMaxCount))
					{
						CString strMsg = CStringSupport::FormatString(_T("Panel [%s][%s] Same NG Over %s Count, DefectCode : %s, ChNum : %s"),
							iter->second.m_strPanelID, iter->second.m_strFpcID, theApp.m_strSameDefectAlarmMaxCount, iter->second.m_strDefectCode, iter->second.m_strChNum);

						theApp.m_pTraceLog->LOG_DEBUG(strMsg);

						theApp.m_pMsgBoxAlarm->WaitShowHide(SW_SHOW, strMsg);
						//theApp.getMsgBox(MS_OK, strMsg, strMsg, strMsg);
						m_mapSameDefect.clear();
						theApp.m_iTotalCompareCount = 0;
						bOverDefectCount = FALSE;
					}
				}
			}
			else
			{
				if (!theApp.m_strSameDefectCode.CompareNoCase(strCode) && iter->second.m_strPanelID.CompareNoCase(defectList.m_strPanelID))
				{
					iter->second.m_iSameDefectCount++;
					m_mapSameDefect.insert(make_pair(strKey, iter->second));

					if (iter->second.m_iSameDefectCount >= _ttoi(theApp.m_strSameDefectAlarmMaxCount))
					{
						CString strMsg = CStringSupport::FormatString(_T("Panel [%s][%s] Same NG Over %s Count, DefectCode : %s"),
							iter->second.m_strPanelID, iter->second.m_strFpcID, theApp.m_strSameDefectAlarmMaxCount, iter->second.m_strDefectCode);
						theApp.m_pTraceLog->LOG_DEBUG(strMsg);
						theApp.m_pMsgBoxAlarm->WaitShowHide(SW_SHOW, strMsg);
						//theApp.getMsgBox(MS_OK, strMsg, strMsg, strMsg);
						m_mapSameDefect.clear();
						theApp.m_iTotalCompareCount = 0;
						bOverDefectCount = FALSE;
					}
				}
			}
		}
		else
		{
			if (!theApp.m_strSameDefectCode.CompareNoCase(strCode))
			{
				defectList.m_iSameDefectCount = 1;
				m_mapSameDefect.insert(make_pair(strKey, defectList));
			}
		}

		if (theApp.m_iTotalCompareCount >= _ttoi(theApp.m_strSameDefectMaxCount))
		{
			theApp.m_iTotalCompareCount = 0;
			m_mapSameDefect.clear();
		}
	}

	return bOverDefectCount;
}

BOOL CDFSClient::DfsIDXFileCreate(CString strEqpName, CString *strIdxFileName)
{
	CString strDfsFilePath;
	CString strString;
	CStdioFile File;

	strDfsFilePath = DFS_SHARE_PATH + GetDateString2() + _T("\\") + GetDateString2() + _T("_") + strEqpName + _T(".csv");

	/*if (File.Open(strDfsFilePath, CFile::modeCreate | CFile::modeWrite) == FALSE)
		return FALSE;

	File.Close();

	FILE* fOutFile1;
	char* fileName = StringToChar(strDfsFilePath);
	fOutFile1 = fopen(fileName, "wt");

	strString.Format(_T("Date_Time,Path\n"));
	fputs(CStringToUtf8(strString), fOutFile1);

	for (CString IndexValue : m_vecIndexValue)
	{
		strString.Format(_T("%s,%s\n"), GetDateString6(), IndexValue);
		fputs(CStringToUtf8(strString), fOutFile1);
	}

	fclose(fOutFile1);*/
	BOOL bOpen = FALSE;
	if (!File.Open(strDfsFilePath, CFile::modeReadWrite | CFile::shareDenyNone))
	{
		if (File.Open(strDfsFilePath, CFile::modeCreate | CFile::modeWrite))
		{
			bOpen = TRUE;

			strString.Format(_T("Date_Time,Path\n"));
			File.WriteString(strString);
		}

	}
	else bOpen = TRUE;

	if (bOpen){
		File.SeekToEnd();
		for (CString IndexValue : m_vecIndexValue)
		{
			strString.Format(_T("%s,%s\n"), GetDateString6(), IndexValue);
			File.WriteString(strString);
		}
		File.Close();
	}

	*strIdxFileName = strDfsFilePath;

	return TRUE;
}

UINT CDFSClient::DfsUploadTask(LPVOID pParam)
{
	CDFSClient* pThis = reinterpret_cast<CDFSClient*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->RunDfsUploadThread();
	return 1L;
}

BOOL CDFSClient::CreateDfsTask() 
{
	BOOL bRet = TRUE;
	m_pThreadDfsUpload = ::AfxBeginThread(DfsUploadTask, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadDfsUpload)
		bRet = FALSE;
	m_pThreadDfsUpload->m_bAutoDelete = FALSE;	/// ¾²·¹µå Á¾·á½Ã WaitForSingleObject Àû¿ëÀ§ÇØ...
	m_pThreadDfsUpload->ResumeThread();
	return bRet;
}

void CDFSClient::CloseDfsTask()
{
	if (m_pThreadDfsUpload != NULL)
	{
		SetEvent(m_hDfsUploadQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadDfsUpload->m_hThread, 6000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hDfsUploadQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadDfsUpload->m_hThread, 10000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadDfsUpload->m_hThread, 1L);
				TRACE(_T("Terminate DFS Upload Thread\n"));
			}
		}
		delete m_pThreadDfsUpload;
		m_pThreadDfsUpload = NULL;
	}
	if (m_hDfsUploadQuit)
	{
		CloseHandle(m_hDfsUploadQuit);
		m_hDfsUploadQuit = NULL;
	}
}