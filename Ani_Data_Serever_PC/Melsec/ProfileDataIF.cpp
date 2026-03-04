// @file ProfileDataIF.cpp: implementation of the CProfileDataIF class.
//
//_____________________________________________________________________________

#include "stdafx.h"
#include "ProfileDataIF.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//_____________________________________________________________________________
// Construction/Destruction
CProfileDataIF::CProfileDataIF()
{
}

CProfileDataIF::~CProfileDataIF()
{
}

//_____________________________________________________________________________
// file Read function
CString CProfileDataIF::GetString(CString node, CString key, CString default_value)
{
	CString tmp_s;
	TCHAR buff[100];

	try
	{
		GetPrivateProfileString(node, key, default_value, buff, sizeof(buff), m_sFilePath);
		tmp_s.Format(_T("%s"), buff);
		return tmp_s;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return default_value;
	}
}

int CProfileDataIF::GetInt(CString node, CString key, int default_value)
{
	CString tmp_s;
	TCHAR buff[100];

	try
	{
		tmp_s.Format(_T("%d"), default_value);
		GetPrivateProfileString(node, key, tmp_s, buff, sizeof(buff)-1, m_sFilePath);
		return _ttoi(buff);
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return default_value;
	}
}

long CProfileDataIF::GetLong(CString node, CString key, long default_value)
{
	CString tmp_s;
	TCHAR buff[100];

	try
	{
		tmp_s.Format(_T("%ld"), default_value);
		GetPrivateProfileString(node, key, tmp_s, buff, sizeof(buff)-1, m_sFilePath);
		return _ttol(buff);
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return default_value;
	}
}

double CProfileDataIF::GetDouble(CString node, CString key, double default_value)
{
	CString tmp_s;
	TCHAR buff[100];

	try
	{
		tmp_s.Format(_T("%f"), default_value);
		GetPrivateProfileString(node, key, tmp_s, buff, sizeof(buff)-1, m_sFilePath);
		return _ttof(buff);
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return default_value;
	}
}

//_____________________________________________________________________________
// file Write function
BOOL CProfileDataIF::SetString(CString node, CString key, CString s_data)
{
	try
	{
		WritePrivateProfileString(node, key, s_data, m_sFilePath); 
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}


BOOL CProfileDataIF::SetInt(CString node, CString key, int i_data)
{
	CString tmp_s;
	try
	{
		tmp_s.Format(_T("%d"), i_data);
		WritePrivateProfileString(node, key, tmp_s, m_sFilePath); 
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}

BOOL CProfileDataIF::SetLong(CString node, CString key, long l_data)
{
	CString tmp_s;
	try
	{
		tmp_s.Format(_T("%ld"), l_data);
		WritePrivateProfileString(node, key, tmp_s, m_sFilePath); 
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}

BOOL CProfileDataIF::SetDouble(CString node, CString key, double d_data)
{
	CString tmp_s;
	try
	{
		tmp_s.Format(_T("%f"), d_data);
		WritePrivateProfileString(node, key, tmp_s, m_sFilePath); 
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}


BOOL CProfileDataIF::GetSectionNames(LPTSTR lpszReturnBuffer, DWORD nSize)
{
	try
	{
		GetPrivateProfileSectionNames(lpszReturnBuffer, nSize, m_sFilePath);
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}

BOOL CProfileDataIF::DeleteSection(CString node)
{
	try
	{
		// delete section
		WritePrivateProfileString(node, NULL, _T(""), m_sFilePath);
		return TRUE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}

BOOL CProfileDataIF::IsExistSection(CString node)
{
	CString tmp_s;
	TCHAR buff[1000];

	try
	{
		if (GetPrivateProfileSection(node, buff, sizeof(buff), m_sFilePath) > 0)
			return TRUE;
		else
			return FALSE;
	}
	catch (CException* perr)
	{
		perr->ReportError();
		perr->Delete();
		return FALSE;
	}
}
