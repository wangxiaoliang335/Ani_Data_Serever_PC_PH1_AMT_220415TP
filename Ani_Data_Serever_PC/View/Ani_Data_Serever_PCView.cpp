
// Operrator_view_YoungView.cpp : CAni_Data_Serever_PCView Å¬·¡½ºÀÇ ±¸Çö
//

#include "stdafx.h"
// SHARED_HANDLERS´Â ¹Ì¸® º¸±â, Ãà¼ÒÆÇ ±×¸² ¹× °Ë»ö ÇÊÅÍ Ã³¸®±â¸¦ ±¸ÇöÇÏ´Â ATL ÇÁ·ÎÁ§Æ®¿¡¼­ Á¤ÀÇÇÒ ¼ö ÀÖÀ¸¸ç
// ÇØ´ç ÇÁ·ÎÁ§Æ®¿Í ¹®¼­ ÄÚµå¸¦ °øÀ¯ÇÏµµ·Ï ÇØ ÁÝ´Ï´Ù.
#ifndef SHARED_HANDLERS
#include "Ani_Data_Serever_PC.h"
#endif

#include "Ani_Data_Serever_PCDoc.h"
#include "Ani_Data_Serever_PCView.h"
#include "MainFrm.h"
#include "BtnEnhReadability.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAni_Data_Serever_PCView
IMPLEMENT_DYNCREATE(CAni_Data_Serever_PCView, CFormView)

// CAni_Data_Serever_PCView »ý¼º/¼Ò¸ê

CAni_Data_Serever_PCView::CAni_Data_Serever_PCView()
: CFormView(CAni_Data_Serever_PCView::IDD)
{
	// TODO: ¿©±â¿¡ »ý¼º ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	m_pAlarmHistory = NULL;
	m_pTactTimeHistory = NULL;
	m_SetSystem = NULL;
	m_pMain = NULL;
	m_pInspect = NULL;
	m_pTimeInspet = NULL;
	m_pIdCardHistory = NULL;
	m_SetRank = NULL;
	m_SetVision = NULL;
#if _SYSTEM_AMTAFT_
	m_pMainLog = NULL;
#endif

}

CAni_Data_Serever_PCView::~CAni_Data_Serever_PCView()
{
#if _SYSTEM_AMTAFT_
	if (m_pMainLog != NULL)
		delete m_pMainLog;
#endif
	if (m_pMain != NULL)
		delete m_pMain;
	if (m_SetRank != NULL)
		delete m_SetRank;
	if (m_SetVision != NULL)
		delete m_SetVision;
	if (m_pAlarmHistory != NULL)
		delete m_pAlarmHistory;
	if (m_pInspect != NULL)
		delete m_pInspect;
	if (m_pTimeInspet != NULL)
		delete m_pTimeInspet;
	if (m_SetSystem != NULL)
		delete m_SetSystem;
	if (m_SetTimerDlg != NULL)
		delete m_SetTimerDlg;
	if (m_pTactTimeHistory != NULL)
		delete m_pTactTimeHistory;
	if (m_pIdCardHistory != NULL)
		delete m_pIdCardHistory;
}

void CAni_Data_Serever_PCView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDB_BTN_KOR, m_btnKor);
	DDX_Control(pDX, IDB_BTN_ENG, m_btnEng);
	DDX_Control(pDX, IDB_BTN_CHI, m_btnChi);
	DDX_Control(pDX, IDB_BTN_DEFECT_COUNT, m_btnLabelName);
}

BOOL CAni_Data_Serever_PCView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

void CAni_Data_Serever_PCView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	pMainFrame = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

	CRect rc;		rc.SetRectEmpty();
	GetDlgItem(IDC_MAIN_FRM_DIS1)->GetWindowRect(&rc);
 
	CREATE_RECT_DLG(m_pAlarmHistory, CDlgAlarmHistory, DLG_ALARM_HISTORY, this, rc, SW_HIDE);
	CREATE_RECT_DLG(m_pTactTimeHistory, CDlgTactTimeHistory, DLG_TACT_TIME_HISTORY, this, rc, SW_HIDE);
	CREATE_RECT_DLG(m_pIdCardHistory, CDlgIdCardHistory, DLG_ID_CARD_READER_HISTORY, this, rc, SW_HIDE);
