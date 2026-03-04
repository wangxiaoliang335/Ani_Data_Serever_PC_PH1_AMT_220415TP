// SerialCom.cpp: implementation of the CSerialRS485 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SerialRS485.h"
#include "afxconv.h"
#include "Ani_Data_Serever_PC.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialRS485::CSerialRS485()
{
	m_bFirstCheck = FALSE;
}

CSerialRS485::~CSerialRS485()
{

}

void CSerialRS485::InitData(int PortNo)
{
	m_iBaudRate = 7;
	m_iSerialPort = PortNo;
	m_iStopBit = 1;
	m_iParity = 0;
	m_iDataBit = 3;
}


bool CSerialRS485::OnPortOpen(int PortNo)
{
	CString PortName(_T(""));

	InitData(PortNo);

	if (m_bConnected == FALSE)//포트가 닫혀 있을 경우에만 포트를 열기 위해
	{
		if (OpenPort(IndexComPort(m_iSerialPort), IndexBaud(m_iBaudRate), IndexData(m_iDataBit), IndexStop(m_iStopBit), IndexParity(m_iParity)) == TRUE)
		{
			PortName.Format(_T("OPEN PORT: %s\r\n"), IndexComPort(m_iSerialPort));
		}
		else //<<150704 JYLee
		{
			//theApp.getMsgBox(MS_OK, _T("Port not yet open"), _T("Port not yet open"), _T("端口尚未打开"));
			theApp.m_pARSSendReceiverLog->LOG_INFO(_T("Port not yet open"));
			return false;
		}
	}
	else
	{
		theApp.m_pARSSendReceiverLog->LOG_INFO(_T("Already Port open"));
		//theApp.getMsgBox(MS_OK, _T("Already Port open"), _T("Already Port open"), _T("端口已开启"));
		return false;
	}

	return true;
}

void CSerialRS485::OnPortClose()
{
	CString PortName(_T(""));

	if (m_bConnected == TRUE)
	{
		ClosePort();
		PortName.Format(_T("CLOSE PORT: %s \r\n"), IndexComPort(m_iSerialPort));
	}
	else
	{
		PortName.Format(_T("%s Port not yet open \r\n"), IndexComPort(m_iSerialPort));
	}
}

CString CSerialRS485::IndexComPort(int xPort)
{
	CString PortName(_T(""));

	//<<150707 JYLee
	if (xPort < 9)
		PortName.Format(_T("COM%d"), xPort + 1);
	else
		PortName.Format(_T("\\\\.\\COM%d"), xPort + 1);

	return PortName;
}

DWORD CSerialRS485::IndexBaud(int xBaud)
{
	DWORD dwBaud;
	switch (xBaud)
	{
	case 0:		dwBaud = CBR_4800;		break;

	case 1:		dwBaud = CBR_9600;		break;

	case 2:		dwBaud = CBR_14400;		break;

	case 3:		dwBaud = CBR_19200;		break;

	case 4:		dwBaud = CBR_38400;		break;

	case 5:		dwBaud = CBR_56000;		break;

	case 6:		dwBaud = CBR_57600;		break;

	case 7:		dwBaud = CBR_115200;	break;
	}

	return dwBaud;
}

BYTE CSerialRS485::IndexData(int xData)
{
	BYTE byData;
	switch (xData)
	{
	case 0:	byData = 5;			break;

	case 1:	byData = 6;			break;

	case 2:	byData = 7;			break;

	case 3:	byData = 8;			break;
	}
	return byData;
}

BYTE CSerialRS485::IndexStop(int xStop)
{
	BYTE byStop;
	if (xStop == 0)
	{
		byStop = ONESTOPBIT;
	}
	else if (xStop == 1)
	{
		byStop = ONESTOPBIT;
	}
	else
	{
		byStop = TWOSTOPBITS;
	}
	return byStop;
}

BYTE CSerialRS485::IndexParity(int xParity)
{
	BYTE byParity;
	switch (xParity)
	{
	case 0:	byParity = NOPARITY;	break;

	case 1:	byParity = ODDPARITY;	break;

	case 2:	byParity = EVENPARITY;	break;
	}

	return byParity;
}

void CSerialRS485::ThreadRun()
{
	while (::WaitForSingleObject(m_hQuit, 100) != WAIT_OBJECT_0)
	{
		if (theApp.m_bExitFlag == FALSE)
			return;

		theApp.m_ARSConnectStatus = m_bConnected;

		if (theApp.m_ARSConnectStatus == TRUE)
		{
			if (m_bFirstCheck == FALSE)
			{
				time_check.SetCheckTime(1000);
				time_check.StartTimer();
				m_bFirstCheck = TRUE;

				for (int ii = 1; ii <= 3; ii++)
				{
					ARSDataStartMethod(ii);
					Delay(1000);
				}
					
			}

			if (time_check.IsTimeOver())
			{
				for (int ii = 1; ii <= 3; ii++)
				{
					ARSDataStartMethod(ii);
					Delay(1000);
				}

				time_check.StartTimer();
			}
		}
	}
}

