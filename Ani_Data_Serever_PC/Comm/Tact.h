#pragma once

using namespace std;
class CTact
{
public:
	int m_numPanel;
	int m_startTime;
	int m_iSumTimeValue;
	int m_iTactTimeCount;

	list<int> m_tactTimeList;

	CTact();
	void BeginTactTime();		// Cell Load시 호출
	BOOL IsBeginTactTime();
	void EndTactTime(int nUnitNum, int iShiftNum);			// Cell Unload시 호출
	int GetLastTactTime();
	int GetAvgTactTime();
};