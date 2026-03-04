// CDlgAlignCount.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Ani_Data_Serever_PC.h"
#include "DlgAlignCount.h"
#include "afxdialogex.h"


// CDlgAlignCount 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgAlignCount, CDialog)

CDlgAlignCount::CDlgAlignCount(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAlignCount::IDD, pParent)
{

}

CDlgAlignCount::~CDlgAlignCount()
{
}

void CDlgAlignCount::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_MACHINE_COUNT, m_cmbPcCount);

	for (int ii = 0; ii < MaxAlignCount; ii++)
	{
		DDX_Control(pDX, IDC_MACHINE_PC_1 + ii, m_btnMachinePc[ii]);
		DDX_Control(pDX, IDC_ALIGN_PATTERN_1 + ii, m_btnPatternAlign[ii]);
		DDX_Control(pDX, IDC_ALIGN_TRAY_CHECK_1 + ii, m_btnTrayCheck[ii]);
		DDX_Control(pDX, IDC_ALIGN_LOWER_ALIGN_1 + ii, m_btnTrayLowerAlign[ii]);
		DDX_Control(pDX, IDC_ALIGN_TRAY_ALIGN_1 + ii, m_btnTrayAlign[ii]);
	}
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAlignCount, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_MACHINE_COUNT, &CDlgAlignCount::OnCbnSelchangeComboMachineCount)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDlgAlignCount, CDialog)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_PATTERN_1, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_PATTERN_2, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_PATTERN_3, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_PATTERN_4, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_PATTERN_5, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_PATTERN_6, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_PATTERN_7, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_PATTERN_8, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_PATTERN_9, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_PATTERN_10, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)

	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_CHECK_1, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_CHECK_2, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_CHECK_3, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_CHECK_4, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_CHECK_5, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_CHECK_6, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_CHECK_7, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_CHECK_8, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_CHECK_9, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_CHECK_10, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)

	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_ALIGN_1, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_ALIGN_2, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_ALIGN_3, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_ALIGN_4, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_ALIGN_5, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_ALIGN_6, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_ALIGN_7, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_ALIGN_8, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_ALIGN_9, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_TRAY_ALIGN_10, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)

	ON_EVENT(CDlgAlignCount, IDC_ALIGN_LOWER_ALIGN_1, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_LOWER_ALIGN_2, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_LOWER_ALIGN_3, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_LOWER_ALIGN_4, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_LOWER_ALIGN_5, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_LOWER_ALIGN_6, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_LOWER_ALIGN_7, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_LOWER_ALIGN_8, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_LOWER_ALIGN_9, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)
	ON_EVENT(CDlgAlignCount, IDC_ALIGN_LOWER_ALIGN_10, DISPID_CLICK, CDlgAlignCount::ClickAlignInspect, VTS_NONE)

	ON_EVENT(CDlgAlignCount, IDB_BTN_OK, DISPID_CLICK, CDlgAlignCount::ClickBtnOk, VTS_NONE)
END_EVENTSINK_MAP()

// CDlgAlignCount 메시지 처리기입니다.
BOOL CDlgAlignCount::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strName;
	for (int ii = 0; ii < MaxAlignCount; ii++)
	{
		strName.Format(_T("%d"), ii + 1);
		m_cmbPcCount.AddString(strName);

		m_btnMachinePc[ii].SetWindowTextW(CStringSupport::FormatString(_T("Align Type %d"), ii + 1));
		m_btnPatternAlign[ii].SetWindowTextW(AlignTypeName[PatternAlign]);
		m_btnTrayCheck[ii].SetWindowTextW(AlignTypeName[TrayCheck]);
		m_btnTrayAlign[ii].SetWindowTextW(AlignTypeName[TrayAlign]);
		m_btnTrayLowerAlign[ii].SetWindowTextW(AlignTypeName[TrayLowerAlign]);
	}

	// twice
	/*EZIni ini(DATA_SYSTEM_DATA_PATH);
	CString str = ini[_T("DATA")][_T("ALIGN_COUNT")];
	m_iAlignCount = _ttoi(str);*/
	m_iAlignCount = _ttoi(theApp.m_strAlignCount);

	m_cmbPcCount.SetCurSel(m_iAlignCount - 1);
	
	for (int ii = 0; ii < m_iAlignCount; ii++)
	{
		m_btnMachinePc[ii].ShowWindow(TRUE);
		m_btnPatternAlign[ii].ShowWindow(TRUE);
		m_btnTrayCheck[ii].ShowWindow(TRUE);
		m_btnTrayAlign[ii].ShowWindow(TRUE);
		m_btnTrayLowerAlign[ii].ShowWindow(TRUE);
		switch (theApp.m_iAlignInspectType[ii])
		{
		case PatternAlign:	m_btnPatternAlign[ii].SetValue(TRUE); break;
		case TrayCheck:		m_btnTrayCheck[ii].SetValue(TRUE); break;
		case TrayAlign:	 m_btnTrayAlign[ii].SetValue(TRUE); break;
		case TrayLowerAlign:	m_btnTrayLowerAlign[ii].SetValue(TRUE); break;
		}
	}

	

	return TRUE;
}