#if _SYSTEM_AMTAFT_
	CREATE_RECT_DLG(m_pMainLog, CDlgMainLog, DLG_MAIN_LOG_BOX, this, rc, SW_HIDE);
	CREATE_RECT_DLG(m_pMain, CDlgMainView, DLG_MAIN_BOX, this, rc, SW_HIDE);
	CREATE_RECT_DLG(m_pInspect, CDlgInspect, DLG_INSPECT_AMT_AFT, this, rc, SW_SHOW);
	CREATE_RECT_DLG(m_pTimeInspet, CDlgTimeInspect, DLG_TIME_INSPECT_AMT_AFT, this, rc, SW_HIDE);

	m_btnLabelName.SetCaption(_T("LOG2"));
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDB_BTN_INSPECT);
	pBtnEnh->SetValue(TRUE);
#else
	CREATE_RECT_DLG(m_pMain, CDlgGammaMain, DLG_GAMMA_MAIN_LOG_BOX1, this, rc, SW_HIDE);
	CREATE_RECT_DLG(m_pInspect, CDlgGammaInspect, DLG_INSPECT_GAMMA, this, rc, SW_SHOW);
	CREATE_RECT_DLG(m_pTimeInspet, CDlgGammaTimeInspect, DLG_TIME_INSPECT_GAMMA, this, rc, SW_HIDE);

	CBtnEnh *pBtnEnh = (CBtnEnh*)GetDlgItem(IDB_BTN_INSPECT);
	pBtnEnh->SetValue(TRUE);

	CBtnEnh *pBtnRank = (CBtnEnh*)GetDlgItem(IDB_BTN_SETNGRANK);
	pBtnRank->ShowWindow(SW_HIDE);

	CBtnEnh *pBtnSetVision = (CBtnEnh*)GetDlgItem(IDB_BTN_SETVISION);
	pBtnSetVision->ShowWindow(SW_HIDE);

	CBtnEnh *pBtnDefectCount = (CBtnEnh*)GetDlgItem(IDB_BTN_DEFECT_COUNT);
	pBtnDefectCount->ShowWindow(SW_HIDE);
#endif
	m_SetRank = new CDlgSetRank;
	m_SetRank->Create(IDD_DLG_SETRANK, this);
	m_SetRank->ShowWindow(SW_HIDE);

	m_SetSystem = new CDLGSetSystem;
	m_SetSystem->Create(IDD_DLG_SETSYSTEM, this);
	m_SetSystem->ShowWindow(SW_HIDE);

	m_SetVision = new CDLGSetVision;
	m_SetVision->Create(IDD_DLG_SETVISION, this);
	m_SetVision->ShowWindow(SW_HIDE);

	m_SetTimerDlg = new CSetTimerDlg;
	m_SetTimerDlg->Create(IDD_DLG_SET_TIMER, this);
	m_SetTimerDlg->ShowWindow(SW_HIDE);

	theApp.m_pMsgBox = new CMsgBox(0, _T(""));
	theApp.m_pMsgBox->Create(DLG_MSG_BOX, this);
	theApp.m_pMsgBox->ShowWindow(SW_HIDE);

	theApp.m_pMsgBoxAlarm = new CMsgBox(MS_OK, _T(""));
	theApp.m_pMsgBoxAlarm->Create(DLG_MSG_BOX, this);
	theApp.m_pMsgBoxAlarm->ShowWindow(SW_HIDE);

	switch (theApp.m_iLanguageSelect)
	{
	case KOR:m_btnKor.SetValue(TRUE); break;
	case ENG:m_btnEng.SetValue(TRUE); break;
	case CHI:m_btnChi.SetValue(TRUE); break;
	}

	ReapplyMainViewTopStripReadability();
}

