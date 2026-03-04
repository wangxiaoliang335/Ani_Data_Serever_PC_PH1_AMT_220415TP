#pragma once

#if _SYSTEM_AMTAFT_
// CDlgAlignStatus 대화 상자입니다.

class CDlgAlignStatus : public CDialog
{
	DECLARE_DYNAMIC(CDlgAlignStatus)

public:
	CDlgAlignStatus(int ishift, int iContactAlign);   // 표준 생성자입니다.
	virtual ~CDlgAlignStatus();

	int m_ishift;
	int m_iContactAlign;

	CBtnEnh m_btnZoneGood[MaxZone][PanelMaxCount];
	CBtnEnh m_btnZoneNg[MaxZone][PanelMaxCount];

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ALGIN_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
#endif