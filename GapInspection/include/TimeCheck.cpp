#include "stdafx.h"
#include "TimeCheck.h"
#include <time.h>

//_____________________________________________________________________________
CTimerCheck::CTimerCheck(long msec) // msec
{
	m_CheckTime = msec;
}

CTimerCheck::~CTimerCheck()
{
}

//_____________________________________________________________________________
void CTimerCheck::StartTimer()
{
	StartTime = GetCurrentTime();
}

//_____________________________________________________________________________
bool CTimerCheck::IsLessThan(long msec)
{
	//End Time 삭제 [20090209 LJY]
//	EndTime = GetCurrentTime();

	if (TimePassed() < msec)
		return TRUE;
	else
		return FALSE;
}

//_____________________________________________________________________________
bool CTimerCheck::IsMoreThan(long msec)
{
	//End Time 삭제 [20090209 LJY]
//	EndTime = GetCurrentTime();

	if (TimePassed() >= msec)
		return TRUE;
	else
		return FALSE;
}

//_____________________________________________________________________________
long CTimerCheck::TimePassed()
{
	//End Time 추가[20090209 LJY]
	EndTime = GetCurrentTime();
	return (EndTime - StartTime);
}

//_____________________________________________________________________________
// 시간이 경과된경우 'TRUE' 를 리턴 한다.
bool CTimerCheck::IsTimeOver()
{
	//End Time 삭제 [20090209 LJY]
//	EndTime = GetCurrentTime();

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
