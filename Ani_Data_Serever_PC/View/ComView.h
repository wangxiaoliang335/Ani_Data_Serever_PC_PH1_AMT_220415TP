#pragma once
#if _SYSTEM_AMTAFT_

#include "ClrListBox.h"
#include "StringManager.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "btnenh.h"
#include "MachineManager.h" //210422 yjlim

#define MAX_CONNECTION 10
#define MAX_CONNECTION_PG 16


class CComView : public CFormView, public IStringChangeListener
{
	DECLARE_DYNCREATE(CComView)

public:
	enum { IDD = AMT_COM_VIEW };

	CComView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CComView();

	CListCtrl m_ctrlComList;

	CComboBox m_cmbPgCommand;
	CComboBox m_cmbVisionCommand;
	CComboBox m_cmbAlignCommand;
	CComboBox m_cmbTpCommand;
	CComboBox m_cmbViewingAngleCommand;
	CComboBox m_cmbPgChCommand;
	CComboBox m_cmbFFUCommand;
	CComboBox m_cmbAlignChCommand;
	CComboBox m_cmbOpvCommand;
	CComboBox m_cmbLumitopCommand;


	CComboBox m_cmbVisionCount;
	CComboBox m_cmbViewingAngleCount;
	CComboBox m_cmbTpCount;
	CComboBox m_cmbOpvCount;
	CComboBox m_cmbLumitopCount;

	CBtnEnh m_OpvState;
	CBtnEnh m_VisionState;
	CBtnEnh m_AlignState;
	CBtnEnh m_ViewingAngleState;
	CBtnEnh m_PgState;
	CBtnEnh m_PlcState;
	CBtnEnh m_TpState;
	CBtnEnh m_FFUState;
	CBtnEnh m_LumitopState;

	CCriticalSection m_csList;

	void StringChanged();
	void ChangeAlignList();

protected:
	void UpdateStateButton(BOOL bState, CBtnEnh* pStateBtn);

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
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_EVENTSINK_MAP()
	
	void OnClickSendVisionCommand();
	void OnClickSendAlignCommand();
	void OnClickSendViewingAngleCommand();
	void OnClickSendTpCommand();
	void OnClickSendPgCommand();
	void OnClickSendOpvCommand();

	void SetComboBoxReadOnly(int item);
	void StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi);
	BOOL PlcStart();

	void ClickSendFfuCommand2();
	void ClickFfuState();
	void ClickSendLumitopCommand();
public:
	//>> 210422 yjlim
	vector<CMachineManager*> m_SocketClient; //210123 yjlim
	BOOL m_bPcConnect[1];
	BOOL m_bFlag[1];

	CBtnEnh m_Btn_Port[1];
	//<<
};
#endif