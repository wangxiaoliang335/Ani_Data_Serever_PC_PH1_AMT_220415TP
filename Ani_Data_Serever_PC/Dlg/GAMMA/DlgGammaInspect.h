#pragma once

#if _SYSTEM_GAMMA_
// CDlgGammaInspect 대화 상자입니다.

class CDlgGammaInspect : public CDialog
{
	DECLARE_DYNAMIC(CDlgGammaInspect)

public:
	CDlgGammaInspect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgGammaInspect();

	// 대화 상자 데이터입니다.
	enum { IDD = DLG_INSPECT_GAMMA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CBtnEnh m_btnTotalSum;
	CBtnEnh m_btnGoodTotalSum;
	CBtnEnh m_btnNgTotalSum;
	CBtnEnh m_btnAutoContactTotalSum;
	CBtnEnh m_btnManualContactTotalSum;
	CBtnEnh m_btnMtpTotalSum;
	CBtnEnh m_btnAlignTotalSum;

	CBtnEnh m_btnStageTotal[MaxGammaStage];
	CBtnEnh m_btnGoodTotal[MaxGammaStage];
	CBtnEnh m_btnNgotal[MaxGammaStage];
	CBtnEnh m_btnAutoContactTotal[MaxGammaStage];
	CBtnEnh m_btnManualContactTotal[MaxGammaStage];
	CBtnEnh m_btnMtpTotal[MaxGammaStage];
	CBtnEnh m_btnAlignTotal[MaxGammaStage];

	CBtnEnh m_btnLabelName;

	int m_iShift = 0;
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();

	void UpdateDisplay(int nShift);
	DECLARE_EVENTSINK_MAP()
	void ClickChResult();
	void OnClickDataReset();
	void ShiftData();

	BOOL PreTranslateMessage(MSG* pMsg);
};
#endif