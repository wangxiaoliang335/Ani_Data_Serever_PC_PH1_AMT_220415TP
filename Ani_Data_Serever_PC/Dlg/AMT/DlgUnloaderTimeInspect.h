#pragma once

// CDlgUnloaderTimeInspect 대화 상자입니다.
#if _SYSTEM_AMTAFT_
class CDlgUnloaderTimeInspect : public CDialog
{
	DECLARE_DYNAMIC(CDlgUnloaderTimeInspect)

public:
	CDlgUnloaderTimeInspect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgUnloaderTimeInspect();

	// 대화 상자 데이터입니다.
	enum { IDD = DLG_TIME_INSPECT_UNLOADER};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	CBtnEnh m_btnTimeTotal;
	CBtnEnh m_btnSumTotal;
	CBtnEnh m_btnGoodTotal;
	CBtnEnh m_btnNgTotal;
	CBtnEnh m_btnAutoContactTotal;
	CBtnEnh m_btnAlignTotal;
	CBtnEnh m_btnManualOpvTotal;
	CBtnEnh m_btnManualTouchTotal;
	CBtnEnh m_btnManualGammaTotal;
	CBtnEnh m_btnSampleTotal;
	CBtnEnh m_btnBufferTrayTotal;
	
	CBtnEnh m_btnTime[InspectTimeTotalCount];
	CBtnEnh m_btnSum[InspectTimeTotalCount];
	CBtnEnh m_btnGood[InspectTimeTotalCount];
	CBtnEnh m_btnNg[InspectTimeTotalCount];
	CBtnEnh m_btnAutoContact[InspectTimeTotalCount];
	CBtnEnh m_btnAlign[InspectTimeTotalCount];
	CBtnEnh m_btnManualOpv[InspectTimeTotalCount];
	CBtnEnh m_btnManualTouch[InspectTimeTotalCount];
	CBtnEnh m_btnManualGamma[InspectTimeTotalCount];
	CBtnEnh m_btnSample[InspectTimeTotalCount];
	CBtnEnh m_btnBufferTray[InspectTimeTotalCount];
	

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();

	void UpdateDisplay(int nShift);
	DECLARE_EVENTSINK_MAP()
	void ClickBtnDy();
	void ClickBtnNt();

	int m_iSelectShift;
	void ClickDyDataReset();
	ULDProductionData SumProduction;
	BOOL PreTranslateMessage(MSG* pMsg);
};
#endif