#pragma once

#include "Ani_Data_Serever_PC.h"

#if _SYSTEM_GAMMA_

#include "SocketComm.h"
#include "TimeCheck.h"
#include "StringSupport.h"

class CGammaThread
{
public:
	CGammaThread(int iStageNum);
	virtual ~CGammaThread();
	void ThreadRun();

	BOOL CreateTask();
	void CloseTask();

	CWinThread *m_pThreadGamma;
	static UINT GammaThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

	void GammaVecAdd(CString strPanelID, CString strFpcID, int iPanelNum, int iStageNum, int iChNum, int iGammaOrder, int iTimerNum);
	void GammaPanelCheck();
	void GammaMtpStart(int iPanelNum);
	void GammaContactOnStart();
	void GammaContactOffStart();
	void GammaNextStart();
	void GammaBackStart();
	void GammaPIDCheckStart();

	BOOL m_bOperatorModeFlag[MaxGammaStage];

private:
	int m_iPcNum;
	int m_istageNum;
	int m_iPanelNum;

	BOOL m_bStart[MaxGammaStage][GammaPGStatusCount];
	BOOL m_bStartFlag[MaxGammaStage];

	CCriticalSection m_csPgStatus;
	CCriticalSection m_csVecData;
};
#endif