
#include "stdafx.h"

//===========================================================================
//		SATPOS calculates the satellite position and velocity
//		vectors.
//		INPUTS are time since, element set, satpos structure
//		RETURNS the values of position into the satpos structure
//===========================================================================
SATPOS SatPos(double dDeltaT, SATELSET* pSatTLE)
{
	SATPOS SatPos;
	double A0R=0.0;      // recovered A0
	double SMA=0.0;
	double MM0R=0.0;                             // Mean motion recovered
	double DeltaT=dDeltaT;
	double TEMP1=0.0; double TEMP2=0.0; double TEMP3=0.0; double TEMPN=0.0;
	double TEMPD=0.0; double TEMPDEL=0.0; double A=0.0; double A0=0.0;
	double DEL1= 0.0; double DEL0=0.0; double A1=0.0; double A2=0.0;
	double p0=0; double p=0.0; double q0=0.0; double L0=0.0;
	double omegadot=0.0; double raandot=0.0; double RAANS0=0.0;
	double AOPS0=0.0; double LS=0.0; double O_Period=0.0;

//=== CONVERT ANGLES TO RADIANS =========================================
	double MA0=Rad(pSatTLE->dMA);                       // Mean Anomaly
	double AOP0=Rad(pSatTLE->dAOP);                     // Argument of perigee
	double RAAN0=Rad(pSatTLE->dRAAN);                   // right angle ascending node
	double IN0=Rad(pSatTLE->dIncl);                     // inclination
	double MM0=((pSatTLE->dMM*2*PI)/1440);				// mean motion --- rad per min
    double E0=pSatTLE->dEcc;                            // eccentricity
	double ECC=pSatTLE->dEcc;                           // eccentricity
	double cosIN=cos(Rad(pSatTLE->dIncl));
	double sinIN=sin(Rad(pSatTLE->dIncl));
	double cosAOP=cos(Rad(pSatTLE->dAOP));
	double sinAOP=sin(Rad(pSatTLE->dAOP));
	double cosRAAN=cos(Rad(pSatTLE->dRAAN));
	double sinRAAN=sin(Rad(pSatTLE->dRAAN));
	double esinAOP=pSatTLE->dEcc*sinAOP;
	double ecosAOP=pSatTLE->dEcc*cosAOP;
	double NDot2=(2*PI*pSatTLE->dDecayRate)/Sqr(1440.0);	// rad/min/min
	double NDot6=(2*PI*pSatTLE->dNddot6)/Cube(1440.0);	    // rad/min/min/min

/*============================================
    RECOVER original A0, SEMIMAJOR AXIS from TLE
    Equations are from SGP4 report 2 and related sources
    and page 10 from Hoots S6.1 paper == using this
==========================================   */
	O_Period=1440/pSatTLE->dMM;                 // minutes
	SMA=SMA_from_Period(O_Period);              // kilometers
	//cout<<"SMA from Period= "<<SMA<<endl;
	cout<<"Orbital Period="<<O_Period<<endl;

	double tpSMA1=sqrt(MUE)*60/MM0;
	A1=pow(tpSMA1,tothrd);
	//cout<<"A1 = "<<A1<<endl;

    tpSMA1=xke/MM0;
	A1=pow(tpSMA1,tothrd);
	cout<<"A1 revised = "<<A1*EARTHRADIUS<<endl;

    tpSMA1=sqrt(MUE)*60/MM0R;
	A0R=pow(tpSMA1,tothrd);                 // Recovered A0,semi-major axis
	cout<<"Line 72 A0R= "<<A0R<<endl;

/*******************************************************
        Recover original mean motion
*******************************************************/
	MM0R = Recover_Mean_Motion(pSatTLE);            // revolutions per day
	cout<<"TLE Mean Motion= "<<pSatTLE->dMM<<"\tRecovered MM= "<<MM0R<<endl;

	O_Period=1440/MM0R;                 // minutes
	SMA=SMA_from_Period(O_Period);              // kilometers
	//cout<<"Recovered SMA from Period= "<<SMA<<endl;
	//cout<<"Recovered Orbital Period="<<O_Period<<endl;

	MM0R=((MM0R*2*PI)/1440);	// radians per minute
    cout<<"MM0R from recover MM= "<<MM0R<<endl;
	//MM0R=((pSatTLE->dMM*2*PI)/1440);	// for test purpose
	//cout<<"MM0R from TLE MM= "<<MM0R<<endl;


/*******************************************************
        Recover original semi-major axis
*******************************************************/
    tpSMA1=xke/MM0R;
	double sdc_A1=pow(tpSMA1,tothrd);
	cout<<"A0R from sdc_A1= "<<sdc_A1*EARTHRADIUS<<endl;
	A0R=sdc_A1*EARTHRADIUS;


/********************************************
 UPDATE MEAN ANOMALY POSITION FOR TIME SINCE
***********************************************/
	double M=MM0R*(DeltaT)+MA0;		// time since equation
	if(M>TWO_PI)
		M=fmod(M,TWO_PI);

    cout<<"M from time since= "<<M<<endl;
    cout<<"MA0 at time since line= "<<MA0<<endl;


/**************************************************
//      SOLVE KEPLERS EQUATION
**************************************************/
	double EA=Kep_Newton(M,ECC);
	//double EB=kep_binary(M, ECC);
	//cout<<"EA="<<Deg(EA)<<endl;

	double TA=TA_from_EA(EA,ECC);
    //cout<<"TA="<<Deg(TA)<<endl;

/*******************************************
    R vector is A(cosE-e) in the P direction (XW)
	and a(1-e*e)^0.5*sinE in the Q direction (YW)
	RW is magnitude of R
******************************************/
	double XW=A0R*(cos(EA)-ECC);
	//cout<<"XW="<<XW<<endl;
	//cout<<"Acoswea-ecc "<<A0R<<endl;

	double YW=A0R*sin(EA)*pow((1-ECC*ECC),.5);
	//cout<<"YW="<<YW<<endl;

	double RW=pow(((XW*XW)+(YW*YW)),.5);
	//cout<<"RW="<<RW<<endl;

	double check_XW=RW*cos(TA);
	double check_YW=RW*sin(TA);
    //cout<<"check_XW= "<<check_XW<<endl;
	//cout<<"check_YW= "<<check_YW<<endl;

	double UX=atan2(YW,XW);
    //cout<<"UX="<<Deg(UX)<<endl;

	double U=fmod((TA+AOP0),TWO_PI);   //was double U=fmod((TA+AOPS0),TWO_PI);
	cout<<"U="<<Deg(U)<<endl;
    cout<<"TA="<<Deg(TA)<<endl;
    cout<<"AOP0="<<Deg(AOP0)<<endl;

//=== MAGNITUDE OF R VECTOR==============================================
	//double REA=SMA*(1-ECC*cos(EA));
	double REA=A0R*(1-ECC*cos(EA));
	//cout<<"REA="<<REA<<endl;

//=== I,J,K POSITION IN ECI FRAME OF SATELLITE POSITION ==================

	double Mx=-sinRAAN*cosIN;
	double My=cosRAAN*cosIN;
	double Ux=Mx*sin(U)+cosRAAN*cos(U);     // direction cosine, unit vector U
	double Uy=My*sin(U)+sinRAAN*cos(U);     // direction cosine, unit vector U
	double Uz=sin(U)*sinIN;                 // direction cosine, unit vector U
	double Vx=Mx*cos(U)-cosRAAN*sin(U);
	double Vy=My*cos(U)-sinRAAN*sin(U);
	double Vz=cos(U)*sinIN;
	double RX=REA*Ux;               // same as SatPos.X below
	double RY=REA*Uy;               // same as SatPos.Y below
	double RZ=REA*Uz;               // same as SatPos.Z below
	//cout<<"RX is "<<RX<<endl;
	//cout<<"Ry is "<<RY<<endl;
	//cout<<"Rz is "<<RZ<<endl;


// test data  =============================================

    double Px= cosRAAN*cosAOP - sinRAAN*cosIN*sinAOP;
    double Py= sinRAAN*cosAOP + cosRAAN*cosIN*sinAOP;
    double Pz= sinAOP*sinIN;

    double Qx= -sinAOP*cosRAAN - cosAOP*sinRAAN*cosIN;
    double Qy= -sinAOP*sinRAAN + cosAOP*cosRAAN*cosIN;
    double Qz= cosAOP*sinIN;

    double Wx= sinRAAN*sinIN;
    double Wy= -cosRAAN*sinIN;
    double Wz= cosIN;

    double P_length = Px*Px+Py*Py+Pz*Pz;
    double Q_length = Qx*Qx+Qy*Qy+Qz*Qz;
    double W_length = Wx*Wx+Wy*Wy+Wz*Wz;

    double ZW=0.0;

    double IJK_x= XW*Px+YW*Qx+ZW*Wx;
    double IJK_y= XW*Py+YW*Qy+ZW*Wy;
    double IJK_z= XW*Pz+YW*Qz+ZW*Wz;
    double IJK_r= pow(IJK_x*IJK_x+IJK_y*IJK_y+IJK_z*IJK_z,0.5);
    //cout<<"R from IJK set= "<<IJK_r<<endl;

	double U_length = Ux*Ux+Uy*Uy+Uz*Uz;        // should be 1
	double V_length = Vx*Vx+Vy*Vy+Vz*Vz;        // should be 1
	//double R_length = RX*RX+RY*RY+RZ*RZ;

/****************************************
    equations 5.34 - 5.36, page 32
    Orbital Motion document
*****************************************/
	SatPos.X=REA*(cosRAAN*cos(U)-sinRAAN*sin(U)*cosIN);
	SatPos.Y=REA*(sinRAAN*cos(U)+cosRAAN*sin(U)*cosIN);
	SatPos.Z=REA*sin(U)*sinIN;
	//cout<<"X="<<SatPos.X<<"\tY="<<SatPos.Y<<"\tZ="<<SatPos.Z<<endl;

	double tMAG=(SatPos.X*SatPos.X)+(SatPos.Y*SatPos.Y)+(SatPos.Z*SatPos.Z);
	//double tMAG=Sqr(pSatPos->X)+Sqr(pSatPos->Y)+Sqr(pSatPos->Z);
	tMAG=sqrt(tMAG);
	cout<<"R Mag="<<tMAG<<endl;

	cout<<"Altitude: "<<REA-EARTHRADIUS<<endl;
	cout<<"Ecc Anomaly: "<<Deg(EA)<<endl;
	cout<<"True Anomaly: "<<Deg(TA)<<endl;
	cout<<"Mean Anomaly: "<<Deg(M)<<endl;
	cout<<"MA from 2 line file: "<<pSatTLE->dMA<<endl;


	return SatPos;
}

