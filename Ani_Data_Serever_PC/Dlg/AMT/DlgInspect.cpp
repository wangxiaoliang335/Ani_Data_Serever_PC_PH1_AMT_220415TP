// DlgInspect.cpp : ±¸Çö ÆÄÀÏÀÔ´Ï´Ù.
//

#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "DlgInspect.h"
#include "afxdialogex.h"
#include "DlgAlignStatus.h"
#include "DlgReslutCode.h"
#include "DlgOkGrade.h"
#include "BtnEnhReadability.h"

namespace {

// Header / row labels on DLG_INSPECT_AMT_AFT not bound in DoDataExchange (data cells use members).
static const UINT s_kInspectExtraBtnEnhIds[] = {
	IDC_LBL_DY_TOTAL, IDC_LBL_DY_GOOD, IDC_LBL_DY_BAD, IDC_LBL_DY_ALIGN_FAILED, IDC_LBL_DY,
	IDC_DY_DATA_RESET, IDC_LBL_DY_CONTACT_NG, IDC_LBL_DY_AOI_NG2, IDC_LBL_DY_VIEWING_NG,
	IDC_LBL_DY_TP_NG, IDC_LBL_DY_ALIGN,
	IDC_LBL_DY_STAGE1, IDC_LBL_DY_STAGE2, IDC_LBL_DY_STAGE3, IDC_LBL_DY_STAGE4,
	IDC_LBL_NT, IDC_NT_DATA_RESET,
	IDC_LBL_NT_TOTAL, IDC_LBL_NT_GOOD, IDC_LBL_NT_BAD, IDC_LBL_NT_CONTACT_NG,
	IDC_LBL_NT_ALIGN_FAILED, IDC_LBL_NT_ALIGN, IDC_LBL_NT_AOI_NG2, IDC_LBL_NT_VIEWING_NG,
	IDC_LBL_NT_TP_NG,
	IDC_LBL_NT_STAGE1, IDC_LBL_NT_STAGE2, IDC_LBL_NT_STAGE3, IDC_LBL_NT_STAGE4,
};

static void ApplyInspectDlgExtraBtnStyles(CDlgInspect* pDlg)
{
	for (UINT id : s_kInspectExtraBtnEnhIds)
	{
		HWND h = ::GetDlgItem(pDlg->GetSafeHwnd(), static_cast<int>(id));
		if (!h)
			continue;
		CBtnEnh tmp;
		if (!tmp.Attach(h))
			continue;
		ApplyBtnEnhReadabilityStyle(tmp);
		tmp.Detach();
	}
}

} // namespace

// CDlgInspect ´ëÈ­ »óÀÚÀÔ´Ï´Ù.

IMPLEMENT_DYNAMIC(CDlgInspect, CDialog)

CDlgInspect::CDlgInspect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInspect::IDD, pParent)
{

}

CDlgInspect::~CDlgInspect()
{
}

