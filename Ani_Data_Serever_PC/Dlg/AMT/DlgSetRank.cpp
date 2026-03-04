// SetRank.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "DlgSetRank.h"
#include "afxdialogex.h"
#include "GetNumDlg.h"


// CDlgSetRank 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgSetRank, CDialogEx)

CDlgSetRank::CDlgSetRank(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSetRank::IDD, pParent)
{
	m_iFocusRowPosition = -1;
}

CDlgSetRank::~CDlgSetRank()
{
}

void CDlgSetRank::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_AOI_RANK_LIST, m_RankCtrlList);
	DDX_Control(pDX, IDB_BTN_AOI, m_btnAOIPart);
	DDX_Control(pDX, IDS_PLC_SEND_NUMBER, m_btnPlcSendNumber);
}


BEGIN_MESSAGE_MAP(CDlgSetRank, CDialogEx)
	ON_MESSAGE(WM_USER_LISTCTRLEX, ClickEvent)
END_MESSAGE_MAP()



// CDlgSetRank 메시지 처리기입니다.

BEGIN_EVENTSINK_MAP(CDlgSetRank, CDialogEx)


ON_EVENT(CDlgSetRank, IDC_RESETRANK, DISPID_CLICK, CDlgSetRank::ResetRankData, VTS_NONE)
ON_EVENT(CDlgSetRank, IDC_SAVERANK, DISPID_CLICK, CDlgSetRank::SaveChangedData, VTS_NONE)
ON_EVENT(CDlgSetRank, IDC_CHANGERANK, DISPID_CLICK, CDlgSetRank::ClickChange, VTS_NONE)
ON_EVENT(CDlgSetRank, IDC_QUITRANK, DISPID_CLICK, CDlgSetRank::OkayQuitFunction, VTS_NONE)
ON_EVENT(CDlgSetRank, IDB_BTN_AOI, DISPID_CLICK, CDlgSetRank::SelectPartFuncion, VTS_NONE)
ON_EVENT(CDlgSetRank, IDB_BTN_OPV, DISPID_CLICK, CDlgSetRank::SelectPartFuncion, VTS_NONE)
ON_EVENT(CDlgSetRank, IDC_RACK_ADD, DISPID_CLICK, CDlgSetRank::ClickRackAdd, VTS_NONE)
ON_EVENT(CDlgSetRank, IDC_RANK_DELETE, DISPID_CLICK, CDlgSetRank::ClickRankDelete, VTS_NONE)
ON_EVENT(CDlgSetRank, IDS_PLC_SEND_NUMBER, DISPID_CLICK, CDlgSetRank::ClickSetNumber, VTS_NONE)
END_EVENTSINK_MAP()