void CAni_Data_Serever_PCView::ReapplyMainViewTopStripReadability()
{
	static const UINT s_mainViewModeRowIds[] = {
		IDB_BTN_INSPECT, IDB_BTN_TIME_INSPECT, IDB_BTN_ALARM, IDB_BTN_TACT_TIME,
		IDB_BTN_CARD_READER, IDB_BTN_START, IDB_BTN_DEFECT_COUNT,
	};
	for (UINT id : s_mainViewModeRowIds)
		ApplyBtnEnhReadabilityById(this, id);

	ApplyBtnEnhReadabilityStyle(m_btnKor);
	ApplyBtnEnhReadabilityStyle(m_btnEng);
	ApplyBtnEnhReadabilityStyle(m_btnChi);
	static const UINT s_mainViewSmallBtnIds[] = {
		IDB_BTN_SETTIMER, IDB_BTN_SETNGRANK, IDB_BTN_SETSYSTEM,
		IDB_BTN_SETNGRANK2, IDB_BTN_SETNGRANK3, IDB_BTN_SETVISION,
	};
	for (UINT id : s_mainViewSmallBtnIds)
		ApplyBtnEnhReadabilityById(this, id);
}


#ifdef _DEBUG
void CAni_Data_Serever_PCView::AssertValid() const
{
	CFormView::AssertValid();
}

void CAni_Data_Serever_PCView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

#endif //_DEBUG

BEGIN_MESSAGE_MAP(CAni_Data_Serever_PCView, CFormView)
	//ON_BN_CLICKED(IDC_BUTTON1, &CAni_Data_Serever_PCView::OnBnClickedButton1)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON5, &CAni_Data_Serever_PCView::OnBnClickedButton5)
END_MESSAGE_MAP()


void CAni_Data_Serever_PCView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{	
	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CAni_Data_Serever_PCView::OnBnClickedButton1()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	//Invalidate(TRUE);
	//UpdateData(false);
}


void CAni_Data_Serever_PCView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	CRect rc;		rc.SetRectEmpty();
	GetDlgItem(IDC_MAIN_FRM_DIS1)->GetWindowRect(&rc);

}

BEGIN_EVENTSINK_MAP(CAni_Data_Serever_PCView, CFormView)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_START, DISPID_CLICK, CAni_Data_Serever_PCView::OnClickIdbBtnStart, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_DEFECT_COUNT, DISPID_CLICK, CAni_Data_Serever_PCView::OnClickIdbBtnDefectCount, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_INSPECT, DISPID_CLICK, CAni_Data_Serever_PCView::OnClickIdbBtnInspect, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_ALARM, DISPID_CLICK, CAni_Data_Serever_PCView::OnClickIdbBtnAlarm, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_KOR, DISPID_CLICK, CAni_Data_Serever_PCView::OnClickIdbBtnLanguage, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_ENG, DISPID_CLICK, CAni_Data_Serever_PCView::OnClickIdbBtnLanguage, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_CHI, DISPID_CLICK, CAni_Data_Serever_PCView::OnClickIdbBtnLanguage, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_SETTIMER, DISPID_CLICK, CAni_Data_Serever_PCView::OnClickIdbBtnSetTimer, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_SETNGRANK, DISPID_CLICK, CAni_Data_Serever_PCView::OnClickIdbBtnSetRank, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_TACT_TIME, DISPID_CLICK, CAni_Data_Serever_PCView::ClickBtnTactTime, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_TIME_INSPECT, DISPID_CLICK, CAni_Data_Serever_PCView::ClickBtnTimeInspect, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_SETSYSTEM, DISPID_CLICK, CAni_Data_Serever_PCView::ClickBtnSetsystem, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_CARD_READER, DISPID_CLICK, CAni_Data_Serever_PCView::ClickBtnCardReader, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_SETVISION, DISPID_CLICK, CAni_Data_Serever_PCView::ClickBtnSetVision, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_SETNGRANK2, DISPID_CLICK, CAni_Data_Serever_PCView::ClickBtnSetngrank2, VTS_NONE)
	ON_EVENT(CAni_Data_Serever_PCView, IDB_BTN_SETNGRANK3, DISPID_CLICK, CAni_Data_Serever_PCView::ClickBtnSetngrank3, VTS_NONE)
