#pragma once

#if _SYSTEM_GAMMA_
// CDlgAlignStatus 대화 상자입니다.

class CDlgGammaAlignStatus : public CDialog
{
	DECLARE_DYNAMIC(CDlgGammaAlignStatus)

public:
	CDlgGammaAlignStatus(int iShift, int iContactAlign);   // 표준 생성자입니다.
	virtual ~CDlgGammaAlignStatus();

	int m_iShift;
	int m_iContactAlign;

	CBtnEnh m_btnStageGood[MaxGammaStage][ChMaxCount];
	CBtnEnh m_btnStageNg[MaxGammaStage][ChMaxCount];

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ALGIN_STATUS_GAMMA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
#endif