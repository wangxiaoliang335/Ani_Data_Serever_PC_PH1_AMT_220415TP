#include "stdafx.h"
#include "AniUtil.h"
#include <math.h>
#include <string>
#include <regex>
#include <stdlib.h>

using namespace std;

#ifdef _UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif

//______________________________________________________________________________
/** @brief Event¸¦ Ã³¸®ÇÑ´Ù.			 										 
*/
void DoEvents()
{
	MSG message;
	if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}
}

/** @brief msµ¿¾È WaitingÇÑ´Ù.			 										 
*/
void Delay(long ms, BOOL bEvent)
{
	double s, e;
	double Ms = (double) ms;
	s = (double) GetCurrentTime();
	do
	{
		if(bEvent)
			DoEvents();

		Sleep(1);
		e = (double) GetCurrentTime();
	}
	while ((e - s) < Ms);
}



//______________________________________________________________________________
/** @brief ¹Ý¿Ã¸² Ã³¸®
*/
int RoundUp(double value)
{
	return (int) ((value > 0) ? value + 0.9 : value - 0.9);
}

int RoundupInt(double value)
{
	return (int) ((value > 0) ? value + 0.5 : value - 0.5);
}

long RoundupLong(double value)
{
	return (long) ((value > 0) ? value + 0.5 : value - 0.5);
}

/** @brief Å«°ª 
*/
double GetMax(double v1, double v2)
{
	return (double) ((v1 > v2) ? v1 : v2);
}

/** @brief ±Ù»çÄ¡ È®ÀÎ 
*/
bool IsAboutSame(double ref, double v1, double in_pos)
{
	return ( (ref-in_pos < v1) && (ref+in_pos > v1) );
}


/** @brief »ê¼ú Æò±Õ ±¸ÇÏ±â
*/
double GetMean(double* array, int size)
{
	double sum = 0.0;

	for (int i = 0; i < size; i++)
		sum += array[i];

	return sum / size;
}

/** @brief Ç¥ÁØ ÆíÂ÷ °è»ê 
*/
double GetStandardDeviation(double* array, int size, int option)
{
	// Option 0 / 1
	// Ç¥º»ÀÇ Ç¥ÁØÆíÂ÷ ±¸ÇÏ±â (STDEV) : GetStandardDeviation(foo, sizeof(foo) / sizeof(foo[0]), 1));
	// ¸ðÁý´Ü ÀüÃ¼ÀÇ Ç¥ÁØÆíÂ÷ ±¸ÇÏ±â (STDEVP) : GetStandardDeviation(foo, sizeof(foo) / sizeof(foo[0]), 0));

	// ¹è¿­ ¿ä¼Ò°¡ 1°³¹Û¿¡ ¾øÀ» ¶§´Â
	// NaN(¼ýÀÚ°¡ ¾Æ´Ô)ÀÌ¶ó´Â ÀÇ¹Ì·Î
	// -1.#IND0 À» ¹ÝÈ¯
	if (size < 2) return sqrt(-1.0);

	double sum = 0.0;
	double sd = 0.0;
	double diff;
	double meanValue = GetMean(array, size);

	for (int i = 0; i < size; i++) {
		diff = array[i] - meanValue;
		sum += diff * diff;
	}
	sd = sqrt(sum / (size - option));

	return sd;
}

// ÇÇÅ¸°í¶ó½º Á¤¸® ( ´ë°¢ÀÇ ±æÀÌ ¿Í ¹Øº¯À» °¡Áö°í ³ôÀÌ ±¸ÇÏ±â)
double GetPitaHight(double x, double y)
{
	// x = ´ë°¢, y = ¹Øº¯
	return sqrt((x*x) - (y*y));
}
// ÇÇÅ¸°í¶ó½º Á¤¸® ( ³ôÀÌ ¿Í ¹Øº¯À» °¡Áö°í ´ë°¢ÀÇ ±æÀÌ ±¸ÇÏ±â)
double GetPita(double x, double y)
{
	// x = ¹Øº¯, y = ³ôÀÌ
	return sqrt((x*x) + (y*y));
}

//______________________________________________________________________________
/** @brief um -> mm (Cstring)
 @return "0.001"
*/
CString GetDouble2String_MM(double tmp_d)
{
	CString strValue;
	tmp_d *= 0.001;
	strValue.Format(_T("%6.3f"),tmp_d);
	return strValue;
}

//______________________________________________________________________________
/** @brief ½Ã½ºÅÛ ³¯Â¥¸¦ ±¸ÇÏ´Â ÇÔ¼ö
 @return "1999-00-00"
*/
CString GetDateString()
{
	COleDateTime now;
	CString strDate;
	now = COleDateTime::GetCurrentTime();
	strDate.Format(_T("%d-%02d-%02d"), now.GetYear(), now.GetMonth(), now.GetDay());
	return strDate;
}
CString GetDateStringChangeDay(int iChngDay)
{
	COleDateTime now;
	CString strDate;
	now = COleDateTime::GetCurrentTime();
	now.m_dt += iChngDay;
	strDate.Format(_T("%d-%02d-%02d"), now.GetYear(), now.GetMonth(), now.GetDay());
	return strDate;
}
//______________________________________________________________________________
/** @brief ½Ã½ºÅÛ ³¯Â¥¸¦ ±¸ÇÏ´Â ÇÔ¼ö2
 @return "19990000"
*/
CString GetDateString2()
{
	COleDateTime now;
	CString strDate;
	now = COleDateTime::GetCurrentTime();
	strDate.Format(_T("%d%02d%02d"), now.GetYear(), now.GetMonth(), now.GetDay());
	return strDate;
}
//______________________________________________________________________________
/** @brief ½Ã½ºÅÛ ³¯Â¥,½Ã°£¸¦ ±¸ÇÏ´Â ÇÔ¼ö2
// @return "1999000000" << 131110 kmh
 @return "99000000"
*/
CString GetDateString3()
{
	COleDateTime now;
	CString strDate;
	now = COleDateTime::GetCurrentTime();
//<< 131110 kmh
//	strDate.Format("%04d%02d%02d%02d", now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour());
	strDate.Format(_T("%02d%02d%02d%02d"), now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour());
//<<
	return strDate;
}

