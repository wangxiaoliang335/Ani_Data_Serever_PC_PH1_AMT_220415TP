// MainViewAlarmHistoryPage.cpp : implementation file
//

#include "stdafx.h"
#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "DlgDefectCountHistory.h"
#include "afxdialogex.h"
#include "StringSupport.h"

// CDlgAlarmHistory dialog

IMPLEMENT_DYNAMIC(CDlgDefectCountHistory, CDialog)
CDlgDefectCountHistory::CDlgDefectCountHistory(int iShift, int iListType, CString strTitleName)
: CDialog(CDlgDefectCountHistory::IDD)
{
	m_iShift = iShift;
	m_iListType = iListType;
	m_strTitleName = strTitleName;
}

CDlgDefectCountHistory::~CDlgDefectCountHistory()
{
}

void CDlgDefectCountHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PANEL_DEFECT_COUNT_LIST, m_DefectCountList);
	DDX_Control(pDX, IDC_LABLE_DEFECT_LIST, m_btnLabelName);
}

BEGIN_EVENTSINK_MAP(CDlgDefectCountHistory, CDialog)
	ON_EVENT(CDlgDefectCountHistory, IDB_BTN_OK, DISPID_CLICK, CDlgDefectCountHistory::ClickBtnOk, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CDlgDefectCountHistory::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgDefectCountHistory::ClickBtnOk()
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	CDialog::OnOK();
}

