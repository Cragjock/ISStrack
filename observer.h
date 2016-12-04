

#ifndef _OBSERVER_H
#define _OBSERVER_H

//#include <cmath>
//#include "constants.h"


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
//====================================================================
//
//	structure for any vector x-y-z
//
//====================================================================
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



//==============================================================================
//
//	Structure for capturing data from the
//	two line element input.
//
//==============================================================================

/*******************************************************************
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

****************************************************/



#endif