void CDlgInspect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LBL_DY_OTP_NG, m_btnLabelName[Shift_DY]);
	DDX_Control(pDX, IDC_LBL_NT_OTP_NG, m_btnLabelName[Shift_NT]);

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

	DDX_Control(pDX, IDC_TOTAL_DY_AOI_NG, m_btnAoiTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_AOI_NG, m_btnAoiTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_VIEWING_NG, m_btnViewingTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_VIEWING_NG, m_btnViewingTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_OTP_NG, m_btnOtpTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_OTP_NG, m_btnOtpTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_TP_NG, m_btnTpTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_TP_NG, m_btnTpTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_TOTAL_DY_TRAY_OUT, m_btnTrayOutTotalSum[Shift_DY]);
	DDX_Control(pDX, IDC_TOTAL_NT_TRAY_OUT, m_btnTrayOutTotalSum[Shift_NT]);

	DDX_Control(pDX, IDC_AZONE_DY_TOTAL, m_btnZoneTotal[Shift_DY][AZone]);
	DDX_Control(pDX, IDC_BZONE_DY_TOTAL, m_btnZoneTotal[Shift_DY][BZone]);
	DDX_Control(pDX, IDC_CZONE_DY_TOTAL, m_btnZoneTotal[Shift_DY][CZone]);
	DDX_Control(pDX, IDC_DZONE_DY_TOTAL, m_btnZoneTotal[Shift_DY][DZone]);

	DDX_Control(pDX, IDC_AZONE_NT_TOTAL, m_btnZoneTotal[Shift_NT][AZone]);
	DDX_Control(pDX, IDC_BZONE_NT_TOTAL, m_btnZoneTotal[Shift_NT][BZone]);
	DDX_Control(pDX, IDC_CZONE_NT_TOTAL, m_btnZoneTotal[Shift_NT][CZone]);
	DDX_Control(pDX, IDC_DZONE_NT_TOTAL, m_btnZoneTotal[Shift_NT][DZone]);

	DDX_Control(pDX, IDC_AZONE_DY_GOOD,			m_btnGoodTotal[Shift_DY][AZone]);
	DDX_Control(pDX, IDC_AZONE_DY_BAD,			m_btnNgotal[Shift_DY][AZone]);
	DDX_Control(pDX, IDC_AZONE_DY_CONTACT_NG,	m_btnAutoContactTotal[Shift_DY][AZone]);
	DDX_Control(pDX, IDC_AZONE_DY_ALIGN_NG,		m_btnAlignTotal[Shift_DY][AZone]);
	DDX_Control(pDX, IDC_AZONE_DY_AOI_NG,		m_btnAoiTotal[Shift_DY][AZone]);
	DDX_Control(pDX, IDC_AZONE_DY_VIEWING_NG,	m_btnViewingTotal[Shift_DY][AZone]);
	DDX_Control(pDX, IDC_AZONE_DY_OTP_NG,		m_btnOtpTotal[Shift_DY][AZone]);
	DDX_Control(pDX, IDC_AZONE_DY_TP_NG,		m_btnTpTotal[Shift_DY][AZone]);

	DDX_Control(pDX, IDC_BZONE_DY_GOOD, m_btnGoodTotal[Shift_DY][BZone]);
	DDX_Control(pDX, IDC_BZONE_DY_BAD, m_btnNgotal[Shift_DY][BZone]);
	DDX_Control(pDX, IDC_BZONE_DY_CONTACT_NG, m_btnAutoContactTotal[Shift_DY][BZone]);
	DDX_Control(pDX, IDC_BZONE_DY_ALIGN_NG, m_btnAlignTotal[Shift_DY][BZone]);
	DDX_Control(pDX, IDC_BZONE_DY_AOI_NG, m_btnAoiTotal[Shift_DY][BZone]);
	DDX_Control(pDX, IDC_BZONE_DY_VIEWING_NG, m_btnViewingTotal[Shift_DY][BZone]);
	DDX_Control(pDX, IDC_BZONE_DY_OTP_NG, m_btnOtpTotal[Shift_DY][BZone]);
	DDX_Control(pDX, IDC_BZONE_DY_TP_NG, m_btnTpTotal[Shift_DY][BZone]);

	DDX_Control(pDX, IDC_CZONE_DY_GOOD, m_btnGoodTotal[Shift_DY][CZone]);
	DDX_Control(pDX, IDC_CZONE_DY_BAD, m_btnNgotal[Shift_DY][CZone]);
	DDX_Control(pDX, IDC_CZONE_DY_CONTACT_NG, m_btnAutoContactTotal[Shift_DY][CZone]);
	DDX_Control(pDX, IDC_CZONE_DY_ALIGN_NG, m_btnAlignTotal[Shift_DY][CZone]);
	DDX_Control(pDX, IDC_CZONE_DY_AOI_NG, m_btnAoiTotal[Shift_DY][CZone]);
	DDX_Control(pDX, IDC_CZONE_DY_VIEWING_NG, m_btnViewingTotal[Shift_DY][CZone]);
	DDX_Control(pDX, IDC_CZONE_DY_OTP_NG, m_btnOtpTotal[Shift_DY][CZone]);
	DDX_Control(pDX, IDC_CZONE_DY_TP_NG, m_btnTpTotal[Shift_DY][CZone]);

	DDX_Control(pDX, IDC_DZONE_DY_GOOD, m_btnGoodTotal[Shift_DY][DZone]);
	DDX_Control(pDX, IDC_DZONE_DY_BAD, m_btnNgotal[Shift_DY][DZone]);
	DDX_Control(pDX, IDC_DZONE_DY_CONTACT_NG, m_btnAutoContactTotal[Shift_DY][DZone]);
	DDX_Control(pDX, IDC_DZONE_DY_ALIGN_NG, m_btnAlignTotal[Shift_DY][DZone]);
	DDX_Control(pDX, IDC_DZONE_DY_AOI_NG, m_btnAoiTotal[Shift_DY][DZone]);
	DDX_Control(pDX, IDC_DZONE_DY_VIEWING_NG, m_btnViewingTotal[Shift_DY][DZone]);
	DDX_Control(pDX, IDC_DZONE_DY_OTP_NG, m_btnOtpTotal[Shift_DY][DZone]);
	DDX_Control(pDX, IDC_DZONE_DY_TP_NG, m_btnTpTotal[Shift_DY][DZone]);


	DDX_Control(pDX, IDC_AZONE_NT_GOOD, m_btnGoodTotal[Shift_NT][AZone]);
	DDX_Control(pDX, IDC_AZONE_NT_BAD, m_btnNgotal[Shift_NT][AZone]);
	DDX_Control(pDX, IDC_AZONE_NT_CONTACT_NG, m_btnAutoContactTotal[Shift_NT][AZone]);
	DDX_Control(pDX, IDC_AZONE_NT_ALIGN_NG, m_btnAlignTotal[Shift_NT][AZone]);
	DDX_Control(pDX, IDC_AZONE_NT_AOI_NG, m_btnAoiTotal[Shift_NT][AZone]);
	DDX_Control(pDX, IDC_AZONE_NT_VIEWING_NG, m_btnViewingTotal[Shift_NT][AZone]);
	DDX_Control(pDX, IDC_AZONE_NT_OTP_NG, m_btnOtpTotal[Shift_NT][AZone]);
	DDX_Control(pDX, IDC_AZONE_NT_TP_NG, m_btnTpTotal[Shift_NT][AZone]);

	DDX_Control(pDX, IDC_BZONE_NT_GOOD, m_btnGoodTotal[Shift_NT][BZone]);
	DDX_Control(pDX, IDC_BZONE_NT_BAD, m_btnNgotal[Shift_NT][BZone]);
	DDX_Control(pDX, IDC_BZONE_NT_CONTACT_NG, m_btnAutoContactTotal[Shift_NT][BZone]);
	DDX_Control(pDX, IDC_BZONE_NT_ALIGN_NG, m_btnAlignTotal[Shift_NT][BZone]);
	DDX_Control(pDX, IDC_BZONE_NT_AOI_NG, m_btnAoiTotal[Shift_NT][BZone]);
	DDX_Control(pDX, IDC_BZONE_NT_VIEWING_NG, m_btnViewingTotal[Shift_NT][BZone]);
	DDX_Control(pDX, IDC_BZONE_NT_OTP_NG, m_btnOtpTotal[Shift_NT][BZone]);
	DDX_Control(pDX, IDC_BZONE_NT_TP_NG, m_btnTpTotal[Shift_NT][BZone]);

	DDX_Control(pDX, IDC_CZONE_NT_GOOD, m_btnGoodTotal[Shift_NT][CZone]);
	DDX_Control(pDX, IDC_CZONE_NT_BAD, m_btnNgotal[Shift_NT][CZone]);
	DDX_Control(pDX, IDC_CZONE_NT_CONTACT_NG, m_btnAutoContactTotal[Shift_NT][CZone]);
	DDX_Control(pDX, IDC_CZONE_NT_ALIGN_NG, m_btnAlignTotal[Shift_NT][CZone]);
	DDX_Control(pDX, IDC_CZONE_NT_AOI_NG, m_btnAoiTotal[Shift_NT][CZone]);
	DDX_Control(pDX, IDC_CZONE_NT_VIEWING_NG, m_btnViewingTotal[Shift_NT][CZone]);
	DDX_Control(pDX, IDC_CZONE_NT_OTP_NG, m_btnOtpTotal[Shift_NT][CZone]);
	DDX_Control(pDX, IDC_CZONE_NT_TP_NG, m_btnTpTotal[Shift_NT][CZone]);

	DDX_Control(pDX, IDC_DZONE_NT_GOOD, m_btnGoodTotal[Shift_NT][DZone]);
	DDX_Control(pDX, IDC_DZONE_NT_BAD, m_btnNgotal[Shift_NT][DZone]);
	DDX_Control(pDX, IDC_DZONE_NT_CONTACT_NG, m_btnAutoContactTotal[Shift_NT][DZone]);
	DDX_Control(pDX, IDC_DZONE_NT_ALIGN_NG, m_btnAlignTotal[Shift_NT][DZone]);
	DDX_Control(pDX, IDC_DZONE_NT_AOI_NG, m_btnAoiTotal[Shift_NT][DZone]);
	DDX_Control(pDX, IDC_DZONE_NT_VIEWING_NG, m_btnViewingTotal[Shift_NT][DZone]);
	DDX_Control(pDX, IDC_DZONE_NT_OTP_NG, m_btnOtpTotal[Shift_NT][DZone]);
	DDX_Control(pDX, IDC_DZONE_NT_TP_NG, m_btnTpTotal[Shift_NT][DZone]);
}


