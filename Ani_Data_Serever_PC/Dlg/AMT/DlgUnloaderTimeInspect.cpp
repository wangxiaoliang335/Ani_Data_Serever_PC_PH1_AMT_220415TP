// DlgTimeInspect.cpp : ±¸Çö ÆÄÀÏÀÔ´Ï´Ù.
//

#include "stdafx.h"
#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "DlgUnloaderTimeInspect.h"
#include "afxdialogex.h"

// CDlgUnloaderTimeInspect ´ëÈ­ »óÀÚÀÔ´Ï´Ù.

IMPLEMENT_DYNAMIC(CDlgUnloaderTimeInspect, CDialog)

CDlgUnloaderTimeInspect::CDlgUnloaderTimeInspect(CWnd* pParent /*=NULL*/)
: CDialog(CDlgUnloaderTimeInspect::IDD, pParent)
{
}

CDlgUnloaderTimeInspect::~CDlgUnloaderTimeInspect()
{
}

void CDlgUnloaderTimeInspect::DoDataExchange(CDataExchange* pDX)
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

	DDX_Control(pDX, IDC_OPV_NG_1,  m_btnManualOpv[InspectTime1]);
	DDX_Control(pDX, IDC_OPV_NG_2,  m_btnManualOpv[InspectTime2]);
	DDX_Control(pDX, IDC_OPV_NG_3,  m_btnManualOpv[InspectTime3]);
	DDX_Control(pDX, IDC_OPV_NG_4,  m_btnManualOpv[InspectTime4]);
	DDX_Control(pDX, IDC_OPV_NG_5,  m_btnManualOpv[InspectTime5]);
	DDX_Control(pDX, IDC_OPV_NG_6,  m_btnManualOpv[InspectTime6]);
	DDX_Control(pDX, IDC_OPV_NG_7,  m_btnManualOpv[InspectTime7]);
	DDX_Control(pDX, IDC_OPV_NG_8,  m_btnManualOpv[InspectTime8]);
	DDX_Control(pDX, IDC_OPV_NG_9,  m_btnManualOpv[InspectTime9]);
	DDX_Control(pDX, IDC_OPV_NG_10, m_btnManualOpv[InspectTime10]);
	DDX_Control(pDX, IDC_OPV_NG_11, m_btnManualOpv[InspectTime11]);
	DDX_Control(pDX, IDC_OPV_NG_12, m_btnManualOpv[InspectTime12]);
	DDX_Control(pDX, IDC_OPV_NG_13, m_btnManualOpvTotal);

	DDX_Control(pDX, IDC_TOUCH_NG_1,  m_btnManualTouch[InspectTime1]);
	DDX_Control(pDX, IDC_TOUCH_NG_2,  m_btnManualTouch[InspectTime2]);
	DDX_Control(pDX, IDC_TOUCH_NG_3,  m_btnManualTouch[InspectTime3]);
	DDX_Control(pDX, IDC_TOUCH_NG_4,  m_btnManualTouch[InspectTime4]);
	DDX_Control(pDX, IDC_TOUCH_NG_5,  m_btnManualTouch[InspectTime5]);
	DDX_Control(pDX, IDC_TOUCH_NG_6,  m_btnManualTouch[InspectTime6]);
	DDX_Control(pDX, IDC_TOUCH_NG_7,  m_btnManualTouch[InspectTime7]);
	DDX_Control(pDX, IDC_TOUCH_NG_8,  m_btnManualTouch[InspectTime8]);
	DDX_Control(pDX, IDC_TOUCH_NG_9,  m_btnManualTouch[InspectTime9]);
	DDX_Control(pDX, IDC_TOUCH_NG_10, m_btnManualTouch[InspectTime10]);
	DDX_Control(pDX, IDC_TOUCH_NG_11, m_btnManualTouch[InspectTime11]);
	DDX_Control(pDX, IDC_TOUCH_NG_12, m_btnManualTouch[InspectTime12]);
	DDX_Control(pDX, IDC_TOUCH_NG_13, m_btnManualTouchTotal);

	DDX_Control(pDX, IDC_GAMMA_NG_1,  m_btnManualGamma[InspectTime1]);
	DDX_Control(pDX, IDC_GAMMA_NG_2,  m_btnManualGamma[InspectTime2]);
	DDX_Control(pDX, IDC_GAMMA_NG_3,  m_btnManualGamma[InspectTime3]);
	DDX_Control(pDX, IDC_GAMMA_NG_4,  m_btnManualGamma[InspectTime4]);
	DDX_Control(pDX, IDC_GAMMA_NG_5,  m_btnManualGamma[InspectTime5]);
	DDX_Control(pDX, IDC_GAMMA_NG_6,  m_btnManualGamma[InspectTime6]);
	DDX_Control(pDX, IDC_GAMMA_NG_7,  m_btnManualGamma[InspectTime7]);
	DDX_Control(pDX, IDC_GAMMA_NG_8,  m_btnManualGamma[InspectTime8]);
	DDX_Control(pDX, IDC_GAMMA_NG_9,  m_btnManualGamma[InspectTime9]);
	DDX_Control(pDX, IDC_GAMMA_NG_10, m_btnManualGamma[InspectTime10]);
	DDX_Control(pDX, IDC_GAMMA_NG_11, m_btnManualGamma[InspectTime11]);
	DDX_Control(pDX, IDC_GAMMA_NG_12, m_btnManualGamma[InspectTime12]);
	DDX_Control(pDX, IDC_GAMMA_NG_13, m_btnManualGammaTotal);

	DDX_Control(pDX, IDC_BUFFER_TRAY_1,  m_btnBufferTray[InspectTime1]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_2,  m_btnBufferTray[InspectTime2]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_3,  m_btnBufferTray[InspectTime3]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_4,  m_btnBufferTray[InspectTime4]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_5,  m_btnBufferTray[InspectTime5]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_6,  m_btnBufferTray[InspectTime6]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_7,  m_btnBufferTray[InspectTime7]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_8,  m_btnBufferTray[InspectTime8]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_9,  m_btnBufferTray[InspectTime9]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_10, m_btnBufferTray[InspectTime10]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_11, m_btnBufferTray[InspectTime11]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_12, m_btnBufferTray[InspectTime12]);
	DDX_Control(pDX, IDC_BUFFER_TRAY_13, m_btnBufferTrayTotal);

	DDX_Control(pDX, IDC_SAMPLE_OUT_1,  m_btnSample[InspectTime1]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_2,  m_btnSample[InspectTime2]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_3,  m_btnSample[InspectTime3]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_4,  m_btnSample[InspectTime4]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_5,  m_btnSample[InspectTime5]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_6,  m_btnSample[InspectTime6]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_7,  m_btnSample[InspectTime7]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_8,  m_btnSample[InspectTime8]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_9,  m_btnSample[InspectTime9]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_10, m_btnSample[InspectTime10]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_11, m_btnSample[InspectTime11]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_12, m_btnSample[InspectTime12]);
	DDX_Control(pDX, IDC_SAMPLE_OUT_13, m_btnSampleTotal);

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