//______________________________________________________________________________
/** @brief ½Ã½ºÅÛ ³¯Â¥,½Ã°£¸¦ ±¸ÇÏ´Â ÇÔ¼ö2
 @return "1999000000"
*/
CString GetDateString4()
{
	COleDateTime now;
	CString strDate;
	now = COleDateTime::GetCurrentTime();
	strDate.Format(_T("%04d%02d%02d%02d%02d%02d"), now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond());
	return strDate;
}

//______________________________________________________________________________
/** @brief ½Ã½ºÅÛ ³¯Â¥,½Ã°£¸¦ ±¸ÇÏ´Â ÇÔ¼ö5
 @return "0000"
*/
CString GetDateString5()
{
	COleDateTime now;
	CString strDate;
	now = COleDateTime::GetCurrentTime();
	strDate.Format(_T("%02d-%02d"), now.GetMonth(), now.GetDay());
	return strDate;
}

//______________________________________________________________________________
/** @brief ½Ã½ºÅÛ ³¯Â¥,½Ã°£¸¦ ±¸ÇÏ´Â ÇÔ¼ö5
@return "0000/00/00 00:00:00"
*/
CString GetDateString6()
{
	COleDateTime now;
	CString strDate;
	now = COleDateTime::GetCurrentTime();
	strDate.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond());
	return strDate;
}

//______________________________________________________________________________
/** @brief AlarmDateString
@return "0000/00/00
*/
CString GetDateString7()
{
	COleDateTime now;
	CString strDate;
	now = COleDateTime::GetCurrentTime();
	strDate.Format(_T("%04d/%02d/%02d"), now.GetYear(), now.GetMonth(), now.GetDay());
	return strDate;
}

//______________________________________________________________________________
/** @brief ½Ã½ºÅÛ ³¯Â¥,½Ã°£¸¦ Æú´õ·Î ±¸ÇÏ´Â ÇÔ¼ö4
 @return "1999\\00\\00\\00\\"
*/
CString GetDateString4Folder()
{
	COleDateTime now;
	CString strDate;
	now = COleDateTime::GetCurrentTime();
	strDate.Format(_T("%04d\\%02d\\%02d\\%02d\\"), now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour());
	return strDate;
}
//______________________________________________________________________________
/** @brief ½Ã½ºÅÛ ½Ã°£À» ±¸ÇÏ´Â ÇÔ¼ö 
 @return "00:00:00"
*/
CString GetTimeString()
{
	COleDateTime now;
	CString strTime;
	now = COleDateTime::GetCurrentTime();
	strTime.Format(_T("%02d:%02d:%02d"), now.GetHour(), now.GetMinute(), now.GetSecond());
	return strTime;
}

//______________________________________________________________________________
/** @brief ½Ã½ºÅÛ ½Ã°£À» ±¸ÇÏ´Â ÇÔ¼ö2
 @return "000000"
*/
CString GetTimeString2()
{
	COleDateTime now;
	CString strTime;
	now = COleDateTime::GetCurrentTime();
	strTime.Format(_T("%02d%02d%02d"), now.GetHour(), now.GetMinute(), now.GetSecond());
	return strTime;
}

//______________________________________________________________________________
/** @brief ½Ã½ºÅÛ ½Ã°£À» ±¸ÇÏ´Â ÇÔ¼ö3
 @return "000000"
*/
CString GetTimeString3()
{
	COleDateTime now;
	CString strTime;
	now = COleDateTime::GetCurrentTime();
	strTime.Format(_T("%02d%02d%02d%02d%02d"), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond());
	return strTime;
}
//________________________________________________________________________________
/** @brief ½Ã½ºÅÛ ½Ã°£À» ±¸ÇÏ´Â ÇÔ¼ö3
 @return "00"
 */
CString GetTimeString4()
{
	COleDateTime now;
	CString strDate;
	now = COleDateTime::GetCurrentTime();
	strDate.Format(_T("%02d"), now.GetHour());
	return strDate;
}
//______________________________________________________________________________
/** CSOT DFS INSPECTION TIME FORMAT YYYY/MM/DD HH:MM:SS
@return "0000/00/00 00:00:00"
*/
CString GetTimeString5(CString strContents)
{
	CString strData;
	CString strYear, strMonth, strDay, strHour, strMinute, strSecond;
	strYear = strContents.Left(4);
	strMonth = strContents.Mid(4, 2);
	strDay = strContents.Mid(6, 2);
	strHour = strContents.Mid(8, 2);
	strMinute = strContents.Mid(10, 2);
	strSecond = strContents.Right(2);

	strData = CStringSupport::FormatString(_T("%s/%s/%s %s:%s:%s"), strYear, strMonth, strDay, strHour, strMinute, strSecond);
	return strData;
}
//______________________________________________________________________________
int String2Hex(CString strVal)
{
	if (strVal.IsEmpty())
		return 0x0;
	int value;
	strVal.MakeUpper();
//>> 130227 JSPark
#if _MSC_VER > 1200 // VS6.0 ÀÌ»ó
#ifdef _MBCS
	sscanf_s(strVal.GetString(), _T("%X"), &value);
#else
	size_t bufSize = strVal.GetLength()+1;
	size_t ret;
	char * szchar = new char[bufSize];
	memset(szchar, 0, bufSize);
	wcstombs_s(&ret, szchar, bufSize, strVal.GetBuffer(), bufSize); 
	
	sscanf_s(szchar, "%X", &value);
	delete [] szchar;
#endif
#else // VS6.0 ÀÌÇÏ
	_stscanf(strVal, "%X", &value);
#endif
//<<
	return value;
}

