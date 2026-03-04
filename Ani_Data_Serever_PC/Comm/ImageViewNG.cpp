#include "stdafx.h"
#include "ImageViewNG.h"
#include "Ani_Data_Serever_PC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CImageViewNG, CStatic)

CImageViewNG::CImageViewNG()
{
	m_bFixedSize = FALSE;
	m_bMapView = FALSE;
	m_bShowGrid = FALSE;
}

CImageViewNG::~CImageViewNG()
{
	
}

BOOL CImageViewNG::Initialize(CWnd* pParentWnd, int depth, int channel)
{
	m_pParentWnd = pParentWnd;
	
	CRect rect;
	GetClientRect(&rect);

	m_vvImage.Destroy();
	m_vvImage.Create2(rect.Width(), rect.Height(), depth, channel);
	
	cvSetZero(m_vvImage.GetImage());
	Clear(cvScalar(128,128,128));
	
	this->ModifyStyleEx(1, SS_NOTIFY);

	//UpdateDefectPos(m_vecDefectInfo);



	return TRUE;
}

BEGIN_MESSAGE_MAP(CImageViewNG, CStatic)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
//	ON_WM_RBUTTONUP()
//	ON_WM_NOTIFYFORMAT()
	ON_WM_PAINT()
	ON_CONTROL_REFLECT(STN_CLICKED, &CImageViewNG::OnStnClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageViewNG drawing

void CImageViewNG::OnDraw(CDC* pDC)
{
	if(m_vvImage.GetImage()) 
	{
		CRect rect;

		if(!m_bFixedSize) {
			GetClientRect(&rect);
		} else {
			rect = CRect(0, 0, m_vvImage.Width(), m_vvImage.Height());
		}
				
		m_vvImage.DrawToHDC(pDC, rect);	
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImageViewNG diagnostics



#ifdef _DEBUG
void CImageViewNG::AssertValid() const
{
	CStatic::AssertValid();
}

void CImageViewNG::Dump(CDumpContext& dc) const
{
	CStatic::Dump(dc);
}
#endif //_DEBUG

void CImageViewNG::OnMouseMove(UINT nFlags, CPoint point)
{
	IplImage *pCurImage = m_vvImage.GetImage();

	if(!pCurImage)
		return;
	
	CStatic::OnMouseMove(nFlags, point);
}

//#define SEARCH_DISTANCE 30
#define SEARCH_DISTANCE 70		//<< 171010 jwan

void CImageViewNG::OnLButtonUp(UINT nFlags, CPoint point)
{	
	float minDistance = FLT_MAX;
	
	IplImage *pCurImage = m_vvImage.GetImage();
	if (!pCurImage)
		return;

	CRect rectStatic;
	this->GetWindowRect(&rectStatic);

	float fScaleX = pCurImage->width*1.0f / rectStatic.Width();
	float fScaleY = pCurImage->height*1.0f / rectStatic.Height();

	CPoint m_clickPos;
	//>> 171010 jwan - 좌표계 변경요청후 수정
	m_clickPos.x = (LONG)(fScaleX * point.x);
	m_clickPos.y = pCurImage->height - (LONG)(fScaleY * point.y);
	//m_clickPos.y = (LONG)(fScaleY * point.y);

	m_selectedDefectPos.iIndex = -1;
	/*std::for_each(m_vecDefectPos.begin(), m_vecDefectPos.end(), [this, &m_clickPos, &minDistance, &rectStatic](DefectPos& defectPos)
	{
		int distance = pow((double)(m_clickPos.x - defectPos.iX), 2.0) + pow((double)(m_clickPos.y - defectPos.iY), 2.0);

		if (distance < minDistance)
		{
			minDistance = distance;
			m_selectedDefectPos = defectPos;
		}
	});*/

	for (int ii = 0; ii < m_vecDefectPos.size(); ii++)
	{
		int distance = pow((double)(m_clickPos.x - m_vecDefectPos[ii].iX), 2.0) + pow((double)(m_clickPos.y - m_vecDefectPos[ii].iY), 2.0);

		if (distance < minDistance)
		{
			minDistance = distance;
			m_selectedDefectPos = m_vecDefectPos[ii];
		}
	}


	//if (minDistance > SEARCH_DISTANCE)
	//{
	//	m_selectedDefectPos.iIndex = -1;
	//	m_pParentWnd->PostMessage(_WM_USER_DEFECT_UNSELECTED, m_panelIndex, 0);
	//}
	//else
	//{
	//	m_pParentWnd->PostMessage(_WM_USER_DEFECT_SELECTED, m_panelIndex, 0);
	//}

	CStatic::OnLButtonUp(nFlags, point);
}

void CImageViewNG::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class

	CStatic::PostNcDestroy();
}

//void CImageViewNG::OnRButtonUp(UINT nFlags, CPoint point)
//{
//	CStatic::OnRButtonUp(nFlags, point);
//}

void CImageViewNG::SetImage(IplImage* pImage)
{
	cvResize(pImage, m_vvImage.GetImage());
}

void CImageViewNG::Clear(CvScalar color)
{
	cvSet(m_vvImage.GetImage(), color);
}

void CImageViewNG::ShowMessage(int posX, int posY, CvScalar color, CvFont& font, LPCTSTR message)
{
	IplImage *pImgae = m_vvImage.GetImage();
	IplImage *pImgaeFlip = cvCreateImage(cvSize( pImgae->width, pImgae->height), pImgae->depth, pImgae->nChannels);
	cvFlip(pImgae, pImgaeFlip, 0);
	cvPutText(pImgaeFlip, CStringA(message), cvPoint(posX, posY), &font, color);
	cvFlip(pImgaeFlip, pImgae, 0);
	//m_vvImage.SetImage(pImgae);
}

void CImageViewNG::UpdateDefectPos(DefectInfo& defectInfo)
{
	IplImage* pImage = m_vvImage.GetImage();

	if (pImage == NULL)
		return;

	Clear(cvScalar(128, 128, 128));
	//cvZero(pImage);
	int imgWidth = pImage->width;
	int imgHeight = pImage->height;

	if (m_bShowGrid)
	{
		CRect rect;
		GetClientRect(&rect);

		cvLine(pImage, cvPoint(0, rect.bottom / 3), cvPoint(rect.right, rect.bottom / 3), CV_RGB(0, 0, 255), 2);
		cvLine(pImage, cvPoint(0, rect.bottom / 3 * 2), cvPoint(rect.right, rect.bottom / 3 * 2), CV_RGB(0, 0, 255), 2);
		cvLine(pImage, cvPoint(rect.right / 3, 0), cvPoint(rect.right / 3, rect.bottom), CV_RGB(0, 0, 255), 2);
		cvLine(pImage, cvPoint(rect.right / 3 * 2, 0), cvPoint(rect.right / 3 * 2, rect.bottom), CV_RGB(0, 0, 255), 2);
	}

	m_vecDefectPos.clear();

	int iPosX = defectInfo.ptPos.x;
	int iPosY = defectInfo.ptPos.y;

	float fResizeRateX = 0.3f;
	if (defectInfo.iPanel_Size_w > 0 && defectInfo.iPanel_Size_h > 0)
		fResizeRateX = (float)imgWidth / (float)defectInfo.iPanel_Size_w;

	float fResizeRateY = 0.3f;
	if (defectInfo.iPanel_Size_h > 0)
		fResizeRateY = (float)imgHeight / (float)defectInfo.iPanel_Size_h;

	DefectPos defectPos;

	//>> 171010 jwan - 좌표계 변경요청후 수정
	defectPos.iX = imgWidth - __max(__min((int)(iPosX * fResizeRateX), imgWidth - 2), 2);			//외곽 빼기
	defectPos.iY = __max(__min((int)(iPosY * fResizeRateY), imgHeight - 2), 2);

	defectPos.bFull = FALSE;
	defectPos.iType = defectInfo.iType;
	defectPos.strDefectCode = defectInfo.strDefectCode;
	defectPos.iIndex = defectInfo.iIndex;

	cvCircle(pImage, cvPoint(defectPos.iX, defectPos.iY), 10, CVRED, 4);

//	std::for_each(vecDefectInfo.begin(), vecDefectInfo.end(), [this, pImage, imgWidth, imgHeight](DefectInfo& defectInfo)
//	{
//		//>> 170922 mojw
//		//int iPosX = defectInfo.ptPos.x + defectInfo.rectArea.width / 2;
//		//int iPosY = defectInfo.ptPos.y + defectInfo.rectArea.height / 2; 
//		//<<
//		//int iPosX = /*imgWidth - */defectInfo.ptPos.x + defectInfo.rectArea.width / 2; //170925 jwmo 임시 이미지돌리기
//		//int iPosY = /*imgHeight - */defectInfo.ptPos.y + defectInfo.rectArea.height / 2;
//
//		int iPosX = defectInfo.ptPos.x;
//		int iPosY = defectInfo.ptPos.y;
//
//		float fResizeRateX = 0.3f;
//		if (defectInfo.iPanel_Size_w > 0 && defectInfo.iPanel_Size_h > 0)
//			fResizeRateX = (float)imgWidth / (float)defectInfo.iPanel_Size_w;
//
//		float fResizeRateY = 0.3f;
//		if (defectInfo.iPanel_Size_h > 0)
//			fResizeRateY = (float)imgHeight / (float)defectInfo.iPanel_Size_h;
//
//		DefectPos defectPos;		
////		defectPos.iX = __max(__min((int)(iPosX * fResizeRateX), imgWidth - 2), 2);
////		//defectPos.iY = __max(__min((int)(iPosY * fResizeRateY), imgHeight - 2), 2);
////		
////		//>> 170830 jwan - 좌표계가 일치하지 않아 변경.
////		defectPos.iY = imgHeight-__max(__min((int)(iPosY * fResizeRateY), imgHeight - 2), 2);
//
////>> 171010 jwan - 좌표계 변경요청후 수정
//		defectPos.iX = imgWidth - __max(__min((int)(iPosX * fResizeRateX), imgWidth - 2), 2);			//외곽 빼기
//		defectPos.iY = __max(__min((int)(iPosY * fResizeRateY), imgHeight - 2), 2);
//
//		defectPos.bFull = FALSE;
//		defectPos.iType = defectInfo.iType;
//		defectPos.strDefectCode = defectInfo.strDefectCode;
//		defectPos.iIndex = defectInfo.iIndex;
//
//		m_vecDefectPos.push_back(defectPos);
//
//		AddDefectPos(pImage, defectPos);
//	});


	Invalidate(FALSE);
	UpdateWindow();
}

void CImageViewNG::AddDefectPos(IplImage* pImage, DefectPos& defectPos)
{
	int index = 0;

	CRect rect;
	GetClientRect(&rect);
	int nWidth = rect.Width();
	int nHeight = rect.Height();

	CvFont font;
	cvInitFont(&font, /*CV_FONT_VECTOR0*/ 0, 0.45, 0.45, 0, 1);

	int imessagPos_X, imessagPos_Y;

	if (defectPos.iX > nWidth - 100)
		imessagPos_X = defectPos.iX - 85;
	else
		imessagPos_X = defectPos.iX + 15;

	if (defectPos.iY > nHeight - 30)
		imessagPos_Y = defectPos.iY - 30;
	else if (defectPos.iY < 15)
		imessagPos_Y = defectPos.iY + 30;
	else
		imessagPos_Y = defectPos.iY + 15;

//	for (int defectType = 0; defectType < _DEFECT_TYPE_MAX_NUM; defectType++)
//	{
//		if (defectPos.iType == defectType)
//		{
//			CvScalar color = CVRED;
//			//switch (g_defectColorBuffer[defectType])
//			//{
//			//case _TWHITE:	color = CVWHITE;	break;
//			//case _TRED:		color = CVRED;		break;
//			//case _TBLUE:	color = CVBLUE;		break;
//			//case _TGREEN:	color = CVGREEN;	break;
//			//case _TYELLOW:	color = CVYELLOW;	break;
//			//case _TCYAN:	color = CVCYAN;		break;
//			//case _TVIOLET:	color = CVVIOLET;	break;
//			//default:
//			//	break;
//			//}
//			CvFont Textfont;
//			cvInitFont(&Textfont, CV_FONT_HERSHEY_SIMPLEX, 2, 2, 0, 8); //170927 jwmo
//			
//			if (defectPos.strDefectCode == "M601110")
//			{
//				cvPutText(pImage,"AD", cvPoint(pImage->width / 2, pImage->height / 2),
//					&Textfont, color);
//			}
//			else if (defectPos.strDefectCode == "M601100")
//			{
//				cvPutText(pImage, "ND", cvPoint(pImage->width / 2, pImage->height / 2),
//					&Textfont, color);
//			}
//			else
//			{ 
//				cvCircle(pImage, cvPoint(defectPos.iX, defectPos.iY), 10, color, 4);
//
//				//>>171101 하철영, 김태윤 화면에 디팩코드 추가
//				CvFont TextDefectNamefont;
//				cvInitFont(&TextDefectNamefont, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4, 2, 1);
//
//				cvFlip(pImage, pImage,0);	//목지수 화면 돌려줌
//
//				EZIni DefectIni;
//#ifdef _SYSTEM_FOF_
//				DefectIni.SetFileName(_DEFECT_CODE_FOF_FILE_);
//#elif _SYSTEM_FINAL_
//				DefectIni.SetFileName(_DEFECT_CODE_FINAL_FILE_);
//#endif
//				CString strDefectKey, strDefectCode, strDefectName;
//
//				for (int i = 0; i < MAX_DEFECT_COUNT; i++){
//					strDefectKey.Format(_T("DEFECT_CODE_%ld"), i);
//					strDefectCode = DefectIni[_T("DEFECT")][strDefectKey];
//
//					if (strDefectCode == defectPos.strDefectCode){
//						strDefectKey.Format(_T("DEFECT_NAME_E_%ld"), i);
//						strDefectName = DefectIni[_T("DEFECT")][strDefectKey];
//					}
//				}
//
//				if (defectPos.iX > pImage->width/2)
//				{ 
//					if (defectPos.iY < 10)
//						cvPutText(pImage, StringToChar(strDefectName), cvPoint(defectPos.iX - 80, pImage->height - defectPos.iY - 15),
//						&TextDefectNamefont, color);
//					else if (defectPos.iY > pImage->height - 10)
//						cvPutText(pImage, StringToChar(strDefectName), cvPoint(defectPos.iX - 80, pImage->height - defectPos.iY + 15),
//						&TextDefectNamefont, color);
//					else
//					cvPutText(pImage, StringToChar(strDefectName), cvPoint(defectPos.iX - 80, pImage->height - defectPos.iY),
//						&TextDefectNamefont, color);
//				}
//				else
//				{
//					if (defectPos.iY < 10)
//						cvPutText(pImage, StringToChar(strDefectName), cvPoint(defectPos.iX + 20, pImage->height - defectPos.iY - 15),
//						&TextDefectNamefont, color);
//					else if (defectPos.iY > pImage->height - 10)
//						cvPutText(pImage, StringToChar(strDefectName), cvPoint(defectPos.iX + 20, pImage->height - defectPos.iY + 15),
//						&TextDefectNamefont, color);
//					else
//						cvPutText(pImage, StringToChar(strDefectName), cvPoint(defectPos.iX + 20, pImage->height - defectPos.iY),
//						&TextDefectNamefont, color);
//				}
//				
//				cvFlip(pImage, pImage, 0);	//목지수 화면 돌려줌
//				//<<
//			}
//			//ShowMessage(imessagPos_X, imessagPos_Y, color, font, g_defectNameTable[defectType]);
//		}
//	}
}


void CImageViewNG::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


//UINT CImageViewNG::OnNotifyFormat(CWnd *pWnd, UINT nCommand)
//{
//	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
//
//	return CStatic::OnNotifyFormat(pWnd, nCommand);
//}


void CImageViewNG::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_vvImage.GetImage())
	{
		CRect rect;

		if (!m_bFixedSize) {
			GetClientRect(&rect);
		}
		else {
			rect = CRect(0, 0, m_vvImage.Width(), m_vvImage.Height());
		}

		m_vvImage.DrawToHDC(&dc, rect);
	}
}


void CImageViewNG::OnStnClicked()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}