// Lets cheat and use satpos framework to calculate ellipse data
//typedef struct tag_ellipse
//	{
//		double a;				// semi-major axis
//		double b;				// semi-minor axis
//		double ecc;				// eccentricity
//		double p;				// semi-latus a(1-e^2)
//		double rp;				// perigee distance a(1-ecc)
//		double hp;				// height at perigee rp-R
//		double ra;				// apogee distance a(1+ecc)
//		double ha;				// height at apogee ra-R
//	} ELLIPSE;
//      1 Kilometer = 3280.8399 Feet
//      EARTHRADIUS 6378.137
//
ELLIPSE SatPos1(double dDeltaT, SATELSET* pSatTLE)
{
    ELLIPSE myEllipse;
    double DeltaT=dDeltaT;
    double MA0=Rad(pSatTLE->dMA);
	double AOP0=Rad(pSatTLE->dAOP);
	double RAAN0=Rad(pSatTLE->dRAAN);
	double IN0=Rad(pSatTLE->dIncl);
	//double MM0=((pSatTLE->dMM*2*PI)/1440);				//--- rad per min
	double cosIN=cos(Rad(pSatTLE->dIncl));
	double sinIN=sin(Rad(pSatTLE->dIncl));
	double cosAOP=cos(Rad(pSatTLE->dAOP));
	double sinAOP=sin(Rad(pSatTLE->dAOP));
	double cosRAAN=cos(Rad(pSatTLE->dRAAN));
	double sinRAAN=sin(Rad(pSatTLE->dRAAN));
	double ECC=pSatTLE->dEcc;

	myEllipse.ecc=ECC;
	double TP=period_from_MM(pSatTLE->dMM);     //period in minutes per revolution


    myEllipse.a=SMA_from_Period(1440/pSatTLE->dMM);
    myEllipse.p=myEllipse.a*(1-myEllipse.ecc*myEllipse.ecc);
    double testRp= myEllipse.a*(1-myEllipse.ecc);
    myEllipse.rp=testRp;

    cout<<"height about earth (km)= "<<myEllipse.a-EARTHRADIUS<<endl;
    cout<<"R magnitude: "<<Rmag_from_Mean_Anamoly(myEllipse.p, myEllipse.ecc,MA0)<<endl;

    //cout<<myEllipse<<endl;
    return myEllipse;
}

