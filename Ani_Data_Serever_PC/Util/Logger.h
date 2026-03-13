#pragma once

#include <stdio.h>
#include <stdarg.h>

#include <list>
#include <string>
using namespace std;

#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif

/** @brief log class     
	logger logger1(_T("logger1"), _T("D:\\Logger\\Log.log"), FALSE);
	logger.log(_T("\n========================\n"));
	logger1.LOG_ERR(_T("failed at...: E.%d\n"), GetLastError());
	
	// output ======================== 
	[ERRO] 18:23:22:796,05/15 (logger1:LoggerTestDlg.cpp:127,Thread:0x4125) - failed at...: E.183 
*/ 

const int MAX_MESSAGE = 1024;

enum ELogLevel
{
	eLogAlways = 0,
	eLogFatal,
	eLogErr,
	eLogWarn,
	eLogInfo,
	eLogStep,
	eLogDebug,
	eLogMax
};

enum ELogPeriod
{
	eLogMinute = 0,
	eLogHour,
	eLog3H,
	eLog6H,
	eLog12H,
	eLogDay,
	eLogWeek,
	eLog15D,
	eLogMonth
};

enum EDelPeriod
{
	eDelNo = -1,
	eDelHour = 0,
	eDelDay,
	eDelWeek,
	eDel15D,
	eDelMonth,
	eDelQuart,
	eDelHalf,
	eDelYear
};

struct LogInfo
{
	tstring m_strLevel;	// Log ????
	tstring m_strSrcFile; // Log ??? ??? ????
	int m_iSrcLine; // Log ??? ???
	DWORD m_dwSrcThreadId; // Log ??? Thread
	tstring m_strMsg; // ?????
	volatile BOOL m_bSrc; // Log ??? ???? ??

	LogInfo() : m_bSrc(FALSE) {}
};

class CLogger
{
public:
	CLogger(LPCTSTR szLabel, LPCTSTR szLogFile = _T(""), BOOL bDbgView = TRUE);
		
	~CLogger(void);
	
	BOOL m_bDbgView;
	HANDLE m_hFile;

	tstring	m_strAppPath;
	tstring	m_strLogFile;
	tstring m_strLabel;
	LogInfo m_loginfo;
	tstring	m_strUser;

	ELogLevel m_eLogLevel;
	ELogPeriod m_eLogPeriod;
	EDelPeriod m_eDelPeriod;
	BOOL m_bLogging;
	BOOL m_bLogTickCount;
	BOOL m_bUseCache;
	BOOL m_bAutoDelete;
	int m_iMsgMaxSize;

	list<LogInfo> m_lsLogInfo;

	SYSTEMTIME m_tmStart;

#ifndef LOG_THREAD_ANSYNC
	CRITICAL_SECTION  m_csLog;
#ifndef LOG_SAFE_MODE
	CRITICAL_SECTION  m_csAdd;
#endif 
#endif

	HANDLE m_hThread, m_hThreadDel;
	HANDLE m_hExit, m_hExitDel;
	
	void Run();
	static UINT WINAPI LogThreadProc(LPVOID pParam);

	void RunDel();
	static UINT WINAPI DelThreadProc(LPVOID pParam);

	BOOL ReadLogConfig();

/** @brief set logging level     
	@param  level : [LOG_ALWAYS ~ LOG_MAX) 
*/
	void setLevel(ELogLevel eLevel) { ASSERT(eLevel < eLogMax); m_eLogLevel = eLevel; }

/** @brief set source position     
	@param  lpszFile    source file name     
	@param  line		source file line     
	@param  ThreadId    source Thread ID     
	@return LogChannel& self object 
*/     
	CLogger* SetSrcPos(LPCTSTR lpszFile = NULL, int line = 0, DWORD ThreadId = 0);

	void AddInfo(LPCTSTR level, LPCTSTR fmt, va_list& ap);
	void AddInfo2(LPCTSTR level, LPCTSTR msg);


/** @brief set log output channel     
	@param  lpszLogPath log file path, NULL : not using file logging.     
	@return bool        success/fail 
*/
	BOOL SetOutput(LPCTSTR lpszFilePath);

	void SetUserName(LPCTSTR lpszUserName); //20130705 lee

	BOOL CreateFolders(LPCTSTR lpszFolderPath);
	void DeleteLogFiles(LPCTSTR lpszFolderPath);
/** @brief set log output channel     
	@param  tmNow Current Local Time.     
	@return bool        success/fail 
*/
	BOOL ChangeFile(SYSTEMTIME& tmNow);


/** @brief logging function    
	@param  fmt. log format like printf 
*/    
	void Log(LPCTSTR fmt, ...); 
	void Log(LogInfo& loginfo); 
	void Log(LPCTSTR szLevel, LPCTSTR fmt, va_list& ap);
	void Log2(LPCTSTR szLevel, LPCTSTR szMsg); 
/** @brief logging function     
	@param  fmt. log format like printf 
*/     
	void Always(LPCTSTR fmt, ...); 
	void Fatal(LPCTSTR fmt, ...);
	void Error(LPCTSTR fmt, ...); 
	void Warn(LPCTSTR fmt, ...); 
	void Info(LPCTSTR fmt, ...);
	void Step(LPCTSTR fmt, ...);
	void Debug(LPCTSTR fmt, ...);

	void Always2(LPCTSTR msg);
	void Fatal2(LPCTSTR msg);
	void Error2(LPCTSTR msg); 
	void Warn2(LPCTSTR msg); 
	void Info2(LPCTSTR msg);
	void Step2(LPCTSTR msg);
	void Debug2(LPCTSTR msg);	
};

#ifdef _UNICODE 
#define __toWideChar(str) L##str 
#define _toWideChar(str) __toWideChar(str) 
#define __TFILE__  _toWideChar(__FILE__) 
#else 
#define __TFILE__  __FILE__ 
#endif  

#define SRC_POS		SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId()) 
#define LOG_NONE	SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Log 
#define LOG_FATAL	SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Fatal 
#define LOG_ERR		SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Error 
#define LOG_WARN	SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Warn 
#define LOG_INFO	SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Info 
#define LOG_STEP	SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Step 
#define LOG_DEBUG	SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Debug

//#define LOG_MCVS    Always 
//#define LOG_PGVS    Always 
//#define LOG_ALWS    Always
//#define LOG_INFO	Info 
//#define LOG_STEP	Step 
//#define LOG_DEBUG	Debug



#define LOG_NONE2		SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Log2
#define LOG_FATAL2		SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Fatal2 
#define LOG_ERR2		SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Error2 
#define LOG_WARN2 	    SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Warn2 
#define LOG_INFO2		SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Info2 
#define LOG_STEP2		SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Step2 
#define LOG_DEBUG2	    SetSrcPos(__TFILE__, __LINE__, GetCurrentThreadId())->Debug2
//////////////// 



