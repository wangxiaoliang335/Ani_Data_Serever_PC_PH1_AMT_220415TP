// InitDialogBar.h: interface for the CInitDialogBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITDIALOGBAR_H__96BD7C5A_09F2_4870_9786_11F5C46B9202__INCLUDED_)
#define AFX_INITDIALOGBAR_H__96BD7C5A_09F2_4870_9786_11F5C46B9202__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CInitDialogBar : public CDialogBar  
{
	// Construction
public:
	virtual BOOL OnInitDialogBar(); // 파생 클래스에 동적 결합을 위해 가상함수로...
	CInitDialogBar(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CInitDialogBar)
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	BOOL Create(CWnd * pParentWnd, UINT nIDTemplate, UINT nStyle, UINT	nID);
	BOOL Create(CWnd * pParentWnd, LPCTSTR lpszTemplateName, UINT nStyle, UINT nID);
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInitDialogBar)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CInitDialogBar)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_INITDIALOGBAR_H__96BD7C5A_09F2_4870_9786_11F5C46B9202__INCLUDED_)
