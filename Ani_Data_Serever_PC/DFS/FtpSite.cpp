/****************************************************************/
/*																*/
/*  FtpSite.cpp													*/
/*																*/
/*  Implementation of the CFtpSite class.						*/
/*																*/
/*  Programmed by Pablo van der Meer							*/
/*  Copyright Pablo Software Solutions 2002						*/
/*	http://www.pablovandermeer.nl								*/
/*																*/
/*  Last updated: 5 may 2002									*/
/*																*/
/****************************************************************/

#include "stdafx.h"
#include "FtpSite.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CFtpSite, CObject, 1)

CFtpSite::CFtpSite()
{
	m_nRetries = 1;
	m_nPort = 21;
	m_nRetryDelay = 15;
	m_bUseFirewall = FALSE;
	m_bUsePASVMode = FALSE;
}

CFtpSite::~CFtpSite()
{

}

void CFtpSite::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// 'store' data
		ar << m_strAddress;
		ar << m_nRetries;
		ar << m_strDescription;
		ar << m_strLocalPath;
		ar << m_strLogin;
		ar << m_strName;
		ar << m_strPassword;
		ar << m_nPort;
		ar << m_strRemotePath;
		ar << m_nRetryDelay;
		ar << m_bUseFirewall;
		ar << m_bUsePASVMode;
	}
	else
	{
		// 'load' data
		ar >> m_strAddress;
		ar >> m_nRetries;
		ar >> m_strDescription;
		ar >> m_strLocalPath;
		ar >> m_strLogin;
		ar >> m_strName;
		ar >> m_strPassword;
		ar >> m_nPort;
		ar >> m_strRemotePath;
		ar >> m_nRetryDelay;
		ar >> m_bUseFirewall;
		ar >> m_bUsePASVMode;
	}
}


/* Copy-constructor */
CFtpSite::CFtpSite(const CFtpSite &ftpSite)
{
	m_strAddress = ftpSite.m_strAddress;
	m_nRetries = ftpSite.m_nRetries;
	m_strDescription = ftpSite.m_strDescription;
	m_strLocalPath = ftpSite.m_strLocalPath;
	m_strLogin = ftpSite.m_strLogin;
	m_strName = ftpSite.m_strName;
	m_strPassword = ftpSite.m_strPassword;
	m_nPort = ftpSite.m_nPort;
	m_strRemotePath = ftpSite.m_strRemotePath;
	m_nRetryDelay = ftpSite.m_nRetryDelay;
	m_bUseFirewall = ftpSite.m_bUseFirewall;
	m_bUsePASVMode = ftpSite.m_bUsePASVMode;
}

/* = operator definition */
CFtpSite& CFtpSite::operator=(const CFtpSite &ftpSite)
{
	if (&ftpSite != this)
	{
		m_strAddress = ftpSite.m_strAddress;
		m_nRetries = ftpSite.m_nRetries;
		m_strDescription = ftpSite.m_strDescription;
		m_strLocalPath = ftpSite.m_strLocalPath;
		m_strLogin = ftpSite.m_strLogin;
		m_strName = ftpSite.m_strName;
		m_strPassword = ftpSite.m_strPassword;
		m_nPort = ftpSite.m_nPort;
		m_strRemotePath = ftpSite.m_strRemotePath;
		m_nRetryDelay = ftpSite.m_nRetryDelay;
		m_bUseFirewall = ftpSite.m_bUseFirewall;
		m_bUsePASVMode = ftpSite.m_bUsePASVMode;
	}
	return *this;
}

/*--- END OF FtpSite.cpp ---*/
