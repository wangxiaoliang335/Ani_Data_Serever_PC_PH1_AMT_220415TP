// DlgInspect.cpp : ±¸Çö ÆÄÀÏÀÔ´Ï´Ù.
//

#include "stdafx.h"
#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "DlgUnloaderInspect.h"
#include "afxdialogex.h"
#include "DlgAlignStatus.h"
#include "DlgReslutCode.h"
#include "DlgOkGrade.h"


// CDlgUnloaderInspect ´ëÈ­ »óÀÚÀÔ´Ï´Ù.

IMPLEMENT_DYNAMIC(CDlgUnloaderInspect, CDialog)

CDlgUnloaderInspect::CDlgUnloaderInspect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUnloaderInspect::IDD, pParent)
{

}

CDlgUnloaderInspect::~CDlgUnloaderInspect()
{
}

void CDlgUnloaderInspect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TOTAL_DY, m_btnTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT, m_btnTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_GOOD, m_btnGoodTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_GOOD, m_btnGoodTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_BAD, m_btnNgTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_BAD, m_btnNgTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_CONTACT_NG, m_btnAutoContactTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_CONTACT_NG, m_btnAutoContactTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_ALIGN_NG, m_btnAlignTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_ALIGN_NG, m_btnAlignTotalSum[Shift_NT]);
	
	DDX_Control(pDX, IDC_TOTAL_DY_OPV_NG, m_btnManualOpvTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_OPV_NG, m_btnManualOpvTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_TOUCH_NG, m_btnManualTouchTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_TOUCH_NG, m_btnManualTouchTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_GAMMA_NG, m_btnManualGammaTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_GAMMA_NG, m_btnManualGammaTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_BUFFER_TRAY, m_btnBufferTrayTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_BUFFER_TRAY, m_btnBufferTrayTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_MANUAL_CONTACT_NG, m_btnManualContactTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_MANUAL_CONTACT_NG, m_btnManualContactTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_SAMPLE_NUM, m_btnSampleTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_SAMPLE_NUM, m_btnSampleTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_DY_TOTAL_1, m_btnChTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_DY_TOTAL_2, m_btnChTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_NT_TOTAL_1, m_btnChTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_NT_TOTAL_2, m_btnChTotal[Shift_NT][CH_2]);

	DDX_Control(pDX, IDC_DY_GOOD_1, m_btnGoodTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_DY_GOOD_2, m_btnGoodTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_NT_GOOD_1, m_btnGoodTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_NT_GOOD_2, m_btnGoodTotal[Shift_NT][CH_2]);

	DDX_Control(pDX, IDC_DY_BAD_1, m_btnNgTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_DY_BAD_2, m_btnNgTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_NT_BAD_1, m_btnNgTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_NT_BAD_2, m_btnNgTotal[Shift_NT][CH_2]);

	DDX_Control(pDX, IDC_DY_CONTACT_NG_1, m_btnAutoContactTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_DY_CONTACT_NG_2, m_btnAutoContactTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_NT_CONTACT_NG_1, m_btnAutoContactTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_NT_CONTACT_NG_2, m_btnAutoContactTotal[Shift_NT][CH_2]);

	DDX_Control(pDX, IDC_DY_ALIGN_NG_1, m_btnAlignTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_DY_ALIGN_NG_2, m_btnAlignTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_NT_ALIGN_NG_1, m_btnAlignTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_NT_ALIGN_NG_2, m_btnAlignTotal[Shift_NT][CH_2]);

	DDX_Control(pDX, IDC_DY_OPV_NG_1, m_btnManualOpvTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_DY_OPV_NG_2, m_btnManualOpvTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_NT_OPV_NG_1, m_btnManualOpvTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_NT_OPV_NG_2, m_btnManualOpvTotal[Shift_NT][CH_2]);

	DDX_Control(pDX, IDC_DY_TOUCH_NG_1, m_btnManualTouchTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_DY_TOUCH_NG_2, m_btnManualTouchTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_NT_TOUCH_NG_1, m_btnManualTouchTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_NT_TOUCH_NG_2, m_btnManualTouchTotal[Shift_NT][CH_2]);

	DDX_Control(pDX, IDC_DY_GAMMA_NG_1, m_btnManualGammaTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_DY_GAMMA_NG_2, m_btnManualGammaTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_NT_GAMMA_NG_1, m_btnManualGammaTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_NT_GAMMA_NG_2, m_btnManualGammaTotal[Shift_NT][CH_2]);

	DDX_Control(pDX, IDC_TOTAL_DY_BUFFER_TRAY_1, m_btnBufferTrayTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_TOTAL_DY_BUFFER_TRAY_2, m_btnBufferTrayTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_TOTAL_NT_BUFFER_TRAY_1, m_btnBufferTrayTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_TOTAL_NT_BUFFER_TRAY_2, m_btnBufferTrayTotal[Shift_NT][CH_2]);

	DDX_Control(pDX, IDC_DY_MANUAL_CONTACT_NG_1, m_btnManualContactTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_DY_MANUAL_CONTACT_NG_2, m_btnManualContactTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_NT_MANUAL_CONTACT_NG_1, m_btnManualContactTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_NT_MANUAL_CONTACT_NG_2, m_btnManualContactTotal[Shift_NT][CH_2]);

	DDX_Control(pDX, IDC_TOTAL_DY_SAMPLE_NUM_1, m_btnSampleTotal[Shift_DY][CH_1]);
	DDX_Control(pDX, IDC_TOTAL_DY_SAMPLE_NUM_2, m_btnSampleTotal[Shift_DY][CH_2]);
	DDX_Control(pDX, IDC_TOTAL_NT_SAMPLE_NUM_1, m_btnSampleTotal[Shift_NT][CH_1]);
	DDX_Control(pDX, IDC_TOTAL_NT_SAMPLE_NUM_2, m_btnSampleTotal[Shift_NT][CH_2]);
}


