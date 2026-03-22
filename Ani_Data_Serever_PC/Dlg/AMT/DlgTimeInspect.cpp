// DlgTimeInspect.cpp : ???? ????????.
//

#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "DlgTimeInspect.h"
#include "afxdialogex.h"
#include "BtnEnhReadability.h"


// CDlgTimeInspect ??? ????????.

IMPLEMENT_DYNAMIC(CDlgTimeInspect, CDialog)

CDlgTimeInspect::CDlgTimeInspect(CWnd* pParent /*=NULL*/)
: CDialog(CDlgTimeInspect::IDD, pParent)
{
}

CDlgTimeInspect::~CDlgTimeInspect()
{
}

void CDlgTimeInspect::DoDataExchange(CDataExchange* pDX)
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

	DDX_Control(pDX, IDC_AOI_NG_1, m_btnAoi[InspectTime1]);
	DDX_Control(pDX, IDC_AOI_NG_2, m_btnAoi[InspectTime2]);
	DDX_Control(pDX, IDC_AOI_NG_3, m_btnAoi[InspectTime3]);
	DDX_Control(pDX, IDC_AOI_NG_4, m_btnAoi[InspectTime4]);
	DDX_Control(pDX, IDC_AOI_NG_5, m_btnAoi[InspectTime5]);
	DDX_Control(pDX, IDC_AOI_NG_6, m_btnAoi[InspectTime6]);
	DDX_Control(pDX, IDC_AOI_NG_7, m_btnAoi[InspectTime7]);
	DDX_Control(pDX, IDC_AOI_NG_8, m_btnAoi[InspectTime8]);
	DDX_Control(pDX, IDC_AOI_NG_9, m_btnAoi[InspectTime9]);
	DDX_Control(pDX, IDC_AOI_NG_10, m_btnAoi[InspectTime10]);
	DDX_Control(pDX, IDC_AOI_NG_11, m_btnAoi[InspectTime11]);
	DDX_Control(pDX, IDC_AOI_NG_12, m_btnAoi[InspectTime12]);
	DDX_Control(pDX, IDC_AOI_NG_13, m_btnAoiTotal);

	DDX_Control(pDX, IDC_VIEWING_NG_1, m_btnViewing[InspectTime1]);
	DDX_Control(pDX, IDC_VIEWING_NG_2, m_btnViewing[InspectTime2]);
	DDX_Control(pDX, IDC_VIEWING_NG_3, m_btnViewing[InspectTime3]);
	DDX_Control(pDX, IDC_VIEWING_NG_4, m_btnViewing[InspectTime4]);
	DDX_Control(pDX, IDC_VIEWING_NG_5, m_btnViewing[InspectTime5]);
	DDX_Control(pDX, IDC_VIEWING_NG_6, m_btnViewing[InspectTime6]);
	DDX_Control(pDX, IDC_VIEWING_NG_7, m_btnViewing[InspectTime7]);
	DDX_Control(pDX, IDC_VIEWING_NG_8, m_btnViewing[InspectTime8]);
	DDX_Control(pDX, IDC_VIEWING_NG_9, m_btnViewing[InspectTime9]);
	DDX_Control(pDX, IDC_VIEWING_NG_10, m_btnViewing[InspectTime10]);
	DDX_Control(pDX, IDC_VIEWING_NG_11, m_btnViewing[InspectTime11]);
	DDX_Control(pDX, IDC_VIEWING_NG_12, m_btnViewing[InspectTime12]);
	DDX_Control(pDX, IDC_VIEWING_NG_13, m_btnViewingTotal);

	DDX_Control(pDX, IDC_OTP_NG_1, m_btnOtp[InspectTime1]);
	DDX_Control(pDX, IDC_OTP_NG_2, m_btnOtp[InspectTime2]);
	DDX_Control(pDX, IDC_OTP_NG_3, m_btnOtp[InspectTime3]);
	DDX_Control(pDX, IDC_OTP_NG_4, m_btnOtp[InspectTime4]);
	DDX_Control(pDX, IDC_OTP_NG_5, m_btnOtp[InspectTime5]);
	DDX_Control(pDX, IDC_OTP_NG_6, m_btnOtp[InspectTime6]);
	DDX_Control(pDX, IDC_OTP_NG_7, m_btnOtp[InspectTime7]);
	DDX_Control(pDX, IDC_OTP_NG_8, m_btnOtp[InspectTime8]);
	DDX_Control(pDX, IDC_OTP_NG_9, m_btnOtp[InspectTime9]);
	DDX_Control(pDX, IDC_OTP_NG_10, m_btnOtp[InspectTime10]);
	DDX_Control(pDX, IDC_OTP_NG_11, m_btnOtp[InspectTime11]);
	DDX_Control(pDX, IDC_OTP_NG_12, m_btnOtp[InspectTime12]);
	DDX_Control(pDX, IDC_OTP_NG_13, m_btnOtpTotal);

	DDX_Control(pDX, IDC_TP_NG_1, m_btnTp[InspectTime1]);
	DDX_Control(pDX, IDC_TP_NG_2, m_btnTp[InspectTime2]);
	DDX_Control(pDX, IDC_TP_NG_3, m_btnTp[InspectTime3]);
	DDX_Control(pDX, IDC_TP_NG_4, m_btnTp[InspectTime4]);
	DDX_Control(pDX, IDC_TP_NG_5, m_btnTp[InspectTime5]);
	DDX_Control(pDX, IDC_TP_NG_6, m_btnTp[InspectTime6]);
	DDX_Control(pDX, IDC_TP_NG_7, m_btnTp[InspectTime7]);
	DDX_Control(pDX, IDC_TP_NG_8, m_btnTp[InspectTime8]);
	DDX_Control(pDX, IDC_TP_NG_9, m_btnTp[InspectTime9]);
	DDX_Control(pDX, IDC_TP_NG_10, m_btnTp[InspectTime10]);
	DDX_Control(pDX, IDC_TP_NG_11, m_btnTp[InspectTime11]);
	DDX_Control(pDX, IDC_TP_NG_12, m_btnTp[InspectTime12]);
	DDX_Control(pDX, IDC_TP_NG_13, m_btnTpTotal);

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

	DDX_Control(pDX, IDC_TRAY_OUT_1,  m_btnTrayOut[InspectTime1]);
	DDX_Control(pDX, IDC_TRAY_OUT_2,  m_btnTrayOut[InspectTime2]);
	DDX_Control(pDX, IDC_TRAY_OUT_3,  m_btnTrayOut[InspectTime3]);
	DDX_Control(pDX, IDC_TRAY_OUT_4,  m_btnTrayOut[InspectTime4]);
	DDX_Control(pDX, IDC_TRAY_OUT_5,  m_btnTrayOut[InspectTime5]);
	DDX_Control(pDX, IDC_TRAY_OUT_6,  m_btnTrayOut[InspectTime6]);
	DDX_Control(pDX, IDC_TRAY_OUT_7,  m_btnTrayOut[InspectTime7]);
	DDX_Control(pDX, IDC_TRAY_OUT_8,  m_btnTrayOut[InspectTime8]);
	DDX_Control(pDX, IDC_TRAY_OUT_9,  m_btnTrayOut[InspectTime9]);
	DDX_Control(pDX, IDC_TRAY_OUT_10, m_btnTrayOut[InspectTime10]);
	DDX_Control(pDX, IDC_TRAY_OUT_11, m_btnTrayOut[InspectTime11]);
	DDX_Control(pDX, IDC_TRAY_OUT_12, m_btnTrayOut[InspectTime12]);
	DDX_Control(pDX, IDC_TRAY_OUT_13, m_btnTrayOutTotal);

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


