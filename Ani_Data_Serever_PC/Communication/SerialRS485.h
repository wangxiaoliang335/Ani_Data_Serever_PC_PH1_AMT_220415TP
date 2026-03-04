// SerialCom.h: interface for the CSerialCom class.
//
//////////////////////////////////////////////////////////////////////


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommThread.h"

class CSerialRS485 : public CCommThread
{
public:
	void InitData(int PortNo);

	int		m_iStopBit;
	int		m_iSerialPort;
	int		m_iParity;
	int		m_iDataBit;
	int		m_iBaudRate;


	CSerialRS485();
	virtual ~CSerialRS485();

	CString IndexComPort(int xPort);
	DWORD IndexBaud(int xBaud);
	BYTE IndexData(int xData);
	BYTE IndexStop(int xStop);
	BYTE IndexParity(int xParity);

public:
	bool OnPortOpen(int PortNo);
	void OnPortClose();

	void ThreadRun();
	BOOL CreateTask();
	void CloseTask();

	CWinThread *m_pThread;
	static UINT ThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

	CTimerCheck time_check;
	BOOL m_bStartFlag;
	BOOL m_bStart;
	BOOL m_bFirstCheck;

	void ARSDataStartMethod(int iNum);
	void OnDataReceive(WPARAM wParam, LPARAM lParam);


	CString m_lastContent;
	CString m_lastRequest;

	CString GetLastContents() { return m_lastContent; }
	CString GetLastRequest() { return m_lastRequest; }
};

