// ComView.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "Ani_Data_Serever_PC.h"
#include "ComView.h"
#include "StringSupport.h"
#include "DFSInfo.h"

#define IF_CHECK_TIMER 0
#define TMR_SERVER_CONNECT_CHECK 1 //210422 yjlim

CComView::CComView()
	: CFormView(CComView::IDD)
{
	//>>210422 
	for (int ii = 0; ii < 1; ii++) // 서버에 붙는 갯수 추가 시 추가하자.
	{
		CMachineManager *SocketClient = new CMachineManager(ii);
		m_SocketClient.push_back(SocketClient);

		m_bPcConnect[ii] = FALSE;
		m_bFlag[ii] = FALSE;
	}
	//<< 210422
}

IMPLEMENT_DYNCREATE(CComView, CFormView)
CComView::~CComView()
{
}

void CComView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COM_LIST, m_ctrlComList);
	DDX_Control(pDX, IDC_CMB_PG_COMMAND, m_cmbPgCommand);
	DDX_Control(pDX, IDC_CMB_VISION_COMMAND, m_cmbVisionCommand);
	DDX_Control(pDX, IDC_CMB_ALIGN_COMMAND, m_cmbAlignCommand);
	DDX_Control(pDX, IDC_CMB_TP_COMMAND, m_cmbTpCommand);
	DDX_Control(pDX, IDC_CMB_VIEWING_ANGLE_COMMAND, m_cmbViewingAngleCommand);
	DDX_Control(pDX, IDC_CMB_VISION_NUM, m_cmbVisionCount);
	DDX_Control(pDX, IDC_CMB_VIEWING_ANGLE_NUM, m_cmbViewingAngleCount);
	DDX_Control(pDX, IDC_CMB_TP_NUM, m_cmbTpCount);
	DDX_Control(pDX, IDC_CMB_PG_CH_COMMAND, m_cmbPgChCommand);
	DDX_Control(pDX, IDC_CMB_FFU_COMMAND, m_cmbFFUCommand);
	DDX_Control(pDX, IDC_CMB_ALIGN_CH_COMMAND2, m_cmbAlignChCommand);
	DDX_Control(pDX, IDC_CMB_LUMITOP_COMMAND, m_cmbLumitopCommand);
	DDX_Control(pDX, IDC_CMB_LUMITOP_NUM, m_cmbLumitopCount);

	DDX_Control(pDX, IDC_CMB_OPV_NUM, m_cmbOpvCount);
	DDX_Control(pDX, IDC_CMB_OPV_COMMAND, m_cmbOpvCommand);
	DDX_Control(pDX, IDC_OPV_STATE, m_OpvState);

	DDX_Control(pDX, IDC_VISION_STATE, m_VisionState);
	DDX_Control(pDX, IDC_ALIGN_STATE, m_AlignState);
	DDX_Control(pDX, IDC_VIEWING_ANGLE_STATE, m_ViewingAngleState);
	DDX_Control(pDX, IDC_PG_STATE, m_PgState);
	DDX_Control(pDX, IDC_TP_STATE, m_TpState);
	DDX_Control(pDX, IDC_FFU_STATE, m_FFUState);
	DDX_Control(pDX, IDC_LUMITOP_STATE, m_LumitopState);

}

BEGIN_MESSAGE_MAP(CComView, CFormView)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CComView, CFormView)
	ON_EVENT(CComView, IDC_SEND_VISION_COMMAND, DISPID_CLICK, CComView::OnClickSendVisionCommand, VTS_NONE)
	ON_EVENT(CComView, IDC_SEND_ALIGN_COMMAND, DISPID_CLICK, CComView::OnClickSendAlignCommand, VTS_NONE)
	ON_EVENT(CComView, IDC_SEND_VIEWING_ANGLE_COMMAND, DISPID_CLICK, CComView::OnClickSendViewingAngleCommand, VTS_NONE)
	ON_EVENT(CComView, IDC_SEND_TP_COMMAND, DISPID_CLICK, CComView::OnClickSendTpCommand, VTS_NONE)
	ON_EVENT(CComView, IDC_SEND_PG_COMMAND, DISPID_CLICK, CComView::OnClickSendPgCommand, VTS_NONE)
	ON_EVENT(CComView, IDC_SEND_FFU_COMMAND2, DISPID_CLICK, CComView::ClickSendFfuCommand2, VTS_NONE)
	ON_EVENT(CComView, IDC_FFU_STATE, DISPID_CLICK, CComView::ClickFfuState, VTS_NONE)
	ON_EVENT(CComView, IDC_SEND_OPV_COMMAND, DISPID_CLICK, CComView::OnClickSendOpvCommand, VTS_NONE)
	ON_EVENT(CComView, IDC_SEND_LUMITOP_COMMAND, DISPID_CLICK, CComView::ClickSendLumitopCommand, VTS_NONE)
