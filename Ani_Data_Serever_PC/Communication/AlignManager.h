#pragma once

#include "Ani_Data_Serever_PC.h"
//#include "ClientSocket.h"
#include "SocketComm.h"
#include "EZini.h"
#include "StringSupport.h"


class CAlignManager : public CSocketComm
{
public:
	CAlignManager(int iAlignType, int iAlignTypeNum, int iAlignNum);
	virtual ~CAlignManager();

	void LogWrite(int iNum, CString strContents);
	void SocketSendto(int iNum, CString strContents, int iCommand);

	bool SocketServerOpen(CString strServerPort);
	BOOL getConectCheck();
	virtual void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
	virtual void OnEvent(UINT uEvent, LPVOID lpvData);

	void AlignLightOff(int iNum);

	void AlignModelRequest();
	void AlignPcTimeRequest();

	void AlignGrabEnd(CString strContents);
	void AlignTrayCheckGrabEnd(CString strContents);
	void AlignTrayAlignGrabEnd(CString strContents);
	void AlignTrayLowerAlignGrabEnd(CString strContents);

	void AlignDataSum(int iChNum, int iResultValue, int iInspSection);

	BOOL m_bCheck[2];
	CString m_lastContent;
	CString m_lastCommand;
	CString m_lastRequest;

	CString GetLastContents() { return m_lastContent; }
	CString GetLastCommand() { return m_lastCommand; }
	CString GetLastRequest() { return m_lastRequest; }

public:
	BOOL m_bstart = FALSE;//HDM test
	BOOL m_testT[ChMaxCount];

	int m_iNum;
	int m_iAlignType;
	int m_iAlignTypeNum;
	int m_iAlignNum;
};