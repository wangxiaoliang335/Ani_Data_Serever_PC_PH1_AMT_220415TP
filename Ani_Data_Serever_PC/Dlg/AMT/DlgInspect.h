#pragma once

#if _SYSTEM_AMTAFT_
// CDlgInspect 대화 상자입니다.

class CDlgInspect : public CDialog
{
	DECLARE_DYNAMIC(CDlgInspect)

public:
	CDlgInspect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgInspect();

// 대화 상자 데이터입니다.
	enum { IDD = DLG_INSPECT_AMT_AFT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public :

	CBtnEnh m_btnLabelName[eNumShift];
	CBtnEnh m_btnTotalSum[eNumShift];
	CBtnEnh m_btnGoodTotalSum[eNumShift];
	CBtnEnh m_btnNgTotalSum[eNumShift];
	CBtnEnh m_btnAutoContactTotalSum[eNumShift];
	CBtnEnh m_btnAlignTotalSum[eNumShift];
	CBtnEnh m_btnAoiTotalSum[eNumShift];
	CBtnEnh m_btnViewingTotalSum[eNumShift];
	CBtnEnh m_btnOtpTotalSum[eNumShift];
	CBtnEnh m_btnTpTotalSum[eNumShift];
	CBtnEnh m_btnTrayOutTotalSum[eNumShift];

	CBtnEnh m_btnZoneTotal[eNumShift][MaxZone];
	CBtnEnh m_btnGoodTotal[eNumShift][MaxZone];
	CBtnEnh m_btnNgotal[eNumShift][MaxZone];
	CBtnEnh m_btnAutoContactTotal[eNumShift][MaxZone];
	CBtnEnh m_btnAlignTotal[eNumShift][MaxZone];
	CBtnEnh m_btnAoiTotal[eNumShift][MaxZone];
	CBtnEnh m_btnViewingTotal[eNumShift][MaxZone];
	CBtnEnh m_btnOtpTotal[eNumShift][MaxZone];
	CBtnEnh m_btnTpTotal[eNumShift][MaxZone];
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();

	void UpdateDisplay(int nShift);	
	DECLARE_EVENTSINK_MAP()
	void ClickChResult();
	void OnClickDataReset();
	void ClickInspectResultCode();
	void ClicGoodGrade();
	BOOL PreTranslateMessage(MSG* pMsg);
};
#endif