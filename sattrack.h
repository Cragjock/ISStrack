
#ifndef _SATTRACK_H
#define _SATTRACK_H


typedef struct tag_VectorPQ
	{
		double P;
		double Q;
	} VectorPQ;
//====================================================================
//
//	structure for an observer's position
//
//====================================================================
typedef struct tag_Observer
	{
		char obsname[MAXNAME];	// location name
		double lat;				// latitude in radians
		double lon;				// longitude in radians
		double height;			// height in kilometers
		double theta;			// LMST angle in radians
	} Observer;

typedef struct tag_VectorIJK
	{
		double I;				// I component
		double J;				// J component
		double K;				// K component
		double Mag;				// Magnitude
	} VectorIJK;
//====================================================================
//
//	structure for an observer's look angle
//
//====================================================================
typedef struct tag_VectLook
	{
		double AZ;				// Azimuth component
		double EL;				// Elevation component
		double RG;				// Range component
	} VectLook;


//====================================================================
//
//	structure for any satellite subpoint on ground
//
//====================================================================
typedef struct tag_SATSUB
	{
		double lat;
		double lon;
		double range;
	} SATSUB;
//====================================================================
//
//	structure for any ellipse parameters
//
//====================================================================
typedef struct tag_ellipse
	{
		double a;				// semi-major axis
		double b;				// semi-minor axis
		double ecc;				// eccentricity
		double p;				// semi-latus
		double rp;				// perigee distance a(1-ecc)
		double hp;				// height at perigee rp-R
		double ra;				// apogee distance a(1+ecc)
		double ha;				// height at apogee ra-R

	} ELLIPSE;
//====================================================================
//
//	structure for a satellite's position
//
//====================================================================
typedef struct tag_satpos
	{
		double X;				// x component in ECI frame
		double Y;				// y component in ECI frame
		double Z;				// z component in ECI frame
		double XDOT;
		double YDOT;
		double ZDOT;
		double Rmag;            // magnitude
		double angle_f;         // true anomaly
	} SATPOS;
//===============================================================================
//
//  Function prototypes for basic orbit and ellipse data
//
//==============================================================================
double period_from_MM(double mm);
double SMA_from_Period(double period);
double Vmag_from_R_SMA(double r,double a);
double Semi_Latum(double a, double e);
double Rmag_from_Mean_Anamoly(double p, double e, double ma);
double Rad_componentPQ(double r, double V, VectorPQ &PQ);
double Vel_componentPQ(double p, double ecc, double V, VectorPQ &PQ);




//==============================================================================
//
//	Structure for capturing data from the
//	two line element input.
//
//==============================================================================
typedef struct _SATELSET
{
	// -- LINE 1 DATA ------------------------------
	char    szName[32];			// Satellite name
    int     iCatalogNum;        // Catalog number
    int     iEpochYear;         // Epoch Year
    int     iLaunchYear;        // Launch year
    int     iLaunchNum;         // Launch Number
    char    szLaunchPiece[5];   // Launch piece
    double  dEpochDay;          // Epoch day (incl. year)
    double  dDecayRate;         // Decay rate
    double  dNddot6;            // Nddot6
    double  dBSTAR;             // drag term
    int     iModelType;         // prediction model used (1 - 5)
    int     iElementSet;        // Element set
	// -- LINE 2 DATA -------------------------------
    double  dIncl;				// Inclination
    double  dRAAN;				// Right ascension of ascending node
    double  dEcc;				// Eccentricity
    double  dAOP;				// Argument of Perigee (Omega)
    double  dMA;				// Mean anomaly, position at Epoch from perigee
    double  dMM;				// Mean motion from rev/day
    int     iEpochRev;          // Rev. number at epoch time

    // Next data set
    struct _SATELSET* pNext;
} SATELSET;

