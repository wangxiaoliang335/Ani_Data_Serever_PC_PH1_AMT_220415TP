#pragma once
#include "afxcmn.h"
#include "ListCtrlEx.h"

#if _SYSTEM_AMTAFT_
// CDlgReslutCode 대화 상자입니다.

class CDlgReslutCode : public CDialog
{
	DECLARE_DYNAMIC(CDlgReslutCode)

	CListCtrlEx m_ResultCodeListCtrl;
	CListCtrlEx m_ResultCodeRankListCtrl;
	CBtnEnh	m_ctrTitleName;

public:
	CDlgReslutCode(CString strInspName, int iShift);   // 표준 생성자입니다.
	virtual ~CDlgReslutCode();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_RESULT_CODE_CHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	void SetItemText(int iRow, int iCol, CString strVal);
	void SetItemRankText(int iRow, int iCol, CString strVal);
	void SetHistoryReslutCode(int index, pair<CString, ResultCodeRank> code);
	void SetRankReslutCode(int index, pair<CString, int> code);

	CString m_strInspName;
	int m_iShift;
	int m_iModeNum;
	int m_iSelectZone;

	void ClickSelectBtn();
	void SelectZoneResultCode(int iIndexNum, int iModeNum);
	void ClickHistoryMode();
	void ModeSelect(int iModeNum);
};
#endif