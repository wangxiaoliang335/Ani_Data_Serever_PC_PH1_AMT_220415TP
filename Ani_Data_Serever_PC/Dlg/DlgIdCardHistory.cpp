// MainViewAlarmHistoryPage.cpp : implementation file
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "DlgIdCardHistory.h"
#include "afxdialogex.h"
#include "StringSupport.h"
#include "UserList.h"

// CDlgIdCardHistory dialog

IMPLEMENT_DYNAMIC(CDlgIdCardHistory, CDialog)

CDlgIdCardHistory::CDlgIdCardHistory(CWnd* pParent /*=NULL*/)
: CDialog(CDlgIdCardHistory::IDD, pParent)
{
}

CDlgIdCardHistory::~CDlgIdCardHistory()
{
}

void CDlgIdCardHistory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ID_CARD_READER_LIST_CTRL, m_IdCardHistory);
}

BEGIN_MESSAGE_MAP(CDlgIdCardHistory, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgIdCardHistory, CDialog)
	ON_EVENT(CDlgIdCardHistory, IDB_BTN_USER_LIST, DISPID_CLICK, CDlgIdCardHistory::ClickBtnUserList, VTS_NONE)
	ON_EVENT(CDlgIdCardHistory, IDB_BTN_CUR_LIST, DISPID_CLICK, CDlgIdCardHistory::ClickBtnCurList, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CDlgIdCardHistory::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDlgIdCardHistory::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_IdCardHistory.SetExtendedStyle(m_IdCardHistory.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_IdCardHistory.InsertColumn(0, L"", 0, 0);
	m_IdCardHistory.InsertColumn(1, _T("No"), LVCFMT_CENTER, 60);
	m_IdCardHistory.InsertColumn(2, _T("Occurrence\nTime"), LVCFMT_CENTER, 200);
	m_IdCardHistory.InsertColumn(3, _T("User\nLevel"), LVCFMT_CENTER, 50);
	m_IdCardHistory.InsertColumn(4, _T("User\nName"), LVCFMT_LEFT, 190);
	m_IdCardHistory.InsertColumn(5, _T("User\nID"), LVCFMT_LEFT, 200);
	m_IdCardHistory.InsertColumn(6, _T("User\nCardNo"), LVCFMT_LEFT, 200);
	m_IdCardHistory.InsertColumn(7, _T("User\nDivision"), LVCFMT_LEFT, 110);
	m_IdCardHistory.InsertColumn(8, _T("User\nLogin/Out"), LVCFMT_LEFT, 110);

	m_IdCardHistory.SetFontSize(17, 2.0, 17, 0);
	m_IdCardHistory.CreateImageList(77, 35);

	CBitmap bmpEmpty, bmpClear;
	m_IdCardHistory.GetCurImageList()->Add(&bmpEmpty, RGB(0, 0, 0));
	m_IdCardHistory.SetMainHandle(this);
	m_IdCardHistory.ShowSelectionBar(TRUE);
	m_IdCardHistory.RedrawWindow();

	SetTimer(0, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CDlgIdCardHistory::SetCardReaderData(int index, IDCardReader pIDCardReader)
{
	while (m_IdCardHistory.GetItemCount() <= index)
		m_IdCardHistory.InsertItem(index, L"");
	
	SetItemText(index, 1, CStringSupport::N2C(index % 1000 + 1));
	SetItemText(index, 2, pIDCardReader.m_strLogintTime);
	SetItemText(index, 3, pIDCardReader.m_strLevel);
	SetItemText(index, 4, pIDCardReader.m_strUserName);
	SetItemText(index, 5, pIDCardReader.m_strUserID);
	SetItemText(index, 6, pIDCardReader.m_strIDCardNo);
	SetItemText(index, 7, pIDCardReader.m_strDivision);
	SetItemText(index, 8, pIDCardReader.m_strLoginOut);
}

void CDlgIdCardHistory::SetItemText(int iRow, int iCol, CString strVal)
{
	if (m_IdCardHistory.GetItemText(iRow, iCol) != strVal)
		m_IdCardHistory.SetItemText(iRow, iCol, strVal);
}


void CDlgIdCardHistory::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	if (theApp.m_LoginOutData.size() == 0)
		return;
	
	m_IdCardHistory.LockWindowUpdate();
	
	for (int ii = 0; ii < theApp.m_LoginOutData.size(); ii++)
	{
		SetCardReaderData(ii, theApp.m_LoginOutData[ii]);
		if (ii == 1000)
			break;
	}
	
	m_IdCardHistory.UnlockWindowUpdate();
}

void CDlgIdCardHistory::ClickBtnUserList()
{
	CUserList dlg(UserList);
	dlg.DoModal();
}


void CDlgIdCardHistory::ClickBtnCurList()
{
	CUserList dlg(UserCurrentList);
	dlg.DoModal();
}
