
// MainFrm.h : CMainFrame 클래스의 인터페이스
//

#pragma once

#include "TopCtrl.h"
#include "ViewCtrl.h"
#include "AddrView.h"
#if _SYSTEM_AMTAFT_
#include "ComView.h"
//#include "ComUnloaderView.h"
#include "MainUnloaderView.h"
#include "DefectView.h"
#else
#include "ComGammaView.h"
#endif

#include "Ani_Data_Serever_PCDoc.h"
#include "Ani_Data_Serever_PCView.h"


class CMainFrame : public CFrameWnd
{
protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:

// 작업입니다.
public:

	CTopCtrl m_cTopCtrl;
	CViewCtrl m_cViewCtrl;
#if _SYSTEM_AMTAFT_
	//CComView *m_pComView;
	CManiUnloaderView *m_pMainUnloaderView;
#else
	CComGammaView *m_pComView;
#endif

	CAddrView *m_pAddrView;

	UINT m_nCurView;
	void SwitchingView(UINT nID);

	CAni_Data_Serever_PCDoc *m_pDoc;
	CAni_Data_Serever_PCDoc * GetMainDoc()const { return m_pDoc; }
	void SetMainDoc(CAni_Data_Serever_PCDoc *doc){ m_pDoc = doc; }

	CView *m_pMainView;

	// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CToolBar          m_wndToolBar;

	// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnClose(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL DestroyWindow();
	virtual CFrameWnd* GetActiveFrame();

	virtual void ActivateFrame(int nCmdShow = -1);
};