CString Hex2String(int value)
{
	CString strVal;
	strVal.Format(_T("%X"), value);
	return strVal;
}

int Hex2Int(char cHex)
{
	switch(cHex){
		case 'F':
		case 'f':	return(15);
		case 'E':
		case 'e':	return(14);
		case 'D':
		case 'd':	return(13);
		case 'C':
		case 'c':	return(12);
		case 'B':
		case 'b':	return(11);
		case 'A':
		case 'a':	return(10);
		case '9':	return(9);
		case '8':	return(8);
		case '7':	return(7);
		case '6':	return(6);
		case '5':	return(5);
		case '4':	return(4);
		case '3':	return(3);
		case '2':	return(2);
		case '1':	return(1);
		default:	return(0);
	}
}

unsigned char Hex2Byte(char hex)
{
	switch(hex)
	{
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	case 'A': case 'a': return 10;
	case 'B': case 'b': return 11;
	case 'C': case 'c': return 12;
	case 'D': case 'd': return 13;
	case 'E': case 'e': return 14;
	case 'F': case 'f': return 15;
	}
	return 32;
}

//______________________________________________________________________________
int String2Int(CString strVal)
{
	if (strVal.IsEmpty())
		return 0;
	int value;
//>> 130227 JSPark
#if _MSC_VER > 1200 // VS6.0 ÀÌ»ó
#ifdef _MBCS
	sscanf_s(strVal.GetString(), _T("%X"), &value);
#else
	size_t bufSize = strVal.GetLength()+1;
	size_t ret;
	char * szchar = new char[bufSize];
	memset(szchar, 0, bufSize);
	wcstombs_s(&ret, szchar, bufSize, strVal.GetBuffer(), bufSize); 
	
	sscanf_s(szchar, "%d", &value);
	delete [] szchar;
#endif
#else // VS6.0 ÀÌÇÏ
	_stscanf(strVal, "%X", &value);
#endif
//<<
	return value;
}

CString Int2String(int value)
{
	CString strVal;
	strVal.Format(_T("%d"), value);
	return strVal;
}

//______________________________________________________________________________
float String2Float(CString strVal)
{
	if (strVal.IsEmpty())
		return 0.0f;
	float value;
//>> 130227 JSPark
#if _MSC_VER > 1200 // VS6.0 ÀÌ»ó
#ifdef _MBCS
	sscanf_s(strVal.GetString(), _T("%f"), &value);
#else
	size_t bufSize = strVal.GetLength()+1;
	size_t ret;
	char * szchar = new char[bufSize];
	memset(szchar, 0, bufSize);
	wcstombs_s(&ret, szchar, bufSize, strVal.GetBuffer(), bufSize); 
	
	sscanf_s(szchar, "%f", &value);
	delete [] szchar;
#endif
#else // VS6.0 ÀÌÇÏ
	_stscanf(strVal, "%f", &value);
#endif
//<<
	return value;
}

CString Float2String(float value, CString strFormat)
{
	CString strVal;
	strVal.Format(strFormat, value);
	return strVal;
}

//______________________________________________________________________________
double String2Double(CString strVal)
{
	if (strVal.IsEmpty())
		return 0.0;
	double value;
//>> 130227 JSPark
#if _MSC_VER > 1200 // VS6.0 ÀÌ»ó
#ifdef _MBCS
	sscanf_s(strVal.GetString(), _T("%lf"), &value);
#else
	size_t bufSize = strVal.GetLength()+1;
	size_t ret;
	char * szchar = new char[bufSize];
	memset(szchar, 0, bufSize);
	wcstombs_s(&ret, szchar, bufSize, strVal.GetBuffer(), bufSize); 
	
	sscanf_s(szchar, "%lf", &value);
	delete [] szchar;
#endif
#else // VS6.0 ÀÌÇÏ
	_stscanf(strVal, "%lf", &value);
#endif
//<<
	return value;
}

CString Double2String(double value, CString strFormat)
{
	CString strVal;
	strVal.Format(strFormat, value);
	return strVal;
}

//______________________________________________________________________________
BOOL FileExists(LPCTSTR strFilePath)
{
	CFileFind aFile;
	BOOL IsExist = aFile.FindFile(strFilePath);
	aFile.Close();
	return IsExist;
}

BOOL FileExists(LPCTSTR strStartPath, LPCTSTR strFileName)
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	tstring strFileFound = strStartPath;
	strFileFound += _T("\\*.*");		 
	
    WIN32_FIND_DATA info;
	
	BOOL bRet = FALSE;
	HANDLE hp = FindFirstFile(strFileFound.c_str(), &info);   //µð·ºÅä¸®¿¡ ÆÄÀÏÀÌ ÀÖ´ÂÁö Ã¹¹øÂ° ÆÄÀÏ¸¸.
    do
    {
        if (!((_tcscmp(info.cFileName, _T("."))==0) || (_tcscmp(info.cFileName, _T(".."))==0))) 
		{
            if((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)  //Subµð·ºÅä¸®°¡ Á¸ÀçÇÏ´Â°æ¿ì 
            {
                tstring strSubFolder = strStartPath;
				strSubFolder += _T("\\");
				strSubFolder += info.cFileName;
				
				if((bRet = FileExists(strSubFolder.c_str(), strFileName)))
					break;
            }
			else if(!_tcscmp(info.cFileName, strFileName))
			{
				bRet = TRUE;
				break;
			}
        }
		
    }while(FindNextFile(hp, &info));
	
    FindClose(hp);
	
	return bRet;
}

