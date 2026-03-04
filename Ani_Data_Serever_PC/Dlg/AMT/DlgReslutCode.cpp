// DlgAlignStatus.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "DlgReslutCode.h"
#include "afxdialogex.h"


// CDlgReslutCode 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgReslutCode, CDialog)

CDlgReslutCode::CDlgReslutCode(CString strTypeName, int iShift)
: CDialog(CDlgReslutCode::IDD)
{
	m_strInspName = strTypeName;
	m_iShift = iShift;
	m_iSelectZone = MaxZone;
}

CDlgReslutCode::~CDlgReslutCode()
{
}

void CDlgReslutCode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_RESULT_CODE_RANK_LIST_CTRL, m_ResultCodeRankListCtrl);
	DDX_Control(pDX, IDC_RESULT_CODE_LIST_CTRL, m_ResultCodeListCtrl);
	DDX_Control(pDX, IDC_RESULT_CODE_TITLE, m_ctrTitleName);
}


BEGIN_MESSAGE_MAP(CDlgReslutCode, CDialog)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgReslutCode, CDialog)
	ON_EVENT(CDlgReslutCode, IDB_BTN_OK, DISPID_CLICK, CDlgReslutCode::OnOK, VTS_NONE)
	ON_EVENT(CDlgReslutCode, IDB_BTN_ALL, DISPID_CLICK, CDlgReslutCode::ClickSelectBtn, VTS_NONE)
	ON_EVENT(CDlgReslutCode, IDB_BTN_AZONE, DISPID_CLICK, CDlgReslutCode::ClickSelectBtn, VTS_NONE)
	ON_EVENT(CDlgReslutCode, IDB_BTN_BZONE, DISPID_CLICK, CDlgReslutCode::ClickSelectBtn, VTS_NONE)
	ON_EVENT(CDlgReslutCode, IDB_BTN_CZONE, DISPID_CLICK, CDlgReslutCode::ClickSelectBtn, VTS_NONE)
	ON_EVENT(CDlgReslutCode, IDB_BTN_DZONE, DISPID_CLICK, CDlgReslutCode::ClickSelectBtn, VTS_NONE)
	ON_EVENT(CDlgReslutCode, IDC_HISTORY_MODE, DISPID_CLICK, CDlgReslutCode::ClickHistoryMode, VTS_NONE)
	ON_EVENT(CDlgReslutCode, IDC_RANK_MODE, DISPID_CLICK, CDlgReslutCode::ClickHistoryMode, VTS_NONE)
END_EVENTSINK_MAP()
// CDlgReslutCode 메시지 처리기입니다.


