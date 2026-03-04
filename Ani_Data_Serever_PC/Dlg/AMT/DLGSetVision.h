#pragma once
#include "DLGChangeSySemData.h"

class CDLGSetVision : public CDialogEx
{
	DECLARE_DYNAMIC(CDLGSetVision)

public:
	CDLGSetVision(CWnd* pParent = NULL);  
	virtual ~CDLGSetVision();

	void SelectChangePart();
	void IniSetDataText();
	CString GetStringFromDialog();
	BOOL OnInitDialog();

	enum { IDD = IDD_DLG_SETVISION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
public:
	DECLARE_EVENTSINK_MAP()

	void ClickBtnReload();
	void ClickBtnOk();
	void ClickBtnSave();
	void ClickSetParm();
	void ClickSameDefectMode();
};