END_EVENTSINK_MAP()

// CComView 진단입니다.

#ifdef _DEBUG
void CComView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CComView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CComView 메시지 처리기입니다.
void CComView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	if (theApp.m_iMachineType == SetAMT)
	{
		GetDlgItem(IDC_STATIC_LUMITOP_COMMUNICATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LUMITOP_STATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LUMITOP_NUM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LUMITOP_COMMAND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LUMITOP_DATA)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LUMITOP_DATA)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMB_LUMITOP_NUM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CMB_LUMITOP_COMMAND)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SEND_LUMITOP_COMMAND)->ShowWindow(SW_HIDE);
	}

	ChangeAlignList();

	SetTimer(IF_CHECK_TIMER, 2000, NULL);
	//>> 0428 psh
	SetTimer(TMR_SERVER_CONNECT_CHECK, 3000, NULL);
}

void CComView::ChangeAlignList()
{
	m_ctrlComList.DeleteAllItems();
	m_cmbPgCommand.ResetContent();
	m_cmbVisionCommand.ResetContent();
	m_cmbAlignCommand.ResetContent();
	m_cmbTpCommand.ResetContent();
	m_cmbViewingAngleCommand.ResetContent();
	m_cmbVisionCount.ResetContent();
	m_cmbViewingAngleCount.ResetContent();
	m_cmbTpCount.ResetContent();
	m_cmbPgChCommand.ResetContent();
	m_cmbFFUCommand.ResetContent();
	m_cmbAlignChCommand.ResetContent();
	m_cmbOpvCount.ResetContent();
	m_cmbOpvCommand.ResetContent();
	m_cmbLumitopCommand.ResetContent();
	m_cmbLumitopCount.ResetContent();

	CString msg, strName;

	m_ctrlComList.InsertColumn(0, _T("Name"), 0, 150);
	m_ctrlComList.InsertColumn(1, _T("Last Command"), 0, 200);
	m_ctrlComList.InsertColumn(2, _T("Result"), 0, 100);
	m_ctrlComList.InsertColumn(3, _T("Response"), 0, 400);
	m_ctrlComList.InsertColumn(4, _T("Request"), 0, 400);


	for (int ii = 0; ii < MC_PACKET_MAX_NUM; ii++)
	{
		m_cmbVisionCommand.AddString(MC_PacketNameTable[ii]);
		m_cmbAlignCommand.AddString(MC_PacketNameTable[ii]);
		m_cmbViewingAngleCommand.AddString(MC_PacketNameTable[ii]);
		m_cmbOpvCommand.AddString(MC_PacketNameTable[ii]);
		m_cmbLumitopCommand.AddString(MC_PacketNameTable[ii]);
	}

	for (int ii = 0; ii < _ttoi(theApp.m_strAlignCount); ii++)
	{
		strName.Format(_T("%d"), ii + 1);
		m_cmbAlignChCommand.AddString(strName);
	}

	m_cmbTpCommand.AddString(_T("DOT TEST"));

	for (int ii = 0; ii < PgCommand_Max_Num; ii++)
		m_cmbPgCommand.AddString(PG_PacketNameTable[ii]);

	for (int ii = 1; ii <= PG_MAX_CH; ii++)
	{
		strName.Format(_T("%d"), ii);
		m_cmbPgChCommand.AddString(strName);
		m_cmbTpCount.AddString(strName);
	}

	m_cmbFFUCommand.AddString(_T("Current Data"));
	m_cmbFFUCommand.AddString(_T("Setting Data"));

	for (int ii = 0; ii < PCMaxCount; ii++)
	{
		msg.Format(_T("%d"), ii + 1);
		m_cmbVisionCount.AddString(msg);

		strName.Format(_T("Vision %d Ch"), ii + 1);
		m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);
	}

	m_cmbVisionCount.AddString(_T("ALL"));


	for (int ii = 0; ii < PanelMaxCount; ii++)
	{
		msg.Format(_T("%d"), ii + 1);
		m_cmbViewingAngleCount.AddString(msg);

		strName.Format(_T("Viewing Angle %d Ch"), ii + 1);
		m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);
	}

	m_cmbViewingAngleCount.AddString(_T("ALL"));

	for (int ii = 0; ii < _ttoi(theApp.m_strAlignCount); ii++)
	{
		strName.Format(_T("Align %d"), ii + 1);
		m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);
	}

	if (theApp.m_iMachineType == SetAFT)
	{
		for (int ii = 0; ii < PCMaxCount; ii++)
		{
			msg.Format(_T("%d"), ii + 1);
			m_cmbLumitopCount.AddString(msg);

			strName.Format(_T("Lumitop %d Ch"), ii + 1);
			m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);
		}

		m_cmbLumitopCount.AddString(_T("ALL"));
	}

	for (int ii = 0, Ch = 0; ii < MaxZone; ii++)
	{
		for (int jj = 0; jj < PanelMaxCount; jj++, Ch++)
		{
			strName.Format(_T("%s Ch %d"), PG_IndexName[ii], Ch + 1);
			m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);
		}
	}

	strName = _T("MStage1 Ch 17");
	m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);

	strName = _T("MStage2 Ch 18");
	m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);

	for (int ii = 1; ii <= PG_MAX_CH; ii++)
	{
		strName.Format(_T("TP %d"), ii);
		m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);
	}

	m_cmbTpCount.AddString(_T("ALL"));

	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		msg.Format(_T("%d"), ii + 1);
		m_cmbOpvCount.AddString(msg);

		strName.Format(_T("Opv %d Ch"), ii + 1);
		m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), strName);
	}
	m_cmbOpvCount.AddString(_T("ALL"));

	m_ctrlComList.InsertItem(m_ctrlComList.GetItemCount(), _T("FFU"));

	m_cmbPgCommand.SetCurSel(0);
	m_cmbVisionCommand.SetCurSel(0);
	m_cmbAlignCommand.SetCurSel(0);
	m_cmbTpCommand.SetCurSel(0);
	m_cmbViewingAngleCommand.SetCurSel(0);
	m_cmbVisionCount.SetCurSel(0);
	m_cmbViewingAngleCount.SetCurSel(0);
	m_cmbTpCount.SetCurSel(0);
	m_cmbPgChCommand.SetCurSel(0);
	m_cmbFFUCommand.SetCurSel(0);
	m_cmbAlignChCommand.SetCurSel(0);
	m_cmbOpvCount.SetCurSel(0);
	m_cmbOpvCommand.SetCurSel(0);
	m_cmbLumitopCommand.SetCurSel(0);
	m_cmbLumitopCount.SetCurSel(0);

	SetComboBoxReadOnly(IDC_CMB_OPV_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_PG_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_VISION_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_ALIGN_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_TP_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_VIEWING_ANGLE_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_VISION_NUM);
	SetComboBoxReadOnly(IDC_CMB_TP_NUM);
	SetComboBoxReadOnly(IDC_CMB_PG_CH_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_FFU_COMMAND);
	SetComboBoxReadOnly(IDC_CMB_ALIGN_CH_COMMAND2);
	SetComboBoxReadOnly(IDC_CMB_LUMITOP_COMMAND);
}

