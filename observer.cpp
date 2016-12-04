

#include "stdafx.h"
//#include "observer.h"

//==========================================================================
//		Calculate Observer x,y,z in ECI.
//		takes the observers lat,long & alt in geodetic, converts to
//		geocentric and calculates x,y, z
//
//===========================================================================
VectorIJK Obs_to_ECI(Observer& obs,double rGMST)
{

	//Observer PHILLY={"Philly",Rad(40.0),Rad(-75.0),0,0}; //test data
	//obs=PHILLY;
    // test data
	// lat =40N long = 75W alt=10
	// thetaG = 144.627degrees(2.524)  Theta=69.627degrees
	// lamdaE= -75(longitude)
	// observer ECI should be x=1703.295, y=4586.650 z= 4077.984

	//rGMST= Rad(144.627);
	obs.theta=fmod((rGMST+obs.lon),TWO_PI);
	VectorIJK Vgeo;

	double flat2=(1-EARTH_FLAT)*(1-EARTH_FLAT);
	double tanphi=tan(obs.lat);
	double geophi=atan(flat2*tanphi);

	double costheta=cos(obs.theta);
	double sinetheta=sin(obs.theta);
	double coslat=cos(obs.lat);
	double sinlat=sin(obs.lat);

	double denom=1+EARTH_FLAT*(EARTH_FLAT-2)*sinlat*sinlat;
	double C=1/pow(denom,.5);

	double S=flat2*C;

	double x,y,z;

	//x=EARTHRADIUS*C*coslat*costheta;
	//y=EARTHRADIUS*C*coslat*sinetheta;
	//z=EARTHRADIUS*S*sinlat;

	x=((EARTHRADIUS*C)+obs.height)*coslat*costheta;
	y=((EARTHRADIUS*C)+obs.height)*coslat*sinetheta;
	z=((EARTHRADIUS*S)+obs.height)*sinlat;

	Vgeo.I=x;
	Vgeo.J=y;
	Vgeo.K=z;
	Vgeo.Mag=0;

	return Vgeo;
}
//==============================================================================
//	calculate observer position on circular earth
//	INPUT struct observer and GMST in radians
//	returns x,y,z in ECI
//
//==============================================================================
VectorIJK Obs_Position(Observer& obs,double rGMST)
{

	//Observer PHILLY={"Philly",Rad(40.0),Rad(-75.0),30,0}; //test data
	//obs=PHILLY;
    // test data
	// lat =40N long = 75W alt=10
	// thetaG = 144.627degrees(2.524)  Theta=69.627degrees
	// lamdaE= -75(longitude)
	// observer ECI should be x=1700.938, y=4580.302 z= 4099.786

	//rGMST= Rad(144.627);
	obs.theta=fmod((rGMST+obs.lon),TWO_PI);     // for test data. s/b 69.627(1.21522)
	VectorIJK Vobs;
	double rxy=(EARTHRADIUS+obs.height)*cos(obs.lat);
	Vobs.I=rxy*cos(obs.theta);
	Vobs.J=rxy*sin(obs.theta);
	Vobs.K=(EARTHRADIUS+obs.height)*sin(obs.lat);
	Vobs.Mag=0;
	//cout<<"Observer position from function\n"<<Vobs<<endl;

	return Vobs;
}
//===================================================================
//	calculate the look angles from the observer's topocentric coord
//	to the satellite position
//	Time input is GMST in radians
//
//===================================================================
VectLook LookAngles(SATPOS SPos, Observer& obs, double rGMST)
{
	double rx,ry,rz;
	double topos,topoe,topoz,rg2;
	double sinlat=sin(obs.lat);
	double coslat=cos(obs.lat);
	VectLook Look;
	obs.theta=fmod((rGMST+obs.lon),TWO_PI);
	double sintheta=sin(obs.theta);
	double costheta=cos(obs.theta);
	//VectorIJK obsIJK=Obs_Position(obs,rGMST);
	VectorIJK obsIJK=Obs_to_ECI(obs,rGMST);

//--- vector sum of satellite and observer in ECI
	rx=SPos.X-obsIJK.I;
	ry=SPos.Y-obsIJK.J;
	rz=SPos.Z-obsIJK.K;

//--- calculate position in topocentric space SEZ
	topos=sinlat*costheta*rx+sinlat*sintheta*ry-coslat*rz;
	topoe=-sintheta*rx+costheta*ry;
	topoz=coslat*costheta*rx+coslat*sintheta*ry+sinlat*rz;

//--- calulate the look angles
	Look.AZ=atan(-topoe/topos);
	if(topos > 0)
		Look.AZ+=PI;
	if(Look.AZ < 0)
		Look.AZ+=TWO_PI;

	rg2=Sqr(rx)+Sqr(ry)+Sqr(rz);
	Look.RG=pow(rg2,.5);
	Look.EL=asin(topoz/Look.RG);

	return Look;

}
//=======================================================================
//
//	operator overload to output the VectorIJK structure
//
//=======================================================================

ostream& operator<< ( ostream& os, VectorIJK& V)
{
	os	<<"Vec X"<<"\t\tVec Y"<<"\t\tVec Z\n"
		<<V.I<<"\t"<<V.J<<"\t"<<V.K<<endl;

	return os;
}
//=======================================================================
//
//	operator overload to output the Look angles structure
//
//=======================================================================
ostream& operator<< ( ostream& os, VectLook& Look)
{
	os	<<"Look AZ"<<"\t\tLook EL"<<"\t\tRange\n"
		<<Deg(Look.AZ)<<"\t"<<Deg(Look.EL)<<"\t"<<Look.RG<<endl;

	return os;
}