//=================================================================================
//  THE FOLLOWING ARE BASIC FUNCTIONS FOR ORBITS
//	-- period_from_MM(double mm)				// period from mean motion
//	-- SMA_from_Period(double period)			// Semi-major axis (SMA) from period
//	-- Vmag_from_R_SMA(double r,double a)		// velocity magnitude from r and SMA
//	-- Semi_Latum(double a, double e)			// calculate P, semi latum of ellipse
//	-- Rmag_from_Mean_Anomaly(double p, double e, double ma)
//

//=================================================================================
//
//	Period from mean motion.
//	Inputs mean motion units are revolutions per day.
//	Period=1440 minutes per day/mean motion.
//	Returns period in minutes.
//
//==================================================================================
double period_from_MM(double mm)		// period from mean motion
{
	return MIN_PER_DAY/mm;				// 1440 minutes per day/mean motion
}
//===========================================================================
//	Semi-major axis (SMA) from period.
//	Inputs the period in minutes.
//	SMA=331.25*(Period)^.6666667
//	Returns SMA in kilometers
//============================================================================
double SMA_from_Period(double period)	// Semi-major axis (SMA) from period
{
	double temp=(MUE*3600)/(TWO_PI*TWO_PI);	// MU/(2*PI)^2, 3600 convert MU to km^3/min^2
	double temp1=pow(temp,onethrd);
	//cout<<temp1<<endl;
	double temp2=temp*period*period;
	double temp3=pow(temp2,onethrd);

	return temp3;                       // SMA in kilometers
}
//=============================================================================
//	Determine magnitude of velocity vector from r and SMA.
//	r is the satellite geo-center distance
//	r in meters and v in meters/second
//============================================================================
double Vmag_from_R_SMA(double r,double a)			// velocity magnitude from r
{
	double temp=((2/r)-(1/a))*MUE;//*1000*1000*1000;
	double v=sqrt(temp);

	return v;
}
//=======================================================================
//	Determine semi latus rectum of ellipse P.
//	Input is SMA and eccentricity.
//	P=a*(1-ecc^2)
//========================================================================
double Semi_Latum(double a, double e)				// calulate P, semi latum of ellipse
{
	return a*(1-(e*e));
}
//===========================================================================
//	Determine magnitude of the r vector.
//	where r=p/(1+e*cosV)
//	V is the true anomaly of the satellites position from perigee
//	needs eccentricity, latum P and mean anomaly in radians
//	returns r in meters
//===========================================================================
double Rmag_from_Mean_Anamoly(double p, double e, double ma)
{
	double temp=1+e*cos(ma);
	double temp1=p/temp;
	return temp1;
}
//==========================================================================
//
//	r(vector)=r*cosV P(vector) + r*sinV Q(vector)
//
//==========================================================================
double Rad_componentPQ(double r, double V, VectorPQ &PQ)
{
	double VS=sin(V);
	double VC=cos(V);
	PQ.P=r*VC;
	PQ.Q=r*VS;

	return sqrt(PQ.P*PQ.P+PQ.Q*PQ.Q);
}
//=======================================================================
//
//	v(vector)=( (mu/p)^.5)*[ -sinV P(vector)+(e+cosV)Q(vector)]
//
//=======================================================================
double Vel_componentPQ(double p, double ecc, double V, VectorPQ &PQ)
{
	double tempmu=MUE;	// *1000*1000*1000;  // adjust from km^3 to meters^3
	double coeff=sqrt(tempmu/p);
	double VS=sin(V);
	double VC=cos(V);
	double temp=ecc+VC;

	PQ.P=coeff*(-1)*VS;
	PQ.Q=coeff*temp;

	return sqrt(PQ.P*PQ.P+PQ.Q*PQ.Q);

}
//===============================================================================
//		Solve KEPLER'S EQUATION using Newton iteration (pg 221 of Fund of Astro.)
//		Verified with data from Meeus
//		Inputs are eccentricity and initial mean anomaly(radians).
//		returns eccentricity anomaly in radians.
//		Time since must be considered prior to entry !
//================================================================================
double Kep_Newton(double M0, double ecc)
{
	double denom=1;
	int loop=0;
	double deltaM=1;
	double Enplus1=0;
	double temp=0;
	double En=M0;						//--- initial value
	double Mn=En-ecc*sin(En);			//--- initial value
	//deltaM=M0-Mn;						//--- initial value

	do
	{
		deltaM=M0-Mn;
		denom=1-ecc*cos(En);			//--- denominator
		Enplus1=En+(deltaM/denom);		//--- evaluate for E

		Mn=Enplus1-ecc*sin(Enplus1);	//--- new M based on new E

		En=Enplus1;						//--- new E for next iteration
		temp=Deg(En);
		loop++;
		//deltaM=M0-Mn;
	}
	while(fabs(deltaM) > .000001);		//--- fabs-float abs value
	//cout<<"loop count="<<loop<<endl;
	//cout<<"Ecc anomaly="<<temp<<endl;

	return En;
}
//======================================================================
//	True anomaly from Eccentricity anomaly
//	inputs EA and eccentricity
//	returns true anomaly
//======================================================================
double kep_binary(double M0, double ecc)
{
	double F,M,M1,E0,D;
	M=M0;
	F=Sgn(M);
	M=fabs(M)/TWO_PI;
	M=(M-int(M))*TWO_PI*F;
	if(M<0)
		M=M+TWO_PI;
	F=1;
	if(M>PI)
		F=-1;
	if(M>PI)
		M=TWO_PI-M;
	E0=PI/2;
	D=PI/4;
	for(int j=1; j<110; j++)
	{
		M1=E0-ecc*sin(E0);
		E0=E0+D*Sgn(M-M1);
		D=D/2;
	}
	E0=E0*F;


	return E0;
}
//======================================================================
//	True anomaly from Eccentricity anomaly
//	inputs EA and eccentricity
//	returns true anomaly
//======================================================================
double TA_from_EA(double EA,double ecc)
{
    // from Vallardo page 48 =====
    //EA=Rad(247.5);
    //ecc=.20;

    // cosine version to obtain TA from EA
	double denom=1-(ecc*cos(EA));
	double numerator = cos(EA)-ecc;
	double cosNU=numerator/denom;
	double TOnu= acos(cosNU);
	//cout<<" TA in degrees from cos "<<Deg(TOnu)<<endl;

    // arctangent version to obtain TA from EA
	double tanEn=tan(EA/2);
	double ee=(1+ecc)/(1-ecc);
	double powee=sqrt(ee);
	double tp=tanEn*powee;
	double nu=2*atan(tp);       // the true anomaly value
	//cout<<" TA in degrees "<<Deg(nu)<<endl;
	if(nu<0 && nu<PI)           // need the quadrant check for correct TA
         nu+=TWO_PI;
		//nu+=PI;
    //else
        //nu+=TWO_PI;
    //cout<<" TA in degrees "<<Deg(nu)<<endl;
	return nu;
	//return TOnu;

}
//=======================================================================
//
//	Calculate the satellite latitude, longitude and height
//	on the earth sub-point
//
//=======================================================================
SATSUB SatSubPoint(SATPOS& SP, double rGMST)
{
	double C=0.0;
	double del1=0.0;
	double phinow=0.0;
	//double angle=0.0;
	double e2=2*EARTH_FLAT-Sqr(EARTH_FLAT);
	double R=pow((Sqr(SP.X)+Sqr(SP.Y)),0.5);
	SATSUB Subpoint;


//	Calculate on circular earth first

    double RC=pow((SP.X*SP.X+SP.Y*SP.Y),0.5);
    double RD=SP.Z/RC;
    double C_Lat=atan(RD);
    //cout<<"Latitude from circular earth "<<Deg(C_Lat)<<endl;
    double C_Long=atan2(SP.Y,SP.X)-rGMST;
    //cout<<"Longitude from circular earth "<<360+Deg(C_Long)<<endl;


	if(SP.X==0)
	{
		if(SP.Y>0)
			Subpoint.lon=PI/2;
	}
	else if(SP.X>0)
			Subpoint.lon=atan(SP.Y/SP.X);
		else
			Subpoint.lon = PI + atan(SP.Y/SP.X);

	Subpoint.lon=fmod(Subpoint.lon-rGMST,TWO_PI);
	if(Subpoint.lon > PI)
		Subpoint.lon-=TWO_PI;
	if(Subpoint.lon < -PI)
		Subpoint.lon+=TWO_PI;

	double lat=atan2(SP.Z,R);
	do
	{
		phinow=lat;
		C=1/pow((1-e2*sin(phinow)*sin(phinow)),0.5);
		lat=atan2((SP.Z+EARTHRADIUS*C*e2*sin(phinow)),R);
		del1=lat-phinow;
	}
	while(fabs(del1)>.000001);
	Subpoint.lat=phinow;
	Subpoint.range=(R/cos(phinow))-C*EARTHRADIUS;
	//cout<<"Latitude Geodetic= "<<Deg(Subpoint.lat)<<endl;
	//cout<<"Longitude Geodetic= "<<Deg(Subpoint.lon)<<endl;

	return Subpoint;
}
//=======================================================================
//	Calculate the satellite lat, lon and height
//	on the earth sub-point
//  for spherical earth
//=======================================================================
SATSUB SatSubPointS(SATPOS& SP, double rGMST)
{
	double C=0.0,R1;
	double del1=0.0;
	double phinow=0.0;
	//double angle=0.0;
	double e2=2*EARTH_FLAT-Sqr(EARTH_FLAT);
	double R=pow((Sqr(SP.X)+Sqr(SP.Y)),0.5);
	SATSUB Subpoint;

	if(SP.X==0)
	{
		if(SP.Y>0)
			Subpoint.lon=PI/2;
	}
	else if(SP.X>0)
			Subpoint.lon=atan(SP.Y/SP.X);
		else
			Subpoint.lon = PI + atan(SP.Y/SP.X);

//================================
//Latitude sub-point
    phinow=SP.Z/(sqrt(SP.X*SP.X+SP.Y*SP.Y));
    Subpoint.lat=atan(phinow);

// ===================================
// Satellite height
    R1=SP.X*SP.X+SP.Y*SP.Y+SP.Z*SP.Z;    //x^2+y^2+z^2
    R=sqrt(R1)-EARTHRADIUS;              //sqrt x^2+y^2+z^2
    Subpoint.range=R;

//================================
//Longitude sub-point
    del1=atan(SP.Y/SP.X)-rGMST;
    Subpoint.lon=del1;

	//Subpoint.lon=fmod(Subpoint.lon-rGMST,TWO_PI);
	if(Subpoint.lon > PI)
		Subpoint.lon-=TWO_PI;
	if(Subpoint.lon < -PI)
		Subpoint.lon+=TWO_PI;

	return Subpoint;
}