BEGIN_MESSAGE_MAP(CDlgTimeInspect, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgTimeInspect ????? ?????????.

BOOL CDlgTimeInspect::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iSelectShift = Shift_DY;
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDB_BTN_DY);
	pBtnEnh->SetValue(TRUE);

	ApplyBtnEnhReadabilitySubtree(this);

	// TODO:  ???? ??? ???? ????? ???????.
	SetTimer(TMR_MAIN_INSPECT_INFO, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// ????: OCX ??? ???????? FALSE?? ?????? ????.
}

BOOL CDlgTimeInspect::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgTimeInspect::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	// TODO: ???? ????? ????? ??? ??? ??/??? ?????? ???????.
	if (nIDEvent == TMR_MAIN_INSPECT_INFO)
	{
		UpdateDisplay(m_iSelectShift);
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgTimeInspect::UpdateDisplay(int nShift)
{
	SumProduction.Reset(nShift);

	theApp.AOIInspctionTimeDataSum(theApp.m_UiShift_TimeProduction, nShift, SumProduction);

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
		m_btnAoiTotal.SetWindowText(sTemp);
		m_btnViewingTotal.SetWindowText(sTemp);
		m_btnOtpTotal.SetWindowText(sTemp);
		m_btnTpTotal.SetWindowText(sTemp);
		m_btnTrayOutTotal.SetWindowText(sTemp);
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

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_VisionResult[nShift], SumProduction.m_VisionResult[nShift] / fNumTotal * 100);
		m_btnAoiTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_ViewingResult[nShift], SumProduction.m_ViewingResult[nShift] / fNumTotal * 100);
		m_btnViewingTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_PreGammaResult[nShift], SumProduction.m_PreGammaResult[nShift] / fNumTotal * 100);
		m_btnOtpTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_TpResult[nShift], SumProduction.m_TpResult[nShift] / fNumTotal * 100);
		m_btnTpTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_TrayDataOut[nShift], SumProduction.m_TrayDataOut[nShift] / fNumTotal * 100);
		m_btnTrayOutTotal.SetWindowText(sTemp);

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
			m_btnAoi[ii].SetWindowText(sTemp);
			m_btnViewing[ii].SetWindowText(sTemp);
			m_btnOtp[ii].SetWindowText(sTemp);
			m_btnTp[ii].SetWindowText(sTemp);
			m_btnTrayOut[ii].SetWindowText(sTemp);

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

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShift_TimeProduction[ii].m_VisionResult[nShift], theApp.m_UiShift_TimeProduction[ii].m_VisionResult[nShift] / fNumTotal * 100);
			m_btnAoi[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShift_TimeProduction[ii].m_ViewingResult[nShift], theApp.m_UiShift_TimeProduction[ii].m_ViewingResult[nShift] / fNumTotal * 100);
			m_btnViewing[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShift_TimeProduction[ii].m_PreGammaResult[nShift], theApp.m_UiShift_TimeProduction[ii].m_PreGammaResult[nShift] / fNumTotal * 100);
			m_btnOtp[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShift_TimeProduction[ii].m_TpResult[nShift], theApp.m_UiShift_TimeProduction[ii].m_TpResult[nShift] / fNumTotal * 100);
			m_btnTp[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShift_TimeProduction[ii].m_TrayDataOut[nShift], theApp.m_UiShift_TimeProduction[ii].m_TrayDataOut[nShift] / fNumTotal * 100);
			m_btnTrayOut[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_UiShift_TimeProduction[ii].m_AlignResult[nShift]);
			m_btnAlign[ii].SetWindowText(sTemp);
		}
	}
}


