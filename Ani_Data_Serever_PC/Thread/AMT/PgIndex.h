#pragma once

#if _SYSTEM_AMTAFT_

#include "Ani_Data_Serever_PC.h"
#include "StringSupport.h"
#include "EZini.h"

class CPgIndex
{
public:
	CPgIndex(int iZoneNum);
	virtual ~CPgIndex();
	void ThreadRun();

	BOOL CreateTask();
	void CloseTask();

	CWinThread *m_pPgIndex;
	static UINT PgIndexProc(LPVOID pParam);
	HANDLE m_hQuit;
	
	void ZonePanelCheck(int iNum);

	void ZoneContactOn(int Num);
	void ZoneContactOff(int Num);
	void ZonePreGamma(int Num);
	//void ZonePatternNext(int Num);
	//void ZonePatternBack(int Num);
	void ZoneTouchInspection(int Num);

	void PgVecAdd(CString strPanel, CString strFpcID, int iPanelNum, int iZoneNum, int iPgOrderNum, int iPgTimerNum, int iChNum);
private:
	int m_iIndexNum;
	CString m_strIndexName;
	int m_iZoneNum;
	int m_iPanelNameAddr;
	
	BOOL m_bStart[MaxZone][6][PanelMaxCount];	//0=ContactOn, 1=Contact OFf. 2=Next, 3=Back, 4=PreGamma 5=TP
	BOOL m_bStartFlag[MaxZone];

	BOOL m_bNextFlag;
	BOOL m_bBackFlag;

	CCriticalSection m_csVecData;
};

#endif