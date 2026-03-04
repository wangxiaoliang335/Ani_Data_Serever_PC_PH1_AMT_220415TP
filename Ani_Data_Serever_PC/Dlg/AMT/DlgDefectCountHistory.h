#pragma once
#include "afxcmn.h"
#include "ListCtrlEx.h"

// CDlgDefectCountHistory dialog
#if _SYSTEM_AMTAFT_
class CDlgDefectCountHistory : public CDialog//, IStringChangeListener
{
	DECLARE_DYNAMIC(CDlgDefectCountHistory)

	CListCtrlEx m_DefectCountList;

public:
	int m_iShift;
	int m_iListType;
	CString m_strTitleName;

	CDlgDefectCountHistory(int iShift, int iListType, CString strTitleName = NULL);   // standard constructor
	virtual ~CDlgDefectCountHistory();

	CBtnEnh m_btnLabelName;

	// Dialog Data
	enum { IDD = DLG_PANEL_DEFECT_COUNT_HISTORY };

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL OnInitDialog();

	void SetItemText(int iRow, int iCol, CString strVal);
	void SetItemInt(int iRow, int iCol, int strVal);
	void SetPanelDefectCountData(int index, DefectCountData PanelDefectData);
	void SetTitleDefectCountData(int index, CString strResult, CString strCode, CString strGrade, int iCount);
	void HistoryView(int nShift, int iListType, CString strTitleName);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnOk();
};
#endif
