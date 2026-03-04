#pragma once
#if _SYSTEM_GAMMA_

#include "ClrListBox.h"
#include "StringManager.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "btnenh.h"

class CComGammaView : public CFormView, public IStringChangeListener
{
	DECLARE_DYNCREATE(CComGammaView)

public:
	enum { IDD = GAMMA_COM_VIEW };

	CComGammaView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CComGammaView();

	CListCtrl m_ctrlComList;

	CComboBox m_cmbAlignCommand;
	CComboBox m_cmbAlignChCommand;
	CComboBox m_cmbPgCommand;
	CComboBox m_cmbPgChCommand;
	CComboBox m_cmbFFUCommand;

	CBtnEnh m_AlignState;
	CBtnEnh m_PgState;
	CBtnEnh m_FFUState;

	CCriticalSection m_csList;

	void StringChanged();
	void StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi);

protected:
	void UpdateStateButton(BOOL bState, CBtnEnh* pStateBtn);
	void SetComboBoxReadOnly(int item);
	BOOL PlcStart();

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	void ClickSendAlignCommand();
	void ClickSendPgCommand();
	void ClickSendFFUCommand();

public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void ChangeAlignList();
	DECLARE_EVENTSINK_MAP()

	void ClickAlignTrdTest1();
	void ClickFfuState();
};
#endif