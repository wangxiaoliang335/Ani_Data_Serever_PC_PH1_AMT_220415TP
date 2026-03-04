#pragma once
#include "DLGChangeSySemData.h"

// CDLGSetSystem 대화 상자입니다.

class CDLGSetSystem : public CDialogEx
{
	DECLARE_DYNAMIC(CDLGSetSystem)

public:
	CDLGSetSystem(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDLGSetSystem();

	void SelectChangePart();
	void IniSetDataText();
	CString GetStringFromDialog();
	BOOL OnInitDialog();
	void GetNewContent();

	CBtnEnh m_MechineType;
	CBtnEnh m_EqpId;
	CBtnEnh m_EqpNum;
	CBtnEnh m_CompanyLine;
	CBtnEnh m_OPV_ImageWidth;
	CBtnEnh m_OPV_ImageHeight;
	CBtnEnh m_OK_Grade;
	CBtnEnh m_ContactNg_Grade;
	CBtnEnh m_ContactNg_Code;


// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_SETSYSTEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
public:
	DECLARE_EVENTSINK_MAP()

	void ClickBtnReload();
	void ClickBtnOk();
	void ClickBtnSave();
	void ClickAlignCount();
	void ClickMachinType();
	void ClickPGCodeUsable();
};