void CComView::SetComboBoxReadOnly(int item)
{
	CWnd *p_combo = GetDlgItem(item);
	HWND h_wnd = ::FindWindowEx(p_combo->m_hWnd, NULL, _T("Edit"), NULL);
	if (NULL != h_wnd) ((CEdit *)CWnd::FromHandle(h_wnd))->SetReadOnly(TRUE);
}

BOOL CComView::PlcStart()
{
	if (theApp.m_PLCStatus)
	{
		theApp.getMsgBox(MS_OK, _T("PLC Auto Mode Start"), _T("PLC Auto Mode Start"), _T("PLC Auto Mode Start"));
		return TRUE;
	}
	return FALSE;
}


void CComView::StringChanged()
{
	StringChnageMsg(IDC_STATIC_PG_COMMUNICATION, _T("PG 통신"), _T("PG Communication"), _T("PG 通信"));
	StringChnageMsg(IDC_STATIC_ALIGN_COMMUNICATION, _T("Align 통신"), _T("Align Communication"), _T("Align 通信"));
	StringChnageMsg(IDC_STATIC_VEWING_COMMUNICATION, _T("시야각 통신"), _T("Viewing Communication"), _T("测试检 通信"));
	StringChnageMsg(IDC_STATIC_TP_COMMUNICATION, _T("TP 통신"), _T("TP Communication"), _T("TP 通信"));
	StringChnageMsg(IDC_STATIC_VISION_COMMUNICATION, _T("화면검사 통신"), _T("Vision Communication"), _T("画面检查 通信"));
	
	StringChnageMsg(IDC_STATIC_PG_CH_COMMAND, _T("Ch"), _T("Ch"), _T("频道"));
	StringChnageMsg(IDC_STATIC_VISION_NUM, _T("Num"), _T("Num"), _T("编号"));
	StringChnageMsg(IDC_STATIC_TP_NUM, _T("Num"), _T("Num"), _T("编号"));
	
	StringChnageMsg(IDC_STATIC_PG_COMMAND, _T("Command"), _T("Command"), _T("命令"));
	StringChnageMsg(IDC_STATIC_ALIGN_COMMAND, _T("Command"), _T("Command"), _T("命令"));
	StringChnageMsg(IDC_STATIC_VIEWING_ANGLE_COMMAND, _T("Command"), _T("Command"), _T("命令"));
	StringChnageMsg(IDC_STATIC_VISION_COMMAND, _T("Command"), _T("Command"), _T("命令"));
	StringChnageMsg(IDC_STATIC_TP_COMMAND, _T("Command"), _T("Command"), _T("命令"));
	
	StringChnageMsg(IDC_STATIC_PG_DATA, _T("Data"), _T("Data"), _T("数据"));
	StringChnageMsg(IDC_STATIC_ALIGN_DATA, _T("Data"), _T("Data"), _T("数据"));
	StringChnageMsg(IDC_STATIC_VIEWING_ANGLE_DATA, _T("Data"), _T("Data"), _T("数据"));
	StringChnageMsg(IDC_STATIC_VISION_DATA, _T("Data"), _T("Data"), _T("数据"));
	
	StringChnageMsg(IDC_PG_STATE, _T("연결"), _T("Connected"), _T("连接"));
	StringChnageMsg(IDC_VISION_STATE, _T("연결"), _T("Connected"), _T("连接"));
	StringChnageMsg(IDC_ALIGN_STATE, _T("연결"), _T("Connected"), _T("连接"));
	StringChnageMsg(IDC_VIEWING_ANGLE_STATE, _T("연결"), _T("Connected"), _T("连接"));
	StringChnageMsg(IDC_TP_STATE, _T("연결"), _T("Connected"), _T("连接"));
	
	StringChnageMsg(IDC_SEND_PG_COMMAND, _T("전송"), _T("Send"), _T("发送"));
	StringChnageMsg(IDC_SEND_VISION_COMMAND, _T("전송"), _T("Send"), _T("发送"));
	StringChnageMsg(IDC_SEND_ALIGN_COMMAND, _T("전송"), _T("Send"), _T("发送"));
	StringChnageMsg(IDC_SEND_VIEWING_ANGLE_COMMAND, _T("전송"), _T("Send"), _T("发送"));
	StringChnageMsg(IDC_SEND_TP_COMMAND, _T("전송"), _T("Send"), _T("发送"));

}

