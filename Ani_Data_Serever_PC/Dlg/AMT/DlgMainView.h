#pragma once
#if _SYSTEM_AMTAFT_

#include "Ani_Data_Serever_PC.h"
#include "afxwin.h"
#include "VisionThread.h"
#include "ViewingAngleThread.h"
#include "LumitopThread.h"
#include "ClrListBox.h"

// CDlgMainView 폼 뷰입니다.

class CDlgMainView : public CDialog
{
	DECLARE_DYNAMIC(CDlgMainView)

public:
	CDlgMainView(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMainView();

	// 대화 상자 데이터입니다.
	enum { IDD = DLG_MAIN_BOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void SetWindowPosition();
	DECLARE_EVENTSINK_MAP()
	void OnClickIdbSystem();
	void SocketServerOpen();

public:

	CClrListBox m_ViewingAngleListBox[2];
	CClrListBox m_VisionListBox[2];

	virtual void OnCancel();
	virtual void OnOK();
	void OnClickIdbViewingAngleLogClean();
	void OnClickIdbViewingAngleLogClean2();
	void OnClickIdbVisionPcLogClean();
	void OnClickIdbVisionPcLogClean3();
	void StringChanged();
	void StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi);

	CBtnEnh m_btnViewingAngleModelChange1;
	CBtnEnh m_btnViewingAngleModelChange2;
	CBtnEnh m_btnViewingAngleModelChange3;
	CBtnEnh m_btnViewingAngleModelChange4;
	CBtnEnh m_btnVisionModelChange1;
	CBtnEnh m_btnVisionModelChange2;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
extern CDlgMainView *g_DlgMainView;

#endif