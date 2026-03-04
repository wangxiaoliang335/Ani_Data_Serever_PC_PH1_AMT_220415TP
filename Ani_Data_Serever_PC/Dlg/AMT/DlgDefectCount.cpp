// CDlgDefectCount.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "DlgDefectCountHistory.h"
#include "DlgDefectCount.h"
#include "EZini.h"

// CDlgDefectCount

IMPLEMENT_DYNCREATE(CDlgDefectCount, CDialog)
#define IF_CHECK_TIMER 0

CDlgDefectCount *g_DlgDefectCount;
CDlgDefectCount::CDlgDefectCount(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDefectCount::IDD, pParent)
{
	g_DlgDefectCount = this;
}

CDlgDefectCount::~CDlgDefectCount()
{
}

void CDlgDefectCount::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LBL_DY_DOT, m_btnTitleLabel[0][0]);
	DDX_Control(pDX, IDC_LBL_DY_MURA, m_btnTitleLabel[0][1]);
	DDX_Control(pDX, IDC_LBL_DY_LINE, m_btnTitleLabel[0][2]);
	DDX_Control(pDX, IDC_LBL_DY_APPEAR, m_btnTitleLabel[0][3]);
	DDX_Control(pDX, IDC_LBL_DY_FUNCTION, m_btnTitleLabel[0][4]);
	DDX_Control(pDX, IDC_LBL_DY_DEFECT_TOTAL, m_btnTitleLabel[0][5]);

	DDX_Control(pDX, IDC_LBL_NT_DOT, m_btnTitleLabel[1][0]);
	DDX_Control(pDX, IDC_LBL_NT_MURA, m_btnTitleLabel[1][1]);
	DDX_Control(pDX, IDC_LBL_NT_LINE, m_btnTitleLabel[1][2]);
	DDX_Control(pDX, IDC_LBL_NT_APPEAR, m_btnTitleLabel[1][3]);
	DDX_Control(pDX, IDC_LBL_NT_FUNCTION, m_btnTitleLabel[1][4]);
	DDX_Control(pDX, IDC_LBL_NT_DEFECT_TOTAL, m_btnTitleLabel[1][5]);

	DDX_Control(pDX, IDC_DY_OPV_TOTAL_NG, m_btnOpvTotalNgSum[0]);
	DDX_Control(pDX, IDC_DY_OPV_OK, m_btnOpvOkSum[0]);
	DDX_Control(pDX, IDC_DY_OPV_NG, m_btnOpvNgSum[0]);

	DDX_Control(pDX, IDC_NT_OPV_TOTAL_NG, m_btnOpvTotalNgSum[1]);
	DDX_Control(pDX, IDC_NT_OPV_OK, m_btnOpvOkSum[1]);
	DDX_Control(pDX, IDC_NT_OPV_NG, m_btnOpvNgSum[1]);

	DDX_Control(pDX, IDC_DY_DOT_AOI, m_btnAoiDefectTotalSum[0][0]);
	DDX_Control(pDX, IDC_DY_MURA_AOI, m_btnAoiDefectTotalSum[0][1]);
	DDX_Control(pDX, IDC_DY_LINE_AOI, m_btnAoiDefectTotalSum[0][2]);
	DDX_Control(pDX, IDC_DY_APPEAR_AOI, m_btnAoiDefectTotalSum[0][3]);
	DDX_Control(pDX, IDC_DY_FUNCTION_AOI, m_btnAoiDefectTotalSum[0][4]);
	DDX_Control(pDX, IDC_DY_DEFECT_TOTAL_AOI, m_btnAoiDefectTotalSum[0][5]);

	DDX_Control(pDX, IDC_DY_DOT_OPV, m_btnOpvDefectTotalSum[0][0]);
	DDX_Control(pDX, IDC_DY_MURA_OPV, m_btnOpvDefectTotalSum[0][1]);
	DDX_Control(pDX, IDC_DY_LINE_OPV, m_btnOpvDefectTotalSum[0][2]);
	DDX_Control(pDX, IDC_DY_APPEAR_OPV, m_btnOpvDefectTotalSum[0][3]);
	DDX_Control(pDX, IDC_DY_FUNCTION_OPV, m_btnOpvDefectTotalSum[0][4]);
	DDX_Control(pDX, IDC_DY_DEFECT_TOTAL_OPV, m_btnOpvDefectTotalSum[0][5]);

	DDX_Control(pDX, IDC_DY_DOT_MATCH, m_btnMatchDefectTotalSum[0][0]);
	DDX_Control(pDX, IDC_DY_MURA_MATCH, m_btnMatchDefectTotalSum[0][1]);
	DDX_Control(pDX, IDC_DY_LINE_MATCH, m_btnMatchDefectTotalSum[0][2]);
	DDX_Control(pDX, IDC_DY_APPEAR_MATCH, m_btnMatchDefectTotalSum[0][3]);
	DDX_Control(pDX, IDC_DY_FUNCTIIN_MATCH, m_btnMatchDefectTotalSum[0][4]);
	DDX_Control(pDX, IDC_DY_DEFECT_TOTAL_MATCH, m_btnMatchDefectTotalSum[0][5]);

	DDX_Control(pDX, IDC_DY_DOT_OVERKILL, m_btnOverKillDefectTotalSum[0][0]);
	DDX_Control(pDX, IDC_DY_MURA_OVERKILL, m_btnOverKillDefectTotalSum[0][1]);
	DDX_Control(pDX, IDC_DY_LINE_OVERKILL, m_btnOverKillDefectTotalSum[0][2]);
	DDX_Control(pDX, IDC_DY_APPEAR_OVERKILL, m_btnOverKillDefectTotalSum[0][3]);
	DDX_Control(pDX, IDC_DY_FUNCTION_OVERKILL, m_btnOverKillDefectTotalSum[0][4]);
	DDX_Control(pDX, IDC_DY_DEFECT_TOTAL_OVERKILL, m_btnOverKillDefectTotalSum[0][5]);

	DDX_Control(pDX, IDC_DY_DOT_UNDERKILL, m_btnUnderKillDefectTotalSum[0][0]);
	DDX_Control(pDX, IDC_DY_MURA_UNDERKILL, m_btnUnderKillDefectTotalSum[0][1]);
	DDX_Control(pDX, IDC_DY_LINE_UNDERKILL, m_btnUnderKillDefectTotalSum[0][2]);
	DDX_Control(pDX, IDC_DY_APPEAR_UNDERKILL, m_btnUnderKillDefectTotalSum[0][3]);
	DDX_Control(pDX, IDC_DY_FUNCTION_UNDERKILL, m_btnUnderKillDefectTotalSum[0][4]);
	DDX_Control(pDX, IDC_DY_DEFECT_TOTAL_UNDERKILL, m_btnUnderKillDefectTotalSum[0][5]);

	DDX_Control(pDX, IDC_NT_DOT_AOI, m_btnAoiDefectTotalSum[1][0]);
	DDX_Control(pDX, IDC_NT_MURA_AOI, m_btnAoiDefectTotalSum[1][1]);
	DDX_Control(pDX, IDC_NT_LINE_AOI, m_btnAoiDefectTotalSum[1][2]);
	DDX_Control(pDX, IDC_NT_APPEAR_AOI, m_btnAoiDefectTotalSum[1][3]);
	DDX_Control(pDX, IDC_NT_FUNCTION_AOI, m_btnAoiDefectTotalSum[1][4]);
	DDX_Control(pDX, IDC_NT_DEFECT_TOTAL_AOI, m_btnAoiDefectTotalSum[1][5]);

	DDX_Control(pDX, IDC_NT_DOT_OPV, m_btnOpvDefectTotalSum[1][0]);
	DDX_Control(pDX, IDC_NT_MURA_OPV, m_btnOpvDefectTotalSum[1][1]);
	DDX_Control(pDX, IDC_NT_LINE_OPV, m_btnOpvDefectTotalSum[1][2]);
	DDX_Control(pDX, IDC_NT_APPEAR_OPV, m_btnOpvDefectTotalSum[1][3]);
	DDX_Control(pDX, IDC_NT_FUNCTION_OPV, m_btnOpvDefectTotalSum[1][4]);
	DDX_Control(pDX, IDC_NT_DEFECT_TOTAL_OPV, m_btnOpvDefectTotalSum[1][5]);

	DDX_Control(pDX, IDC_NT_DOT_MATCH, m_btnMatchDefectTotalSum[1][0]);
	DDX_Control(pDX, IDC_NT_MURA_MATCH, m_btnMatchDefectTotalSum[1][1]);
	DDX_Control(pDX, IDC_NT_LINE_MATCH, m_btnMatchDefectTotalSum[1][2]);
	DDX_Control(pDX, IDC_NT_APPEAR_MATCH, m_btnMatchDefectTotalSum[1][3]);
	DDX_Control(pDX, IDC_NT_FUNCTIIN_MATCH, m_btnMatchDefectTotalSum[1][4]);
	DDX_Control(pDX, IDC_NT_DEFECT_TOTAL_MATCH, m_btnMatchDefectTotalSum[1][5]);

	DDX_Control(pDX, IDC_NT_DOT_OVERKILL, m_btnOverKillDefectTotalSum[1][0]);
	DDX_Control(pDX, IDC_NT_MURA_OVERKILL, m_btnOverKillDefectTotalSum[1][1]);
	DDX_Control(pDX, IDC_NT_LINE_OVERKILL, m_btnOverKillDefectTotalSum[1][2]);
	DDX_Control(pDX, IDC_NT_APPEAR_OVERKILL, m_btnOverKillDefectTotalSum[1][3]);
	DDX_Control(pDX, IDC_NT_FUNCTION_OVERKILL, m_btnOverKillDefectTotalSum[1][4]);
	DDX_Control(pDX, IDC_NT_DEFECT_TOTAL_OVERKILL, m_btnOverKillDefectTotalSum[1][5]);

	DDX_Control(pDX, IDC_NT_DOT_UNDERKILL, m_btnUnderKillDefectTotalSum[1][0]);
	DDX_Control(pDX, IDC_NT_MURA_UNDERKILL, m_btnUnderKillDefectTotalSum[1][1]);
	DDX_Control(pDX, IDC_NT_LINE_UNDERKILL, m_btnUnderKillDefectTotalSum[1][2]);
	DDX_Control(pDX, IDC_NT_APPEAR_UNDERKILL, m_btnUnderKillDefectTotalSum[1][3]);
	DDX_Control(pDX, IDC_NT_FUNCTION_UNDERKILL, m_btnUnderKillDefectTotalSum[1][4]);
	DDX_Control(pDX, IDC_NT_DEFECT_TOTAL_UNDERKILL, m_btnUnderKillDefectTotalSum[1][5]);
}