void CComView::StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi)
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

void CComView::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	if (nIDEvent == IF_CHECK_TIMER)
	{
		m_csList.Lock();

		UpdateData();

		int iItemNum = 0;

		for (int ii = 0; ii < PCMaxCount; ii++)
		{
			m_ctrlComList.SetItemText(iItemNum, 1, theApp.m_VisionSocketManager[ii].GetLastCommand(ii));
			m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_VisionThread->GetLastContents(ii));
			m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_VisionSocketManager[ii].GetLastRequest(ii));
			iItemNum++;
		}

		for (int ii = 0; ii < PanelMaxCount; ii++)
		{
			m_ctrlComList.SetItemText(iItemNum, 1, theApp.m_ViewingAngleSocketManager[ii].GetLastCommand(ii));
			m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_ViewingAngleThread->GetLastContents(ii));
			m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_ViewingAngleSocketManager[ii].GetLastRequest(ii));
			iItemNum++;
		}

		for (int ii = 0; ii < _ttoi(theApp.m_strAlignCount); ii++)
		{
			m_ctrlComList.SetItemText(iItemNum, 1, theApp.m_AlignSocketManager[ii]->GetLastCommand());
			m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_AlignSocketManager[ii]->GetLastContents());
			m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_AlignSocketManager[ii]->GetLastRequest());
			iItemNum++;
		}

		if (theApp.m_iMachineType == SetAFT)
		{
			for (int ii = 0; ii < PCMaxCount; ii++)
			{
				m_ctrlComList.SetItemText(iItemNum, 1, theApp.m_LumitopSocketManager[ii].GetLastCommand(ii));
				m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_LumitopThread->GetLastContents(ii));
				m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_LumitopSocketManager[ii].GetLastRequest(ii));
				iItemNum++;
			}
		}

		for (int ii = 0; ii < PG_MAX_CH; ii++)
		{
			int iPgNum = ii / 16;
			if (iPgNum == 1)
				iPgNum += abs(ii - PG_CH_17);

			m_ctrlComList.SetItemText(iItemNum, 1, theApp.m_PgSocketManager[iPgNum].GetLastCommand(ii));
			m_ctrlComList.SetItemText(iItemNum, 2, theApp.m_PgSocketManager[iPgNum].GetLastResult(ii));
			m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_PgSocketManager[iPgNum].GetLastContents(ii));
			m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_PgSocketManager[iPgNum].GetLastRequest(ii));
			iItemNum++;
		}

		for (int ii = 0; ii < PG_MAX_CH; ii++)
		{
			m_ctrlComList.SetItemText(iItemNum, 2, theApp.m_TpSocketManager.GetLastResult(ii));
			m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_TpSocketManager.GetLastContents(ii));
			m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_TpSocketManager.GetLastRequest(ii));
			iItemNum++;
		}

		for (int ii = 0; ii < ChMaxCount; ii++)
		{
			m_ctrlComList.SetItemText(iItemNum, 1, theApp.m_OpvSocketManager[ii].GetLastCommand(ii));
			m_ctrlComList.SetItemText(iItemNum, 2, theApp.m_OpvSocketManager[ii].GetLastResult(ii));
			m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_OpvSocketManager[ii].GetLastContents(ii));
			m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_OpvSocketManager[ii].GetLastRequest(ii));
			iItemNum++;
		}

		m_ctrlComList.SetItemText(iItemNum, 3, theApp.m_FFUSerialCom->GetLastContents());
		m_ctrlComList.SetItemText(iItemNum, 4, theApp.m_FFUSerialCom->GetLastRequest());
		iItemNum++;

		// 5601 端口 (Lighting) 连接状态
		BOOL flag = theApp.m_LightingConectStatus;
		UpdateStateButton(flag, &m_VisionState);

		flag = theApp.m_ViewingAngleConectStatus[PanelNum1] && theApp.m_ViewingAngleConectStatus[PanelNum2] && 
			theApp.m_ViewingAngleConectStatus[PanelNum3] && theApp.m_ViewingAngleConectStatus[PanelNum4];
		UpdateStateButton(flag, &m_ViewingAngleState);

		UpdateStateButton(theApp.m_TpConectStatus, &m_TpState);
		UpdateStateButton(theApp.m_AlignConectStatus[m_cmbAlignChCommand.GetCurSel()], &m_AlignState);

		flag = theApp.m_PgConectStatus[PgServer_1] && theApp.m_PgConectStatus[PgServer_2] &&
			theApp.m_PgConectStatus[PgServer_3];
		UpdateStateButton(flag, &m_PgState);
		UpdateStateButton(theApp.m_FFUConectStatus, &m_FFUState);

		flag = theApp.m_OpvConectStatus[CH_1] && theApp.m_OpvConectStatus[CH_2];
		UpdateStateButton(flag, &m_OpvState);

		flag = theApp.m_LumitopConectStatus[PC1] && theApp.m_LumitopConectStatus[PC2];
		UpdateStateButton(flag, &m_LumitopState);

		m_csList.Unlock();
	}
	//>> 0428 psh
	if (nIDEvent == TMR_SERVER_CONNECT_CHECK)
	{
		/*if (theApp.m_pDataSys->m_iMachineStartPort == 0)
		return;*/

		for (int ii = 0; ii < /*theApp.m_pDataSys->m_iMachinePcCount + 현재 MesAdapter 하나만 연결*/ 1; ii++)
		{
			if (m_SocketClient[ii]->IsOpen())
			{
				m_bPcConnect[ii] = TRUE;
			}
			else
			{
				m_bPcConnect[ii] = FALSE;
				//m_strStartPort.Format(_T("%d"), theApp.m_pDataSys->m_iMachineStartPort + ii);
				bool clientOpenChecka = m_SocketClient[ii]->ConnectClient(this, _T("192.168.1.101"), theApp.m_strMesAdapterPort);

				if (clientOpenChecka) {
					m_bPcConnect[ii] = TRUE;
					m_SocketClient[ii]->WatchComm();
				}
			}
			if (m_bFlag[ii] == !m_bPcConnect[ii])
			{
				m_bFlag[ii] = m_bPcConnect[ii];
				if (m_bFlag[ii])
					m_Btn_Port[ii].SetBackColorInterior(GREEN);
				else
					m_Btn_Port[ii].SetBackColorInterior(RED);
			}
		}
	}
	CFormView::OnTimer(nIDEvent);
}

