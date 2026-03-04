#pragma once

#include "Ani_Data_Serever_PC.h"

#if _SYSTEM_AMTAFT_

#include "SocketComm.h"
#include "TimeCheck.h"
#include "StringSupport.h"

class CLumitopThread : public CSocketComm
{
public:
	CLumitopThread();
	virtual ~CLumitopThread();

	void ThreadRun();
	bool SocketServerOpen(CString strServerPort);
	BOOL getConectCheck();
	BOOL CreateTask();
	void CloseTask();
	void RemoveClient();
	void LumitopFirstCheckMethod(int Num);
	void LumitopCheckMethod(int Num);
	void LumitopInspectionMethod(int Num, int panelNum);
	void ParsingGrabEnd(int Num, CString strContents);
	void ParsingInspectionResult(int Num, CString strContents);
	void ParsingModelRequest(int Num, CString strContents);
	void ParsingPcTimeRequest(int Num, CString strContents);
	void SocketSendto(int Num, CString strContents, int iCommand);
	void LogWrite(CString strContents, int Num);
	BOOL LumitopVecAdd(CString strPanel, CString strFpcID, int iPanelNum, int iIndexNum, int iPCNo, int iCurIndex);

	void LumitopPanelCheck();

	void LumitopPLCResult(int Num, int iPanelNum, CString ResultMsg, int ResultCode, CString strPanelID, int iIndexPanelNum);

	void ParshingLumitopData(int Num, CString strContents);

	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent, LPVOID lpvData);

	CWinThread *m_pThreadLumitop;
	static UINT LumitopThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

	std::vector<CString> m_lastContent;
	std::vector<CString> m_lastCommand;
	std::vector<CString> m_lastRequest;

	CString GetLastContents(int index) { return m_lastContent[index]; }
	CString GetLastCommand(int index) { return m_lastCommand[index]; }
	CString GetLastRequest(int index) { return m_lastRequest[index]; }

private:
	CString m_strModelName;

	int m_iLumitopSocketCheckCount;
	int m_iPcNum;

	CCriticalSection m_csDfsData;
	CCriticalSection m_csSocketSend;
	BOOL m_bFirstStatus;
	CTimerCheck time_check;

	BOOL m_bStartLumitop[4];
	BOOL m_bStartFlag;
};

#endif