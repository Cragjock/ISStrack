
#ifndef DISPLAY_H
#define DISPLAY_H

#define rowlength 23
#define row_length 20
#define LCD_row_length 20
#define TRACK 0
#define LOCATION 1
#define SATDATA 2

int display_control(int item_to_do, Observer Obs,
                    SATSUB SS,
                    SATELSET ES,
                    VectLook SPos);
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
#endif