BEGIN_MESSAGE_MAP(CDlgDefectCount, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgDefectCount, CDialog)
	ON_EVENT(CDlgDefectCount, IDC_DY_DATA_RESET, DISPID_CLICK, CDlgDefectCount::OnClickDataReset, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_NT_DATA_RESET, DISPID_CLICK, CDlgDefectCount::OnClickDataReset, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_DY_OPV_TOTAL_NG, DISPID_CLICK, CDlgDefectCount::PanelDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_NT_OPV_TOTAL_NG, DISPID_CLICK, CDlgDefectCount::PanelDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_DY_DOT, DISPID_CLICK, CDlgDefectCount::TitleDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_DY_MURA, DISPID_CLICK, CDlgDefectCount::TitleDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_DY_LINE, DISPID_CLICK, CDlgDefectCount::TitleDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_DY_APPEAR, DISPID_CLICK, CDlgDefectCount::TitleDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_DY_FUNCTION, DISPID_CLICK, CDlgDefectCount::TitleDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_NT_DOT, DISPID_CLICK, CDlgDefectCount::TitleDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_NT_MURA, DISPID_CLICK, CDlgDefectCount::TitleDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_NT_LINE, DISPID_CLICK, CDlgDefectCount::TitleDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_NT_APPEAR, DISPID_CLICK, CDlgDefectCount::TitleDefectListView, VTS_NONE)
	ON_EVENT(CDlgDefectCount, IDC_LBL_NT_FUNCTION, DISPID_CLICK, CDlgDefectCount::TitleDefectListView, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CDlgDefectCount::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDefectTitle();
	SetTimer(TMR_MAIN_DEFECT_INFO, 1000, NULL);
	return TRUE;
}

