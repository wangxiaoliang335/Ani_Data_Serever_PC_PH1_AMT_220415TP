// DLGSetSystem.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "DLGSetVision.h"
#include "afxdialogex.h"


// CDLGSetVision 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDLGSetVision, CDialogEx)

CDLGSetVision::CDLGSetVision(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDLGSetVision::IDD, pParent)
{

}

CDLGSetVision::~CDLGSetVision()
{
}

void CDLGSetVision::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_EVENTSINK_MAP(CDLGSetVision, CDialogEx)
	ON_EVENT(CDLGSetVision, IDS_DEFECT_CODE, DISPID_CLICK, CDLGSetVision::ClickSetParm, VTS_NONE)
	ON_EVENT(CDLGSetVision, IDS_MAX_COUNT, DISPID_CLICK, CDLGSetVision::ClickSetParm, VTS_NONE)
	ON_EVENT(CDLGSetVision, IDS_ALARM_COUNT, DISPID_CLICK, CDLGSetVision::ClickSetParm, VTS_NONE)
	ON_EVENT(CDLGSetVision, IDS_CH_CHECK_USE, DISPID_CLICK, CDLGSetVision::ClickSameDefectMode, VTS_NONE)
	ON_EVENT(CDLGSetVision, IDS_CH_CHECK_NOTUSE, DISPID_CLICK, CDLGSetVision::ClickSameDefectMode, VTS_NONE)
	ON_EVENT(CDLGSetVision, IDS_SAME_DEFECT_ALARM_USE, DISPID_CLICK, CDLGSetVision::ClickSameDefectMode, VTS_NONE)
	ON_EVENT(CDLGSetVision, IDS_SAME_DEFECT_ALARM_NOTUSE, DISPID_CLICK, CDLGSetVision::ClickSameDefectMode, VTS_NONE)
	ON_EVENT(CDLGSetVision, IDB_BTN_RELOAD, DISPID_CLICK, CDLGSetVision::ClickBtnReload, VTS_NONE)
	ON_EVENT(CDLGSetVision, IDB_BTN_OK, DISPID_CLICK, CDLGSetVision::ClickBtnOk, VTS_NONE)
	ON_EVENT(CDLGSetVision, IDB_BTN_SAVE, DISPID_CLICK, CDLGSetVision::ClickBtnSave, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CDLGSetVision::OnInitDialog(){
	CDialog::OnInitDialog();
	CBtnEnh *pBtnEnh;
	CBtnEnh *pBtnEnh2;

	IniSetDataText();

	if (theApp.m_bSameDefectChCheckMode == TRUE)
		pBtnEnh = (CBtnEnh*)GetDlgItem(IDS_CH_CHECK_USE);
	else
		pBtnEnh = (CBtnEnh*)GetDlgItem(IDS_CH_CHECK_NOTUSE);

	if (theApp.m_bSameDefectMode == TRUE)
		pBtnEnh2 = (CBtnEnh*)GetDlgItem(IDS_SAME_DEFECT_ALARM_USE);
	else
		pBtnEnh2 = (CBtnEnh*)GetDlgItem(IDS_SAME_DEFECT_ALARM_NOTUSE);

	pBtnEnh->SetValue(TRUE);
	pBtnEnh2->SetValue(TRUE);

	UpdateData(FALSE);
	return TRUE;
}


void CDLGSetVision::IniSetDataText()
{
	GetDlgItem(IDS_DEFECT_CODE)->SetWindowTextW(theApp.m_strSameDefectCode);
	GetDlgItem(IDS_MAX_COUNT)->SetWindowTextW(theApp.m_strSameDefectMaxCount);
	GetDlgItem(IDS_ALARM_COUNT)->SetWindowTextW(theApp.m_strSameDefectAlarmMaxCount);
}

void CDLGSetVision::SelectChangePart()
{
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDS_DEFECT_CODE: theApp.m_iSysPartSelect = DefectCode; break;
	case IDS_MAX_COUNT: theApp.m_iSysPartSelect = SameDefectMaxCount; break;
	case IDS_ALARM_COUNT: theApp.m_iSysPartSelect = SmaeDefectAlarmCount; break;
	}
}

CString CDLGSetVision::GetStringFromDialog()
{
	SelectChangePart();

	CString strSysContent;
	switch (theApp.m_iSysPartSelect){
	case DefectCode: GetDlgItem(IDS_DEFECT_CODE)->GetWindowTextW(strSysContent); break;
	case SameDefectMaxCount: GetDlgItem(IDS_MAX_COUNT)->GetWindowTextW(strSysContent); break;
	case SmaeDefectAlarmCount: GetDlgItem(IDS_ALARM_COUNT)->GetWindowTextW(strSysContent); break;
	}
	return strSysContent;
}

void CDLGSetVision::ClickSetParm()
{
	CString strSysContent = GetStringFromDialog();
	CDLGChangeSySemData ChangeContent(theApp.m_iSysPartSelect, strSysContent, SetVisionParm);
	ChangeContent.DoModal();
	CString strNewContent = ChangeContent.GetNewContent();  

	switch (theApp.m_iSysPartSelect){
	case DefectCode: GetDlgItem(IDS_DEFECT_CODE)->SetWindowTextW(strNewContent); break;
	case SameDefectMaxCount: GetDlgItem(IDS_MAX_COUNT)->SetWindowTextW(strNewContent); break;
	case SmaeDefectAlarmCount: GetDlgItem(IDS_ALARM_COUNT)->SetWindowTextW(strNewContent); break;
	}
}

void CDLGSetVision::ClickBtnReload()
{
	IniSetDataText();
}

void CDLGSetVision::ClickBtnOk()
{
	CDialog::OnOK();
}

void CDLGSetVision::ClickBtnSave()
{
	EZIni ini(DATA_SYSTEM_DATA_PATH);
	CString strSysContent;

	GetDlgItem(IDS_DEFECT_CODE)->GetWindowTextW(strSysContent);
	theApp.m_strSameDefectCode = ini[_T("VISION")][_T("DEFECT_CODE")] = strSysContent;

	GetDlgItem(IDS_MAX_COUNT)->GetWindowTextW(strSysContent);
	theApp.m_strSameDefectMaxCount = ini[_T("VISION")][_T("MAX_COUNT")] = strSysContent;

	GetDlgItem(IDS_ALARM_COUNT)->GetWindowTextW(strSysContent);
	theApp.m_strSameDefectAlarmMaxCount = ini[_T("VISION")][_T("ALARM_COUNT")] = strSysContent;

	ini[_T("VISION")][_T("CH_CHECK_MODE")] = theApp.m_bSameDefectChCheckMode;
	ini[_T("VISION")][_T("SAME_DEFECT_MODE")] = theApp.m_bSameDefectMode;

	theApp.getMsgBox(MS_OK, _T("SAVE FINISHED"), _T("SAVE FINISHED"), _T("SAVE FINISHED"));
}


void CDLGSetVision::ClickSameDefectMode()
{
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDS_CH_CHECK_USE:
		if (pBtnEnh->GetValue())
			theApp.m_bSameDefectChCheckMode = TRUE;
		break;
	case IDS_CH_CHECK_NOTUSE:
		if (pBtnEnh->GetValue())
			theApp.m_bSameDefectChCheckMode = FALSE;
		break;
	case IDS_SAME_DEFECT_ALARM_USE:
		if (pBtnEnh->GetValue())
			theApp.m_bSameDefectMode = TRUE;
		break;
	case IDS_SAME_DEFECT_ALARM_NOTUSE:
		if (pBtnEnh->GetValue())
			theApp.m_bSameDefectMode = FALSE;
		break;
	}
}