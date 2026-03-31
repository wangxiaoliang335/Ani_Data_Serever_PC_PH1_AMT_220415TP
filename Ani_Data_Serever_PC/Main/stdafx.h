// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ���������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.

#include "TimeCheck.h"
#include "Migration.h"
#include "AniUtil.h"

#include <afxsock.h>            // MFC socket extensions
#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.

#include "cv.h"
#include "cvaux.h"
#include "highgui.h"


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define _USE_MELSEC_

#ifdef _USE_MELSEC_
#pragma comment(lib, "MdFunc32.lib")
#endif

class CMyvvImage// : public CvvImage
{
	IplImage* m_img;
public:
	CMyvvImage() { /*CvvImage::CvvImage();*/ };
	~CMyvvImage() { /*CvvImage::~CvvImage();*/ };

	bool Create2(int w, int h, int bpp, int channels)
	{
		const unsigned max_img_size = 10000;

		m_img = cvCreateImage(cvSize(w, h), bpp, channels);
		if (m_img)
			m_img->origin = IPL_ORIGIN_TL;

		return m_img != 0;
	}

	IplImage *GetImage() {
		if (m_img) {
			if (m_img->nSize > 0 && m_img->ID == 0 && m_img->alphaChannel == 0)
				return m_img;
		}
		return NULL;
	}

	void SetImage(IplImage *pImage)
	{
		if (m_img)
		{
			cvReleaseImage(&m_img);
			m_img = NULL;
		}
		m_img = pImage;
	}

	void Destroy()
	{

	}

	void DrawToHDC(CDC* pdc, CRect cRect)
	{
		if (m_img->imageData != NULL) {
			BITMAPINFO bitmapInfo;

			memset(&bitmapInfo, 0, sizeof(bitmapInfo));

			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biPlanes = 1;
			bitmapInfo.bmiHeader.biCompression = BI_RGB;
			bitmapInfo.bmiHeader.biWidth = m_img->width;
			bitmapInfo.bmiHeader.biHeight = m_img->height;
			bitmapInfo.bmiHeader.biBitCount = 24;

			IplImage* color_img = cvCreateImage(cvSize(m_img->width, m_img->height), m_img->depth, 3);
			if (m_img->nChannels == 1) {
				cvCvtColor(m_img, color_img, CV_GRAY2RGB);
			}
			else {
				cvCopy(m_img, color_img);
			}

			pdc->SetStretchBltMode(COLORONCOLOR);
			::StretchDIBits(pdc->m_hDC, cRect.left, cRect.top, cRect.right, cRect.bottom,
				0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, color_img->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

			cvReleaseImage(&color_img);
		}
	}

	int Width() { return !m_img ? 0 : !m_img->roi ? m_img->width : m_img->roi->width; };
	int Height() { return !m_img ? 0 : !m_img->roi ? m_img->height : m_img->roi->height; };
};

extern float g_fViewResizeRateW;
extern float g_fViewResizeRateH;

#define DEBUG_SIM


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

static BOOL PathEndCheck(CString & strPath)
{
	if (strPath == "")
	{
		strPath.Insert(strPath.GetLength(), '\\');
		return FALSE;
	}

	if (strPath.GetAt(strPath.GetLength() - 1) != '\\')
	{
		strPath.Insert(strPath.GetLength(), '\\');
		return FALSE;
	}

	return TRUE;
}
static BOOL GetPathOnly(CString & strFilePath)
{
	CString strPath;
	int nSize = strFilePath.ReverseFind('\\');
	if (nSize == -1)
		return FALSE;

	strPath = strFilePath.Left(nSize);
	strFilePath = strPath;
	PathEndCheck(strFilePath);
	return TRUE;
}
