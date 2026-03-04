#pragma once

#if _SYSTEM_AMTAFT_
// CDlgOkGrade 대화 상자입니다.

class CDlgOkGrade : public CDialog
{
	DECLARE_DYNAMIC(CDlgOkGrade)

public:
	CDlgOkGrade(int m_ishift);   // 표준 생성자입니다.
	virtual ~CDlgOkGrade();

	int m_ishift;
	CBtnEnh m_btnZoneGrade[3];

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_OK_GRADE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
#endif