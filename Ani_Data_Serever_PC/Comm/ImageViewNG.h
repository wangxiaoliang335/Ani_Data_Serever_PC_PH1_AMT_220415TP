#if !defined(AFX_IMAGEVIEWNG_H__14065FBD_AC2E_493B_8C3B_ABE358C138C5__INCLUDED_)
#define AFX_IMAGEVIEWNG_H__14065FBD_AC2E_493B_8C3B_ABE358C138C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageView.h : header file
//

#include "DefectInfo.h"

struct DefectPos
{
	int iIndex;
	int iX;
	int iY;
	BOOL bFull;
	int iType;
	CString strDefectCode;
};

typedef std::vector<DefectPos> VecDefectPos;

class CImageViewNG : public CStatic
{
	CWnd*		m_pParentWnd;

	CMyvvImage			m_vvImage;

	BOOL				m_bFixedSize;

	BOOL		m_bMapView;
	BOOL		m_bShowGrid;

	CCriticalSection	m_csImageData;
	VecDefectPos		m_vecDefectPos;
	DefectPos			m_selectedDefectPos;
	//CPoint				m_clickPos;

	//TEST CODE
	VecDefectInfo m_vecDefectInfo;
	
public:
	CImageViewNG();           // protected constructor used by dynamic creation
	virtual ~CImageViewNG();
	DECLARE_DYNCREATE(CImageViewNG)
	

public:
	BOOL Initialize(CWnd* pParentWnd, int depth, int channel);
	void Clear(CvScalar color = CV_RGB(0, 0, 0));
	void UpdateDefectPos(DefectInfo& defectInfo);
	void ShowMessage(int posX, int posY, CvScalar color, CvFont& font, LPCTSTR message);
	DefectPos& GetSelectedDefectPos() {	return m_selectedDefectPos; }
	
	void SetImage(IplImage* pImage);

// Overrides
	// ClassWizard generated virtual function overrides
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void PostNcDestroy();

	void AddDefectPos(IplImage* pImage, DefectPos& defectPos);

	DECLARE_MESSAGE_MAP()

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
//	afx_msg UINT OnNotifyFormat(CWnd *pWnd, UINT nCommand);
	afx_msg void OnPaint();
	afx_msg void OnStnClicked();
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEVIEWNG_H__14065FBD_AC2E_493B_8C3B_ABE358C138C5__INCLUDED_)
