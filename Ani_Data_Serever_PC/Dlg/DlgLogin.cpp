// DlgLogin.cpp : implementation file
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "DlgLogin.h"
#include "afxdialogex.h"
#include "TopCtrl.h"
#include "EzIni.h"

// CDlgLogin dialog
IMPLEMENT_DYNAMIC(CDlgLogin, CDialog)

CDlgLogin *g_pLoginDlg;

CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogin::IDD, pParent)
{
	g_pLoginDlg = this;
	m_strInputPW.Format(_T(""));
	m_iChangePWState = _STATE_CONFIRM_PASSWORD;
}

CDlgLogin::~CDlgLogin()
{
}

void CDlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_USER, m_ctrlListUser);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strInputPW);
	DDX_Control(pDX, IDB_CHANGE_PASSWORD, m_ctrlChangePW);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_ctrlApply);
}


BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDlgLogin message handlers
BEGIN_EVENTSINK_MAP(CDlgLogin, CDialog)
	ON_EVENT(CDlgLogin, IDB_CANCEL, DISPID_CLICK, CDlgLogin::ClickCancel, VTS_NONE)
	ON_EVENT(CDlgLogin, IDB_OK, DISPID_CLICK, CDlgLogin::ClickOk, VTS_NONE)
	ON_EVENT(CDlgLogin, IDB_CHANGE_PASSWORD, DISPID_CLICK, CDlgLogin::ClickChangePassword, VTS_NONE)
	ON_EVENT(CDlgLogin, IDC_BUTTON_APPLY, DISPID_CLICK, CDlgLogin::ClickButtonApply, VTS_NONE)
END_EVENTSINK_MAP()


void CDlgLogin::ClickCancel()
{
	CDialog::OnCancel();
	m_strInputPW.SetString(_T(""));
	UpdateData(FALSE);
}


void CDlgLogin::ClickOk()
{
	UpdateData();
	int iCurSel = m_ctrlListUser.GetCurSel();

	SetUserState(iCurSel);

	m_strInputPW.SetString(_T("")); 

	theApp.LoginCheckMethod();

	UpdateData(FALSE);
}

void CDlgLogin::SetUserState(int iUserState)
{
	CString	strKeyValue;

	EZIni ini(DATA_SYSTEM_DATA_PATH);

	switch (iUserState)
	{
	case USER_OPERATOR:
		strKeyValue = ini[_T("LOGIN_INFO")][_T("OPERATOR")];
		m_strPassword.Format(_T("%s"), strKeyValue);
		if (!m_strInputPW.CompareNoCase(m_strPassword))
		{
			theApp.m_iUserClass = USER_OPERATOR;
			g_topCtrl->m_strLoginName.SetCaption(_T("LogIn :\nOperator"));

			this->ShowWindow(SW_HIDE);
		}
		else
		{
			m_ctrlListUser.SetCurSel(USER_OPERATOR);
			theApp.getMsgBox(MS_OK, _T("비밀번호 확인하세요."), _T("Operator Password Wrong"), _T("操作员密码错误"));
		}
		break;
	case USER_ENGINEER:
		strKeyValue = ini[_T("LOGIN_INFO")][_T("ENGINEER")];
		m_strPassword.Format(_T("%s"), strKeyValue);
		if (!m_strInputPW.CompareNoCase(m_strPassword))
		{
			theApp.m_iUserClass = USER_ENGINEER;
			g_topCtrl->m_strLoginName.SetCaption(_T("LogIn :\nEngineer"));

			this->ShowWindow(SW_HIDE);
		}
		else
		{
			m_ctrlListUser.SetCurSel(USER_ENGINEER);
			theApp.getMsgBox(MS_OK, _T("비밀번호 확인하세요."), _T("Engineer Password Wrong"), _T("工程师密码错误"));
			return;
		}
		break;
	case USER_MAKER:
		strKeyValue = ini[_T("LOGIN_INFO")][_T("MAKER")];
		m_strPassword.Format(_T("%s"), strKeyValue);
		if (!m_strInputPW.CompareNoCase(m_strPassword))
		{
			theApp.m_iUserClass = USER_MAKER;
			g_topCtrl->m_strLoginName.SetCaption(_T("LogIn :\nMaker"));

			this->ShowWindow(SW_HIDE);
		}
		else
		{
			m_ctrlListUser.SetCurSel(USER_MAKER);
			theApp.getMsgBox(MS_OK, _T("비밀번호 확인하세요."), _T("Maker Password Wrong"), _T("管理员密码错误"));
			return;
		}
		break;
	}

	UpdateData(FALSE);
}


