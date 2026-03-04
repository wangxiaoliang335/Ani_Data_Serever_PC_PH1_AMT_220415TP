// StringSupport.h: interface for the StringSupport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRINGSUPPORT_H__44B5E3F1_76BF_4361_8CC1_F047E0AA012B__INCLUDED_)
#define AFX_STRINGSUPPORT_H__44B5E3F1_76BF_4361_8CC1_F047E0AA012B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStringSupport
{
public:
	static CString MakeShortString(CDC* pDC, LPCTSTR lpszLong, int width);
	static int WildCmp(LPCTSTR wild, LPCTSTR string);
	static CString N2C(long n);
	static CString N2C(int n);
	static CString N2C(double f, LPCTSTR format = _T("%.3f"));
	static BOOL GetGuid(CString& rString);
	static CString GetResString(UINT resID, HINSTANCE hModule = 0);
	static CString GetLanguageID();
	static CString FormatString(LPCTSTR pstrFormat, ...);
	static CString GetString(CStringList& stringList, LPCTSTR lineMarker = _T("\r\n"));
	static CString GetErrorMessage();
	static void CopyStringList(CStringList& srcList, CStringList& destList);
	static void GetTokenArray(CString& content, TCHAR delimiter, CStringArray& stringArray);

	static CString ToWString(char* strData, int strSize);
	static CString ToShorString(USHORT strData);
	static char* ToAString(LPCTSTR strData, char* destData, int destSize);
};

#define BOOL_STR(x) ((x)?_T("TRUE"):_T("FALSE"))

#endif // !defined(AFX_STRINGSUPPORT_H__44B5E3F1_76BF_4361_8CC1_F047E0AA012B__INCLUDED_)
