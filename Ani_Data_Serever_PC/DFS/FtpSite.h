// FtpSite.h: interface for the CFtpSite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTPSITE_H__F8601FF4_46CE_11D6_AB39_00D0B70C3D79__INCLUDED_)
#define AFX_FTPSITE_H__F8601FF4_46CE_11D6_AB39_00D0B70C3D79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CFtpSite : public CObject  
{
	DECLARE_SERIAL(CFtpSite)

	CFtpSite();
	virtual ~CFtpSite();

public:
	virtual void Serialize(CArchive&);
	CFtpSite (const CFtpSite &ftpSite);				// copy-constructor
	CFtpSite &operator=(const CFtpSite &ftpSite);	// =-operator

	CString	m_strAddress;
	int		m_nRetries;
	CString	m_strDescription;
	CString	m_strLocalPath;
	CString	m_strLogin;
	CString	m_strName;
	CString	m_strPassword;
	int		m_nPort;
	CString	m_strRemotePath;
	int		m_nRetryDelay;
	BOOL	m_bUseFirewall;
	BOOL	m_bUsePASVMode;
};

#endif // !defined(AFX_FTPSITE_H__F8601FF4_46CE_11D6_AB39_00D0B70C3D79__INCLUDED_)

/*--- END OF FtpSite.h ---*/
