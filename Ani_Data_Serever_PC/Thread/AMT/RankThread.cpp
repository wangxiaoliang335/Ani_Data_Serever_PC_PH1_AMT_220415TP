
#include "stdafx.h"

#if _SYSTEM_AMTAFT_
#include "DlgMainView.h"
#include "RankThread.h"
#include "DFSInfo.h"
#include "DataInfo.h"

CRankThread::CRankThread()
{
	m_hQuit = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CRankThread::~CRankThread()
{
}

void CRankThread::ThreadRun()
{
	map<CString, CString>::iterator iter2;
	map<CString, map<CString, CString>>::iterator iter;
	map<CString, int>::iterator DefectCodeiter;
	map<CString, map<CString, int>>::iterator Totalrankiter;
	map<CString, int>::iterator rankiter;
	map<CString, vector<pair< CString, ResultCodeRank>>>::iterator Totaliter;
	ResultCodeRank RankList;
	PLCSendDefect defectinfo;
	CStringArray responseTokens;

	CString strPath, strPanelID, strFpcID, strDefectCode, strInspName, strFilePath, strKey;
	CDFSInfo DfsInfo;
	int iPanelNum, iInspNum, iIndexNum, iValue;
	BOOL bFlag = FALSE;

	while (::WaitForSingleObject(m_hQuit, 200) != WAIT_OBJECT_0)
	{
		if (!m_RankCodeList.empty())
		{
			while (!m_RankCodeList.empty())
			{
				theApp.m_pTraceLog->LOG_INFO(_T("**************** START!!!! ****************"));
				strPath = strPanelID = strFpcID = strDefectCode = strInspName = strFilePath = _T("");
				RankList.Reset();
				defectinfo.Reset();
				DfsInfo.Clear();
				responseTokens.RemoveAll();

				m_csDfsData.Lock();
				strDefectCode = m_RankCodeList.front();
				m_csDfsData.Unlock();

				if (strDefectCode.IsEmpty())
				{
					theApp.m_pTraceLog->LOG_INFO(_T("**************** Defect Code What Error ****************"));
				}
				else
				{
					if (strDefectCode.Find(_T('^')) != -1)
					{
						CStringSupport::GetTokenArray(strDefectCode, _T('^'), responseTokens);
						if (responseTokens.GetSize() < 5)
						{
							theApp.m_pTraceLog->LOG_INFO(_T("**************** Defect Code Parser Size Error ****************"));
						}
						else
						{
							strPanelID = responseTokens[0];
							strFpcID = responseTokens[1];
							iPanelNum = _ttoi(responseTokens[2]);
							iIndexNum = _ttoi(responseTokens[3]);
							iInspNum = _ttoi(responseTokens[4]);

							if (iInspNum == RankAOI)
							{
								strPath = DFS_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\AOI\\") + strPanelID + _T(".csv"); 
								strInspName = _T("AOI");
							}
							else if (iInspNum == RankViewing)
							{
								strPath = DFS_VIEWING_ANGLE_SHARE_PATH + GetDateString2() + _T("\\") + strPanelID + _T("\\Viewing\\") + strPanelID + _T(".csv");
								strInspName = _T("Viewing");
							}

							DfsInfo.DFSDefectBeginLoad(strPath, strInspName, FALSE);
						}
					}
					else
						theApp.m_pTraceLog->LOG_INFO(_T("**************** Parsing Error ****************"));
				}

				if (DfsInfo.m_mapPanelDefect.size() > 0)
				{
					for (auto Rank : theApp.m_strVecRank[AOI])
					{
						if (defectinfo.m_iCount == theApp.m_iNumberSendToPlc)
							break;

						iter = DfsInfo.m_mapPanelDefect.find(strInspName);
						if (iter != DfsInfo.m_mapPanelDefect.end())
						{
							iter2 = iter->second.find(Rank.first);
							if (iter2 != iter->second.end())
							{
								defectinfo.m_strCode = iter2->first;
								defectinfo.m_strGrade = iter2->second;
								defectinfo.m_iCount++;
							}
						}
						
						if (defectinfo.m_iCount > 0)
							theApp.SetSaveResultCode(strPanelID, strFpcID, strInspName, defectinfo, Machine_AOI);
					}
				}
				else
					theApp.m_pTraceLog->LOG_INFO(_T("**************** PanelID [%s] FpcID [%s] SendPlcCode size error ****************"), strPanelID, strFpcID);

				if (DfsInfo.m_mapDefectCodeList.size() > 0)
				{
					for (auto Rank : theApp.m_strVecRank[AOI])
					{
						DefectCodeiter = DfsInfo.m_mapDefectCodeList.find(Rank.first);
						if (DefectCodeiter != DfsInfo.m_mapDefectCodeList.end())
						{
							RankList.m_iCh = iPanelNum + 1;
							RankList.m_strZone = PG_IndexName[iIndexNum];
							RankList.m_strResultCode = DefectCodeiter->first;
							RankList.m_iResultCodeCount = DefectCodeiter->second;

							if (iInspNum == RankAOI)
							{
								theApp.m_iInspAoiMaxCount++;
								if (theApp.m_iInspAoiMaxCount > 999)
								{
									theApp.m_iInspAoiMaxCount = 0;
									bFlag = TRUE;
								}
							}
							else
							{
								theApp.m_iInspViewingMaxCount++;
								if (theApp.m_iInspViewingMaxCount > 999)
								{
									theApp.m_iInspViewingMaxCount = 0;
									bFlag = TRUE;
								}
							}

							theApp.SetSaveHistoryCode(strPanelID, strInspName, RankList, bFlag);

							strKey = _T("");
							iValue = 0;
							strKey.Format(_T("%s"), CStringSupport::FormatString(_T("%s^%d^%s"), RankList.m_strZone, RankList.m_iCh, RankList.m_strResultCode));

							Totalrankiter = theApp.m_mapRankCodeCount[theApp.m_lastShiftIndex].find(strInspName);
							if (Totalrankiter != theApp.m_mapRankCodeCount[theApp.m_lastShiftIndex].end())
							{
								rankiter = Totalrankiter->second.find(strKey);
								if (rankiter != Totalrankiter->second.end())
								{
									rankiter->second += RankList.m_iResultCodeCount;
									iValue = rankiter->second;
								}
								else
								{
									Totalrankiter->second.insert(make_pair(strKey, RankList.m_iResultCodeCount));
									iValue = RankList.m_iResultCodeCount;
								}

								theApp.SetSaveRankCode(strInspName, strKey, iValue, bFlag);
							}
							else
								theApp.m_pTraceLog->LOG_INFO(_T("**************** PanelID [%s] rank error****************"), strPanelID);

							Totaliter = theApp.m_mapRankTotalList[theApp.m_lastShiftIndex].find(strInspName);
							if (Totaliter != theApp.m_mapRankTotalList[theApp.m_lastShiftIndex].end())
								Totaliter->second.push_back(make_pair(strPanelID, RankList));
						}
					}
				}
				else
					theApp.m_pTraceLog->LOG_INFO(_T("**************** PanelID [%s] RankCode size error ****************"), strPanelID);

				theApp.m_pTraceLog->LOG_INFO(_T("**************** END!!!! ****************"));
				m_csDfsData.Lock();
				m_RankCodeList.pop();
				m_csDfsData.Unlock();
			}
		}
	}
}

void CRankThread::AddRankCodeList(CString strPanel, CString strFpcID, int iPanelNum, int iIndexNum, int iInspNum)
{
	if (strPanel.GetLength() > 0)
	{
		CString strInspName = InspectName[iInspNum];
		CString msg = CStringSupport::FormatString(_T("%s^%s^%d^%d^%d"), strPanel, strFpcID, iPanelNum, iIndexNum, iInspNum);
		theApp.m_pTraceLog->LOG_INFO(_T("******************[%s] %s Start ******************"), strInspName, msg);
		m_csDfsData.Lock();
		m_RankCodeList.push(msg);
		m_csDfsData.Unlock();
	}
}

UINT CRankThread::RankThreadProc(LPVOID pParam)
{
	CRankThread* pThis = reinterpret_cast<CRankThread*>(pParam);
	_ASSERTE(pThis != NULL);
	pThis->ThreadRun();
	return 1L;

}

BOOL CRankThread::CreateTask(){
	BOOL bRet = TRUE;
	m_pThreadRank = ::AfxBeginThread(RankThreadProc, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!m_pThreadRank)
		bRet = FALSE;
	m_pThreadRank->m_bAutoDelete = FALSE;	/// ¾²·¹µå Á¾·á½Ã WaitForSingleObject Àû¿ëÀ§ÇØ...
	m_pThreadRank->ResumeThread();
	return TRUE;
}

void CRankThread::CloseTask()
{
	if (m_pThreadRank != NULL)
	{
		SetEvent(m_hQuit);
		Delay(100, TRUE);
		if (::WaitForSingleObject(m_pThreadRank->m_hThread, 1000) == WAIT_TIMEOUT)
		{
			SetEvent(m_hQuit);
			Delay(100, TRUE);
			if (::WaitForSingleObject(m_pThreadRank->m_hThread, 1000) == WAIT_TIMEOUT) {
				::TerminateThread(m_pThreadRank->m_hThread, 1L);
				theApp.m_pTraceLog->LOG_INFO(_T("Terminate Rank Thread"));
			}
		}
		delete m_pThreadRank;
		m_pThreadRank = NULL;

	}
	if (m_hQuit)
	{
		CloseHandle(m_hQuit);
		m_hQuit = NULL;
	}
}
#endif


