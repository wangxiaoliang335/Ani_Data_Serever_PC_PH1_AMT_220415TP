#if !defined(AFX_MYLISTCTRL_H__E9448758_1C91_489E_AD36_7C659E30F3B2__INCLUDED_)
#define AFX_MYLISTCTRL_H__E9448758_1C91_489E_AD36_7C659E30F3B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl window

class CMyListCtrl : public CListCtrl
{
// Construction
public:
	CMyListCtrl();

// Attributes
public:
	int m_iControlID;
	CEdit	m_ctrEidt;
	
	int m_nItem;
	int m_nSubItem;
	BOOL	m_bEnableDblClick;
	// Operations
public:
	void EndModify();
	void CreateEditBox();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ResetIndex();
	void DeleteData();
	void SetControlID(int nID);
	virtual ~CMyListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListCtrl)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__E9448758_1C91_489E_AD36_7C659E30F3B2__INCLUDED_)
