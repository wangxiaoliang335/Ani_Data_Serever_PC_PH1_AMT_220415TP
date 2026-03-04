// ViewCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "ViewCtrl.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewCtrl dialog


CViewCtrl::CViewCtrl(CWnd* pParent /*=NULL*/)
	: CInitDialogBar() //CDialog(CViewCtrl::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewCtrl)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bClickBtn = false;
}

CViewCtrl::~CViewCtrl()
{
}

void CViewCtrl::DoDataExchange(CDataExchange* pDX)
{
	CInitDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewCtrl)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewCtrl, CInitDialogBar)
	//{{AFX_MSG_MAP(CViewCtrl)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewCtrl message handlers

BEGIN_EVENTSINK_MAP(CViewCtrl, CInitDialogBar)
    //{{AFX_EVENTSINK_MAP(CViewCtrl)
	ON_EVENT(CViewCtrl, IDC_END_BTN, -600 /* Click */, OnClickEndBtn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_UNLOADERVIEW_BTN, -600 /* Click */, ClickBottomBtn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_INSPDEFECT_BTN, -600 /* Click */, ClickBottomBtn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_COMVIEW_BTN, -600 /* Click */, ClickBottomBtn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_ADDRVIEW_BTN, -600 /* Click */, ClickBottomBtn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_MAINVIEW_BTN, -600 /* Click */, ClickBottomBtn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_PANEL_TEST, -600 /* Click */, ClickTestOn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_ANGLE_PASS, -600 /* Click */, ClickTestOn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_PG_PASS, -600 /* Click */, ClickTestOn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_AOI_PASS, -600 /* Click */, ClickTestOn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_TP_PASS, -600 /* Click */, ClickTestOn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_OP_PASS, -600 /* Click */, ClickTestOn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_BC_PASS, -600 /* Click */, ClickTestOn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_DFS_PASS, -600 /* Click */, ClickTestOn, VTS_NONE)
	ON_EVENT(CViewCtrl, IDC_LUMITOP_PASS, -600 /* Click */, ClickTestOn, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CViewCtrl::OnInitDialogBar()
{
	CInitDialogBar::OnInitDialogBar();

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDC_MAINVIEW_BTN);
	pBtnEnh->SetValue(TRUE);

#if _SYSTEM_GAMMA_
	GetDlgItem(IDC_UNLOADERVIEW_BTN)->ShowWindow(SW_HIDE);
#endif
	
	GetDlgItem(IDC_ANGLE_PASS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PG_PASS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_AOI_PASS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TP_PASS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_OP_PASS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TEST2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BC_PASS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DFS_PASS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LUMITOP_PASS)->ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CViewCtrl::OnClickEndBtn() 
{
	if (theApp.YesNoMsgBox(MS_YESNO, _T("Do you want to Exit?"), _T("Do you want to Exit?"), _T("Do you want to Exit?")) == IDYES)
	{
		theApp.m_pTraceLog->LOG_INFO(_T("-----------------------------------------------Program Exit-----------------------------------------------"));
		CAni_Data_Serever_PCApp* pApp = (CAni_Data_Serever_PCApp*)::AfxGetApp();
		CMainFrame* pMainFrame = (CMainFrame*)pApp->GetMainWnd();

		pMainFrame->PostMessage(WM_USER_CLOSE, 0, 0);
	}
}

void CViewCtrl::OnTimer(UINT_PTR nIDEvent)
{
	CInitDialogBar::OnTimer(nIDEvent);
}



void CViewCtrl::ClickBottomBtn()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CAni_Data_Serever_PCApp* pApp = (CAni_Data_Serever_PCApp*)::AfxGetApp();
	CMainFrame* pMainFrame = (CMainFrame*)pApp->GetMainWnd();

	int nBtnID, nViewID;
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	nBtnID = pBtnEnh->GetDlgCtrlID();

	if (theApp.m_iUserClass == USER_OPERATOR && !(nBtnID == IDC_MAINVIEW_BTN || nBtnID == IDC_ADDRVIEW_BTN || nBtnID == IDC_UNLOADERVIEW_BTN))
	{
		theApp.getMsgBox(MS_OK, _T("사용자는 이용 불가능합니다."), _T("Operator is not USE"), _T("操作员无法使用"));
		CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDC_MAINVIEW_BTN);
		pBtnEnh->SetValue(TRUE);
		nBtnID = IDC_MAINVIEW_BTN;
	}
	
	//if (theApp.m_PLCStatus && !(nBtnID == IDC_MAINVIEW_BTN || nBtnID == IDC_ADDRVIEW_BTN))
	//{
	//	theApp.getMsgBox(MS_OK, _T("Machine Auto Mode"), _T("Machine Auto Mode"), _T("Machine Auto Mode"));
	//	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDC_MAINVIEW_BTN);
	//	pBtnEnh->SetValue(TRUE);
	//	nBtnID = IDC_MAINVIEW_BTN;
	//}

	if (m_bClickBtn)
		return;
	else
		m_bClickBtn = true;

	switch (nBtnID)
	{
	case IDC_MAINVIEW_BTN:
		theApp.m_iUserClass = USER_OPERATOR;
		g_topCtrl->m_strLoginName.SetCaption(_T("LogIn :\nOperator"));
		theApp.LoginCheckMethod();
		nViewID = MAINVIEW;
		break;

	case IDC_COMVIEW_BTN:
		theApp.m_pComView->ChangeAlignList();
		nViewID = COMVIEW;
		break;
		
	case IDC_ADDRVIEW_BTN:
		nViewID = ADDRVIEW;
		break;

	case IDC_UNLOADERVIEW_BTN:
		nViewID = UNLOADERVIEW;
		break;

	case IDC_INSPDEFECT_BTN:
		nViewID = DEFECTCOUNTVIEW;
		break;

	default:
		m_bClickBtn = false; //130320 JSPark
		return;
	}

	pMainFrame->SwitchingView(nViewID);
	m_iOldViewCtrl = nBtnID;
	m_bClickBtn = false;
}

