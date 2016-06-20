

#include "stdafx.h"
#include <pthread.h>
#include <unistd.h>
#include "lcd.h"
#include "pitime.h"

//=============================================================
//		MAIN
//=============================================================


// setup for threading access
VectLook AntTracker;
VectLook testlook;
void get_NIST();
// NIST
char NL = '\n';
char EL = '\0';
int k=0;
char str_filenm[33];

int (*device_open)(void);
//char* (*alt_pitime)();
int (*alt_pitime)(char*);

char buf[80];
char upper[16] = {"Az: "};
char lower[16] = {"El: "};

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

/* Loop 'arg' times incrementing 'glob' */
static void *threadFunc(void *arg)
{
    char* sdcmsg = (char*)arg;
    printf("%s", sdcmsg);

    int sdc;
    char buf[80];
    int j;
    int loops=1;

    sdc = pthread_mutex_lock(&mtx);
    if (sdc != 0)
       printf("error pthread_mutex_lock");

    AntTracker.AZ = Deg(testlook.AZ);        // in radians
    AntTracker.EL = Deg(testlook.EL);

    //sleep(1);
    for (j = 0; j < loops; j++)
        AntTracker.AZ++;

    sdc = pthread_mutex_unlock(&mtx);
    if (sdc != 0)
        printf("error pthread_mutex_unlock");

    printf("@@@@@@@AZ: %f\n",AntTracker.AZ);
    printf("@@@@@@@EL: %f\n",AntTracker.EL);
    pthread_t whoami=pthread_self();
    printf("who am I %i: \n",whoami);
    //sleep(1);
    return NULL;
}

char m1[]="Hello 1\n";
char m2[]="Hello 2\n";



int main(int argc, char* argv[])
{

    lcd_open();
	lcd_write("Hello from Steve's\nLCD stuff");
	lcd_clear();
	get_NIST();



	cout.setf(ios::fixed);
	//=== SET CURRENT TIME ==========================
	struct tm *newtime;								//--- for time now
	time_t long_time;								//--- Get time as long integer.
	double DeltaT=0.0;								//--- time since is in minutes

	Observer PLACENTIA={"Yorba Linda",Rad(33.909),Rad(-117.782),30.0,0};
	//Observer PHILLY={"Philly",Rad(40.0),Rad(-75.0),0.0,0};
	double sdctime;
	SATELSET Eset;
	SATPOS satpos;
	ELLIPSE myEllipse;
	//double SP,JDG,E2JD,JDN;
	double JDG,E2JD,JDN;
	VectorIJK test,test1;           //ptest;
	//VectLook testlook;
	SATSUB SB;

	clock_t goal;
	clock_t wait=(clock_t)2 * CLOCKS_PER_SEC;       // change the 2 for update rate, 2= about 2 seconds

	Read_TLE(argv[1], Eset);                        // read the 2 line data

	do
	{
		time( &long_time );
		newtime=gmtime( &long_time );                   // time, expressed as a UTC time, GMT timezone
		JDN=JD_Now(newtime);							//--- JD based on system clock as GMT
		JDG=ThetaG_JD(JDN);								//--- in radians
		E2JD=Epoch2JD(Eset.iEpochYear,Eset.dEpochDay);	//--- JD based on TLE epoch
		double local_time=0.0;
		double test_time=0.0;

        local_time=newtime->tm_yday+1+(newtime->tm_hour+(newtime->tm_min+newtime->tm_sec/60.0)/60.0)/24.0;
        test_time=local_time-Eset.dEpochDay;
        //cout<<"test_time delta days "<<test_time<<endl;
        test_time*=1440.0;
        //cout<<"test_time delta minutes "<<test_time<<endl;




/**************************************
 local_time minus Eset.dEpochDay matches JDN-E2JD.
 And is easier to check and calculate and no need for
 all the JD and JD0 code.
*************************************/
		sdctime=JDN-E2JD;								//--- delta days
		sdctime*=1440.0;                                // delta minutes
		//sdctime=fmod(sdctime,60);
        //cout<<"Current sdctime "<<sdctime<<endl;

		DeltaT=sdctime;

        //satpos=SatPos(DeltaT, &Eset);				    //--- get satellite position
		satpos=clean_SatPos(DeltaT, &Eset);

		cout<<"=====Satellite ECI position============================\n"<<satpos;

		test=Obs_Position(PLACENTIA,JDG);				//--- get observer position
		//test1=Obs_to_ECI(PHILLY,JDG);                 //-- test data from TS Kelso
		test1=Obs_to_ECI(PLACENTIA,JDG);
		testlook=LookAngles(satpos, PLACENTIA,JDG);		//--- get look angles
		SB= SatSubPoint(satpos,JDG);

		cout<<"=====Observer ECI====================\n"<<test1;
		cout<<"=====Observer Look angles============\n"<<testlook; // for antenna tracker
		cout<<"=====Sat Sub Point===================\n"<<SB;

        lcd_home();
        if(testlook.EL > 0)
            lcd_write("Look angles:visible\n");
        else
            lcd_write("Look angles:below  \n");

		sprintf(buf, "AZ:%6.2f EL:%6.2f", Deg(testlook.AZ), Deg(testlook.EL));
        lcd_write(buf);

        lcd_set_cursor_address(0x14);
        lcd_write("Sat Lat/long\n");

        sprintf(buf, "LT:%6.2f LG:%6.2f", Deg(SB.lat), Deg(SB.lon));
        lcd_write(buf);


/**********
        pthread_t t1, t2;
        int loops, sdc;
        sdc = pthread_create(&t1, NULL, threadFunc, (void*)m1);
        if (sdc != 0)
            printf("erro pthread_create");
        sdc = pthread_create(&t2, NULL, threadFunc, (void*)m2);
        if (sdc != 0)
            printf("error pthread_create");
        sdc = pthread_join(t1, NULL);
        if (sdc != 0)
            printf("error pthread_join");
        sdc = pthread_join(t2, NULL);
        if (sdc != 0)
            printf("error pthread_join");
*************/


		//myEllipse=SatPos1(DeltaT, &Eset);

		goal = wait + clock();
		while( goal > clock() );

	}



	#ifdef __linux__
    	while(1);
    #elif _WIN32
        while(!(_kbhit()));
    #else
    #endif


	//while(1);
    //while(!(_kbhit()));

		//pthread_exit(NULL);
		lcd_close();

	return 0;
}

