// DlgGammaTimeInspect.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#if _SYSTEM_GAMMA_
#include "Ani_Data_Serever_PC.h"
#include "DlgGammaTimeInspect.h"
#include "afxdialogex.h"


// CDlgGammaTimeInspect 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgGammaTimeInspect, CDialog)

CDlgGammaTimeInspect::CDlgGammaTimeInspect(CWnd* pParent /*=NULL*/)
: CDialog(CDlgGammaTimeInspect::IDD, pParent)
{
}

CDlgGammaTimeInspect::~CDlgGammaTimeInspect()
{
}

void CDlgGammaTimeInspect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIME_1, m_btnTime[InspectTime1]);
	DDX_Control(pDX, IDC_TIME_2, m_btnTime[InspectTime2]);
	DDX_Control(pDX, IDC_TIME_3, m_btnTime[InspectTime3]);
	DDX_Control(pDX, IDC_TIME_4, m_btnTime[InspectTime4]);
	DDX_Control(pDX, IDC_TIME_5, m_btnTime[InspectTime5]);
	DDX_Control(pDX, IDC_TIME_6, m_btnTime[InspectTime6]);
	DDX_Control(pDX, IDC_TIME_7, m_btnTime[InspectTime7]);
	DDX_Control(pDX, IDC_TIME_8, m_btnTime[InspectTime8]);
	DDX_Control(pDX, IDC_TIME_9, m_btnTime[InspectTime9]);
	DDX_Control(pDX, IDC_TIME_10, m_btnTime[InspectTime10]);
	DDX_Control(pDX, IDC_TIME_11, m_btnTime[InspectTime11]);
	DDX_Control(pDX, IDC_TIME_12, m_btnTime[InspectTime12]);
	DDX_Control(pDX, IDC_TIME_13, m_btnTimeTotal);

	DDX_Control(pDX, IDC_GOOD_1, m_btnGood[InspectTime1]);
	DDX_Control(pDX, IDC_GOOD_2, m_btnGood[InspectTime2]);
	DDX_Control(pDX, IDC_GOOD_3, m_btnGood[InspectTime3]);
	DDX_Control(pDX, IDC_GOOD_4, m_btnGood[InspectTime4]);
	DDX_Control(pDX, IDC_GOOD_5, m_btnGood[InspectTime5]);
	DDX_Control(pDX, IDC_GOOD_6, m_btnGood[InspectTime6]);
	DDX_Control(pDX, IDC_GOOD_7, m_btnGood[InspectTime7]);
	DDX_Control(pDX, IDC_GOOD_8, m_btnGood[InspectTime8]);
	DDX_Control(pDX, IDC_GOOD_9, m_btnGood[InspectTime9]);
	DDX_Control(pDX, IDC_GOOD_10, m_btnGood[InspectTime10]);
	DDX_Control(pDX, IDC_GOOD_11, m_btnGood[InspectTime11]);
	DDX_Control(pDX, IDC_GOOD_12, m_btnGood[InspectTime12]);
	DDX_Control(pDX, IDC_GOOD_13, m_btnGoodTotal);

	DDX_Control(pDX, IDC_NG_1, m_btnNg[InspectTime1]);
	DDX_Control(pDX, IDC_NG_2, m_btnNg[InspectTime2]);
	DDX_Control(pDX, IDC_NG_3, m_btnNg[InspectTime3]);
	DDX_Control(pDX, IDC_NG_4, m_btnNg[InspectTime4]);
	DDX_Control(pDX, IDC_NG_5, m_btnNg[InspectTime5]);
	DDX_Control(pDX, IDC_NG_6, m_btnNg[InspectTime6]);
	DDX_Control(pDX, IDC_NG_7, m_btnNg[InspectTime7]);
	DDX_Control(pDX, IDC_NG_8, m_btnNg[InspectTime8]);
	DDX_Control(pDX, IDC_NG_9, m_btnNg[InspectTime9]);
	DDX_Control(pDX, IDC_NG_10, m_btnNg[InspectTime10]);
	DDX_Control(pDX, IDC_NG_11, m_btnNg[InspectTime11]);
	DDX_Control(pDX, IDC_NG_12, m_btnNg[InspectTime12]);
	DDX_Control(pDX, IDC_NG_13, m_btnNgTotal);

	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_1, m_btnAutoContact[InspectTime1]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_2, m_btnAutoContact[InspectTime2]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_3, m_btnAutoContact[InspectTime3]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_4, m_btnAutoContact[InspectTime4]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_5, m_btnAutoContact[InspectTime5]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_6, m_btnAutoContact[InspectTime6]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_7, m_btnAutoContact[InspectTime7]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_8, m_btnAutoContact[InspectTime8]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_9, m_btnAutoContact[InspectTime9]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_10, m_btnAutoContact[InspectTime10]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_11, m_btnAutoContact[InspectTime11]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_12, m_btnAutoContact[InspectTime12]);
	DDX_Control(pDX, IDC_AUTO_CONTACT_NG_13, m_btnAutoContactTotal);

	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_1, m_btnManualContact[InspectTime1]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_2, m_btnManualContact[InspectTime2]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_3, m_btnManualContact[InspectTime3]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_4, m_btnManualContact[InspectTime4]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_5, m_btnManualContact[InspectTime5]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_6, m_btnManualContact[InspectTime6]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_7, m_btnManualContact[InspectTime7]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_8, m_btnManualContact[InspectTime8]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_9, m_btnManualContact[InspectTime9]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_10, m_btnManualContact[InspectTime10]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_11, m_btnManualContact[InspectTime11]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_12, m_btnManualContact[InspectTime12]);
	DDX_Control(pDX, IDC_MANUAL_CONTACT_NG_13, m_btnManualContactTotal);

	DDX_Control(pDX, IDC_MTP_NG_1, m_btnMtp[InspectTime1]);
	DDX_Control(pDX, IDC_MTP_NG_2, m_btnMtp[InspectTime2]);
	DDX_Control(pDX, IDC_MTP_NG_3, m_btnMtp[InspectTime3]);
	DDX_Control(pDX, IDC_MTP_NG_4, m_btnMtp[InspectTime4]);
	DDX_Control(pDX, IDC_MTP_NG_5, m_btnMtp[InspectTime5]);
	DDX_Control(pDX, IDC_MTP_NG_6, m_btnMtp[InspectTime6]);
	DDX_Control(pDX, IDC_MTP_NG_7, m_btnMtp[InspectTime7]);
	DDX_Control(pDX, IDC_MTP_NG_8, m_btnMtp[InspectTime8]);
	DDX_Control(pDX, IDC_MTP_NG_9, m_btnMtp[InspectTime9]);
	DDX_Control(pDX, IDC_MTP_NG_10, m_btnMtp[InspectTime10]);
	DDX_Control(pDX, IDC_MTP_NG_11, m_btnMtp[InspectTime11]);
	DDX_Control(pDX, IDC_MTP_NG_12, m_btnMtp[InspectTime12]);
	DDX_Control(pDX, IDC_MTP_NG_13, m_btnMtpTotal);

	DDX_Control(pDX, IDC_ALIGN_NG_1, m_btnAlign[InspectTime1]);
	DDX_Control(pDX, IDC_ALIGN_NG_2, m_btnAlign[InspectTime2]);
	DDX_Control(pDX, IDC_ALIGN_NG_3, m_btnAlign[InspectTime3]);
	DDX_Control(pDX, IDC_ALIGN_NG_4, m_btnAlign[InspectTime4]);
	DDX_Control(pDX, IDC_ALIGN_NG_5, m_btnAlign[InspectTime5]);
	DDX_Control(pDX, IDC_ALIGN_NG_6, m_btnAlign[InspectTime6]);
	DDX_Control(pDX, IDC_ALIGN_NG_7, m_btnAlign[InspectTime7]);
	DDX_Control(pDX, IDC_ALIGN_NG_8, m_btnAlign[InspectTime8]);
	DDX_Control(pDX, IDC_ALIGN_NG_9, m_btnAlign[InspectTime9]);
	DDX_Control(pDX, IDC_ALIGN_NG_10, m_btnAlign[InspectTime10]);
	DDX_Control(pDX, IDC_ALIGN_NG_11, m_btnAlign[InspectTime11]);
	DDX_Control(pDX, IDC_ALIGN_NG_12, m_btnAlign[InspectTime12]);
	DDX_Control(pDX, IDC_ALIGN_NG_13, m_btnAlignTotal);

	DDX_Control(pDX, IDC_UNKNOW_NG_1, m_btnSum[InspectTime1]);
	DDX_Control(pDX, IDC_UNKNOW_NG_2, m_btnSum[InspectTime2]);
	DDX_Control(pDX, IDC_UNKNOW_NG_3, m_btnSum[InspectTime3]);
	DDX_Control(pDX, IDC_UNKNOW_NG_4, m_btnSum[InspectTime4]);
	DDX_Control(pDX, IDC_UNKNOW_NG_5, m_btnSum[InspectTime5]);
	DDX_Control(pDX, IDC_UNKNOW_NG_6, m_btnSum[InspectTime6]);
	DDX_Control(pDX, IDC_UNKNOW_NG_7, m_btnSum[InspectTime7]);
	DDX_Control(pDX, IDC_UNKNOW_NG_8, m_btnSum[InspectTime8]);
	DDX_Control(pDX, IDC_UNKNOW_NG_9, m_btnSum[InspectTime9]);
	DDX_Control(pDX, IDC_UNKNOW_NG_10, m_btnSum[InspectTime10]);
	DDX_Control(pDX, IDC_UNKNOW_NG_11, m_btnSum[InspectTime11]);
	DDX_Control(pDX, IDC_UNKNOW_NG_12, m_btnSum[InspectTime12]);
	DDX_Control(pDX, IDC_UNKNOW_NG_13, m_btnSumTotal);
}

