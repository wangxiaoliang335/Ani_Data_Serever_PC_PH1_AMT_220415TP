// SerialCom.cpp: implementation of the CSerialCom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SerialCom.h"
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

CSerialCom::CSerialCom()
{
	m_bFirstFlag = TRUE;
}

CSerialCom::~CSerialCom()
{

}

void CSerialCom::InitData(int PortNo)
{
	m_iBaudRate = 1;
	m_iSerialPort = PortNo;
	m_iStopBit = 1;
	m_iParity = 0;
	m_iDataBit = 3;
	time_check.SetCheckTime(60000);
	time_check.StartTimer();

	//FFU 고정데이터
	m_CurrentFFUBuf[0] = _STX;
	m_CurrentFFUBuf[1] = 0x8A;
	m_CurrentFFUBuf[2] = 0x87;
	m_CurrentFFUBuf[3] = 0x81;
	m_CurrentFFUBuf[4] = 0x9F;
	m_CurrentFFUBuf[5] = 0x81;
	m_CurrentFFUBuf[6] = String2Hex(theApp.m_strFFUEndPoint);
	int iValue = 0;
	CString strCheckSum;
	for (int ii = 1; ii < 7; ii++)
		iValue += String2Hex(CStringSupport::FormatString(_T("%X"), m_CurrentFFUBuf[ii]));

	strCheckSum = Hex2String(iValue);
	m_CurrentFFUBuf[7] = String2Hex(strCheckSum.Right(2));
	m_CurrentFFUBuf[8] = _ETX;

	m_FFUSettingBuf[0] = _STX;
	m_FFUSettingBuf[1] = 0x89;
	m_FFUSettingBuf[2] = 0x84;
	m_FFUSettingBuf[3] = 0x81;
	m_FFUSettingBuf[4] = 0x9F;
	m_FFUSettingBuf[5] = 0x81;
	m_FFUSettingBuf[6] = String2Hex(theApp.m_strFFUEndPoint);
}


bool CSerialCom::OnPortOpen(int PortNo)
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
			theApp.m_pFFUSendReceiverLog->LOG_INFO(_T("Port not yet open"));
			return false;
		}
	}
	else
	{
		theApp.m_pFFUSendReceiverLog->LOG_INFO(_T("Already Port open"));
		//theApp.getMsgBox(MS_OK, _T("Already Port open"), _T("Already Port open"), _T("端口已开启"));
		return false;
	}

	return true;
}

void CSerialCom::OnPortClose() 
{
	CString PortName(_T(""));

	if(m_bConnected == TRUE)
	{	
		ClosePort();
 		PortName.Format(_T("CLOSE PORT: %s \r\n"),IndexComPort(m_iSerialPort));
	}
	else
	{
 		PortName.Format(_T("%s Port not yet open \r\n"),IndexComPort(m_iSerialPort));
	}
}

CString CSerialCom::IndexComPort(int xPort)
{
	CString PortName(_T(""));

	//<<150707 JYLee
	if (xPort < 9)
		PortName.Format(_T("COM%d"), xPort + 1);
	else
		PortName.Format(_T("\\\\.\\COM%d"), xPort + 1);

	return PortName;
}