double Recover_Mean_Motion(SATELSET* pSatTLE)
{
	double MM=((pSatTLE->dMM)*TWO_PI)/(MIN_PER_DAY);    //rev/day to rad/min

	double a1=pow(xke/MM,tothrd);
	//double a1=pow(xke/pSatTLE->dMM,tothrd);
	double theta2=cos(pSatTLE->dIncl)*cos(pSatTLE->dIncl);
	double eosq=(pSatTLE->dEcc)*(pSatTLE->dEcc);
	double x3thm1=3.0-theta2-1;
	double beta02=1.0-eosq;
	double beta0=pow(beta02,0.5);
	double del1=(1.5*ck2*x3thm1)/(a1*a1*beta0*beta02);
	double a2= a1*(1.0-(1.0/3.0)*del1-(del1*del1)-(134.0/81.0)*del1*del1*del1);
    //double ao = a1*(1.0 - del1*(0.5*tothrd + del1*(1.0 + (134.0/81.0)*del1)));
	double del0=(1.5*ck2*x3thm1)/(a2*a2*beta0*beta02);
	double temp2=(MM)/(1+del0);                       // rev per day
	//double temp2=(pSatTLE->dMM)/(1+del0);                       // rev per day
	//temp2=(temp2*(TWO_PI))/1440.0;                              // radians per minute
	double temp3=pSatTLE->dMM;
	temp2=(temp2*MIN_PER_DAY)/TWO_PI;
    return temp2;                                               // rev per day
}

