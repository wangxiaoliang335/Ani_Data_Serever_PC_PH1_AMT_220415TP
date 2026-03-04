// StringSupport.cpp: implementation of the StringSupport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StringSupport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CString CStringSupport::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int width)
{
	static const _TCHAR szThreeDots[]=_T("...");
	CString convString = lpszLong;

	int nStringLen=lstrlen(lpszLong);
	int pixelWidth = pDC->GetTextExtent(lpszLong,nStringLen).cx;
	if(nStringLen==0 || pixelWidth < width+1)
		return lpszLong;

	int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

	CString tmpString;
	int start = 0 , end = nStringLen;
	int half = nStringLen / 2;
	while ((start != end) && half ) {
		tmpString = convString.Left(start+half);
		if(pDC->GetTextExtent(tmpString,half).cx + nAddLen < width) {
			start += half;
		} else {
			end -= half;
		}
		half /= 2;
	}

	return convString.Left(start) + szThreeDots;
}

int CStringSupport::WildCmp(LPCTSTR wild, LPCTSTR string)
{
	LPCTSTR cp = NULL;
	LPCTSTR mp = NULL;
	
	while ((*string) && (*wild != '*')) {
		if ((*wild != *string) && (*wild != '?')) {
			return 0;
		}
		wild++;
		string++;
	}
		
	while (*string) {
		if (*wild == '*') {
			if (!*++wild) {
				return 1;
			}
			mp = wild;
			cp = string+1;
		} else if ((*wild == *string) || (*wild == '?')) {
			wild++;
			string++;
		} else if (cp != NULL && mp != NULL) {
			wild = mp;
			string = cp++;
		}
	}
		
	while (*wild == '*') {
		wild++;
	}
	return !*wild;
}

CString CStringSupport::N2C(long n) 
{
	CString str;
	str.Format(_T("%d"), n);

	return str;
}

CString CStringSupport::N2C(int n) 
{
	CString str;
	str.Format(_T("%d"), n);

	return str;
}

CString CStringSupport::N2C(double f, LPCTSTR format) 
{
	CString str;
	str.Format(format, f);

	return str;
}

CString CStringSupport::GetResString(UINT resID, HINSTANCE hModule)
{
	CString text;
	if (hModule != 0) {

		HINSTANCE hInstResource = AfxGetResourceHandle();
		AfxSetResourceHandle(hModule);

		text.LoadString(resID);

		AfxSetResourceHandle(hInstResource);

	} else {
		text.LoadString(resID);
	}

	return text;
}

BOOL CStringSupport::GetGuid(CString& rString)
{
	CString strFormat = _T("%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X");

	GUID guidValue = GUID_NULL;
	::CoCreateGuid(&guidValue);
	if (guidValue == GUID_NULL) {
		return FALSE;
	}
 
	// then format into destination
	rString.Format(strFormat,
		// first copy...
		guidValue.Data1, guidValue.Data2, guidValue.Data3,
		guidValue.Data4[0], guidValue.Data4[1], guidValue.Data4[2], guidValue.Data4[3],
		guidValue.Data4[4], guidValue.Data4[5], guidValue.Data4[6], guidValue.Data4[7],
		// second copy...
		guidValue.Data1, guidValue.Data2, guidValue.Data3,
		guidValue.Data4[0], guidValue.Data4[1], guidValue.Data4[2], guidValue.Data4[3],
		guidValue.Data4[4], guidValue.Data4[5], guidValue.Data4[6], guidValue.Data4[7]);

	return TRUE;
}

CString CStringSupport::GetLanguageID()
{
	LCID lcid = GetThreadLocale();
	switch (lcid)
	{
	case 0x412:
		return _T("ko");
	default:
		return _T("en");
	}
}

CString CStringSupport::FormatString(LPCTSTR pstrFormat, ...)
{
	CString str;

	va_list args;
	va_start(args, pstrFormat);
	str.FormatV(pstrFormat, args);

	return str;
}

CString CStringSupport::GetString(CStringList& stringList, LPCTSTR lineMarker)
{		
	size_t markerLen = _tcslen(lineMarker);

	SIZE_T length = 0;
	POSITION posList = stringList.GetHeadPosition();
	while (posList != NULL)
		length += stringList.GetNext(posList).GetLength();

	length += stringList.GetCount() * markerLen;

	CString str = _T("");
	LPTSTR strBuf = str.GetBuffer((int)(length+2));

	int pos = 0;
	if (lineMarker == 0)
	{
		posList = stringList.GetHeadPosition();
		while (posList != NULL)
			pos += _stprintf_s( strBuf+pos, length + 2, _T("%s"), (LPCTSTR)stringList.GetNext(posList));
	}
	else
	{
		posList = stringList.GetHeadPosition();
		while (posList != NULL)
			pos += _stprintf_s( strBuf+pos, length + 2, _T("%s%s"), (LPCTSTR)stringList.GetNext(posList), lineMarker);
	}
	str.ReleaseBuffer();

	return str;
}

CString CStringSupport::GetErrorMessage()
{
	CString errorMsg;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, errorMsg.GetBuffer(128), 128 , 0);
	errorMsg.ReleaseBuffer();
	
	return errorMsg;
}

void CStringSupport::CopyStringList(CStringList& srcList, CStringList& destList)
{
	CString str;
	POSITION pos = srcList.GetHeadPosition();
	while (pos != NULL)
	{
		str = srcList.GetNext(pos);
		str.TrimRight();
		destList.AddTail(str);
	}
}

void CStringSupport::GetTokenArray(CString& content, TCHAR delimiter, CStringArray& stringArray)
{
	int startIndex = 0;
	int foundIndex = 0;
	do
	{
		foundIndex = content.Find(delimiter, startIndex);

		if (foundIndex == -1)
			stringArray.Add(content.Mid(startIndex));
		else
			stringArray.Add(content.Mid(startIndex, foundIndex - startIndex));
		startIndex = foundIndex + 1;
	}
	while (foundIndex != -1);
}


CString CStringSupport::ToWString(char* strData, int strSize)
{
	char* pTempStr = new char[strSize + 1];
	memcpy_s(pTempStr, strSize + 1, strData, strSize);
	pTempStr[strSize] = '\0';
	CString msg = CString(CStringA(pTempStr));
	delete pTempStr;
	return msg.Trim();
}

CString CStringSupport::ToShorString(USHORT strData)
{
	char pTempStr;;
	pTempStr = strData;
	CString msg = CString(CStringA(pTempStr));
	return msg;
}

char* CStringSupport::ToAString(LPCTSTR strData, char* destData, int destSize)
{
	int strLen = _tcslen(strData);

	memset(destData, 0x20, destSize);
	CStringA tempStr(strData);
	memcpy_s(destData, destSize, tempStr.GetBuffer(strLen), __min(tempStr.GetLength(), destSize));
	tempStr.ReleaseBuffer();

	return destData;
}
