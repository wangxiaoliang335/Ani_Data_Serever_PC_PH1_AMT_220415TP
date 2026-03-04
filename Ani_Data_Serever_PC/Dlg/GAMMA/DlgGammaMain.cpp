// Dlg_Align_PC_View.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#if _SYSTEM_GAMMA_

#include "Ani_Data_Serever_PC.h"
#include "DlgGammaMain.h"
#include "MainFrm.h"

// CDlgMainView

IMPLEMENT_DYNAMIC(CDlgGammaMain, CDialog)

CDlgGammaMain *g_MainLog;
CDlgGammaMain::CDlgGammaMain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGammaMain::IDD, pParent)
{
	g_MainLog = this;
}


CDlgGammaMain::~CDlgGammaMain()
{
	theApp.m_bExitFlag = FALSE;
}

void CDlgGammaMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLC_LOG, m_PlcListBox);
	DDX_Control(pDX, IDC_ALIGN_LOG, m_AlignListBox);
	DDX_Control(pDX, IDC_PG_LOG, m_PgListBox);
	DDX_Control(pDX, IDC_ALIGN_MODEL_CHANGE, m_btnAlignModelChange);
}

BEGIN_MESSAGE_MAP(CDlgGammaMain, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgMainView 진단입니다.

BOOL CDlgGammaMain::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if (!theApp.m_pEqIf)
	{
		theApp.m_pEqIf = new CEqInterface;
	}

	if (!theApp.m_PlcThread)
	{
		theApp.m_PlcThread = new CPlcThread;
		theApp.m_PlcThread->CreateTask();
		theApp.m_PlcThread->TactCreateTask();
		theApp.m_PlcThread->HeartBitCreateTask();
	}

	theApp.ThreadCreateDelete(FALSE, 0);

	if (!theApp.m_pFTP)
	{
		theApp.m_pFTP = new CDFSClient;
#if _SYSTEM_AMTAFT_
		theApp.m_pFTP->CreateTask();
#endif
		theApp.m_pFTP->CreateDfsTask();
	}

	if (!theApp.m_pFS)
	{
		theApp.m_pFS = new CFTPClient;
		theApp.m_pFS->CreateTask();
	}


	for (int ii = 0; ii < MaxGammaStage; ii++)
	{
		if (!theApp.m_GammaThread[ii])
		{
			theApp.m_GammaThread[ii] = new CGammaThread(ii);
			theApp.m_GammaThread[ii]->CreateTask();
		}
	}

	if (!theApp.m_pFS)
	{
		theApp.m_pFS = new CFTPClient;
		theApp.m_pFS->CreateTask();
	}

	if (!theApp.m_FFUSerialCom)
	{
		theApp.m_FFUSerialCom = new CSerialCom;
		theApp.m_FFUSerialCom->CreateTask();
	}
	
	theApp.m_FFUSerialCom->OnPortOpen(_ttoi(theApp.m_strFFUPortNum));

	if (!theApp.m_ARSSerialRS485)
	{
		theApp.m_ARSSerialRS485 = new CSerialRS485;
		theApp.m_ARSSerialRS485->CreateTask();
	}
	theApp.m_ARSSerialRS485->OnPortOpen(_ttoi(theApp.m_strARSPortNum));

	SocketServerOpen();		//Socket 통신할수 있도록 실행하자마자 Server Open
	SetTimer(_TIMER_DISPLAY_TIME, 500, NULL);

	return TRUE;
}

void CDlgGammaMain::SocketServerOpen()
{
	for (int ii = 0; ii < _THREAD_TOTAL_COUNT; ii++)
	{
		switch (ii)
		{
		case _THREAD_ALIGN:
			for (int ii = 0; ii < theApp.m_AlignSocketManager.size(); ii++)
				theApp.m_AlignThreadOpenFlag[ii] = theApp.m_AlignSocketManager[ii]->SocketServerOpen(ALIGN_PORT_NUM[ii]);

			break;
		case _THREAD_PG:
			for (int ii = 0; ii < PgServerMaxCount; ii++)
				theApp.m_PgThreadOpenFlag[ii] = theApp.m_PgSocketManager[ii].SocketServerOpen(PG_PORT_NUM[ii], ii);

			break;
		}
	}
}

BEGIN_EVENTSINK_MAP(CDlgGammaMain, CFormView)
	ON_EVENT(CDlgGammaMain, IDB_PG_CLEAN, DISPID_CLICK, CDlgGammaMain::OnClickIdbPgClean, VTS_NONE)
	ON_EVENT(CDlgGammaMain, IDB_PLC_LOG_CLEAN, DISPID_CLICK, CDlgGammaMain::OnClickIdbPlcLogClean, VTS_NONE)
	ON_EVENT(CDlgGammaMain, IDB_ALIGN_LOG_CLEAN, DISPID_CLICK, CDlgGammaMain::OnClickIdbAlignLogClean, VTS_NONE)

END_EVENTSINK_MAP()

void CDlgGammaMain::OnClickIdbPlcLogClean()
{
	m_PlcListBox.ResetContent();
	theApp.m_PlcLog->LOG_INFO(_T("Plc List Box Clean"));
}

void CDlgGammaMain::OnClickIdbPgClean()
{
	m_PgListBox.ResetContent();
	theApp.m_PgLog->LOG_INFO(_T("Pg List Box Clean"));
}

void CDlgGammaMain::OnClickIdbAlignLogClean()
{
	m_AlignListBox.ResetContent();
	theApp.m_AlignLog->LOG_INFO(_T("Align List Box Clean"));
}

void CDlgGammaMain::StringChanged()
{
	StringChnageMsg(IDB_PG_CLEAN, _T("PG 로그 정리"), _T("PG Log Clean"), _T("PG Log清理"));
	StringChnageMsg(IDB_PLC_LOG_CLEAN, _T("PLC 로그 정리"), _T("PLC Log Clean"), _T("PLC Log清理"));
	StringChnageMsg(IDB_ALIGN_LOG_CLEAN, _T("Align 로그 정리"), _T("Align Log Clean"), _T("Align Log清理"));
}

void CDlgGammaMain::StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi)
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
void CDlgGammaMain::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	if (nIDEvent == _TIMER_DISPLAY_TIME)
		theApp.ModelCheck(theApp.m_ChangeModelAlign, &m_btnAlignModelChange);
	
	CDialog::OnTimer(nIDEvent);
}


#endif