void CComView::UpdateStateButton(BOOL bState, CBtnEnh* pStateBtn)
{
	if (bState)
		pStateBtn->SetBackColorInterior(TGREEN);
	else
		pStateBtn->SetBackColorInterior(TRED);
}

BOOL CComView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


void CComView::OnClickSendVisionCommand()
{
	if (PlcStart())
		return;

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int icommand = m_cmbVisionCommand.GetCurSel();
	int iPcNum = m_cmbVisionCount.GetCurSel();

	if (iPcNum == PC1)
	{
		if (!theApp.m_LightingConectStatus)
		{
			theApp.getMsgBox(MS_OK, _T("检查PC2连接确认."), _T("Vision PC2 Connect Check"), _T("检查PC连接确认"));
			return;
		}
	}
	else if (iPcNum == PC2)
	{
		if (!theApp.m_LightingConectStatus)
		{
			theApp.getMsgBox(MS_OK, _T("检查 PC1/PC2 连接确认."), _T("Vision PC1/PC2 Connect Check"), _T("检查PC连接确认"));
			return;
		}
	}
	else
	{
		if (!theApp.m_LightingConectStatus)
		{
			theApp.getMsgBox(MS_OK, _T("检查PC1连接确认."), _T("Vision PC1 Connect Check"), _T("检查PC连接确认"));
			return;
		}
	}


	if (icommand < 0 || iPcNum < 0){
		theApp.getMsgBox(MS_OK, _T("Command or PC 번호 선택하세요."), _T("Command or PcNum Choice"), _T("选择命令或者PC编号"));
		return;
	}
		
	CString strData, sendMsg;
	GetDlgItemText(IDC_VISION_DATA, strData);
	sendMsg.Format(_T("%d,%s"), icommand, strData);

	if (iPcNum == PCMaxCount)
	{
		theApp.m_VisionThread->SocketSendto(PC1, sendMsg, icommand);
		theApp.m_VisionThread->SocketSendto(PC2, sendMsg, icommand);
	}
	else
	{
		theApp.m_VisionThread->SocketSendto(iPcNum, sendMsg, icommand);
	}
}

