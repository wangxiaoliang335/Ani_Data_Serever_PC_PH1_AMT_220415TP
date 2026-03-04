// SetTimerDlg.cpp : implementation file
//


#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "SetTimerDlg.h"
#include "afxdialogex.h"
#include "EzIni.h"
#include "GetNumDlg.h"
#include "DefectView.h"


// CSetTimerDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSetTimerDlg, CDialog)

CSetTimerDlg::CSetTimerDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSetTimerDlg::IDD, pParent)
{
}

CSetTimerDlg::~CSetTimerDlg()
{
}

void CSetTimerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetTimerDlg, CDialog)
END_MESSAGE_MAP()


// CCellInfo 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CSetTimerDlg, CDialog)
	ON_EVENT(CSetTimerDlg, IDB_BTN_OK, DISPID_CLICK, CSetTimerDlg::OnBtnOk, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDB_BTN_RELOAD, DISPID_CLICK, CSetTimerDlg::ReLoadTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_ALIGN_GRAB_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_ALIGN_LAST_GRAB_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_VISION_GRAB_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_VISION_LAST_GRAB_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_VIEWING_ANGLE_GRAB_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_VIEWING_ANGLE_LAST_GRAB_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_PG_OTP_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_PG_CONTACT_ON_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_PG_CONTACT_OFF_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_PG_NEXT_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_PG_BACK_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_PG_TP_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_START_TIME, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_END_TIME, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_LUMITOP_GRAB_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDS_LUMITOP_LAST_GRAB_TIMER, DISPID_CLICK, CSetTimerDlg::ClickSetTimer, VTS_NONE)
	ON_EVENT(CSetTimerDlg, IDB_BTN_SAVE, DISPID_CLICK, CSetTimerDlg::ClickBtnSave, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CSetTimerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	if (theApp.m_iMachineType == SetAMT)
	{
		GetDlgItem(IDC_STATIC_LUMITOP_TITLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LUMITOP_TITLE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LUMITOP_TITLE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDS_LUMITOP_GRAB_TIMER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDS_LUMITOP_LAST_GRAB_TIMER)->ShowWindow(SW_HIDE);
	}
	else if (theApp.m_iMachineType == SetGAMMA)
	{
		GetDlgItem(IDC_STATIC_VIEW_ANGE_TITLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_VIEW_ANGLE_TITLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_VIEW_ANGLE_TITLE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDS_VIEWING_ANGLE_GRAB_TIMER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDS_VIEWING_ANGLE_LAST_GRAB_TIMER)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_VISION_TITLE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_VISION_1_TITLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_VISION_1_TITLE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDS_VISION_GRAB_TIMER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDS_VISION_LAST_GRAB_TIMER)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_LUMITOP_TITLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LUMITOP_TITLE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LUMITOP_TITLE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDS_LUMITOP_GRAB_TIMER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDS_LUMITOP_LAST_GRAB_TIMER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PG_TITLE6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDS_PG_TP_TIMER)->ShowWindow(SW_HIDE);
	}
	
	DisplayTimerInfo();
	return TRUE;
}


void CSetTimerDlg::OnBtnOk()
{
	CDialog::OnOK();
}

