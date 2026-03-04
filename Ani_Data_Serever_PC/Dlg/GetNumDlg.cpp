// GetNumDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "GetNumDlg.h"
#include "AniUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetNumDlg dialog

CGetNumDlg::CGetNumDlg(int maxchar, LPCTSTR getnum, LPCTSTR title, CWnd* pParent, BOOL pwd)
	: CDialog(CGetNumDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetNumDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bCalu = FALSE;
	m_nCalu = 0;

	m_FirstFlag = TRUE;
	m_intMaxChar = maxchar;
	m_bpwd = pwd;

	/*if( m_bpwd )
	m_strGetNum = _T("");
	else */ //modeless
	m_strGetNum = getnum;

	if (m_strGetNum.Find('-') == -1)
	{
		m_numFlag = TRUE;
	}
	else
	{
		m_numFlag = FALSE;
	}

	if (m_strGetNum.Find('.') == -1)
	{
		m_nPointNum = 0;
	}
	else
	{
		m_nPointNum = 6;
	}


	m_strWndText = title;

	m_strOrg = m_strGetNum;

	if (m_bpwd)BOOL Create(CWnd* pParentWnd);
}


void CGetNumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetNumDlg)
	DDX_Control(pDX, IDC_PRESENT, m_sSetValue);
	DDX_Control(pDX, IDC_MODIFY, m_sDispNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetNumDlg, CDialog)
	//{{AFX_MSG_MAP(CGetNumDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetNumDlg message handlers