BEGIN_MESSAGE_MAP(CDlgGammaTimeInspect, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgTimeInspect 메시지 처리기입니다.

BOOL CDlgGammaTimeInspect::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iSelectShift = Shift_DY;
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDB_BTN_DY);
	pBtnEnh->SetValue(TRUE);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetTimer(TMR_MAIN_INSPECT_INFO, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CDlgGammaTimeInspect::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgGammaTimeInspect::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == TMR_MAIN_INSPECT_INFO)
	{
		UpdateDisplay(m_iSelectShift);
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgGammaTimeInspect::UpdateDisplay(int nShift)
{
	SumProduction.Reset(nShift);

	theApp.InspctionTimeDataSum(theApp.m_UiShift_TimeProduction, nShift, SumProduction);

	CString sTemp;
	sTemp.Format(_T("%s ~ %s"), GetTimeString(theApp.m_stuTimeInspect[nShift][InspectTime1].m_iShiftTimeStart),
		GetTimeString(theApp.m_stuTimeInspect[nShift][InspectTime12].m_iShiftTimeEnd));

	m_btnTimeTotal.SetWindowText(sTemp);

	sTemp.Format(_T("%d"), SumProduction.m_InspectionTotal[nShift]);
	m_btnSumTotal.SetWindowText(sTemp);
	float fNumTotal = 0.;
	if (SumProduction.m_InspectionTotal[nShift] == 0)
	{
		sTemp.Format(_T("0 (0.0%%)"));
		m_btnGoodTotal.SetWindowText(sTemp);
		m_btnNgTotal.SetWindowText(sTemp);
		m_btnAutoContactTotal.SetWindowText(sTemp);
		m_btnManualContactTotal.SetWindowText(sTemp);
		m_btnMtpTotal.SetWindowText(sTemp);

		sTemp.Format(_T("0"));
		m_btnAlignTotal.SetWindowText(sTemp);
	}
	else
	{
		fNumTotal = SumProduction.m_InspectionTotal[nShift];

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_GoodResult[nShift], SumProduction.m_GoodResult[nShift] / fNumTotal * 100);
		m_btnGoodTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_BadResult[nShift], SumProduction.m_BadResult[nShift] / fNumTotal * 100);
		m_btnNgTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_ContactResult[nShift], SumProduction.m_ContactResult[nShift] / fNumTotal * 100);
		m_btnAutoContactTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_ManualContactResult[nShift], SumProduction.m_ManualContactResult[nShift] / fNumTotal * 100);
		m_btnManualContactTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_MtpResult[nShift], SumProduction.m_MtpResult[nShift] / fNumTotal * 100);
		m_btnMtpTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_AlignResult[nShift]);
		m_btnAlignTotal.SetWindowText(sTemp);
	}

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		sTemp.Format(_T("%s ~ %s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart),
			GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeEnd));
		m_btnTime[ii].SetWindowText(sTemp);

		sTemp.Format(_T("%d"), theApp.m_UiShift_TimeProduction[ii].m_InspectionTotal[nShift]);
		m_btnSum[ii].SetWindowText(sTemp);

		if (theApp.m_UiShift_TimeProduction[ii].m_InspectionTotal[nShift] == 0)
		{
			sTemp.Format(_T("0 (0.0%%)"));
			m_btnGood[ii].SetWindowText(sTemp);
			m_btnNg[ii].SetWindowText(sTemp);
			m_btnAutoContact[ii].SetWindowText(sTemp);
			m_btnManualContact[ii].SetWindowText(sTemp);
			m_btnMtp[ii].SetWindowText(sTemp);

			sTemp.Format(_T("0"));
			m_btnAlign[ii].SetWindowText(sTemp);
		}
		else
		{
			fNumTotal = theApp.m_UiShift_TimeProduction[ii].m_InspectionTotal[nShift];

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShift_TimeProduction[ii].m_GoodResult[nShift], theApp.m_UiShift_TimeProduction[ii].m_GoodResult[nShift] / fNumTotal * 100);
			m_btnGood[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShift_TimeProduction[ii].m_BadResult[nShift], theApp.m_UiShift_TimeProduction[ii].m_BadResult[nShift] / fNumTotal * 100);
			m_btnNg[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShift_TimeProduction[ii].m_ContactResult[nShift], theApp.m_UiShift_TimeProduction[ii].m_ContactResult[nShift] / fNumTotal * 100);
			m_btnAutoContact[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShift_TimeProduction[ii].m_ManualContactResult[nShift], theApp.m_UiShift_TimeProduction[ii].m_ManualContactResult[nShift] / fNumTotal * 100);
			m_btnManualContact[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShift_TimeProduction[ii].m_MtpResult[nShift], theApp.m_UiShift_TimeProduction[ii].m_MtpResult[nShift] / fNumTotal * 100);
			m_btnMtp[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_UiShift_TimeProduction[ii].m_AlignResult[nShift]);
			m_btnAlign[ii].SetWindowText(sTemp);
		}
	}
}