void CDlgDefectCount::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	if (nIDEvent == TMR_MAIN_DEFECT_INFO)
	{
		UpdateDisplay(Shift_DY);
		UpdateDisplay(Shift_NT);
	}
	CDialog::OnTimer(nIDEvent);
}


BOOL CDlgDefectCount::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgDefectCount::OnClickDataReset()
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
			theApp.m_pTraceLog->LOG_INFO(_T("************************%s DefectCountData Reset************************"), strShift);
			theApp.m_SumDefectCountData[iShift].Reset();
		}
	}
}

void CDlgDefectCount::PanelDefectListView()
{
	int iShift = 0;

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_LBL_DY_OPV_TOTAL_NG: iShift = Shift_DY; break;
	case IDC_LBL_NT_OPV_TOTAL_NG: iShift = Shift_NT; break;
	}

	CDlgDefectCountHistory dlg(iShift, PanelDefectCount);
	dlg.DoModal();
}

void CDlgDefectCount::TitleDefectListView()
{
	int iTitleNum = 0;
	int iShift = 0;
	CString strTitleName = _T("");

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_LBL_DY_DOT:
	case IDC_LBL_NT_DOT: iTitleNum = DefectTitleName_1;
		break;
	case IDC_LBL_DY_MURA:
	case IDC_LBL_NT_MURA: iTitleNum = DefectTitleName_2;
		break;
	case IDC_LBL_DY_LINE:
	case IDC_LBL_NT_LINE: iTitleNum = DefectTitleName_3;
		break;
	case IDC_LBL_DY_APPEAR:
	case IDC_LBL_NT_APPEAR: iTitleNum = DefectTitleName_4;
		break;
	case IDC_LBL_DY_FUNCTION:
	case IDC_LBL_NT_FUNCTION: iTitleNum = DefectTitleName_5;
		break;
	}

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_LBL_DY_DOT:
	case IDC_LBL_DY_MURA:
	case IDC_LBL_DY_LINE:
	case IDC_LBL_DY_APPEAR:
	case IDC_LBL_DY_FUNCTION: iShift = Shift_DY; break;
	case IDC_LBL_NT_DOT:
	case IDC_LBL_NT_MURA:
	case IDC_LBL_NT_LINE:
	case IDC_LBL_NT_APPEAR:
	case IDC_LBL_NT_FUNCTION: iShift = Shift_NT; break;
	}

	strTitleName = m_btnTitleLabel[iShift][iTitleNum].GetCaption();

	CDlgDefectCountHistory dlg(iShift, TitleDefectCount, strTitleName);
	dlg.DoModal();
}

