#pragma once

#if _SYSTEM_AMTAFT_
#include "DlgUnloaderInspect.h"
#include "DlgUnloaderTimeInspect.h"
#include "DlgTactTimeHistory.h"
#include "DlgDefectCount.h"

class CManiUnloaderView : public CFormView
{
	DECLARE_DYNCREATE(CManiUnloaderView)

protected:
	
public:
	CDlgUnloaderInspect *m_pUnloaderInspect;
	CDlgUnloaderTimeInspect *m_pUnloaderTimeInspect;
	CDlgDefectCount *m_pDefectCount;

	enum { IDD = MAIN_UNLOADER_VIEW };

	CManiUnloaderView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CManiUnloaderView();

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();

private:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

public:
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnPaint();
	void ChangeTab();
};
#endif


