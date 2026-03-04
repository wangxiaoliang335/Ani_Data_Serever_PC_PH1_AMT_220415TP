
#include "stdafx.h"

#if _SYSTEM_GAMMA_

#include "Ani_Data_Serever_PC.h"
#include "ComGammaView.h"
#include "StringSupport.h"

#define IF_CHECK_TIMER 0

CComGammaView::CComGammaView()
	: CFormView(CComGammaView::IDD)
{
}

IMPLEMENT_DYNCREATE(CComGammaView, CFormView)
CComGammaView::~CComGammaView()
{
}

void CComGammaView::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COM_LIST, m_ctrlComList);
	DDX_Control(pDX, IDC_CMB_PG_COMMAND, m_cmbPgCommand);
	DDX_Control(pDX, IDC_CMB_ALIGN_COMMAND, m_cmbAlignCommand);
	DDX_Control(pDX, IDC_CMB_FFU_COMMAND, m_cmbFFUCommand);
	DDX_Control(pDX, IDC_CMB_PG_CH_COMMAND, m_cmbPgChCommand);
	DDX_Control(pDX, IDC_CMB_ALIGN_CH_COMMAND, m_cmbAlignChCommand);

	DDX_Control(pDX, IDC_ALIGN_STATE, m_AlignState);
	DDX_Control(pDX, IDC_PG_STATE, m_PgState);
	DDX_Control(pDX, IDC_FFU_STATE, m_FFUState);
}

BEGIN_MESSAGE_MAP(CComGammaView, CFormView)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CComGammaView, CFormView)
	ON_EVENT(CComGammaView, IDC_SEND_ALIGN_COMMAND, DISPID_CLICK, CComGammaView::ClickSendAlignCommand, VTS_NONE)
	ON_EVENT(CComGammaView, IDC_SEND_PG_COMMAND, DISPID_CLICK, CComGammaView::ClickSendPgCommand, VTS_NONE)
	ON_EVENT(CComGammaView, IDC_SEND_FFU_COMMAND2, DISPID_CLICK, CComGammaView::ClickSendFFUCommand, VTS_NONE)
	ON_EVENT(CComGammaView, IDC_FFU_STATE, DISPID_CLICK, CComGammaView::ClickFfuState, VTS_NONE)
	ON_EVENT(CComGammaView, IDB_ALIGN_TRD_TEST1, DISPID_CLICK, CComGammaView::ClickAlignTrdTest1, VTS_NONE)
	ON_EVENT(CComGammaView, IDB_ALIGN_TRD_TEST2, DISPID_CLICK, CComGammaView::ClickAlignTrdTest1, VTS_NONE)
	ON_EVENT(CComGammaView, IDB_ALIGN_TRD_TEST3, DISPID_CLICK, CComGammaView::ClickAlignTrdTest1, VTS_NONE)
	ON_EVENT(CComGammaView, IDB_ALIGN_TRD_TEST4, DISPID_CLICK, CComGammaView::ClickAlignTrdTest1, VTS_NONE)
END_EVENTSINK_MAP()

