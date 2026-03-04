// DlgAlignStatus.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#if _SYSTEM_GAMMA_
#include "Ani_Data_Serever_PC.h"
#include "DlgGammaAlignStatus.h"
#include "afxdialogex.h"


// CDlgAlignStatus 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgGammaAlignStatus, CDialog)

CDlgGammaAlignStatus::CDlgGammaAlignStatus(int iShift, int iContactAlign)
: CDialog(CDlgGammaAlignStatus::IDD)
{
	m_iShift = iShift;
	m_iContactAlign = iContactAlign;
}

CDlgGammaAlignStatus::~CDlgGammaAlignStatus()
{
}

void CDlgGammaAlignStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STAGE1_GOOD_1, m_btnStageGood[GammaStage_1][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE1_GOOD_2, m_btnStageGood[GammaStage_1][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE1_NG_1, m_btnStageNg[GammaStage_1][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE1_NG_2, m_btnStageNg[GammaStage_1][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE2_GOOD_1, m_btnStageGood[GammaStage_2][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE2_GOOD_2, m_btnStageGood[GammaStage_2][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE2_NG_1, m_btnStageNg[GammaStage_2][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE2_NG_2, m_btnStageNg[GammaStage_2][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE3_GOOD_1, m_btnStageGood[GammaStage_3][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE3_GOOD_2, m_btnStageGood[GammaStage_3][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE3_NG_1, m_btnStageNg[GammaStage_3][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE3_NG_2, m_btnStageNg[GammaStage_3][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE4_GOOD_1, m_btnStageGood[GammaStage_4][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE4_GOOD_2, m_btnStageGood[GammaStage_4][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE4_NG_1, m_btnStageNg[GammaStage_4][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE4_NG_2, m_btnStageNg[GammaStage_4][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE5_GOOD_1, m_btnStageGood[GammaStage_5][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE5_GOOD_2, m_btnStageGood[GammaStage_5][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE5_NG_1, m_btnStageNg[GammaStage_5][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE5_NG_2, m_btnStageNg[GammaStage_5][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE6_GOOD_1, m_btnStageGood[GammaStage_6][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE6_GOOD_2, m_btnStageGood[GammaStage_6][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE6_NG_1, m_btnStageNg[GammaStage_6][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE6_NG_2, m_btnStageNg[GammaStage_6][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE7_GOOD_1, m_btnStageGood[GammaStage_7][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE7_GOOD_2, m_btnStageGood[GammaStage_7][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE7_NG_1, m_btnStageNg[GammaStage_7][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE7_NG_2, m_btnStageNg[GammaStage_7][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE8_GOOD_1, m_btnStageGood[GammaStage_8][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE8_GOOD_2, m_btnStageGood[GammaStage_8][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE8_NG_1, m_btnStageNg[GammaStage_8][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE8_NG_2, m_btnStageNg[GammaStage_8][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE9_GOOD_1, m_btnStageGood[GammaStage_9][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE9_GOOD_2, m_btnStageGood[GammaStage_9][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE9_NG_1, m_btnStageNg[GammaStage_9][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE9_NG_2, m_btnStageNg[GammaStage_9][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE10_GOOD_1, m_btnStageGood[GammaStage_10][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE10_GOOD_2, m_btnStageGood[GammaStage_10][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE10_NG_1, m_btnStageNg[GammaStage_10][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE10_NG_2, m_btnStageNg[GammaStage_10][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE11_GOOD_1, m_btnStageGood[GammaStage_11][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE11_GOOD_2, m_btnStageGood[GammaStage_11][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE11_NG_1, m_btnStageNg[GammaStage_11][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE11_NG_2, m_btnStageNg[GammaStage_11][PanelNum2]);

	DDX_Control(pDX, IDC_STAGE12_GOOD_1, m_btnStageGood[GammaStage_12][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE12_GOOD_2, m_btnStageGood[GammaStage_12][PanelNum2]);
	DDX_Control(pDX, IDC_STAGE12_NG_1, m_btnStageNg[GammaStage_12][PanelNum1]);
	DDX_Control(pDX, IDC_STAGE12_NG_2, m_btnStageNg[GammaStage_12][PanelNum2]);
}


BEGIN_MESSAGE_MAP(CDlgGammaAlignStatus, CDialog)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgGammaAlignStatus, CDialog)
	ON_EVENT(CDlgGammaAlignStatus, IDB_BTN_OK, DISPID_CLICK, CDlgGammaAlignStatus::OnOK, VTS_NONE)
END_EVENTSINK_MAP()


// CDlgAlignStatus 메시지 처리기입니다.
BOOL CDlgGammaAlignStatus::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString sTemp;

	if (m_iContactAlign == Align_View)
	{
		for (int ii = 0; ii < MaxGammaStage; ii++)
			for (int jj = 0; jj < ChMaxCount; jj++)
			{
				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_AlignShiftGood[m_iShift][jj]);
				m_btnStageGood[ii][jj].SetWindowText(sTemp);

				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_AlignShiftNg[m_iShift][jj]);
				m_btnStageNg[ii][jj].SetWindowText(sTemp);
			}
	}
	else if (m_iContactAlign == Contact_View)
	{
		for (int ii = 0; ii < MaxGammaStage; ii++)
			for (int jj = 0; jj < ChMaxCount; jj++)
			{
				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_ContactGood[m_iShift][jj]);
				m_btnStageGood[ii][jj].SetWindowText(sTemp);

				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_ContactNg[m_iShift][jj]);
				m_btnStageNg[ii][jj].SetWindowText(sTemp);
			}
	}
	else
	{
		for (int ii = 0; ii < MaxGammaStage; ii++)
			for (int jj = 0; jj < ChMaxCount; jj++)
			{
				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_MtpGood[m_iShift][jj]);
				m_btnStageGood[ii][jj].SetWindowText(sTemp);

				sTemp.Format(_T("%d"), theApp.m_UiShiftProduction[ii].m_MtpNg[m_iShift][jj]);
				m_btnStageNg[ii][jj].SetWindowText(sTemp);
			}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgGammaAlignStatus::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnOK();
}
#endif