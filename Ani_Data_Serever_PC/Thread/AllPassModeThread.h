#pragma once

#include "Ani_Data_Serever_PC.h"
#include "SocketComm.h"
#include "TimeCheck.h"
#include "StringSupport.h"

class CAllPassModeThread
{
public:
	CAllPassModeThread();
	virtual ~CAllPassModeThread();
	void ThreadRun();

	BOOL CreateTask();
	void CloseTask();

	CWinThread *m_pThreadAllPassMode;
	static UINT AllPassModeThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

	BOOL m_bFisrtStartFlag;

	BOOL m_bVisionStart;
	BOOL m_bStartVision[4];

	BOOL m_bStartViewingAngle[4];
	BOOL m_bViewingStartFlag[4];

	BOOL m_bAlignStartFlag[4];
	BOOL m_bStartAlign[4];

	BOOL m_bStartFlag;
	BOOL m_bAlarmStart = FALSE;
	BOOL m_AxisStart = FALSE;
	BOOL m_bModelStart = FALSE;
	BOOL m_bModelFirstCheck = FALSE;
	BOOL m_OperateTimeStart = FALSE;
	BOOL m_bDataFlag[PanelMaxCount];
	BOOL m_bPreGammaFlag[PanelMaxCount];
	BOOL m_bTouchFlag[PanelMaxCount];
	BOOL m_bContactOnFlag[PanelMaxCount];
	BOOL m_bContactOffFlag[PanelMaxCount];

	BOOL m_bUnloaderFlag[ManualStageMaxCount][ChMaxCount];
	BOOL m_bGammaFlag[GammaPGStatusCount][ChMaxCount];
};