BEGIN_MESSAGE_MAP(CDlgInspect, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgInspect ¸Þ½ÃÁö Ã³¸®±âÀÔ´Ï´Ù.

BOOL CDlgInspect::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int ii = 0; ii < eNumShift; ii++)
	{
		if (theApp.m_iMachineType == SetAMT)
			m_btnLabelName[ii].SetCaption(_T("PREGAMMA NG"));
		else if (theApp.m_iMachineType == SetAFT)
			m_btnLabelName[ii].SetCaption(_T("LUMITOP NG"));
	}

	// After captions — DDX BtnEnh subtree + extra labels without DDX
	ApplyBtnEnhReadabilitySubtree(this);
	ApplyInspectDlgExtraBtnStyles(this);
	
	// TODO:  ¿©±â¿¡ Ãß°¡ ÃÊ±âÈ­ ÀÛ¾÷À» Ãß°¡ÇÕ´Ï´Ù.
	SetTimer(TMR_MAIN_INSPECT_INFO, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// ¿¹¿Ü: OCX ¼Ó¼º ÆäÀÌÁö´Â FALSE¸¦ ¹ÝÈ¯ÇØ¾ß ÇÕ´Ï´Ù.
}

BOOL CDlgInspect::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgInspect::OnTimer(UINT_PTR nIDEvent)
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

