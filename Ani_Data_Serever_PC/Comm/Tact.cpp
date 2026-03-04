#include "stdafx.h"
#include "Tact.h"
#include "Ani_Data_Serever_PC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTact::CTact() : m_startTime(0)
{

}

void CTact::BeginTactTime()
{
	m_startTime = ::GetTickCount();
}

BOOL CTact::IsBeginTactTime()
{
	return m_startTime >0;
}

//<<AMT에서 같이 집계하는용 분리해야하기 떄문에 bUnloadFlag 하나 만들었음
//저거 있으면 Unload Plc Thread 에서 오는용이고 나머지는 PLC Thread 에서 오는 용도이니깐 머리아파하지말고 사용하길
//bUnloadFlag 이것이 같이 오면 금마는 무조건 패널이 한장이라서 저렇게 분류한거임...
void CTact::EndTactTime(int nUnitNum, int iShiftNum)
{
#if _SYSTEM_AMTAFT_
	if (m_startTime || nUnitNum == Panel_Tact)
#else
	if (m_startTime)
#endif
	{
		DWORD dwCurPos = ::GetTickCount();
		DWORD PanelTactTime = dwCurPos - m_startTime;

#if _SYSTEM_AMTAFT_ // sylee INDEX 만
		switch (nUnitNum)
		{
		case INDEX_AZONE:
		case INDEX_BZONE:
		case INDEX_CZONE:
		case INDEX_DZONE:		PanelTactTime = (dwCurPos - m_startTime) / 4; break;
		case Panel_Tact:
			PanelTactTime = theApp.m_pTactTimeList[IN_LOAD_STAGE_WAIT].GetAvgTactTime() < theApp.m_pTactTimeList[UNLOADER_STAGE].GetAvgTactTime() ? theApp.m_pTactTimeList[UNLOADER_STAGE].GetAvgTactTime() - theApp.m_pTactTimeList[IN_LOAD_STAGE_WAIT].GetAvgTactTime() : theApp.m_pTactTimeList[IN_LOAD_STAGE_WAIT].GetAvgTactTime() - theApp.m_pTactTimeList[UNLOADER_STAGE].GetAvgTactTime();
			break;
		default:				PanelTactTime = (dwCurPos - m_startTime) / 2; break;
		}
#endif

		m_tactTimeList.push_back(PanelTactTime);
		m_iSumTimeValue += PanelTactTime;
		m_iTactTimeCount++;
		while (m_tactTimeList.size() > MAX_TACT_NUM)
			m_tactTimeList.pop_front();
	}

	m_startTime = 0;
}

int CTact::GetLastTactTime() // 사이클 텍타임
{
	if (m_tactTimeList.size() == 0)
		return 0;

	return m_tactTimeList.back();
}

int CTact::GetAvgTactTime()
{
	if (m_iTactTimeCount == 0)
		return 0;

	return (int)(m_iSumTimeValue / m_iTactTimeCount);
}