DWORD CSerialCom::IndexBaud(int xBaud)
{
	DWORD dwBaud;
	switch(xBaud)
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

BYTE CSerialCom::IndexData(int xData)
{
	BYTE byData;
	switch(xData)
	{
		case 0 :	byData = 5;			break;
	
		case 1 :	byData = 6;			break;
		
		case 2 :	byData = 7;			break;
		
		case 3 :	byData = 8;			break;
	}
	return byData;
}

BYTE CSerialCom::IndexStop(int xStop)
{
	BYTE byStop;
	if(xStop == 0)
	{
		byStop = ONESTOPBIT;
	}
	else if(xStop == 1)
	{
		byStop = ONESTOPBIT;
	}
	else
	{
		byStop = TWOSTOPBITS;
	}
	return byStop;
}

BYTE CSerialCom::IndexParity(int xParity)
{
	BYTE byParity;
	switch(xParity)
	{
	case 0 :	byParity = NOPARITY;	break;
	
	case 1 :	byParity = ODDPARITY;	break;
	
	case 2 :	byParity = EVENPARITY;	break;
	}

	return byParity;
}

void CSerialCom::ThreadRun()
{
	FFUData pFFUData;
	while (::WaitForSingleObject(m_hQuit, 100) != WAIT_OBJECT_0)
	{
		if (theApp.m_bExitFlag == FALSE)
			return;

		theApp.m_FFUConectStatus = m_bConnected;
		
		if (time_check.IsTimeOver())
		{
			time_check.StartTimer();
			CurrentFFUData();
		}
		
		m_bStartFlag = theApp.m_pEqIf->m_pMNetH->GetPlcBitData(eBitType_FFUStart, OffSet_0);
		
		if (m_bStartFlag == FALSE)
		{
			theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_FFUEnd, OffSet_0, FALSE);
		}
		
		if (m_bStart == !m_bStartFlag)
		{
			m_bStart = m_bStartFlag;
			theApp.m_PlcThread->LogWrite(CStringSupport::FormatString(_T("FFU Data Start Flag [%s]"), m_bStart == FALSE ? _T("FALSE") : _T("TRUE")));
			if (m_bStart == TRUE)
			{
				theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_FFUEnd, OffSet_0, FALSE);
				FFUDataStartMethod();
			}
		}

	}
}

void CSerialCom::CurrentFFUData()
{
	CStringA Sendmsg;
	Sendmsg.Format(("%c%c%c%c%c%c%c%c%c"), m_CurrentFFUBuf[0], m_CurrentFFUBuf[1], m_CurrentFFUBuf[2], m_CurrentFFUBuf[3], m_CurrentFFUBuf[4]
		, m_CurrentFFUBuf[5], m_CurrentFFUBuf[6], m_CurrentFFUBuf[7], m_CurrentFFUBuf[8]);

	CString msg;
	for (int ii = 0; ii < 9; ii++)
		msg.AppendFormat(_T("0x%02X "), m_CurrentFFUBuf[ii]);

	m_lastContent = msg;
	msg.Format(_T("[MC -> FFU] %s"), msg);
	theApp.m_pFFUSendReceiverLog->LOG_INFO(msg);

	SetWriteComm(Sendmsg);
}

void CSerialCom::FFUDataStartMethod()
{
	CStringA Sendmsg;
	CString strSettingValue , strCheckSum;
	FFUData pFFUData;
	int iValue = 0;
	long FFUSettingData = 0;

	theApp.m_pEqIf->m_pMNetH->GetPlcWordData(eWordType_FFUSetValue, &FFUSettingData);

	strSettingValue.Format(_T("%X"), FFUSettingData / 10);
		
	for (int ii = 1; ii < 7; ii++)
		iValue += String2Hex(CStringSupport::FormatString(_T("%X"), m_FFUSettingBuf[ii]));

	iValue += String2Hex(strSettingValue);
	strCheckSum = Hex2String(iValue);

	m_FFUSettingBuf[7] = FFUSettingData / 10;
	m_FFUSettingBuf[8] = String2Hex(strCheckSum.Right(2));
	m_FFUSettingBuf[9] = _ETX;

	Sendmsg.Format(("%c%c%c%c%c%c%c%c%c%c"), m_FFUSettingBuf[0], m_FFUSettingBuf[1], m_FFUSettingBuf[2], m_FFUSettingBuf[3], m_FFUSettingBuf[4]
		, m_FFUSettingBuf[5], m_FFUSettingBuf[6], m_FFUSettingBuf[7], m_FFUSettingBuf[8], m_FFUSettingBuf[9]);

	CString msg;
	for (int ii = 0; ii < 10; ii++)
		msg.AppendFormat(_T("0x%02X "), m_FFUSettingBuf[ii]);

	m_lastContent = msg;

	msg.Format(_T("[MC -> FFU] %s"), msg);
	theApp.m_pFFUSendReceiverLog->LOG_INFO(msg);

	SetWriteComm(Sendmsg);
	theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_FFUEnd, OffSet_0, TRUE);
}

