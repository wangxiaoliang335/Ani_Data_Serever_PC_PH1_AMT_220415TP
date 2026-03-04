#pragma once

#if _SYSTEM_GAMMA_
// CDlgGammaTimeInspect 대화 상자입니다.

class CDlgGammaTimeInspect : public CDialog
{
	DECLARE_DYNAMIC(CDlgGammaTimeInspect)

public:
	CDlgGammaTimeInspect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgGammaTimeInspect();

	// 대화 상자 데이터입니다.
	enum { IDD = DLG_TIME_INSPECT_GAMMA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	CBtnEnh m_btnTimeTotal;
	CBtnEnh m_btnGoodTotal;
	CBtnEnh m_btnNgTotal;
	CBtnEnh m_btnAutoContactTotal;
	CBtnEnh m_btnManualContactTotal;
	CBtnEnh m_btnMtpTotal;
	CBtnEnh m_btnAlignTotal;
	CBtnEnh m_btnSumTotal;

	CBtnEnh m_btnTime[InspectTimeTotalCount];
	CBtnEnh m_btnGood[InspectTimeTotalCount];
	CBtnEnh m_btnNg[InspectTimeTotalCount];
	CBtnEnh m_btnAutoContact[InspectTimeTotalCount];
	CBtnEnh m_btnManualContact[InspectTimeTotalCount];
	CBtnEnh m_btnMtp[InspectTimeTotalCount];
	CBtnEnh m_btnAlign[InspectTimeTotalCount];
	CBtnEnh m_btnSum[InspectTimeTotalCount];

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();

	void UpdateDisplay(int nShift);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnShift();

	int m_iSelectShift;
	void ClickDataReset();
	ProductionData SumProduction;
	BOOL PreTranslateMessage(MSG* pMsg);
};
#endif