void CDlgInspect::UpdateDisplay(int nShift)
{
	AOIProductionData SumProduction;
	SumProduction.Reset(nShift);

	theApp.AOIInspctionDataSum(theApp.m_UiShiftProduction, nShift, SumProduction);

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
		m_btnAoiTotalSum[nShift].SetWindowText(sTemp);
		m_btnViewingTotalSum[nShift].SetWindowText(sTemp);
		m_btnOtpTotalSum[nShift].SetWindowText(sTemp);
		m_btnTpTotalSum[nShift].SetWindowText(sTemp);
		m_btnTrayOutTotalSum[nShift].SetWindowText(sTemp);
		sTemp.Format(_T("0"));
		m_btnAlignTotalSum[nShift].SetWindowText(sTemp);
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

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_VisionResult[nShift], SumProduction.m_VisionResult[nShift] / fNumTotal * 100);
		m_btnAoiTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_ViewingResult[nShift], SumProduction.m_ViewingResult[nShift] / fNumTotal * 100);
		m_btnViewingTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_PreGammaResult[nShift], SumProduction.m_PreGammaResult[nShift] / fNumTotal * 100);
		m_btnOtpTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_TpResult[nShift], SumProduction.m_TpResult[nShift] / fNumTotal * 100);
		m_btnTpTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_TrayDataOut[nShift], SumProduction.m_TrayDataOut[nShift] / fNumTotal * 100);
		m_btnTrayOutTotalSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_AlignResult[nShift]);
		m_btnAlignTotalSum[nShift].SetWindowText(sTemp);
	}

	for (int ii = 0; ii < MaxZone; ii++)
	{
		
		sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_InspectionTotal[nShift]);
		m_btnZoneTotal[nShift][ii].SetWindowText(sTemp);

		if (theApp.m_UiShiftProduction[ii].m_InspectionTotal[nShift] == 0)
		{
			sTemp.Format(_T("0 (0.0%%)"));
			m_btnGoodTotal[nShift][ii].SetWindowText(sTemp);
			m_btnNgotal[nShift][ii].SetWindowText(sTemp);
			m_btnAutoContactTotal[nShift][ii].SetWindowText(sTemp);
			m_btnAoiTotal[nShift][ii].SetWindowText(sTemp);
			m_btnViewingTotal[nShift][ii].SetWindowText(sTemp);
			m_btnOtpTotal[nShift][ii].SetWindowText(sTemp);
			m_btnTpTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("0"));
			m_btnAlignTotal[nShift][ii].SetWindowText(sTemp);
		}
		else
		{
			fNumTotal = theApp.m_UiShiftProduction[ii].m_InspectionTotal[nShift];

			sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_InspectionTotal[nShift]);
			m_btnZoneTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_GoodResult[nShift], theApp.m_UiShiftProduction[ii].m_GoodResult[nShift] / fNumTotal * 100);
			m_btnGoodTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_BadResult[nShift], theApp.m_UiShiftProduction[ii].m_BadResult[nShift] / fNumTotal * 100);
			m_btnNgotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_ContactResult[nShift], theApp.m_UiShiftProduction[ii].m_ContactResult[nShift] / fNumTotal * 100);
			m_btnAutoContactTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_VisionResult[nShift], theApp.m_UiShiftProduction[ii].m_VisionResult[nShift] / fNumTotal * 100);
			m_btnAoiTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_ViewingResult[nShift], theApp.m_UiShiftProduction[ii].m_ViewingResult[nShift] / fNumTotal * 100);
			m_btnViewingTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_PreGammaResult[nShift], theApp.m_UiShiftProduction[ii].m_PreGammaResult[nShift] / fNumTotal * 100);
			m_btnOtpTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_TpResult[nShift], theApp.m_UiShiftProduction[ii].m_TpResult[nShift] / fNumTotal * 100);
			m_btnTpTotal[nShift][ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_AlignResult[nShift]);
			m_btnAlignTotal[nShift][ii].SetWindowText(sTemp);
		}
	}
}