#ifdef _DEBUG
void CComGammaView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CComGammaView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG
void CComGammaView::ChangeAlignList()
{
	m_ctrlComList.DeleteAllItems();
	m_cmbAlignCommand.ResetContent();
	m_cmbAlignChCommand.ResetContent();
	m_cmbPgCommand.ResetContent();
	m_cmbPgChCommand.ResetContent();
	m_cmbFFUCommand.ResetContent();
	CString msg, strName;

	m_ctrlComList.InsertColumn(0, _T("Name"), 0, 150);
	m_ctrlComList.InsertColumn(1, _T("Last Command"), 0, 200);
	m_ctrlComList.InsertColumn(2, _T("Result"), 0, 100);
	m_ctrlComList.InsertColumn(3, _T("Response"), 0, 400);
	m_ctrlComList.InsertColumn(4, _T("Request"), 0, 400);


	for (int ii = 0; ii < MC_PACKET_MAX_NUM; ii++)
		m_cmbAlignCommand.AddString(MC_PacketNameTable[ii]);

	for (int ii = 0; ii < _ttoi(theApp.m_strAlignCount); ii++)
	{
		strName.Format(_T("%d"), ii + 1);
		m_cmbAlignChCommand.AddString(strName);
	}

	for (int ii = 0; ii < PgCommand_Max_Num; ii++)
		m_cmbPgCommand.AddString(PG_PacketNameTable[ii]);

	for (int ii = 1; ii <= PG_MAX_CH; ii++)
	{
		strName.Format(_T("%d"), ii);
		m_cmbPgChCommand.AddString(strName);
	}

	for (int ii = 0; ii < _ttoi(theApp.m_strAlignCount); ii++)
	{
		strName.Format(_T("Align %d"), ii + 1);
		m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);
	}
	for (int ii = 0; ii < PG_MAX_CH; ii++) {
		strName.Format(_T("PG CH%d"), ii + 1);
		m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);
	}

	m_cmbFFUCommand.AddString(_T("Current Data"));
	m_cmbFFUCommand.AddString(_T("Setting Data"));

	m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), _T("FFU"));


	m_cmbPgCommand.SetCurSel(0);
	m_cmbAlignCommand.SetCurSel(0);
	m_cmbPgChCommand.SetCurSel(0);
	m_cmbAlignChCommand.SetCurSel(0);
	m_cmbFFUCommand.SetCurSel(0);

	SetComboBoxReadOnly(IDC_CMB_PG_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_ALIGN_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_PG_CH_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_ALIGN_CH_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_FFU_COMMAND);
}

void CComGammaView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	ChangeAlignList();

	CStringManager::AddListener(this);

	SetTimer(IF_CHECK_TIMER, 2000, NULL);
}

BOOL CComGammaView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CComGammaView::PlcStart()
{
	//if (theApp.m_PLCStatus)
	{
		//theApp.getMsgBox(MS_OK, _T("PLC Auto Mode Start"), _T("PLC Auto Mode Start"), _T("PLC Auto Mode Start"));
		//return TRUE;
	}
	return FALSE;
}

void CComGammaView::ClickSendAlignCommand()
{
	if (PlcStart())
		return;

	int icommand = m_cmbAlignCommand.GetCurSel();
	int iChCommand = m_cmbAlignChCommand.GetCurSel();


	CString strData, sendMsg;
	GetDlgItemText(IDC_ALIGN_DATA, strData);


	if (!theApp.m_AlignConectStatus[iChCommand])
	{
		theApp.getMsgBox(MS_OK, _T("Align PC 연결 확인하세요."), _T("Align PC Connect Check"), _T("对位PC连接确认"));
		return;
	}
	
	if (icommand < 0){
		theApp.getMsgBox(MS_OK, _T("Command 선택하세요."), _T("Command Choice"), _T("选择指令"));
		return;
	}
	if (iChCommand < 0){
		theApp.getMsgBox(MS_OK, _T("CH 선택하세요."), _T("CH Choice"), _T("选择频道"));
		return;
	}

	sendMsg.Format(_T("%d,%s"), icommand, strData);
	theApp.m_AlignSocketManager[iChCommand]->SocketSendto(iChCommand, sendMsg, icommand);
}

void CComGammaView::ClickSendPgCommand()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (PlcStart())
		return;

	int icommand = m_cmbPgCommand.GetCurSel();
	int iChCommand = m_cmbPgChCommand.GetCurSel();
	int iPcNum = (iChCommand / 12) % 2;

	if (!theApp.m_PgConectStatus[iPcNum])
	{
		theApp.getMsgBox(MS_OK, _T("PG PC 연결 확인하세요."), _T("PG PC Connect Check"), _T("PG PC连接确认"));
		return;
	}
	
	if (icommand < 0){
		theApp.getMsgBox(MS_OK, _T("Command 선택하세요."), _T("Command Choice"), _T("选择指令"));
		return;
	}
	
	if (iChCommand < 0){
		theApp.getMsgBox(MS_OK, _T("CH 선택하세요."), _T("CH Choice"), _T("选择频道"));
		return;
	}

	CString strData, sendMsg;
	GetDlgItemText(IDC_PG_DATA, strData);

	if (!sendMsg.CompareNoCase(_T("CONTACTOFF")))
	{
		if (_ttoi(theApp.m_strPGName) == PG_MuhanZC)
			sendMsg = CStringSupport::FormatString(_T("Ch,%d,KEY,RESET"), iChCommand + 1);
		else
			sendMsg = CStringSupport::FormatString(_T("Ch,%d,CONTACTOFF"), iChCommand + 1);
	}
	else
	{
		if (strData == _T(""))
			sendMsg.Format(_T("CH,%d,%s"), iChCommand + 1, PG_PacketNameTable[icommand]);
		else
			sendMsg.Format(_T("CH,%d,%s,%s"), iChCommand + 1, PG_PacketNameTable[icommand], strData);
	}

	theApp.m_PgSocketManager[iPcNum].SendPGMessage(sendMsg, iChCommand);
}

