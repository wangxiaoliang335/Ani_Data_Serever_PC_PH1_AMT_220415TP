// SerialCom.h: interface for the CSerialCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALCOM_H__8F80055D_6F0B_4855_A081_648F97096CB3__INCLUDED_)
#define AFX_SERIALCOM_H__8F80055D_6F0B_4855_A081_648F97096CB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommThread.h"

class CSerialCom : public CCommThread
{
public:
	void InitData(int PortNo);

	int		m_iStopBit;
	int		m_iSerialPort;
	int		m_iParity;
	int		m_iDataBit;
	int		m_iBaudRate;
	BOOL	m_bFirstFlag;

	CSerialCom();
	virtual ~CSerialCom();

	
	CString IndexComPort(int xPort);
	DWORD IndexBaud(int xBaud);
	BYTE IndexData(int xData);
	BYTE IndexStop(int xStop);
	BYTE IndexParity(int xParity);

public:
	bool OnPortOpen(int PortNo);
	void OnPortClose();

	void SetWriteComm(CStringA msg);

	void ThreadRun();
	BOOL CreateTask();
	void CloseTask();

	CWinThread *m_pThreadFFU;
	static UINT FFUThreadProc(LPVOID pParam);
	HANDLE m_hQuit;

	unsigned char m_CurrentFFUBuf[10];
	unsigned char m_FFUSettingBuf[10];
	CTimerCheck time_check;
	BOOL m_bStartFlag;
	BOOL m_bStart;

	void CurrentFFUData();
	void FFUDataStartMethod();
	void OnDataReceive(WPARAM wParam, LPARAM lParam);


	CString m_lastContent;
	CString m_lastRequest;

	CString GetLastContents() { return m_lastContent; }
	CString GetLastRequest() { return m_lastRequest; }
};

#endif