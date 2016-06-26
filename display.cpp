

#include "stdafx.h"
#include <unistd.h>
#include "lcd.h"
#include "display.h"

static void padrows();
static int dc; // draw counter to determine when to swap lcd write buffers


char smallbuf[20];
char buf[80];
char buf1[80];
char buf2[80];
char buf_temp[80]; // used to copy to as interim

char row1[rowlength];  /// was 25, 22 is no good, 23 is ok
char row2[rowlength];
char row3[rowlength];
char row4[rowlength];

char* row_1=new char[row_length];       // 22
char* row_2=new char[row_length];
char* row_3=new char[row_length];
char* row_4=new char[row_length];
char* ddram_data=new char[81]();    ///the () inits to all zero
char* ddram_data1=new char[81]();    ///the () inits to all zero
char* ddram_data2=new char[81]();    ///the () inits to all zero
char* ddram_data3=new char[81]();    ///the () inits to all zero
void padrows();

char upper[16] = {"Az: "};
char lower[16] = {"El: "};

/// LCD setup and stuff


int display_control(int item_to_do,
                    Observer PLACENTIA,
                    SATSUB SB,
                    SATELSET Eset,
                    VectLook testlook)
{

    padrows();
    lcd_home();

    switch(item_to_do)
    {
        case TRACK:
        {
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
        }
        break;

        case LOCATION:
        {
            sprintf(row_1, "Location %s", PLACENTIA.obsname);
            dc = strlen(row_1);
            memcpy(row1, row_1, dc);
            strcpy(ddram_data1,row1);        /// need this to set ddram_data ????
            sprintf(row_3, "LT:%6.2f LG:%6.2f", Deg(PLACENTIA.lat), Deg(PLACENTIA.lon));
            dc = strlen(row_3);
            memcpy(row3, row_3, dc);
            strcat(ddram_data1,row3);
            sprintf(row_2, "Sat Range: %6.2f", testlook.RG);
            //sprintf(row_2, "Range: %6.2f", SB.range);
            dc = strlen(row_2);
            memcpy(row2, row_2, dc);
            strcat(ddram_data1,row2);
            sprintf(row_4, "Steve %s", "Playing");
            dc = strlen(row_4);
            memcpy(row4, row_4, dc);
            strcat(ddram_data1,row4);
/***
        sprintf(buf, "Location %s", PLACENTIA.obsname);
        sprintf(buf1, "LT:%6.2f LG:%6.2f", Deg(PLACENTIA.lat), Deg(PLACENTIA.lon));
        strcat(buf,buf1);
        sprintf(buf1, "Range: %6.2f", SB.range);
        strcat(buf,buf1);
        lcd_write(buf);
        dc = strlen(buf);
        //cout<<buf<<" length is "<<dc<<endl;
***/
            sprintf(buf, "LT:%6.2f LG:%6.2f", Deg(PLACENTIA.lat), Deg(PLACENTIA.lon));
            cout<<buf<<endl;
            sprintf(buf, "Range: %6.2f", SB.range);
            cout<<buf<<endl;
            lcd_write(ddram_data1);
        }
        break;

        case SATDATA:
        {
            sprintf(row_1, "Tracking:%s ", "ISS (Zarya)");
            dc = strlen(row_1);
            memcpy(row1, row_1, dc-1);
            strcpy(ddram_data,row1);        /// need this to set ddram_data ????

            sprintf(row_3, "Incl: %6.2f", Eset.dIncl);
            dc = strlen(row_3);
            memcpy(row3, row_3, dc-1);
            strcat(ddram_data,row3);

            double lcd_period = period_from_MM(Eset.dMM);

            sprintf(row_2, "MM: %6.2f",Eset.dMM);
            dc = strlen(row_2);
            memcpy(row2, row_2, dc-1);
            strcat(ddram_data,row2);


            sprintf(row_4, "Period:%6.2f", lcd_period);
            dc = strlen(row_4);
            memcpy(row4, row_4, dc);
            strcat(ddram_data,row4);

        //sprintf(row_4, "MA:%6.2f", Eset.dMA);
        //dc = strlen(row_4);
        //memcpy(row4, row_4, dc-1);
        //strcat(ddram_data,row4);

        //cout<<"ddram_data "<<ddram_data<<endl;
        //cout<<"row 1: "<<row1<<endl;
        //cout<<"size row 1: "<<strlen(row1)<<endl;
        //cout<<"row 2: "<<row2<<endl;
        //cout<<"size row 2: "<<strlen(row2)<<endl;
        //cout<<"row 3: "<<row3<<endl;
        //cout<<"size row 3: "<<strlen(row3)<<endl;
        //cout<<"row 4: "<<row4<<endl;
        //cout<<"size row 4: "<<strlen(row4)<<endl;
            cout<<row1<<endl;
            cout<<row2<<endl;
            cout<<row3<<endl;
            cout<<row4<<endl;
            lcd_write(ddram_data);
        }
        break;

        default:
            lcd_write("NOTHING TO SAY");
    }




/***
    #ifdef __linux__
    lcd_clear();
    lcd_write(ddram_data);
    lcd_clear();
    lcd_write(ddram_data1);
    lcd_clear();
    lcd_write(row_1);
    lcd_set_cursor_address(0x40);
    lcd_write(row_2);
    lcd_set_cursor_address(0x14);
    lcd_write(row_3);
    lcd_set_cursor_address(0x54);
    lcd_write(row_4);
    #endif // __linux__
***/
	return 0;
}


void padrows()
{
    memset(row1, ' ', row_length);
    memset(row2, ' ', row_length);
    memset(row3, ' ', row_length);
    memset(row4, ' ', row_length);
    memset(row_1, ' ', row_length);
    memset(row_2, ' ', row_length);
    memset(row_3, ' ', row_length);
    memset(row_4, ' ', row_length);

}