void CDlgAlignCount::OnCbnSelchangeComboMachineCount()
{
	int iAlignCount = m_cmbPcCount.GetCurSel() + 1;

	for (int ii = 0; ii < iAlignCount; ii++)
	{
		m_btnMachinePc[ii].ShowWindow(TRUE);
		m_btnPatternAlign[ii].ShowWindow(TRUE);
		m_btnTrayCheck[ii].ShowWindow(TRUE);
		m_btnTrayAlign[ii].ShowWindow(TRUE);
		m_btnTrayLowerAlign[ii].ShowWindow(TRUE);
		switch (theApp.m_iAlignInspectType[ii])
		{
		case PatternAlign:	m_btnPatternAlign[ii].SetValue(TRUE); break;
		case TrayCheck:		m_btnTrayCheck[ii].SetValue(TRUE); break;
		case TrayAlign:	 m_btnTrayAlign[ii].SetValue(TRUE); break;
		case TrayLowerAlign: m_btnTrayLowerAlign[ii].SetValue(TRUE);  break;
		}
	}

	for (int jj = iAlignCount; jj < MaxAlignCount; jj++)
	{
		m_btnMachinePc[jj].ShowWindow(FALSE);
		m_btnPatternAlign[jj].ShowWindow(FALSE);
		m_btnTrayCheck[jj].ShowWindow(FALSE);
		m_btnTrayAlign[jj].ShowWindow(FALSE);
		m_btnTrayLowerAlign[jj].ShowWindow(FALSE);
	}
	
	m_iAlignCount = iAlignCount;
}

void CDlgAlignCount::ClickAlignInspect()
{
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	for (int ii = 0; ii < m_iAlignCount; ii++)
	{
		int iAlignPattern = IDC_ALIGN_PATTERN_1 + ii;
		int iAlignTrayCheck = IDC_ALIGN_TRAY_CHECK_1 + ii;
		int iAlignTrayAlign = IDC_ALIGN_TRAY_ALIGN_1 + ii;
		int iAlignTrayLower = IDC_ALIGN_LOWER_ALIGN_1 + ii;

		if (pBtnEnh->GetDlgCtrlID() == iAlignPattern)
		{
			theApp.m_iAlignInspectType[abs(IDC_ALIGN_PATTERN_1 - pBtnEnh->GetDlgCtrlID())] = PatternAlign;
			break;
		}
		else if (pBtnEnh->GetDlgCtrlID() == iAlignTrayCheck)
		{
			theApp.m_iAlignInspectType[abs(IDC_ALIGN_TRAY_CHECK_1 - pBtnEnh->GetDlgCtrlID())] = TrayCheck;
			break;
		}
		else if (pBtnEnh->GetDlgCtrlID() == iAlignTrayAlign)
		{
			theApp.m_iAlignInspectType[abs(IDC_ALIGN_TRAY_ALIGN_1 - pBtnEnh->GetDlgCtrlID())] = TrayAlign;
			break;
		}
		else if (pBtnEnh->GetDlgCtrlID() == iAlignTrayLower)
		{
			theApp.m_iAlignInspectType[abs(IDC_ALIGN_LOWER_ALIGN_1 - pBtnEnh->GetDlgCtrlID())] = TrayLowerAlign;
			break;
		}
			
	}
}

void CDlgAlignCount::ClickBtnOk()
{
	CDialog::OnOK();
}
