// ProfileDataIF.h: interface for the CProfileDataIF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROFILEDATAIF_H__4DFF9E8B_25BC_4B96_B653_5CA1F2E3EA01__INCLUDED_)
#define AFX_PROFILEDATAIF_H__4DFF9E8B_25BC_4B96_B653_5CA1F2E3EA01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProfileDataIF : public CObject
{
private:
	CString m_sFilePath;

public:
	CProfileDataIF();
	virtual ~CProfileDataIF();

	// file Read function
	CString GetFilePath()
	{
		return m_sFilePath;
	}
	CString GetString(CString node, CString key, CString default_value);
	int GetInt(CString node, CString key, int default_value);
	long GetLong(CString node, CString key, long default_value);
	double GetDouble(CString node, CString key, double default_value);
	BOOL GetSectionNames(LPTSTR lpszReturnBuffer, DWORD nSize);
	BOOL IsExistSection(CString node);

	// file write function
	void SetFilePath(CString filepath)
	{
		m_sFilePath = filepath;
	}
	BOOL SetString(CString node, CString key, CString s_data);
	BOOL SetInt(CString node, CString key, int i_data);
	BOOL SetLong(CString node, CString key, long l_data);
	BOOL SetDouble(CString node, CString key, double d_data);
	BOOL DeleteSection(CString node);
};

#endif // !defined(AFX_PROFILEDATAIF_H__4DFF9E8B_25BC_4B96_B653_5CA1F2E3EA01__INCLUDED_)