BEGIN_MESSAGE_MAP(CDlgUnloaderTimeInspect, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgUnloaderTimeInspect ¸Þ½ÃÁö Ã³¸®±âÀÔ´Ï´Ù.

BOOL CDlgUnloaderTimeInspect::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iSelectShift = Shift_DY;
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDB_BTN_DY);
	pBtnEnh->SetValue(TRUE);

	// TODO:  ¿©±â¿¡ Ãß°¡ ÃÊ±âÈ­ ÀÛ¾÷À» Ãß°¡ÇÕ´Ï´Ù.
	SetTimer(TMR_MAIN_INSPECT_INFO, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// ¿¹¿Ü: OCX ¼Ó¼º ÆäÀÌÁö´Â FALSE¸¦ ¹ÝÈ¯ÇØ¾ß ÇÕ´Ï´Ù.
}

BOOL CDlgUnloaderTimeInspect::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgUnloaderTimeInspect::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ ¹×/¶Ç´Â ±âº»°ªÀ» È£ÃâÇÕ´Ï´Ù.
	if (nIDEvent == TMR_MAIN_INSPECT_INFO)
	{
		UpdateDisplay(m_iSelectShift);
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgUnloaderTimeInspect::UpdateDisplay(int nShift)
{
	SumProduction.Reset(nShift);

	theApp.ULDInspctionTimeDataSum(theApp.m_ULDUiShift_TimeProduction, nShift, SumProduction);

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
		m_btnManualOpvTotal.SetWindowText(sTemp);
		m_btnManualTouchTotal.SetWindowText(sTemp);
		m_btnManualGammaTotal.SetWindowText(sTemp);

		sTemp.Format(_T("0"));
		m_btnAlignTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_BufferTrayResult[nShift]);
		m_btnBufferTrayTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_SampleResult[nShift]);
		m_btnSampleTotal.SetWindowText(sTemp);
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

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_OpvResult[nShift], SumProduction.m_OpvResult[nShift] / fNumTotal * 100);
		m_btnManualOpvTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_TouchResult[nShift], SumProduction.m_TouchResult[nShift] / fNumTotal * 100);
		m_btnManualTouchTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_GammaResult[nShift], SumProduction.m_GammaResult[nShift] / fNumTotal * 100);
		m_btnManualGammaTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_BufferTrayResult[nShift]);
		m_btnBufferTrayTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_SampleResult[nShift]);
		m_btnSampleTotal.SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_AlignResult[nShift]);
		m_btnAlignTotal.SetWindowText(sTemp);
	}

	for (int ii = 0; ii < InspectTimeTotalCount; ii++)
	{
		
		sTemp.Format(_T("%s ~ %s"), GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeStart),
			GetTimeString(theApp.m_stuTimeInspect[nShift][ii].m_iShiftTimeEnd));
		m_btnTime[ii].SetWindowText(sTemp);

		sTemp.Format(_T("%d"), theApp.m_ULDUiShift_TimeProduction[ii].m_InspectionTotal[nShift]);
		m_btnSum[ii].SetWindowText(sTemp);

		if (theApp.m_ULDUiShift_TimeProduction[ii].m_InspectionTotal[nShift] == 0)
		{
			sTemp.Format(_T("0 (0.0%%)"));
			m_btnGood[ii].SetWindowText(sTemp);
			m_btnNg[ii].SetWindowText(sTemp);
			m_btnAutoContact[ii].SetWindowText(sTemp);
			m_btnManualOpv[ii].SetWindowText(sTemp);
			m_btnManualTouch[ii].SetWindowText(sTemp);
			m_btnManualGamma[ii].SetWindowText(sTemp);

			sTemp.Format(_T("0"));
			m_btnAlign[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_ULDUiShift_TimeProduction[ii].m_BufferTrayResult[nShift]);
			m_btnBufferTray[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_ULDUiShift_TimeProduction[ii].m_SampleResult[nShift]);
			m_btnSample[ii].SetWindowText(sTemp);
		}
		else
		{
			fNumTotal = theApp.m_ULDUiShift_TimeProduction[ii].m_InspectionTotal[nShift];

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShift_TimeProduction[ii].m_GoodResult[nShift], theApp.m_ULDUiShift_TimeProduction[ii].m_GoodResult[nShift] / fNumTotal * 100);
			m_btnGood[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShift_TimeProduction[ii].m_BadResult[nShift], theApp.m_ULDUiShift_TimeProduction[ii].m_BadResult[nShift] / fNumTotal * 100);
			m_btnNg[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShift_TimeProduction[ii].m_ContactResult[nShift], theApp.m_ULDUiShift_TimeProduction[ii].m_ContactResult[nShift] / fNumTotal * 100);
			m_btnAutoContact[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShift_TimeProduction[ii].m_OpvResult[nShift], theApp.m_ULDUiShift_TimeProduction[ii].m_OpvResult[nShift] / fNumTotal * 100);
			m_btnManualOpv[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShift_TimeProduction[ii].m_TouchResult[nShift], theApp.m_ULDUiShift_TimeProduction[ii].m_TouchResult[nShift] / fNumTotal * 100);
			m_btnManualTouch[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShift_TimeProduction[ii].m_GammaResult[nShift], theApp.m_ULDUiShift_TimeProduction[ii].m_GammaResult[nShift] / fNumTotal * 100);
			m_btnManualGamma[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_ULDUiShift_TimeProduction[ii].m_BufferTrayResult[nShift]);
			m_btnBufferTray[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_ULDUiShift_TimeProduction[ii].m_SampleResult[nShift]);
			m_btnSample[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_ULDUiShift_TimeProduction[ii].m_AlignResult[nShift]);
			m_btnAlign[ii].SetWindowText(sTemp);
		}
	}
}


