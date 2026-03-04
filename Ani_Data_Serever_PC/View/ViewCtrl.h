//{{AFX_INCLUDES()
#include "btnenh.h"
//}}AFX_INCLUDES
#if !defined(AFX_VIEWCTRL_H__51CD6BA9_2672_4E23_A4B8_800AFE2CAF14__INCLUDED_)
#define AFX_VIEWCTRL_H__51CD6BA9_2672_4E23_A4B8_800AFE2CAF14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ViewCtrl.h : header file
//
#include "InitDialogBar.h"
/////////////////////////////////////////////////////////////////////////////
// CViewCtrl dialog

class CViewCtrl : public CInitDialogBar
{
// Construction
public:
	virtual BOOL OnInitDialogBar();
	CViewCtrl(CWnd* pParent = NULL);   // standard constructor
	~CViewCtrl();
// Dialog Data
	//{{AFX_DATA(CViewCtrl)
	enum { IDD = MAIN_BOTTOM_VIEW };
	//}}AFX_DATA
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewCtrl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewCtrl)
	afx_msg void OnClickEndBtn();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool m_bClickBtn;
	int m_iOldViewCtrl;
public:
	void ClickBottomBtn();
	void ClickTestOn();
	void StringChanged();
	void StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi);
	void SetTestButtonVisible();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWCTRL_H__51CD6BA9_2672_4E23_A4B8_800AFE2CAF14__INCLUDED_)
