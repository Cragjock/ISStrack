#include "ads1015.h"
#include "myi2c.h"

static char buf[10];

char* mux_type[]=
{
    "Ain_p=Ain0 & Ain_n=Ain1",
    "Ain_p=Ain0 & Ain_n=Ain3",
    "Ain_p=Ain1 & Ain_n=Ain3",
    "Ain_p=Ain2 & Ain_n=Ain3",
    "Ain_p=Ain0 & Ain_n=gnd",
    "Ain_p=Ain1 & Ain_n=gnd",
    "Ain_p=Ain2 & Ain_n=gnd",
    "Ain_p=Ain3 & Ain_n=gnd"
};

/*** Samples per second ***/
unsigned int data_rates[8] = {128, 250, 490, 920, 1600, 2400, 3300, 3300};

/*** PGA fullscale voltages in mV ***/
unsigned int fullscale[8] = {6144, 4096, 2048, 1024, 512, 256, 256, 256 };

char* c_mode[]={"Continuous convert", "Single-shot convert"};

enum channel {MUX0, MUX1, MUX2, MUX3, MUX4, MUX5, MUX6, MUX7};
enum D_R {DR128SPS, DR250SPS, DR490SPS, DR920SPS, DR1600SPS, DR2400SPS, DR3300SPS, DR3301SPS};
enum FScale {mv6144, mv4096, mv2048, mv1024, mv512, mv256};

//myADS1015 ADC = {0x48, mv2048, 1, MUX4, DR250SPS};
myADS1015 ADC = {0x48, mv4096, 0, MUX0, DR250SPS};

myADS1015 arrADC[4]=  {
                    {0x48, mv4096, 0, MUX0, DR250SPS},
                    {0x48, mv2048, 1, MUX1, DR250SPS},
                    {0x48, mv1024, 1, MUX4, DR1600SPS},
                    {0x48, 0, 0, 0, 0},
                };


//myADS1015 ADC = {0x48, mv2048, 1, MUX4, DR250SPS};

//const char * i2cdev[2] = {"/dev/ic2-0","/dev/i2c-1"};
static int slave_address = 0x48;
static int P_slave_address = 0x40;
uint16_t init_config_reg = 0;



UINT read_config_reg(int file)
{
    UINT result = 67;   //BS number to start
    UINT resultswap = 17;

    result=i2c_smbus_read_word_data(file, Config_Reg);
    printf("read config register: %x \n", result);

    resultswap=myI2C_read_swap(file, Config_Reg);
    printf("read swapped config register: %x \n", resultswap);

    myADS1015 current_CR;
    current_CR.data_rate = (result & 0x0e0) >> CR_DR0;
    current_CR.Mux = (result & 0x7000) >> CR_MUX0;    current_CR.PGA = (result &0x0E00 ) >> CR_PGA0;

    arrADC[3].data_rate = (resultswap & 0x0e0) >> CR_DR0;
    arrADC[3].Mux = (resultswap & 0x7000) >> CR_MUX0;
    arrADC[3].PGA = (resultswap &0x0E00 ) >> CR_PGA0;

    printf("current config reg is: %x\ndata rate is: %x\nmux is: %x\nPGA is: %x\n",
                    resultswap,
                    arrADC[3].data_rate,
                    arrADC[3].Mux,
                    arrADC[3].PGA);

    printf("Mux number is: %s \n", mux_type[arrADC[3].Mux]);
    printf("PGA setting is: %d \n", fullscale[arrADC[3].PGA]);
    printf("Data rate is: %d \n", data_rates[arrADC[3].data_rate]);

    return resultswap;
}

/**************************************
    Initialize bus and ADC
**************************************/
int ADS1015_Init(const char* devname)
{
    int file= I2C_Open(1, slave_address);
    //file = open(devname, O_RDWR);

    init_config_reg = mux_diff_1 | PGA_4096 | DR_250sps | MODE_CONTINUOUS | COMP_QUE_DISABLE;
    /// should be 0x0223
    myI2C_write_swap(file, Config_Reg, init_config_reg);

    return file;
}

