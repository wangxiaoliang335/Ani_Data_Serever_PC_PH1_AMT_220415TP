// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx
CListCtrlEx::CListCtrlEx()
{
	//	m_iListMode=0;
	m_nItem = -1;
	m_nSubItem = -1;
	m_colRow1 = RGB(255, 255, 255);
	m_colRow2 = RGB(250, 250, 250);
	m_pFont = NULL;
	bSelectionBar = FALSE;
}
void CListCtrlEx::DeleteImageList() {
	m_imgList.DeleteImageList();

}
void CListCtrlEx::CreateImageList(int iCx, int iCy) {
	m_imgList.DeleteImageList();
	m_imgList.Create(iCx, iCy, ILC_COLOR24, 1, 1);
	SetImageList(&m_imgList, LVSIL_SMALL);
}
void CListCtrlEx::SetFontSize(int iHeaderSize, double iHeaderGab, int iSize, double iGab) {
	if (m_pFont == NULL) {
		m_imgList.DeleteImageList();
		m_imgList.Create(1, ((int)((double)iSize*iGab)), ILC_COLOR24, 1, 1);
		SetImageList(&m_imgList, LVSIL_SMALL);


		int fontHeight = iSize;
		m_pFont = new CFont();
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		GetFont()->GetLogFont(&lf);
		lf.lfHeight = fontHeight;
		lf.lfWeight = FW_BOLD;
		m_pFont->CreateFontIndirect(&lf);
		SetFont(m_pFont);
		Initializing(iHeaderSize, (int)(((double)iHeaderSize*10.)*iHeaderGab), lf.lfFaceName);
	}
}
CListCtrlEx::~CListCtrlEx()
{
	if (m_pFont != NULL)
		delete m_pFont;
}
void CListCtrlEx::ShowSelectionBar(BOOL bShow) {
	bSelectionBar = bShow;
}
BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//ON_WM_MEASUREITEM_REFLECT()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CLICK, &CListCtrlEx::OnNMClick)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, &CListCtrlEx::OnLvnItemchanging)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers
void CListCtrlEx::SetHeaderEnable(BOOL bEnable) {
	GetHeaderCtrl()->EnableWindow(bEnable);
}

BOOL CListCtrlEx::Initializing(int nPointSize, int iHeaderHeight, LPCTSTR lpszFaceName, CDC *pDC)
{
	// 리스트 콘트롤 멀티라인 헤더
	m_NewHeaderFont.CreatePointFont(iHeaderHeight, lpszFaceName);

	m_HeaderCtrlEx.SetHeaderFont(nPointSize, lpszFaceName);

	CHeaderCtrl* pHeader = NULL;
	pHeader = GetHeaderCtrl();

	if (pHeader == NULL)
		return FALSE;

	VERIFY(m_HeaderCtrlEx.SubclassWindow(pHeader->m_hWnd));

	// 위에서 생성한 폰트가 클수록 콘트롤이 더 커진다.
	m_HeaderCtrlEx.SetFont(&m_NewHeaderFont);

	HDITEM hdItem;

	hdItem.mask = HDI_FORMAT;

	for (int i = 0; i<m_HeaderCtrlEx.GetItemCount(); i++)
	{
		m_HeaderCtrlEx.GetItem(i, &hdItem);

		/*if(i==m_HeaderCtrlEx.GetItemCount()-1)
		hdItem.fmt|= HDF_LEFT|HDF_OWNERDRAW;
		else
		hdItem.fmt|= HDF_CENTER|HDF_OWNERDRAW;
		*/
		hdItem.fmt |= HDF_OWNERDRAW;
		m_HeaderCtrlEx.SetItem(i, &hdItem);
	}

	return TRUE;
}

