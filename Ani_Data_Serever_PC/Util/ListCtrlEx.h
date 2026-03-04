#if !defined(AFX_LISTCTRLEX_H__3E6294FD_2953_4C48_A347_6D300ED5B606__INCLUDED_)
#define AFX_LISTCTRLEX_H__3E6294FD_2953_4C48_A347_6D300ED5B606__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlEx.h : header file
//
#define WM_USER_LISTCTRLEX WM_USER+1
#include "HeaderCtrlEx.h"
/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window

class CListCtrlEx : public CListCtrl
{
	// Construction
public:
	CListCtrlEx();
	// Attributes
public:
	int m_nItem;																					// 선택된 Row Index 값
	int m_nSubItem;																					// 선택된 Col Index 값

	void SetFontSize(int iHeaderSize, double iHeaderGab, int iSize, double iGab);					// ListCtrl 크기 조정 
																									// iHeaderSize는 상단 폰트, iHeaderGab은 1.0일때 기본 2.0일때 두줄가능
																									// iSize는 Item 글자 크기 조절가능, iGab은 1.0일때 기본 2.0일때 두줄가능
	void ShowSelectionBar(BOOL bShow);																// SelectBar TRUE 보이기, FALSE 숨기기
	void CreateImageList(int iCx, int iCy);															// ImageList 추가시 사용 
	void SetHeaderEnable(BOOL bEnable);
	void SetMainHandle(CWnd* wnd);																	// 메시지 받을 핸들
	void SetSelectListPos(int iPos);																// 셀 선택
	void SetListColumn(int nCol, CString lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	void DeleteImageList();
	void SetListItem(int iItem, int iSubitem, CString strText);
	void SetListItemTime(int iItem, int iSubitem, DWORD dwTime);
	//afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	// Operations
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CListCtrlEx();

	// Generated message map functions
//protected:
public:
	COLORREF		m_colRow1, m_colRow2;
	CFont			m_NewHeaderFont, m_NewDataFont;
	CHeaderCtrlEx	m_HeaderCtrlEx;
	CFont *m_pFont;
	BOOL bSelectionBar;
	CImageList m_imgList;
	CWnd* wndResponse;
	//int m_iListMode;
	BOOL Initializing(int nPointSize, int iHeaderHeight, LPCTSTR lpszFaceName, CDC *pDC = NULL);
	//{{AFX_MSG(CListCtrlEx)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg void CListCtrlEx::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
public:
	CImageList* GetCurImageList() { return &m_imgList; }
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEX_H__3E6294FD_2953_4C48_A347_6D300ED5B606__INCLUDED_)
