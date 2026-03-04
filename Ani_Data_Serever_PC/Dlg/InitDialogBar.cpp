// InitDialogBar.cpp: implementation of the CInitDialogBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InitDialogBar.h"
#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "DlgMainLog.h"
#else
#include "DlgGammaMain.h"
#endif


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CInitDialogBar::CInitDialogBar(CWnd* pParent /*=NULL*/)
: CDialogBar()//(CInitDialogBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInitDialogBar)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CInitDialogBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInitDialogBar)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInitDialogBar, CDialogBar)
//{{AFX_MSG_MAP(CInitDialogBar)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInitDialogBar message handlers


BOOL CInitDialogBar::Create(CWnd * pParentWnd, LPCTSTR lpszTemplateName,
							UINT nStyle, UINT nID)
{	
	// Let MFC Create the control
	if(!CDialogBar::Create(pParentWnd, lpszTemplateName, nStyle, nID))
		return FALSE;	// Since there is no WM_INITDIALOG message we have to call
	// our own InitDialog function ourselves after m_hWnd is valid
	if(!OnInitDialogBar())		
		return FALSE;	
	return TRUE;
}

BOOL CInitDialogBar::Create(CWnd * pParentWnd, UINT nIDTemplate,
							UINT nStyle, UINT nID)
{
	if(!CDialogBar::Create(pParentWnd, MAKEINTRESOURCE(nIDTemplate), nStyle, nID))
		return FALSE;	// Since there is no WM_INITDIALOG message we have to call
	// our own InitDialog function ourselves after m_hWnd is valid
	if(!OnInitDialogBar())		
		return FALSE;	
	return TRUE;
}

//다이알로그에 데이터 갱신...
BOOL CInitDialogBar::OnInitDialogBar()
{
	UpdateData(FALSE);
	return TRUE;
}


BOOL CInitDialogBar::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateDialogControls(this, TRUE);
	return CDialogBar::PreTranslateMessage(pMsg);
}

