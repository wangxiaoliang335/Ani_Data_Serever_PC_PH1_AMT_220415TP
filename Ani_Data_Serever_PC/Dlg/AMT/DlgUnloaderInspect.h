#pragma once

// CDlgUnloaderInspect 대화 상자입니다.
#if _SYSTEM_AMTAFT_
class CDlgUnloaderInspect : public CDialog
{
	DECLARE_DYNAMIC(CDlgUnloaderInspect)

public:
	CDlgUnloaderInspect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgUnloaderInspect();

// 대화 상자 데이터입니다.
	enum { IDD = DLG_INSPECT_UNLOADER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public :
	CBtnEnh m_btnTotalSum[eNumShift];
	CBtnEnh m_btnGoodTotalSum[eNumShift];
	CBtnEnh m_btnNgTotalSum[eNumShift];
	CBtnEnh m_btnAutoContactTotalSum[eNumShift];
	CBtnEnh m_btnAlignTotalSum[eNumShift];
	CBtnEnh m_btnManualOpvTotalSum[eNumShift];
	CBtnEnh m_btnManualTouchTotalSum[eNumShift];
	CBtnEnh m_btnManualGammaTotalSum[eNumShift];
	CBtnEnh m_btnBufferTrayTotalSum[eNumShift];
	CBtnEnh m_btnManualContactTotalSum[eNumShift];
	CBtnEnh m_btnSampleTotalSum[eNumShift];
	
	CBtnEnh m_btnChTotal[eNumShift][ChMaxCount];
	CBtnEnh m_btnGoodTotal[eNumShift][ChMaxCount];
	CBtnEnh m_btnNgTotal[eNumShift][ChMaxCount];
	CBtnEnh m_btnAutoContactTotal[eNumShift][ChMaxCount];
	CBtnEnh m_btnAlignTotal[eNumShift][ChMaxCount];
	CBtnEnh m_btnManualOpvTotal[eNumShift][ChMaxCount];
	CBtnEnh m_btnManualTouchTotal[eNumShift][ChMaxCount];
	CBtnEnh m_btnManualGammaTotal[eNumShift][ChMaxCount];
	CBtnEnh m_btnBufferTrayTotal[eNumShift][ChMaxCount];
	CBtnEnh m_btnManualContactTotal[eNumShift][ChMaxCount];
	CBtnEnh m_btnSampleTotal[eNumShift][ChMaxCount];

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();

	void UpdateDisplay(int nShift);	
	DECLARE_EVENTSINK_MAP()
	void OnClickDataReset();
	BOOL PreTranslateMessage(MSG* pMsg);
};
#endif