#pragma once
#include "afxcmn.h"
#include "ListCtrlEx.h"

// CDlgTactTimeHistory 대화 상자입니다.

class CDlgTactTimeHistory : public CDialog
{
	DECLARE_DYNAMIC(CDlgTactTimeHistory)

public:
	CDlgTactTimeHistory(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgTactTimeHistory();

	// 대화 상자 데이터입니다.
	enum { IDD = DLG_TACT_TIME_HISTORY };



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void UpdateTactTime(int itemIndex, CTact& tactTime);

	DECLARE_MESSAGE_MAP()

	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	CListCtrlEx m_TactTimeListCtrl;

};