BOOL CDlgReslutCode::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ResultCodeRankListCtrl.SetExtendedStyle(m_ResultCodeRankListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_ResultCodeRankListCtrl.InsertColumn(0, L"", 0, 0);
	m_ResultCodeRankListCtrl.InsertColumn(1, _T("Zone"), LVCFMT_CENTER, 80);
	m_ResultCodeRankListCtrl.InsertColumn(2, _T("Ch"), LVCFMT_CENTER, 60);
	m_ResultCodeRankListCtrl.InsertColumn(3, _T("Code"), LVCFMT_CENTER, 300);
	m_ResultCodeRankListCtrl.InsertColumn(4, _T("Desctiption"), LVCFMT_CENTER, 290);
	m_ResultCodeRankListCtrl.InsertColumn(5, _T("Count"), LVCFMT_CENTER, 170);

	m_ResultCodeRankListCtrl.SetFontSize(17, 2.0, 17, 0);
	m_ResultCodeRankListCtrl.CreateImageList(77, 35);

	CBitmap bmpEmpty, bmpClear;
	m_ResultCodeRankListCtrl.GetCurImageList()->Add(&bmpEmpty, RGB(0, 0, 0));
	m_ResultCodeRankListCtrl.SetMainHandle(this);
	m_ResultCodeRankListCtrl.ShowSelectionBar(TRUE);
	m_ResultCodeRankListCtrl.RedrawWindow();

	m_ResultCodeListCtrl.SetExtendedStyle(m_ResultCodeListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_ResultCodeListCtrl.InsertColumn(0, L"", 0, 0);
	m_ResultCodeListCtrl.InsertColumn(1, _T("Zone"), LVCFMT_CENTER, 80);
	m_ResultCodeListCtrl.InsertColumn(2, _T("Ch"), LVCFMT_CENTER, 60);
	m_ResultCodeListCtrl.InsertColumn(3, _T("PanelID"), LVCFMT_CENTER, 300);
	m_ResultCodeListCtrl.InsertColumn(4, _T("Code"), LVCFMT_CENTER, 170);
	m_ResultCodeListCtrl.InsertColumn(5, _T("Desctiption"), LVCFMT_CENTER, 290);

	m_ResultCodeListCtrl.SetFontSize(17, 2.0, 17, 0);
	m_ResultCodeListCtrl.CreateImageList(77, 35);

	m_ResultCodeListCtrl.GetCurImageList()->Add(&bmpEmpty, RGB(0, 0, 0));
	m_ResultCodeListCtrl.SetMainHandle(this);
	m_ResultCodeListCtrl.ShowSelectionBar(TRUE);
	m_ResultCodeListCtrl.RedrawWindow();

	m_ctrTitleName.SetCaption(m_strInspName);

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDB_BTN_ALL);
	pBtnEnh->SetValue(TRUE);

	pBtnEnh = (CBtnEnh*)GetDlgItem(IDC_HISTORY_MODE);
	pBtnEnh->SetValue(TRUE);

	map<CString, vector<pair<CString, ResultCodeRank>>>::iterator Historyiter;
	vector<pair<CString, int>> vecCode;
	vecCode.clear();

	Historyiter = theApp.m_mapRankTotalList[m_iShift].find(m_strInspName);
	if (Historyiter != theApp.m_mapRankTotalList[m_iShift].end())
	{
		int index = 0;
		for (auto History : Historyiter->second)
		{
			SetHistoryReslutCode(index, History);
			index++;
		}
	}
	else
		return TRUE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgReslutCode::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnOK();
}

void CDlgReslutCode::SetHistoryReslutCode(int index, pair<CString, ResultCodeRank> code)
{
	while (m_ResultCodeListCtrl.GetItemCount() <= index)
		m_ResultCodeListCtrl.InsertItem(index, L"");

	SetItemText(index, 1, code.second.m_strZone);
	SetItemText(index, 2, CStringSupport::FormatString(_T("%d"), code.second.m_iCh));
	SetItemText(index, 3, code.first);
	SetItemText(index, 4, code.second.m_strResultCode);
	SetItemText(index, 5, theApp.ParsingDefectDesctiption(code.second.m_strResultCode));

	m_ResultCodeListCtrl.UnlockWindowUpdate();
}

void CDlgReslutCode::SetRankReslutCode(int index, pair<CString, int> code)
{
	ResultCodeRank RankData;
	CStringArray responseTokens;

	while (m_ResultCodeRankListCtrl.GetItemCount() <= index)
		m_ResultCodeRankListCtrl.InsertItem(index, L"");

	CStringSupport::GetTokenArray(code.first, _T('^'), responseTokens);

	RankData.m_strZone = responseTokens[0];
	RankData.m_iCh = _ttoi(responseTokens[1]);
	RankData.m_strResultCode = responseTokens[2];

	SetItemRankText(index, 1, RankData.m_strZone);
	SetItemRankText(index, 2, CStringSupport::FormatString(_T("%d"), RankData.m_iCh));
	SetItemRankText(index, 3, RankData.m_strResultCode);
	SetItemRankText(index, 4, theApp.ParsingDefectDesctiption(RankData.m_strResultCode));
	SetItemRankText(index, 5, CStringSupport::FormatString(_T("%d"), code.second));

	m_ResultCodeRankListCtrl.UnlockWindowUpdate();
}

