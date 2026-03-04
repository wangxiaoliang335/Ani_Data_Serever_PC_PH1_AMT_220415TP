
#include "stdafx.h"

#if _SYSTEM_AMTAFT_

#include "DlgMainView.h"
#include "DlgMainLog.h"

#include "TpThread.h"

CTpThread::CTpThread()
{
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bFirstStatus = TRUE;
}

CTpThread::~CTpThread()
{
}

void CTpThread::ThreadRun()
{	
	while (::WaitForSingleObject(m_hQuit, 100) != WAIT_OBJECT_0)
	{
		//if (theApp.m_bExitFlag == FALSE)
		//	return;
		//
		//if (theApp.m_TpConectStatus || theApp.m_TpPassMode)
		//{
		//	if (m_bFirstStatus)
		//	{
		//		m_bFirstStatus = FALSE;
		//		time_check.SetCheckTime(60000);
		//		time_check.StartTimer();
		//		for (int ii = 0; ii < 18; ii++)
		//			theApp.m_TpSocketManager.SendTPMessage(ii, TP_CheckConnect);
		//
		//	}
		//
		//	if (time_check.IsTimeOver())
		//	{
		//		time_check.StartTimer();
		//
		//		if (theApp.m_TpSocketManager.m_iTpSocketCheckCount > 5)
		//			theApp.m_TpSocketManager.TpLogMessage(CStringSupport::FormatString(_T("TP PC Client Drop")));
		//
		//		theApp.m_TpSocketManager.m_iTpSocketCheckCount++;
		//	}
		//}
	}
}

UINT CTpThread::TpThreadProc(LPVOID pParam)
{
	CTpThread* pThis = reinterpret_cast<CTpThread*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;

} // end TpThreadProc

BOOL CTpThread::CreateTask(){
	BOOL bRet = TRUE;
	m_pThreadTp = ::AfxBeginThread(TpThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadTp)
		bRet = FALSE;
	m_pThreadTp->m_bAutoDelete = FALSE;
	m_pThreadTp->ResumeThread();
	return bRet;
}

void CTpThread::CloseTask()
{
	if (m_pThreadTp != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadTp->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadTp->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadTp->m_hThread, 1L);
				theApp.m_pTpLog->LOG_INFO(_T("Terminate TP Thread"));
			}
		}
		delete m_pThreadTp;
		m_pThreadTp = NULL;

	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}
#endif