void CSerialRS485::ARSDataStartMethod(int iNum)
{
	char szSendData[100] = "";
	DWORD dwSize;

	char PrivitCode[4] = {"PRN"};
	char HeaderTemp1[4] = {"DIT"};
	char HeaderTemp2[4];
	char AddrData = 0;
	char CheckSum = 0;

	memset(szSendData, 0, 100);
	
	szSendData[0] = 0x02;
	
	for (int i = 0; i<3; i++) 
		HeaderTemp2[i] = HeaderTemp1[i] ^ PrivitCode[i]; 

	szSendData[1] = HeaderTemp2[0];
	szSendData[2] = HeaderTemp2[1];
	szSendData[3] = HeaderTemp2[2];
	
	dwSize = strlen(szSendData);
	strcpy(szSendData + dwSize, "0");
	dwSize = strlen(szSendData);

	CStringSupport::ToAString(CStringSupport::FormatString(_T("%d"), iNum), &AddrData, sizeof(AddrData));
	strcpy(szSendData + dwSize, &AddrData);
	dwSize = strlen(szSendData);
	strcpy(szSendData + dwSize, "0");

	dwSize = strlen(szSendData);

	for (int i = 1; i<(int)dwSize; i++)
		CheckSum += (int)szSendData[i];

	szSendData[dwSize] = (char)(CheckSum & 0x00ff);
	szSendData[dwSize + 1] = 0x03;
	dwSize = strlen(szSendData);
	
	WriteComm((BYTE*)szSendData, dwSize);

	CString msg, sendMsg;
	for (int ii = 0; ii < 8; ii++)
		msg.AppendFormat(_T("0x%02X "), szSendData[ii]);

	sendMsg.Format(_T("[MC -> ARS] %s"), msg);
	theApp.m_pARSSendReceiverLog->LOG_INFO(sendMsg);
}

UINT CSerialRS485::ThreadProc(LPVOID pParam)
{
	CSerialRS485* pThis = reinterpret_cast<CSerialRS485*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;
} 

BOOL CSerialRS485::CreateTask(){
	BOOL bRet = TRUE;
	m_pThread = ::AfxBeginThread(ThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThread)
		bRet = FALSE;
	m_pThread->m_bAutoDelete = FALSE;
	m_pThread->ResumeThread();
	return bRet;
}

void CSerialRS485::CloseTask()
{
	if (m_pThread != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThread->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThread->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThread->m_hThread, 1L);
				theApp.m_pTpLog->LOG_INFO(_T("Terminate Thread"));
			}
		}
		delete m_pThread;
		m_pThread = NULL;

	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}

void CSerialRS485::OnDataReceive(WPARAM wParam, LPARAM lParam)
{
	BYTE *pBuff = (BYTE*)wParam;
	int pBuffSize = lParam;

	unsigned char* reciverMsg = pBuff;
	CString msg;
	EsdPlcData pEsdData;
	EsdData LogData;

	for (int ii = 0; ii < pBuffSize; ii++)
	{
		switch (ii)
		{
		case 7:
			LogData.m_iAddr = _ttoi(CStringSupport::ToShorString(reciverMsg[ii])) - 1;
			break;
		case 9: 
			pEsdData.m_strEsdPole = reciverMsg[ii];
			LogData.m_strEsdPole = CStringSupport::ToShorString(reciverMsg[ii]);
			break;
		case 10: 
			pEsdData.m_iEsdData5 = reciverMsg[ii];
			LogData.m_iEsdData5 = _ttoi(CStringSupport::ToShorString(reciverMsg[ii]));
		case 11:   
			pEsdData.m_iEsdData4 = reciverMsg[ii];
			LogData.m_iEsdData4 = _ttoi(CStringSupport::ToShorString(reciverMsg[ii]));
		case 12: 
			pEsdData.m_iEsdData3 = reciverMsg[ii];
			LogData.m_iEsdData3 = _ttoi(CStringSupport::ToShorString(reciverMsg[ii]));
		case 13:
			pEsdData.m_iEsdData2 = reciverMsg[ii];
			LogData.m_iEsdData2 = _ttoi(CStringSupport::ToShorString(reciverMsg[ii]));
		case 14:
			pEsdData.m_iEsdData1 = reciverMsg[ii];
			LogData.m_iEsdData1 = _ttoi(CStringSupport::ToShorString(reciverMsg[ii]));
		case 15:
			pEsdData.m_strEsdResult = reciverMsg[ii];
			LogData.m_strEsdResult = CStringSupport::ToShorString(reciverMsg[ii]);
			break;
		}
	}

	
	theApp.m_pEqIf->m_pMNetH->SetEsdData(eWordType_EsdValue1 + LogData.m_iAddr, &pEsdData);

	msg = CStringSupport::FormatString(_T("[ARS -> MC] : %d , Pole : %s , Data1 : %d , Data2 : %d , Data3 : %d , Data4 : %d , Data5 : %d , Result : %s"),
		LogData.m_iAddr + 1,
		LogData.m_strEsdPole,
		LogData.m_iEsdData1,
		LogData.m_iEsdData2,
		LogData.m_iEsdData3,
		LogData.m_iEsdData4,
		LogData.m_iEsdData5,
		LogData.m_strEsdResult
		);

	theApp.m_pARSSendReceiverLog->LOG_INFO(msg);

}