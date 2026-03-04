
#include "stdafx.h"

#if _SYSTEM_GAMMA_
#include "Ani_Data_Serever_PC.h"
#include "DlgGammaInspect.h"
#include "afxdialogex.h"
#include "DlgGammaAlignStatus.h"

IMPLEMENT_DYNAMIC(CDlgGammaInspect, CDialog)

CDlgGammaInspect::CDlgGammaInspect(CWnd* pParent /*=NULL*/)
: CDialog(CDlgGammaInspect::IDD, pParent)
{

}

CDlgGammaInspect::~CDlgGammaInspect()
{
}

void CDlgGammaInspect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TOTAL, m_btnTotalSum);
	DDX_Control(pDX, IDC_TOTAL_GOOD, m_btnGoodTotalSum);
	DDX_Control(pDX, IDC_TOTAL_BAD, m_btnNgTotalSum);
	DDX_Control(pDX, IDC_TOTAL_CONTACT_NG, m_btnAutoContactTotalSum);
	DDX_Control(pDX, IDC_TOTAL_M_CONTACT_NG, m_btnManualContactTotalSum);
	DDX_Control(pDX, IDC_TOTAL_MTP_NG, m_btnMtpTotalSum);
	DDX_Control(pDX, IDC_TOTAL_ALIGN_NG, m_btnAlignTotalSum);
	DDX_Control(pDX, IDC_LBL_DY, m_btnLabelName);

	DDX_Control(pDX, IDC_STAGE1_TOTAL, m_btnStageTotal[GammaStage_1]);
	DDX_Control(pDX, IDC_STAGE2_TOTAL, m_btnStageTotal[GammaStage_2]);
	DDX_Control(pDX, IDC_STAGE3_TOTAL, m_btnStageTotal[GammaStage_3]);
	DDX_Control(pDX, IDC_STAGE4_TOTAL, m_btnStageTotal[GammaStage_4]);
	DDX_Control(pDX, IDC_STAGE5_TOTAL, m_btnStageTotal[GammaStage_5]);
	DDX_Control(pDX, IDC_STAGE6_TOTAL, m_btnStageTotal[GammaStage_6]);
	DDX_Control(pDX, IDC_STAGE7_TOTAL, m_btnStageTotal[GammaStage_7]);
	DDX_Control(pDX, IDC_STAGE8_TOTAL, m_btnStageTotal[GammaStage_8]);
	DDX_Control(pDX, IDC_STAGE9_TOTAL, m_btnStageTotal[GammaStage_9]);
	DDX_Control(pDX, IDC_STAGE10_TOTAL, m_btnStageTotal[GammaStage_10]);
	DDX_Control(pDX, IDC_STAGE11_TOTAL, m_btnStageTotal[GammaStage_11]);
	DDX_Control(pDX, IDC_STAGE12_TOTAL, m_btnStageTotal[GammaStage_12]);

	DDX_Control(pDX, IDC_STAGE1_GOOD, m_btnGoodTotal[GammaStage_1]);
	DDX_Control(pDX, IDC_STAGE1_BAD, m_btnNgotal[GammaStage_1]);
	DDX_Control(pDX, IDC_STAGE1_CONTACT_NG, m_btnAutoContactTotal[GammaStage_1]);
	DDX_Control(pDX, IDC_STAGE1_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_1]);
	DDX_Control(pDX, IDC_STAGE1_MPT_NG, m_btnMtpTotal[GammaStage_1]);
	DDX_Control(pDX, IDC_STAGE1_ALIGN_NG, m_btnAlignTotal[GammaStage_1]);

	DDX_Control(pDX, IDC_STAGE2_GOOD, m_btnGoodTotal[GammaStage_2]);
	DDX_Control(pDX, IDC_STAGE2_BAD, m_btnNgotal[GammaStage_2]);
	DDX_Control(pDX, IDC_STAGE2_CONTACT_NG, m_btnAutoContactTotal[GammaStage_2]);
	DDX_Control(pDX, IDC_STAGE2_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_2]);
	DDX_Control(pDX, IDC_STAGE2_MPT_NG, m_btnMtpTotal[GammaStage_2]);
	DDX_Control(pDX, IDC_STAGE2_ALIGN_NG, m_btnAlignTotal[GammaStage_2]);

	DDX_Control(pDX, IDC_STAGE3_GOOD, m_btnGoodTotal[GammaStage_3]);
	DDX_Control(pDX, IDC_STAGE3_BAD, m_btnNgotal[GammaStage_3]);
	DDX_Control(pDX, IDC_STAGE3_CONTACT_NG, m_btnAutoContactTotal[GammaStage_3]);
	DDX_Control(pDX, IDC_STAGE3_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_3]);
	DDX_Control(pDX, IDC_STAGE3_MPT_NG, m_btnMtpTotal[GammaStage_3]);
	DDX_Control(pDX, IDC_STAGE3_ALIGN_NG, m_btnAlignTotal[GammaStage_3]);

	DDX_Control(pDX, IDC_STAGE4_GOOD, m_btnGoodTotal[GammaStage_4]);
	DDX_Control(pDX, IDC_STAGE4_BAD, m_btnNgotal[GammaStage_4]);
	DDX_Control(pDX, IDC_STAGE4_CONTACT_NG, m_btnAutoContactTotal[GammaStage_4]);
	DDX_Control(pDX, IDC_STAGE4_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_4]);
	DDX_Control(pDX, IDC_STAGE4_MPT_NG, m_btnMtpTotal[GammaStage_4]);
	DDX_Control(pDX, IDC_STAGE4_ALIGN_NG, m_btnAlignTotal[GammaStage_4]);

	DDX_Control(pDX, IDC_STAGE5_GOOD, m_btnGoodTotal[GammaStage_5]);
	DDX_Control(pDX, IDC_STAGE5_BAD, m_btnNgotal[GammaStage_5]);
	DDX_Control(pDX, IDC_STAGE5_CONTACT_NG, m_btnAutoContactTotal[GammaStage_5]);
	DDX_Control(pDX, IDC_STAGE5_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_5]);
	DDX_Control(pDX, IDC_STAGE5_MPT_NG, m_btnMtpTotal[GammaStage_5]);
	DDX_Control(pDX, IDC_STAGE5_ALIGN_NG, m_btnAlignTotal[GammaStage_5]);

	DDX_Control(pDX, IDC_STAGE6_GOOD, m_btnGoodTotal[GammaStage_6]);
	DDX_Control(pDX, IDC_STAGE6_BAD, m_btnNgotal[GammaStage_6]);
	DDX_Control(pDX, IDC_STAGE6_CONTACT_NG, m_btnAutoContactTotal[GammaStage_6]);
	DDX_Control(pDX, IDC_STAGE6_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_6]);
	DDX_Control(pDX, IDC_STAGE6_MPT_NG, m_btnMtpTotal[GammaStage_6]);
	DDX_Control(pDX, IDC_STAGE6_ALIGN_NG, m_btnAlignTotal[GammaStage_6]);

	DDX_Control(pDX, IDC_STAGE7_GOOD, m_btnGoodTotal[GammaStage_7]);
	DDX_Control(pDX, IDC_STAGE7_BAD, m_btnNgotal[GammaStage_7]);
	DDX_Control(pDX, IDC_STAGE7_CONTACT_NG, m_btnAutoContactTotal[GammaStage_7]);
	DDX_Control(pDX, IDC_STAGE7_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_7]);
	DDX_Control(pDX, IDC_STAGE7_MPT_NG, m_btnMtpTotal[GammaStage_7]);
	DDX_Control(pDX, IDC_STAGE7_ALIGN_NG, m_btnAlignTotal[GammaStage_7]);

	DDX_Control(pDX, IDC_STAGE8_GOOD, m_btnGoodTotal[GammaStage_8]);
	DDX_Control(pDX, IDC_STAGE8_BAD, m_btnNgotal[GammaStage_8]);
	DDX_Control(pDX, IDC_STAGE8_CONTACT_NG, m_btnAutoContactTotal[GammaStage_8]);
	DDX_Control(pDX, IDC_STAGE8_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_8]);
	DDX_Control(pDX, IDC_STAGE8_MPT_NG, m_btnMtpTotal[GammaStage_8]);
	DDX_Control(pDX, IDC_STAGE8_ALIGN_NG, m_btnAlignTotal[GammaStage_8]);

	DDX_Control(pDX, IDC_STAGE9_GOOD, m_btnGoodTotal[GammaStage_9]);
	DDX_Control(pDX, IDC_STAGE9_BAD, m_btnNgotal[GammaStage_9]);
	DDX_Control(pDX, IDC_STAGE9_CONTACT_NG, m_btnAutoContactTotal[GammaStage_9]);
	DDX_Control(pDX, IDC_STAGE9_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_9]);
	DDX_Control(pDX, IDC_STAGE9_MPT_NG, m_btnMtpTotal[GammaStage_9]);
	DDX_Control(pDX, IDC_STAGE9_ALIGN_NG, m_btnAlignTotal[GammaStage_9]);

	DDX_Control(pDX, IDC_STAGE10_GOOD, m_btnGoodTotal[GammaStage_10]);
	DDX_Control(pDX, IDC_STAGE10_BAD, m_btnNgotal[GammaStage_10]);
	DDX_Control(pDX, IDC_STAGE10_CONTACT_NG, m_btnAutoContactTotal[GammaStage_10]);
	DDX_Control(pDX, IDC_STAGE10_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_10]);
	DDX_Control(pDX, IDC_STAGE10_MPT_NG, m_btnMtpTotal[GammaStage_10]);
	DDX_Control(pDX, IDC_STAGE10_ALIGN_NG, m_btnAlignTotal[GammaStage_10]);

	DDX_Control(pDX, IDC_STAGE11_GOOD, m_btnGoodTotal[GammaStage_11]);
	DDX_Control(pDX, IDC_STAGE11_BAD, m_btnNgotal[GammaStage_11]);
	DDX_Control(pDX, IDC_STAGE11_CONTACT_NG, m_btnAutoContactTotal[GammaStage_11]);
	DDX_Control(pDX, IDC_STAGE11_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_11]);
	DDX_Control(pDX, IDC_STAGE11_MPT_NG, m_btnMtpTotal[GammaStage_11]);
	DDX_Control(pDX, IDC_STAGE11_ALIGN_NG, m_btnAlignTotal[GammaStage_11]);

	DDX_Control(pDX, IDC_STAGE12_GOOD, m_btnGoodTotal[GammaStage_12]);
	DDX_Control(pDX, IDC_STAGE12_BAD, m_btnNgotal[GammaStage_12]);
	DDX_Control(pDX, IDC_STAGE12_CONTACT_NG, m_btnAutoContactTotal[GammaStage_12]);
	DDX_Control(pDX, IDC_STAGE12_M_CONTACT_NG, m_btnManualContactTotal[GammaStage_12]);
	DDX_Control(pDX, IDC_STAGE12_MPT_NG, m_btnMtpTotal[GammaStage_12]);
	DDX_Control(pDX, IDC_STAGE12_ALIGN_NG, m_btnAlignTotal[GammaStage_12]);
}

