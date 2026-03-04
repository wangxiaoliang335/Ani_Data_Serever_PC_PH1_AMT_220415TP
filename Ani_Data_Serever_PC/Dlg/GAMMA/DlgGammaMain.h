#pragma once
#if _SYSTEM_GAMMA_

#include "Ani_Data_Serever_PC.h"
#include "afxwin.h"
#include "ClrListBox.h"

// CDlgGammaMain 폼 뷰입니다.

class CDlgGammaMain : public CDialog
{
	DECLARE_DYNAMIC(CDlgGammaMain)

public:
	CDlgGammaMain(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgGammaMain();

	// 대화 상자 데이터입니다.
	enum { IDD = DLG_GAMMA_MAIN_LOG_BOX1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void SetWindowPosition();
	DECLARE_EVENTSINK_MAP()

	CBtnEnh m_btnAlignModelChange;
public:
	CClrListBox m_AlignListBox;
	CClrListBox m_PlcListBox;
	CClrListBox m_PgListBox;

	void SocketServerOpen();
	void OnClickIdbAlignLogClean();
	void OnClickIdbPgClean();
	void OnClickIdbPlcLogClean();
	void StringChanged();
	void StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi);
	afx_msg void OnTimer(UINT_PTR nIDEvent);


	int m_iLogDispNum = 0;

	void ClickBtnAlignLog();
};
extern CDlgGammaMain *g_MainLog;

#endif