/*************/
int ADS1015_op_init(int file)   // maybe not needed
{
    //unsigned int i = 1;
    //char *c = (char*)&i;
    //if (*c)
    //   printf("Little endian\n");
    //else
    //   printf("Big endian\n");;

    UINT result = 0x1234; // BS number
    UINT res = 0x4823;
    UINT result1 = 0x5678;

    //result=i2c_smbus_read_word_data(file, Config_Reg);
    result = myI2C_read_swap(file, Config_Reg);
    printf("the op intit config register is x%x: \n", result);

    result1 = (result & 0x1000); // check if still converting or not
    if((result & 0x1000) == 0x1000)             // if (x & MASK) = MASK, ok
        printf("Not doing a conversion\n");
    else
        printf("conversion in process\n");

    result1 = ((result & 0x7000)>> 12);
    printf("Mux number is :%s \n", mux_type[result1]);       // which MUX type

    result1 = ((result >>5) & 0x0007);
    printf("Data rate is :%d \n", data_rates[result1]);        //data rate
    result1 = ((result >>9) & 0x0007);
    printf("PGA setting is :%d \n\n\n\n", fullscale[result1]);

	UINT config = res;
	config &= 0x001f;                   // strip for COMP items
	config |= (0 << 15) | (0 << 8);     // set for continuous
	config |= (ADC.Mux & 0x0007) << 12;
	config |= (ADC.PGA & 0x0007) << 9;
	config |= (ADC.data_rate & 0x0007) << 5;

    result1 = ((config & 0x7000)>> 12);
    printf("Mux number is :%s \n", mux_type[result1]);       // which MUX type

    result1 = (config >>5)& 0x0007;
    printf("Data rate is: %d SPS\n", data_rates[result1]);        //data rate

    result1 = (config >>9) & 0x0007;
    printf("PGA setting is: %d mv\n", fullscale[result1]);

    result1 = (config >>8)& 0x0001;
    printf("Current mode is: %s\n", c_mode[result1]);

    //=== get the byte order correct ====
    buf[2]=HBYTE(config);
    buf[3]=LBYTE(config);
    result=(buf[3]<<8) | buf[2];    //x = lobyte << 8 | hibyte;
    printf("byte order correct config reg: 0x%x\n", result);

    result=i2c_smbus_write_word_data(file, Config_Reg, result);
    //result=myI2C_write_swap(file, Config_Reg, result);

    //result=i2c_smbus_write_word_data(file, Config_Reg,0x4302);
    //result=i2c_smbus_write_word_data(file, Config_Reg,config);

    //result=i2c_smbus_read_word_data(file, Config_Reg);
    result=read_config_reg(file);
    //result = myI2C_read_swap(file, Config_Reg);

    //uint16_t sdc = bswap_16(result); // grt the right order to display
    printf("the op intit config register is x%x: \n", result);

    return 0;
}


/**< KEEP THIS ************** */
float read_convert_register(int file)
{

    //SINT result = 23; // BS number
    int16_t result = 0x1234; // due to __s32 i2c_smbus_read_word_data(int file, __u8 command)
    int16_t result_sw = 0x7893;
    uint16_t result1= 0x8312; // BS data

    //result = i2c_smbus_read_word_data(file, Convert_Reg);       // read the data
    result = myI2C_read_swap(file, Convert_Reg);                // read the data
    printf("raw convert register count is: 0x%x\n", result);

    /************************************
        conversion is:
        (conversion register count)/16 (right shift 4 bits)
        then times the gain, this gives volt count times 1000
        then divide 1000 to set decimal place correctly
    ************************************/

    // check if negative
    if((result & SIGN_MASK) == SIGN_MASK)
        result = -(~(result)+1);

    result = (result>>4)*2;         // assumes gain is 2
    printf("===================");
    printf("the voltage (x1) is: %2.3f\n", (float)result/1000);
    float fresult = (float)result/1000;

    float PCAcount = ((float)result/1000*80)+320;
    printf("Test equation for servo count: %2.3f\n", PCAcount);

    //result=i2c_smbus_read_word_data(file, Config_Reg); not needed
    //result = myI2C_read_swap(file, Config_Reg);
    //printf("the config register is: x%x\n", result);

    //return fresult; //this returns voltage
    return PCAcount; //this returns servo count for a given voltage
}


/**************************************
    Initialize bus and ADC
**************************************/
int PCA_Init(const char* devname)
{
    int file= I2C_Open(1, P_slave_address);
    //file = open(devname, O_RDWR);



    return file;
}

