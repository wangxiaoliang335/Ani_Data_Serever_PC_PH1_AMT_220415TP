// MainLogDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "Ani_Data_Serever_PC.h"
#include "DlgMainLog.h"
#include "afxdialogex.h"

// 使用 OutputDebugString 输出日志
static void LightingDbgPrint(LPCTSTR pszFmt, ...)
{
	TCHAR szBuf[512];
	va_list args;
	va_start(args, pszFmt);
	_vsntprintf_s(szBuf, _TRUNCATE, pszFmt, args);
	va_end(args);
	OutputDebugString(szBuf);
}


// CDlgMainLog 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMainLog, CDialogEx)

CDlgMainLog *g_MainLog;
CDlgMainLog::CDlgMainLog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMainLog::IDD, pParent)
{
	g_MainLog = this;
}

CDlgMainLog::~CDlgMainLog()
{
	theApp.m_bExitFlag = FALSE;
}

void CDlgMainLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLC_LOG, m_PlcListBox);
	DDX_Control(pDX, IDC_TOUCH_LOG, m_TouchListBox);
	DDX_Control(pDX, IDC_ALIGN_LOG, m_AlignListBox);
	DDX_Control(pDX, IDC_PG_LOG, m_PgListBox);
	DDX_Control(pDX, IDC_ALIGN_MODEL_CHANGE, m_btnAlignModelChange);
}


BEGIN_MESSAGE_MAP(CDlgMainLog, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CDlgMainLog::OnInitDialog()
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

	if (!theApp.m_TpThread)
	{
		theApp.m_TpThread = new CTpThread;
		theApp.m_TpThread->CreateTask();
	}

	if (!theApp.m_FFUSerialCom)
	{
		theApp.m_FFUSerialCom = new CSerialCom;
		theApp.m_FFUSerialCom->CreateTask();
	}

	theApp.ThreadCreateDelete(FALSE, 0);


	for (int ii = 0; ii < MaxZone; ii++)
	{
		if (!theApp.m_PgInexThread[ii])
		{
			theApp.m_PgInexThread[ii] = new CPgIndex(ii);
			theApp.m_PgInexThread[ii]->CreateTask();
		}
	}

	if (!theApp.m_AllPassModeThread)
	{
		theApp.m_AllPassModeThread = new CAllPassModeThread;
		theApp.m_AllPassModeThread->CreateTask();
	}

	if (!theApp.m_ManualThread)
	{
		theApp.m_ManualThread = new CManualThread;
		theApp.m_ManualThread->CreateTask();
	}

	CString msg;
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
		case _THREAD_OPV:
			theApp.m_OpvThreadOpenFlag[CH_1] = theApp.m_OpvSocketManager[CH_1].SocketServerOpen(OPV1_PORT_NUM);
			theApp.m_OpvThreadOpenFlag[CH_2] = theApp.m_OpvSocketManager[CH_2].SocketServerOpen(OPV2_PORT_NUM);

			break;
		case _THREAD_TP:
			theApp.m_TpThreadOpenFlag = theApp.m_TpSocketManager.SocketServerOpen(TP_PC_PORT_NUM);
			break;
		case _THREAD_LIGHTING:
		LightingDbgPrint(_T("[MainLog] _THREAD_LIGHTING: IP=%s, Port=%s\n"),
				theApp.m_strLightingIP, theApp.m_strLightingPort);
		if (!theApp.m_strLightingIP.IsEmpty() && !theApp.m_strLightingPort.IsEmpty())
		{
			// 确保 VisionThread 已创建（它继承 ILightingEventHandler）
			if (!theApp.m_VisionThread)
			{
				theApp.m_VisionThread = new CVisionThread();
				theApp.m_VisionThread->CreateTask();
			}
			// 设置回调到 VisionThread（包含 OnLightingResult 等处理）
			theApp.m_LightingSocketManager.SetEventHandler(theApp.m_VisionThread);
			theApp.m_LightingThreadOpenFlag = theApp.m_LightingSocketManager.Connect(
				theApp.m_strLightingIP, theApp.m_strLightingPort);
				if (theApp.m_LightingThreadOpenFlag)
				{
					theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
						_T("Lighting connected to %s:%s"), theApp.m_strLightingIP, theApp.m_strLightingPort));
				}
				else
				{
					theApp.m_pLightingLog->LOG_INFO(CStringSupport::FormatString(
						_T("Lighting connection failed to %s:%s"), theApp.m_strLightingIP, theApp.m_strLightingPort));
				}
			}
			break;
		}
	}

	theApp.m_FFUSerialCom->OnPortOpen(_ttoi(theApp.m_strFFUPortNum)); // 1

	SetTimer(_TIMER_DISPLAY_TIME, 1000, NULL);

	return TRUE;
}


// CDlgMainLog 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CDlgMainLog, CDialogEx)
	ON_EVENT(CDlgMainLog, IDB_ALIGN_LOG_CLEAN, DISPID_CLICK, CDlgMainLog::OnClickIdbAlignLogClean, VTS_NONE)
	ON_EVENT(CDlgMainLog, IDB_TOUCH_PC_LOG_CLEAN, DISPID_CLICK, CDlgMainLog::OnClickIdbTouchLogClean, VTS_NONE)
	ON_EVENT(CDlgMainLog, IDB_PG_CLEAN, DISPID_CLICK, CDlgMainLog::OnClickIdbPgClean, VTS_NONE)
	ON_EVENT(CDlgMainLog, IDB_PLC_LOG_CLEAN, DISPID_CLICK, CDlgMainLog::OnClickIdbPlcLogClean, VTS_NONE)
END_EVENTSINK_MAP()


void CDlgMainLog::OnClickIdbAlignLogClean()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_AlignListBox.ResetContent();
	theApp.m_AlignLog->LOG_INFO(_T("Align List Box Clean"));
}


void CDlgMainLog::OnClickIdbTouchLogClean()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_TouchListBox.ResetContent();
	theApp.m_pTpLog->LOG_INFO(_T("Touch List Box Clean"));
}


void CDlgMainLog::OnClickIdbPgClean()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_PgListBox.ResetContent();
	theApp.m_PgLog->LOG_INFO(_T("PG List Box Clean"));
}


void CDlgMainLog::OnClickIdbPlcLogClean()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	m_PlcListBox.ResetContent();
	theApp.m_PlcLog->LOG_INFO(_T("PLC List Box Clean"));
}

void CDlgMainLog::StringChanged()
{
	StringChnageMsg(IDB_ALIGN_LOG_CLEAN, _T("Align 로그 정리"), _T("Align Log Clean"), _T("Align Log清理"));
	StringChnageMsg(IDB_TOUCH_PC_LOG_CLEAN, _T("TP / OPV 로그 정리"), _T("TP / OPV Log Clean"), _T("TP / OPV Log清理"));
	StringChnageMsg(IDB_PG_CLEAN, _T("PG 로그 정리"), _T("PG Log Clean"), _T("PG Log清理"));
	StringChnageMsg(IDB_PLC_LOG_CLEAN, _T("PLC 로그 정리"), _T("PLC Log Clean"), _T("PLC Log清理"));
}

void CDlgMainLog::StringChnageMsg(int btn, CString strKor, CString strEng, CString strChi)
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

void CDlgMainLog::OnTimer(UINT_PTR nIDEvent)
{
	if (this->IsWindowVisible() == FALSE)
		return;

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDEvent == _TIMER_DISPLAY_TIME)
	{
		theApp.ModelCheck(theApp.m_ChangeModelAlign, &m_btnAlignModelChange);
	}

	CDialogEx::OnTimer(nIDEvent);
}
#endif

