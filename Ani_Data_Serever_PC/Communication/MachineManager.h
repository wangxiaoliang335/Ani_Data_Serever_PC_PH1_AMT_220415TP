//>>210422

#pragma once

#include "SocketComm.h"
#include "EZini.h"
#include "StringSupport.h"


class CMachineManager:  public CSocketComm
{
public:
	CMachineManager(int iPcNum);
	virtual ~CMachineManager();

	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent, LPVOID lpvData);

	bool ConnectClient(bool bUpper, CString strServerIP, CString strServerPort);
	void SendMachineMessage(CString strMsg, int iPcNum, int iCommand);
	void LogMessage(CString strContents);

	CCriticalSection m_csSocketSend;

	int m_iPcNum;
};

//<<