BEGIN_EVENTSINK_MAP(CDlgGammaTimeInspect, CDialog)
	ON_EVENT(CDlgGammaTimeInspect, IDB_BTN_DY, DISPID_CLICK, CDlgGammaTimeInspect::ClickBtnShift, VTS_NONE)
	ON_EVENT(CDlgGammaTimeInspect, IDB_BTN_NT, DISPID_CLICK, CDlgGammaTimeInspect::ClickBtnShift, VTS_NONE)
	ON_EVENT(CDlgGammaTimeInspect, IDC_DY_DATA_RESET, DISPID_CLICK, CDlgGammaTimeInspect::ClickDataReset, VTS_NONE)
END_EVENTSINK_MAP()

void CDlgGammaTimeInspect::ClickBtnShift()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDB_BTN_DY: m_iSelectShift = Shift_DY; break;
	case IDB_BTN_NT: m_iSelectShift = Shift_NT; break;
	}

	UpdateDisplay(m_iSelectShift);
}

void CDlgGammaTimeInspect::ClickDataReset()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (theApp.m_iUserClass != USER_MAKER)
	{
		theApp.getMsgBox(MS_OK, _T("관리자만 이용 가능합니다."), _T("Maker is USE"), _T("Maker is USE"));
		return;
	}
	else
	{
		if (theApp.YesNoMsgBox(MS_YESNO, _T("Do You Want To Data Clear?"), _T("Do You Want To Data Clear?"), _T("Do You Want To Data Clear?")) == MSG_OK)
		{
			theApp.m_pTraceLog->LOG_INFO(CStringSupport::FormatString(_T("************************ %s Time Data Reset************************"), ShiftDY_NT[m_iSelectShift]));
			for (int ii = 0; ii < InspectTimeTotalCount; ii++)
				theApp.m_UiShift_TimeProduction[ii].Reset(m_iSelectShift);

			theApp.InspectionTimeDataSave(m_iSelectShift);
			theApp.AlignDataSave(m_iSelectShift);
			theApp.ContactDataSave(m_iSelectShift);
			theApp.MtpDataSave(m_iSelectShift);
		}
	}
}

#endif