BOOL CDlgDefectCountHistory::OnInitDialog()
{
	CDialog::OnInitDialog();

	switch (m_iListType)
	{
	case PanelDefectCount:
		m_btnLabelName.SetCaption(_T("PanelDefectCount"));
		m_DefectCountList.SetExtendedStyle(m_DefectCountList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
		m_DefectCountList.InsertColumn(0, L"", 0, 0);
		m_DefectCountList.InsertColumn(1, _T("No"), LVCFMT_CENTER, 50);
		m_DefectCountList.InsertColumn(2, _T("Time"), LVCFMT_CENTER, 140);
		m_DefectCountList.InsertColumn(3, _T("PanelID"), LVCFMT_CENTER, 270);
		m_DefectCountList.InsertColumn(4, _T("DefectResult"), LVCFMT_CENTER, 100);
		m_DefectCountList.InsertColumn(5, _T("Total\nMatch"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(6, _T("Total\nOverKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(7, _T("Total\nUnderKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(8, _T("Dot\nMath"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(9, _T("Dot\nOverKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(10, _T("Dot\nUnderKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(11, _T("Mura\nMath"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(12, _T("Mura\nOverKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(13, _T("Mura\nUnderKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(14, _T("List\nMath"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(15, _T("List\nOverKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(16, _T("List\nUnderKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(17, _T("Appearance\nMath"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(18, _T("Appearance\nOverKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(19, _T("Appearance\nUnderKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(20, _T("Function\nMath"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(21, _T("Function\nOverKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(22, _T("Function\nUnderKill"), LVCFMT_CENTER, 80);
		m_DefectCountList.InsertColumn(23, _T("OperatorID"), LVCFMT_CENTER, 150);
		break;
	case TitleDefectCount:
		//<< Dlg size change
		MoveWindow(250, 150, 680, 600);
		m_btnLabelName.MoveWindow(7, 7, 650, 32);
		GetDlgItem(IDB_BTN_OK)->MoveWindow(550, 500, 100, 61);
		//<<
		m_btnLabelName.SetCaption(m_strTitleName + _T("DefectCount"));
		m_DefectCountList.SetExtendedStyle(m_DefectCountList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
		m_DefectCountList.InsertColumn(0, L"", 0, 0);
		m_DefectCountList.InsertColumn(1, _T("No"), LVCFMT_CENTER, 50);
		m_DefectCountList.InsertColumn(2, _T("Desctiption"), LVCFMT_CENTER, 200);
		m_DefectCountList.InsertColumn(3, _T("DefectCode"), LVCFMT_CENTER, 100);
		m_DefectCountList.InsertColumn(4, _T("DefectGrade"), LVCFMT_CENTER, 100);
		m_DefectCountList.InsertColumn(5, _T("DefectResult"), LVCFMT_CENTER, 100);
		m_DefectCountList.InsertColumn(6, _T("Count"), LVCFMT_CENTER, 100);
		break;
	}

	m_DefectCountList.SetFontSize(17, 2.0, 17, 0);
	m_DefectCountList.CreateImageList(77, 35);

	CBitmap bmpEmpty, bmpClear;
	m_DefectCountList.GetCurImageList()->Add(&bmpEmpty, RGB(0, 0, 0));
	m_DefectCountList.SetMainHandle(this);
	m_DefectCountList.ShowSelectionBar(TRUE);
	m_DefectCountList.RedrawWindow();
		
	HistoryView(m_iShift, m_iListType, m_strTitleName);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgDefectCountHistory::SetItemText(int iRow, int iCol, CString strVal)
{
	if (m_DefectCountList.GetItemText(iRow, iCol) != strVal)
		m_DefectCountList.SetItemText(iRow, iCol, strVal);
}

void CDlgDefectCountHistory::SetItemInt(int iRow, int iCol, int strVal)
{
	CString msg;
	msg.Format(_T("%d"), strVal);

	if (m_DefectCountList.GetItemText(iRow, iCol) != msg)
		m_DefectCountList.SetItemText(iRow, iCol, msg);
}

void CDlgDefectCountHistory::HistoryView(int nShift, int iListType, CString strTitleName)
{
	map<CString, map<CString, int>>::iterator iter;
	vector<pair<CString, int>> vecSort;
	CStringArray responseTokens;
	DefectSumCountData DefectSumData;
	DefectSumData.Reset();
	vecSort.clear();

	if (iListType == PanelDefectCount)
	{
		theApp.InspctionDefectDataSum(theApp.m_SumDefectCountData[nShift], theApp.m_lastShiftIndex, DefectSumData);

		if (DefectSumData.m_TotalDefectSum == 0)
			return;
	}
	else
	{
		if (theApp.m_mapOpvDefectList[nShift].size() == 0)
			return;
	}

	m_DefectCountList.LockWindowUpdate();

	int jj = 0;
	if (iListType == PanelDefectCount)
	{
		for (auto Defect : theApp.m_VecDefectHistory[nShift])
		{
			SetPanelDefectCountData(jj, Defect);
			jj++;
		}
	}
	else
	{
		int ii = 0;
		iter = theApp.m_mapOpvDefectList[nShift].find(strTitleName);
		if (iter != theApp.m_mapOpvDefectList[nShift].end())
		{
			for (auto sortVec : iter->second)
				vecSort.push_back(make_pair(sortVec.first, sortVec.second));
			
			sort(vecSort.begin(), vecSort.end(), theApp.comp);

			for (auto list : vecSort)
			{
				responseTokens.RemoveAll();
				CStringSupport::GetTokenArray(list.first, _T(','), responseTokens);
				if (responseTokens.GetSize() > 1)
				{
					SetTitleDefectCountData(ii, responseTokens[0], responseTokens[1], responseTokens[2], list.second);
					ii++;
					if (ii == 1000)
						break;
				}
			}
		}
	}

	m_DefectCountList.UnlockWindowUpdate();
}

void CDlgDefectCountHistory::SetPanelDefectCountData(int index, DefectCountData PanelDefectData)
{
	while (m_DefectCountList.GetItemCount() <= index)
		m_DefectCountList.InsertItem(index, L"");

	SetItemText(index, 1, CStringSupport::N2C(index % 1000 + 1));
	SetItemText(index, 2, PanelDefectData.m_strTime);
	SetItemText(index, 3, PanelDefectData.m_strPanelID);
	SetItemText(index, 4, PanelDefectData.m_strOpvResult);
	SetItemInt(index, 5, PanelDefectData.m_iTotalMatch);
	SetItemInt(index, 6, PanelDefectData.m_iTotalOverKill);
	SetItemInt(index, 7, PanelDefectData.m_iTotalUnderKill);
	SetItemInt(index, 8, PanelDefectData.m_MatchDefectTotalSum[0]);
	SetItemInt(index, 9, PanelDefectData.m_OverKillDefectTotalSum[0]);
	SetItemInt(index, 10, PanelDefectData.m_UnderKillDefectTotalSum[0]);
	SetItemInt(index, 11, PanelDefectData.m_MatchDefectTotalSum[1]);
	SetItemInt(index, 12, PanelDefectData.m_OverKillDefectTotalSum[1]);
	SetItemInt(index, 13, PanelDefectData.m_UnderKillDefectTotalSum[1]);
	SetItemInt(index, 14, PanelDefectData.m_MatchDefectTotalSum[2]);
	SetItemInt(index, 15, PanelDefectData.m_OverKillDefectTotalSum[2]);
	SetItemInt(index, 16, PanelDefectData.m_UnderKillDefectTotalSum[2]);
	SetItemInt(index, 17, PanelDefectData.m_MatchDefectTotalSum[3]);
	SetItemInt(index, 18, PanelDefectData.m_OverKillDefectTotalSum[3]);
	SetItemInt(index, 19, PanelDefectData.m_UnderKillDefectTotalSum[3]);
	SetItemInt(index, 20, PanelDefectData.m_MatchDefectTotalSum[4]);
	SetItemInt(index, 21, PanelDefectData.m_OverKillDefectTotalSum[4]);
	SetItemInt(index, 22, PanelDefectData.m_UnderKillDefectTotalSum[4]);
	SetItemText(index, 23, PanelDefectData.m_strOperationID);
}

void CDlgDefectCountHistory::SetTitleDefectCountData(int index, CString strResult, CString strCode, CString strGrade, int iCount)
{
	while (m_DefectCountList.GetItemCount() <= index)
		m_DefectCountList.InsertItem(index, L"");

	SetItemText(index, 1, CStringSupport::N2C(index % 1000 + 1));
	SetItemText(index, 2, theApp.ParsingDefectDesctiption(strCode));
	SetItemText(index, 3, strCode);
	SetItemText(index, 4, strGrade);
	SetItemText(index, 5, strResult);
	SetItemText(index, 6, CStringSupport::FormatString(_T("%d"), iCount));
}
#endif