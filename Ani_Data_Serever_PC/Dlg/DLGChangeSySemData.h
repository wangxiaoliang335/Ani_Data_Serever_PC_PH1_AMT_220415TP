#pragma once


// CDLGChangeSySemData 대화 상자입니다.

class CDLGChangeSySemData : public CDialogEx
{
	DECLARE_DYNAMIC(CDLGChangeSySemData)

public:
	CDLGChangeSySemData(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CDLGChangeSySemData(int nParts, CString strContent, int iSetType);
	BOOL OnInitDialog();
	CString GetNewContent();
	virtual ~CDLGChangeSySemData();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_CHANGE_SYSTEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	int nPartProtect;
	CString strContentProtect;
	CString strNewContentProtect;
	int m_iSetType;

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void ClickBtnCancle();
	void ClickBtnSure();
};
