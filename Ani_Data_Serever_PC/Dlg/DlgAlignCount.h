#pragma once


// CDlgAlignCount 대화 상자입니다.

class CDlgAlignCount : public CDialog
{
	DECLARE_DYNAMIC(CDlgAlignCount)

public:
	CDlgAlignCount(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgAlignCount();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ALIGN_COUNT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_iAlignCount;

	CComboBox m_cmbPcCount;
	CBtnEnh m_btnMachinePc[MaxAlignCount];
	CBtnEnh m_btnPatternAlign[MaxAlignCount];
	CBtnEnh m_btnTrayCheck[MaxAlignCount];
	CBtnEnh m_btnTrayLowerAlign[MaxAlignCount];
	CBtnEnh m_btnTrayAlign[MaxAlignCount];

	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboMachineCount();
	DECLARE_EVENTSINK_MAP()
	void ClickAlignInspect();
	void ClickBtnOk();
};
