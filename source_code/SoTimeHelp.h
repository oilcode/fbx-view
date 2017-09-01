//--------------------------------------------------------------------
#ifndef _SoTimeHelp_h_
#define _SoTimeHelp_h_
//----------------------------------------------------------------
struct stTimeTM
{
	//e.g. 2013, 2014.
	int nYear;
	//[1, 12], 1 == January.
	int nMonth;
	//[1, 31], 1 is the first day.
	int nDayInMonth;
	//[1, 7], 1 is Monday, 7 is Sunday.
	int nDayInWeek;
	//[1, 366], 1 is the first day.
	int nDayInYear;
	//[0, 23]
	int nHour;
	//[0, 59]
	int nMinute;
	//[0, 59]
	int nSecond;
	//[0, 999]
	int nMillisecond;

	stTimeTM()
	:nYear(-1), nMonth(-1), nDayInMonth(-1), nDayInWeek(-1), nDayInYear(-1)
	,nHour(-1), nMinute(-1), nSecond(-1), nMillisecond(-1)
	{
	}
};
//--------------------------------------------------------------------
class SoTimeHelp
{
public:
	//Unix Timestamp. UTC time.
	//1970-1-1 0:0:0
	//2038-1-18 19:14:07
	static int GetTimestamp();
	//broken-down time. UTC time.
	static void GetTimeTM(stTimeTM* pMT);
	//stTimeTM::nMillisecond is valid
	static void GetTimeTM_Millisecond(stTimeTM* pMT);

	//local time.
	static int GetLocalTimestamp();
	//local time.
	static void GetLocalTimeTM(stTimeTM* pMT);
	//stTimeTM::nMillisecond is valid
	static void GetLocalTimeTM_Millisecond(stTimeTM* pMT);

	static int GetTimestampByTimeTM(stTimeTM* pMT);
	static void GetTimeTMByTimestamp(stTimeTM* pMT, const int nTimestamp);
};
//--------------------------------------------------------------------
#endif //_SoTimeHelp_h_
//--------------------------------------------------------------------
