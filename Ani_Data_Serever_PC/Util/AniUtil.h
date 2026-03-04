// AniUtil.h: Utilities.
//
//////////////////////////////////////////////////////////////////////

#if !defined(ANIUTIL_HEADER_INCLUDED_)
#define ANIUTIL_HEADER_INCLUDED_

#include <math.h>
#include "StringSupport.h"

//______________________________________________________________________________
// Delay
extern void DoEvents();
extern void Delay(long ms, BOOL bEvent = FALSE);

//______________________________________________________________________________
// 반올림 처리
extern int RoundupInt(double value);
extern long RoundupLong(double value);

#define RoundupDouble(x,dig) (floor((x)*pow(10,dig)+0.5)/pow(10,dig))
/**
실수 x와 반올림할 자리수 dig를 인수로 전달받는데 
dig는 0을 기준(Zero Base)으로 하므로 우리가 생각하는 소수점 자리수보다는 하나 더 작다. 
즉 dig가 0일 때 소수점 첫 번째 자리를 지정하며 n일 때 n+1자리에서 반올림하여 
결국 반올림 후 dig자리까지가 유효한 수로 남게 된다. 
pow 함수는 음수승도 잘 계산하므로 dig에 음수를 주면 정수부에서 반올림을 할 수도 있다.
123.456789의 -2자리 반올림 = 100.000000
123.456789의 0자리 반올림 = 123.000000
123.456789의 2자리 반올림 = 123.460000
*/
extern CString GetNowSystemTimeMilliseconds();
extern CString GetNowSystemTimeMilliseconds2();
extern CString GetNowSystemTimeMillisecondsSirius();  //JMHWANG 12.02.08
extern CString GetNowSystemTimeMillisecondsSirius2();  //JMHWANG 12.02.08
extern CString GetNowSystemTimeMillisecondsSirius3(); //JMHWANG 12.02.08
extern CString GetNowSystemTimeMillisecondsSirius4(); //HACY 18.12.24

// 큰값
extern double GetMax(double v1, double v2);

// 근사치 확인
extern bool IsAboutSame(double ref, double v1, double in_pos);

// 산술 평균 구하기
extern double GetMean(double* array, int size);

// 표준 편차 계산 함수
double GetStandardDeviation(double* array, int size, int option);

// 피타고라스 정리 ( 대각의 길이 와 밑변을 가지고 높이 구하기)
extern double GetPitaHight(double x, double y);
// 피타고라스 정리 ( 높이 와 밑변을 가지고 대각의 길이 구하기)
extern double Getpita(double x, double y);

extern CString Hex2Bin(CString& sHex);
//______________________________________________________________________________
extern CString GetDouble2String_MM(double dd);
//______________________________________________________________________________
extern CString GetExtractionCommaMsg(CString& strMsg);
extern CString GetExtractionTabMsg(CString& strMsg);
extern CString GetExtractionDotMsg(CString& strMsg);
extern CString GetExtractionSpaceMsg(CString& strMsg);
//>> 140404 JSPark
extern CString GetExtractionDashMsg(CString& strMsg);
extern CString GetExtractionColonMsg(CString& strMsg);
//<<
//______________________________________________________________________________
extern CString GetDateString();
extern CString GetDateStringChangeDay(int iChngDay);
extern CString GetDateString2();
extern CString GetDateString3();
extern CString GetDateString4();
extern CString GetDateString5();
extern CString GetDateString6(); //160512 JSLee
extern CString GetDateString7();
extern CString GetDateString4Folder();

extern CString GetTimeString();
extern CString GetTimeString2();
extern CString GetTimeString3();
extern CString GetTimeString4(); // kkj 12.04.10
extern CString GetTimeString5(CString strContents);

extern int String2Hex(CString strVal);
extern int String2Int(CString strVal);
extern float String2Float(CString strVal);
extern CString Hex2String(int value);
extern CString Int2String(int value);
extern CString Float2String(float value, CString strFormat = _T("%f"));
extern double String2Double(CString strVal);
extern CString Double2String(double value, CString strFormat = _T("%lf"));
extern int Hex2Int(char cHex);
extern unsigned char Hex2Byte(char hex);

