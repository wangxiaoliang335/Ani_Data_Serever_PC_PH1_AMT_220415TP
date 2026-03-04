#pragma once
#include "afxcmn.h"
#include "ListCtrlEx.h"

// CDlgIdCardHistory dialog

class CDlgIdCardHistory : public CDialog//, IStringChangeListener
{
	DECLARE_DYNAMIC(CDlgIdCardHistory)

	CListCtrlEx m_IdCardHistory;

public:
	int m_iCurrentPos;					 // 0~999±Ó¡ˆ¿”
	int m_iOrder;

	CDlgIdCardHistory(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgIdCardHistory();

	// Dialog Data
	enum { IDD = DLG_ID_CARD_READER_HISTORY };

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void SetCardReaderData(int index, IDCardReader pIDCardReader);
	void SetItemText(int iRow, int iCol, CString strVal);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnUserList();
	void ClickBtnCurList();
};
