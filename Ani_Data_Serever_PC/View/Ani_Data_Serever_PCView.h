
// Ani_Data_Serever_PCView.h : CAni_Data_Serever_PCView Ŭ������ �������̽�
//

#pragma once

#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "DlgMainLog.h"
#include "DlgInspect.h"
#include "DlgTimeInspect.h"
#else
#include "DlgGammaMain.h"
#include "DlgGammaInspect.h"
#include "DlgGammaTimeInspect.h"
#endif
#include "DlgSetRank.h"
#include "DLGSetSystem.h"
#include "DLGSetVision.h"
#include "DlgTactTimeHistory.h"
#include "DlgAlarmHistory.h"
#include "SetTimerDlg.h"
#include "DlgIdCardHistory.h"

class CMainFrame;

class CAni_Data_Serever_PCView : public CFormView
{
protected: // serialization������ ��������ϴ�.
	CAni_Data_Serever_PCView();
	DECLARE_DYNCREATE(CAni_Data_Serever_PCView)

	// Ư���Դϴ�.
public:

#if _SYSTEM_AMTAFT_
	CDlgMainView *m_pMain;
	CDlgMainLog *m_pMainLog;
	CDlgInspect *m_pInspect;
	CDlgTimeInspect *m_pTimeInspet;
#else
	CDlgGammaMain *m_pMain;
	CDlgGammaInspect *m_pInspect;
	CDlgGammaTimeInspect *m_pTimeInspet;
#endif
	CDlgSetRank* m_SetRank;
	CDLGSetSystem* m_SetSystem;
	CDLGSetVision *m_SetVision;
	CDlgTactTimeHistory *m_pTactTimeHistory;
	CDlgAlarmHistory *m_pAlarmHistory;
	CDlgIdCardHistory *m_pIdCardHistory;
	enum{ IDD = MAIN_VIEW };

	// �۾��Դϴ�.
public:

	// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

	// �����Դϴ�.
public:
	virtual ~CAni_Data_Serever_PCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

private:
	CMainFrame* pMainFrame;
	// After BtnEnh option-group SetValue, OCX may restore black skin ? re-apply readability
	void ReapplyMainViewTopStripReadability();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	CBtnEnh m_btnKor;
	CBtnEnh m_btnEng;
	CBtnEnh m_btnChi;
	CBtnEnh m_btnLabelName;
	CSetTimerDlg *m_SetTimerDlg;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnPaint();
	DECLARE_EVENTSINK_MAP()
	void OnClickIdbBtnStart();
	void OnClickIdbBtnDefectCount();
	void OnClickIdbBtnInspect();
	void OnClickIdbBtnAlarm();
	void OnClickIdbBtnKor();
	void OnClickIdbBtnEng();
	void OnClickIdbBtnLanguage();
	void OnClickIdbBtnSetTimer();
	void ClickBtnTactTime();
	void ClickBtnTimeInspect();
	void OnClickIdbBtnSetRank();
	void ClickBtnSetsystem();
	void ClickBtnCardReader();
	void ClickBtnSetVision();
	void ClickBtnSetngrank2();
	void ClickBtnSetngrank3();
	afx_msg void OnBnClickedButton5();
	void SendPlcDefectCode(int iNum, DfsDataValue PanelData, int iType);
};