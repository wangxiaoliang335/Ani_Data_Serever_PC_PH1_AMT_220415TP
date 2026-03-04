#pragma once
#if _SYSTEM_AMTAFT_

// CDlgMainLog 대화 상자입니다.
#include "resource.h"
#include "btnenh.h"
#include "ClrListBox.h"

class CDlgMainLog : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMainLog)

public:
	CDlgMainLog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMainLog();

// 대화 상자 데이터입니다.
	enum { IDD = DLG_MAIN_LOG_BOX };
	CClrListBox m_PlcListBox;
	CClrListBox m_TouchListBox;
	CClrListBox m_AlignListBox;
	CClrListBox m_PgListBox;
	void StringChanged();
	void StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void OnClickIdbAlignLogClean();
	void OnClickIdbTouchLogClean();
	void OnClickIdbPgClean();
	void OnClickIdbPlcLogClean();
	CBtnEnh m_btnAlignModelChange;
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);

};
extern CDlgMainLog *g_MainLog;

#endif