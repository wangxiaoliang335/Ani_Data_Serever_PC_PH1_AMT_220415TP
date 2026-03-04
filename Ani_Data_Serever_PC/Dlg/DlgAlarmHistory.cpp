// MainViewAlarmHistoryPage.cpp : implementation file
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "DlgAlarmHistory.h"
#include "afxdialogex.h"
#include "StringSupport.h"

// CDlgAlarmHistory dialog

IMPLEMENT_DYNAMIC(CDlgAlarmHistory, CDialog)

CDlgAlarmHistory::CDlgAlarmHistory(CWnd* pParent /*=NULL*/)
: CDialog(CDlgAlarmHistory::IDD, pParent)
{
	m_iModeNum = HistoryMode; 
	m_iRankShiftType = Shift_DY;
	m_bRankClickFlag = TRUE;
}

CDlgAlarmHistory::~CDlgAlarmHistory()
{
}

void CDlgAlarmHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ALARM_LIST_CTRL, m_alarmListCtrl);
	DDX_Control(pDX, IDC_ALARM_RANK_CTRL, m_alarmRankCtrl);
}

BEGIN_MESSAGE_MAP(CDlgAlarmHistory, CDialog)
	ON_WM_TIMER()

END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgAlarmHistory, CDialog)
	ON_EVENT(CDlgAlarmHistory, IDB_HISTORY, DISPID_CLICK, CDlgAlarmHistory::ClickHistoryMode, VTS_NONE)
	ON_EVENT(CDlgAlarmHistory, IDB_RANK, DISPID_CLICK, CDlgAlarmHistory::ClickHistoryMode, VTS_NONE)
	ON_EVENT(CDlgAlarmHistory, IDC_RANK_DY, DISPID_CLICK, CDlgAlarmHistory::ClickRankType, VTS_NONE)
	ON_EVENT(CDlgAlarmHistory, IDC_RANK_NT, DISPID_CLICK, CDlgAlarmHistory::ClickRankType, VTS_NONE)
END_EVENTSINK_MAP()



