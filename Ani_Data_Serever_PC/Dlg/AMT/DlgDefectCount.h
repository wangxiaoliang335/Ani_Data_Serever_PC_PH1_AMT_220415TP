#pragma once



// CDlgDefectCount 폼 뷰입니다.
#if _SYSTEM_AMTAFT_
class CDlgDefectCount : public CDialog
{
	DECLARE_DYNCREATE(CDlgDefectCount)

public:
	CDlgDefectCount(CWnd* pParent = NULL);
	virtual ~CDlgDefectCount();

	enum { IDD = DLG_DEFECT_COUNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

public:
	CBtnEnh m_btnTitleLabel[eNumShift][6];
	CBtnEnh m_btnOpvTotalNgSum[eNumShift];
	CBtnEnh m_btnOpvOkSum[eNumShift];
	CBtnEnh m_btnOpvNgSum[eNumShift];
	CBtnEnh m_btnAoiDefectTotalSum[eNumShift][6];
	CBtnEnh m_btnOpvDefectTotalSum[eNumShift][6];
	CBtnEnh m_btnMatchDefectTotalSum[eNumShift][6];
	CBtnEnh m_btnOverKillDefectTotalSum[eNumShift][6];
	CBtnEnh m_btnUnderKillDefectTotalSum[eNumShift][6];

	virtual BOOL OnInitDialog();

	void UpdateDisplay(int nShift);
	void SetDefectTitle();
	DECLARE_EVENTSINK_MAP()
	void OnClickDataReset();
	void PanelDefectListView();
	void TitleDefectListView();
	void ClickTitle();
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

extern CDlgDefectCount *g_DlgDefectCount;
#endif