// DlgOkGrade.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "DlgOkGrade.h"
#include "afxdialogex.h"


// CDlgOkGrade 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgOkGrade, CDialog)

CDlgOkGrade::CDlgOkGrade(int ishift)
	: CDialog(CDlgOkGrade::IDD)
{
	m_ishift = ishift;
}

CDlgOkGrade::~CDlgOkGrade()
{
}

void CDlgOkGrade::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_A_GRADE, m_btnZoneGrade[0]);
	DDX_Control(pDX, IDC_B_GRADE, m_btnZoneGrade[1]);
	DDX_Control(pDX, IDC_C_GRADE, m_btnZoneGrade[2]);
}


BEGIN_MESSAGE_MAP(CDlgOkGrade, CDialog)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgOkGrade, CDialog)
	ON_EVENT(CDlgOkGrade, IDB_BTN_OK, DISPID_CLICK, CDlgOkGrade::OnOK, VTS_NONE)
END_EVENTSINK_MAP()


// CDlgOkGrade 메시지 처리기입니다.
BOOL CDlgOkGrade::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString sTemp;
	int nSumData[3] = { 0, 0, 0 };
	for (int ii = 0; ii < MaxZone; ii++)
	{
		nSumData[0] += theApp.m_UiShiftProduction[ii].m_GoodAGradeResult[m_ishift];
		nSumData[1] += theApp.m_UiShiftProduction[ii].m_GoodBGradeResult[m_ishift];
		nSumData[2] += theApp.m_UiShiftProduction[ii].m_GoodCGradeResult[m_ishift];
	}

	for (int ii = 0; ii < 3; ii++)
	{
		sTemp.Format(_T("%d"), nSumData[ii]);
		m_btnZoneGrade[ii].SetWindowText(sTemp);
	}
	

	return TRUE;
}

void CDlgOkGrade::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnOK();
}
#endif