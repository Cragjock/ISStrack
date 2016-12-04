
//#include "stdafx.h"
#include "tle_read.h"



int Read_TLE(char* file, SATELSET &eset)
{
	char buffer[80];								//--- buffer array for reading lines of TLE file
	ifstream infile(file);						//--- open the file for input
	//ifstream infile(argv[1]);
	if(!infile)
	{
		cout<<"open failed"<<endl;
		return 1;									//--- return if file failed
	}

//=== READ THE KEP.TLE FILE ============================================
	do
	{
		infile.getline(buffer,80);					//--- read a line
		//cout<<buffer<<endl;						//--- test code line
		switch(buffer[0])
		{
			case 0:
				Read_Line(&eset,NULLLINE,buffer);	//--- only a NULL on the line
				break;
			case '1':
				Read_Line(&eset,LINEONE,buffer);		//--- read line 1 of the TLE
				break;
			case '2':
				Read_Line(&eset,LINETWO,buffer);		//--- read line 2 ofthe TLE
				break;
			default:
				Read_Line(&eset,NAMELINE,buffer);	//--- read the name line of the TLE
				//cout<<"Default"<<endl;
		}
	}
	while(infile);									//--- done with the TLE file
	infile.close();									//--- close the file
	return 0;
}

/*************************************************
    Read the data from the two line element file. In order to meet the need for
	for the atof(or atoi) char* buffer, the routine starts at the end of the buffer
	and moves to the beginning. this way a \0 can be inserted at the end of the
	needed ASCII string that gets passed to atof. Since the data has been consumed,
	the inserted \0 does not cause any data loss.

**************************************************/
void Read_Line(SATELSET *psat, int lne, char* chbuf)
{
	int i, ChkSum;
	double d;
	switch(lne)
	{
		case NULLLINE:
			//cout<<"NULLLINE"<<endl;
			break;
		case LINEONE:
			//cout<<"LINEONE"<<endl;

			ChkSum = atoi(&chbuf[68]);
			chbuf[68] = '\0';

			psat->iElementSet = atoi(&chbuf[64]);
			chbuf[64] = '\0';

			psat->iModelType = atoi(&chbuf[62]);
			chbuf[62] = '\0';

			i = atoi(&chbuf[59]);
			chbuf[59] = '\0';
			d = atof(&chbuf[53]);
			chbuf[53] = '\0';
			psat->dBSTAR = d * pow(10.0, (double)(i - 5));
			i = atoi(&chbuf[50]);
			chbuf[50] = '\0';

			d = atof(&chbuf[44]);
			chbuf[44] = '\0';
			psat->dNddot6 = d * pow(10.0, (double)(i - 5));
			psat->dDecayRate = atof(&chbuf[33]);
			chbuf[33] = '\0';

			psat->dEpochDay = atof(&chbuf[20]);
			chbuf[20] = '\0';

			psat->iEpochYear =atoi(&chbuf[18]);
			chbuf[18] = '\0';

			strncpy(psat->szLaunchPiece, &chbuf[14], 3);
			chbuf[14] = '\0';

			psat->iLaunchNum = atoi(&chbuf[11]);
			chbuf[11] = '\0';

			psat->iLaunchYear = atoi(&chbuf[9]);
			chbuf[9] = '\0';

			psat->iCatalogNum = atoi(&chbuf[2]);
			chbuf[2] = '\0';

			break;
		case LINETWO:
			//cout<<"LINETWO"<<endl;

			ChkSum = atoi(&chbuf[68]);
			chbuf[68] = '\0';

			psat->iEpochRev = atoi(&chbuf[63]);
			chbuf[63] = '\0';

			psat->dMM = atof(&chbuf[52]);
			chbuf[52] = '\0';

			psat->dMA = atof(&chbuf[43]);
			chbuf[43] = '\0';

			psat->dAOP = atof(&chbuf[34]);
			chbuf[34] = '\0';

			psat->dEcc = atof(&chbuf[26]) / 1.0e7;
			chbuf[26] = '\0';

			psat->dRAAN = atof(&chbuf[17]);
			chbuf[17] = '\0';

			psat->dIncl = atof(&chbuf[8]);
			chbuf[8] = '\0';

			break;
		case NAMELINE:
			strcpy(psat->szName,chbuf);
			//cout<<"NAMELINE"<<endl;
			break;
		default:
			cout<<"default"<<endl;
	}
}