BOOL FileExists(CString& strStartFolder, CString strFileName, bool SearchSubFolder)
{
	CString strFilePath;
	if (strStartFolder.Right(1) != _T("\\"))
		strStartFolder += _T("\\");
	strFilePath = strStartFolder + strFileName;
	CFileFind aFile;
	BOOL IsExist = aFile.FindFile(strFilePath);
	if (IsExist || !SearchSubFolder)
	{
		aFile.Close();
		return IsExist;
	}

	CString strFile = strStartFolder + _T("*.*");
	IsExist = aFile.FindFile(strFile);
	if (IsExist)
	{
		IsExist = aFile.FindNextFile();
		while (IsExist)
		{
			if (aFile.IsDirectory() && !aFile.IsDots())
			{
				strFile = aFile.GetFilePath() + _T("\\") + strFileName;
				if (FileExists(strFile))
				{
					strStartFolder = aFile.GetFilePath();
					aFile.Close();
					return TRUE;
				}
			}
			IsExist = aFile.FindNextFile();
			Delay(2, TRUE);  //<< 150613 JYLee
		}
		if (aFile.IsDirectory() && !aFile.IsDots())
		{
			strFile = aFile.GetFilePath() + _T("\\") + strFileName;
			if (FileExists(strFile))
			{
				strStartFolder = aFile.GetFilePath();
				aFile.Close();
				return TRUE;
			}
		}
	}
	aFile.Close();
	return FALSE;
}

//______________________________________________________________________________
/** @brief Radian -> Degree ÇÔ¼ö
 @return Degree
*/
double Rad2Deg(double s)
{
	return (s * 180.0 / M_PI);
}

/** @brief Degree -> Radian ÇÔ¼ö
 @return Radian
*/
double Deg2Rad(double s)
{
	return (s * M_PI / 180.0);
}

CString GetExtractionCommaMsg(CString& strMsg)
{
	int count = strMsg.Find(',');
	CString m = strMsg.SpanExcluding(_T(","));
	if(count == -1)
	{
		strMsg = _T(""); 
		return m;
	}
	strMsg = strMsg.Mid(count+1);
	return m;
}

CString GetExtractionTabMsg(CString& strMsg)
{
	int count = strMsg.Find('\t');
	CString m = strMsg.SpanExcluding(_T("\t"));
	if(count == -1)
	{
		strMsg = _T(""); 
		return m;
	}
	strMsg = strMsg.Mid(count+1);
	return m;
}

CString GetExtractionDotMsg(CString& strMsg)
{
	int count = strMsg.Find('.');
	CString m = strMsg.SpanExcluding(_T("."));
	if(count == -1)
	{
		strMsg = _T(""); 
		return m;
	}
	strMsg = strMsg.Mid(count+1);
	return m;
}

CString GetExtractionSpaceMsg(CString& strMsg)
{
	int count = strMsg.Find(' ');
	CString m = strMsg.SpanExcluding(_T(" "));
	if(count == -1)
	{
		strMsg = _T(""); 
		return m;
	}
	strMsg = strMsg.Mid(count+1);
	return m;
}

CString GetExtractionDashMsg(CString& strMsg) //140404 JSPark
{
	int count = strMsg.Find('-');
	CString m = strMsg.SpanExcluding(_T("-"));
	strMsg = strMsg.Mid(count+1);
	return m;
}

CString GetExtractionColonMsg(CString& strMsg) //140404 JSPark
{
	int count = strMsg.Find(':');
	CString m = strMsg.SpanExcluding(_T(":"));
	strMsg = strMsg.Mid(count+1);
	return m;
}

CString Hex2Bin(CString& sHex)
{
	CString sBin;
	long	lValue;
	TCHAR *ptr;
	
	lValue = _tcstol(sHex, &ptr, 16);
	_ltot_s(lValue, sBin.GetBuffer(0), sBin.GetLength(), 2);

// 	CString sBin, sTmp;
// 	long n;
// 	char *str, *ptr;
// 
// 	str = new char[2];
// 	memset(str, NULL, sizeof(char)*2);
// 	for(int i=0; i< sHex.GetLength(); i++)
// 	{
// 		memcpy(str, sHex.Mid(i,1), 1);
// 		n = strtol(str, &ptr, 16);
// 		for(int j=0; j<4; j++)
// 		{
// 			itoa(n & 0x1, str,10);
// 			sTmp += str;
// 			n = n>>1;
// 		}
// 		sTmp.MakeReverse();
// 		sBin += sTmp;
// 		sTmp = _T("");
// 	}
// 
// 	delete [] str; str = NULL;

	return sBin;
}

extern CString GetNowSystemTimeMilliseconds()
{
	SYSTEMTIME local;
	CString strTime = _T("");
	
	//Year-month-Day Hour:Minute:Second:Milliseconds Display

	::GetLocalTime(&local);		
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d:%03d"),
		(int)local.wYear, 
		(int)local.wMonth, 
		(int)local.wDay ,
		(int)local.wHour, 
		(int)local.wMinute, 
		(int)local.wSecond,
		(int)local.wMilliseconds);

	return strTime;
}