BEGIN_EVENTSINK_MAP(CDlgUnloaderTimeInspect, CDialog)
	ON_EVENT(CDlgUnloaderTimeInspect, IDB_BTN_DY, DISPID_CLICK, CDlgUnloaderTimeInspect::ClickBtnDy, VTS_NONE)
	ON_EVENT(CDlgUnloaderTimeInspect, IDB_BTN_NT, DISPID_CLICK, CDlgUnloaderTimeInspect::ClickBtnNt, VTS_NONE)
	ON_EVENT(CDlgUnloaderTimeInspect, IDC_DY_DATA_RESET, DISPID_CLICK, CDlgUnloaderTimeInspect::ClickDyDataReset, VTS_NONE)
END_EVENTSINK_MAP()

void CDlgUnloaderTimeInspect::ClickBtnDy()
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	m_iSelectShift = Shift_DY;
	UpdateDisplay(m_iSelectShift);
}

void CDlgUnloaderTimeInspect::ClickBtnNt()
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	m_iSelectShift = Shift_NT;
	UpdateDisplay(m_iSelectShift);
}

void CDlgUnloaderTimeInspect::ClickDyDataReset()
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
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
				theApp.m_ULDUiShift_TimeProduction[ii].Reset(m_iSelectShift);

			theApp.ULDInspectionTimeDataSave(m_iSelectShift);
			theApp.ULDAlignDataSave(m_iSelectShift);
		}
	}
}
#endif