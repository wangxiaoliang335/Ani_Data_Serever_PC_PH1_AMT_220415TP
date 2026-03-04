#pragma once
#include "btnenh.h"

// CDlgLogin dialog

class CSetTimerDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetTimerDlg)

public:
	CSetTimerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetTimerDlg();

	// Dialog Data
	enum { IDD = IDD_DLG_SET_TIMER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void OnBtnOk();
	void DisplayTimerInfo();
	void ClickSetTimer(int Name);
	void ReLoadTimer();

	virtual BOOL OnInitDialog();
	void ClickBtnSave();
};