void CComGammaView::ClickSendFFUCommand()
{
	CStringA msg;
	CString strSendMsg;
	int icommand = m_cmbFFUCommand.GetCurSel();

	if (icommand == 0)
	{
		//현재값 가지고 와서 셋팅 해주는거
		msg.Format(("%c%c%c%c%c%c%c%c%c"), theApp.m_FFUSerialCom->m_CurrentFFUBuf[0], theApp.m_FFUSerialCom->m_CurrentFFUBuf[1], theApp.m_FFUSerialCom->m_CurrentFFUBuf[2], theApp.m_FFUSerialCom->m_CurrentFFUBuf[3], theApp.m_FFUSerialCom->m_CurrentFFUBuf[4]
			, theApp.m_FFUSerialCom->m_CurrentFFUBuf[5], theApp.m_FFUSerialCom->m_CurrentFFUBuf[6], theApp.m_FFUSerialCom->m_CurrentFFUBuf[7], theApp.m_FFUSerialCom->m_CurrentFFUBuf[8]);


		for (int ii = 0; ii < 9; ii++)
			strSendMsg.AppendFormat(_T("0x%02X "), theApp.m_FFUSerialCom->m_CurrentFFUBuf[ii]);

		theApp.m_FFUSerialCom->m_lastContent = strSendMsg;
	}
	else
	{
		//값 셋팅 해주는거
		CString strData, strSettingValue, strCheckSum;
		int iValue = 0;
		GetDlgItemText(IDC_FFU_DATA, strData);

		if (!GetNumberCheck(strData))
		{
			if (_ttoi(strData) < 1501)
			{
				strSettingValue.Format(_T("%X"), _ttoi(strData) / 10);

				for (int ii = 1; ii < 7; ii++)
					iValue += String2Hex(CStringSupport::FormatString(_T("%X"), theApp.m_FFUSerialCom->m_FFUSettingBuf[ii]));

				iValue += String2Hex(strSettingValue);
				strCheckSum = Hex2String(iValue);

				theApp.m_FFUSerialCom->m_FFUSettingBuf[7] = _ttoi(strData) / 10;
				theApp.m_FFUSerialCom->m_FFUSettingBuf[8] = String2Hex(strCheckSum.Right(2));
				theApp.m_FFUSerialCom->m_FFUSettingBuf[9] = _ETX;

				msg.Format(("%c%c%c%c%c%c%c%c%c%c"), theApp.m_FFUSerialCom->m_FFUSettingBuf[0], theApp.m_FFUSerialCom->m_FFUSettingBuf[1], theApp.m_FFUSerialCom->m_FFUSettingBuf[2], theApp.m_FFUSerialCom->m_FFUSettingBuf[3], theApp.m_FFUSerialCom->m_FFUSettingBuf[4]
					, theApp.m_FFUSerialCom->m_FFUSettingBuf[5], theApp.m_FFUSerialCom->m_FFUSettingBuf[6], theApp.m_FFUSerialCom->m_FFUSettingBuf[7], theApp.m_FFUSerialCom->m_FFUSettingBuf[8], theApp.m_FFUSerialCom->m_FFUSettingBuf[9]);

				for (int ii = 0; ii < 10; ii++)
					strSendMsg.AppendFormat(_T("0x%02X "), theApp.m_FFUSerialCom->m_FFUSettingBuf[ii]);

				theApp.m_FFUSerialCom->m_lastContent = strSendMsg;
			}
			else
			{
				theApp.getMsgBox(MS_OK, _T("Max Value 1500"), _T("Max Value 1500"), _T("Max Value 1500"));
				return;
			}
		}
		else
		{
			theApp.getMsgBox(MS_OK, _T("숫자만 입력하세요."), _T("Number Check"), _T("请输入数值"));
			return;
		}

	}

	strSendMsg.Format(_T("[MC -> FFU] %s"), strSendMsg);
	theApp.m_pFFUSendReceiverLog->LOG_INFO(strSendMsg);
	theApp.m_FFUSerialCom->SetWriteComm(msg);
}

