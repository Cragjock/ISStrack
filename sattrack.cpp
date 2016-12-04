

#include "stdafx.h"
#include <poll.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include "lcd.h"
#include "pitime.h"
#include "display.h"
#include "ads1015.h"

#define SW_GPIO_INTERRUPT_PIN 16   // for switch
#define GPIO_INTERRUPT_PIN 26   // for LED

//=============================================================
//		MAIN
//=============================================================


int mcp23s17_enable_interrupts(int GPIO_ID);
int mcp23s17_disable_interrupts(int GPIO_ID);
int LED_on(int GPIO_ID);
int LED_off(int GPIO_ID);

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

static int display_count;
float adcresult = 0.3;
char dis_buf[20];
const char* netname ="/home/pi/share/testit.txt";
FILE* myFP;
char read_buf[100];

void sig_handler(int sig);
bool ctrl_c_pressed = false;


int main(int argc, char* argv[])
{


 	#ifdef __linux__
 	struct sigaction sig_struct;
	sig_struct.sa_handler = sig_handler;
	sig_struct.sa_flags = 0;
	sigemptyset(&sig_struct.sa_mask);

	if (sigaction(SIGINT, &sig_struct, NULL) == -1)
    {
		cout << "Problem with sigaction" << endl;
		exit(1);
	}
	#endif // __linux__


/// ===    File read needed if moving something from PC to here
//    myFP = fopen(netname, "a+");
//    if(myFP == NULL)
//        {
//        cout<<"ERROR opening"<<endl;
//        exit(1);
//        }
//    fread(read_buf, 1, 100, myFP);
//    int buffersize = strlen(read_buf);
//    fclose(myFP);
//    cout<<"read the file: "<<read_buf<<endl;
// =============================================================

    int lcdp=lcd_open();
    int adcp=ADS1015_Init("/dev/i2c-1");

    PCA9685 myPCA={0x40, 0, 69, 0, 0, 0x11, 0x4, 50, 0x79,}; // control structure
    myPCA.file=PCA_Init("/dev/i2c-1");
    PCA9685_start(myPCA.file);

    //adcresult=read_convert_register(adcp);
    //sprintf(dis_buf, "ADC: %6.3f V", adcresult);
    //lcd_write(dis_buf);


	lcd_write("Hello from Steve's\nLCD stuff");
	lcd_clear();
	get_NIST();

	mcp23s17_enable_interrupts(GPIO_INTERRUPT_PIN);
	//mcp23s17_enable_interrupts(SW_GPIO_INTERRUPT_PIN);

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
	//ELLIPSE myEllipse;
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



/// used before
    //int s_count=read_convert_register_count(adcp);
    //set_count(myPCA.file, 0, 5, s_count); // file channel, start count, end count




/// LCD setup and stuff

    adcresult=read_convert_register_volts(adcp);
    sprintf(dis_buf, "ADC: %6.3f V", adcresult);
    lcd_write(dis_buf);

    /// aztovolts is the target reference position
    double aztovolts = (Deg(testlook.AZ)) * (3.2/360.0);

    /// wtf is the difference of the pot input, adcresults, and reference
    double wtf = aztovolts - adcresult; /// double - float

    printf("\nVOLTS: %6.3f V\n",adcresult);
    printf("AZ: %6.3f \n",Deg(testlook.AZ));
    printf("az to volts: %6.3f V\n",aztovolts);
    printf("DELTA: %6.3f \n",wtf);

    /**
    volts   0       1.6     3.2
    count   200     320     450
            max left    no motion   max right
            1 ms    1.5 ms  2ms
            50 hz timing
    **/

    /// for applying delta Vin
    //float PCAcount = (wtf*80)+320;   ///this is for  0 - 3.2 Vin
    float PCAcount = (wtf*80)+340;   ///this is for  0 - 3.2 Vin, 340 from measurements

    if(wtf< -1.25)
        PCAcount = 240;
    else if(wtf> 1.25)
            PCAcount = 425;


    //set_count(myPCA.file, 0, 5, PCAcount); // file channel, start count, end count
    set_count(myPCA.file, 0, 1, PCAcount); // file channel, start count, end count

    printf("MOTOR count: %6.3f \n",PCAcount);



//#define TRACK 0
//#define LOCATION 1
//#define SATDATA 2
//#define NIST 3

    if(display_count < 5)
    {
        display_control(TRACK, PLACENTIA, SB, Eset, testlook);
        display_count++;
        LED_off(GPIO_INTERRUPT_PIN);
    }
    else
    {
        display_control(LOCATION, PLACENTIA, SB, Eset, testlook);
        display_count++;
        LED_on(GPIO_INTERRUPT_PIN);
    }

    if(display_count > 10)
    {
        display_count = 0;
	}

        /**
            ====================
            Look angles:visible
            AZ:123456 EL:123456
            Sat LAT/LONG
            LT:123456 LG:123456
            ====================
            Location Yorba Linda
            LT:123456 LG:123456
            Range: 123456

            ====================
            Tracking:ISS (ZARYA)
            Incl:12345
            MM: 123456
            MA: 123456

        **/
/// LCD done

		goal = wait + clock();
		while( goal > clock() );

        #ifdef __linux__
        if(ctrl_c_pressed)
            {
                cout << "Ctrl^C Pressed" << endl;
                cout << "unexporting pins" << endl;
                //gpio26->unexport_gpio();
                //gpio16->unexport_gpio();
                mcp23s17_disable_interrupts(GPIO_INTERRUPT_PIN);
                //mcp23s17_disable_interrupts(SW_GPIO_INTERRUPT_PIN);

                cout << "deallocating GPIO Objects" << endl;
                //delete gpio26;
                //gpio26 = 0;
                //delete gpio16;
                //gpio16 =0;
                break;

            }
        #endif // __linux__
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
		set_all(myPCA.file, 0, 0);      /// kill the servos
		lcd_close();                    /// kill the LCD

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



int mcp23s17_enable_interrupts(int GPIO_ID)
{
    int fd, len;
    char str_gpio[3];
    char str_filenm[33];

    if ((fd = open("/sys/class/gpio/export", O_WRONLY)) < 0)
        return -1;
    printf("IN INIT EXPORT ~~~ \n");
    len = snprintf(str_gpio, sizeof(str_gpio), "%d", GPIO_ID);
    write(fd, str_gpio, len);
    close(fd);

    snprintf(str_filenm, sizeof(str_filenm), "/sys/class/gpio/gpio%d/direction", GPIO_ID);
    if ((fd = open(str_filenm, O_WRONLY)) < 0)
        return -1;
    printf("IN INIT OUT~~~ \n");
    write(fd, "out", 4);
    close(fd);
/**
    snprintf(str_filenm, sizeof(str_filenm), "/sys/class/gpio/gpio%d/edge", GPIO_INTERRUPT_PIN);
    if ((fd = open(str_filenm, O_WRONLY)) < 0)
        return -1;
    printf("IN INIT falling ~~~ \n");
    write(fd, "falling", 8);
    close(fd);
**/

    snprintf(str_filenm, sizeof(str_filenm), "/sys/class/gpio/gpio%d/value", GPIO_ID);
    if ((fd = open(str_filenm, O_WRONLY)) < 0)
        return -1;
    printf("IN INIT~~ LED ON ~~~ \n");
    write(fd, "1", 1);
    close(fd);

    return 0;
}

int mcp23s17_disable_interrupts(int GPIO_ID)
{
    int fd, len;
    char str_gpio[3];

    if ((fd = open("/sys/class/gpio/unexport", O_WRONLY)) < 0)
        return -1;

    len = snprintf(str_gpio, sizeof(str_gpio), "%d", GPIO_ID);
    write(fd, str_gpio, len);
    close(fd);

    return 0;
}



int LED_on(int GPIO_ID)
{
    int fd, len;
    char str_gpio[3];
    char str_filenm[33];
    snprintf(str_filenm, sizeof(str_filenm), "/sys/class/gpio/gpio%d/value", GPIO_ID);
    if ((fd = open(str_filenm, O_WRONLY)) < 0)
        return -1;
    printf("LED ON ~~~ \n");
    write(fd, "1", 1);      // this write turns the LED on
    close(fd);

    return 0;
}



int LED_off(int GPIO_ID)
{
    int fd, len;
    char str_gpio[3];
    char str_filenm[33];
    snprintf(str_filenm, sizeof(str_filenm), "/sys/class/gpio/gpio%d/value", GPIO_ID);
    if ((fd = open(str_filenm, O_WRONLY)) < 0)
        return -1;
    printf("LED OFF ~~~ \n");
    write(fd, "0", 1);      // this write turns the LED off
    close(fd);

    return 0;
}

void sig_handler(int sig)
{
	write(0,"\nCtrl^C pressed in sig handler\n",32);
	ctrl_c_pressed = true;
}


