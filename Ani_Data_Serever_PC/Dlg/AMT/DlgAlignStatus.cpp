// DlgAlignStatus.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "DlgAlignStatus.h"
#include "afxdialogex.h"


// CDlgAlignStatus 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgAlignStatus, CDialog)

CDlgAlignStatus::CDlgAlignStatus(int ishift, int iContactAlign)
	: CDialog(CDlgAlignStatus::IDD)
{
	m_ishift = ishift;
	m_iContactAlign = iContactAlign;
}

CDlgAlignStatus::~CDlgAlignStatus()
{
}

void CDlgAlignStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AZONE_GOOD_1, m_btnZoneGood[AZone][PanelNum1]);
	DDX_Control(pDX, IDC_AZONE_GOOD_2, m_btnZoneGood[AZone][PanelNum2]);
	DDX_Control(pDX, IDC_AZONE_GOOD_3, m_btnZoneGood[AZone][PanelNum3]);
	DDX_Control(pDX, IDC_AZONE_GOOD_4, m_btnZoneGood[AZone][PanelNum4]);
	DDX_Control(pDX, IDC_AZONE_NG_1, m_btnZoneNg[AZone][PanelNum1]);
	DDX_Control(pDX, IDC_AZONE_NG_2, m_btnZoneNg[AZone][PanelNum2]);
	DDX_Control(pDX, IDC_AZONE_NG_3, m_btnZoneNg[AZone][PanelNum3]);
	DDX_Control(pDX, IDC_AZONE_NG_4, m_btnZoneNg[AZone][PanelNum4]);

	DDX_Control(pDX, IDC_BZONE_GOOD_1, m_btnZoneGood[BZone][PanelNum1]);
	DDX_Control(pDX, IDC_BZONE_GOOD_2, m_btnZoneGood[BZone][PanelNum2]);
	DDX_Control(pDX, IDC_BZONE_GOOD_3, m_btnZoneGood[BZone][PanelNum3]);
	DDX_Control(pDX, IDC_BZONE_GOOD_4, m_btnZoneGood[BZone][PanelNum4]);
	DDX_Control(pDX, IDC_BZONE_NG_1, m_btnZoneNg[BZone][PanelNum1]);
	DDX_Control(pDX, IDC_BZONE_NG_2, m_btnZoneNg[BZone][PanelNum2]);
	DDX_Control(pDX, IDC_BZONE_NG_3, m_btnZoneNg[BZone][PanelNum3]);
	DDX_Control(pDX, IDC_BZONE_NG_4, m_btnZoneNg[BZone][PanelNum4]);

	DDX_Control(pDX, IDC_CZONE_GOOD_1, m_btnZoneGood[CZone][PanelNum1]);
	DDX_Control(pDX, IDC_CZONE_GOOD_2, m_btnZoneGood[CZone][PanelNum2]);
	DDX_Control(pDX, IDC_CZONE_GOOD_3, m_btnZoneGood[CZone][PanelNum3]);
	DDX_Control(pDX, IDC_CZONE_GOOD_4, m_btnZoneGood[CZone][PanelNum4]);
	DDX_Control(pDX, IDC_CZONE_NG_1, m_btnZoneNg[CZone][PanelNum1]);
	DDX_Control(pDX, IDC_CZONE_NG_2, m_btnZoneNg[CZone][PanelNum2]);
	DDX_Control(pDX, IDC_CZONE_NG_3, m_btnZoneNg[CZone][PanelNum3]);
	DDX_Control(pDX, IDC_CZONE_NG_4, m_btnZoneNg[CZone][PanelNum4]);

	DDX_Control(pDX, IDC_DZONE_GOOD_1, m_btnZoneGood[DZone][PanelNum1]);
	DDX_Control(pDX, IDC_DZONE_GOOD_2, m_btnZoneGood[DZone][PanelNum2]);
	DDX_Control(pDX, IDC_DZONE_GOOD_3, m_btnZoneGood[DZone][PanelNum3]);
	DDX_Control(pDX, IDC_DZONE_GOOD_4, m_btnZoneGood[DZone][PanelNum4]);
	DDX_Control(pDX, IDC_DZONE_NG_1, m_btnZoneNg[DZone][PanelNum1]);
	DDX_Control(pDX, IDC_DZONE_NG_2, m_btnZoneNg[DZone][PanelNum2]);
	DDX_Control(pDX, IDC_DZONE_NG_3, m_btnZoneNg[DZone][PanelNum3]);
	DDX_Control(pDX, IDC_DZONE_NG_4, m_btnZoneNg[DZone][PanelNum4]);

}


BEGIN_MESSAGE_MAP(CDlgAlignStatus, CDialog)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgAlignStatus, CDialog)
	ON_EVENT(CDlgAlignStatus, IDB_BTN_OK, DISPID_CLICK, CDlgAlignStatus::OnOK, VTS_NONE)
END_EVENTSINK_MAP()


// CDlgAlignStatus 메시지 처리기입니다.
BOOL CDlgAlignStatus::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString sTemp;

	if (m_iContactAlign == Align_View)
	{
		for (int ii = 0; ii < MaxZone; ii++)
			for (int jj = 0; jj < PanelMaxCount; jj++)
			{
				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_AlignShiftGood[m_ishift][jj]);
				m_btnZoneGood[ii][jj].SetWindowText(sTemp);

				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_AlignShiftNg[m_ishift][jj]);
				m_btnZoneNg[ii][jj].SetWindowText(sTemp);
			}
	}
	else if (m_iContactAlign == Contact_View)
	{
		for (int ii = 0; ii < MaxZone; ii++)
			for (int jj = 0; jj < PanelMaxCount; jj++)
			{
				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_ContactGood[m_ishift][jj]);
				m_btnZoneGood[ii][jj].SetWindowText(sTemp);

				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_ContactNg[m_ishift][jj]);
				m_btnZoneNg[ii][jj].SetWindowText(sTemp);
			}
	}
	else if (m_iContactAlign == PreGamma_View)
	{
		for (int ii = 0; ii < MaxZone; ii++)
			for (int jj = 0; jj < PanelMaxCount; jj++)
			{
				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_PreGammaGood[m_ishift][jj]);
				m_btnZoneGood[ii][jj].SetWindowText(sTemp);

				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_PreGammaNg[m_ishift][jj]);
				m_btnZoneNg[ii][jj].SetWindowText(sTemp);
			}
	}
	else
	{
		for (int ii = 0; ii < MaxZone; ii++)
			for (int jj = 0; jj < PanelMaxCount; jj++)
			{
				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_TpGood[m_ishift][jj]);
				m_btnZoneGood[ii][jj].SetWindowText(sTemp);

				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_TpNg[m_ishift][jj]);
				m_btnZoneNg[ii][jj].SetWindowText(sTemp);
			}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgAlignStatus::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnOK();
}

#endif