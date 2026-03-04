//{{AFX_INCLUDES()
#include "btnenh.h"
//}}AFX_INCLUDES
#if !defined(AFX_TOPCTRL_H__2C820CB9_B60B_4622_9C06_2FC3BAE61C92__INCLUDED_)
#define AFX_TOPCTRL_H__2C820CB9_B60B_4622_9C06_2FC3BAE61C92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TopCtrl.h : header file
//
#include "InitDialogBar.h"
#include "DlgLogin.h"

/////////////////////////////////////////////////////////////////////////////
// CTopCtrl dialog
class CMainFrame;

class CTopCtrl : public CInitDialogBar
{
// Construction
public:
	CTopCtrl(CWnd* pParent = NULL);   // standard constructor
	~CTopCtrl();

#if _SYSTEM_AMTAFT_
	enum { IDD = AMT_MAIN_TOP_VIEW };
#else
	enum { IDD = GAMMA_MAIN_TOP_VIEW };
#endif
	
	void CloseView();
	void SetStartTime();
	virtual BOOL OnInitDialogBar();

	CTime	m_tCurTime;
	CString m_strDispTime;

	CBtnEnh m_CurrentTime;
	CBtnEnh m_TitleName;
	CBtnEnh	m_ctrlModelName;
	CBtnEnh m_ctrlVersion;

	CBtnEnh m_strLoginName;

	CBtnEnh m_netWorkIf[NUM_NETWORK_ICON];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTopCtrl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTopCtrl)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CMainFrame* pMainFrame;
	void ClickLogIn();
	CDlgLogin *m_LoginDlg;
	BOOL bFlag;
	BOOL bFlag2;
	BOOL m_bAlignFlag;

public:
	afx_msg void OnLoginBtn();
};
extern CTopCtrl *g_topCtrl;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOPCTRL_H__2C820CB9_B60B_4622_9C06_2FC3BAE61C92__INCLUDED_)

