// TopCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "Ani_Data_Serever_PCDoc.h"
#include "Ani_Data_Serever_PCView.h"
#include "MainFrm.h"
#include <assert.h>
#include "TopCtrl.h"
#include "BtnEnhReadability.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTopCtrl dialog

CTopCtrl *g_topCtrl;
CTopCtrl::CTopCtrl(CWnd* pParent /*=NULL*/)
	: CInitDialogBar()
{
	g_topCtrl = this;
}

CTopCtrl::~CTopCtrl()
{
	delete m_LoginDlg;
}

void CTopCtrl::DoDataExchange(CDataExchange* pDX)
{
	CInitDialogBar::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TITLE_LABEL, m_TitleName);
	DDX_Control(pDX, IDC_TIME_LABEL, m_CurrentTime);
	DDX_Control(pDX, IDC_WORK_MODEL_NAME_LABEL, m_ctrlModelName);
	DDX_Control(pDX, IDC_VERSION_LABEL, m_ctrlVersion);
	DDX_Control(pDX, IDC_LOGIN_BTN, m_strLoginName);

#if _SYSTEM_AMTAFT_
	DDX_Control(pDX, IDC_NETWORK_IF_1, m_netWorkIf[NUM_NETWORK_VISION_1]);
	DDX_Control(pDX, IDC_NETWORK_IF_2, m_netWorkIf[NUM_NETWORK_VISION_2]);
	DDX_Control(pDX, IDC_NETWORK_IF_3, m_netWorkIf[NUM_NETWORK_ALIGN]);
	DDX_Control(pDX, IDC_NETWORK_IF_4, m_netWorkIf[NUM_NETWORK_PLC]);
	DDX_Control(pDX, IDC_NETWORK_IF_5, m_netWorkIf[NUM_NETWORK_VIEWING_ANGLE_1]); // AMT = Viewing, AFT = Lumitop
	DDX_Control(pDX, IDC_NETWORK_IF_9, m_netWorkIf[NUM_NETWORK_VIEWING_ANGLE_2]); // AMT = Viewing, AFT = Lumitop
	DDX_Control(pDX, IDC_NETWORK_IF_6, m_netWorkIf[NUM_NETWORK_PG1]);
	DDX_Control(pDX, IDC_NETWORK_IF_7, m_netWorkIf[NUM_NETWORK_TP_1]);
	DDX_Control(pDX, IDC_NETWORK_IF_25, m_netWorkIf[NUM_NETWORK_PG2]);
	DDX_Control(pDX, IDC_NETWORK_IF_26, m_netWorkIf[NUM_NETWORK_PG3]);
	DDX_Control(pDX, IDC_NETWORK_IF_27, m_netWorkIf[NUM_NETWORK_OPV_1]);
	DDX_Control(pDX, IDC_NETWORK_IF_28, m_netWorkIf[NUM_NETWORK_OPV_2]);
#else
	DDX_Control(pDX, IDC_NETWORK_IF_1, m_netWorkIf[NUM_NETWORK_ALIGN]);
	DDX_Control(pDX, IDC_NETWORK_IF_2, m_netWorkIf[NUM_NETWORK_PLC]);
	DDX_Control(pDX, IDC_NETWORK_IF_3, m_netWorkIf[NUM_NETWORK_PG1]);
	DDX_Control(pDX, IDC_NETWORK_IF_4, m_netWorkIf[NUM_NETWORK_PG2]);
#endif
}

void CTopCtrl::CloseView()
{
}

BEGIN_MESSAGE_MAP(CTopCtrl, CInitDialogBar)
	//{{AFX_MSG_MAP(CTopCtrl)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTopCtrl message handlers
BOOL CTopCtrl::OnInitDialogBar()
{
	//MainFrame Pointer ??? ??????. 
	pMainFrame = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	CInitDialogBar::OnInitDialogBar();

	pMainFrame = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	m_ctrlVersion.SetCaption(CStringSupport::FormatString(_T("Ver. 22.03.10_AMT#4")));

#if _SYSTEM_AMTAFT_
	if (theApp.m_iMachineType == SetAMT)
		m_TitleName.SetWindowText(_T("CSOT AMT"));
	else
		m_TitleName.SetWindowText(_T("CSOT AFT"));
#else
	m_TitleName.SetWindowText(_T("CSOT GAMMA"));
#endif

#if _SYSTEM_AMTAFT_
	if (theApp.m_iMachineType == SetAMT)
	{
		m_netWorkIf[NUM_NETWORK_VIEWING_ANGLE_1].SetWindowText(_T("Viewing1"));
		m_netWorkIf[NUM_NETWORK_VIEWING_ANGLE_2].SetWindowText(_T("Viewing2"));
	}
	else
	{
		m_netWorkIf[NUM_NETWORK_VIEWING_ANGLE_1].SetWindowText(_T("Lumitop1"));
		m_netWorkIf[NUM_NETWORK_VIEWING_ANGLE_2].SetWindowText(_T("Lumitop2"));
	}
#endif

	m_strLoginName.SetCaption(_T("LogIn :\nOperator"));

	ApplyBtnEnhReadabilityStyle(m_TitleName);
	ApplyBtnEnhReadabilityStyle(m_CurrentTime);
	ApplyBtnEnhReadabilityStyle(m_ctrlModelName);
	ApplyBtnEnhReadabilityStyle(m_ctrlVersion);
	ApplyBtnEnhReadabilityStyle(m_strLoginName);
#if _SYSTEM_AMTAFT_
	for (int ii = 0; ii < NUM_NETWORK_ICON; ++ii)
	{
		if (m_netWorkIf[ii].GetSafeHwnd())
			ApplyBtnEnhReadabilityStyle(m_netWorkIf[ii]);
	}
	ApplyBtnEnhReadabilityById(this, IDC_BTNENHCTRL2);
#else
	static const int s_gammaNetIdx[] = {
		NUM_NETWORK_ALIGN, NUM_NETWORK_PLC, NUM_NETWORK_PG1, NUM_NETWORK_PG2,
	};
	for (int idx : s_gammaNetIdx)
	{
		if (m_netWorkIf[idx].GetSafeHwnd())
			ApplyBtnEnhReadabilityStyle(m_netWorkIf[idx]);
	}
#endif

	m_LoginDlg = new CDlgLogin;
	m_LoginDlg->Create(IDD_DLG_LOG_IN, this);
	m_LoginDlg->ShowWindow(SW_HIDE);

	SetTimer(_TIMER_DISPLAY_TIME, 500, NULL);

	

	return true;
}

void CTopCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	if (nIDEvent == _TIMER_DISPLAY_TIME){
		m_tCurTime = CTime::GetCurrentTime();
		m_strDispTime = m_tCurTime.Format("[%Y-%m-%d] %H:%M:%S %p");
		m_CurrentTime.SetCaption(m_strDispTime);
	}

	for (int ii = 0; ii < _ttoi(theApp.m_strAlignCount); ii++)
		if (theApp.m_AlignConectStatus[ii] == FALSE)
		{
			m_bAlignFlag = FALSE;
			break;
		}
		else
			m_bAlignFlag = TRUE;
			
		

#if _SYSTEM_AMTAFT_
	m_netWorkIf[NUM_NETWORK_VISION_1].SetValue(theApp.m_VisionConectStatus[PC1]);
	m_netWorkIf[NUM_NETWORK_VISION_2].SetValue(theApp.m_VisionConectStatus[PC2]);

	m_netWorkIf[NUM_NETWORK_ALIGN].SetValue(m_bAlignFlag);
	m_netWorkIf[NUM_NETWORK_PLC].SetValue(theApp.m_PlcConectStatus);

	if (theApp.m_iMachineType == SetAMT)
	{
		bFlag = theApp.m_ViewingAngleConectStatus[PanelNum1] && theApp.m_ViewingAngleConectStatus[PanelNum2];
		bFlag2 = theApp.m_ViewingAngleConectStatus[PanelNum3] && theApp.m_ViewingAngleConectStatus[PanelNum4];
		m_netWorkIf[NUM_NETWORK_VIEWING_ANGLE_1].SetValue(bFlag);
		m_netWorkIf[NUM_NETWORK_VIEWING_ANGLE_2].SetValue(bFlag2);
	}
	else
	{
		m_netWorkIf[NUM_NETWORK_VIEWING_ANGLE_1].SetValue(theApp.m_LumitopConectStatus[PC1]);
		m_netWorkIf[NUM_NETWORK_VIEWING_ANGLE_2].SetValue(theApp.m_LumitopConectStatus[PC2]);
	}

	m_netWorkIf[NUM_NETWORK_PG1].SetValue(theApp.m_PgConectStatus[PgServer_1]);
	m_netWorkIf[NUM_NETWORK_PG2].SetValue(theApp.m_PgConectStatus[PgServer_2]);
	m_netWorkIf[NUM_NETWORK_PG3].SetValue(theApp.m_PgConectStatus[PgServer_3]);
	m_netWorkIf[NUM_NETWORK_TP_1].SetValue(theApp.m_TpConectStatus);

	m_netWorkIf[NUM_NETWORK_OPV_1].SetValue(theApp.m_OpvConectStatus[CH_1]);
	m_netWorkIf[NUM_NETWORK_OPV_2].SetValue(theApp.m_OpvConectStatus[CH_2]);
#else
	m_netWorkIf[NUM_NETWORK_ALIGN].SetValue(m_bAlignFlag);
	m_netWorkIf[NUM_NETWORK_PLC].SetValue(theApp.m_PlcConectStatus);
	m_netWorkIf[NUM_NETWORK_PG1].SetValue(theApp.m_PgConectStatus[PgServer_1]);
	m_netWorkIf[NUM_NETWORK_PG2].SetValue(theApp.m_PgConectStatus[PgServer_2]);
#endif

	UpdateData(FALSE);

	CInitDialogBar::OnTimer(nIDEvent);
}

BEGIN_EVENTSINK_MAP(CTopCtrl, CInitDialogBar)
	ON_EVENT(CTopCtrl, IDC_LOGIN_BTN, DISPID_CLICK, CTopCtrl::ClickLogIn, VTS_NONE)
END_EVENTSINK_MAP()


void CTopCtrl::ClickLogIn()
{
	// TODO: Add your message handler code here
	m_LoginDlg->ShowWindow(SW_SHOW);
}