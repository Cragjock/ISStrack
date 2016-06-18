

#include <time.h>
#ifndef _TIME_IT_H
#define _TIME_IT_H


double frac_of_day(double hr, double min, double sec);		// Fraction of a day
int day_of_year(int,int,int);								// Day of Year
double Julian_Day(int month,int day,int year);				// Julian Day
double JD_Jan0(int year);									// Julian Day at Jan 0.0
double Epoch_from_time(const tm*);
double ThetaG_JD(double jd);
double JD2Epoch(double jd);
double Epoch2JD(double year,double day);
double JD_Now(const tm* pTM);
double Calendar_date_from_JD(double jd);
int Day_of_Week(double jd);
int All_Date_Time(double jd, const tm* pTM);

/**************************************************************
		Get the current local time from the PC system
		This will be used as Time Now.
		Epoch time is extracted from the 2 line elements
		The difference will be the "time since" time for satellite
		position calculations

	the tm struct:

	tm_sec: Seconds after minute (0-59)
	tm_min: Minutes after hour (0-59)
	tm_hour: Hours after midnight (0-23)
	tm_mday: Day of month (1-31)
	tm_mon: Month (0-11; January = 0)
	tm_year: Year (current year minus 1900)
	tm_wday: Day of week (0-6; Sunday = 0)
	tm_yday: Day of year (0-365; January 1 = 0)
	tm_isdst: Positive value if daylight saving time is in effect;
		0 if daylight saving time is not in effect;
		negative value if status of daylight saving time is unknown.
		The C run-time library assumes the United States’s rules for implementing
		the calculation of Daylight Saving Time (DST).
***************************************************************/

#endif