BEGIN_EVENTSINK_MAP(CDlgInspect, CDialog)
	ON_EVENT(CDlgInspect, IDC_LBL_DY_ALIGN, DISPID_CLICK, CDlgInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_NT_ALIGN, DISPID_CLICK, CDlgInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_DY_CONTACT_NG, DISPID_CLICK, CDlgInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_NT_CONTACT_NG, DISPID_CLICK, CDlgInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_DY_TP_NG, DISPID_CLICK, CDlgInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_NT_TP_NG, DISPID_CLICK, CDlgInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_DY_OTP_NG, DISPID_CLICK, CDlgInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_NT_OTP_NG, DISPID_CLICK, CDlgInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_DY_DATA_RESET, DISPID_CLICK, CDlgInspect::OnClickDataReset, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_NT_DATA_RESET, DISPID_CLICK, CDlgInspect::OnClickDataReset, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_DY_AOI_NG2, DISPID_CLICK, CDlgInspect::ClickInspectResultCode, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_DY_VIEWING_NG, DISPID_CLICK, CDlgInspect::ClickInspectResultCode, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_NT_AOI_NG2, DISPID_CLICK, CDlgInspect::ClickInspectResultCode, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_NT_VIEWING_NG, DISPID_CLICK, CDlgInspect::ClickInspectResultCode, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_DY_GOOD, DISPID_CLICK, CDlgInspect::ClicGoodGrade, VTS_NONE)
	ON_EVENT(CDlgInspect, IDC_LBL_NT_GOOD, DISPID_CLICK, CDlgInspect::ClicGoodGrade, VTS_NONE)
