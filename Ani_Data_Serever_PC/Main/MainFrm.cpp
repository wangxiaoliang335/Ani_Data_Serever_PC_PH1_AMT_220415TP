
// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_USER_CLOSE, OnClose)
END_MESSAGE_MAP()

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	m_pAddrView = NULL;
	//m_pComView = NULL;

#if _SYSTEM_AMTAFT_
	m_pMainUnloaderView = NULL;
#endif
	m_nCurView = MAINVIEW;
	

}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#if _SYSTEM_AMTAFT_
	if (!m_cTopCtrl.Create(this, AMT_MAIN_TOP_VIEW, CBRS_ALIGN_TOP, AMT_MAIN_TOP_VIEW))
		return -1;
#else
	if (!m_cTopCtrl.Create(this, GAMMA_MAIN_TOP_VIEW, CBRS_ALIGN_TOP, GAMMA_MAIN_TOP_VIEW))
		return -1;
#endif

	if (!m_cViewCtrl.Create(this, MAIN_BOTTOM_VIEW, CBRS_ALIGN_BOTTOM, MAIN_BOTTOM_VIEW))
		return -1;

	EnableDocking(CBRS_ALIGN_ANY);
	SendMessage(WM_COMMAND, WM_USER_INIT_SYSTEM, 0);

#if _SYSTEM_AMTAFT_
	theApp.m_pComView = new CComView;
	theApp.m_pComView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rectDefault, this,
		AFX_IDW_PANE_FIRST + AMT_COM_VIEW, NULL);

	m_pMainUnloaderView = new CManiUnloaderView;
	m_pMainUnloaderView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rectDefault, this,AFX_IDW_PANE_FIRST + MAIN_UNLOADER_VIEW, NULL);
#else
	m_pComView = new CComGammaView;
	m_pComView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rectDefault, this,
		AFX_IDW_PANE_FIRST + GAMMA_COM_VIEW, NULL);
#endif

	m_pAddrView = new CAddrView;
	m_pAddrView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rectDefault, this,
		AFX_IDW_PANE_FIRST + ADDR_VIEW, NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	cs.style = WS_POPUP & ~WS_THICKFRAME;
	cs.hMenu = NULL;

	return TRUE;
}

// CMainFrame ����

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame �޽��� ó����



BOOL CMainFrame::DestroyWindow()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
//	SwitchingView(ENDVIEW);//
	Delay(100, TRUE);
	m_cTopCtrl.CloseView();

	if (theApp.m_pComView != NULL)
	{
		theApp.m_pComView->DestroyWindow();
		theApp.m_pComView = NULL;
	}
#if _SYSTEM_AMTAFT_
	if (m_pMainUnloaderView != NULL)
	{
		m_pMainUnloaderView->DestroyWindow();
		m_pMainUnloaderView = NULL;
	}
#endif
	return CFrameWnd::DestroyWindow();
}


CFrameWnd* CMainFrame::GetActiveFrame()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CFrameWnd::GetActiveFrame();
}


LRESULT CMainFrame::OnClose(WPARAM wParam, LPARAM lParam)
{
	DestroyWindow();

	return 0;
}

void CMainFrame::SwitchingView(UINT nID)
{
	if (nID == m_nCurView)
		return; //aleady selected

	CView *pNewView, *pOldView;

	pOldView = GetActiveView();

	if (pOldView == NULL)
		return;

	switch (nID)
	{
	case MAINVIEW:
		pNewView = m_pMainView;
		break;

	case COMVIEW:
		pNewView = theApp.m_pComView;
		break;

	case ADDRVIEW:
		pNewView = m_pAddrView;
		break;
#if _SYSTEM_AMTAFT_
	case UNLOADERVIEW:
		pNewView = m_pMainUnloaderView;
		break;
#endif
	default:
		ASSERT(0);
		return;
	}

	int nSwitchChildID = AFX_IDW_PANE_FIRST + m_nCurView;

	if (!pNewView->SetDlgCtrlID(AFX_IDW_PANE_FIRST))
		return;

	if (!pOldView->SetDlgCtrlID(nSwitchChildID))
		return;

	Delay(50, TRUE);

	pOldView->ShowWindow(SW_HIDE);
	pNewView->ShowWindow(SW_SHOW);

	CDocument *pDoc = GetActiveDocument();
	pDoc->AddView(pNewView);
	pDoc->RemoveView(pOldView);

	SetActiveView(pNewView);
	RecalcLayout();

	m_nCurView = nID;
}

void CMainFrame::ActivateFrame(int nCmdShow)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_pMainView = GetActiveView();
	CFrameWnd::ActivateFrame(nCmdShow);

	m_pDoc = (CAni_Data_Serever_PCDoc *)GetActiveDocument();
	CFrameWnd::ActivateFrame(nCmdShow);

}