BEGIN_MESSAGE_MAP(CDlgGammaInspect, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgGammaInspect, CDialog)
	ON_EVENT(CDlgGammaInspect, IDC_LBL_ALIGN_NG, DISPID_CLICK, CDlgGammaInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgGammaInspect, IDC_LBL_CONTACT_NG, DISPID_CLICK, CDlgGammaInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgGammaInspect, IDC_LBL_M_CONTACT_NG, DISPID_CLICK, CDlgGammaInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgGammaInspect, IDC_LBL_MTP_NG, DISPID_CLICK, CDlgGammaInspect::ClickChResult, VTS_NONE)
	ON_EVENT(CDlgGammaInspect, IDC_DATA_RESET, DISPID_CLICK, CDlgGammaInspect::OnClickDataReset, VTS_NONE)
	ON_EVENT(CDlgGammaInspect, IDB_BTN_DY, DISPID_CLICK, CDlgGammaInspect::ShiftData, VTS_NONE)
	ON_EVENT(CDlgGammaInspect, IDB_BTN_NT, DISPID_CLICK, CDlgGammaInspect::ShiftData, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CDlgGammaInspect::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iShift = Shift_DY;
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDB_BTN_DY);
	pBtnEnh->SetValue(TRUE);

	SetTimer(TMR_MAIN_INSPECT_INFO, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CDlgGammaInspect::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgGammaInspect::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	if (nIDEvent == TMR_MAIN_INSPECT_INFO)
		UpdateDisplay(m_iShift);

	CDialog::OnTimer(nIDEvent);
}