BOOL CListCtrlEx::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	CListCtrlEx::GetClientRect(rect);

	POINT mypoint;
	CBrush brush0(m_colRow1);
	CBrush brush1(m_colRow2);

	int chunk_height = GetCountPerPage();
	pDC->FillRect(&rect, &brush1);

	for (int i = 0; i <= chunk_height; i++)
	{
		GetItemPosition(i, &mypoint);
		rect.top = mypoint.y;
		GetItemPosition(i + 1, &mypoint);
		rect.bottom = mypoint.y;
		pDC->FillRect(&rect, i % 2 ? &brush1 : &brush0);

	}
	brush0.DeleteObject();
	brush1.DeleteObject();

	//return FALSE;
	return CListCtrl::OnEraseBkgnd(pDC);
}
void CListCtrlEx::SetMainHandle(CWnd* wnd) {
	wndResponse = wnd;
}
void CListCtrlEx::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Required for other coloring stuff to work, if you have any.
	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	int iRow = (int)(lplvcd->nmcd.dwItemSpec);

	switch (lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}

	// Modify item text and or background
	case CDDS_ITEMPREPAINT:
	{
		lplvcd->clrText = RGB(0, 0, 0);
		// If you want the sub items the same as the item,
		// set *pResult to CDRF_NEWFONT
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		return;
	}

	// Modify sub item text and/or background
	case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
	{
		if (iRow % 2) {
			lplvcd->clrTextBk = m_colRow2;
		}
		else {
			lplvcd->clrTextBk = m_colRow1;
		}
		/*NMLVCUSTOMDRAW *pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
		CString text= GetItemText(iRow, pDraw->iSubItem);
		if(text == _T("Light")){
		pDraw->clrText = 0xff;
		pDraw->clrTextBk = 0xf0fff0;
		}
		else{
		pDraw->clrText = 0x0;
		pDraw->clrTextBk = 0xffffff;
		}*/
		/*NMLVCUSTOMDRAW *pDraw = (NMLVCUSTOMDRAW*)(pNMHDR);
		CString text = GetItemText(iRow, pDraw->iSubItem);
		if (text == _T("11.11")){
		pDraw->clrText = pDraw->clrTextBk;
		//pDraw->clrTextBk = 0xf0fff0;
		}*/
		*pResult = CDRF_DODEFAULT;
		return;
	}
	}
}
void CListCtrlEx::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	m_nItem = pNMItemActivate->iItem;
	m_nSubItem = pNMItemActivate->iSubItem;
	wndResponse->SendMessage(WM_USER_LISTCTRLEX, reinterpret_cast<WPARAM>(this), reinterpret_cast<LPARAM>(pNMHDR));
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CListCtrlEx::OnLvnItemchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	// LVN_ITEMCHANGING notification handler
	if (!bSelectionBar) {
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVNI_SELECTED))
		{
			// yes - never allow a selected item
			*pResult = 1;
		}
		else
		{
			// no - allow any other change
			*pResult = 0;
		}
	}
}

void CListCtrlEx::OnDestroy()
{
	CListCtrl::OnDestroy();
	if (m_pFont) m_pFont->DeleteObject();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CListCtrlEx::SetSelectListPos(int iPos) {
	if (iPos == -1) return;
	if (iPos >= GetItemCount()) return;
	m_nItem = iPos;
	//>> 160831 jwan
	SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
	SetItemState(iPos, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	EnsureVisible(iPos, false);
	SetFocus();
}
void CListCtrlEx::SetListColumn(int nCol, CString lpszColumnHeading, int nFormat, int nWidth, int nSubItem) {

	TCHAR szName[128]; // This is the buffer where the name will be stored
	LVCOLUMN lvColInfo;
	ZeroMemory(&lvColInfo, sizeof(lvColInfo)); // This line is not really needed and you can remove it
	lvColInfo.mask = LVCF_TEXT;
	lvColInfo.pszText = szName;
	lvColInfo.cchTextMax = _countof(szName);
	GetColumn(nCol, &lvColInfo);
	_tcscpy_s(lvColInfo.pszText, 128, lpszColumnHeading);
	SetColumn(nCol, &lvColInfo);
}
void CListCtrlEx::SetListItem(int iItem, int iSubitem, CString strText) {
	if (GetItemText(iItem, iSubitem) != strText)
		SetItemText(iItem, iSubitem, strText);
}
void CListCtrlEx::SetListItemTime(int iItem, int iSubitem, DWORD dwTime) {
	CString strTemp;
	//strTemp.Format(_T("%02d:%02d:%02d.%02d"), dwTime / 3600000, dwTime / 60000, (dwTime / 1000) % 60, (dwTime / 10) % 100);
	strTemp.Format(_T("%02d:%02d.%02d"), dwTime / 60000, (dwTime / 1000) % 60, (dwTime / 10) % 100);
	SetListItem(iItem, iSubitem, strTemp);
}