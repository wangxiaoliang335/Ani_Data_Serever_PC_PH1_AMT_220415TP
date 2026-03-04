#pragma once

#if _SYSTEM_AMTAFT_

#include "Ani_Data_Serever_PC.h"
#include "TimeCheck.h"
#include "StringSupport.h"

class CTpThread
{
public:
	CTpThread();
	virtual ~CTpThread();
	void ThreadRun();
	BOOL CreateTask();
	void CloseTask();

	CWinThread *m_pThreadTp;
	static UINT TpThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

	CTimerCheck time_check;
	void Initialize();
	BOOL m_bFirstStatus;
};
#endif