extern CString GetNowSystemTimeMilliseconds2()
{
	SYSTEMTIME local;
	CString strTime = _T("");
	

	::GetLocalTime(&local);		
	strTime.Format(_T("%02d:%02d:%02d:%03d"),
		(int)local.wHour, 
		(int)local.wMinute, 
		(int)local.wSecond,
		(int)local.wMilliseconds);

	return strTime;
}
//>>JMhwang 12.02.08
extern CString GetNowSystemTimeMillisecondsSirius()
{
	SYSTEMTIME local;
	CString strTime = _T("");
	
	//Year-month-Day Hour:Minute:Second:Milliseconds Display
	
	::GetLocalTime(&local);		
	strTime.Format(_T("%04d%02d%02d_%02d%02d%02d"),
		(int)local.wYear, 
		(int)local.wMonth, 
		(int)local.wDay ,
		(int)local.wHour, 
		(int)local.wMinute, 
		(int)local.wSecond);

	return strTime;

}
extern CString GetNowSystemTimeMillisecondsSirius2()
{
	SYSTEMTIME local;
	CString strTime = _T("");
	
	//Year-month-Day Hour:Minute:Second:Milliseconds Display
	
	::GetLocalTime(&local);		
	strTime.Format(_T("%04d-%02d-%02d\t%02d:%02d:%02d:%03d"),
		(int)local.wYear, 
		(int)local.wMonth, 
		(int)local.wDay ,
		(int)local.wHour, 
		(int)local.wMinute, 
		(int)local.wSecond,
		(int)local.wMilliseconds);
	
	return strTime;
}

extern CString GetNowSystemTimeMillisecondsSirius3()
{
	SYSTEMTIME local;
	CString strTime = _T("");
	
	//Year-month-Day Hour:Minute:Second:Milliseconds Display
	
	::GetLocalTime(&local);		
	strTime.Format(_T("%02d:%02d:%02d:%03d"),
		(int)local.wHour, 
		(int)local.wMinute, 
		(int)local.wSecond,
		(int)local.wMilliseconds);
	
	return strTime;
}

extern CString GetNowSystemTimeMillisecondsSirius4()
{
	SYSTEMTIME local;
	CString strTime = _T("");

	::GetLocalTime(&local);
	strTime.Format(_T("%04d%02d%02d_%02d%02d%02d%03d"),
		(int)local.wYear,
		(int)local.wMonth,
		(int)local.wDay,
		(int)local.wHour,
		(int)local.wMinute,
		(int)local.wSecond,
		(int)local.wMilliseconds);

	return strTime;
}
//<<
//______________________________________________________________________________
// ¹®ÀÚ¿­ °ø¹é Ã¤¿ì±â
void FillSpace(TCHAR *pszIn, short nStr)
{
	TCHAR *pszDm = new TCHAR[_tcslen(pszIn) + 1];
	short	nLen = _tcslen(pszIn);
	
	memset(pszDm, 0, sizeof(TCHAR) * (nLen + 1));
	if (nLen != 0) { _tcsncpy_s(pszDm, nLen, pszIn, nLen); }
	
	memset(pszIn, ' ', nStr);	*(pszIn + nStr) = '\0';
	if (nLen == 0) { 
		delete [] pszDm;	pszDm = NULL; 
		return; 
	}
	if (nStr == 0) { 
		delete [] pszDm;	pszDm = NULL;
		return; 
	}
	if (nLen > nStr) { nLen = nStr; }
	
	_tcsncpy_s(pszIn, nLen, pszDm, nLen);	
	delete [] pszDm;	pszDm = NULL;
}

void Select_Sort(int parm_data[], int parm_count) 
{ 	
    int min_data = 0, min_index = 0; 	
    int i, j; 	
    int comparison_count = 0; 	
	
    for(i = 0; i < parm_count - 1; i++){ 		
        min_index = i; 		
        min_data = parm_data[i]; 		
        for(j = i + 1; j < parm_count; j++){ 			
            comparison_count++; 			
            if(min_data > parm_data[j]){ 				
                min_data = parm_data[j]; 				
                min_index = j; 				
            } 			
        } 		
        parm_data[min_index] = parm_data[i];		
        parm_data[i] = min_data; 		
    } 	
} 

// CString ¡æ Char
char* StringToChar(CString str)
{	
	char *szStr = NULL;	
#if defined(UNICODE) || defined(_UNICODE)	
	int nLen = str.GetLength() + 1;	
	TCHAR *tszTemp = NULL;	
	tszTemp = new TCHAR[nLen];	
	memset(tszTemp, 0x00, nLen*sizeof(TCHAR));	
	_tcscpy_s(tszTemp, nLen, str);	
	// Get size (½ÇÁ¦»ç¿ëµÇ´Â¹ÙÀÌÆ®»çÀÌÁî)	
	int nSize = WideCharToMultiByte(CP_ACP, 0, tszTemp, -1, NULL, NULL, NULL, NULL);	
	szStr = new char[nSize];	
	memset(szStr, 0x00, nSize);	
	WideCharToMultiByte(CP_ACP, 0, tszTemp, -1, szStr, nSize, NULL, NULL);	
	if(tszTemp)		
	{		
		delete [] tszTemp;		
		tszTemp = NULL;		
	}	
#else	
	int nLen = str.GetLength() + 1;	
	szStr = new char[nLen];	
	memset(szStr, 0x00, nLen);	
	strcpy(szStr, str);	
#endif	
	return szStr;	
}

// Char ¡æ CString
CString CharToString(TCHAR *str)
{	
	CString cStr;	
	cStr.Format(_T("%s"), str);	
	return cStr;	
}

//20180210 hacy Á¤±Ô½ÄÀ¸·Î ¼ýÀÚÃ¼Å©
BOOL GetNumberCheck(CString str)
{
	CT2CA CTString(str);
	string s(CTString);
	regex number("[0-9]+");
	if (regex_match(s, number))
		return FALSE;
	else
		return TRUE;

}