void CViewCtrl::ClickTestOn()
{
	int nBtnID;
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	nBtnID = pBtnEnh->GetDlgCtrlID();
	switch (nBtnID)
	{
	case IDC_PANEL_TEST:
		if (pBtnEnh->GetValue())
			theApp.m_PanelTestStart = TRUE;
		else
			theApp.m_PanelTestStart = FALSE;

		break;
	case IDC_ANGLE_PASS:
		if (pBtnEnh->GetValue())
			theApp.m_AnglePassMode = TRUE;
		else
			theApp.m_AnglePassMode = FALSE;

		break;
	case IDC_PG_PASS:
		if (pBtnEnh->GetValue())
			theApp.m_PgPassMode = TRUE;
		else
			theApp.m_PgPassMode = FALSE;

		break;
	case IDC_AOI_PASS:
		if (pBtnEnh->GetValue())
			theApp.m_AOIPassMode = TRUE;
		else
			theApp.m_AOIPassMode = FALSE;

		break;
	case IDC_TP_PASS:
		if (pBtnEnh->GetValue())
			theApp.m_TpPassMode = TRUE;
		else
			theApp.m_TpPassMode = FALSE;

		break;
	case IDC_OP_PASS:
		if (pBtnEnh->GetValue())
			theApp.m_OpvPassMode = TRUE;
		else
			theApp.m_OpvPassMode = FALSE;
		break;
	case IDC_BC_PASS:
		if (pBtnEnh->GetValue())
			theApp.m_bBCTestMode = TRUE;
		else
			theApp.m_bBCTestMode = FALSE;
		break;
	case IDC_DFS_PASS:
		if (pBtnEnh->GetValue())
			theApp.m_bDFSTestMode = TRUE;
		else
			theApp.m_bDFSTestMode = FALSE;
		break;
	case IDC_LUMITOP_PASS:
		if (pBtnEnh->GetValue())
			theApp.m_LumitopPassMode = TRUE;
		else
			theApp.m_LumitopPassMode = FALSE;
		break;
	}
}

void CViewCtrl::StringChanged()
{
	StringChnageMsg(IDC_MAINVIEW_BTN, _T("자동"), _T("Auto"), _T("自动"));
	StringChnageMsg(IDC_COMVIEW_BTN, _T("통신"), _T("Com"), _T("通信"));
	StringChnageMsg(IDC_ADDRVIEW_BTN, _T("주소"), _T("Addr"), _T("地址"));
	StringChnageMsg(IDC_UNLOADERVIEW_BTN, _T("언로더"), _T("Unloader"), _T("Unloader"));
	StringChnageMsg(IDC_END_BTN, _T("종료"), _T("Exit"), _T("退出"));
}

void CViewCtrl::StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi)
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

void CViewCtrl::SetTestButtonVisible()
{
	int nShowHide = SW_HIDE;
	if (theApp.m_iUserClass == USER_MAKER)
	{
		nShowHide = SW_SHOW;
	}

#if _SYSTEM_AMTAFT_
	GetDlgItem(IDC_PANEL_TEST)->ShowWindow(nShowHide);
	GetDlgItem(IDC_PG_PASS)->ShowWindow(nShowHide);
	GetDlgItem(IDC_BC_PASS)->ShowWindow(nShowHide);
	GetDlgItem(IDC_DFS_PASS)->ShowWindow(nShowHide);
	GetDlgItem(IDC_ANGLE_PASS)->ShowWindow(nShowHide);
	GetDlgItem(IDC_AOI_PASS)->ShowWindow(nShowHide);
	GetDlgItem(IDC_TP_PASS)->ShowWindow(nShowHide);
	GetDlgItem(IDC_OP_PASS)->ShowWindow(nShowHide);
	if (theApp.m_iMachineType == SetAMT)
		GetDlgItem(IDC_LUMITOP_PASS)->ShowWindow(SW_HIDE);
	else if (theApp.m_iMachineType == SetAFT)
		GetDlgItem(IDC_LUMITOP_PASS)->ShowWindow(nShowHide);
#else
	GetDlgItem(IDC_PANEL_TEST)->ShowWindow(nShowHide);
	GetDlgItem(IDC_PG_PASS)->ShowWindow(nShowHide);
	GetDlgItem(IDC_BC_PASS)->ShowWindow(nShowHide);
	GetDlgItem(IDC_DFS_PASS)->ShowWindow(nShowHide);
	GetDlgItem(IDC_ANGLE_PASS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_AOI_PASS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TP_PASS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_OP_PASS)->ShowWindow(SW_HIDE);
#endif
}