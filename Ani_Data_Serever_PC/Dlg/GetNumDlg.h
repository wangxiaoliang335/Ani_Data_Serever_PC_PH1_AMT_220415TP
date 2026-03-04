//{{AFX_INCLUDES()
#include "btnenh.h"
//}}AFX_INCLUDES
#if !defined(AFX_GETNUMDLG_H__377960AA_52D6_47EB_A2DA_D7B991FA7CAA__INCLUDED_)
#define AFX_GETNUMDLG_H__377960AA_52D6_47EB_A2DA_D7B991FA7CAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetNumDlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CGetNumDlg dialog

class CGetNumDlg : public CDialog
{
	// Construction
public:

	CString GetstrNum();
	void SetstrNum(CString strNum);
	void SetstrNum(double dNum);
	void SetstrNum(int nNum);
	void DispChangeLang();
	void SetWindowTitle(LPCTSTR title);
	CGetNumDlg(int maxchar = 10, LPCTSTR getnum = _T("0.0"), LPCTSTR title = _T("Insert Number Only."),
		CWnd* pParent = NULL, BOOL pwd = FALSE);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CGetNumDlg)
	enum { IDD = DLG_NUMBER_BOX };
	CBtnEnh	m_sSetValue;
	CBtnEnh	m_sDispNum;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetNumDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	int  m_nPointNum;
	BOOL m_bCalu;
	int  m_nCalu;
	BOOL m_FirstFlag;
	BOOL m_bpwd;
	CString m_strGetNum;
	CString m_strOrg;
	int m_intMaxChar;
	CString m_strWndText;

	BOOL VerifyMaxChar();
	void InitStaticDispWnd();

	// Generated message map functions
	//{{AFX_MSG(CGetNumDlg)
	afx_msg void OnBtnPlus();
	afx_msg void OnBtnMinus();
	afx_msg void OnBtnEqual();
	afx_msg void OnBtnDot();
	afx_msg void OnBtnSign();
	afx_msg void OnBtnBack();
	afx_msg void OnBtnClear();
	afx_msg void OnBtnOk();
	afx_msg void OnBtnEsc();
	afx_msg void OnBtn1();
	afx_msg void OnBtn2();
	afx_msg void OnBtn3();
	afx_msg void OnBtn4();
	afx_msg void OnBtn5();
	afx_msg void OnBtn6();
	afx_msg void OnBtn7();
	afx_msg void OnBtn8();
	afx_msg void OnBtn9();
	afx_msg void OnBtn0();
	afx_msg void OnPaint();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int m_cyChar;
	int m_cxChar;
	BOOL m_numFlag;
	CRect m_rect;
	void UpdateDisplay(COLORREF bkcolor = RGB(255, 255, 255));
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETNUMDLG_H__377960AA_52D6_47EB_A2DA_D7B991FA7CAA__INCLUDED_)