//20180703 hacy Á¤±Ô½ÄÀ¸·Î Ç×¸ñÁ¦°Å
//1. ¹®ÀÚ¿­ 2. Á¦°ÅÇÏ°í½ÍÀº Á¤±Ô½Ä ¹æ¹ý
//ex) SetStringReplace(_T("[abc]"), _T("[\\]\\[]"))   // result = abc;
void SetStringReplace(CString *str, CString reg)
{
	CT2CA CTString(*str);
	string s(CTString);
	CT2CA CTString2(reg);
	string reg2(CTString2);
	string buf;
	regex number(reg2);
	buf = regex_replace(s, number, "");

	*str = CA2CT(buf.c_str());
}

//>>20180608 0800 > 08:00 °°Àº ½Ã°£À¸·Î ¸¸µé¾îÁÖ´Â±â´É
CString GetTimeString(int iNum)
{
	CString strMsg, strSendMsg;
	strMsg.Format(_T("%d"), iNum);

	if (strMsg.GetLength() == 4)
		strSendMsg.Format(_T("%s:%s"), strMsg.Left(2), strMsg.Right(2));
	else if (strMsg.GetLength() == 3)
		strSendMsg.Format(_T("0%s:%s"), strMsg.Left(1), strMsg.Right(2));
	else if (strMsg.GetLength() == 2)
		strSendMsg.Format(_T("00:%s"), strMsg.Right(2));
	else
		strSendMsg.Format(_T("00:0%s"), strMsg.Right(1));

	return strSendMsg;
}

//20180219 CString Null Ã¼Å©
BOOL GetNullCheck(CString str , TCHAR parser)
{
	CStringArray responseTokens;
	CStringSupport::GetTokenArray(str, parser, responseTokens);

	for (int ii = 0; ii < responseTokens.GetSize(); ii++)
	{
		if (responseTokens[ii] == _T(""))
		{
			return TRUE;
		}
	}
	return FALSE;
}

CStringA CStringAToCstring(CString cstr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, LPCTSTR(cstr), -1, NULL, 0, NULL, NULL);
	char *str = new char[len];
	memset(str, 0, len * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstr), -1, str, len, NULL, NULL);
	CStringA cstrDestA = str;
	return cstrDestA;
}

char* CStringToUtf8(CString str)
{
	int MAXPACKETLEN = 999;
	char*   pPacket_ASCII = StringToChar(str);
	TCHAR*  pPacket_UNICODE = new TCHAR[MAXPACKETLEN * 2];
	char*   pPacket_UTF8 = new char[MAXPACKETLEN];

	// ASCII -> UNICODE
	MultiByteToWideChar(CP_ACP, 0, pPacket_ASCII, -1, pPacket_UNICODE, MAXPACKETLEN);

	// UNICODE -> UTF-8 
	int nLen = WideCharToMultiByte(CP_UTF8, 0, pPacket_UNICODE, -1, NULL, 0, NULL, NULL);

	// UNICODE -> UTF-8
	WideCharToMultiByte(CP_UTF8, 0, pPacket_UNICODE, -1, pPacket_UTF8, nLen, NULL, NULL);

	delete pPacket_ASCII;
	delete pPacket_UNICODE;

	return pPacket_UTF8;
}

int CheckAndCreateFolder(LPCTSTR szFolderPath)
{
	DWORD dwRet = GetFileAttributes(szFolderPath);
	if(dwRet != FILE_ATTRIBUTE_DIRECTORY)		// Æú´õ°¡ Á¸ÀçÇÏÁö ¾Ê´Â °æ¿ì.
	{	
		int iRet = CreateDirectory(szFolderPath, NULL);
		
		if( iRet==0 )						// »ý¼ºÇÏÁö ¸øÇÑ °æ¿ì. 
		{
			AfxMessageBox(_T("Directory Creation Fail.!!"));
			return -1;
		}
	}
	else
		return 0;
	
	return 1;
}

void DeleteFolderAndFile(LPCTSTR szFolderPath)
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	tstring strFileFound = szFolderPath;
	strFileFound += _T("\\*.*");		 
	
    WIN32_FIND_DATA info;

	HANDLE hp = FindFirstFile(strFileFound.c_str(), &info);   //µð·ºÅä¸®¿¡ ÆÄÀÏÀÌ ÀÖ´ÂÁö Ã¹¹øÂ° ÆÄÀÏ¸¸.
    do
    {
        if (!((_tcscmp(info.cFileName, _T("."))==0) || (_tcscmp(info.cFileName, _T(".."))==0))) 
		{
            if((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)  //Subµð·ºÅä¸®°¡ Á¸ÀçÇÏ´Â°æ¿ì 
            {
                tstring strSubFolder = szFolderPath;
				strSubFolder += _T("\\");
				strSubFolder += info.cFileName;

				DeleteFolderAndFile(strSubFolder.c_str()); /// {return (_Ptr == 0 ? _Nullstr() : _Ptr); } 
				//RemoveDirectory(strPath);		// ¿©±â¼­ È£ÃâÇÏ¸é ÇÏÀ§ Æú´õ¸¸ »èÁ¦
             }
            else
			{
				strFileFound = szFolderPath;
				strFileFound += _T("\\");
				strFileFound += info.cFileName;
				BOOL retVal = DeleteFile(strFileFound.c_str());
            }

        }

    }while(FindNextFile(hp, &info));
    
	FindClose(hp);

	RemoveDirectory(szFolderPath); //¿©±â¼­ È£ÃâÇÏ¸é ÃÖ»óÀ§ Æú´õ±îÁö »èÁ¦
}
BOOL GetFileNameOnly(CString & strFilePath)
{
	CString strPath;
	int nSize = strFilePath.ReverseFind('\\');
	strPath = strFilePath.Right(strFilePath.GetLength() - nSize - 1);
	strFilePath = strPath;
	return TRUE;
}

