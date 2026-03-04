#pragma once


#include "ImageViewNG.h"
#include "MyListCtrl.h"
#include "DFSInfo.h"
// CDefectView 폼 뷰입니다.

class CDefectView : public CFormView
{
	DECLARE_DYNCREATE(CDefectView)

protected:


public:
	enum { IDD = DEFECT_VIEW };

	CDefectView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CDefectView();
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
	void SetComboBoxReadOnly(int item);
	void SetDefectCodeValue(CDFSInfo DFS);
	void OnInitialList();
	BOOL GetPathMethod();
	BOOL bOpenFlag;

	CString strDfsPath;
	CString m_strPanelID;

	CImageViewNG m_ctrlImageMapView;
	CMyListCtrl m_ctrlDefectList;
	CComboBox m_cmbDefectCommand;
	VecDefectInfo m_vecDefectInfo;
	DefectInfo defectinfo;
	CDFSInfo DfsInfo;

	CBtnEnh m_btnAOIINFO;
	CBtnEnh m_btnAOTINFO;
	CBtnEnh m_btnVIEWINFO;
	CBtnEnh m_btnSUMINFO;

	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEnUpdatePanelIdData();
	afx_msg void OnSelchangeCmbDefectCommand();
	DECLARE_EVENTSINK_MAP()
	void OnSelchangeCmbDefectClean();
	void OnClickIdbBtnUnit();
	void ClickSendAlignCommand();
};