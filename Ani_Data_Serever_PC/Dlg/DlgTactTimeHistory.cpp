// DlgTactTimeHistory.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "DlgTactTimeHistory.h"
#include "afxdialogex.h"
#include "StringSupport.h"
#include "Tact.h"

// CDlgTactTimeHistory 대화 상자입니다.
IMPLEMENT_DYNAMIC(CDlgTactTimeHistory, CDialog)

CDlgTactTimeHistory::CDlgTactTimeHistory(CWnd* pParent /*=NULL*/)
: CDialog(CDlgTactTimeHistory::IDD, pParent)
{

}

CDlgTactTimeHistory::~CDlgTactTimeHistory()
{

}

void CDlgTactTimeHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TACT_TIME_LIST_CTRL, m_TactTimeListCtrl);
}

BOOL CDlgTactTimeHistory::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strTemp;
	m_TactTimeListCtrl.SetExtendedStyle(m_TactTimeListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_TactTimeListCtrl.InsertColumn(0, L"", 0, 0);
	m_TactTimeListCtrl.InsertColumn(1, _T("Unit"), LVCFMT_CENTER, 200);
	m_TactTimeListCtrl.InsertColumn(2, _T("Average"), LVCFMT_CENTER, 93);

	for (int i = 0; i < MAX_TACT_NUM; i++) {
		strTemp.Format(_T("%ld"), i + 1);
		m_TactTimeListCtrl.InsertColumn(i + 3, strTemp, LVCFMT_CENTER, 93);
	}

	m_TactTimeListCtrl.SetFontSize(17, 2.0, 17, 0);
	m_TactTimeListCtrl.CreateImageList(77, 50);
	m_TactTimeListCtrl.SetMainHandle(this);
	m_TactTimeListCtrl.ShowSelectionBar(TRUE);
	m_TactTimeListCtrl.RedrawWindow();

	for (int ii = 0; ii < theApp.m_vecTactName.size(); ii++)
	{
		m_TactTimeListCtrl.InsertItem(ii, _T(""));
		m_TactTimeListCtrl.SetItemText(ii, 1, theApp.m_vecTactName[ii].m_strTactTimeName);
	}

	int itemCount = m_TactTimeListCtrl.GetItemCount();
	for (int ii = 0; ii < itemCount; ii++)
	{
		for (int jj = 2; jj < MAX_TACT_NUM + 3; jj++)
			m_TactTimeListCtrl.SetListItemTime(ii, jj, 0);
	}

	SetTimer(0, 1000, NULL);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgTactTimeHistory, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CDlgTactTimeHistory::OnCancel()
{

}


void CDlgTactTimeHistory::OnOK()
{
}

// CDlgTactTimeHistory 메시지 처리기입니다.
void CDlgTactTimeHistory::UpdateTactTime(int itemIndex, CTact& tactTime)
{
	int index = 0;
	int itemCnt = (int)tactTime.m_tactTimeList.size();
	for (int tactTimeValue : tactTime.m_tactTimeList)
	{
		m_TactTimeListCtrl.SetListItemTime(itemIndex, ((itemCnt - 1) - index) + 3, tactTimeValue);
		index++;
	}
	m_TactTimeListCtrl.SetListItemTime(itemIndex, 2, tactTime.GetAvgTactTime());
}

void CDlgTactTimeHistory::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	m_TactTimeListCtrl.LockWindowUpdate();

	int itemCount = m_TactTimeListCtrl.GetItemCount();
	for (int ii = 0; ii < itemCount; ii++)
	{
		for (int jj = 2; jj < MAX_TACT_NUM; jj++)
			m_TactTimeListCtrl.SetListItemTime(ii, jj, 0);
	}

	for (int ii = 0; ii < theApp.m_vecTactName.size(); ii++)
	{
		UpdateTactTime(ii, theApp.m_pTactTimeList[ii]);
	}
	
	m_TactTimeListCtrl.UnlockWindowUpdate();

	CDialog::OnTimer(nIDEvent);
}