void get_NIST()
{

//  adding NIST read time code ========
    char myTime[80]={0};
    char *mysplit[10];
    //char *myTest;
    char *myTest=(char*)malloc(sizeof(char) * 80);
    char *pch;
    //char *pch = malloc(sizeof(char) * 80);
    int sizecheck;

/*** function pointer ***/
    //sizecheck = buf_pitime(myTime);

    alt_pitime= &buf_pitime;      // This uses socket for NIST
    //alt_pitime= &no_net_pitime;     // This bypasses socket items but returns a time

    sizecheck = alt_pitime(myTime);

    //char* testtest= pitime();

/*****
57523 16-05-15 00:58:16 50 0 0 257.3 UTC(NIST) *
len = 51
*****/

    //pch = strtok (myTest," ");
    pch = strtok (myTime," ");
    //pch = strtok (testtest," ");  // this works
    while (pch != NULL)
    {
        //printf ("%s\n",pch);
        mysplit[k]= (char*)malloc(strlen(pch+1));
        strcpy(mysplit[k],pch);
        pch = strtok (NULL, " ");
        k++;
    }
    // format for LCD
    strcat(str_filenm,mysplit[7]);      // UTC(NIST)
    strcat(str_filenm,"\n");
    strcat(str_filenm,mysplit[2]);      // Time
    strcat(str_filenm,"\n");
    strcat(str_filenm,mysplit[1]);      // date
    printf("strcat %s\n",str_filenm);

    char* mytest1 = myTest+15;
    *mytest1=NL;
    char* mytest3=myTest+7;

    char* mytest4= myTest+24;
    *mytest4=EL;

    myTest[0]=0x20;     // clear the new line from NIST return

    lcd_write(str_filenm);   //NIST
// ==== NIST end
}