BEGIN_EVENTSINK_MAP(CGetNumDlg, CDialog)
	//{{AFX_EVENTSINK_MAP(CGetNumDlg)
	ON_EVENT(CGetNumDlg, IDB_BTN_PLUS, -600 /* Click */, OnBtnPlus, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDB_BTN_MINUS, -600 /* Click */, OnBtnMinus, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDB_BTN_EQUAL, -600 /* Click */, OnBtnEqual, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDB_BTN_DOT, -600 /* Click */, OnBtnDot, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDB_BTN_SIGN, -600 /* Click */, OnBtnSign, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDB_BTN_BACK, -600 /* Click */, OnBtnBack, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDB_BTN_CLEAR, -600 /* Click */, OnBtnClear, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDB_BTN_OK, -600 /* Click */, OnBtnOk, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDB_BTN_ESC, -600 /* Click */, OnBtnEsc, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDC_BTN_1, -600 /* Click */, OnBtn1, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDC_BTN_2, -600 /* Click */, OnBtn2, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDC_BTN_3, -600 /* Click */, OnBtn3, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDC_BTN_4, -600 /* Click */, OnBtn4, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDC_BTN_5, -600 /* Click */, OnBtn5, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDC_BTN_6, -600 /* Click */, OnBtn6, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDC_BTN_7, -600 /* Click */, OnBtn7, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDC_BTN_8, -600 /* Click */, OnBtn8, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDC_BTN_9, -600 /* Click */, OnBtn9, VTS_NONE)
	ON_EVENT(CGetNumDlg, IDC_BTN_0, -600 /* Click */, OnBtn0, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CGetNumDlg::OnBtnPlus()
{
	// TODO: Add your control notification handler code here
	//	if(m_FirstFlag ) return;

	m_bCalu = TRUE;
	m_nCalu = TRUE;

	m_FirstFlag = TRUE;

	m_strOrg = m_strGetNum;
	m_strGetNum = _T("");
	m_sSetValue.SetCaption(m_strOrg);
	UpdateDisplay();
}

void CGetNumDlg::OnBtnMinus()
{
	// TODO: Add your control notification handler code here
	//	if(m_FirstFlag ) return;

	m_bCalu = TRUE;
	m_nCalu = FALSE;

	m_FirstFlag = TRUE;

	m_strOrg = m_strGetNum;
	m_strGetNum = _T("");
	m_sSetValue.SetCaption(m_strOrg);
	UpdateDisplay();
}

void CGetNumDlg::OnBtnEqual()
{
	// TODO: Add your control notification handler code here
	double fi, se, rel;
	TCHAR buf[100];

	if (!m_bCalu) return;

#ifdef _UNICODE
	fi = _ttof(m_strOrg);
	se = _ttof(m_strGetNum);

	if (m_nCalu)
		rel = (fi)+(se);
	else
		rel = (fi)-(se);

	//if (!m_nPointNum)
	//	swprintf(buf, _T("%.0f"), rel);
	//else
	//	swprintf(buf, _T("%.6f"), rel);
#else

	fi = atof(m_strOrg);
	se = atof(m_strGetNum);

	if (m_nCalu)
		rel = (fi)+(se);
	else
		rel = (fi)-(se);

	if (!m_nPointNum)
		sprintf(buf, _T("%.0f"), rel);
	else
		sprintf(buf, _T("%.6f"), rel);
#endif // _UNICODE

	m_strGetNum = buf;
	UpdateDisplay();

	m_FirstFlag = TRUE;
	m_bCalu = FALSE;
}

void CGetNumDlg::OnBtnDot()
{
	// TODO: Add your control notification handler code here
	if (m_bpwd) return;

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T(".");
	UpdateDisplay();
}

void CGetNumDlg::OnBtnSign()
{
	// TODO: Add your control notification handler code here
	if (m_bpwd) return;

	if (!m_strGetNum.GetLength()) return;

	if (m_numFlag == TRUE)
	{
		m_strGetNum.Insert(0, '-');
		m_numFlag = FALSE;
	}
	else
	{
		m_strGetNum.Remove('-');
		m_numFlag = TRUE;
	}

	UpdateDisplay();
}

void CGetNumDlg::OnBtnBack()
{
	// TODO: Add your control notification handler code here
	m_FirstFlag = FALSE;

	int len = m_strGetNum.GetLength();
	if (len)
	{
		m_strGetNum.SetAt(len - 1, ' ');
		m_strGetNum.TrimRight();
		UpdateDisplay();
	}
}

void CGetNumDlg::OnBtnClear()
{
	// TODO: Add your control notification handler code here
	m_strGetNum = _T("");
	m_numFlag = TRUE;
	UpdateDisplay();
}

void CGetNumDlg::OnBtnOk()
{
	// TODO: Add your control notification handler code here
	if (m_bpwd) 	{
		//		GetNum_rtn = TRUE;
		//		lstrcpy(codebuf, m_strGetNum);
		DestroyWindow();
	}
	else
	{
		if (m_strGetNum.IsEmpty())
			CDialog::OnCancel();
		else
			CDialog::OnOK();
	}
}

void CGetNumDlg::OnBtnEsc()
{
	// TODO: Add your control notification handler code here
	if (m_bpwd) {
		//		GetNum_rtn = TRUE;
		//		lstrcpy(codebuf, "0");
		DestroyWindow();

	}
	else CDialog::OnCancel();
}

void CGetNumDlg::OnBtn1()
{
	// TODO: Add your control notification handler code here
	if (m_FirstFlag)
	{
		m_FirstFlag = FALSE;
		m_numFlag = TRUE;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("1");
	UpdateDisplay();
}

void CGetNumDlg::OnBtn2()
{
	// TODO: Add your control notification handler code here
	if (m_FirstFlag)
	{
		m_FirstFlag = FALSE;
		m_numFlag = TRUE;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("2");
	UpdateDisplay();
}

void CGetNumDlg::OnBtn3()
{
	// TODO: Add your control notification handler code here
	if (m_FirstFlag)
	{
		m_FirstFlag = FALSE;
		m_numFlag = TRUE;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("3");
	UpdateDisplay();
}

void CGetNumDlg::OnBtn4()
{
	// TODO: Add your control notification handler code here
	if (m_FirstFlag)
	{
		m_FirstFlag = FALSE;
		m_numFlag = TRUE;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("4");
	UpdateDisplay();
}

void CGetNumDlg::OnBtn5()
{
	// TODO: Add your control notification handler code here
	if (m_FirstFlag)
	{
		m_FirstFlag = FALSE;
		m_numFlag = TRUE;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("5");
	UpdateDisplay();
}

void CGetNumDlg::OnBtn6()
{
	// TODO: Add your control notification handler code here
	if (m_FirstFlag)
	{
		m_FirstFlag = FALSE;
		m_numFlag = TRUE;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("6");
	UpdateDisplay();
}

void CGetNumDlg::OnBtn7()
{
	// TODO: Add your control notification handler code here
	if (m_FirstFlag)
	{
		m_FirstFlag = FALSE;
		m_numFlag = TRUE;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("7");
	UpdateDisplay();
}

void CGetNumDlg::OnBtn8()
{
	// TODO: Add your control notification handler code here
	if (m_FirstFlag)
	{
		m_FirstFlag = FALSE;
		m_numFlag = TRUE;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("8");
	UpdateDisplay();
}

void CGetNumDlg::OnBtn9()
{
	// TODO: Add your control notification handler code here
	if (m_FirstFlag)
	{
		m_FirstFlag = FALSE;
		m_numFlag = TRUE;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("9");
	UpdateDisplay();
}

void CGetNumDlg::OnBtn0()
{
	// TODO: Add your control notification handler code here
	if (m_FirstFlag)
	{
		m_FirstFlag = FALSE;
		m_numFlag = TRUE;
		m_strGetNum = _T("");
	}

	if (!VerifyMaxChar()) return;

	m_strGetNum += _T("0");
	UpdateDisplay();
}

void CGetNumDlg::InitStaticDispWnd()
{
	CStatic* pRect = (CStatic*)GetDlgItem(IDC_MODIFY);
	pRect->GetWindowRect(&m_rect);
	pRect->DestroyWindow();
	ScreenToClient(&m_rect);

	TEXTMETRIC tm;
	CClientDC dc(this);
	dc.GetTextMetrics(&tm);
	m_cxChar = tm.tmAveCharWidth;
	m_cyChar = tm.tmHeight - tm.tmDescent;
}

BOOL CGetNumDlg::VerifyMaxChar()
{
	if (m_strGetNum.GetLength() >= m_intMaxChar) return FALSE;
	else return TRUE;
}

void CGetNumDlg::UpdateDisplay(COLORREF bkcolor)
{
	int len, i;
	CString bufstr;

	i = 0; len = 0;
	m_sDispNum.SetCaption(m_strGetNum);
}

void CGetNumDlg::SetWindowTitle(LPCTSTR title)
{
	m_strWndText = title;
}

void CGetNumDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	UpdateDisplay();
	SetWindowText(m_strWndText);
	// Do not call CDialog::OnPaint() for painting messages
}

CString CGetNumDlg::GetstrNum()
{
	return m_strGetNum;
}

void CGetNumDlg::SetstrNum(CString strNum)
{
	m_strGetNum = strNum;
}

void CGetNumDlg::SetstrNum(double dNum)
{
	m_strGetNum.Format(_T("%3.3f"), dNum);
}

void CGetNumDlg::SetstrNum(int nNum)
{
	m_strGetNum.Format(_T("%d"), nNum);
}