BOOL CDlgSetRank::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_RankCtrlList.SetExtendedStyle(m_RankCtrlList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_GRIDLINES);
	m_RankCtrlList.InsertColumn(0, L"", 0, 0);
	m_RankCtrlList.InsertColumn(1, _T("No"), LVCFMT_CENTER, 60);
	m_RankCtrlList.InsertColumn(2, _T("DEFECT\nCODE"), LVCFMT_CENTER, 100);
	m_RankCtrlList.InsertColumn(3, _T("DESCRIBE"), LVCFMT_CENTER, 400);

	m_RankCtrlList.SetFontSize(17, 2.0, 17, 0);
	m_RankCtrlList.CreateImageList(77, 35);

	CBitmap bmpEmpty, bmpClear;
	m_RankCtrlList.GetCurImageList()->Add(&bmpEmpty, RGB(0, 0, 0));
	m_RankCtrlList.SetMainHandle(this);
	m_RankCtrlList.ShowSelectionBar(TRUE);
	m_RankCtrlList.RedrawWindow();

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDB_BTN_AOI);
	pBtnEnh->SetValue(TRUE);
	m_btnAOIPart.SetValue(TRUE);
	m_nSelectParts = AOI;
	m_btnPlcSendNumber.SetWindowTextW(CStringSupport::FormatString(_T("%d"), theApp.m_iNumberSendToPlc));
	SelectPartFuncion();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgSetRank::ReLoadRank()
{	
	m_RankCtrlList.DeleteAllItems();
	CString strRankNo;
	int ii = 0;
	for (auto rankCode : theApp.m_strVecRank[m_nSelectParts])
	{
		m_RankCtrlList.InsertItem(ii, _T(""));
		strRankNo.Format(_T("%d"), ii + 1);
		m_RankCtrlList.SetItemText(ii, 1, strRankNo);
		m_RankCtrlList.SetItemText(ii, 2, rankCode.first);
		m_RankCtrlList.SetItemText(ii, 3, rankCode.second);
		ii++;
	}

	m_RankCtrlList.UnlockWindowUpdate();

}
void CDlgSetRank::SaveRank()
{
	theApp.m_strVecRank[m_nSelectParts].clear();
	CString strCode, strDescribe;
	for (int ii = 0; ii < m_RankCtrlList.GetItemCount(); ii++)
	{
		strCode = m_RankCtrlList.GetItemText(ii, 2);
		strDescribe = m_RankCtrlList.GetItemText(ii, 3);
		theApp.m_strVecRank[m_nSelectParts].push_back(make_pair(strCode, strDescribe));
	}

	EZIni ini(DATA_SYSTEM_SET_NGRANK_PATH);
	int ii = 0;
	CString strValue, strTemp;

	if (ini[RankIniTital[m_nSelectParts]].Exists())
		ini[RankIniTital[m_nSelectParts]].Delete();

	for (auto code : theApp.m_strVecRank[m_nSelectParts])
	{
		strTemp.Format(_T("%d"), ii);
		strValue.Format(_T("%s^%s"), code.first, code.second);
		ini[RankIniTital[m_nSelectParts]][strTemp] = strValue;
		ii++;
	}

	theApp.getMsgBox(MS_OK, _T("Save finish"), _T("Save finish"), _T("保存完成"));

}


void CDlgSetRank::ResetRankData()
{
	ReLoadRank();
}


void CDlgSetRank::SaveChangedData()
{
	SaveRank();
}

void CDlgSetRank::ClickChange()
{
	
	m_iFocusRowPosition = m_RankCtrlList.GetSelectionMark();
	
	if (m_iFocusRowPosition == -1){
		theApp.getMsgBox(MB_OK, _T("Please Select"), _T("Please Select"), _T("Please Select"));
		return;
	}

	CString strSelectCode,strSelectDescribe;

	if (m_iFocusRowPosition != -1){
		strSelectCode = m_RankCtrlList.GetItemText(m_iFocusRowPosition, 2);
		strSelectDescribe = m_RankCtrlList.GetItemText(m_iFocusRowPosition, 3);
	}

	CDlgAddRankCode dlg(ChangeRank, strSelectCode, strSelectDescribe);
	dlg.DoModal();
	CString strChangeRank = dlg.ReturnAddCode();
	if (strChangeRank==_T("")){
		strChangeRank = strSelectCode;
	}
	CString strChangeRankDescribe = dlg.ReturnAddDescribe();
	if (strChangeRankDescribe == _T("")){
		strChangeRankDescribe = strSelectDescribe;
	}

	m_RankCtrlList.SetItemText(m_iFocusRowPosition, 2, strChangeRank);
	m_RankCtrlList.SetItemText(m_iFocusRowPosition, 3, strChangeRankDescribe);
}


void CDlgSetRank::OkayQuitFunction()
{
	CDialog::OnOK();
}

void CDlgSetRank::StringChanged()
{
	StringChnageMsg(IDC_RESETRANK, _T("Reload"), _T("Reload"), _T("Reload"));
	StringChnageMsg(IDC_RACK_ADD, _T("Add"), _T("Add"), _T("增加"));
	StringChnageMsg(IDC_RANK_DELETE, _T("Delete"), _T("Delete"), _T("删除"));
	StringChnageMsg(IDC_CHANGERANK, _T("Change"), _T("Change"), _T("更改"));
	StringChnageMsg(IDC_SAVERANK, _T("Save"), _T("Save"), _T("保存"));
	StringChnageMsg(IDC_QUITRANK, _T("OK"), _T("OK"), _T("确定"));
}

void CDlgSetRank::StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi)
{
	CString msg;
	switch (theApp.m_iLanguageSelect)
	{
	case KOR:msg = strKor; break;
	case ENG:msg = strEng; break;
	case CHI:msg = strChi; break;
	}

	((CBtnEnh*)GetDlgItem(btn))->SetCaption(msg);
}