END_EVENTSINK_MAP()

void CAni_Data_Serever_PCView::OnClickIdbBtnStart()
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
#if _SYSTEM_AMTAFT_
	m_pMain->ShowWindow(SW_SHOW);
	m_pMainLog->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#else
	m_pMain->ShowWindow(SW_SHOW);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#endif
	ReapplyMainViewTopStripReadability();
}


void CAni_Data_Serever_PCView::OnClickIdbBtnDefectCount()
{
	// TODO: ¿©±â¿¡ ¸Þ½ÃÁö Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
#if _SYSTEM_AMTAFT_
	m_pMain->ShowWindow(SW_HIDE);
	m_pMainLog->ShowWindow(SW_SHOW);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#else
	m_pMain->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#endif
	ReapplyMainViewTopStripReadability();
}

void CAni_Data_Serever_PCView::OnClickIdbBtnInspect()
{
#if _SYSTEM_AMTAFT_
	m_pMain->ShowWindow(SW_HIDE);
	m_pMainLog->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_SHOW);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#else
	m_pMain->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_SHOW);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#endif
	ReapplyMainViewTopStripReadability();
}

void CAni_Data_Serever_PCView::OnClickIdbBtnAlarm()
{
#if _SYSTEM_AMTAFT_
	m_pMain->ShowWindow(SW_HIDE);
	m_pMainLog->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_SHOW);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#else
	m_pMain->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_SHOW);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#endif
	ReapplyMainViewTopStripReadability();
}

void CAni_Data_Serever_PCView::ClickBtnTactTime()
{
#if _SYSTEM_AMTAFT_
	m_pMain->ShowWindow(SW_HIDE);
	m_pMainLog->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_SHOW);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#else
	m_pMain->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_SHOW);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#endif
	ReapplyMainViewTopStripReadability();
}

void CAni_Data_Serever_PCView::ClickBtnTimeInspect()
{
#if _SYSTEM_AMTAFT_
	m_pMain->ShowWindow(SW_HIDE);
	m_pMainLog->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_SHOW);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#else
	m_pMain->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_SHOW);
	m_pIdCardHistory->ShowWindow(SW_HIDE);
#endif
	ReapplyMainViewTopStripReadability();
}


void CAni_Data_Serever_PCView::ClickBtnCardReader()
{
#if _SYSTEM_AMTAFT_
	m_pMain->ShowWindow(SW_HIDE);
	m_pMainLog->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_SHOW);
#else
	m_pMain->ShowWindow(SW_HIDE);
	m_pInspect->ShowWindow(SW_HIDE);
	m_pAlarmHistory->ShowWindow(SW_HIDE);
	m_pTactTimeHistory->ShowWindow(SW_HIDE);
	m_pTimeInspet->ShowWindow(SW_HIDE);
	m_pIdCardHistory->ShowWindow(SW_SHOW);
#endif
	ReapplyMainViewTopStripReadability();
}


void CAni_Data_Serever_PCView::OnClickIdbBtnLanguage()
{
	//theApp.m_OpvSocketManager[0].OpvInspectionResult(0, _T("321,0,123"));
	CBtnEnh *pBtnEnh = (CBtnEnh*)GetFocus();

	switch (pBtnEnh->GetDlgCtrlID())
	{
	case IDB_BTN_KOR: theApp.m_iLanguageSelect = KOR;  break;
	case IDB_BTN_ENG: theApp.m_iLanguageSelect = ENG; break;
	case IDB_BTN_CHI: theApp.m_iLanguageSelect = CHI; break;
	}

	EZIni ini(DATA_SYSTEM_DATA_PATH);
	ini[_T("DATA")][_T("LANGUAGAE")] = theApp.m_iLanguageSelect;
	theApp.LanguageChange();
}