void CDlgDefectCount::SetDefectTitle()
{
	theApp.OpvLoadTitleName();

	for (int ii = 0; ii < eNumShift; ii++)
	{
		for (int jj = 0; jj < DefectTitleMaxCount; jj++)
		{
			m_btnTitleLabel[ii][jj].SetWindowTextW(theApp.m_strDefectTitleName[jj]);
		}
	}
}

void CDlgDefectCount::UpdateDisplay(int nShift)
{
	CString sTemp, sTemp1, sTemp2;
	DefectSumCountData DefectSumData;
	DefectSumData.Reset();
	sTemp1.Format(_T("0"));
	sTemp2.Format(_T("0 (0.0%%)"));
	float fNumTotal = 0.;

	theApp.InspctionDefectDataSum(theApp.m_SumDefectCountData[nShift], theApp.m_lastShiftIndex, DefectSumData);
	for (int ii = 0; ii < 6; ii++)
	{
		if (ii == 5)
			fNumTotal = DefectSumData.m_TotalDefectSum;
		else
			fNumTotal = theApp.m_SumDefectCountData[nShift].m_TotalDefectSum[ii];
		

		if (fNumTotal == 0)
		{
			m_btnAoiDefectTotalSum[nShift][ii].SetWindowText(sTemp1);
			m_btnOpvDefectTotalSum[nShift][ii].SetWindowText(sTemp1);
			m_btnMatchDefectTotalSum[nShift][ii].SetWindowText(sTemp2);
			m_btnOverKillDefectTotalSum[nShift][ii].SetWindowText(sTemp2);
			m_btnUnderKillDefectTotalSum[nShift][ii].SetWindowText(sTemp2);
		}
		else
		{
			if (ii == 5)
			{
				sTemp.Format(_T("%d"), DefectSumData.m_AoiDefectTotalSum);
				m_btnAoiDefectTotalSum[nShift][ii].SetWindowText(sTemp);

				sTemp.Format(_T("%d"), DefectSumData.m_OpvDefectTotalSum);
				m_btnOpvDefectTotalSum[nShift][ii].SetWindowText(sTemp);

				sTemp.Format(_T("%d (%.1f%%)"), DefectSumData.m_MatchDefectTotalSum, DefectSumData.m_MatchDefectTotalSum / fNumTotal * 100);
				m_btnMatchDefectTotalSum[nShift][ii].SetWindowText(sTemp);

				sTemp.Format(_T("%d (%.1f%%)"), DefectSumData.m_OverKillDefectTotalSum, DefectSumData.m_OverKillDefectTotalSum / fNumTotal * 100);
				m_btnOverKillDefectTotalSum[nShift][ii].SetWindowText(sTemp);

				sTemp.Format(_T("%d (%.1f%%)"), DefectSumData.m_UnderKillDefectTotalSum, DefectSumData.m_UnderKillDefectTotalSum / fNumTotal * 100);
				m_btnUnderKillDefectTotalSum[nShift][ii].SetWindowText(sTemp);
			}
			else
			{
				sTemp.Format(_T("%d"), theApp.m_SumDefectCountData[nShift].m_AoiDefectTotalSum[ii]);
				m_btnAoiDefectTotalSum[nShift][ii].SetWindowText(sTemp);

				sTemp.Format(_T("%d"), theApp.m_SumDefectCountData[nShift].m_OpvDefectTotalSum[ii]);
				m_btnOpvDefectTotalSum[nShift][ii].SetWindowText(sTemp);

				sTemp.Format(_T("%d (%.1f%%)"), theApp.m_SumDefectCountData[nShift].m_MatchDefectTotalSum[ii], theApp.m_SumDefectCountData[nShift].m_MatchDefectTotalSum[ii] / fNumTotal * 100);
				m_btnMatchDefectTotalSum[nShift][ii].SetWindowText(sTemp);

				sTemp.Format(_T("%d (%.1f%%)"), theApp.m_SumDefectCountData[nShift].m_OverKillDefectTotalSum[ii], theApp.m_SumDefectCountData[nShift].m_OverKillDefectTotalSum[ii] / fNumTotal * 100);
				m_btnOverKillDefectTotalSum[nShift][ii].SetWindowText(sTemp);

				sTemp.Format(_T("%d (%.1f%%)"), theApp.m_SumDefectCountData[nShift].m_UnderKillDefectTotalSum[ii], theApp.m_SumDefectCountData[nShift].m_UnderKillDefectTotalSum[ii] / fNumTotal * 100);
				m_btnUnderKillDefectTotalSum[nShift][ii].SetWindowText(sTemp);
			}
		}
	}

	if (theApp.m_SumDefectCountData[nShift].m_OpvTotalNgSum == 0)
	{
		m_btnOpvTotalNgSum[nShift].SetWindowText(sTemp1);
		m_btnOpvOkSum[nShift].SetWindowText(sTemp1);
		m_btnOpvNgSum[nShift].SetWindowText(sTemp1);
	}
	else
	{
		sTemp.Format(_T("%d"), theApp.m_SumDefectCountData[nShift].m_OpvTotalNgSum);
		m_btnOpvTotalNgSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d"), theApp.m_SumDefectCountData[nShift].m_OpvOkSum);
		m_btnOpvOkSum[nShift].SetWindowText(sTemp);

		sTemp.Format(_T("%d"), theApp.m_SumDefectCountData[nShift].m_OpvNgSum);
		m_btnOpvNgSum[nShift].SetWindowText(sTemp);
	}
}
#endif