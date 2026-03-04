#pragma once
#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"

class CRankThread
{
public:
	CRankThread();
	virtual ~CRankThread();
	void ThreadRun();
	BOOL CreateTask();
	void CloseTask();
	

	CWinThread *m_pThreadRank;
	static UINT RankThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

	queue<CString> m_RankCodeList;

public:
	CCriticalSection m_csDfsData;

	void AddRankCodeList(CString strPanel, CString strFpcID, int iPanelNum, int iInspNum, int iIndexNum);
}; 
#endif

