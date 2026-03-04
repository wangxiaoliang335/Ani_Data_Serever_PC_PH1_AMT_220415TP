// FTPClient.h: interface for the CFTPClient class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxinet.h>
#include "FtpSite.h"
#include "DFSSession.h"
#include <queue>
using namespace std;

//#define BUF_SIZE 4096
#define BUF_SIZE 20480
const CString AMT_FS_IP_ADDRESS[FS_IPAdressCount] =
{
	_T("172.18.208.4"),
	_T("172.18.208.7"),
	_T("172.18.208.10"),
	_T("172.18.208.13"),
	_T("172.18.208.16"),
};

const CString AFT_FS_IP_ADDRESS[FS_IPAdressCount] =
{
	_T("172.18.208.73"),
	_T("172.18.208.76"),
	_T("172.18.208.79"),
	_T("172.18.208.85"),
	_T("172.18.208.82"),
};

const CString GAMMA_FS_IP_ADDRESS[FS_IPAdressCount] =
{
	_T("172.18.208.163"),
	_T("172.18.208.190"),
	_T("172.18.208.193"),
	_T("172.18.208.196"),
	_T("172.18.208.199"),
};

class CFTPClient
{
public:
	CFTPClient();
	virtual ~CFTPClient();

	BOOL bActive;
	CDFSSession *m_pDFSSession;
	CFtpConnection *m_pFtpConnection;
	CFtpSite m_ftpSite;

	CString m_strCurrentDirectory;
	CString m_strLastDirectory;
	CString m_strRootDirectory;

	CString m_strServerName;
	BOOL    m_bConnectState;

	void Connect(LPCTSTR lpszSiteName, LPCTSTR lpszUserName, LPCTSTR lpszPassword);
	void UploadFile(CString &source, CString &dest, BOOL &bSend);
	void DownLoadFile(CString &source, CString &dest, BOOL &bSend);
	BOOL CheckDirectory(CString strPath); //140607 JSLee

	
	queue<SJobDataShop> m_transferFileList;

	BOOL CreateTask();
	void CloseTask();

	void AddTransferFile(SJobDataShop strTransferFile);

	static UINT FtpUploadTask(LPVOID pParam);
	void RunFtpUploadThread();
	void Disconnect();
	BOOL DataCompare(CString strFilePath, SJobDataShop pJobData);
	int		m_nRetries;
	int		m_nRetryDelay;
	CString m_strPassword;
	CString m_strUserName;
	int		m_bUsePASVMode;
	int		m_nPort;
	CString m_strAddress;
	CString m_strDescription;
	CString m_strLocalPath;
	CString m_strLogin;
	CString m_strName;
	CString m_strRemotePath;
	CString m_strFileServerName;
	CString strDest, strSrc;
	CString m_strModuleInfo[ModuleData_Count];
	HANDLE m_hQuit;
	CWinThread *m_pThreadFtpUpload;
	CCriticalSection m_csLock;
};