void CComView::OnClickSendAlignCommand()
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

void CComView::OnClickSendViewingAngleCommand()
{
	if (PlcStart())
		return;

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int icommand = m_cmbViewingAngleCommand.GetCurSel();
	int iPcNum = m_cmbViewingAngleCount.GetCurSel();


	if (iPcNum == PanelNum1)
	{
		if (!theApp.m_ViewingAngleConectStatus[PanelNum1])
		{
			theApp.getMsgBox(MS_OK, _T("시야각 PC1 연결 확인하세요."), _T("Viewing Angle PC1 Connect Check"), _T("侧视检查PC连接确认"));
			return;
		}
	}
	else if (iPcNum == PanelNum2)
	{
		if (!theApp.m_ViewingAngleConectStatus[PanelNum2])
		{
			theApp.getMsgBox(MS_OK, _T("시야각 PC2 연결 확인하세요."), _T("Viewing Angle PC2 Connect Check"), _T("侧视检查PC连接确认"));
			return;
		}
	}
	else if (iPcNum == PanelNum3)
	{
		if (!theApp.m_ViewingAngleConectStatus[PanelNum3])
		{
			theApp.getMsgBox(MS_OK, _T("시야각 PC3 연결 확인하세요."), _T("Viewing Angle PC3 Connect Check"), _T("侧视检查PC连接确认"));
			return;
		}
	}
	else if (iPcNum == PanelNum4)
	{
		if (!theApp.m_ViewingAngleConectStatus[PanelNum4])
		{
			theApp.getMsgBox(MS_OK, _T("시야각 PC4 연결 확인하세요."), _T("Viewing Angle PC4 Connect Check"), _T("侧视检查PC连接确认"));
			return;
		}
	}
	else
	{
		if (!theApp.m_ViewingAngleConectStatus[PanelNum1] || !theApp.m_ViewingAngleConectStatus[PanelNum2]
			|| !theApp.m_ViewingAngleConectStatus[PanelNum3] || !theApp.m_ViewingAngleConectStatus[PanelNum4])
		{
			theApp.getMsgBox(MS_OK, _T("시야각 PC 연결 확인하세요."), _T("Viewing Angle PC Connect Check"), _T("侧视检查PC连接确认"));
			return;
		}
	}


	if (icommand < 0){
		theApp.getMsgBox(MS_OK, _T("Command 선택하세요."), _T("Command Choice"), _T("选择指令"));
		return;
	}

	CString strData, sendMsg;
	GetDlgItemText(IDC_VIEWING_ANGLE_DATA, strData);
	sendMsg.Format(_T("%d,%s"), icommand, strData);

	if (iPcNum == PanelMaxCount)
	{
		for (int ii = 0; ii < PanelMaxCount; ii++)
			theApp.m_ViewingAngleThread->SocketSendto(ii, sendMsg, icommand);
	}
	else
	{
		theApp.m_ViewingAngleThread->SocketSendto(iPcNum, sendMsg, icommand);
	}
}


