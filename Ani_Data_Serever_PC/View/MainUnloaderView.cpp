// DataView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#if _SYSTEM_AMTAFT_
#include "Ani_Data_Serever_PC.h"
#include "MainUnloaderView.h"
#include "DlgUnloaderTimeInspect.h"

// CManiUnloaderView

IMPLEMENT_DYNCREATE(CManiUnloaderView, CFormView)


CManiUnloaderView::CManiUnloaderView()
	: CFormView(CManiUnloaderView::IDD)
{
	m_pUnloaderInspect = NULL;
	m_pUnloaderTimeInspect = NULL;
	m_pDefectCount = NULL;
}

CManiUnloaderView::~CManiUnloaderView()
{
	if (m_pUnloaderInspect != NULL)
		delete m_pUnloaderInspect;
	if (m_pUnloaderTimeInspect != NULL)
		delete m_pUnloaderTimeInspect;
	if (m_pDefectCount != NULL)
		delete m_pDefectCount;
}

void CManiUnloaderView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_EVENTSINK_MAP(CManiUnloaderView, CFormView)
	ON_EVENT(CManiUnloaderView, IDB_BTN_UNLOADER_INSPECT, DISPID_CLICK, CManiUnloaderView::ChangeTab, VTS_NONE)
	ON_EVENT(CManiUnloaderView, IDB_BTN_UNLOADER_TIME_INSPECT, DISPID_CLICK, CManiUnloaderView::ChangeTab, VTS_NONE)
	ON_EVENT(CManiUnloaderView, IDB_BTN_UNLOADER_DEFECT_COUNT, DISPID_CLICK, CManiUnloaderView::ChangeTab, VTS_NONE)
END_EVENTSINK_MAP()


BEGIN_MESSAGE_MAP(CManiUnloaderView, CFormView)
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CManiUnloaderView 진단입니다.

#ifdef _DEBUG
void CManiUnloaderView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CManiUnloaderView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


BOOL CManiUnloaderView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CManiUnloaderView::OnPaint()
{
	CPaintDC dc(this);

	CRect rc;		rc.SetRectEmpty();
	GetDlgItem(IDC_UNLOADER_MAIN_FRM_DIS1)->GetWindowRect(&rc);

}

BOOL CManiUnloaderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	return CFormView::PreCreateWindow(cs);
}

void CManiUnloaderView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CManiUnloaderView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	CRect rc;		rc.SetRectEmpty();
	GetDlgItem(IDC_UNLOADER_MAIN_FRM_DIS1)->GetWindowRect(&rc);
	ScreenToClient(&rc);

	CREATE_RECT_DLG(m_pUnloaderInspect, CDlgUnloaderInspect, DLG_INSPECT_UNLOADER, this, rc, SW_SHOW);
	CREATE_RECT_DLG(m_pUnloaderTimeInspect, CDlgUnloaderTimeInspect, DLG_TIME_INSPECT_UNLOADER, this, rc, SW_HIDE);
	CREATE_RECT_DLG(m_pDefectCount, CDlgDefectCount, DLG_DEFECT_COUNT, this, rc, SW_HIDE);

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDB_BTN_UNLOADER_INSPECT);
	pBtnEnh->SetValue(TRUE);
}

void CManiUnloaderView::ChangeTab()
{
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();
	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDB_BTN_UNLOADER_INSPECT:
		m_pUnloaderInspect->ShowWindow(SW_SHOW);
		m_pUnloaderTimeInspect->ShowWindow(SW_HIDE);
		m_pDefectCount->ShowWindow(SW_HIDE);
		break;
	case IDB_BTN_UNLOADER_TIME_INSPECT:
		m_pUnloaderInspect->ShowWindow(SW_HIDE);
		m_pUnloaderTimeInspect->ShowWindow(SW_SHOW);
		m_pDefectCount->ShowWindow(SW_HIDE);
		break;
	case IDB_BTN_UNLOADER_DEFECT_COUNT:
		m_pUnloaderInspect->ShowWindow(SW_HIDE);
		m_pUnloaderTimeInspect->ShowWindow(SW_HIDE);
		m_pDefectCount->ShowWindow(SW_SHOW);
		break;
	}
}
#endif