void CAni_Data_Serever_PCView::OnClickIdbBtnSetTimer()
{
	if (theApp.m_iUserClass != USER_MAKER)
	{
		theApp.getMsgBox(MS_OK, _T("관리자만 사용가능 합니다."), _T("Maker is USE"), _T("Maker is USE"));
		return;
	}

	m_SetTimerDlg->ShowWindow(SW_SHOW);
}

void CAni_Data_Serever_PCView::OnClickIdbBtnSetRank()
{
	if (theApp.m_iUserClass != USER_MAKER)
	{
		theApp.getMsgBox(MS_OK, _T("관리자만 사용가능 합니다."), _T("Maker is USE"), _T("Maker is USE"));
		return;
	}

	m_SetRank->StringChanged();
	m_SetRank->ShowWindow(SW_SHOW);
}

void CAni_Data_Serever_PCView::ClickBtnSetsystem()
{
	if (theApp.m_iUserClass != USER_MAKER)
	{
		theApp.getMsgBox(MS_OK, _T("관리자만 사용가능 합니다."), _T("Maker is USE"), _T("Maker is USE"));
		return;
	}

	m_SetSystem->ShowWindow(SW_SHOW);
}

void CAni_Data_Serever_PCView::ClickBtnSetVision()
{
	if (theApp.m_iUserClass != USER_MAKER)
	{
		theApp.getMsgBox(MS_OK, _T("관리자만 사용가능 합니다."), _T("Maker is USE"), _T("Maker is USE"));
		return;
	}

	m_SetVision->ShowWindow(SW_SHOW);
}

void CAni_Data_Serever_PCView::ClickBtnSetngrank2()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	DfsDataValue pTempDFSData;

	pTempDFSData.m_TypeNum = Machine_AOI;
	pTempDFSData.m_FpcID = _T("A7F59BW03CB48");
	pTempDFSData.m_PanelID = _T("A7F59BW03CB48");
	pTempDFSData.m_StartTime = _T("20191230181828");
	pTempDFSData.m_EndTime = _T("20191230181943");
	pTempDFSData.m_LoadHandlerTime = _T("20191230181901");
	pTempDFSData.m_UnloadHandlerTime = _T("20191230181940");
	pTempDFSData.m_TpTime = _T("6192");
	pTempDFSData.m_PreGammaTime = _T("10348");
	pTempDFSData.m_TactTime = _T("115");
	pTempDFSData.m_PreGammaContactStatus = _T("1");
	pTempDFSData.m_ModelID = _T("");
	pTempDFSData.m_IndexNum = _T("3");
	pTempDFSData.m_ChNum = _T("1");
	pTempDFSData.m_TpResult = _T("1");
#if _SYSTEM_AMTAFT_
	theApp.m_pFTP->AddTransferFile(pTempDFSData);
#endif
}


void CAni_Data_Serever_PCView::ClickBtnSetngrank3()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	DfsDataValue pTempDFSData;

	pTempDFSData.m_TypeNum = Machine_ULD;
	pTempDFSData.m_FpcID = _T("A7F59BW03CB48");
	pTempDFSData.m_PanelID = _T("A7F59BW03CB48");
	pTempDFSData.m_StartTime = _T("");
	pTempDFSData.m_EndTime = _T("");
	pTempDFSData.m_LoadHandlerTime = _T("");
	pTempDFSData.m_UnloadHandlerTime = _T("");
	pTempDFSData.m_TpTime = _T("");
	pTempDFSData.m_PreGammaTime = _T("");
	pTempDFSData.m_TactTime = _T("");
	pTempDFSData.m_PreGammaContactStatus = _T("3");
	pTempDFSData.m_ModelID = _T("");
	pTempDFSData.m_IndexNum = _T("3");
	pTempDFSData.m_ChNum = _T("1");
	pTempDFSData.m_TpResult = _T("0");

	theApp.m_pFTP->DfsAddTransferFile(pTempDFSData);

	/*SJobDataShop dd;
	theApp.m_pFS->AddTransferFile(dd);*/

	
}


