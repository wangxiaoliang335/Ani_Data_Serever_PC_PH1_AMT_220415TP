// MsgBox.cpp : implementation file
//

#include "stdafx.h"
#include "MsgBox.h"
//#include "ClassDefine.h"
#include "StringManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgBox dialog


CMsgBox::CMsgBox(int istyle,CString keyString, CWnd* pParent /*=NULL*/) //130225 JSPark
	: CDialog(CMsgBox::IDD, pParent)
{
	m_keyString = keyString;
	m_iStyle = istyle;

	m_bAlarmShow = FALSE;
	
}

void CMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgBox)
	DDX_Control(pDX, IDB_NO, m_ctrlNo);
	DDX_Control(pDX, IDB_OK, m_ctrlOk);
	DDX_Control(pDX, IDB_YES, m_ctrlYes);
	DDX_Control(pDX, IDC_MARK_1, m_ctrlMark1);
	DDX_Control(pDX, IDC_MARK_2, m_ctrlMark2);
	DDX_Control(pDX, IDS_MESSAGE, m_ctrlMsg);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CMsgBox message handlers

BEGIN_EVENTSINK_MAP(CMsgBox, CDialog)
    //{{AFX_EVENTSINK_MAP(CMsgBox)
	ON_EVENT(CMsgBox, IDB_YES, -600 /* Click */, OnClickYes, VTS_NONE)
	ON_EVENT(CMsgBox, IDB_OK, -600 /* Click */, OnClickOk, VTS_NONE)
	ON_EVENT(CMsgBox, IDB_NO, -600 /* Click */, OnClickNo, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

BEGIN_MESSAGE_MAP(CMsgBox, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CMsgBox::OnClickOk() 
{	
	if (m_bAlarmShow)
	{
		WaitShowHide(SW_HIDE);
		m_bAlarmShow = FALSE;
		theApp.m_pEqIf->m_pMNetH->SetPlcBitData(eBitType_VisionSameDefectAlarmStart, OffSet_0, FALSE); //??? ?? ???? ?? ???? ?? .. ?? ???...
	}

	CDialog::OnOK();	
}

void CMsgBox::OnClickNo()
{
	int ret = IDNO;
	EndDialog(ret);
	return;
}

void CMsgBox::OnClickYes()
{
	int ret = IDYES;
	EndDialog(ret);
	return;
}

BOOL CMsgBox::OnInitDialog() //130224 JSPark
{
	CDialog::OnInitDialog();

	switch (m_iStyle)
	{
	case MS_YESNO:
		m_ctrlMark1.ShowWindow(SW_HIDE);
		m_ctrlOk.ShowWindow(SW_HIDE);
		break;
	case MS_OK:
	default:
		m_ctrlMark2.ShowWindow(SW_HIDE);
		m_ctrlYes.ShowWindow(SW_HIDE);
		m_ctrlNo.ShowWindow(SW_HIDE);
		break;
	}
	
	m_ctrlMsg.SetCaption(CStringManager::GetString(m_keyString));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CMsgBox::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (this->IsWindowVisible() == FALSE)
		return;

	if (nIDEvent == TMR_MSG_BOX)
	{
		m_ctrlMsg.GetWindowTextW(m_strWait);
		m_strWait.AppendFormat(_T("."));
		m_ctrlMsg.SetCaption(m_strWait);
	}

	CDialog::OnTimer(nIDEvent);
}

void CMsgBox::WaitShowHide(int bShowHide, CString strMsg, BOOL bAlarmShow)
{
	if (bShowHide == SW_SHOWNORMAL)
	{
		
		SetTimer(TMR_MSG_BOX, 10, NULL);
		ShowWindow(SW_SHOWNORMAL);
		SetWindowPos(NULL, 400, 300, 0, 0, SWP_NOSIZE);
		m_ctrlMark1.ShowWindow(SW_SHOW);
		m_ctrlMark2.ShowWindow(SW_HIDE);
		m_ctrlOk.ShowWindow(SW_HIDE);
		m_ctrlNo.ShowWindow(SW_HIDE);
		m_ctrlYes.ShowWindow(SW_HIDE);
		m_ctrlMsg.SetCaption(strMsg);
		UpdateWindow();
	}
	else if (bShowHide == SW_SHOW)
	{
		m_bAlarmShow = TRUE;
		ShowWindow(SW_SHOW);
		SetWindowPos(NULL, 400, 300, 0, 0, SWP_NOSIZE);
		/*m_ctrlMark1.ShowWindow(SW_SHOW);
		m_ctrlMark2.ShowWindow(SW_HIDE);
		m_ctrlOk.ShowWindow(SW_SHOW);
		m_ctrlNo.ShowWindow(SW_HIDE);
		m_ctrlYes.ShowWindow(SW_HIDE);*/
		m_ctrlMsg.SetCaption(strMsg);
		UpdateWindow();
	}
	else
	{
		ShowWindow(SW_HIDE);
		KillTimer(TMR_MSG_BOX);
	}
}
