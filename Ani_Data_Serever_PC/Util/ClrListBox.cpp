// ClrListBox.cpp : implementation file
//

#include "stdafx.h"
#include "ClrListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClrListBox

CClrListBox::CClrListBox()
{
}

CClrListBox::~CClrListBox()
{
}


BEGIN_MESSAGE_MAP(CClrListBox, CListBox)
	//{{AFX_MSG_MAP(CClrListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CClrListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	if((int)lpDIS->itemID < 0)
		return; 

	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	COLORREF crText;
	CString sText;
	COLORREF crNorm = (COLORREF)lpDIS->itemData;		// Color information is in item data.
	COLORREF crHilite = RGB(255-GetRValue(crNorm), 255-GetGValue(crNorm), 255-GetBValue(crNorm));

	if((lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) {
		CBrush brush(crNorm);
		pDC->FillRect(&lpDIS->rcItem, &brush);
	}

	if(!(lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & ODA_SELECT))
	{
		CBrush brush(::GetSysColor(COLOR_WINDOW));
		pDC->FillRect(&lpDIS->rcItem, &brush);
	}	 	

	if((lpDIS->itemAction & ODA_FOCUS) && (lpDIS->itemState & ODS_FOCUS))
		pDC->DrawFocusRect(&lpDIS->rcItem); 

	if((lpDIS->itemAction & ODA_FOCUS) &&	!(lpDIS->itemState & ODS_FOCUS))
		pDC->DrawFocusRect(&lpDIS->rcItem); 


	int nBkMode = pDC->SetBkMode(TRANSPARENT);

	if(lpDIS->itemData)	{
		if(lpDIS->itemState & ODS_SELECTED)
			crText = pDC->SetTextColor(crHilite);
		else if(lpDIS->itemState & ODS_DISABLED)
			crText = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
		else
			crText = pDC->SetTextColor(crNorm);
	} else {
		if(lpDIS->itemState & ODS_SELECTED)
			crText = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		else if(lpDIS->itemState & ODS_DISABLED)
			crText = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
		else
			crText = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	}


	GetText(lpDIS->itemID, sText);
	CRect rect = lpDIS->rcItem;

	UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	if(GetStyle() & LBS_USETABSTOPS)
		nFormat |= DT_EXPANDTABS;
	
	pDC->DrawText(sText, -1, &rect, nFormat | DT_CALCRECT);
	pDC->DrawText(sText, -1, &rect, nFormat);

	pDC->SetTextColor(crText); 
	pDC->SetBkMode(nBkMode);	
}

void CClrListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	lpMIS->itemHeight = ::GetSystemMetrics(SM_CYMENUCHECK);
}

int CClrListBox::AddString(LPCTSTR lpszItem)
{
	return ((CListBox*)this)->AddString(lpszItem);
}
int CClrListBox::InsertString(int nIndex, LPCTSTR lpszItem)
{
	//20180725 hacy
	if (((CListBox*)this)->GetCount() > 20000)
		((CListBox*)this)->ResetContent();

	((CListBox*)this)->InsertString(nIndex, lpszItem);

	int iExt = GetTextLen(lpszItem);
	if (iExt > GetHorizontalExtent())
		SetHorizontalExtent(iExt);

	return TRUE;
}

int CClrListBox::AddString(LPCTSTR lpszItem, COLORREF rgb)
{
	int nItem = AddString(lpszItem);
	if(nItem >= 0)
		SetItemData(nItem, rgb);
	return nItem;
}

int CClrListBox::InsertString(int nIndex, LPCTSTR lpszItem, COLORREF rgb)
{
	int nItem = ((CListBox*)this)->InsertString(nIndex,lpszItem);
	if(nItem >= 0)
		SetItemData(nItem, rgb);
	return nItem;
}

void CClrListBox::SetItemColor(int nIndex, COLORREF rgb)
{
	SetItemData(nIndex, rgb);	
	RedrawWindow();
}

//<<20180201 hacy 가로스크롤 추가할수 있도록 추가
int CClrListBox::GetTextLen(LPCTSTR lpszText)
{
	ASSERT(AfxIsValidString(lpszText));

	CDC *pDC = GetDC();
	ASSERT(pDC);

	CSize size;
	CFont* pOldFont = pDC->SelectObject(GetFont());
	if ((GetStyle() & LBS_USETABSTOPS) == 0)
	{
		size = pDC->GetTextExtent(lpszText, (int)_tcslen(lpszText));
		size.cx += 3;
	}
	else
	{
		// Expand tabs as well
		size = pDC->GetTabbedTextExtent(lpszText, (int)_tcslen(lpszText), 0, NULL);
		size.cx += 2;
	}
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	return size.cx;
}