void CSetTimerDlg::DisplayTimerInfo()
{
	CString strValue;

	strValue.Format(_T("%d"), theApp.m_iTimer[ViewingAngleGrabTimer]);
	GetDlgItem(IDS_VIEWING_ANGLE_GRAB_TIMER)->SetWindowTextW(strValue);
	
	strValue.Format(_T("%d"), theApp.m_iTimer[ViewingAngleLastGrabTimer]);
	GetDlgItem(IDS_VIEWING_ANGLE_LAST_GRAB_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[VisionGrabTimer]);
	GetDlgItem(IDS_VISION_GRAB_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[VisionLastGrabTimer]);
	GetDlgItem(IDS_VISION_LAST_GRAB_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[PGGammaTimer]);
	GetDlgItem(IDS_PG_OTP_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[PGContactOnTimer]);
	GetDlgItem(IDS_PG_CONTACT_ON_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[PGContactOffTimer]);
	GetDlgItem(IDS_PG_CONTACT_OFF_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[PGNextTimer]);
	GetDlgItem(IDS_PG_NEXT_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[PGBackTimer]);
	GetDlgItem(IDS_PG_BACK_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[PGTPTimer]);
	GetDlgItem(IDS_PG_TP_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[AlignGrabTimer]);
	GetDlgItem(IDS_ALIGN_GRAB_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[AlignLastGrabTimer]);
	GetDlgItem(IDS_ALIGN_LAST_GRAB_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[LumitopGrabTimer]);
	GetDlgItem(IDS_LUMITOP_GRAB_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iTimer[LumitopLastGrabTimer]);
	GetDlgItem(IDS_LUMITOP_LAST_GRAB_TIMER)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iShiftTime[Shift_Start]);
	GetDlgItem(IDS_START_TIME)->SetWindowTextW(strValue);

	strValue.Format(_T("%d"), theApp.m_iShiftTime[Shift_End]);
	GetDlgItem(IDS_END_TIME)->SetWindowTextW(strValue);
}
void CSetTimerDlg::ClickSetTimer(int Name)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CGetNumDlg Dlg;
	CString strValue;
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	int iBtnID = pBtnEnh->GetDlgCtrlID();

	if (Dlg.DoModal() == DLG_OK)
	{
		int iValue = _ttoi(Dlg.GetstrNum());

		if (iValue == 0)
			return;

		strValue.Format(_T("%d"), iValue);

		switch (iBtnID)
		{
		case IDS_VISION_GRAB_TIMER:				theApp.m_iTimer[VisionGrabTimer] = iValue; break;
		case IDS_VISION_LAST_GRAB_TIMER:		theApp.m_iTimer[VisionLastGrabTimer] = iValue; break;
		case IDS_VIEWING_ANGLE_GRAB_TIMER:		theApp.m_iTimer[ViewingAngleGrabTimer] = iValue; break;
		case IDS_VIEWING_ANGLE_LAST_GRAB_TIMER:	theApp.m_iTimer[ViewingAngleLastGrabTimer] = iValue; break;
		case IDS_PG_OTP_TIMER:					theApp.m_iTimer[PGGammaTimer] = iValue; break;
		case IDS_PG_CONTACT_ON_TIMER:			theApp.m_iTimer[PGContactOnTimer] = iValue; break;
		case IDS_PG_CONTACT_OFF_TIMER:			theApp.m_iTimer[PGContactOffTimer] = iValue; break;
		case IDS_PG_NEXT_TIMER:					theApp.m_iTimer[PGNextTimer] = iValue; break;
		case IDS_PG_BACK_TIMER:					theApp.m_iTimer[PGBackTimer] = iValue; break;
		case IDS_PG_TP_TIMER:					theApp.m_iTimer[PGTPTimer] = iValue; break;
		case IDS_ALIGN_GRAB_TIMER:				theApp.m_iTimer[AlignGrabTimer] = iValue; break;
		case IDS_ALIGN_LAST_GRAB_TIMER:			theApp.m_iTimer[AlignLastGrabTimer] = iValue; break;
		case IDS_LUMITOP_GRAB_TIMER:			theApp.m_iTimer[LumitopGrabTimer] = iValue; break;
		case IDS_LUMITOP_LAST_GRAB_TIMER:		theApp.m_iTimer[LumitopLastGrabTimer] = iValue; break;
		case IDS_START_TIME:					theApp.m_iShiftTime[Shift_Start] = iValue; break;
		case IDS_END_TIME:						theApp.m_iShiftTime[Shift_End] = iValue; break;
		}

		GetDlgItem(iBtnID)->SetWindowTextW(strValue);
	}

}

void CSetTimerDlg::ReLoadTimer()
{
	theApp.LoadSetTimer();
	DisplayTimerInfo();
}

void CSetTimerDlg::ClickBtnSave()
{
	if (theApp.YesNoMsgBox(MS_YESNO, _T("저장 하시겠습니까?"), _T("Do You Want To Save?"), _T("Do You Want To Save?")) == MSG_OK)
	{
		theApp.SaveSetTimer();
		theApp.m_pTraceLog->LOG_INFO(_T("************************Timer Save ************************"));
		theApp.getMsgBox(MS_OK, _T("Timer Save Success"), _T("Timer Save Success"), _T("Timer Save Success"));
	}
}
