#pragma once

#include "Ani_Data_Serever_PC.h"
//#include "ClientSocket.h"
#include "SocketComm.h"
#include "EZini.h"
#include "StringSupport.h"


class CTpManager : public CSocketComm
{
public:
	CTpManager();
	virtual ~CTpManager();
	bool SocketServerOpen(CString strServerPort);
	BOOL getConectCheck();

	void SendTPMessage(int iChNum, int iCommand, CString strContent = NULL);
	void TpLogMessage(CString strContents);

	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent, LPVOID lpvData);

	int m_iTpSocketCheckCount;

	std::vector<CString> m_lastContent;
	std::vector<CString> m_lastResult;
	std::vector<CString> m_lastRequest;

	CString GetLastContents(int iChNum) { return m_lastContent[iChNum]; }
	CString GetLastResult(int iChNum) { return m_lastResult[iChNum]; }
	CString GetLastRequest(int iChNum) { return m_lastRequest[iChNum]; }

	CCriticalSection m_csTpData;
	CString m_strDummyContents;
};