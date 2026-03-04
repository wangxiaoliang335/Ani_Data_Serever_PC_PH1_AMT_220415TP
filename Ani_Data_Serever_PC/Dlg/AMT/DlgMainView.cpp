// Dlg_Align_PC_View.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "Ani_Data_Serever_PC.h"
#include "DlgMainView.h"
#include "MainFrm.h"

// CDlgMainView

IMPLEMENT_DYNAMIC(CDlgMainView, CDialog)

CDlgMainView *g_DlgMainView;
CDlgMainView::CDlgMainView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMainView::IDD, pParent)
{
	g_DlgMainView = this;
}


CDlgMainView::~CDlgMainView()
{
	theApp.m_bExitFlag = FALSE;
}

void CDlgMainView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIEWING_LOG, m_ViewingAngleListBox[0]);
	DDX_Control(pDX, IDC_VISION_LOG, m_VisionListBox[0]);
	DDX_Control(pDX, IDC_VIEWING_LOG2, m_ViewingAngleListBox[1]);
	DDX_Control(pDX, IDC_VISION_LOG2, m_VisionListBox[1]);

	DDX_Control(pDX, IDC_VIEWING_ANGLE_MODEL_CHANGE1, m_btnViewingAngleModelChange1);
	DDX_Control(pDX, IDC_VIEWING_ANGLE_MODEL_CHANGE2, m_btnViewingAngleModelChange2);
	DDX_Control(pDX, IDC_VIEWING_ANGLE_MODEL_CHANGE3, m_btnViewingAngleModelChange3);
	DDX_Control(pDX, IDC_VIEWING_ANGLE_MODEL_CHANGE4, m_btnViewingAngleModelChange4);
	DDX_Control(pDX, IDC_VISION_MODEL_CHANGE1, m_btnVisionModelChange1);
	DDX_Control(pDX, IDC_VISION_MODEL_CHANGE2, m_btnVisionModelChange2);
}

BEGIN_MESSAGE_MAP(CDlgMainView, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgMainView 진단입니다.

BOOL CDlgMainView::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if (!theApp.m_VisionThread)
	{
		theApp.m_VisionThread = new CVisionThread();
		theApp.m_VisionThread->CreateTask();
	}

	if (!theApp.m_ViewingAngleThread)
	{
		theApp.m_ViewingAngleThread = new CViewingAngleThread();
		theApp.m_ViewingAngleThread->CreateTask();
	}

	if (!theApp.m_pFTP)
	{
		theApp.m_pFTP = new CDFSClient;
		theApp.m_pFTP->CreateTask();
	}

	if (!theApp.m_pFS)
	{
		theApp.m_pFS = new CFTPClient;
		theApp.m_pFS->CreateTask();
		theApp.m_pFTP->CreateDfsTask();
	}

	if (!theApp.m_pRankTread)
	{
		theApp.m_pRankTread = new CRankThread;
		theApp.m_pRankTread->CreateTask();
	}

	if (theApp.m_iMachineType == SetAFT)
	{
		if (!theApp.m_LumitopThread)
		{
			theApp.m_LumitopThread = new CLumitopThread();
			theApp.m_LumitopThread->CreateTask();
		}
	}
	

	SocketServerOpen();		//Socket 통신할수 있도록 실행하자마자 Server Open
	SetTimer(_TIMER_DISPLAY_TIME, 500, NULL);

	return TRUE; 
}

void CDlgMainView::SocketServerOpen()
{
	for (int ii = 0; ii < _THREAD_TOTAL_COUNT; ii++)
	{
		switch (ii)
		{
		case _THREAD_VISION:
			theApp.m_VisionThreadOpenFlag[PC1] = theApp.m_VisionSocketManager[PC1].SocketServerOpen(VISION_PC1_PORT_NUM);		//1,3 카메라
			theApp.m_VisionThreadOpenFlag[PC2] = theApp.m_VisionSocketManager[PC2].SocketServerOpen(VISION_PC2_PORT_NUM);		//2,4 카메라
			break; 
		case _THREAD_VIEWING_ANGLE:
			theApp.m_ViewingAngleThreadOpenFlag[PanelNum1] = theApp.m_ViewingAngleSocketManager[PanelNum1].SocketServerOpen(VIEWING_ANGLE_PANEL1_PORT_NUM);
			theApp.m_ViewingAngleThreadOpenFlag[PanelNum2] = theApp.m_ViewingAngleSocketManager[PanelNum2].SocketServerOpen(VIEWING_ANGLE_PANEL2_PORT_NUM);
			theApp.m_ViewingAngleThreadOpenFlag[PanelNum3] = theApp.m_ViewingAngleSocketManager[PanelNum3].SocketServerOpen(VIEWING_ANGLE_PANEL3_PORT_NUM);
			theApp.m_ViewingAngleThreadOpenFlag[PanelNum4] = theApp.m_ViewingAngleSocketManager[PanelNum4].SocketServerOpen(VIEWING_ANGLE_PANEL4_PORT_NUM);
			break;
		case _THREAD_LUMITOP:
			theApp.m_LumitopThreadOpenFlag[PC1] = theApp.m_LumitopSocketManager[PC1].SocketServerOpen(LUMITOP_PC1_PORT_NUM);		//1,3 카메라
			theApp.m_LumitopThreadOpenFlag[PC2] = theApp.m_LumitopSocketManager[PC2].SocketServerOpen(LUMITOP_PC2_PORT_NUM);		//2,4 카메라
			break;
		}
	}

	
}