BEGIN_MESSAGE_MAP(CDlgUnloaderInspect, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgUnloaderInspect ¸Þ½ÃÁö Ã³¸®±âÀÔ´Ï´Ù.

BOOL CDlgUnloaderInspect::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO:  ¿©±â¿¡ Ãß°¡ ÃÊ±âÈ­ ÀÛ¾÷À» Ãß°¡ÇÕ´Ï´Ù.
	SetTimer(TMR_MAIN_INSPECT_INFO, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// ¿¹¿Ü: OCX ¼Ó¼º ÆäÀÌÁö´Â FALSE¸¦ ¹ÝÈ¯ÇØ¾ß ÇÕ´Ï´Ù.
}

BOOL CDlgUnloaderInspect::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgUnloaderInspect::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ ¹×/¶Ç´Â ±âº»°ªÀ» È£ÃâÇÕ´Ï´Ù.
	if (nIDEvent == TMR_MAIN_INSPECT_INFO)
	{
		UpdateDisplay(Shift_DY);
		UpdateDisplay(Shift_NT);
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgUnloaderInspect::UpdateDisplay(int nShift)
{
	ULDProductionData SumProduction;
	SumProduction.Reset(nShift);

	theApp.ULDInspctionDataSum(theApp.m_ULDUiShiftProduction, nShift, SumProduction);

	CString sTemp;
	sTemp.Format(_T("%d"), SumProduction.m_InspectionTotal[nShift]);
	m_btnTotalSum[nShift].SetWindowText(sTemp);

	float fNumTotal = 0.;
	if (SumProduction.m_InspectionTotal[nShift] == 0)
	{
		sTemp.Format(_T("0 (0.0%%)"));
		m_btnGoodTotalSum[nShift].SetWindowText(sTemp);
		m_btnNgTotalSum[nShift].SetWindowText(sTemp);
		m_btnAutoContactTotalSum[nShift].SetWindowText(sTemp);
		m_btnManualOpvTotalSum[nShift].SetWindowText(sTemp);
		m_btnManualTouchTotalSum[nShift].SetWindowText(sTemp);
		m_btnManualGammaTotalSum[nShift].SetWindowText(sTemp);
		m_btnManualContactTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("0"));
		m_btnAlignTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_BufferTrayResult[nShift]);
		m_btnBufferTrayTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_SampleResult[nShift]);
		m_btnSampleTotalSum[nShift].SetWindowText(sTemp);
	}
	else
	{
		fNumTotal = SumProduction.m_InspectionTotal[nShift];
		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_GoodResult[nShift], SumProduction.m_GoodResult[nShift] / fNumTotal * 100);
		m_btnGoodTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_BadResult[nShift], SumProduction.m_BadResult[nShift] / fNumTotal * 100);
		m_btnNgTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_ContactResult[nShift], SumProduction.m_ContactResult[nShift] / fNumTotal * 100);
		m_btnAutoContactTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_OpvResult[nShift], SumProduction.m_OpvResult[nShift] / fNumTotal * 100);
		m_btnManualOpvTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_TouchResult[nShift], SumProduction.m_TouchResult[nShift] / fNumTotal * 100);
		m_btnManualTouchTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_GammaResult[nShift], SumProduction.m_GammaResult[nShift] / fNumTotal * 100);
		m_btnManualGammaTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_BufferTrayResult[nShift]);
		m_btnBufferTrayTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_SampleResult[nShift]);
		m_btnSampleTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_ManualContactResult[nShift], SumProduction.m_ManualContactResult[nShift] / fNumTotal * 100);
		m_btnManualContactTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_AlignResult[nShift]);
		m_btnAlignTotalSum[nShift].SetWindowText(sTemp);
	}

	for (int ii = 0; ii < ChMaxCount; ii++)
	{
		sTemp.Format(_T("%d"), theApp.m_ULDUiShiftProduction[ii].m_InspectionTotal[nShift]);
		m_btnChTotal[nShift][ii].SetWindowText(sTemp);

		if (theApp.m_ULDUiShiftProduction[ii].m_InspectionTotal[nShift] == 0)
		{
			sTemp.Format(_T("0 (0.0%%)"));

			m_btnGoodTotal[nShift][ii].SetWindowText(sTemp);
			m_btnNgTotal[nShift][ii].SetWindowText(sTemp);
			m_btnAutoContactTotal[nShift][ii].SetWindowText(sTemp);
			m_btnManualOpvTotal[nShift][ii].SetWindowText(sTemp);
			m_btnManualTouchTotal[nShift][ii].SetWindowText(sTemp);
			m_btnManualGammaTotal[nShift][ii].SetWindowText(sTemp);
			m_btnManualContactTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("0"));
			m_btnAlignTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_ULDUiShiftProduction[ii].m_BufferTrayResult[nShift]);
			m_btnBufferTrayTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_ULDUiShiftProduction[ii].m_SampleResult[nShift]);
			m_btnSampleTotal[nShift][ii].SetWindowText(sTemp);

		}
		else
		{
			fNumTotal = theApp.m_ULDUiShiftProduction[ii].m_InspectionTotal[nShift];

			sTemp.Format(_T("%d"), theApp.m_ULDUiShiftProduction[ii].m_InspectionTotal[nShift]);
			m_btnChTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShiftProduction[ii].m_GoodResult[nShift], theApp.m_ULDUiShiftProduction[ii].m_GoodResult[nShift] / fNumTotal * 100);
			m_btnGoodTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShiftProduction[ii].m_BadResult[nShift], theApp.m_ULDUiShiftProduction[ii].m_BadResult[nShift] / fNumTotal * 100);
			m_btnNgTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShiftProduction[ii].m_ContactResult[nShift], theApp.m_ULDUiShiftProduction[ii].m_ContactResult[nShift] / fNumTotal * 100);
			m_btnAutoContactTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShiftProduction[ii].m_OpvResult[nShift], theApp.m_ULDUiShiftProduction[ii].m_OpvResult[nShift] / fNumTotal * 100);
			m_btnManualOpvTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShiftProduction[ii].m_TouchResult[nShift], theApp.m_ULDUiShiftProduction[ii].m_TouchResult[nShift] / fNumTotal * 100);
			m_btnManualTouchTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShiftProduction[ii].m_GammaResult[nShift], theApp.m_ULDUiShiftProduction[ii].m_GammaResult[nShift] / fNumTotal * 100);
			m_btnManualGammaTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_ULDUiShiftProduction[ii].m_BufferTrayResult[nShift]);
			m_btnBufferTrayTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_ULDUiShiftProduction[ii].m_SampleResult[nShift]);
			m_btnSampleTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_ULDUiShiftProduction[ii].m_ManualContactResult[nShift], theApp.m_ULDUiShiftProduction[ii].m_ManualContactResult[nShift] / fNumTotal * 100);
			m_btnManualContactTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_ULDUiShiftProduction[ii].m_AlignResult[nShift]);
			m_btnAlignTotal[nShift][ii].SetWindowText(sTemp);
		}
	}
}

