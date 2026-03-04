#pragma once



// CAddrView 폼 뷰입니다.

class CAddrView : public CFormView
{
	DECLARE_DYNCREATE(CAddrView)

protected:
	

public:
	enum { IDD = ADDR_VIEW };

	CAddrView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CAddrView();

	struct AddrData
	{
		int m_iNum;
		CString m_strAddrTitle;
		std::vector<CString> m_strAddr;
		std::vector<CString> m_strAddrName;
	};


	std::vector<AddrData> m_addrPLCDataBit;
	std::vector<AddrData> m_addrPLCDataWord;
	std::vector<AddrData> m_addrPCDataBit;
	std::vector<AddrData> m_addrPCDataWord;
	CBtnEnh m_addrBitPLCTitle;
	CBtnEnh m_addrWordPLCTitle;
	CBtnEnh m_addrBitPCTitle;
	CBtnEnh m_addrWordPCTitle;

	CBtnEnh m_addrBitPLCName[16];
	CBtnEnh m_addrBitPLCValue[16];
	CBtnEnh m_addrBitPLCAddr[16];
	CBtnEnh m_addrBitPCName[16];
	CBtnEnh m_addrBitPCValue[16];
	CBtnEnh m_addrBitPCAddr[16];

	CBtnEnh m_addrWordPLCName[8];
	CBtnEnh m_addrWordPLCValue[8];
	CBtnEnh m_addrWordPLCAddr[8];
	CBtnEnh m_addrWordPCName[8];
	CBtnEnh m_addrWordPCValue[8];
	CBtnEnh m_addrWordPCAddr[8];

	int m_iSelectPLCNum;
	int m_iSelectPCNum;
	USHORT m_shortValue;

	void SetAddrData();
	void SeteValuePLCReset();
	void SeteValuePCReset();
	void SetValue(int iValue);

	void StringChanged();
	void StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi);

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_EVENTSINK_MAP()
	void OnClickPLCPrev();
	void OnClickPLCNext();
	void OnClickPCPrev2();
	void OnClickPCNext2();
};


