#pragma once

#if _SYSTEM_AMTAFT_
// CDlgTimeInspect 대화 상자입니다.

class CDlgTimeInspect : public CDialog
{
	DECLARE_DYNAMIC(CDlgTimeInspect)

public:
	CDlgTimeInspect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgTimeInspect();

	// 대화 상자 데이터입니다.
	enum { IDD = DLG_TIME_INSPECT_AMT_AFT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	CBtnEnh m_btnTimeTotal;
	CBtnEnh m_btnGoodTotal;
	CBtnEnh m_btnNgTotal;
	CBtnEnh m_btnAutoContactTotal;
	CBtnEnh m_btnAoiTotal;
	CBtnEnh m_btnViewingTotal;
	CBtnEnh m_btnOtpTotal;
	CBtnEnh m_btnTpTotal;
	CBtnEnh m_btnAlignTotal;
	CBtnEnh m_btnTrayOutTotal;
	CBtnEnh m_btnSumTotal;
	
	CBtnEnh m_btnTime[InspectTimeTotalCount];
	CBtnEnh m_btnGood[InspectTimeTotalCount];
	CBtnEnh m_btnNg[InspectTimeTotalCount];
	CBtnEnh m_btnAutoContact[InspectTimeTotalCount];
	CBtnEnh m_btnAoi[InspectTimeTotalCount];
	CBtnEnh m_btnViewing[InspectTimeTotalCount];
	CBtnEnh m_btnOtp[InspectTimeTotalCount];
	CBtnEnh m_btnTp[InspectTimeTotalCount];
	CBtnEnh m_btnAlign[InspectTimeTotalCount];
	CBtnEnh m_btnTrayOut[InspectTimeTotalCount];
	CBtnEnh m_btnSum[InspectTimeTotalCount];

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();

	void UpdateDisplay(int nShift);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnDy();
	void ClickBtnNt();

	int m_iSelectShift;
	void ClickDyDataReset();
	AOIProductionData SumProduction;
	BOOL PreTranslateMessage(MSG* pMsg);
};
#endif