#pragma once
#if _SYSTEM_AMTAFT_

#include "Ani_Data_Serever_PC.h"
#include "SocketComm.h"

class CViewingAngleThread : public CSocketComm
{
public:
	CViewingAngleThread();
	virtual ~CViewingAngleThread();
	void ThreadRun();
	BOOL getConectCheck();
	bool SocketServerOpen(CString strServerPort);
	BOOL CreateTask();
	void CloseTask();
	
	void ViewingAnglePnaleCheck();
	void ViewingAngleFirstCheckMethod(int iPanelNum);
	void ViewingAngleCheckMethod(int iPcNum);
	void RemoveClient();
	void ViewingAngleMethod(int iPanelNum);
	BOOL ViewingAngleVecAdd(CString strPanel, CString strFpcID, int iPanelNum, int iIndexNum, int iCurIndex);
	void ViewingAngleModelRequest(int iPcNum);
	void ViewingAnglePcTimeRequest(int iPcNum);

	void ViewingAngleInspectionResult(int iPcNum, CString strContents);

	void ViewingAnglePLCError(CString ErrorMsg, int ErrorCode, int iPanelNum);
	void ViewingAnglePLCResult(int iPanelNum, CString ResultMsg, int ResultCode, CString strPanelID);

	void SocketSendto(int Num, CString strContents, int iCommand);
	void LogWrite(CString strContents, int iPanelNum);
	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent, LPVOID lpvData);

	CWinThread *m_pThreadViewingAngle;
	static UINT ViewingAngleThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

	std::vector<CString> m_lastContent;
	std::vector<CString> m_lastCommand;
	std::vector<CString> m_lastRequest;

	CString GetLastContents(int index) { return m_lastContent[index]; }
	CString GetLastCommand(int index) { return m_lastCommand[index]; }
	CString GetLastRequest(int index) { return m_lastRequest[index]; }
public:
	int m_ViewingAngleCheckCount;

	BOOL m_bFirstStatus;
	CTimerCheck time_check;

	BOOL m_bStartViewingAngle[4];
	BOOL m_bStartFlag[4];

	CCriticalSection m_csDfsData;
	CCriticalSection m_csSocketSend;

}; 

#endif
