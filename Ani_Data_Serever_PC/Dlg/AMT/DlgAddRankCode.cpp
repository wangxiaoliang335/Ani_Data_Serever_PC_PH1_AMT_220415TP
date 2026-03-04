// AddRankCodeDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "Ani_Data_Serever_PC.h"
#include "DlgAddRankCode.h"
#include "afxdialogex.h"


// CDlgAddRankCode 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgAddRankCode, CDialogEx)

CDlgAddRankCode::CDlgAddRankCode(int nChangeOrAdd, CString strSelectCode,CString strSelectDescribe)
: CDialogEx(CDlgAddRankCode::IDD)
, m_strOldDefectCode(strSelectCode)
, m_strNewDefectCode(_T(""))
, m_strOldDescribe(strSelectDescribe)
, m_strNewDescribe(_T(""))
{
	m_nChangeOrAdd = nChangeOrAdd;
}
CDlgAddRankCode::CDlgAddRankCode(int nChangeOrAdd)
	: CDialogEx(CDlgAddRankCode::IDD)
{
	m_nChangeOrAdd = nChangeOrAdd;
	

}

CDlgAddRankCode::~CDlgAddRankCode()
{
}

void CDlgAddRankCode::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	DDX_Text(pDX, IDC_EDIT_OLD_DEFECTCODE, m_strOldDefectCode);
	DDX_Text(pDX, IDC_EDIT_NEW_DEFECTCODE, m_strNewDefectCode);
	DDX_Text(pDX, IDC_EDIT_OLD_DESCRIBE, m_strOldDescribe);
	DDX_Text(pDX, IDC_EDIT_NEW_DESCRIBE, m_strNewDescribe);
}


BEGIN_MESSAGE_MAP(CDlgAddRankCode, CDialogEx)
END_MESSAGE_MAP()


// CDlgAddRankCode 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CDlgAddRankCode, CDialogEx)
	ON_EVENT(CDlgAddRankCode, IDC_CHANGE_OK_BUTTON, DISPID_CLICK, CDlgAddRankCode::ClickChangeOkButton, VTS_NONE)
	ON_EVENT(CDlgAddRankCode, IDC_CHANGE_CANCEL_BUTTON, DISPID_CLICK, CDlgAddRankCode::ClickChangeCancelButton, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CDlgAddRankCode::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_nChangeOrAdd == ChangeRank)
	{
		GetDlgItem(IDC_STATIC)->SetWindowTextW(_T("Change"));
		GetDlgItem(IDC_EDIT_OLD_DEFECTCODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_OLD_DESCRIBE)->EnableWindow(FALSE);

	}
	else
	{
		GetDlgItem(IDC_EDIT_OLD_DEFECTCODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_OLD_DESCRIBE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_DESCRIBE)->ShowWindow(SW_HIDE);
	}

	return TRUE;
}


void CDlgAddRankCode::ClickChangeOkButton()
{
	CDialog::OnOK();
	return;
}
CString CDlgAddRankCode::ReturnAddCode()
{
	return m_strNewDefectCode;
	
}

CString CDlgAddRankCode::ReturnAddDescribe()
{
	return m_strNewDescribe;
}


void CDlgAddRankCode::ClickChangeCancelButton()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CDialog::OnCancel();
}