#pragma once

#include "Ani_Data_Serever_PC.h"
#include "SocketComm.h"
#include "EZini.h"
#include "StringSupport.h"

class CPgManager: public CSocketComm
{
public:
	CPgManager();
	virtual ~CPgManager();
	bool SocketServerOpen(CString strServerPort, int iPcNum);
	BOOL getConectCheck();

	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent, LPVOID lpvData);

	void SendPGMessage(CString strMsg, int iChNum, int iStageNum = NULL);
	void PgLogMessage(CString strContents);
	void RemoveClient();
#if _SYSTEM_AMTAFT_
	std::vector<CString> m_lastContent;
	std::vector<CString> m_lastResult;
	std::vector<CString> m_lastCommand;
	std::vector<CString> m_lastRequest;
	std::vector<CString> m_lastGammaL;
	std::vector<CString> m_lastGammaX;
	std::vector<CString> m_lastGammaY;
	
	CString GetLastContents(int iChNum) { return m_lastContent[iChNum]; }
	CString GetLastResult(int iChNum) { return m_lastResult[iChNum]; }
	CString GetLastCommand(int iChNum) { return m_lastCommand[iChNum]; }
	CString GetLastRequest(int iChNum) { return m_lastRequest[iChNum]; }
	CString GetLastGammaL(int iChNum) { return m_lastGammaL[iChNum]; }
	CString GetLastGammaX(int iChNum) { return m_lastGammaX[iChNum]; }
	CString GetLastGammaY(int iChNum) { return m_lastGammaY[iChNum]; }
#else
	CString m_lastContent[MaxGammaStage][ChMaxCount];
	CString m_lastResult[MaxGammaStage][ChMaxCount];
	CString m_lastCommand[MaxGammaStage][ChMaxCount];
	CString m_lastRequest[MaxGammaStage][ChMaxCount];
	CString m_lastGammaVBAT[MaxGammaStage][ChMaxCount];
	CString m_lastGammaVDDI[MaxGammaStage][ChMaxCount];
	CString m_lastGammaVCI[MaxGammaStage][ChMaxCount];
	CString m_lastGammaPGCODE[MaxGammaStage][ChMaxCount];

	CString GetLastContents(int iStageNum, int iChNum) { return m_lastContent[iStageNum][iChNum]; }
	CString GetLastResult(int iStageNum, int iChNum) { return m_lastResult[iStageNum][iChNum]; }
	CString GetLastCommand(int iStageNum, int iChNum) { return m_lastCommand[iStageNum][iChNum]; }
	CString GetLastRequest(int iStageNum, int iChNum) { return m_lastRequest[iStageNum][iChNum]; }
	CString GetLastGammaVBAT(int iStageNum, int iChNum) { return m_lastGammaVBAT[iStageNum][iChNum]; }
	CString GetLastGammaVDDI(int iStageNum, int iChNum) { return m_lastGammaVDDI[iStageNum][iChNum]; }
	CString GetLastGammaVCI(int iStageNum, int iChNum) { return m_lastGammaVCI[iStageNum][iChNum]; }
	CString GetLastGammaPGCODEI(int iStageNum, int iChNum) { return m_lastGammaPGCODE[iStageNum][iChNum]; }
#endif
	CCriticalSection m_csPgData;
	int m_iPcNum;

#if _SYSTEM_AMTAFT_
	void AOIDataReceived(CString strContents);
	void ULDDataReceived(CString strContents);
#else
	void GammaDataReceived(CString strContents);
#endif

	CCriticalSection m_csSocketSend;
};