BOOL CDlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetWindowPos(NULL, 400, 300, 0, 0, SWP_NOSIZE);

	CFont font;
	font.CreateFont(25, 11, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Fixedsys Excelsior 3.01"));

	m_ctrlListUser.SetFont(&font, TRUE);

	m_ctrlListUser.InsertString(0, _T("OPERATOR"));
	m_ctrlListUser.InsertString(1, _T("ENGINEER"));
	m_ctrlListUser.InsertString(2, _T("MAKER"));
	m_ctrlListUser.SetItemHeight(0, 26);
	m_ctrlListUser.SetItemHeight(1, 26);
	m_ctrlListUser.SetItemHeight(2, 26);

	m_ctrlListUser.SetCurSel(0);

	m_ctrlApply.SetEnabled(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CDlgLogin::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CBrush br;
	CRect rect;
	GetClientRect(&rect);
	br.CreateSolidBrush(RGB(205, 205, 210));
	pDC->FillRect(&rect, &br);
	br.DeleteObject();
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}


void CDlgLogin::ClickChangePassword()
{
	int iCurSel = m_ctrlListUser.GetCurSel();

	if (m_ctrlChangePW.GetValue())
	{
		m_iChangePWState = _STATE_CONFIRM_PASSWORD;
		switch (iCurSel)
		{
		case USER_OPERATOR:
			theApp.getMsgBox(MS_OK, _T("현재비밀번호 입력후 쓰기버튼 누르세요"), _T("Input Current Operator Password and Apply Button"), _T("请输入现有密码后选择执行按钮"));
			break;
		case USER_ENGINEER:
			theApp.getMsgBox(MS_OK, _T("현재비밀번호 입력후 쓰기버튼 누르세요"), _T("Input Current Engineer Password and Apply Button"), _T("请输入现有密码后选择执行按钮"));
			break;
		case USER_MAKER:
			theApp.getMsgBox(MS_OK, _T("현재비밀번호 입력후 쓰기버튼 누르세요"), _T("Input Current Maker Password and Apply Button"), _T("请输入现有密码后选择执行按钮"));
			break;
		}

		m_ctrlApply.SetEnabled(TRUE);
	}
	else
	{
		m_iChangePWState = _STATE_CONFIRM_PASSWORD;
	}


}


void CDlgLogin::ClickButtonApply()
{
	UpdateData();
	int iCurSel = m_ctrlListUser.GetCurSel();

	CString	strKeyValue;

	EZIni ini(DATA_SYSTEM_DATA_PATH);

	switch (m_iChangePWState)
	{
	case _STATE_CONFIRM_PASSWORD:
		switch (iCurSel)
		{
		case USER_OPERATOR:
			strKeyValue = ini[_T("LOGIN_INFO")][_T("OPERATOR")];
			m_strPassword.Format(_T("%s"), strKeyValue);
			if (!m_strInputPW.CompareNoCase(m_strPassword))
			{
				theApp.getMsgBox(MS_OK, _T("변경할 비밀번호 입력후 쓰기 버튼 누르세요"), _T("Input Change Operator Password and Apply Button"), _T("请输入变更后密码并选择执行按钮"));
				m_iChangePWState = _STATE_CHANGE_PASSWORD;
				m_strInputPW.Format(_T(""));
			}
			else
			{
				theApp.getMsgBox(MS_OK, _T("비밀번호 확인하세요."), _T("Operator Password Wrong"), _T("操作员密码错误"));
				return;
			}
			break;
		case USER_ENGINEER:
			strKeyValue = ini[_T("LOGIN_INFO")][_T("ENGINEER")];
			m_strPassword.Format(_T("%s"), strKeyValue);
			if (!m_strInputPW.CompareNoCase(m_strPassword))
			{
				theApp.getMsgBox(MS_OK, _T("변경할 비밀번호 입력후 쓰기 버튼 누르세요"), _T("Input Change Engineer Password and Apply Button"), _T("请输入变更后密码并选择执行按钮"));
				m_iChangePWState = _STATE_CHANGE_PASSWORD;
				m_strInputPW.Format(_T(""));
			}
			else
			{
				theApp.getMsgBox(MS_OK, _T("비밀번호 확인하세요."), _T("Engineer Password Wrong"), _T("工程师密码错误"));
				return;
			}
			break;
		case USER_MAKER:
			strKeyValue = ini[_T("LOGIN_INFO")][_T("MAKER")];
			m_strPassword.Format(_T("%s"), strKeyValue);
			if (!m_strInputPW.CompareNoCase(m_strPassword))
			{
				theApp.getMsgBox(MS_OK, _T("변경할 비밀번호 입력후 쓰기 버튼 누르세요"), _T("Input Change Maker Password and Apply Button"), _T("请输入变更后密码并选择执行按钮"));
				m_iChangePWState = _STATE_CHANGE_PASSWORD;
				m_strInputPW.Format(_T(""));
			}
			else
			{
				theApp.getMsgBox(MS_OK, _T("비밀번호 확인하세요."), _T("Maker Password Wrong"), _T("管理员密码错误"));
				return;
			}
			UpdateData(FALSE);
			break;
		}
		break;
	case _STATE_INPUT_PASSWORD:
		m_iChangePWState = _STATE_CHANGE_PASSWORD;
		break;
	case _STATE_CHANGE_PASSWORD:
		switch (iCurSel)
		{
		case USER_OPERATOR:
			ini[_T("LOGIN_INFO")][_T("OPERATOR")] = m_strInputPW;
			theApp.getMsgBox(MS_OK, _T("비밀번호 변경 완료"), _T("Operator Password Change Complete"), _T("操作员密码修改完成"));
			break;
		case USER_ENGINEER:
			ini[_T("LOGIN_INFO")][_T("ENGINEER")] = m_strInputPW;
			theApp.getMsgBox(MS_OK, _T("비밀번호 변경 완료"), _T("Engineer Password Change Complete"), _T("工程师密码修改完成"));
			break;
		case USER_MAKER:
			ini[_T("LOGIN_INFO")][_T("MAKER")] = m_strInputPW;
			theApp.getMsgBox(MS_OK, _T("비밀번호 변경 완료"), _T("Maker Password Change Complete"), _T("管理员密码修改完成"));
			break;
		}
		m_iChangePWState = _STATE_CONFIRM_PASSWORD;
		m_strInputPW.Format(_T(""));
		m_ctrlChangePW.SetValue(FALSE);
		m_ctrlApply.SetEnabled(FALSE);
		UpdateData(FALSE);
		break;
	}
}


BOOL CDlgLogin::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (::GetKeyState(VK_CONTROL) < 0)
		{
			if ((pMsg->wParam == 86))
				pMsg->message = WM_PASTE;
			else if (pMsg->wParam == 67)
				pMsg->message = WM_COPY;
		}

		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE;
			break;
		case VK_RETURN:
			return TRUE;
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}