// DefectView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "DefectView.h"
#include "SetTimerDlg.h"


// CDefectView

IMPLEMENT_DYNCREATE(CDefectView, CFormView)

CDefectView::CDefectView()
: CFormView(CDefectView::IDD)
, m_strPanelID(_T(""))
{
}

CDefectView::~CDefectView()
{
}

void CDefectView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_IMAGE_OP_MAP_VIEW1, m_ctrlImageMapView);
	DDX_Control(pDX, IDC_DEFECT_LIST_1, m_ctrlDefectList);
	DDX_Control(pDX, IDC_CMB_DEFECT_COMMAND, m_cmbDefectCommand);

	DDX_Control(pDX, IDB_BTN_AOIINFO, m_btnAOIINFO);
	DDX_Control(pDX, IDB_BTN_AOTINFO, m_btnAOTINFO);
	DDX_Control(pDX, IDB_BTN_VIEWINFO, m_btnVIEWINFO);
	DDX_Control(pDX, IDB_BTN_SUMINFO, m_btnSUMINFO);
	DDX_Text(pDX, IDC_PANEL_ID_DATA, m_strPanelID);
}

BEGIN_MESSAGE_MAP(CDefectView, CFormView)
	ON_CBN_SELCHANGE(IDC_CMB_DEFECT_COMMAND, &CDefectView::OnSelchangeCmbDefectCommand)
	ON_WM_DROPFILES()
	ON_EN_UPDATE(IDC_PANEL_ID_DATA, &CDefectView::OnEnUpdatePanelIdData)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDefectView, CFormView)
	ON_EVENT(CDefectView, IDB_BTN_AOIINFO, DISPID_CLICK, CDefectView::OnClickIdbBtnUnit, VTS_NONE)
	ON_EVENT(CDefectView, IDB_BTN_AOTINFO, DISPID_CLICK, CDefectView::OnClickIdbBtnUnit, VTS_NONE)
	ON_EVENT(CDefectView, IDB_BTN_VIEWINFO, DISPID_CLICK, CDefectView::OnClickIdbBtnUnit, VTS_NONE)
	ON_EVENT(CDefectView, IDB_BTN_SUMINFO, DISPID_CLICK, CDefectView::OnClickIdbBtnUnit, VTS_NONE)
	ON_EVENT(CDefectView, IDC_SEND_ALIGN_COMMAND, DISPID_CLICK, CDefectView::ClickSendAlignCommand, VTS_NONE)
END_EVENTSINK_MAP()


// CDefectView 진단입니다.

#ifdef _DEBUG
void CDefectView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDefectView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDefectView 메시지 처리기입니다.

void CDefectView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	strDfsPath = _T("");
	m_ctrlImageMapView.Initialize(this, IPL_DEPTH_8U, 3);
	m_ctrlImageMapView.Invalidate();
	bOpenFlag = FALSE;
	theApp.m_iDFSPartSelect = AOIdfs;

	m_cmbDefectCommand.EnableWindow(FALSE);
	m_btnAOIINFO.SetValue(TRUE);


	LV_COLUMN lvcolumn;
	DWORD dwStyle = m_ctrlDefectList.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;;
	m_ctrlDefectList.SetExtendedStyle(dwStyle);
	wchar_t *sList[4] = { _T(""), _T("List"), _T("Value") };
	int iWidth[3] = { 25, 135, 400 };

	for (int colIndex = 0; colIndex < 3; colIndex++)
	{
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = sList[colIndex];
		lvcolumn.iSubItem = colIndex;
		lvcolumn.cx = iWidth[colIndex];

		m_ctrlDefectList.InsertColumn(colIndex, &lvcolumn);
	}

	CString str;
	for (int ii = 0; ii < _MP_DEFECT_LABEL_PARAM_MAX; ii++)
	{
		str.Format(_T("%d"), ii + 1);
		m_ctrlDefectList.InsertItem(ii, str);
		m_ctrlDefectList.SetItemText(ii, 1, Defect_label[ii]);
	}

	SetComboBoxReadOnly(IDC_CMB_DEFECT_COMMAND);
	m_strPanelID = _T("");


}

void CDefectView::SetComboBoxReadOnly(int item)
{
	CWnd *p_combo = GetDlgItem(item);
	HWND h_wnd = ::FindWindowEx(p_combo->m_hWnd, NULL, _T("Edit"), NULL);
	if (NULL != h_wnd) ((CEdit *)CWnd::FromHandle(h_wnd))->SetReadOnly(TRUE);
}

BOOL CDefectView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CDefectView::GetPathMethod()
{
	TCHAR szFilter[] = _T("text(*.txt)|*.txt|allfile(*.*)|*.*||");
	CFileDialog fileopen(TRUE, _T("jpg"), NULL, 0, szFilter, this);
	CString strFilePath = _T("");
	bOpenFlag = FALSE;
	if (fileopen.DoModal() == IDOK){
		strFilePath = fileopen.GetPathName();
		m_strPanelID = fileopen.GetFileTitle();
		strDfsPath = strFilePath;
		//SetDlgItemText(IDC_EDIT1, strFilePath);
		if (strFilePath != _T("")){
			m_cmbDefectCommand.EnableWindow(TRUE);
			bOpenFlag = TRUE;
		}
		return TRUE;
	}

	else
		return FALSE;
}