void CComGammaView::SetComboBoxReadOnly(int item)
{
	CWnd *p_combo = GetDlgItem(item);
	HWND h_wnd = ::FindWindowEx(p_combo->m_hWnd, NULL, _T("Edit"), NULL);
	if (NULL != h_wnd) ((CEdit *)CWnd::FromHandle(h_wnd))->SetReadOnly(TRUE);
}

void CComGammaView::UpdateStateButton(BOOL bState, CBtnEnh* pStateBtn)
{
	if (bState)
		pStateBtn->SetBackColorInterior(TGREEN);
	else
		pStateBtn->SetBackColorInterior(TRED);
}

void CComGammaView::StringChanged()
{
	StringChnageMsg(IDC_STATIC_PG_COMMUNICATION, _T("PG ????"), _T("PG Communication"), _T("PG 繫斤"));
	StringChnageMsg(IDC_STATIC_ALIGN_COMMUNICATION, _T("Align ????"), _T("Align Communication"), _T("Align 繫斤"));
	StringChnageMsg(IDC_STATIC_FFU_COMMUNICATION, _T("FFU ????"), _T("FFU Communication"), _T("PLC 繫斤"));

	StringChnageMsg(IDC_STATIC_PG_CH_COMMAND, _T("Ch"), _T("Ch"), _T("틉돛"));

	StringChnageMsg(IDC_STATIC_PG_COMMAND, _T("Command"), _T("Command"), _T("츱즈"));
	StringChnageMsg(IDC_STATIC_ALIGN_COMMAND, _T("Command"), _T("Command"), _T("츱즈"));
	StringChnageMsg(IDC_STATIC_FFU_COMMAND, _T("Command"), _T("Command"), _T("츱즈"));

	StringChnageMsg(IDC_STATIC_PG_DATA, _T("Data"), _T("Data"), _T("鑒앴"));
	StringChnageMsg(IDC_STATIC_ALIGN_DATA, _T("Data"), _T("Data"), _T("鑒앴"));
	StringChnageMsg(IDC_STATIC_FFU_DATA, _T("Data"), _T("Data"), _T("鑒앴"));

	StringChnageMsg(IDC_PG_STATE, _T("????"), _T("Connected"), _T("젯쌈"));
	StringChnageMsg(IDC_ALIGN_STATE, _T("????"), _T("Connected"), _T("젯쌈"));
	StringChnageMsg(IDC_FFU_STATE, _T("????"), _T("Connected"), _T("젯쌈"));

	StringChnageMsg(IDC_SEND_PG_COMMAND, _T("????"), _T("Send"), _T("랙箇"));
	StringChnageMsg(IDC_SEND_ALIGN_COMMAND, _T("????"), _T("Send"), _T("랙箇"));
	StringChnageMsg(IDC_SEND_FFU_COMMAND2, _T("????"), _T("Send"), _T("랙箇"));
}

void CComGammaView::StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi)
{
	CString msg;
	switch (theApp.m_iLanguageSelect)
	{
	case KOR:msg = strKor; break;
	case ENG:msg = strEng; break;
	case CHI:msg = strChi; break;
	}

	((CBtnEnh*)GetDlgItem(btn))->SetCaption(msg);
}

