#pragma once
#include "btnenh.h"

// CDlgLogin dialog

class CDlgLogin : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLogin();

// Dialog Data
	enum { IDD = IDD_DLG_LOG_IN };

	CListBox	m_ctrlListUser;
	CString		m_strPassword;
	CString		m_strInputPW;
	int			m_iChangePWState;

	CBtnEnh		m_ctrlChangePW;
	CBtnEnh		m_ctrlApply;

	void		SetUserState(int iUserState);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void ClickCancel();
	void ClickOk();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void ClickChangePassword();
	void ClickButtonApply();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnClick5();
};

extern CDlgLogin *g_pLoginDlg;