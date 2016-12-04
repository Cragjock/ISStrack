
#ifndef _TLE_READ_H
#define _TLE_READ_H

using namespace std;
#include <cstring>
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "constants.h"

//=========================================
//	Structure for capturing data from the
//	two line element input.
//=========================================
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

#endif
