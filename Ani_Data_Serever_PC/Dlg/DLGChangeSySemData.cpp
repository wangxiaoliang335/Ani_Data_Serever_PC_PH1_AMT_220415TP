// DLGChangeSySemData.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "DLGChangeSySemData.h"
#include "afxdialogex.h"


// CDLGChangeSySemData 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDLGChangeSySemData, CDialogEx)

CDLGChangeSySemData::CDLGChangeSySemData(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDLGChangeSySemData::IDD, pParent)
{

}
CDLGChangeSySemData::CDLGChangeSySemData(int nParts, CString strContent, int iSetType) : CDialogEx(CDLGChangeSySemData::IDD), nPartProtect(nParts), strContentProtect(strContent), m_iSetType(iSetType)
{
	
}

CDLGChangeSySemData::~CDLGChangeSySemData()
{
}

void CDLGChangeSySemData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDLGChangeSySemData, CDialogEx)
END_MESSAGE_MAP()


// CDLGChangeSySemData 메시지 처리기입니다.
BOOL CDLGChangeSySemData::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString strPart;
	if (m_iSetType == SetSystemParm)
	{
		switch (nPartProtect){
		case MechineType:strPart = _T("MechineType"); break;
		case DfsId:strPart = _T("EQPID"); break;
		case EqpNum:strPart = _T("EQPNum"); break;
		case CompanyLine:strPart = _T("CompanyLine"); break;
		case FileServerID:strPart = _T("FileServerID"); break;
		case OpvImageWidth:strPart = _T("OpvImageWidth"); break;
		case OpvImageHeight:strPart = _T("OpvImageHeight"); break;
		case OkGrade:strPart = _T("OK_Grade"); break;
		case ContactNgGrade:strPart = _T("ContactNgGrade"); break;
		case ContactNgCode:strPart = _T("ContactNgCode"); break;
		case OK_ProcessID:strPart = _T("OK_ProcessID"); break;
		case NG_ProcessID:strPart = _T("NG_ProcessID"); break;
		}
	}
	else
	{
		switch (nPartProtect)
		{
		case DefectCode:strPart = _T("DefectCode"); break;
		case SameDefectMaxCount:strPart = _T("MaxCount"); break;
		case SmaeDefectAlarmCount:strPart = _T("MaxAlarmCount"); break;
		}
	}
	GetDlgItem(IDS_SELECT_PART)->SetWindowTextW(strPart);
	GetDlgItem(IDS_ORIGIN_CONTENT)->SetWindowTextW(strContentProtect);

	return TRUE;
}
CString CDLGChangeSySemData::GetNewContent()
{
	if (strNewContentProtect != _T("")){
		return strNewContentProtect;
	}
	else return strContentProtect;
}
BEGIN_EVENTSINK_MAP(CDLGChangeSySemData, CDialogEx)
ON_EVENT(CDLGChangeSySemData, IDB_BTN_CANCLE, DISPID_CLICK, CDLGChangeSySemData::ClickBtnCancle, VTS_NONE)
ON_EVENT(CDLGChangeSySemData, IDB_BTN_SURE, DISPID_CLICK, CDLGChangeSySemData::ClickBtnSure, VTS_NONE)
END_EVENTSINK_MAP()


void CDLGChangeSySemData::ClickBtnCancle()
{
	// TODO: 
	CDialog::OnOK();
	return;

}


void CDLGChangeSySemData::ClickBtnSure()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	GetDlgItem(IDC_NEW_CONTENT)->GetWindowTextW(strNewContentProtect);
	CDialog::OnOK();
}