//=======================================================================
//	operator overload to output the SATPOS structure
//=======================================================================
ostream& operator<< ( ostream& os, SATPOS& SP )
{
	os<<"X"<<"\t\tY"<<"\t\tZ\n"
		<<SP.X<<"\t"<<SP.Y<<"\t"<<SP.Z<<endl;

	return os;
}
//=======================================================================
//	operator overload to output the satellite sub-point structure
//	range is height above earth
//=======================================================================
ostream& operator<< (ostream& os, SATSUB& SP )
{
	os<<"Long"<<"\t\tLat"<<"\t\tHeight\n"
		<<Deg(SP.lon)<<"\t"<<Deg(SP.lat)<<"\t"<<SP.range<<endl;

	return os;
}

ostream& operator<< (ostream& os, ELLIPSE& EL )
{

    os<<"Orbital Ellipse data:\n"
        <<"A: "<<EL.a<<"\n"
        <<"B: "<<EL.b<<"\n"
        <<"Ecc: "<<EL.ecc<<"\n"
        <<"P: "<<EL.p
        <<endl;

	return os;
}

SATPOS clean_SatPos(double dDeltaT, SATELSET* pSatTLE)
{
	SATPOS SatPos;
	double A0R=0.0;                         // recovered A0
	double SMA=0.0;
	double MM0R=0.0;                        // Mean motion recovered
	double DeltaT=dDeltaT;                  // passed in as minutes
	double MA0=pSatTLE->dMA;                // Mean Anomaly in degrees
	double AOP0=pSatTLE->dAOP;              // Argument of perigee in degrees
	double RAAN0=pSatTLE->dRAAN;            // right angle ascending node in degrees
	double IN0=pSatTLE->dIncl;              // inclination in degrees
	double MM0=pSatTLE->dMM;                // mean motion in revolutions per day
	double ECC=pSatTLE->dEcc;               // eccentricity

	double cosIN=cos(Rad(pSatTLE->dIncl));
	double sinIN=sin(Rad(pSatTLE->dIncl));
	double cosAOP=cos(Rad(pSatTLE->dAOP));
	double sinAOP=sin(Rad(pSatTLE->dAOP));
	double cosRAAN=cos(Rad(pSatTLE->dRAAN));
	double sinRAAN=sin(Rad(pSatTLE->dRAAN));

	double O_Period=1440/MM0;                                   // minutes
	SMA=SMA_from_Period(O_Period);                              // kilometers

    cout<<endl;

	cout<<"SMA from Period= "<<SMA<<" kilometers "<<endl;
	cout<<"Orbital Period="<<O_Period<<" minutes "<<endl;    // minutes
	cout<<"Mean Motion: "<<MM0<<endl;

	MM0R = Recover_Mean_Motion(pSatTLE);            // revolutions per day

	cout<<"Mean Motion Rec: "<<MM0R<<endl;
	cout<<"Period from MM0R "<<(1440/MM0R)<<" minutes"<<endl;


	double MM0R_1 = (MM0R*TWO_PI)/(60.0*1440.0);    // Mean motion in radians per sec
	double a1=pow(MUE,0.5);
	a1=pow(a1/MM0R_1,tothrd);        //MM0R should be radians per second
    SMA=SMA_from_Period((1440/MM0R));                       // kilometers

	cout<<"SMA_rec from Period= "<<SMA<<" kilometers "<<endl;
	//cout<<"SMA_rec from equation= "<<a1<<" kilometers "<<endl;


/********************************************
 UPDATE MEAN ANOMALY POSITION FOR TIME SINCE
***********************************************/
	// DeltaT= 1.0;enable to force the time delta ~~~~~~~~~~~~~~~~~~~

    MM0=MM0*(TWO_PI/1440.0);                             // rev/day to rad/min

/******************************************/


    //DeltaT=fmod(DeltaT,O_Period);
    //cout<<"DeltaT "<<DeltaT<<endl;
	double M=MM0*(DeltaT)+Rad(MA0);                 // time since equation


	if(M>TWO_PI)
		M=fmod(M,TWO_PI);

    //cout<<"M (in radians) from time since= "<<M<<endl;
    //cout<<"M=  "<<Deg(M)<<endl;


    MM0R=MM0R*(TWO_PI/1440.0);                             // rev/day to rad/min
    //M=MM0R*(DeltaT)+Rad(MA0);                 // time since equation


	if(M>TWO_PI)
		M=fmod(M,TWO_PI);
    cout<<"DeltaT "<<DeltaT<<endl;

    //DeltaT=fmod(DeltaT,O_Period);
    //cout<<"DeltaT "<<DeltaT<<endl;


    //cout<<"Mrec (in radians) from time since= "<<M<<endl;
    cout<<"Mrec=  "<<Deg(M)<<endl;


/***********************************
using MM0R and  SMA from recovered data
*******************************/

/**************************************************
//      SOLVE KEPLERS EQUATION
**************************************************/
	double EA=Kep_Newton(M,ECC);        // returns radians
	//double EB=kep_binary(M, ECC);
	cout<<"EA= "<<Deg(EA)<<endl;        // returns radians
	//cout<<"EB= "<<Deg(EB)<<endl;

    double TA=TA_from_EA(EA,ECC);
    cout<<"TA= "<<Deg(TA)<<endl;

/*******************************************
    R vector is A(cosE-e) in the P direction (XW)
	and a(1-e*e)^0.5*sinE in the Q direction (YW)
	RW is magnitude of R
******************************************/
	double XW=SMA*(cos(EA)-ECC);
	//cout<<"+++++ XW="<<XW<<endl;
	//cout<<"+++++ Acoswea-ecc "<<SMA<<endl;

	double YW=SMA*sin(EA)*pow((1-ECC*ECC),.5);
	//cout<<"+++++ YW="<<YW<<endl;

	double RW=pow(((XW*XW)+(YW*YW)),.5);
	//cout<<"+++++ RW="<<RW<<endl;

	double check_XW=RW*cos(TA);
	double check_YW=RW*sin(TA);
    //cout<<"check_XW= "<<check_XW<<endl;
	//cout<<"check_YW= "<<check_YW<<endl;

	double UX=atan2(YW,XW);
    //cout<<"UX="<<Deg(UX)<<endl;

	double U=fmod((TA+Rad(AOP0)),TWO_PI);   //was double U=fmod((TA+AOPS0),TWO_PI);
	cout<<"U="<<Deg(U)<<endl;
    //cout<<"TA="<<Deg(TA)<<endl;
    //cout<<"AOP0="<<AOP0<<endl;

//=== MAGNITUDE OF R VECTOR==============================================
	//double REA=SMA*(1-ECC*cos(EA));
	double REA=SMA*(1-ECC*cos(EA));
	//cout<<"+++++ REA="<<REA<<endl;

//=== I,J,K POSITION IN ECI FRAME OF SATELLITE POSITION ==================

	double Mx=-sinRAAN*cosIN;
	double My=cosRAAN*cosIN;
	double Ux=Mx*sin(U)+cosRAAN*cos(U);     // direction cosine, unit vector U
	double Uy=My*sin(U)+sinRAAN*cos(U);     // direction cosine, unit vector U
	double Uz=sin(U)*sinIN;                 // direction cosine, unit vector U
	double Vx=Mx*cos(U)-cosRAAN*sin(U);
	double Vy=My*cos(U)-sinRAAN*sin(U);
	double Vz=cos(U)*sinIN;
	double RX=REA*Ux;               // same as SatPos.X below
	double RY=REA*Uy;               // same as SatPos.Y below
	double RZ=REA*Uz;               // same as SatPos.Z below
	//cout<<"RX is "<<RX<<endl;
	//cout<<"Ry is "<<RY<<endl;
	//cout<<"Rz is "<<RZ<<endl;


// test data  =============================================

/*****************************************************************
Direction cosines of P,Q and W in terms of I,J and K and TLE data.
Escobal page 77
******************************************************************/

    double Px= cosRAAN*cosAOP - sinRAAN*cosIN*sinAOP;
    double Py= sinRAAN*cosAOP + cosRAAN*cosIN*sinAOP;
    double Pz= sinAOP*sinIN;

    double Qx= -sinAOP*cosRAAN - cosAOP*sinRAAN*cosIN;
    double Qy= -sinAOP*sinRAAN + cosAOP*cosRAAN*cosIN;
    double Qz= cosAOP*sinIN;

    double Wx= sinRAAN*sinIN;
    double Wy= -cosRAAN*sinIN;
    double Wz= cosIN;

    double P_length = Px*Px+Py*Py+Pz*Pz;
    double Q_length = Qx*Qx+Qy*Qy+Qz*Qz;
    double W_length = Wx*Wx+Wy*Wy+Wz*Wz;

    double ZW=0.0;

/*****************************************************************
Mapping from r in the P, Q, W frame to r in the I, J, K frame.
Escobal page 82
******************************************************************/
    double IJK_x= XW*Px+YW*Qx+ZW*Wx;
    double IJK_y= XW*Py+YW*Qy+ZW*Wy;
    double IJK_z= XW*Pz+YW*Qz+ZW*Wz;
    double IJK_r= pow(IJK_x*IJK_x+IJK_y*IJK_y+IJK_z*IJK_z,0.5);
    cout<<"+=+=+ R from IJK set= "<<IJK_r<<endl;

	double U_length = Ux*Ux+Uy*Uy+Uz*Uz;        // should be 1
	double V_length = Vx*Vx+Vy*Vy+Vz*Vz;        // should be 1
	//double R_length = RX*RX+RY*RY+RZ*RZ;

/****************************************
    equations 5.34 - 5.36, page 32
    Orbital Motion document
*****************************************/
	SatPos.X=REA*(cosRAAN*cos(U)-sinRAAN*sin(U)*cosIN);
	SatPos.Y=REA*(sinRAAN*cos(U)+cosRAAN*sin(U)*cosIN);
	SatPos.Z=REA*sin(U)*sinIN;
	//cout<<"X="<<SatPos.X<<"\tY="<<SatPos.Y<<"\tZ="<<SatPos.Z<<endl;
	//cout<<"X="<<IJK_x<<"\tY="<<IJK_y<<"\tZ="<<IJK_z<<endl;

	//cout<<"cosRAAN="<<cosRAAN<<"\tsinRAAN= "<<sinRAAN<<endl;
	//cout<<"cosU="<<cos(U)<<"\tsinU= "<<sin(U)<<endl;
    //cout<<"cosIN="<<cosIN<<"\tsinIN= "<<sinIN<<endl;

	double tMAG=(SatPos.X*SatPos.X)+(SatPos.Y*SatPos.Y)+(SatPos.Z*SatPos.Z);
	//double tMAG=Sqr(pSatPos->X)+Sqr(pSatPos->Y)+Sqr(pSatPos->Z);
	tMAG=sqrt(tMAG);
	return SatPos;
}


