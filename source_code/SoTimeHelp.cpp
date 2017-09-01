//--------------------------------------------------------------------
#include "SoTimeHelp.h"
#include <cstdlib>
#if defined(WIN32) || defined(_WIN32)
#include <time.h>
#include <Windows.h>
#else
#include <sys/time.h>
#endif
//--------------------------------------------------------------------
void GetTimeTMByTM(stTimeTM* pMT, const tm* pmt)
{
	if (pMT == 0 || pmt == 0)
	{
		return;
	}
	pMT->nYear = pmt->tm_year + 1900;
	pMT->nMonth = pmt->tm_mon + 1;
	pMT->nDayInMonth = pmt->tm_mday;
	pMT->nDayInWeek = (pmt->tm_wday == 0) ? 7 : pmt->tm_wday;
	pMT->nDayInYear = pmt->tm_yday + 1;
	pMT->nHour = pmt->tm_hour;
	pMT->nMinute = pmt->tm_min;
	pMT->nSecond = pmt->tm_sec;
}
//--------------------------------------------------------------------
int SoTimeHelp::GetTimestamp()
{
#if defined(WIN32) || defined(_WIN32)
	return (int)time(0);
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec;
#endif
}
//--------------------------------------------------------------------
void SoTimeHelp::GetTimeTM(stTimeTM* pMT)
{
	return GetTimeTMByTimestamp(pMT, GetTimestamp());
}
//--------------------------------------------------------------------
void SoTimeHelp::GetTimeTM_Millisecond(stTimeTM* pMT)
{
	if (pMT == 0)
	{
		return;
	}
#if defined(WIN32) || defined(_WIN32)
	SYSTEMTIME stTime;
	GetSystemTime(&stTime);
	pMT->nYear = stTime.wYear;
	pMT->nMonth = stTime.wMonth;
	pMT->nDayInMonth = stTime.wDay;
	pMT->nDayInWeek = stTime.wDayOfWeek;
	pMT->nHour = stTime.wHour;
	pMT->nMinute = stTime.wMinute;
	pMT->nSecond = stTime.wSecond;
	pMT->nMillisecond = stTime.wMilliseconds;
#else
	//do nothing
#endif
}
//--------------------------------------------------------------------
int SoTimeHelp::GetLocalTimestamp()
{
	stTimeTM kMT;
	GetLocalTimeTM(&kMT);
	return GetTimestampByTimeTM(&kMT);
}
//--------------------------------------------------------------------
void SoTimeHelp::GetLocalTimeTM(stTimeTM* pMT)
{
	tm ktm;
	time_t _TIME_ = GetTimestamp();
#if defined(WIN32) || defined(_WIN32)
	localtime_s(&ktm, &_TIME_);
#else
	localtime_r(&_TIME_, &ktm);
#endif
	GetTimeTMByTM(pMT, &ktm);
}
//--------------------------------------------------------------------
void SoTimeHelp::GetLocalTimeTM_Millisecond(stTimeTM* pMT)
{
	if (pMT == 0)
	{
		return;
	}
#if defined(WIN32) || defined(_WIN32)
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	pMT->nYear = stTime.wYear;
	pMT->nMonth = stTime.wMonth;
	pMT->nDayInMonth = stTime.wDay;
	pMT->nDayInWeek = stTime.wDayOfWeek;
	pMT->nHour = stTime.wHour;
	pMT->nMinute = stTime.wMinute;
	pMT->nSecond = stTime.wSecond;
	pMT->nMillisecond = stTime.wMilliseconds;
#else
	//do nothing
#endif
}
//--------------------------------------------------------------------
int SoTimeHelp::GetTimestampByTimeTM(stTimeTM* pMT)
{
	if (pMT == 0)
	{
		return 0;
	}

	tm ktm;
	ktm.tm_sec = pMT->nSecond;
	ktm.tm_min = pMT->nMinute;
	ktm.tm_hour = pMT->nHour;
	ktm.tm_mday = pMT->nDayInMonth;
	ktm.tm_mon = pMT->nMonth - 1;
	ktm.tm_year = pMT->nYear - 1900;
	return (int)mktime(&ktm);
}
//--------------------------------------------------------------------
void SoTimeHelp::GetTimeTMByTimestamp(stTimeTM* pMT, const int nTimestamp)
{
	if (pMT == 0)
	{
		return;
	}
	//gmtime_r() is the thread-safe version of gmtime().
	//for windows, its name is gmtime_s;
	//for Linux, its name is gmtime_r.
	tm ktm;
	time_t _TIME_ = (time_t)nTimestamp;
#if defined(WIN32) || defined(_WIN32)
	gmtime_s(&ktm, &_TIME_);
#else
	gmtime_r(&_TIME_, &ktm);
#endif
	GetTimeTMByTM(pMT, &ktm);
}
//--------------------------------------------------------------------
