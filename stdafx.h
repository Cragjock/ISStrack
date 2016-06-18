
#ifndef _STDAFX_H
#define _STDAFX_H


using namespace std;


#include <cstdio>

#ifdef __linux__
    //linux code goes here
#elif _WIN32
#include <conio.h>      // needed for kbhit function
#else

#endif

#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "constants.h"
#include "sattrack.h"
//#include "observer.h"
//#include "tle_read.h"
#include "time_it.h"





#endif
