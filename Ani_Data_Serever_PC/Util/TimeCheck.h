#ifndef	_CTIMER_CHECK
#define	_CTIMER_CHECK

class CTimerCheck
{
public:
	CTimerCheck(long checktime = 100); // 단위는 msec...
	~CTimerCheck();

public:
	long TimePassed();	// 현재 까지의 시간을 리턴....
	long GetCheckTime();
	void SetCheckTime(long ctime);
	bool IsTimeOver();
	bool IsMoreThan(long ctime);	// 단위는 msec..
	bool IsLessThan(long ctime);	// 단위는 msec..
	void StartTimer();
	void StopTimer();
	BOOL IsActive() { return m_bActive; }

private:
	BOOL m_bActive;
	long m_CheckTime;
	long StartTime;
	long EndTime;
};

template<class CheckFn>
BOOL TimeOutCheck(CheckFn checkFn, int timeOutTime = 4000)
{
	CTimerCheck timer;
	timer.SetCheckTime(timeOutTime);
	timer.StartTimer();
	while (checkFn())
	{
		if (timer.IsTimeOver())
		{
			timer.StopTimer();
			return FALSE;
		}
		Sleep(10);
	}

	return TRUE;
}

template<class CheckFn1, class CheckFn2>
BOOL TimeOutCheck(CheckFn1 checkFn1, CheckFn2 checkFn2, int timeOutTime = 4000)
{
	CTimerCheck timer;
	timer.SetCheckTime(timeOutTime);
	timer.StartTimer();
	while (checkFn1())
	{
		if (timer.IsTimeOver())
		{
			timer.StopTimer();
			return FALSE;
		}
		else
		{
			if (checkFn2() == FALSE)
				return FALSE;
		}

		Sleep(10);
	}

	return TRUE;
}

#endif