void CDlgGammaInspect::ShiftData()
{
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDB_BTN_DY: m_iShift = Shift_DY; m_btnLabelName.SetWindowText(_T("白班数据")); break;
	case IDB_BTN_NT: m_iShift = Shift_NT; m_btnLabelName.SetWindowText(_T("夜班数据")); break;
	}

	UpdateDisplay(m_iShift);
}

void CDlgGammaInspect::UpdateDisplay(int nShift)
{
	ProductionData SumProduction;

	SumProduction.Reset(nShift);
	
	theApp.InspctionDataSum(theApp.m_UiShiftProduction, nShift, SumProduction);

	CString sTemp;
	sTemp.Format(_T("%d"), SumProduction.m_InspectionTotal[nShift]);
	m_btnTotalSum.SetWindowText(sTemp);

	float fNumTotal = 0.;
	if (SumProduction.m_InspectionTotal[nShift] == 0)
	{
		sTemp.Format(_T("0 (0.0%%)"));
		m_btnGoodTotalSum.SetWindowText(sTemp);
		m_btnNgTotalSum.SetWindowText(sTemp);
		m_btnAutoContactTotalSum.SetWindowText(sTemp);
		m_btnManualContactTotalSum.SetWindowText(sTemp);
		m_btnMtpTotalSum.SetWindowText(sTemp);

		sTemp.Format(_T("0"));
		m_btnAlignTotalSum.SetWindowText(sTemp);
	}
	else
	{
		fNumTotal = SumProduction.m_InspectionTotal[nShift];
		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_GoodResult[nShift], SumProduction.m_GoodResult[nShift] / fNumTotal * 100);
		m_btnGoodTotalSum.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_BadResult[nShift], SumProduction.m_BadResult[nShift] / fNumTotal * 100);
		m_btnNgTotalSum.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_ContactResult[nShift], SumProduction.m_ContactResult[nShift] / fNumTotal * 100);
		m_btnAutoContactTotalSum.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_ManualContactResult[nShift], SumProduction.m_ManualContactResult[nShift] / fNumTotal * 100);
		m_btnManualContactTotalSum.SetWindowText(sTemp);

		sTemp.Format(_T("%d (%.1f%%)"), SumProduction.m_MtpResult[nShift], SumProduction.m_MtpResult[nShift] / fNumTotal * 100);
		m_btnMtpTotalSum.SetWindowText(sTemp);

		sTemp.Format(_T("%d"), SumProduction.m_AlignResult[nShift]);
		m_btnAlignTotalSum.SetWindowText(sTemp);
	}

	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_InspectionTotal[nShift]);
		m_btnStageTotal[ii].SetWindowText(sTemp);

		if (theApp.m_UiShiftProduction[ii].m_InspectionTotal[nShift] == 0)
		{
			sTemp.Format(_T("0 (0.0%%)"));
			m_btnGoodTotal[ii].SetWindowText(sTemp);
			m_btnNgotal[ii].SetWindowText(sTemp);
			m_btnAutoContactTotal[ii].SetWindowText(sTemp);
			m_btnManualContactTotal[ii].SetWindowText(sTemp);
			m_btnMtpTotal[ii].SetWindowText(sTemp);

			sTemp.Format(_T("0"));
			m_btnAlignTotal[ii].SetWindowText(sTemp);
		}
		else
		{
			fNumTotal = theApp.m_UiShiftProduction[ii].m_InspectionTotal[nShift];

			sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_InspectionTotal[nShift]);
			m_btnStageTotal[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_GoodResult[nShift], theApp.m_UiShiftProduction[ii].m_GoodResult[nShift] / fNumTotal * 100);
			m_btnGoodTotal[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_BadResult[nShift], theApp.m_UiShiftProduction[ii].m_BadResult[nShift] / fNumTotal * 100);
			m_btnNgotal[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_ContactResult[nShift], theApp.m_UiShiftProduction[ii].m_ContactResult[nShift] / fNumTotal * 100);
			m_btnAutoContactTotal[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_ManualContactResult[nShift], theApp.m_UiShiftProduction[ii].m_ManualContactResult[nShift] / fNumTotal * 100);
			m_btnManualContactTotal[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d (%.1f%%)"), theApp.m_UiShiftProduction[ii].m_MtpResult[nShift], theApp.m_UiShiftProduction[ii].m_MtpResult[nShift] / fNumTotal * 100);
			m_btnMtpTotal[ii].SetWindowText(sTemp);

			sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_AlignResult[nShift]);
			m_btnAlignTotal[ii].SetWindowText(sTemp);
		}
	}
}