BOOL GetFileNameOnlyNoExt(CString & strFilePath)
{
	CString strPath;
	int nSize = strFilePath.ReverseFind('\\');
	strPath = strFilePath.Right(strFilePath.GetLength() - nSize - 1);
	strFilePath = strPath;
	nSize = strFilePath.ReverseFind('.');
	strPath = strFilePath.Left(nSize);
	strFilePath = strPath;
	return TRUE;
}
BOOL GetUpFolderPath(CString & strFilePath)
{
	CString strPath;
	CString TempPath;
	int nSize = strFilePath.ReverseFind('\\');
	if (nSize == -1)
		return FALSE;

	TempPath = strFilePath.Left(nSize - 2);

	nSize = TempPath.ReverseFind('\\');
	if (nSize == -1)
		return FALSE;

	strPath = TempPath.Left(nSize);

	strFilePath = strPath;
	PathEndCheck(strFilePath);
	return TRUE;
}
//BOOL GetPathOnly(CString & strFilePath)
//{
//	CString strPath;
//	int nSize = strFilePath.ReverseFind('\\');
//	if (nSize == -1)
//		return FALSE;
//
//	strPath = strFilePath.Left(nSize);
//	strFilePath = strPath;
//	PathEndCheck(strFilePath);
//	return TRUE;
//}
//BOOL PathEndCheck(CString & strPath)
//{
//	if (strPath == "")
//	{
//		strPath.Insert(strPath.GetLength(), '\\');
//		return FALSE;
//	}
//
//	if (strPath.GetAt(strPath.GetLength() - 1) != '\\')
//	{
//		strPath.Insert(strPath.GetLength(), '\\');
//		return FALSE;
//	}
//
//	return TRUE;
//}
BOOL CreateFolders(LPCTSTR szFolderPath)
{
	tstring strPath(szFolderPath);
	tstring strDir;
	DWORD dwRet;
	int iRet;

	int index(0);

	index = strPath.find(':', index);
	if(index < 0)
		index = 0;
	else
		index +=2;

	do
	{
		index = strPath.find('\\', index);
		
		if(index < 0)
			break;
		
		strDir = strPath.substr(0,index);

		if(strDir == _T("") || strDir == _T("\\"))
		{
			index++;
			continue;
		}

		dwRet = GetFileAttributes(strDir.c_str());
		if(dwRet != FILE_ATTRIBUTE_DIRECTORY)		// Æú´õ°¡ Á¸ÀçÇÏÁö ¾Ê´Â °æ¿ì.
		{
			iRet = CreateDirectory(strDir.c_str(), NULL);
			if( iRet == ERROR_PATH_NOT_FOUND )		// »ý¼ºÇÏÁö ¸øÇÑ °æ¿ì. 
				return FALSE;
		}

		index++;

	}while(1);

	//¸¶Áö¸· Æú´õ...
	dwRet = GetFileAttributes(strPath.c_str());
	if(dwRet != FILE_ATTRIBUTE_DIRECTORY)		// Æú´õ°¡ Á¸ÀçÇÏÁö ¾Ê´Â °æ¿ì.
	{
		int iRet = CreateDirectory(strPath.c_str(), NULL);
		if( iRet == ERROR_PATH_NOT_FOUND )		// »ý¼ºÇÏÁö ¸øÇÑ °æ¿ì. 
			return FALSE;
	}

	return TRUE;
} 

BOOL CopyFolderAndFile(LPCTSTR strSvrFolderPath, LPCTSTR strDefFolderPath)
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	tstring strFileFound = strSvrFolderPath;
	strFileFound += _T("\\*.*");
	tstring strDefFile;

	DWORD dwRet = GetFileAttributes(strDefFolderPath);
	if(dwRet != FILE_ATTRIBUTE_DIRECTORY)		// Æú´õ°¡ Á¸ÀçÇÏÁö ¾Ê´Â °æ¿ì.
	{
		int iRet = CreateDirectory(strDefFolderPath, NULL);
		if( iRet == ERROR_PATH_NOT_FOUND )		// »ý¼ºÇÏÁö ¸øÇÑ °æ¿ì. 
			return FALSE;
	}

	WIN32_FIND_DATA info;

	HANDLE hp = FindFirstFile(strFileFound.c_str(), &info);   //µð·ºÅä¸®¿¡ ÆÄÀÏÀÌ ÀÖ´ÂÁö Ã¹¹øÂ° ÆÄÀÏ¸¸.
    do
    {
        if (!((_tcscmp(info.cFileName, _T("."))==0) || (_tcscmp(info.cFileName, _T(".."))==0))) 
		{
            if((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)  //Subµð·ºÅä¸®°¡ Á¸ÀçÇÏ´Â°æ¿ì 
            {
                tstring strSubSvrFolder = strSvrFolderPath;
				strSubSvrFolder += _T("\\");
				strSubSvrFolder += info.cFileName;

				tstring strSubDefFolder = strDefFolderPath;
				strSubDefFolder += _T("\\");
				strSubDefFolder += info.cFileName;

				if(!CopyFolderAndFile(strSubSvrFolder.c_str(), strSubDefFolder.c_str()))
					return FALSE; 
             }
            else
			{
				strFileFound = strSvrFolderPath;
				strFileFound += _T("\\");
				strFileFound += info.cFileName;

				strDefFile = strDefFolderPath;
				strDefFile += _T("\\");
				strDefFile += info.cFileName;
				
				CopyFile(strFileFound.c_str(), strDefFile.c_str(), FALSE);
            }
        }

    }while(FindNextFile(hp, &info));
    
	FindClose(hp);

	return TRUE;
}
//>>150313.kty
// double _ttof(LPCTSTR strVal)
// {
// #ifdef _UNICODE
// 	int len = _tcslen(strVal); //strlenÀº NULL Á¾·á ¹®ÀÚ Àü±îÁö¸¸ ±æÀÌ·Î Ä«¿îÆ®ÇÔ
// 	char* szVal = new char[len];
// 	memset(szVal, NULL, len);
// 	wcstombs(szVal, strVal, len);
// 	double dVal = atof(szVal);
// 	delete[] szVal;
// #else
// 	double dVal = atof(strVal);
// #endif
// 
// 	return dVal;
// }

