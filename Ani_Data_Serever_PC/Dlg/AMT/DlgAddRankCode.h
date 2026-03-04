#pragma once

#include "afxwin.h"

// CDlgAddRankCode 대화 상자입니다.

//class CSetRank;

class CDlgAddRankCode : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAddRankCode)

public:
	CDlgAddRankCode(int nChangeOrAdd, CString strSelectCode, CString strSelectDescribe);
	CDlgAddRankCode(int nChangeOrAdd); // 표준 생성자입니다.
	virtual ~CDlgAddRankCode();
	CString ReturnAddCode();
	CString ReturnAddDescribe();
	


// 대화 상자 데이터입니다.
	enum { IDD = IDD_ADDCODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	
	int m_nChangeOrAdd;
	CString m_strSelect;
	
	DECLARE_EVENTSINK_MAP()
	void ClickChangeOkButton();
	
	BOOL OnInitDialog();

	void ClickChangeCancelButton();
	

	
	CString m_strChangeRankCode;
	
	CEdit m_ctrlAddEdit;
	CEdit m_ctrlChangeEdit;
	
	CString m_strOldDefectCode;
	CString m_strNewDefectCode;
	CString m_strOldDescribe;
	CString m_strNewDescribe;
};