extern BOOL SaveBitmap(HBITMAP hbit, LPCTSTR Path); //150612 HSOh
extern BOOL FileExists(LPCTSTR strFilePath);
extern BOOL FileExists(LPCTSTR strStartPath, LPCTSTR strFileName);
extern BOOL FileExists(CString& strStartFolder, CString strFileName, bool SearchSubFolder);

extern int CheckAndCreateFolder(LPCTSTR szFolderPath);
extern void DeleteFolderAndFile(LPCTSTR szFolderPath);
extern BOOL CreateFolders(LPCTSTR szFolderPath);
extern BOOL CopyFolderAndFile(LPCTSTR szSvrFolderPath, LPCTSTR szDefFolderPath);

//extern BOOL GetPathOnly(CString & strFilePath);// add ydp
extern BOOL GetFileNameOnly(CString & strFilePath);// add ydp
extern BOOL GetFileNameOnlyNoExt(CString & strFilePath);// add ydp
extern BOOL GetUpFolderPath(CString & strFilePath);// add ydp
//extern BOOL PathEndCheck(CString & strPath); // add ydp

extern void FillSpace(TCHAR *pszIn, short nStr);
extern void Select_Sort(int parm_data[], int parm_count);
extern char* StringToChar(CString str);//CString을 char*로 변환
extern CString CharToString(TCHAR *str);//char*을 CString로 변환
extern BOOL GetNumberCheck(CString str);//>>20180210 hacy 정규식으로 숫자체크
extern BOOL GetNullCheck(CString str, TCHAR parser);//>>20180210 hacy Null Check
extern CString GetTimeString(int iNum);//>>20180608 0800 > 08:00 같은 시간으로 만들어주는기능
extern void SetStringReplace(CString *str, CString reg);//>>20180703 hacy 정규식으로 항목제거
extern int CharBinaryToInt(char *hex, int iSize);
extern int StringBinaryToInt(CString str);
extern CStringA CStringAToCstring(CString cstr);
extern char* CStringToUtf8(CString str);
//______________________________________________________________________________
#define U_PI	3.14159265
#define M_PI	3.14159265358979323846	
#define INCH4MM 25.4

extern double Rad2Deg(double s);
extern double Deg2Rad(double s);
//extern double _ttof(LPCTSTR strVal);   //>>150313.kty  Unicode(string-->float)

//>> 140513 JSPark
//////////////////////////////////////////////////////////////////////////
int findstr(const char *str, const char *what);		// 문자열, 찾을 문자
bool str_remove(char* str, int start, int end);		// 문자열, 시작 위치, 끝 위치
bool str_insert(char* str, char* src, int nStart);	// 문자열, 삽입할 문자열, 위치
//////////////////////////////////////////////////////////////////////////
//<<
#endif

/*
년

  %y - 뒤 2자리 (09)  
  %Y - 전체 (2009)  
		
월
  %m - 01~12 (03)			
  %b - 영어 3글자 (Mar)  
  %B - 영어 전체 (March)
		
일					  
  %d - 01~31 월 기준 (10)						
  %j - 001~365 년 기준 (069)
	  
시
  %H - 00~23 24시 기준 (14)								  
  %I (대문자 아이) - 01~12 12시 기준 (02)
				
분						  
  %M - 00~59 (12)												
초							  
  %S - 00~59 (15)
				  
요일														  
  %a - 요약 (Tue)														
  %A - 요일 (Tuesday)												  
  %w - 일요일 0 기준 (2)														
					
주
  %U - 00~53 년기준으로 시작주 일요일 (10)													  
  %W - 00~53 년기준으로 시작주 월요일 (10)
			
기타																			  
  %p - AM/PM (PM)																			
  %x - 월/일/년 (03/10/09)																	  
  %X - 시:분:초 (14:12:15)															
  %c - 월/일/년 시:분:초 (03/10/09 14:12:15)
  %% - % 기호 (%)
  %z, %Z - 시간대 이름 (대한민국 표준시)							
*/