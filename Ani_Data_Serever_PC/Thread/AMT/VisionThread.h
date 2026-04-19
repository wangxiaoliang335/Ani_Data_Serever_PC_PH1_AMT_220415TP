#pragma once

#include "Ani_Data_Serever_PC.h"

#if _SYSTEM_AMTAFT_

#include "SocketComm.h"
#include "TimeCheck.h"
#include "StringSupport.h"
#include "LightingEventHandler.h"

class CVisionThread : public CSocketComm, public ILightingEventHandler
{
public:
	CVisionThread();
	virtual ~CVisionThread();

	void ThreadRun();
	bool SocketServerOpen(CString strServerPort);
	BOOL getConectCheck();
	BOOL CreateTask();
	void CloseTask();
	void RemoveClient();
	void VisionFirstCheckMethod(int Num);
	void VisionCheckMethod(int Num);
	void VisionInspectionMethod(int Num, int panelNum);
	void ParsingGrabEnd(int Num, CString strContents);
	//void ParsingInspectionResult(int Num, CString strContents);
	void ParsingModelRequest(int Num, CString strContents);
	void ParsingPcTimeRequest(int Num, CString strContents);
	void SocketSendto(int Num, CString strContents, int iCommand);
	void LogWrite(CString strContents, int Num);
	BOOL VisionVecAdd(CString strPanel, CString strFpcID, int iPanelNum, int iIndexNum, int iPCNo, int iCurIndex);
	BOOL TryStartLightingFromPlc(const BOOL startFlags[4]);

	void AutoFocusAxis(int Num, int iCommand, CString strContents);
	void AutoFocusSave(int Num);

	void VisionPanelCheck();

	void VisionPLCResult(int Num, int iPanelNum, CString ResultMsg, int ResultCode, CString strPanelID);

	void ParshingVisionData(int Num, CString strContents);

	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent, LPVOID lpvData);

	// ILightingEventHandler 回调函数
	virtual void OnLightingRunning() override;
	virtual void OnLightingSnapFN() override;
	virtual void OnLightingResult(const int resultCode[4]) override;

	CWinThread *m_pThreadVision;
	static UINT VisionThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

	std::vector<CString> m_lastContent;
	std::vector<CString> m_lastCommand;
	std::vector<CString> m_lastRequest;

	// 检测结果缓存（与老代码统一，方便管理和查询）
	std::vector<InspResult> m_lastInspResultVec;

	CString GetLastContents(int index) { return m_lastContent[index]; }
	CString GetLastCommand(int index) { return m_lastCommand[index]; }
	CString GetLastRequest(int index) { return m_lastRequest[index]; }
	
private:
	CString m_strModelName;

	long m_AutoFocusPosition;
	int m_iVisionSocketCheckCount;
	int m_iPcNum;

	CCriticalSection m_csDfsData;
	CCriticalSection m_csSocketSend;
	BOOL m_bFirstStatus;
	CTimerCheck time_check;

	BOOL m_bStartVision[4];
	BOOL m_bStartFlag;

	BOOL m_bAutoFocusStart[MaxCamCount];
	BOOL m_bAutoFocusStartFlag;

};

#endif