BEGIN_EVENTSINK_MAP(CDlgTimeInspect, CDialog)
	ON_EVENT(CDlgTimeInspect, IDB_BTN_DY, DISPID_CLICK, CDlgTimeInspect::ClickBtnDy, VTS_NONE)
	ON_EVENT(CDlgTimeInspect, IDB_BTN_NT, DISPID_CLICK, CDlgTimeInspect::ClickBtnNt, VTS_NONE)
	ON_EVENT(CDlgTimeInspect, IDC_DY_DATA_RESET, DISPID_CLICK, CDlgTimeInspect::ClickDyDataReset, VTS_NONE)
END_EVENTSINK_MAP()

void CDlgTimeInspect::ClickBtnDy()
{
	// TODO: ???? ????? ????? ??? ???????.
	m_iSelectShift = Shift_DY;
	UpdateDisplay(m_iSelectShift);
}

void CDlgTimeInspect::ClickBtnNt()
{
	// TODO: ???? ????? ????? ??? ???????.
	m_iSelectShift = Shift_NT;
	UpdateDisplay(m_iSelectShift);
}

void CDlgTimeInspect::ClickDyDataReset()
{
	// TODO: ???? ????? ????? ??? ???????.
	if (theApp.m_iUserClass != USER_MAKER)
	{
		theApp.getMsgBox(MS_OK, _T("??????? ??? ????????."), _T("Maker is USE"), _T("Maker is USE"));
		return;
	}
	else
	{
		if (theApp.YesNoMsgBox(MS_YESNO, _T("Do You Want To Data Clear?"), _T("Do You Want To Data Clear?"), _T("Do You Want To Data Clear?")) == MSG_OK)
		{
			theApp.m_pTraceLog->LOG_INFO(CStringSupport::FormatString(_T("************************ %s Time Data Reset************************"), ShiftDY_NT[m_iSelectShift]));
			for (int ii = 0; ii < InspectTimeTotalCount; ii++)
				theApp.m_UiShift_TimeProduction[ii].Reset(m_iSelectShift);

			theApp.AOIInspectionTimeDataSave(m_iSelectShift);
			theApp.AlignDataSave(m_iSelectShift);
			theApp.ContactDataSave(m_iSelectShift);
			theApp.TpDataSave(m_iSelectShift);
			theApp.PreGammaDataSave(m_iSelectShift);
		}
	}
}


#endif