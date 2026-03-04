#pragma once

#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "TimeCheck.h"
#include "StringSupport.h"

class CManualThread
{
public:
	CManualThread();
	virtual ~CManualThread();
	void ThreadRun();
	BOOL CreateTask();
	void CloseTask();

	CWinThread *m_pThreadManual;
	static UINT ManualThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

public:
	BOOL m_bStart[ChMaxCount];
	BOOL m_bStartFlag[ManualStageMaxCount][ChMaxCount];

	void ManualStagePanelCheck(int iChNum, int iOrderNum);
	void ManualStageContactOnStart(int iNum, int iPcNum, int iChNum);
	void ManualStageContactOffStart(int iNum, int iPcNum, int iChNum);
	void ManualStageNextStart(int iNum, int iPcNum, int iChNum);
	void ManualStageBackStart(int iNum, int iPcNum, int iChNum);
	void ManualStagePreGammaStart(int iNum, int iPcNum, int iChNum);
	void ManualStageTouchStart(int iNum, int iChNum);
	void ManualStageOperatorViewStart(int iChNum);
	void ULDInspectDataParser(int iPanelNum, int iCommand, CString strPanelID, CString strFpcID);
	void ManualStageVecAdd(CString strPanel, CString strFpcID, int iChNum, int iMStageOrder, int iTimerNum, int iNum);

	void OpvCommCheckMethod(int iNum);
	BOOL m_bOpvFirstStatus[ChMaxCount];
	CTimerCheck time_check[ChMaxCount];
	
};
#endif