BEGIN_EVENTSINK_MAP(CDlgUnloaderInspect, CDialog)
ON_EVENT(CDlgUnloaderInspect, IDC_DY_DATA_RESET, DISPID_CLICK, CDlgUnloaderInspect::OnClickDataReset, VTS_NONE)
ON_EVENT(CDlgUnloaderInspect, IDC_NT_DATA_RESET, DISPID_CLICK, CDlgUnloaderInspect::OnClickDataReset, VTS_NONE)
END_EVENTSINK_MAP()

void CDlgUnloaderInspect::OnClickDataReset()
{
	int iShift = 0;
	CString strShift = _T("");

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_DY_DATA_RESET: iShift = Shift_DY; break;
	case IDC_NT_DATA_RESET: iShift = Shift_NT; break;
	}

	strShift = iShift == Shift_DY ? _T("DY") : _T("NT");

	if (theApp.m_iUserClass != USER_MAKER)
	{
		theApp.getMsgBox(MS_OK, _T("관리자만 이용 가능합니다."), _T("Maker is USE"), _T("Maker is USE"));
		return;
	}
	else
	{
		if (theApp.YesNoMsgBox(MS_YESNO, _T("Do You Want To Data Clear?"), _T("Do You Want To Data Clear?"), _T("Do You Want To Data Clear?")) == MSG_OK)
		{
			theApp.m_pTraceLog->LOG_INFO(_T("************************%s Data Reset************************"), strShift);
			for (int ii = 0; ii < ChMaxCount; ii++)
				theApp.m_ULDUiShiftProduction[ii].Reset(iShift);

			theApp.ULDInspectionDataSave(iShift);
			theApp.ULDAlignDataSave(iShift);
		}
	}
}
#endif