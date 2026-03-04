// FTPClient.cpp: implementation of the CFTPClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "FTPClient.h"
#include "Ani_Data_Serever_PC.h"
#include "StringSupport.h"
#include "DataInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFTPClient::CFTPClient()
{
	m_bConnectState = FALSE;
	m_pFtpConnection = NULL;
	m_pDFSSession = NULL;

	m_strCurrentDirectory = "/";
	m_strRootDirectory = "/";

	if (!theApp.m_strFileServerID.CompareNoCase(_T("MFBOP")))
	{
		for (int ii = 0; ii < FS_IPAdressCount; ii++)
			if (_ttoi(theApp.m_strEqpNum.Left(1)) - 1 == ii)
				m_strAddress = AMT_FS_IP_ADDRESS[ii];
	}
	else if (!theApp.m_strFileServerID.CompareNoCase(_T("MFGRF")))
	{
		for (int ii = 0; ii < FS_IPAdressCount; ii++)
			if (_ttoi(theApp.m_strEqpNum.Left(1)) - 1 == ii)
				m_strAddress = AFT_FS_IP_ADDRESS[ii];
	}
	else if (!theApp.m_strFileServerID.CompareNoCase(_T("MFGGA")))
	{
		for (int ii = 0; ii < FS_IPAdressCount; ii++)
			if (_ttoi(theApp.m_strEqpNum.Left(1)) - 3 == ii)
				m_strAddress = GAMMA_FS_IP_ADDRESS[ii];
	}
	m_bUsePASVMode = FALSE;
	m_nRetries = 1;

	m_nPort = 21;

	m_strDescription = "";
	m_strLocalPath = "";
	m_nRetryDelay = 2;
	m_strUserName = "";
	m_strLogin = "";
	m_strName = "";
	m_strPassword = "";
	m_strRemotePath = "";
	m_strFileServerName = "";

	for (int ii = 0; ii < ModuleData_Count; ii++)
		m_strModuleInfo[ii] = "";

	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CFTPClient::~CFTPClient()
{
	Disconnect();
}

void CFTPClient::Connect(LPCTSTR lpszSiteName, LPCTSTR lpszUserName, LPCTSTR lpszPassword)
{
	m_ftpSite.m_strName.Empty();

	m_strUserName = lpszUserName;
	m_strLogin = lpszUserName;
	m_strName = lpszSiteName;
	m_strPassword = lpszPassword;

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

void CFTPClient::UploadFile(CString &source, CString &dest, BOOL &bSend)
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

	CString strDir;
	m_pFtpConnection->GetCurrentDirectory(strDir);
	m_strCurrentDirectory = strDir + _T("/") + m_strCurrentDirectory;
	// set current directory
	if (!m_pFtpConnection->SetCurrentDirectory(m_strCurrentDirectory)) //¿©±â¼­ °æ·Î ¼³Á¤ÇÏ°í ¾ÈµÇ¸é Æú´õ »ý¼º ±×¸®°í¾ÈµÇ¸é ³ª°¡! 
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

void CFTPClient::DownLoadFile(CString &source, CString &dest, BOOL &bSend)
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
	CString str;
	str.Format(_T("==================FTP connection DownLoad source : %s , dest : %s ================="), source, dest);
	theApp.m_pFTPLog->LOG_INFO2(str);
	if (!m_pFtpConnection->GetFile(source, dest)){
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

/*--- END OF FTPClient.cpp ---*/
void CFTPClient::Disconnect()
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

void CFTPClient::AddTransferFile(SJobDataShop strTransferFile)
{
	if (strTransferFile.Job_ID.IsEmpty())
		theApp.m_pFTPLog->LOG_INFO(_T("Job_ID Error"));

	m_csLock.Lock();
	m_transferFileList.push(strTransferFile);
	m_csLock.Unlock();
}

void CFTPClient::RunFtpUploadThread()
{
	m_strFileServerName = _T("BC");

	CStringArray responseTokens;
	CString strFileServerPath, strTemp, strPath, strEQPPath, strFSLineNum;
	SJobDataShop pJobData;

	while (::WaitForSingleObject(m_hQuit, 3000) != WAIT_OBJECT_0)
	{
		if(!m_transferFileList.empty())
		{
			BOOL bTransfer = TRUE;
			BOOL bFSDataSave = FALSE;
			// 파라미터 라인번호는 DFS용이라 FileServer 패스워드와 다르기 때문에 처리합니다.
			// ex) 190..290..390.. -> 100,200,300
			strFSLineNum = CStringSupport::FormatString(_T("%s00"), theApp.m_strEqpNum.Left(1));

			m_strLogin = theApp.m_strFileServerID + strFSLineNum;
			m_strPassword = m_strLogin + _T("@ftp");

			if (!m_strLogin.CompareNoCase(_T("MFBOP100")))
				m_strPassword = theApp.m_strFileServerID + _T("@ftp");// AMT#1 경우만 비밀번호가 다름.
		
			Connect(m_strFileServerName, m_strLogin, m_strPassword);
			while (!m_transferFileList.empty()) 
			{
				m_csLock.Lock();
				pJobData.Reset();
				strFileServerPath = _T("");
				strTemp = _T("");
				pJobData = m_transferFileList.front();
				m_csLock.Unlock();
				if (pJobData.Job_ID.IsEmpty() == FALSE)
				{
					theApp.m_pFTPLog->LOG_DEBUG(_T("FileServer START Job ID : %s, %s"), pJobData.Job_ID, pJobData.Cassette_Sequence_No);

					strFileServerPath = m_strLogin + _T("/") + pJobData.Cassette_Sequence_No + _T("/") + pJobData.Job_ID + _T(".dat");

					if (pJobData.TypeNum == Machine_AOI)
						strTemp = FS_SHARE_JOBDATA_PATH + GetDateString2() + _T("\\") + _T("AOI") + _T("\\") + pJobData.Job_ID;
					else if (pJobData.TypeNum == Machine_ULD)
						strTemp = FS_SHARE_JOBDATA_PATH + GetDateString2() + _T("\\") + _T("ULD") + _T("\\") + pJobData.Job_ID;
					else
						strTemp = FS_SHARE_JOBDATA_PATH + GetDateString2() + _T("\\") + pJobData.Job_ID;

					CreateFolders(strTemp);
					strTemp = strTemp + _T("\\") + pJobData.Job_ID + _T(".dat");

					if (pJobData.BCDataFileExist == TRUE)
					{
						DownLoadFile(strFileServerPath, strTemp, bTransfer);
						//if (bTransfer == TRUE)
						DataCompare(strTemp, pJobData);
					}
					else
					{
						DataCompare(strTemp, pJobData);
					}
				}
				else
				{
					theApp.m_pFTPLog->LOG_INFO2(_T("Job ID length is short."));
				}

				m_csLock.Lock();
				m_transferFileList.pop();
				m_csLock.Unlock();
				if (!bTransfer)
				{
					Disconnect();
					Connect(m_strFileServerName, m_strLogin, m_strPassword);
				}
				Delay(10, TRUE);
			}
			Disconnect();
		}
	}
}

BOOL CFTPClient::CheckDirectory(CString strPath)
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

UINT CFTPClient::FtpUploadTask(LPVOID pParam)
{
	//>> 161201 jwan
	CFTPClient* pThis = reinterpret_cast<CFTPClient*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->RunFtpUploadThread();
	return 1L;
}


BOOL CFTPClient::CreateTask() {
	//>> 161201 jwan
	BOOL bRet = TRUE;
	m_pThreadFtpUpload = ::AfxBeginThread(FtpUploadTask, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadFtpUpload)
		bRet = FALSE;
	m_pThreadFtpUpload->m_bAutoDelete = FALSE;	/// ¾²·¹µå Á¾·á½Ã WaitForSingleObject Àû¿ëÀ§ÇØ...
	m_pThreadFtpUpload->ResumeThread();
	return bRet;
}

void CFTPClient::CloseTask()
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

BOOL CFTPClient::DataCompare(CString strPath, SJobDataShop pJobDataShop)
{
	//setlocale(LC_ALL, "Chinese");
	CStringArray responseTokens;
	CDataInfo dataInfo;
	CString strInfo;
	int iCount = 0;
	if (pJobDataShop.BCDataFileExist == TRUE)
	{
		FILE *fStream;
		errno_t e = _tfopen_s(&fStream, strPath, _T("rt,ccs=UTF-8"));
		if (e != 0)
			goto NO_FILE;

		CStdioFile file(fStream);  // open the file from this stream
		
		while (file.ReadString(strInfo))
		{
			if (strInfo.Find(_T("=")) != -1)
			{
				if (ModuleData_Count <= iCount)
					break;
				strInfo.Replace(_T("』"), _T(""));
				responseTokens.RemoveAll();
				CStringSupport::GetTokenArray(strInfo, '=', responseTokens);
				m_strModuleInfo[iCount] = responseTokens[1].Trim();
				iCount++;
			}
		}
		file.Close();
	}

NO_FILE:
	CString strTemp, strFilePath, strShift;
	strShift = theApp.m_lastShiftIndex == 0 ? _T("DY") : _T("NT");
	if (pJobDataShop.TypeNum == Machine_AOI)
		strTemp.Format(_T("%s\\%s\\%s_%s\\"), DATA_SYSTEM_DATA_SUM_PATH, _T("AOI"), theApp.m_strCurrentToday, strShift);
	else if (pJobDataShop.TypeNum == Machine_ULD)
		strTemp.Format(_T("%s\\%s\\%s_%s\\"), DATA_SYSTEM_DATA_SUM_PATH, _T("ULD"), theApp.m_strCurrentToday, strShift);
	else
		strTemp.Format(_T("%s\\%s_%s\\"), DATA_SYSTEM_DATA_SUM_PATH, theApp.m_strCurrentToday, strShift);

	CreateFolders(strTemp);
	strFilePath.Format(_T("%s%s.ini"), strTemp, pJobDataShop.Job_ID);
	EZIni ini(strFilePath);

	// PLC -> PC DCR DATA
	ini[_T("JOB_DATA")][_T("Cassette_Sequence_No")] = pJobDataShop.Cassette_Sequence_No;
	ini[_T("JOB_DATA")][_T("Job_Sequence_No")] = pJobDataShop.Job_Sequence_No;
	ini[_T("JOB_DATA")][_T("Group_Index")] = pJobDataShop.Group_Index;
	ini[_T("JOB_DATA")][_T("Product_Type")] = pJobDataShop.Product_Type;
	ini[_T("JOB_DATA")][_T("CST_Operation_Mode")] = pJobDataShop.CST_Operation_Mode;
	ini[_T("JOB_DATA")][_T("SubStrate_Type")] = pJobDataShop.SubStrate_Type;
	ini[_T("JOB_DATA")][_T("CIM_Mode")] = pJobDataShop.CIM_Mode;
	ini[_T("JOB_DATA")][_T("Job_Type")] = pJobDataShop.Job_Type;
	ini[_T("JOB_DATA")][_T("Job_Judge")] = pJobDataShop.Job_Judge;
	ini[_T("JOB_DATA")][_T("Sampling_Slot_Flag")] = pJobDataShop.Sampling_Slot_Flag;
	ini[_T("JOB_DATA")][_T("First_Run")] = pJobDataShop.First_Run;
	ini[_T("JOB_DATA")][_T("Job_Grade")] = pJobDataShop.Job_Grade;
	ini[_T("JOB_DATA")][_T("Job_ID")] = pJobDataShop.Job_ID;
	ini[_T("JOB_DATA")][_T("INSP_Reservation")] = pJobDataShop.INSP_Reservation;
	ini[_T("JOB_DATA")][_T("EQP_Reservation")] = pJobDataShop.EQP_Reservation;
	ini[_T("JOB_DATA")][_T("LastGlass_Flag")] = pJobDataShop.LastGlass_Flag;
	ini[_T("JOB_DATA")][_T("InspJudge_Data")] = pJobDataShop.InspJudge_Data;
	ini[_T("JOB_DATA")][_T("Tracking_Data")] = pJobDataShop.Tracking_Data;
	ini[_T("JOB_DATA")][_T("EQP_Flag")] = pJobDataShop.EQP_Flag;
	ini[_T("JOB_DATA")][_T("Chip_Count")] = pJobDataShop.Chip_Count;
	ini[_T("JOB_DATA")][_T("PP_ID")] = pJobDataShop.PP_ID;
	ini[_T("JOB_DATA")][_T("FPC_ID")] = pJobDataShop.FPC_ID;
	ini[_T("JOB_DATA")][_T("Cassette_Setting_Code")] = pJobDataShop.Cassette_Setting_Code;
	if (pJobDataShop.BCDataFileExist == TRUE) // FileServer JOB DATA
	{
		ini[_T("JOB_DATA")][_T("Panel_ID")] = m_strModuleInfo[Panel_ID];
		ini[_T("JOB_DATA")][_T("ModuleData_FPC_ID")] = m_strModuleInfo[ModuleData_FPC_ID];
		ini[_T("JOB_DATA")][_T("Glass_Type")] = m_strModuleInfo[Glass_Type];
		ini[_T("JOB_DATA")][_T("Product_ID")] = m_strModuleInfo[Product_ID];
		ini[_T("JOB_DATA")][_T("Owner_ID")] = m_strModuleInfo[Owner_ID];
		ini[_T("JOB_DATA")][_T("Owner_Code")] = m_strModuleInfo[Owner_Code];
		ini[_T("JOB_DATA")][_T("Owner_Type")] = m_strModuleInfo[Owner_Type];

		if (theApp.m_bBCTestMode == TRUE)
		{
			if (!theApp.m_strEqpId.CompareNoCase(_T("MFBAP")))
				ini[_T("JOB_DATA")][_T("Process_ID")] = _T("1700");
			else if (!theApp.m_strEqpId.CompareNoCase(_T("MFGAP")))
				ini[_T("JOB_DATA")][_T("Process_ID")] = _T("1L00");
			else if (!theApp.m_strEqpId.CompareNoCase(_T("MFGGA")))
				ini[_T("JOB_DATA")][_T("Process_ID")] = _T("1J00");
		}
		else
			ini[_T("JOB_DATA")][_T("Process_ID")] = m_strModuleInfo[Process_ID];

		ini[_T("JOB_DATA")][_T("Recipe_ID")] = m_strModuleInfo[Recipe_ID];
		ini[_T("JOB_DATA")][_T("SaleOrder")] = m_strModuleInfo[SaleOrder];
		ini[_T("JOB_DATA")][_T("PreProcess_ID_1")] = m_strModuleInfo[PreProcess_ID_1];
		ini[_T("JOB_DATA")][_T("Group_ID")] = m_strModuleInfo[Group_ID];
		ini[_T("JOB_DATA")][_T("Product_Info")] = m_strModuleInfo[Product_Info];
		ini[_T("JOB_DATA")][_T("LOT_Info")] = m_strModuleInfo[LOT_Info];
		ini[_T("JOB_DATA")][_T("Product_Group")] = m_strModuleInfo[Product_Group];
		ini[_T("JOB_DATA")][_T("From_Site")] = m_strModuleInfo[From_Site];
		ini[_T("JOB_DATA")][_T("Current_Site")] = m_strModuleInfo[Current_Site];
		ini[_T("JOB_DATA")][_T("From_Shop")] = m_strModuleInfo[From_Shop];
		ini[_T("JOB_DATA")][_T("Current_Shop")] = m_strModuleInfo[Current_Shop];
		ini[_T("JOB_DATA")][_T("Thickness")] = m_strModuleInfo[Thickness];
		ini[_T("JOB_DATA")][_T("MMGFlag")] = m_strModuleInfo[MMGFlag];
		ini[_T("JOB_DATA")][_T("PANELSIZE")] = m_strModuleInfo[PANELSIZE];
		ini[_T("JOB_DATA")][_T("PGrade")] = m_strModuleInfo[PGRADE];
	}

	return TRUE;
}