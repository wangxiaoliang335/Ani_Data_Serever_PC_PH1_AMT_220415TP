#if !defined(AFX_HEADERCTRLEX1_H__8839C76A_39CB_4CD1_A9B3_D77B3E64E8C8__INCLUDED_)
#define AFX_HEADERCTRLEX1_H__8839C76A_39CB_4CD1_A9B3_D77B3E64E8C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HeaderCtrlEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx window

class CHeaderCtrlEx : public CHeaderCtrl
{
public:
	CHeaderCtrlEx();

	void SetHeaderFont(LONG lHeight, LPCTSTR lpszFaceName) { m_lHeight = lHeight; m_strFaceName = lpszFaceName; };

	int DrawTextEx(CDC *pDC, const CString &str, CRect rect, UINT nFormat);

	LONG m_lHeight;
	CString m_strFaceName;
	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHeaderCtrlEx)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CHeaderCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHeaderCtrlEx)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEADERCTRLEX1_H__8839C76A_39CB_4CD1_A9B3_D77B3E64E8C8__INCLUDED_)
