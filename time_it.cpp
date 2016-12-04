

//#include "stdafx.h"
#include "time_it.h"




//=========================================================================
//
//	Convert the system time to the 2 line epoch format
//	this will be used to generate delta time.
//
//==========================================================================
double Epoch_from_time(const tm* pTM)
{
	double ETime=pTM->tm_yday+1+(pTM->tm_hour+(pTM->tm_min+pTM->tm_sec/60.0)/60.0)/24.0;
	return ETime;

}
//=========================================================================
//
//	Convert the system time to the current JD
//	this will be used to generate delta time.
//
//==========================================================================
double JD_Now(const tm* pTM)
{
// use Meeus page 61
    double Y,M,D,H,S,A,B;
    double jd;
    Y=pTM->tm_year+1900;
    M=pTM->tm_mon+1;
    if(M<=2)
    {
        Y=Y-1;
        M=M=+2;
    }
    D=pTM->tm_mday;
    H=(pTM->tm_hour+(pTM->tm_min+pTM->tm_sec/60.0)/60.0)/24.0;
    D=D+H;
    A=(int)(Y/100);
    B=2-A+(int)(A/4);
    jd=(int)(365.25*(Y+4716))+(int)(30.6001*(M+1))+D+B-1524.5;
//======================

	double ETime=0.0;
	int yr=1999;
	if(pTM->tm_year>99)
		yr=2016;            // why hard coded ?????????
	ETime=JD_Jan0(yr);
	ETime=ETime+(pTM->tm_yday+1+(pTM->tm_hour+(pTM->tm_min+pTM->tm_sec/60.0)/60.0)/24.0);
	return ETime;

}
//========================================================
//
//	convert the epoch date to Julian date
//	inputs are the epoch year and day from the TLE
//
//========================================================
double Epoch2JD(double year,double day)
{
	double E2JD=0.0;
	if(year<50)
		year+=2000;
	else
		year+=1900;
	E2JD=JD_Jan0(year)+day;

	return E2JD;

}
//==================================================================
//
//	calculate fraction of a day
//
//=================================================================
double frac_of_day(double hr, double min, double sec)
{
	double T=min+sec/60.0;
	double T1=hr+T/60.0;
	double T2=T1/24.0;

	//double fraction=hr*3600+min*60+sec;
	//return fraction/86400;		//--- 86400 seconds per day

	return T2;
}
//======================================================================
//
//	calculate day of year
//
//======================================================================
int day_of_year(int month, int day, int year)
{
	if (month<0 || month>12)
		return 0;
	if (day<0 || day>31)
		return 0;
	int dayc=0;
	int leap=0;
	int mday[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	for(int i=0; i< month; i++)
	{
		dayc=dayc+mday[i];
	}
	if((year%4==0) && (((year%100 !=0 ) || (year%400==0)) && (month>2)))
		leap=1;
	else
		leap=0;


//==================================
// From Meeus page 65 and hard coded to no leap year
//==
    int N=(int)(275*month/9)-2*(int)((month+9)/12)+day-30;

	return dayc+day+leap;
}
//================================================================
//
//	calculate the Julian date using Meeus pg 61.
//  NO GOOD DUE TO DAY IS WRONG, returns midnight JDfro the input date
//=================================================================
double Julian_Day(int month,int day,int year)
{
	double JD=0;
	if(month<3)
	{
		year=year-1;
		month=month+12;
	}
	int int_yr=365.25*(year+4716);
	int int_month=30.6001*(month+1);
	int A=year/100;
	int B=2-A+(A/4);
	JD=int_yr+int_month+day+B-1524.5;
	return JD;
}
//===================================================================
//
//	calculate the Jan 0.0 Julian date for the given year
//	this uses Meeus pg 62
//
//===================================================================
double JD_Jan0(int year)
{
	#ifdef DEBUG
        year=1995; //result s/b 2449717.5.5
	#endif // DEBUG

	int yr=365.25*(year-1);
	int A=((year-1)/100);
	double temp=yr-A+(A/4)+1721424.5;
	double temp1=0.0;

	// second method using general JD equation but setting to Jan 0.0
	// page 61 Meeus and Celestrack page 2, Coord System, part 2
	// Month is Jan, so year is year-1 and month is M+12=13
	// D = 0 since day is day 0
	yr=year-1;
	A=(yr/100);
	int B=2-A+(A/4);
	int C=365.25*(yr+4716);
	int E=30.6001*14;
	temp1=(C+E)+B-1524.5;
	//temp1=((365.25*(yr+4716))+(30.6001*14))+B-1524.5;

	return temp;
}
//===============================================================================
//
//		Reference:  The 1992 Astronomical Almanac, page B6.
//		convert any Julian date to GMST
//
//      Greenwich Sidereal Time at UTC 0 hour
//      GST (at time T)= GST(0h UTC)+(earth rotation rate)*observer's east longitude
//      GST(0h UTC)=24110.54841 + TU * (8640184.812866 + TU * (0.093104 - TU * 6.2E-6));
//      TU=(JD-2451545.0)/36525 number of days of UT since
//      JD 2451545.0, Jan 01 2000, 12h UT
//
//===============================================================================
double ThetaG_JD(double jd)
{
    double UT,TU,DU,GMST,GMST0,GMSTemp,temp,temp1,temp2;
    double param, fractpart, intpart;
    double H, M, S;

	//jd=2449991.875;
	//jd=2449991.5;

    UT=jd+0.5;

    //cout<<"UT, jd+0.5 "<<UT<<endl;
	param=UT;

	fractpart = modf (param , &intpart);
	UT=fractpart;

    //cout<<"UT, fractpart "<<UT<<endl;
    jd=jd-UT;
    //cout<<"JD adjusted "<<jd<<endl;


	DU=jd-2451545.0;
	//DU is the number of days of Universal Time elapsed since JD 2451545.0 (2000 January 1, 12h UT1),
	//cout<<"DU, jd-2451545.0 "<<DU<<endl;
	TU=DU/36525;
	//cout<<"TU, DU/36525 "<<TU<<endl;
    // GMST from Explanatory Supplement page 50, GMST0 is in seconds
	GMST0=24110.54841 + TU * (8640184.812866 + TU * (0.093104 - TU * 6.2E-6));
	GMST0=fmod(GMST0,86400.0);
	if(GMST0<0)
        GMST0=GMST0+86400.0;

	GMST=fmod(1.00273790935*UT*SEC_PER_DAY+GMST0,SEC_PER_DAY);


	H=GMST0/3600;
	M=60*modf(H,&intpart);
	S=60*modf(M,&intpart);
	//cout<<"GMST0= \t"<<(int)H<<":"<<(int)M<<":"<<S<<endl;

	H=GMST/3600;
	M=60*modf(H,&intpart);
	S=60*modf(M,&intpart);
	//cout<<"GMST= \t"<<(int)H<<":"<<(int)M<<":"<<S<<endl;



	temp1=TWO_PI * GMST/SEC_PER_DAY;
	return temp1;
}
// ===================
// From Meeus page 63
// seems to be good as proper results from examples
double Calendar_date_from_JD(double jd)
{
   double alpha,A,F,Z,B,C,D,E;
   double Day,Month,Year;
   // Z is integer part, F is fractional part

   //jd=1507900.13;
    jd+=0.5;                            //jd=jd+0.5;
    double param;
    param = jd;
    F = modf (param , &Z); // fractional and integer parts
    //printf ("%f = %f + %f \n", param, intpart, fractpart);


    if(Z<2299161)
        A=Z;
    else
    {
        alpha=(int)((Z-1867216.25)/36524.25);
        A=Z+1+alpha-(int)(alpha/4);
    }
    B=A+1524;
    C=(int)((B-122.1)/365.25);
    D=(int)(365.25*C);
    E=(int)((B-D)/30.6001);
    Day=B-D-(int)(30.6001*E)+F;     // this is in UT, GMT time
    if(E<14)
        E--;
    else if((E=14) || (E=15))
            E=E-13;
    Month=E;
    if(Month>2)
        Year=C-4716;
    else
        Year=C-4715;

    return 0.0;
}
// ===================
// From Meeus page 65
// seems to be good as proper results from examples
int Day_of_Week(double jd)
{
    //jd=2434923.5;
    double F,Z;
    jd+=1.5;            //Get JD +1.5
    double param;
    param = jd;
    F = modf (param , &Z); // fractional and integer parts
    double DOY=fmod(Z,7.0);
    if(DOY>9)
        DOY=0;
    return 0;
}
/**********************************
    Collector routine to calculate and output:
    date, time, GMT, LMST, GMST

    Yorba Linda
    N 33.886014;
    33 53 9.6504

    W 117.789089  117 47 20.7204
    E 242.210911
     or  16.1473904666666 hours
     8.843644 min
     50.61864 sec
     16 hrs min 50.61864 sec
**********************************/

int All_Date_Time(double jd, const tm* pTM)
{
    //cout<<"Current JD is "<<jd<<endl;
    //cout<<"GMT is: "<<pTM->tm_hour<<":"<<pTM->tm_min<<":"<<pTM->tm_sec<<endl;
    //cout<<"GMT Date-time is: "<<asctime (pTM)<<endl;
    double ThetaZero=ThetaG_JD(jd);                 // in radians

    double YLlong =Rad(242.210911);

    double LMST=fmod(ThetaZero+YLlong,TWO_PI);         // radians
/****************************************
radians to HMS
2pi 360 degrees
15 degrees per hour
86400 seconds per day(24*60*60)
*****************************************/
    double H,M,S;

    double LMST_HMS=LMST*((360/TWO_PI))/15;
    H=LMST_HMS;

    double param, fractpart, intpart;
    param = H;
    fractpart = modf (param , &intpart);
    M=fractpart*60;
    param=M;
    fractpart = modf (param , &intpart);
    S=fractpart*60;
    //cout<<"LMST= \t"<<(int)H<<":"<<(int)M<<":"<<S<<endl;

    return 0;
}