void CAni_Data_Serever_PCView::SendPlcDefectCode(int iNum, DfsDataValue PanelData, int iType)
{
	map<CString, CString>::iterator iter;
	DefectCodeRank pDefectCodeRank;
	DefectGradeRank pDefectGradeRank;
	PLCSendDefect PlcSendDefect;
	CString strCodeGrade, strCode = _T(""), strGrade = _T(""), strPanelID = _T(""), strFpcID = _T("");
	strPanelID = PanelData.m_PanelID;
	strFpcID = PanelData.m_FpcID;
	int iCount = 0;
	if (iType == Machine_AOI)
	{
		if (_ttoi(PanelData.m_PreGammaContactStatus) == m_dfsContactNG)
			strCodeGrade = CStringSupport::FormatString(_T("%s^%s"), theApp.m_strContactNgCode, theApp.m_strContactNgGrade);
		else if (_ttoi(PanelData.m_TpResult) == m_dfsTpNG)
			strCodeGrade = CStringSupport::FormatString(_T("%s^%s"), _T("XIMXDE"), _T("R1"));
		else if (_ttoi(PanelData.m_PreGammaContactStatus) == m_dfsPreGammaNG)
			strCodeGrade = CStringSupport::FormatString(_T("%s^%s"), _T("XIMXPG"), _T("R1"));
		else
		{
			// 优先从数据库读取，失败则回退到 INI 文件
			theApp.SetLoadResultCodeFromDB(strPanelID, strFpcID);
			if (theApp.m_Send_Result_Code_Map.empty())
			{
				theApp.SetLoadResultCode(strPanelID, strFpcID);
			}

			if (theApp.m_Send_Result_Code_Map.size() > 0)
			{
				for (auto Rank : theApp.m_strVecRank[AOI])
				{
					if (iCount == theApp.m_iNumberSendToPlc)
						break;

					iter = theApp.m_Send_Result_Code_Map.find(Rank.first);
					if (iter != theApp.m_Send_Result_Code_Map.end())
					{
						iCount++;
						if (iCount == 1)
							strGrade = iter->second;

						strCode.AppendFormat(_T("%s"), iter->first);
					}
				}
			}

			if (strCode.IsEmpty() == FALSE && strGrade.IsEmpty() == FALSE)
				strCodeGrade.Format(_T("%s^%s"), strCode, strGrade);
			else
				strCodeGrade = _T("");
		}
	}
	else
	{
		if (_ttoi(PanelData.m_PreGammaContactStatus) == m_dfsContactNG)
			strCodeGrade = CStringSupport::FormatString(_T("%s^%s"), theApp.m_strContactNgCode, theApp.m_strContactNgGrade);
		else if (_ttoi(PanelData.m_PreGammaContactStatus) == m_dfsPreGammaNG)
			strCodeGrade = CStringSupport::FormatString(_T("%s^%s"), _T("XIMXPG"), _T("R1"));
		else
			strCodeGrade = theApp.SetLoadOpvResultCode(strPanelID);
	}

	if (strCodeGrade.IsEmpty() == FALSE)
	{
		CStringArray responseTokens;
		CStringSupport::GetTokenArray(strCodeGrade, _T('^'), responseTokens);

		strCode = responseTokens[0];
		strGrade = responseTokens[1];

		PlcSendDefect.m_strCode = strCode;
		PlcSendDefect.m_strGrade = strGrade;
		PlcSendDefect.m_iCount = theApp.m_iNumberSendToPlc;
		theApp.SetSaveResultCode(strPanelID, strFpcID, _T("TotalDefectCode"), PlcSendDefect, iType);
	}
	else
	{
		PlcSendDefect.m_strGrade = theApp.m_strOkGrade;
		PlcSendDefect.m_iCount = theApp.m_iNumberSendToPlc;
		theApp.SetSaveResultCode(strPanelID, strFpcID, _T("TotalDefectCode"), PlcSendDefect, iType);
	}

	theApp.m_Send_Result_Code_Map.clear();
}