void CComView::OnClickSendTpCommand()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (PlcStart())
		return;
	
	//m_cmbTpCount
	int icommand = m_cmbTpCommand.GetCurSel();
	int iZoneSelect = m_cmbTpCount.GetCurSel();

	if (!theApp.m_TpConectStatus)
	{
		theApp.getMsgBox(MS_OK, _T("TP 연결 확인하세요."), _T("TP Connect Check"), _T("TP 连接确认"));
		return;
	}

	if (icommand < 0)
	{
		theApp.getMsgBox(MS_OK, _T("Command 선택하세요."), _T("Command Choice"), _T("选择指令"));
		return;
	}

	if (iZoneSelect < 0)
	{
		theApp.getMsgBox(MS_OK, _T("Zone 선택하세요."), _T("Zone Choice"), _T("Zone Choice"));
		return;
	}

	CString strData, sendMsg;

	if (iZoneSelect == PG_MAX_CH)
	{
		for (int ii = 0; ii < PG_MAX_CH; ii++)
		{
			theApp.m_TpSocketManager.SendTPMessage(ii, TP_SendPanelID, _T("PanelID_TEST"));
			Delay(20);
			theApp.m_TpSocketManager.SendTPMessage(ii, TP_InspStart);
		}
	}
	else
	{
		theApp.m_TpSocketManager.SendTPMessage(iZoneSelect, TP_SendPanelID, _T("PanelID_TEST"));
		Delay(20);
		theApp.m_TpSocketManager.SendTPMessage(iZoneSelect, TP_InspStart);
	}
}

void CComView::ClickSendFfuCommand2()
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
		GetDlgItemText(IDC_FFU_DATA2, strData);

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


