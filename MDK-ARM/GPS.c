#include "GPS.h"
///////////////////////////////////////////////////////////////
uint8_t poi;

extern  Simcom_TypeDef hsimcom;
extern  GPS_Simcom_TypeDef hgps;
////////////////////////////////////////////////////////////////
extern  void ShowTEST(char * abc);
int8_t GPS_TurnOff(void )
{
	hsimcom.retry = RETRY_NUM;
	if (Sim_SendCommand("AT+CGNSPWR=0\r\n","OK",3000)==1) 
	{
			hgps.gps_simcom_inited=-1;
			return 1;
	}
	else return -1; 
}
int8_t GPS_TurnOn(void )
{

	hsimcom.retry = RETRY_NUM;
	if (Sim_SendCommand("AT+CGNSPWR=1\r\n","OK",3000)==1) 
	{
			hgps.gps_simcom_inited=1;
			return 1;
	}
	else return -1; 
}
int8_t GPS_Read(void )   // doc 1 lan
{
	char *pt = NULL;
	uint8_t retry_gps=150; // wwait 200 turn until receives true GPS
	hgps.gps_simcom_update_data = -1;
	while (retry_gps>0)
	{
		hsimcom.retry = RETRY_NUM;
		if (Sim_SendCommand("AT+CGNSINF\r\n","+CGNSINF:",3000)==1)   // Kiem tra xem co thong so GPS
		{
				//memcpy(hsimcom.data_in,"+CGNSINF: 1,1,20201117084723.000,21.006495,105.841230,73.900,1.76,251.6,1,,1.9,2.1,1.0,,11,4,,,15,,",200);

			  pt = strstr((char*)hsimcom.data_in, "+CGNSINF:");
				memcpy(hsimcom.data_in, pt, 100);
				char raw_long[20];
				char raw_lat[20];
				memset(raw_long, 0, 20);
				memset(raw_lat, 0, 20);
				for (uint8_t i=0; i<2;i++)
				{
						char* p = strstr((char*)hsimcom.data_in, ",");
						if(strstr((char*)hsimcom.data_in, ",")!= NULL)
						{
							*p = '*';
						}
				}
				
				char* pt1 = strstr((char*)hsimcom.data_in, ",");
				if(pt1 != NULL)
				*pt1 = '*';
				char* pt2 = strstr((char*)hsimcom.data_in, ",");
				if(pt2 != NULL)
				*pt2 = '*';
				char* pt3 = strstr((char*)hsimcom.data_in, ",");
				memcpy(raw_lat, pt1+1,  abs(pt2 -pt1-1));
				memcpy(raw_long, pt2+1, abs(pt3 - pt2-1));

				if ((abs(pt2-pt1)>1) && (abs(pt3-pt2)>1))
				{
					
					hgps.GPS_latitude = atof(raw_lat);
					hgps.GPS_longtitude = atof(raw_long);
					hgps.gps_simcom_update_data = 1;
//					while (1)
//					{
//					if (DEBUG) { ShowTEST(raw_lat);HAL_Delay(1000); }
//					if (DEBUG) { ShowTEST(raw_long);HAL_Delay(1000); }
//				  }
					return 1; // Co du lieu GPS
			    }
				else
				{
					hgps.GPS_longtitude=0;
				  hgps.GPS_latitude=0;
					hgps.gps_simcom_update_data = -1;
					retry_gps --;	
					if (DEBUG) { ShowTEST("Du lieu khong chua GPS");HAL_Delay(1000); }
					//continue;
				}
			
			}
		else 
		{	
				hgps.gps_simcom_update_data = -1;
				return 0 ;   // khong phan hoi lenh
		}
		HAL_Delay(1000);
	}
	if (DEBUG) { ShowTEST("Du lieu khong chua GPS");HAL_Delay(1000); }
	return -1; // ko co du lieu GPSs
}