int set_count(int file, int channel, int start_count, int stop_count)
{

    int Myrec=0;
    int LEDchannel=channel;
    LEDchannel= (4*channel);
    //printf("setcount LEDchannel is 0x%02X\n", LEDchannel);

    char LH_lowbyte=LBYTE(start_count);
    char LH_highbyte=HBYTE(start_count);
    char HL_lowbyte=LBYTE(stop_count);
    char HL_highbyte=HBYTE(stop_count);

    i2c_smbus_write_byte_data(file,LED0_ON_L + LEDchannel,LH_lowbyte);   // reset Mode1 to before
    i2c_smbus_write_byte_data(file,LED0_ON_H + LEDchannel,LH_highbyte);   // reset Mode1 to before
    i2c_smbus_write_byte_data(file,LED0_OFF_L + LEDchannel,HL_lowbyte);   // reset Mode1 to before
    Myrec=i2c_smbus_write_byte_data(file,LED0_OFF_H + LEDchannel,HL_highbyte);   // reset Mode1 to before
    //printf("setcount Bytes written is 0%d\n", Myrec);

    return 0;

}





int set_all(int file, int start_count, int stop_count)
{

    char LH_lowbyte=LBYTE(start_count);
    char LH_highbyte=HBYTE(start_count);
    char HL_lowbyte=LBYTE(stop_count);
    char HL_highbyte=HBYTE(stop_count);

    buf[0] = ALL_LED_ON_L;
    buf[1] = 0xc;                   // 0000 1100
    buf[2] = ALL_LED_ON_H;
    buf[3] = 0x04;                  // 0000 0100
    buf[4] = ALL_LED_OFF_L;
    buf[5] = 0xbc;                  // 1011 1100
    buf[6] = ALL_LED_OFF_H;
    buf[7] = 0x2;                   // 0000 0010

    buf[1]= LH_lowbyte;     //lowtohigh[0];
    buf[3]= LH_highbyte;      //hightolow[0];
    buf[5]= HL_lowbyte;     //lowtohigh[0];
    buf[7]= HL_highbyte;     //hightolow[0];

//    int MyMode1=i2c_smbus_read_byte_data(file, MODE1);    // result in MyMode1

    int Myrec= write(file, buf, 2);
    Myrec= write(file, buf+2, 2);
    Myrec= write(file, buf+4, 2);
    Myrec= write(file, buf+6, 2);

    printf("Bytes written is 0%d\n", Myrec);
    return 0;

}