BOOL CDlgAlarmHistory::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDlgAlarmHistory::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_alarmListCtrl.SetExtendedStyle(m_alarmListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_alarmListCtrl.InsertColumn(0, L"", 0, 0);
	m_alarmListCtrl.InsertColumn(1, _T("No"), LVCFMT_CENTER, 60);
	m_alarmListCtrl.InsertColumn(2, _T("Occurrence\nTime"), LVCFMT_CENTER, 180);
	m_alarmListCtrl.InsertColumn(3, _T("Alarm\nCode"), LVCFMT_CENTER, 80);
	m_alarmListCtrl.InsertColumn(4, _T("Alarm\nMessage"), LVCFMT_LEFT, 910);

	m_alarmListCtrl.SetFontSize(17, 2.0, 17, 0);
	m_alarmListCtrl.CreateImageList(77, 35);

	CBitmap bmpEmpty, bmpClear;
	m_alarmListCtrl.GetCurImageList()->Add(&bmpEmpty, RGB(0, 0, 0));
	m_alarmListCtrl.SetMainHandle(this);
	m_alarmListCtrl.ShowSelectionBar(TRUE);
	m_alarmListCtrl.RedrawWindow();

	m_alarmRankCtrl.SetExtendedStyle(m_alarmRankCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_alarmRankCtrl.InsertColumn(0, L"", 0, 0);
	m_alarmRankCtrl.InsertColumn(1, _T("No"), LVCFMT_CENTER, 60);
	m_alarmRankCtrl.InsertColumn(2, _T("Alarm\nCount"), LVCFMT_CENTER, 180);
	m_alarmRankCtrl.InsertColumn(3, _T("Alarm\nCode"), LVCFMT_CENTER, 80);
	m_alarmRankCtrl.InsertColumn(4, _T("Alarm\nMessage"), LVCFMT_LEFT, 910);

	m_alarmRankCtrl.SetFontSize(17, 2.0, 17, 0);
	m_alarmRankCtrl.CreateImageList(77, 35);

	m_alarmRankCtrl.GetCurImageList()->Add(&bmpEmpty, RGB(0, 0, 0));
	m_alarmRankCtrl.SetMainHandle(this);
	m_alarmRankCtrl.ShowSelectionBar(TRUE);
	m_alarmRankCtrl.RedrawWindow();

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDB_HISTORY);
	pBtnEnh->SetValue(TRUE);

	pBtnEnh = (CBtnEnh*)GetDlgItem(IDC_RANK_DY);
	pBtnEnh->SetValue(TRUE);

	GetDlgItem(IDC_RANK_DY)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RANK_NT)->ShowWindow(SW_HIDE);

	ModeSelect(HistoryMode);

	SetTimer(0, 1000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgAlarmHistory::SetAlarmData(int index, AlarmDataItem pAlarmDataItem)
{
	if (m_iModeNum == HistoryMode)
	{
		while (m_alarmListCtrl.GetItemCount() <= index)
			m_alarmListCtrl.InsertItem(index, L"");

		SetItemText(index, 1, CStringSupport::N2C(index % 1000 + 1));
		SetItemText(index, 2, pAlarmDataItem.m_strTime);
		SetItemText(index, 3, pAlarmDataItem.m_alarmCode);
		SetItemText(index, 4, pAlarmDataItem.m_alarmMsg);
	}
	else
	{
		while (m_alarmRankCtrl.GetItemCount() <= index)
			m_alarmRankCtrl.InsertItem(index, L"");

 		SetItemText(index, 1, CStringSupport::N2C(index % 1000 + 1));
		SetItemText(index, 2, CStringSupport::N2C(pAlarmDataItem.m_alarmCount));
		SetItemText(index, 3, pAlarmDataItem.m_alarmCode);
		SetItemText(index, 4, pAlarmDataItem.m_alarmMsg);
	}
}

void CDlgAlarmHistory::SetItemText(int iRow, int iCol, CString strVal)
{
	if (m_iModeNum == HistoryMode)
	{
		if (m_alarmListCtrl.GetItemText(iRow, iCol) != strVal)
			m_alarmListCtrl.SetItemText(iRow, iCol, strVal);
	}
	else
	{
		if (m_alarmRankCtrl.GetItemText(iRow, iCol) != strVal)
			m_alarmRankCtrl.SetItemText(iRow, iCol, strVal);
	}
		
}


void CDlgAlarmHistory::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	AlarmDataCheck();
}
void  CDlgAlarmHistory::AlarmDataCheck()
{
	if (m_iModeNum == HistoryMode)
	{
		if (theApp.m_AlarmDataList.size() == 0)
			return;

		m_alarmListCtrl.LockWindowUpdate();

		for (int ii = 0; ii < theApp.m_AlarmDataList.size(); ii++)
		{
			SetAlarmData(ii, theApp.m_AlarmDataList[ii]);
			if (ii == 1000)
				break;
		}
		m_alarmListCtrl.UnlockWindowUpdate();
	}

	else
	{
		vector<AlarmDataItem> vecRankList;
		vecRankList.clear();

		if (theApp.m_AlarmRankCount[m_iRankShiftType].size() == 0)
		{
			m_alarmRankCtrl.DeleteAllItems();
			return;
		}

		for (auto RankList : theApp.m_AlarmRankCount[m_iRankShiftType])
		{
			vecRankList.push_back(RankList.second);
			sort(vecRankList.begin(), vecRankList.end(), AlarmComp);
		}

		m_alarmRankCtrl.LockWindowUpdate();

		if (!m_bRankClickFlag)
		{
			m_alarmRankCtrl.DeleteAllItems();
			m_bRankClickFlag = TRUE;
		}
			
		for (int ii = 0; ii < vecRankList.size(); ii++)
		{
			SetAlarmData(ii, vecRankList[ii]);
			if (ii == 1000)
				break;
		}
		m_alarmRankCtrl.UnlockWindowUpdate();
	}
}
void CDlgAlarmHistory::ClickHistoryMode()
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDB_HISTORY:	
		ModeSelect(HistoryMode); 
		m_iModeNum = HistoryMode; 
		GetDlgItem(IDC_RANK_DY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RANK_NT)->ShowWindow(SW_HIDE);
		m_bRankClickFlag = FALSE;
		break;

	case IDB_RANK:		
		ModeSelect(RankMode);	
		m_iModeNum = RankMode; 
		GetDlgItem(IDC_RANK_DY)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RANK_NT)->ShowWindow(SW_SHOW);
		break;
	}
}

void CDlgAlarmHistory::ModeSelect(int iModeNum)
{
	if (iModeNum == HistoryMode)
	{
		GetDlgItem(IDC_ALARM_LIST_CTRL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ALARM_RANK_CTRL)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_ALARM_LIST_CTRL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ALARM_RANK_CTRL)->ShowWindow(SW_SHOW);
	}
}

void CDlgAlarmHistory::ClickRankType()
{
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_RANK_DY:	 m_iRankShiftType = Shift_DY;	break;
	case IDC_RANK_NT:	 m_iRankShiftType = Shift_NT;	break;
	}
	m_bRankClickFlag = FALSE;
	AlarmDataCheck();
}