#pragma once

#include "Ani_Data_Serever_PC.h"
#include "SocketComm.h"
#include "TimeCheck.h"
#include "StringSupport.h"

class CAlignThread
{
public:
	CAlignThread(int iAlignType, int iAlignTypeNum, int iAlignNum);
	virtual ~CAlignThread();
	void ThreadRun();

	BOOL CreateTask();
	void CloseTask();
	
	void AlignCheckMethod();
	void AlignFirstCheckMethod();
	
	void AlignGrabMethod(int PnaelNum, int iPositionTXY, int iAlignNum);
	void TrayCheckNTrayAlignGrabMethod(int iPanelNum);
	void TrayLowerAlignGrabMethod(int iPanelCount, int iAlignNum);

	void AlignPanelCheck(int iPanelAddr, int iReceivedAddr, int iAlignNum);
	void AlignLightControl(int iPanelNum);

	void AlignPLCResult(int iPanelNum, int iAlignType, int iAlignNum, int iResultCode, CString strResultMsg, CString strPanelID);


	CWinThread *m_pThreadAlign;
	static UINT AlignThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

public:
	int m_iAlignType;
	int m_iAlignTypeNum;
	int m_iAlignNum;

	int m_AlignCheckCount;
	long m_positionResult;
	long m_AlignCount;
	long m_PanelCount;
	long m_AlignReverse;

	BOOL m_bFirstStatus;
	CTimerCheck time_check;

	BOOL m_bStartAlign[6];
	BOOL m_bStartFlag[6];
	CCriticalSection m_csSocketSend;
};