UINT CSerialCom::FFUThreadProc(LPVOID pParam)
{
	CSerialCom* pThis = reinterpret_cast<CSerialCom*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;

} // end FFUThreadProc

BOOL CSerialCom::CreateTask(){
	BOOL bRet = TRUE;
	m_pThreadFFU = ::AfxBeginThread(FFUThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadFFU)
		bRet = FALSE;
	m_pThreadFFU->m_bAutoDelete = FALSE;
	m_pThreadFFU->ResumeThread();
	return bRet;
}

void CSerialCom::CloseTask()
{
	if (m_pThreadFFU != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadFFU->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadFFU->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadFFU->m_hThread, 1L);
				theApp.m_pTpLog->LOG_INFO(_T("Terminate FFU Thread"));
			}
		}
		delete m_pThreadFFU;
		m_pThreadFFU = NULL;

	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}

void CSerialCom::SetWriteComm(CStringA msg)
{
	CStringA strCmd = msg;
	size_t len = strCmd.GetLength();

	int DWORD  = WriteComm((BYTE *)strCmd.GetBuffer(), len);

	//FFU 연결이 지속적으로 실패가 나오고 있어서 실패시에는 다시 연결해서 할수있도록 합니다.
	if (DWORD == 0)
	{
		ClosePort();
		Delay(500);
		OnPortOpen(_ttoi(theApp.m_strFFUPortNum));
		WriteComm((BYTE *)strCmd.GetBuffer(), len);
		theApp.m_pFFUSendReceiverLog->LOG_INFO(_T("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	}

	return;
}

void CSerialCom::OnDataReceive(WPARAM wParam, LPARAM lParam)
{
	BYTE *pBuff = (BYTE*)wParam;
	int pBuffSize = lParam;

	unsigned char* reciverMsg = pBuff;
	CString strMsg, strTemp;
	TCHAR   *ptr;
	FFUData pFFUData;

	for (int ii = 0; ii < pBuffSize; ii++)
	{
		strMsg.AppendFormat(_T("0x%02X "), reciverMsg[ii]);
		strTemp.Format(_T("0x%02X"), reciverMsg[ii]);

		switch (ii)
		{
		case 6: pFFUData.m_ICU1_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 8: pFFUData.m_ICU1_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 10: pFFUData.m_ICU2_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 12: pFFUData.m_ICU2_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 14: pFFUData.m_ICU3_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 16: pFFUData.m_ICU3_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 18: pFFUData.m_ICU4_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 20: pFFUData.m_ICU4_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 22: pFFUData.m_ICU5_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 24: pFFUData.m_ICU5_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 26: pFFUData.m_ICU6_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 28: pFFUData.m_ICU6_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 30: pFFUData.m_ICU7_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 32: pFFUData.m_ICU7_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 34: pFFUData.m_ICU8_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 36: pFFUData.m_ICU8_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 38: pFFUData.m_ICU9_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 40: pFFUData.m_ICU9_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 42: pFFUData.m_ICU10_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 44: pFFUData.m_ICU10_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 46: pFFUData.m_ICU11_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 48: pFFUData.m_ICU11_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 50: pFFUData.m_ICU12_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 52: pFFUData.m_ICU12_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 54: pFFUData.m_ICU13_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 56: pFFUData.m_ICU13_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 58: pFFUData.m_ICU14_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 60: pFFUData.m_ICU14_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 62: pFFUData.m_ICU15_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 64: pFFUData.m_ICU15_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;

		case 66: pFFUData.m_ICU16_ProcessValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		case 68: pFFUData.m_ICU16_SettingValue = _tcstol(strTemp, &ptr, 16) * 10; break;
		}
	}

	m_lastRequest = strMsg;

	strMsg.Format(_T("[FFU -> MC] %s"), strMsg);
	theApp.m_pFFUSendReceiverLog->LOG_INFO(strMsg);

	theApp.m_pEqIf->m_pMNetH->SetFFUData(eWordType_FFUResult, &pFFUData);
}