//SECOND
void CDefectView::OnInitialList()
{
	//if (strDfsPath == _T("")){
	//	m_cmbDefectCommand.EnableWindow(FALSE);
	//	return;
	//}
	//if (m_vecDefectInfo.size() != 0){
	//	for (int ii = 0; ii < m_vecDefectInfo.size(); ii++)
	//		m_cmbDefectCommand.DeleteString(0);
	//}
	//
	//CString strFilePath;
	//switch (theApp.m_iDFSPartSelect)
	//{
	//case AOIdfs: strFilePath = DFS_SHARE_PATH + GetDateString2() + _T("\\") + m_strPanelID + _T("\\AOI\\") + m_strPanelID + _T(".txt"); break;
	//case VIEWdfs:strFilePath = DFS_SHARE_PATH + GetDateString2() + _T("\\") + m_strPanelID + _T("\\VIEWING\\") + m_strPanelID + _T(".txt"); break;
	//case SUMdfs: strFilePath = DFS_SHARE_PATH + GetDateString2() + _T("\\") + m_strPanelID + _T("\\") + m_strPanelID + _T(".txt"); break;
	//}
	//
	//OnSelchangeCmbDefectClean();
	//
	//if (!DfsInfo.LoadPanelDFSInfo(strFilePath, Machine_AOI))
	//{
	//	theApp.getMsgBox(MS_OK, _T("NO DFS INFO"), _T("NO DFS INFO"), _T("无 DFS信息"));
	//	return;
	//}
	//SetDefectCodeValue(DfsInfo);
	//
	//if (m_vecDefectInfo.size() == 0)
	//{
	//	theApp.getMsgBox(MS_OK, _T("NO DFS INFO"), _T("NO DFS INFO"), _T("无 DFS信息"));
	//	return;
	//}
	//
	//for (int ii = 0; ii < m_vecDefectInfo.size(); ii++)
	//	m_cmbDefectCommand.InsertString(0, m_vecDefectInfo[ii].strDefectCode);
	//
	//m_cmbDefectCommand.EnableWindow(TRUE);
	//
	//m_cmbDefectCommand.SetCurSel(0);
	//OnSelchangeCmbDefectCommand();
	//UpdateData(FALSE);
}

void CDefectView::SetDefectCodeValue(CDFSInfo DFS)
{

	m_vecDefectInfo = {};

	//for (auto&c : DFS.m_Panel_Defect)
	//{
	//	defectinfo.iPanel_Size_h = 1080;
	//	defectinfo.iPanel_Size_w = 1920;
	//	defectinfo.strPanelID = c.strPANEL_ID;
	//
	//	defectinfo.strDefectCode = c.strAT_DEFECT_CODE;
	//	defectinfo.bRealNG = TRUE;
	//	defectinfo.ptPos.x = _ttof(c.strX_START);
	//	defectinfo.ptPos.y = _ttof(c.strY_START);
	//
	//
	//	m_vecDefectInfo.push_back(defectinfo);
	//}

}

void CDefectView::OnSelchangeCmbDefectCommand()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCmbValue = m_cmbDefectCommand.GetCurSel();

	CString msg;
	msg.Format(_T("%d"), iCmbValue + 1);

	m_ctrlDefectList.SetItemText(0, 2, msg);
	m_ctrlDefectList.SetItemText(1, 2, m_vecDefectInfo[iCmbValue].strDefectCode);

	msg.Format(_T("%d"), m_vecDefectInfo[iCmbValue].ptPos.x);
	m_ctrlDefectList.SetItemText(3, 2, msg);
	msg.Format(_T("%d"), m_vecDefectInfo[iCmbValue].ptPos.y);
	m_ctrlDefectList.SetItemText(4, 2, msg);

	m_ctrlImageMapView.UpdateDefectPos(m_vecDefectInfo[iCmbValue]);
}

void CDefectView::OnSelchangeCmbDefectClean()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCmbValue = m_cmbDefectCommand.GetCurSel();

	CString msg = _T("");

	m_ctrlDefectList.SetItemText(0, 2, msg);
	m_ctrlDefectList.SetItemText(1, 2, msg);
	m_ctrlDefectList.SetItemText(3, 2, msg);
	m_ctrlDefectList.SetItemText(4, 2, msg);

}

void CDefectView::OnClickIdbBtnUnit()
{
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDB_BTN_AOIINFO: theApp.m_iDFSPartSelect = AOIdfs; break;
	case IDB_BTN_VIEWINFO: theApp.m_iDFSPartSelect = VIEWdfs; break;
	case IDB_BTN_SUMINFO: theApp.m_iDFSPartSelect = SUMdfs; break;
	}
	OnInitialList();
	UpdateData(FALSE);
}


void CDefectView::ClickSendAlignCommand()
{
	UpdateData(TRUE);

	if (GetPathMethod())
	{
		OnInitialList();
	}
	else return;
	UpdateData(FALSE);

}


void CDefectView::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	UINT nCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	for (int i = 0; i < nCount; i++)
	{
		DragQueryFile(hDropInfo, 0, szPath, MAX_PATH);
	}

	DragFinish(hDropInfo);
	m_strPanelID = szPath;

}


void CDefectView::OnEnUpdatePanelIdData()
{
	CString strInput;

	GetDlgItemText(IDC_PANEL_ID_DATA, strInput);
	if (strInput.GetLength() > 1){
		if (strInput.GetAt(strInput.GetLength() - 1) == '/n'){
			m_strPanelID = strInput;
			m_cmbDefectCommand.EnableWindow(TRUE);
			OnInitialList();
			UpdateData(FALSE);
		}

	}
}