BEGIN_EVENTSINK_MAP(CDlgMainView, CFormView)
//ON_EVENT(CDlgMainView, IDB_SYSTEM, DISPID_CLICK, CDlgMainView::OnClickIdbSystem, VTS_NONE)
	ON_EVENT(CDlgMainView, IDB_VIEWING_PC_LOG_CLEAN, DISPID_CLICK, CDlgMainView::OnClickIdbViewingAngleLogClean, VTS_NONE)
	ON_EVENT(CDlgMainView, IDB_VIEWING_PC_LOG_CLEAN2, DISPID_CLICK, CDlgMainView::OnClickIdbViewingAngleLogClean2, VTS_NONE)
	ON_EVENT(CDlgMainView, IDB_VISION_PC_LOG_CLEAN, DISPID_CLICK, CDlgMainView::OnClickIdbVisionPcLogClean, VTS_NONE)
	ON_EVENT(CDlgMainView, IDB_VISION_PC_LOG_CLEAN3, DISPID_CLICK, CDlgMainView::OnClickIdbVisionPcLogClean3, VTS_NONE)
END_EVENTSINK_MAP()

void CDlgMainView::OnClickIdbSystem()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CDlgMainView::OnOK()
{
}

void CDlgMainView::OnCancel()
{
}

void CDlgMainView::OnClickIdbViewingAngleLogClean()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ViewingAngleListBox[0].ResetContent();
	theApp.m_ViewingAngleLog->LOG_INFO(_T("Viewing Angle 1 List Box Clean"));
}


void CDlgMainView::OnClickIdbViewingAngleLogClean2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_ViewingAngleListBox[1].ResetContent();
	theApp.m_ViewingAngleLog->LOG_INFO(_T("Viewing Angle 2 List Box Clean"));
}


void CDlgMainView::OnClickIdbVisionPcLogClean()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_VisionListBox[1].ResetContent();
	theApp.m_VisionLog->LOG_INFO(_T("Vision 2 List Box Clean"));
}



void CDlgMainView::OnClickIdbVisionPcLogClean3()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_VisionListBox[0].ResetContent();
	theApp.m_VisionLog->LOG_INFO(_T("Vision 1 List Box Clean"));
}

void CDlgMainView::StringChanged()
{
	if (theApp.m_iMachineType == SetAMT)
	{
		StringChnageMsg(IDB_VIEWING_PC_LOG_CLEAN, _T("시야각검사1 로그 정리"), _T("Viewing Angle1 Log Clean"), _T("Viewing1 Log清理"));
		StringChnageMsg(IDB_VIEWING_PC_LOG_CLEAN2, _T("시야각검사2 로그 정리"), _T("Viewing Angle2 Log Clean"), _T("Viewing2 Log清理"));
	}
	else
	{
		StringChnageMsg(IDB_VIEWING_PC_LOG_CLEAN, _T("루미탑검사1 로그 정리"), _T("Lumitop1 Log Clean"), _T("Lumitop1 Log清理"));
		StringChnageMsg(IDB_VIEWING_PC_LOG_CLEAN2, _T("루미탑검사2 로그 정리"), _T("Lumitop2 Log Clean"), _T("Lumitop2 Log清理"));
	}

	StringChnageMsg(IDB_VISION_PC_LOG_CLEAN, _T("화면검사2 로그 정리"), _T("Vision 2 Log Clean"), _T("画面检查2 Log清理"));
	StringChnageMsg(IDB_VISION_PC_LOG_CLEAN3, _T("화면검사1 로그 정리"), _T("Vision 1 Log Clean"), _T("画面检查1 Log清理"));
}

void CDlgMainView::StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi)
{
	CString msg;
	switch (theApp.m_iLanguageSelect)
	{
	case KOR:msg = strKor; break;
	case ENG:msg = strEng; break;
	case CHI:msg = strChi; break;
	}
	((CBtnEnh*)GetDlgItem(btn))->SetCaption(msg);
}
void CDlgMainView::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == _TIMER_DISPLAY_TIME){
		theApp.ModelCheck(theApp.m_ChangeModelVision1, &m_btnVisionModelChange1);
		theApp.ModelCheck(theApp.m_ChangeModelVision2, &m_btnVisionModelChange2);
		theApp.ModelCheck(theApp.m_ChangeModelViewingAngle1, &m_btnViewingAngleModelChange1);
		theApp.ModelCheck(theApp.m_ChangeModelViewingAngle2, &m_btnViewingAngleModelChange2);
		theApp.ModelCheck(theApp.m_ChangeModelViewingAngle3, &m_btnViewingAngleModelChange3);
		theApp.ModelCheck(theApp.m_ChangeModelViewingAngle4, &m_btnViewingAngleModelChange4);
	}

	CDialog::OnTimer(nIDEvent);
}
#endif