//>> 140513 JSPark
//////////////////////////////////////////////////////////////////////////
int findstr(const char *str, const char *what)
{
	int i, j, Ls=strlen(str), Lw=strlen(what);

	for(i=0;i<=Ls-Lw;i++){
		if(str[i] == what[0]){
			for(j=1;j<Lw;j++){
				if(str[i+j] != what[j]){
					break;
				}
			}
			if(j == Lw) return i;
		}
	}
	return -1;
}

bool str_remove(char* str, int start, int end)
{
	int i, len = strlen(str);	// »èÁ¦ÇÒ ¿µ¿ª ÀÌÈÄ ¹®ÀÚ¿­À» ¾ÕÀ¸·Î º¹»ç.
	for(i=0; i<len; i++)
	{
		// value°¡ NULLÀÏ¶§, Empty Line »ý¼º ¹æÁö!
		if(end==0 && i==len-2)
			break;

		str[start+i-1] = str[end+i];
	}
	
	return true; 
}

bool str_insert(char* str, char* src, int nStart)
{
	char* ctmp=NULL;

	int i;
	int len1 = strlen(str);
	int len2 = strlen(src);

	for(i=len1; i>=nStart-1; i--)
	{
		str[i+len2] = str[i];
	}
	// Ãß°¡ÇÒ ¹®ÀÚ¿­ »ðÀÔ.
	for(i=0; i<len2; i++)
	{
		str[i+nStart-1] = src[i];
	}
	return true; 
}
//////////////////////////////////////////////////////////////////////////
//<<

//>> 150612 HSOh HBITMAP Çü½Ä ÆÄÀÏÀ» bmp ÆÄÀÏ·Î ÀúÀåÇÑ´Ù.
BOOL SaveBitmap(HBITMAP hbit, LPCTSTR Path)
{
	if (hbit == NULL) //150522 JSLee
		return FALSE;

	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	BITMAP bit;
	BITMAPINFO *pih;
	int PalSize;
	HANDLE hFile;
	DWORD dwWritten, Size;
	HDC hdc;
	hdc = GetDC(NULL);
	GetObject(hbit, sizeof(BITMAP), &bit);
	ih.biSize = sizeof(BITMAPINFOHEADER);
	ih.biWidth = bit.bmWidth;
	ih.biHeight = bit.bmHeight;
	ih.biPlanes = 1;
	ih.biBitCount = bit.bmPlanes*bit.bmBitsPixel;

	if (ih.biBitCount > 8)
		ih.biBitCount = 24;

	ih.biCompression = BI_RGB;
	ih.biSizeImage = 0;
	ih.biXPelsPerMeter = 0;
	ih.biYPelsPerMeter = 0;
	ih.biClrUsed = 0;
	ih.biClrImportant = 0;
	PalSize = (ih.biBitCount == 24 ? 0 : 1 << ih.biBitCount)*sizeof(RGBQUAD);
	pih = (BITMAPINFO *)malloc(ih.biSize + PalSize);
	pih->bmiHeader = ih;
	GetDIBits(hdc, hbit, 0, bit.bmHeight, NULL, pih, DIB_RGB_COLORS);
	ih = pih->bmiHeader;
	if (ih.biSizeImage == 0) {
		ih.biSizeImage = ((((ih.biWidth*ih.biBitCount) + 31) & ~31) >> 3) * ih.biHeight;
	}
	Size = ih.biSize + PalSize + ih.biSizeImage;
	pih = (BITMAPINFO *)realloc(pih, Size);
	GetDIBits(hdc, hbit, 0, bit.bmHeight, (PBYTE)pih + ih.biSize + PalSize, pih, DIB_RGB_COLORS);
	fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + PalSize;
	fh.bfReserved1 = 0;
	fh.bfReserved2 = 0;
	fh.bfSize = Size + sizeof(BITMAPFILEHEADER);
	fh.bfType = 0x4d42;
	hFile = CreateFile(Path, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, &fh, sizeof(fh), &dwWritten, NULL);
	WriteFile(hFile, pih, Size, &dwWritten, NULL);
	ReleaseDC(NULL, hdc);
	CloseHandle(hFile);

	return TRUE;
}

int CharBinaryToInt(char *hex, int iSize)
{
	int iMax = 0;

	for (int ii = 0, jj = iSize; ii <= iSize; ii++, jj--)
	{
		if (hex[ii] > 0)
			iMax += pow(2, jj);
	}

	return iMax;
}

int StringBinaryToInt(CString str)
{
	int iMax = 0;
	CString strMsg;

	for (int ii = 0, jj = str.GetLength() - 1; ii <= str.GetLength() - 1; ii++, jj--)
	{
		strMsg = str.GetAt(ii);
		if (!GetNumberCheck(strMsg))
		{
			if (_ttoi(strMsg) > 0)
				iMax += pow(2, jj);
		}
	}

	return iMax;
}
//<<