void CDlgGammaInspect::ClickChResult()
{
	int iCommand = 0;

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_LBL_ALIGN_NG:      iCommand = Align_View;     break;
	case IDC_LBL_CONTACT_NG:    iCommand = Contact_View;   break;
	case IDC_LBL_MTP_NG:        iCommand = PreGamma_View;  break;
	}

	CDlgGammaAlignStatus dlg(m_iShift, iCommand);
	dlg.DoModal();
}

void CDlgGammaInspect::OnClickDataReset()
{
	if (theApp.m_iUserClass != USER_MAKER)
	{
		theApp.getMsgBox(MS_OK, _T("관리자만 이용 가능합니다."), _T("Maker is USE"), _T("Maker is USE"));
		return;
	}
	else
	{
		if (theApp.YesNoMsgBox(MS_YESNO, _T("Do You Want To Data Clear?"), _T("Do You Want To Data Clear?"), _T("Do You Want To Data Clear?")) == MSG_OK)
		{
			theApp.m_pTraceLog->LOG_INFO(_T("************************%s Data Reset************************"), ShiftDY_NT[m_iShift]);

			for (int ii = 0; ii < MaxGammaStage; ii++)
				theApp.m_UiShiftProduction[ii].Reset(m_iShift);

			theApp.InspectionDataSave(m_iShift);
			theApp.AlignDataSave(m_iShift);
			theApp.ContactDataSave(m_iShift);
			theApp.MtpDataSave(m_iShift);
		}
	}
}
#endif