void CAni_Data_Serever_PCView::OnBnClickedButton5()
{

	CString strMsg = _T("Ni Babo");
	//theApp.getMsgBox2(MS_OK, strMsg, strMsg, strMsg);

	theApp.m_pMsgBoxAlarm->WaitShowHide(SW_SHOW, strMsg);

	//// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDFSInfo DfsInfo; 
	////DfsInfo.VisionLoadPanelDFSInfo(_T("TEST_PANELID"),0);
	//DfsData pDfsData;
	//DfsDataValue pDfsDateValue;

	//pDfsDateValue.m_FpcID = _T("A7G507J04ABH9");
	//pDfsDateValue.m_PanelID = _T("A7G507J04ABH9");
	//pDfsDateValue.m_StartTime = _T("2020-07-01- 21:29");
	//pDfsDateValue.m_LoadHandlerTime = _T("");
	//pDfsDateValue.m_UnloadHandlerTime = _T("");
	//pDfsDateValue.m_TpTime = _T("");
	//pDfsDateValue.m_PreGammaTime = _T("");
	//pDfsDateValue.m_EndTime = _T("2020-07-01- 21:32");
	//pDfsDateValue.m_TactTime = _T("");
	//pDfsDateValue.m_PreGammaContactStatus = _T("");
	//pDfsDateValue.m_ModelID = _T("");
	//pDfsDateValue.m_IndexNum = _T("0");
	//pDfsDateValue.m_ChNum = _T("0");
	//pDfsDateValue.m_TpResult = _T("");
	//pDfsDateValue.m_Contact = _T("");
	//pDfsDateValue.m_PreGamma = _T("");
	//pDfsDateValue.m_AOIInpsect = _T("");
	//pDfsDateValue.m_TpResult2 = _T("");
	//pDfsDateValue.m_Lumitop = _T("");
	//pDfsDateValue.m_mura = _T("");
	//pDfsDateValue.m_TypeNum = 0;
	//pDfsDateValue.m_StageNum = 0 + 1;
	//pDfsDateValue.m_ContactCount = _T("1");
	//pDfsDateValue.m_LoadeHandlerNUM = _T("2");
	//pDfsDateValue.m_UnLoadeHandlerNUM = _T("1");
	//theApp.m_OpvSocketManager[0].m_strOPID = _T("321");


	//DfsInfo.m_PanelSummaryInfo.AOI_PAENL_GRADE = _T("NG");
	//DfsInfo.m_PanelSummaryInfo.PRE_PAENL_GRADE = _T("OK");
	//DfsInfo.m_PanelSummaryInfo.DOT_PAENL_GRADE = _T("OK");
	//DfsInfo.m_PanelSummaryInfo.LUMITOP_PAENL_GRADE = _T("BYPASS");
	//DfsInfo.m_PanelSummaryInfo.OPV_PAENL_GRADE = _T("OK");
	//DfsInfo.m_PanelSummaryInfo.OPERATOR_ID = theApp.m_OpvSocketManager[0].m_strOPID;

	//if (pDfsDateValue.m_PanelID.IsEmpty())
	//	pDfsDateValue.m_PanelID = pDfsDateValue.m_FpcID;

	//theApp.m_pFTP->DfsAddTransferFile(pDfsDateValue);
	//SendPlcDefectCode(0, pDfsDateValue, 0);

	//theApp.m_PlcThread->JobDataStart(0, Machine_AOI);

	
}