void CDlgSetRank::SelectPartFuncion()
{
	pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDB_BTN_AOI: m_nSelectParts = AOI; break;
	case IDB_BTN_OPV: m_nSelectParts = OPV; break;
	}

	ReLoadRank();
	UpdateData(FALSE);
}

void CDlgSetRank::ClickRackAdd()
{
	CDlgAddRankCode dlg(AddRank);

	if (dlg.DoModal() == DLG_OK)
	{
		CString strRankNo;
		CString strAddRank = dlg.ReturnAddCode();
		CString strAddDescribe = dlg.ReturnAddDescribe();

		if (strAddRank == _T("") || strAddDescribe == _T("")){
			theApp.getMsgBox(MB_OK, _T("Please Input Rank Code And Describe"), _T("Please Input Rank Code And Describe"), _T("Please Input Rank Code And Describe"));
			m_iFocusRowPosition = -1;
			return;
		}

		if (m_iFocusRowPosition == -1)
		{
			int nRowCount = m_RankCtrlList.GetItemCount();
			strRankNo.Format(_T("%d"), nRowCount + 1);

			m_RankCtrlList.InsertItem(nRowCount, _T(""));
			m_RankCtrlList.SetItemText(nRowCount, 1, strRankNo);
			m_RankCtrlList.SetItemText(nRowCount, 2, strAddRank);
			m_RankCtrlList.SetItemText(nRowCount, 3, strAddDescribe);
		}
		else
		{
			int nRowCount = m_iFocusRowPosition;
			strRankNo.Format(_T("%d"), nRowCount + 1);

			m_RankCtrlList.InsertItem(nRowCount, _T(""));
			m_RankCtrlList.SetItemText(nRowCount, 1, strRankNo);
			m_RankCtrlList.SetItemText(nRowCount, 2, strAddRank);
			m_RankCtrlList.SetItemText(nRowCount, 3, strAddDescribe);

			for (int ii = 0; ii < m_RankCtrlList.GetItemCount(); ii++)
			{
				strRankNo.Format(_T("%d"), ii + 1);
				m_RankCtrlList.SetItemText(ii, 1, strRankNo);
			}
		}

		m_iFocusRowPosition = -1;
	}
}


void CDlgSetRank::ClickRankDelete()
{
	CString strRankNo;
	
	m_iFocusRowPosition = m_RankCtrlList.GetSelectionMark();
	
	if (m_iFocusRowPosition == -1){
		theApp.getMsgBox(MB_OK,_T("Please Select"), _T("Please Select"), _T("Please Select"));
		return;
	}

	m_RankCtrlList.DeleteItem(m_iFocusRowPosition);

	for (int ii = 0; ii <m_RankCtrlList.GetItemCount(); ii++)
	{
		strRankNo.Format(_T("%d"), ii + 1);
		m_RankCtrlList.SetItemText(ii, 1, strRankNo);
	}

	m_RankCtrlList.SetSelectionMark(-1);
}

LRESULT CDlgSetRank::ClickEvent(WPARAM wParam, LPARAM lParam)
{
	CListCtrlEx *pParam = (CListCtrlEx*)wParam;
	m_iFocusRowPosition = pParam->m_nItem;

	return 0;
}
void CDlgSetRank::ClickSetNumber()
{
	EZIni ini(DATA_SYSTEM_SET_NGRANK_PATH);
	CGetNumDlg Dlg;

	if (Dlg.DoModal() == DLG_OK)
	{
		theApp.m_iNumberSendToPlc = _ttoi(Dlg.GetstrNum());

		if (theApp.m_iNumberSendToPlc == 0)
			return;

		if (theApp.m_iNumberSendToPlc > 16)
		{
			theApp.getMsgBox(MB_OK, _T("Maximum input value is 16"), _T("Maximum input value is 16"), _T("Maximum input value is 16"));
			return;
		}
		
		m_btnPlcSendNumber.SetWindowTextW(CStringSupport::FormatString(_T("%d"), theApp.m_iNumberSendToPlc));

		ini[_T("SYSTEM")][_T("PlcSendNumber")] = theApp.m_iNumberSendToPlc;
	}
}