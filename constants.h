

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

//==========================================================================
//
//	Constant definitions, macros and other data.
//
//===========================================================================
#define DEG_TO_RAD 0.0174532925
#define RAD_TO_DEG (1.0/DEG_TO_RAD)
#define PI 3.14159265359
#define TWO_PI (2.0 * PI)
#define MIN_PER_DAY 1440.0
#define SEC_PER_DAY 86400.0
#define EARTHRADIUS 6378.137			// in km, Astro Alm pg K13
#define OMEGA_E 1.00273790934			// Earth rotations per sidereal day (non-constant)
#define OMEGA_ER (OMEGA_E * TWO_PI)		// Earth rotation, radians per sidereal day
#define OMEAGA_EARTH TWO_PI/SEC_PER_DAY // Earth rotation rate, 7.29211510 × 10-5 radians/second
#define EARTH_FLAT (1.0/298.257)		// Earth flattening WGS '84, Astro Alm pg K13
#define MUE 398601						// MU earth=3.986012 exp+5 km^3/s^2
#define onethrd (1.0/3.0)
#define half (1/2)
#define J2RE2 44041.49451				// coef for J2 RE^2
#define CO1 (134.0/81.0)				// coeff for DEL1 134/81

//--------------------- from kelso sgp4 init_f ------------------------
#define ae 1.0
#define tothrd (2.0/3.0)
#define xkmper 6378.135					// Earth equatorial radius - kilometers (WGS '72)
#define f (1/298.26)					// Earth flattening (WGS '72)}
#define ge 398600.8						// Earth gravitational constant (WGS '72)
#define J2 1.0826158E-3					// J2 harmonic (WGS '72)
#define J3 -2.53881E-6					// J3 harmonic (WGS '72)
#define J4 -1.65597E-6					// J4 harmonic (WGS '72)
#define ck2 J2/2
#define ck4 -3*J4/8
#define xj3 J3
#define qo (ae+120)/xkmper
#define s (ae+78)/xkmper
#define e6a 1E-6
#define ke 0.07436574  // earth radii^3/2 / minutes
#define xke sqrt((3600.0*ge)/(xkmper*xkmper*xkmper))  //sqrt(ge) ER^3/min^2

#define dpinit 1						// Deep-space initialization code
#define dpsec 2							// Deep-space secular code
#define dpper 3							// Deep-space periodic code

//------------------ items for reading 2 line data -----------
#define NULLLINE 0
#define LINEONE 1
#define LINETWO 2
#define NAMELINE 3
#define MAXNAME 32

//------------------- Items for perturbed orbit calculations ------
#define SMA_DRAG 1
#define SMA_SECULAR 2
#define ECC_SECULAR 3
#define IN_SECULAR 4
#define RAAN_SECULAR 5
#define AOP_SECULAR 6
#define M_SECULAR 7

// ---------- some math macros -------------------------
#define Sqr(x) ((x)*(x))
#define Rad(x) ((x)*DEG_TO_RAD)
#define Deg(x) ((x)*RAD_TO_DEG)
#define Cube(x) ((x)*(x)*(x))
#define Sgn(x) ((x)>=0 ? 1:-1)

#define MST (-7)
#define UTC (0)
#define CCT (+8)

#endif
