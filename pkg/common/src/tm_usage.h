// **************************************************************************
// File       [ tm_usage.h ]
// Author     [ littleshamoo ]
// Synopsis   [ CPU time and Memory usage calculator ]
// Date       [ Ver 2.0 started 2010/03/23 ]
// History    [ created TmStat structure to store usage ]
// **************************************************************************

#ifndef _COMMON_TM_USAGE_H_
#define _COMMON_TM_USAGE_H_

namespace CommonNs
{

	struct TmStat
	{
		long vmSize; // in kilobytes
		long vmPeak; // in kilobytes
		long vmDiff; // in kilobytes
		long rTime;	 // in micro seconds
		long uTime;	 // in micro seconds
		long sTime;	 // in micro seconds
	};

	class TmUsage
	{
	public:
		TmUsage();
		~TmUsage();

		bool totalStart();
		bool periodStart();
		bool getTotalUsage(TmStat &st) const;
		bool getPeriodUsage(TmStat &st) const;
		bool checkUsage(TmStat &st) const;

	private:
		TmStat tStart_;
		TmStat pStart_;
	};

};

#endif