void CComView::OnClickSendPgCommand()
{
	//if (PlcStart())
	//	return;

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	int icommand = m_cmbPgCommand.GetCurSel();
	int iChCommand = m_cmbPgChCommand.GetCurSel();
	int iPgNum = iChCommand / 16;

	if (iPgNum == 1)
		iPgNum += abs(iChCommand - PG_CH_17);

	if (!theApp.m_PgConectStatus[iPgNum])
	{
		theApp.getMsgBox(MS_OK, _T("PG PC 연결 확인하세요."), _T("PG PC Connect Check"),_T("PG PC连接确认"));
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
	sendMsg.Format(_T("%s"), PG_PacketNameTable[icommand]);

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

	theApp.m_PgSocketManager[iPgNum].SendPGMessage(sendMsg, iChCommand + 1);
}


void CComView::OnClickSendOpvCommand()
{
	int icommand = m_cmbOpvCommand.GetCurSel();
	int iChCommand = m_cmbOpvCount.GetCurSel();

	if (!theApp.m_OpvConectStatus)
	{
		theApp.getMsgBox(MS_OK, _T("OPV PC 연결 확인하세요."), _T("OPV PC Connect Check"), _T("OPV PC连接确认"));
		return;
	}

	if (icommand < 0){
		theApp.getMsgBox(MS_OK, _T("Command 선택하세요."), _T("Command Choice"), _T("选择指令"));
		return;
	}

	if (iChCommand < 0){
		theApp.getMsgBox(MS_OK, _T("Ch 선택하세요."), _T("Ch Choice"), _T("Ch Choice"));
		return;
	}

	CString strData, sendMsg;
	GetDlgItemText(IDC_OPV_DATA, strData);

	sendMsg.Format(_T("%d,%s"), icommand, strData);

	if (iChCommand == ChMaxCount)
	{
		for (int ii = 0; ii < ChMaxCount; ii++)
			theApp.m_OpvSocketManager[ii].SendOpvMessage(sendMsg, ii, icommand);
	}
	else
	{
		theApp.m_OpvSocketManager[iChCommand].SendOpvMessage(sendMsg, iChCommand, icommand);
	}
}

void CComView::ClickFfuState()
{
	if (theApp.m_FFUConectStatus == FALSE)
		theApp.m_FFUSerialCom->OnPortOpen(_ttoi(theApp.m_strFFUPortNum));
}

void CComView::ClickSendLumitopCommand()
{
	if (PlcStart())
		return;

	int icommand = m_cmbLumitopCommand.GetCurSel();
	int iPcNum = m_cmbLumitopCount.GetCurSel();

	if (iPcNum == PC1)
	{
		if (!theApp.m_LumitopConectStatus[PC1])
		{
			theApp.getMsgBox(MS_OK, _T("루미탑 PC1 연결 확인하세요."), _T("Lumitop PC2 Connect Check"), _T("检查PC连接确认"));
			return;
		}
	}
	else if (iPcNum == PC2)
	{
		if (!theApp.m_LumitopConectStatus[PC2])
		{
			theApp.getMsgBox(MS_OK, _T("루미탑 PC2 연결 확인하세요."), _T("Lumitop PC2 Connect Check"), _T("检查PC连接确认"));
			return;
		}
	}
	else
	{
		if (!theApp.m_LumitopConectStatus[PC1] || !theApp.m_LumitopConectStatus[PC2])
		{
			theApp.getMsgBox(MS_OK, _T("루미탑 PC1/PC2 연결 확인하세요."), _T("Lumitop PC1/PC2 Connect Check"), _T("检查PC连接确认"));
			return;
		}
	}


	if (icommand < 0 || iPcNum < 0){
		theApp.getMsgBox(MS_OK, _T("Command or PC 번호 선택하세요."), _T("Command or PcNum Choice"), _T("选择命令或者PC编号"));
		return;
	}

	CString strData, sendMsg;
	GetDlgItemText(IDC_LUMITOP_DATA, strData);
	sendMsg.Format(_T("%d,%s"), icommand, strData);

	if (iPcNum == PCMaxCount)
	{
		theApp.m_LumitopThread->SocketSendto(PC1, sendMsg, icommand);
		theApp.m_LumitopThread->SocketSendto(PC2, sendMsg, icommand);
	}
	else
	{
		theApp.m_LumitopThread->SocketSendto(iPcNum, sendMsg, icommand);
	}
}

#endif

