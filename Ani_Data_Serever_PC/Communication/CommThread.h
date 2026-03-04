
#pragma once
#define WM_COMM_READ (WM_USER +1)
#define BUFF_SIZE			4192


class CQueue
{
public:
	BYTE buff[BUFF_SIZE];
	int m_iHead, m_iTail;
	CQueue();
	void Clear();
	int GetSize();
	BOOL PutByte(BYTE b);
	BOOL GetByte(BYTE *pb);
};

class CCommThread
{
public:
	CCommThread();
	~CCommThread();
	
	BOOL        check;
	HANDLE		m_hComm;				
	CString		m_sPortName;			
	BOOL		m_bConnected;			
	OVERLAPPED	m_osRead, m_osWrite;	
	HANDLE		m_hThreadWatchComm;		
	WORD		m_wPortID;		
	CQueue      m_QueueRead;			

	BOOL	OpenPort(CString strPortName, DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity);
	void	ClosePort();
	DWORD	WriteComm(BYTE *pBuff, DWORD nToWrite);

	DWORD	ReadComm(BYTE *pBuff, DWORD nToRead);

	virtual void OnDataReceive(WPARAM wParam, LPARAM lParam);
};

DWORD	ThreadWatchComm(CCommThread* pComm);