END_EVENTSINK_MAP()

void CDlgInspect::ClickChResult()
{
	int iShiftDy, iCommand;
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_LBL_DY_ALIGN:
		iShiftDy = Shift_DY; iCommand = Align_View;
		break;
	case IDC_LBL_NT_ALIGN:
		iShiftDy = Shift_NT; iCommand = Align_View;
		break;
	case IDC_LBL_DY_CONTACT_NG:
		iShiftDy = Shift_DY; iCommand = Contact_View;
		break;
	case IDC_LBL_NT_CONTACT_NG:
		iShiftDy = Shift_NT; iCommand = Contact_View;
		break;
	case IDC_LBL_DY_TP_NG:
		iShiftDy = Shift_DY; iCommand = TP_View;
		break;
	case IDC_LBL_NT_TP_NG:
		iShiftDy = Shift_NT; iCommand = TP_View;
		break;
	case IDC_LBL_DY_OTP_NG:
		iShiftDy = Shift_DY; iCommand = PreGamma_View;
		break;
	case IDC_LBL_NT_OTP_NG:
		iShiftDy = Shift_NT; iCommand = PreGamma_View;
		break;
	default:
		iShiftDy = 0; iCommand = 0;
		break;
	}

	CDlgAlignStatus dlg(iShiftDy, iCommand);
	dlg.DoModal();
}

void CDlgInspect::ClicGoodGrade()
{
	int iShift = 0;

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_LBL_DY_GOOD: iShift = Shift_DY; break;
	case IDC_LBL_NT_GOOD: iShift = Shift_NT; break;
	}

	CDlgOkGrade dlg(iShift);
	dlg.DoModal();
}

void CDlgInspect::ClickInspectResultCode()
{
	int iShift = 0;
	CString strInspName = _T("");

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_LBL_DY_AOI_NG2: 
	case IDC_LBL_DY_VIEWING_NG: iShift = Shift_DY; break;
	case IDC_LBL_NT_AOI_NG2:
	case IDC_LBL_NT_VIEWING_NG: iShift = Shift_NT; break;
	}

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_LBL_DY_AOI_NG2:
	case IDC_LBL_NT_AOI_NG2:	strInspName = _T("AOI");     break;
	case IDC_LBL_DY_VIEWING_NG:
	case IDC_LBL_NT_VIEWING_NG: strInspName = _T("Viewing"); break;
	}

	CDlgReslutCode dlg(strInspName, iShift);
	dlg.DoModal();
}

void CDlgInspect::OnClickDataReset()
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
			for (int ii = 0; ii < MaxZone; ii++)
				theApp.m_UiShiftProduction[ii].Reset(iShift);

			theApp.AOIInspectionDataSave(iShift);
			theApp.AlignDataSave(iShift);
			theApp.ContactDataSave(iShift);
			theApp.TpDataSave(iShift);
			theApp.PreGammaDataSave(iShift);
		}
	}
}
#endif