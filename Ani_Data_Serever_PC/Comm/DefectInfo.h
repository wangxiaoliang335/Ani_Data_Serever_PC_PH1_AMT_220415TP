#pragma once

struct DefectInfo
{
	int iIndex;

	int iType;
	BOOL bHaveInfo;

	CvRect rectArea;
	CvPoint ptPos;

	double dDiameter_mm;
	BOOL bRealNG;

	CvRect rectClick;

	CvRect	rectCurActiveArea;

	int iPanel_Size_w;
	int iPanel_Size_h;
	int iDefect_cnt;
	CString strDefectPatten;
	CString strImgPath;
	int iImage_num;
	CString strPanelID;
	CString strDefectCode; //170927 jwmo
};

typedef std::vector<DefectInfo> VecDefectInfo;