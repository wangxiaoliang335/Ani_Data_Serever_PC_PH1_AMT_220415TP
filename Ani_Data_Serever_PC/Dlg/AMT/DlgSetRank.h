#pragma once

#include "EZIni.h"
#include "afxcmn.h"
#include "DlgAddRankCode.h"
#include "ListCtrlEx.h"
#include <map>


// CDlgSetRank 대화 상자입니다.

class CDlgSetRank : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetRank)
	
	
	CListCtrlEx m_RankCtrlList;

public:
	CDlgSetRank(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgSetRank();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_SETRANK };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	DECLARE_EVENTSINK_MAP()
	void SetNewRankItem();
	void ReLoadRank();
	void LoadDisply();

	void SaveRank();
	BOOL OnInitDialog();
	
	void ResetRankData();
	void SaveChangedData();
	void CancelEmpty();
	
	
	BOOL flag1=TRUE;

public:
	void OkayQuitFunction();


	std::vector<CString> listOfKeyNames;
	
	int  m_iFocusRowPosition;
	
	void StringChanged();
	void StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi);
	void SelectPartFuncion();
	
	int  m_nSelectParts;
	CBtnEnh m_btnAOIPart;
	CBtnEnh m_btnPlcSendNumber;

	CBtnEnh *pBtnEnh;

	void ClickDelete();

	void ClickChange();
	void AddRankCode();
	void ClickRackAdd();
	void ClickRankDelete();
	void ClickSetNumber();

	afx_msg LRESULT ClickEvent(WPARAM wParam, LPARAM lParam);
};