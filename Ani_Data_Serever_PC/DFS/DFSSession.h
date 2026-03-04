// DFSSession.h: interface for the CMyInternetSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DFSSESSION_H__254FC5C5_3B0E_11D6_AB38_00D0B70C3D79__INCLUDED_)
#define AFX_DFSSESSION_H__254FC5C5_3B0E_11D6_AB38_00D0B70C3D79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxinet.h>
class CDFSSession : public CInternetSession
{
public:
	CDFSSession(LPCTSTR pstrAgent = NULL, DWORD dwContext = 1, DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG, LPCTSTR pstrProxyName = NULL, LPCTSTR pstrProxyBypass = NULL, DWORD dwFlags = 0 ) 
		: CInternetSession(pstrAgent, dwContext, dwAccessType, pstrProxyName, pstrProxyBypass, dwFlags) 
		{ 	
			m_pMainWnd = NULL;	
		};

// Attributes
public:
	HWND m_pMainWnd;  // pointer to parent window
// Operations
public:
	CString GetErrorString(DWORD dwErrorCode);

	virtual void OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInfomration, DWORD dwStatusInformationLen);
};



#endif // !defined(AFX_DFSSESSION_H__254FC5C5_3B0E_11D6_AB38_00D0B70C3D79__INCLUDED_)

/*--- END OF DFSSession.h ---*/