void CComGammaView::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	CFormView::OnTimer(nIDEvent);
	int bFlag = FALSE;

	if (nIDEvent == IF_CHECK_TIMER)
	{
		m_csList.Lock();
		UpdateData();

		int iItemNum = 0;

		for (int ii = 0; ii < _ttoi(theApp.m_strAlignCount); ii++)
		{
				m_ctrlComList.SetItemText(iItemNum, 1, theApp.m_AlignSocketManager[ii]->GetLastCommand());
				m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_AlignSocketManager[ii]->GetLastContents());
				m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_AlignSocketManager[ii]->GetLastRequest());
				iItemNum++;
		}
		
		for (int ii = 0; ii < MaxGammaStage; ii++)
		{
			int iPcNum = (ii / 6) % 2;
			for (int jj = 0; jj < ChMaxCount; jj++)
			{
				m_ctrlComList.SetItemText(iItemNum, 1, theApp.m_PgSocketManager[iPcNum].GetLastCommand(ii, jj));
				m_ctrlComList.SetItemText(iItemNum, 2, theApp.m_PgSocketManager[iPcNum].GetLastResult(ii, jj));
				m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_PgSocketManager[iPcNum].GetLastContents(ii, jj));
				m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_PgSocketManager[iPcNum].GetLastRequest(ii, jj));
				iItemNum++;
			}
		}

		m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_FFUSerialCom->GetLastContents());
		m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_FFUSerialCom->GetLastRequest());
		iItemNum++;

		//m_ctrlComList.SetItemText(24, 1, _T("tq"));
		int iChCommand = m_cmbAlignChCommand.GetCurSel();
		UpdateStateButton(theApp.m_AlignConectStatus[iChCommand], &m_AlignState);
		if (theApp.m_PgConectStatus[PgServer_1] == TRUE && theApp.m_PgConectStatus[PgServer_2] == TRUE)
			bFlag = TRUE;

		UpdateStateButton(bFlag, &m_PgState);
		UpdateStateButton(theApp.m_FFUConectStatus, &m_FFUState);

		m_csList.Unlock();
	}
}

void CComGammaView::ClickAlignTrdTest1()
{
	int itwcie = 0;
	int itwcie1 = 0;
	int itwcie2 = 0;
	int itwcie3 = 0;
	
	for (int ii = 0; ii < theApp.m_AlignThread.size(); ii++)
	{
		switch (theApp.m_AlignThread[ii]->m_iAlignType)
		{
		case PatternAlign:itwcie = ii; break;
		case TrayCheck:itwcie1 = ii; break;
		case TrayAlign:itwcie2 = ii; break;
		case TrayLowerAlign:itwcie3 = ii; break;
		}
	}

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDB_ALIGN_TRD_TEST1:
		if (theApp.m_AlignSocketManager[itwcie]->m_bstart == FALSE)
		{
			theApp.m_AlignSocketManager[itwcie]->m_bstart = TRUE;
			for (int ii = 0; ii < ChMaxCount; ii++)
			{
				theApp.m_AlignSocketManager[itwcie]->m_testT[ii] = TRUE;
				theApp.m_AlignThread[itwcie]->AlignGrabMethod(ii, Align_Start_T, theApp.m_AlignThread[itwcie]->m_iAlignTypeNum);
			}
		}
		else theApp.m_AlignSocketManager[itwcie]->m_bstart = FALSE;
		break;
	case IDB_ALIGN_TRD_TEST2:
		if (theApp.m_AlignSocketManager[itwcie1]->m_bstart == FALSE)
		{
			theApp.m_AlignSocketManager[itwcie1]->m_bstart = TRUE;
			theApp.m_AlignThread[itwcie1]->TrayCheckNTrayAlignGrabMethod(PanelNum1);
		}
		else theApp.m_AlignSocketManager[itwcie1]->m_bstart = FALSE;
		break;
	case IDB_ALIGN_TRD_TEST3:
		if (theApp.m_AlignSocketManager[itwcie2]->m_bstart == FALSE)
		{
			theApp.m_AlignSocketManager[itwcie2]->m_bstart = TRUE;
			theApp.m_AlignThread[itwcie2]->TrayCheckNTrayAlignGrabMethod(PanelNum1);
		}
		else theApp.m_AlignSocketManager[itwcie2]->m_bstart = FALSE;
		break;
	case IDB_ALIGN_TRD_TEST4:
		if (theApp.m_AlignSocketManager[itwcie3]->m_bstart == FALSE)
		{
			theApp.m_AlignSocketManager[itwcie3]->m_bstart = TRUE;
			theApp.m_AlignThread[itwcie3]->TrayLowerAlignGrabMethod(PanelNum1, theApp.m_AlignThread[itwcie3]->m_iAlignTypeNum);
		}
		else theApp.m_AlignSocketManager[itwcie3]->m_bstart = FALSE;
		break;
	}
}

void CComGammaView::ClickFfuState()
{
	if (theApp.m_FFUConectStatus == FALSE)
		theApp.m_FFUSerialCom->OnPortOpen(_ttoi(theApp.m_strFFUPortNum));
}
#endif