void CDlgReslutCode::SetItemText(int iRow, int iCol, CString strVal)
{
	if (m_ResultCodeListCtrl.GetItemText(iRow, iCol) != strVal)
		m_ResultCodeListCtrl.SetItemText(iRow, iCol, strVal);
}

void CDlgReslutCode::SetItemRankText(int iRow, int iCol, CString strVal)
{
	if (m_ResultCodeRankListCtrl.GetItemText(iRow, iCol) != strVal)
		m_ResultCodeRankListCtrl.SetItemText(iRow, iCol, strVal);
}

void CDlgReslutCode::ClickSelectBtn()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDB_BTN_ALL:	SelectZoneResultCode(MaxZone, m_iModeNum);	break;
	case IDB_BTN_AZONE:	SelectZoneResultCode(AZone, m_iModeNum);	break;
	case IDB_BTN_BZONE:	SelectZoneResultCode(BZone, m_iModeNum);	break;
	case IDB_BTN_CZONE:	SelectZoneResultCode(CZone, m_iModeNum);	break;
	case IDB_BTN_DZONE:	SelectZoneResultCode(DZone, m_iModeNum);	break;
	}
}

void CDlgReslutCode::ClickHistoryMode()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDC_HISTORY_MODE:	ModeSelect(HistoryMode); m_iModeNum = HistoryMode;  break;
	case IDC_RANK_MODE:		ModeSelect(RankMode);	 m_iModeNum = RankMode;     break;
	}
}

void CDlgReslutCode::ModeSelect(int iModeNum)
{
	if (iModeNum == HistoryMode)
	{
		GetDlgItem(IDC_RESULT_CODE_LIST_CTRL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RESULT_CODE_RANK_LIST_CTRL)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_RESULT_CODE_LIST_CTRL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RESULT_CODE_RANK_LIST_CTRL)->ShowWindow(SW_SHOW);
	}

	SelectZoneResultCode(m_iSelectZone, iModeNum);
}


void CDlgReslutCode::SelectZoneResultCode(int iIndexNum, int iModeNum)
{
	map<CString, vector<pair<CString, ResultCodeRank>>>::iterator Historyiter;
	map<CString, map<CString, int>>::iterator Rankiter;
	vector<pair<CString, int>> vecCode;

	CString strContents, strZone;
	CStringArray responseTokens;

	m_iSelectZone = iIndexNum;
	int index = 0;

	if (iModeNum == HistoryMode)
	{
		m_ResultCodeListCtrl.DeleteAllItems();

		Historyiter = theApp.m_mapRankTotalList[m_iShift].find(m_strInspName);
		if (Historyiter != theApp.m_mapRankTotalList[m_iShift].end())
		{
			for (auto History : Historyiter->second)
			{
				if (iIndexNum == MaxZone)
				{
					SetHistoryReslutCode(index, History);
					index++;
				}
				else
				{
					if (History.second.m_strZone == PG_IndexName[iIndexNum])
					{
						SetHistoryReslutCode(index, History);
						index++;
					}
				}
			}
		}
	}
	else
	{
		m_ResultCodeRankListCtrl.DeleteAllItems();
		vecCode.clear();

		Rankiter = theApp.m_mapRankCodeCount[m_iShift].find(m_strInspName);
		if (Rankiter != theApp.m_mapRankCodeCount[m_iShift].end())
		{
			for (auto Rank : Rankiter->second)
				vecCode.push_back(make_pair(Rank.first, Rank.second));

			sort(vecCode.begin(), vecCode.end(), theApp.comp);

			for (auto Rank : vecCode)
			{
				if (iIndexNum == MaxZone)
				{
					SetRankReslutCode(index, Rank);
					index++;
				}
				else
				{
					responseTokens.RemoveAll();
					strContents = _T("");
					strZone = _T("");

					strContents = Rank.first;
					CStringSupport::GetTokenArray(strContents, _T('^'), responseTokens);

					strZone = responseTokens[0];
					if (responseTokens[0] == PG_IndexName[iIndexNum])
					{
						SetRankReslutCode(index, Rank);
						index++;
					}
				}
			}
		}
	}
}
#endif