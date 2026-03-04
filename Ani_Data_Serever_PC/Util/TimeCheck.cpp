#include "stdafx.h"
#include "TimeCheck.h"
#include <time.h>

//_____________________________________________________________________________
CTimerCheck::CTimerCheck(long msec) // msec
{
	m_bActive = FALSE;
	m_CheckTime = msec;
}

CTimerCheck::~CTimerCheck()
{
}

//_____________________________________________________________________________
void CTimerCheck::StartTimer()
{
	m_bActive = TRUE;
	StartTime = GetCurrentTime();
}

void CTimerCheck::StopTimer()
{
	m_bActive = FALSE;
	EndTime = GetCurrentTime();
}

//_____________________________________________________________________________
bool CTimerCheck::IsLessThan(long msec)
{
	if (TimePassed() < msec)
		return TRUE;
	else
		return FALSE;
}

//_____________________________________________________________________________
bool CTimerCheck::IsMoreThan(long msec)
{
	if (TimePassed() >= msec)
		return TRUE;
	else
		return FALSE;
}

//_____________________________________________________________________________
long CTimerCheck::TimePassed()
{
	if (m_bActive == TRUE)
		EndTime = GetCurrentTime();
	return (EndTime - StartTime);
}

//_____________________________________________________________________________
// 시간이 경과된경우 'TRUE' 를 리턴 한다.
bool CTimerCheck::IsTimeOver()
{
	if (m_bActive == FALSE)
		return FALSE;

	if (TimePassed() < m_CheckTime)
		return FALSE;
	else
		return TRUE;
}

void CTimerCheck::SetCheckTime(long msec)
{
	m_CheckTime = msec;
}

long CTimerCheck::GetCheckTime()
{
	return m_CheckTime;
}