int PCA9685_start(int file)
{
    int good=set_all(file, 0, 0);


/*********************************************
    playing to see if it sticks?
*********************************************/
    buf[0]=MODE1;
    buf[1]=1;
    i2c_smbus_write_byte_data(file,MODE1,buf[1]);
    //int Myrec= write(file, buf, 2);


/********************************************
    Want MODE2 OUTDRV to be set to on.
    LED set to to totem pole output
    Bit 2 = 1 (0x04)
    Ideal MODE 2 should be 0000 0100
    Should I read mode2 &0 then or 0x04?
********************************************/
    int MyMode2=i2c_smbus_read_byte_data(file, MODE2);    // result in MyMode2
    printf("current Mode 2 data: 0x%02X\n",MyMode2);
    int play = MyMode2 | OUTDRV;        // OUTDRV 0x04 0100
    buf[0]= MyMode2 | OUTDRV;
    MyMode2 = i2c_smbus_write_byte_data(file,MODE2,buf[0]);   // send OUTDRV on
    printf("Mode 2 data with OUTDRV set, bit 2: 0x%02X\n", buf);



/********************************************
    Want EXTCLK to be 0,use internal clock.

    Ideal MODE 1 is 0000 0001
                    R00S 0001
    RESTART
        if sleep set to 1, restart will be set to 1.
        to clear, set SLEEP=0, wait 500us, then
        write a 1 to RESTART. Then restart = 0.

    SLEEP
        0 = normal mode, sleep off
        1 = low power or sleep on, needed to update pre-scale
********************************************/
    int MyMode1=i2c_smbus_read_byte_data(file, MODE1);
    printf("current Mode 1 data: 0x%02X\n",MyMode1);
    play= MyMode1 & EXTCLOCK;           // EXTCLOCK 0xBF  1011 1111, maybe use x81 1000 0001 ?
    play= MyMode1 & 0x81;           // clear all but RESTART and ALLCALL
    play= MyMode1 | SLEEP_ON;       // Now, set SLEEP on
    buf[0]= (MyMode1 & 0x81) | SLEEP_ON;
    // buf[0]= MyMode1 & EXTCLOCK;     //
    i2c_smbus_write_byte_data(file,MODE1,buf[0]);
    printf("Mode 1 should be x000 0101 0x%02X\n", buf);
    //printf("Mode 1 data with EXTCLOCK off, use internal, bit 6: 0x%02X\n", buf);

/** SLEEP should be on, so set PRESCALE    **/
    buf[0]=PRESCALE;
    buf[1]= 0x79;       // hard code pre-scale to 121, 50 hz; 64h for 60 hz
    //int MyModeS= MyMode1 | SLEEP_ON;        // SLEEP_ON 0x10
    //i2c_smbus_write_byte_data(file,MODE1,MyModeS);
    i2c_smbus_write_byte_data(file,PRESCALE,buf);
    write(file, buf, 2);
    printf("=========================\n");

/**
Now, check RESTART, should be = 1. If = 1, then clear sleep.
Then wait 500 us.
Then, write a 1 to RESTART to clear to a 0.
**/
    MyMode1=i2c_smbus_read_byte_data(file, MODE1);

   // while(MyMode1 < 0x80)
   //     printf("RESTART not set to 1 yet");
    buf[0]=MODE1;
    buf[1]= MyMode1 & 0xEF; // 1110 1111
    i2c_smbus_write_byte_data(file,MODE1,buf[0]);
    write(file, buf, 2);
    // ================ wait 500us
    buf[0]=MODE1;
    buf[1]= buf[1] | 0x80;
    i2c_smbus_write_byte_data(file,MODE1,buf[0]);
    write(file, buf, 2);

    //MyModeS= MyMode1 | SLEEP_OFF;
    //i2c_smbus_write_byte_data(file,MODE1,MyModeS);
return 0;


}






/*******************************************************************/
//int I2C_Open(int bus, int addr)
//{
//    int file;
//    file = open(i2cdev[bus], O_RDWR);
//    if (file == -1)
//        {
//            perror(i2cdev[bus]);
//            int errsv = errno;
//            return -1;
//        }
//
//    if (ioctl(file, I2C_SLAVE, addr) < 0)
//    {
//        perror("Failed to acquire bus access and/or talk to slave");
        //exit(1);
//        return -1;
//    }
//    return file;
//}

//void I2C_Close(int filep)
//{
//    close(filep);
//}

/*********************************************************/
//int myI2C_write_data(int file, uint8_t command_reg, uint8_t data)
//{
//    int res = i2c_smbus_write_byte_data(file, command_reg, data);
    /** S Addr Wr [A] Comm [A] Data [A] P **/
//    if (res<0)
//    {
//        printf("result i2c write error");
//        return -1;
//    }
//    return 0;
//}
/***********************************************************/
//int32_t myI2C_read_data(int file, uint8_t command)
//{
//        int32_t res = i2c_smbus_read_byte_data(file, command);
//       if (res < 0)
//        {
//            printf("Read error");
//            return -1;
//        }
//        return res;      // return the read data
//}

//int myI2C_write_swap(int file, uint8_t command_reg, uint16_t data)
//{
    /** use the byte swap header **/
//    uint16_t data_swap = bswap_16(data);

//    printf("write swap: data in: %x, data swapped: %x\n", data, data_swap);

//    int res = i2c_smbus_write_word_data(file, command_reg, data_swap);
    /** S Addr Wr [A] Comm [A] DataLow [A] DataHigh [A] P **/
//    if (res<0)
//    {
//        printf("result i2c write error");
//        return -1;
//    }
//    return 0;
//}
/***********************************************************/
//int16_t myI2C_read_swap(int file, uint8_t command)
//{
//        int16_t res = i2c_smbus_read_word_data(file, command);
        /** S Addr Wr [A] Comm [A] S Addr Rd [A] [DataLow] A [DataHigh] NA P **/
//        if (res == -1)
//        {
//            printf("Read error");
//            return -1;
//        }

        /** use the byte swap header **/
//        uint16_t res_swap = bswap_16(res);
//        printf("read swap: data in: %x, data swapped: %x\n", res, res_swap);

//        return res_swap;      // return the read data
//}
