//{{AFX_INCLUDES()
#include "btnenh.h"
#include "Ani_Data_Serever_PC.h"
//}}AFX_INCLUDES
#if !defined(AFX_MSGBOX_H__F9836095_45D5_4417_B95B_E042DDD8D89F__INCLUDED_)
#define AFX_MSGBOX_H__F9836095_45D5_4417_B95B_E042DDD8D89F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgBox.h : header file
//

#include "resource.h"

#include "StringManager.h"
/////////////////////////////////////////////////////////////////////////////
// CMsgBox dialog

class CMsgBox : public CDialog
{
// Construction
public:
	CMsgBox(int m_iStyle, CString m_keyString, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CMsgBox)
	enum { IDD = DLG_MSG_BOX };
	CBtnEnh	m_ctrlNo;
	CBtnEnh	m_ctrlOk;
	CBtnEnh	m_ctrlYes;
	CBtnEnh	m_ctrlMark1;
	CBtnEnh	m_ctrlMark2;
	CBtnEnh	m_ctrlMsg;
	BOOL m_bAlarmShow;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMsgBox)
	afx_msg void OnClickOk();
	afx_msg void OnClickNo();
	afx_msg void OnClickYes();
	virtual BOOL OnInitDialog();
	//void OnTimer();
	DECLARE_EVENTSINK_MAP()
private:
	CString m_keyString; //130225 JSPark
	int m_iStyle;
	CString m_strWait;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	void WaitShowHide(int bShowHide, CString strMsg = _T(""), BOOL bAlarmShow = FALSE);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGBOX_H__F9836095_45D5_4417_B95B_E042DDD8D89F__INCLUDED_)