//==== Keplerian elements for orbits. Initially equal to TLE
// updated for perturbations.
typedef struct _KEPELEMENTS
    {
        double  A;              //Semi major axis
        double  DecayRate;      // Decay rate
        double  Nddot6;         // Nddot6
        double  BSTAR;          // drag term
        double  Incl;           // Inclination
        double  RAAN;           // Right ascension of ascending node
        double  Ecc;            // Eccentricity
        double  AOP;            // Argument of Perigee (Omega)
        double  MA;				// Mean anomaly, position at Epoch from perigee
        double  MM;				// Mean motion from rev/day
        int     iEpochRev;      // Rev. number at epoch time
        _KEPELEMENTS(SATELSET Eset)
        {
            A=0.0;
            DecayRate=Eset.dDecayRate;
            Nddot6=Eset.dNddot6;
            BSTAR=Eset.dBSTAR;
            Incl=Eset.dIncl;
            RAAN=Eset.dRAAN;
            Ecc=Eset.dEcc;
            AOP=Eset.dAOP;
            MA=Eset.dMA;
            MM=Eset.dMM;
            iEpochRev=Eset.iEpochRev;
        }
    } KEPELEMENTS;


int Read_TLE(char* file, SATELSET &eset);
void Read_Line(SATELSET*, int l,char*);

//================================================================================
//
//	Time and Date Functions
//
//================================================================================
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

//==================================================================================
//
//		Solutions for Keplers equation M=n(t-T)=E-e*sinE
//			M=Mean anomaly
//			n=mean motion
//			e=eccentricity
//			E=eccentricity anomaly
//			t-T= time since
//		from the TLE MA and time since, an initial M can be determined then use
//		that as a first choice for E. once an E is reach,
//		then the true anomaly can be determined.
//		Inputs are initial M,eccentricity and returns true anomaly.
//
//===================================================================================
SATPOS SatPos(double dDeltaT, SATELSET* pSatTLE);
SATPOS clean_SatPos(double dDeltaT, SATELSET* pSatTLE);
double Perturbed(double element, int perturbed_type);
//double SatPos(double dDeltaT, SATELSET* pSatTLE, SATPOS *pSatPos);
double Kep_Newton(double,double);
double kep_binary(double M0, double ecc);
double TA_from_EA(double EA,double ecc);
VectorIJK Obs_to_ECI(Observer& obs,double rGMST);
VectorIJK Obs_Position(Observer& obs,double time);
VectLook LookAngles(SATPOS SPos, Observer& obs, double time);
SATSUB SatSubPoint(SATPOS& SP, double rGMST);
SATSUB SatSubPointS(SATPOS& SP, double rGMST);

double Recover_Mean_Motion(SATELSET* pSatTLE);


ostream& operator<< (ostream& os, SATPOS& SP);
ostream& operator<< (ostream& os, VectorIJK& V);
ostream& operator<< (ostream& os, VectLook& Look);
ostream& operator<< (ostream& os, SATSUB& SP);
ostream& operator<< (ostream& os, ELLIPSE& EL );


ELLIPSE SatPos1(double dDeltaT, SATELSET* pSatTLE);

//=============================================================
//		Get the current local time from the PC system
//		This will be used as Time Now.
//		Epoch time is extracted from the 2 line elements
//		The difference will be the "time since" time for satellite
//		position calculations
//
//	the tm struct:
//
//	tm_sec: Seconds after minute (0-59)
//	tm_min: Minutes after hour (0-59)
//	tm_hour: Hours after midnight (0-23)
//	tm_mday: Day of month (1-31)
//	tm_mon: Month (0-11; January = 0)
//	tm_year: Year (current year minus 1900)
//	tm_wday: Day of week (0-6; Sunday = 0)
//	tm_yday: Day of year (0-365; January 1 = 0)
//	tm_isdst: Positive value if daylight saving time is in effect;
//		0 if daylight saving time is not in effect;
//		negative value if status of daylight saving time is unknown.
//		The C run-time library assumes the United States’s rules for implementing
//		the calculation of Daylight Saving Time (DST).
//============================================================

#define MST (-7)
#define UTC (0)
#define CCT (+8)




#endif
