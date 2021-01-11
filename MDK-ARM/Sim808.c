#include "Sim808.h"
extern char Rx_indx2, Rx_data2[2], Transfer_cplt2;
extern char buffer2[255];
extern char Rx_Buffer2[100];
char * k=NULL;
extern uint8_t SimStatusFlag;
Sms_TypeDef hsms;
Simcom_TypeDef hsimcom;
GPRS_Simcom_TypeDef hgprs;
GPS_Simcom_TypeDef hgps;

extern char IDnumber[20];
/////////////////////////////////////////////////////////////////////////
extern  void ShowTEST(char * abc);
void Init_Sim_Parameter(void )
{
	hsimcom.pwrkey=0;
	hsimcom.data_in_index=0;
	hsimcom.sim_inited=-1;
	hsimcom.retry=RETRY_NUM;
	hsimcom.sim_stat=SIM_UNKOWN;
//	char data_in[SIMCOM_DATA_IN_MAX_SIZE];
//	char data_cmd[SIMCOM_DATA_CMD_MAX_SIZE];

	hsms.sms_inited=-1;
	
 	hgprs.gprs_simcom_inited=-1;
//	hgprs.data_grps_simcom[50];
	hgprs.http_simcom_inited=-1;
	hgprs.http_simcom_post=-1;
	hgprs.GSM_latitude=0;
	hgprs.GSM_longtitude=0;

 	hgps.gps_simcom_inited=-1;
	hgps.gps_simcom_update_data=-1;
	hgps.GPS_latitude=0;
	hgps.GPS_longtitude=0;
}
void simcom_puts(uint8_t* pData)
{

	HAL_UART_Transmit(&huart2, pData, strlen((char*)pData), 3000);

}
void simcom_receive_data_it(void)
{
	if (hsimcom.data_in_index > SIMCOM_DATA_IN_MAX_SIZE - 1)
	{
		hsimcom.data_in_index = 0;
	}
	hsimcom.data_in[hsimcom.data_in_index] = hsimcom.data_in_tmp;
	Rx_Buffer2[hsimcom.data_in_index] = hsimcom.data_in_tmp;
	hsimcom.data_in[hsimcom.data_in_index + 1] = 0;
	Rx_Buffer2[hsimcom.data_in_index + 1] = 0;
	hsimcom.data_in_index ++;
	//hsimcom.data_in_lasttime = HAL_GetTick();
	Transfer_cplt2 = 1;
	HAL_UART_Receive_IT(&huart2, &hsimcom.data_in_tmp, 1);
}
void deleteBuffer(char * buf)
{
	int len  = strlen(buf);
		for (int i =0; i< len ; i++)
	{
		buf[i] =0;
	}
}
void DeleDatain(void)
{
	hsimcom.data_in_index = 0;
	deleteBuffer((char*)hsimcom.data_in);
	for (uint8_t i=0;i<SIMCOM_DATA_IN_MAX_SIZE-1;i++) Rx_Buffer2[i]=hsimcom.data_in[i];
}
int8_t Sim_SendCommand(char * command,char * response, uint32_t timeout)
{
	int8_t answer = -1;
	while (hsimcom.retry > 0)
	{
	
	DeleDatain();
	uint32_t time = HAL_GetTick();
	uint32_t time1 = HAL_GetTick();
	simcom_puts((uint8_t*)command);
	do	
		{ 
			while(Transfer_cplt2 ==0)
			{
				if(HAL_GetTick() - time > timeout) 
				{
					return -1;
				}
			}
				time = HAL_GetTick();
				 
				// HAL_Delay(200);
				while((HAL_GetTick() - time < timeout))
				{
					if (HAL_UART_Receive_IT(&huart2, &hsimcom.data_in_tmp, 1) ==HAL_OK)
					{
						time1 = HAL_GetTick();
					}
					else 
					{
						if ( HAL_GetTick() - time1 > 100)
						{
							Transfer_cplt2 =0;
							if( strstr((char*)hsimcom.data_in,response)!=NULL)
							{
								answer =1;
								if (DEBUG) { ShowTEST(hsimcom.data_in);HAL_Delay(1000); }
								
							}
							else answer =-1;
		//			ShowTEST("OK");
		//			HAL_Delay(1000);
							break;
						}
					}
				}
		}
		
					
	while(answer == -1);
	//if (DEBUG) ShowTEST(hsimcom.data_in);
	if (answer==1) break;
	HAL_Delay(1000);
	hsimcom.retry --;	
	}	
	return answer;
}
int8_t Sim_init(void)
{
	Init_Sim_Parameter();
	hsimcom.retry = RETRY_NUM;
	//////////////////
	k = NULL;
	if (DEBUG) ShowTEST("Sim_init");
	/////////////////////////////
	HAL_UART_Receive_IT(&huart2 , &hsimcom.data_in_tmp, 1);
	//power down module sim
	//simcom_puts((uint8_t*)"AT+CPOWD=1\r\n");
	HAL_Delay(1000);
	
	//power on module sim
	hsimcom.pwrkey=1;
	HAL_GPIO_WritePin(SIM_PWRKEY_GPIO_Port, SIM_PWRKEY_Pin, GPIO_PIN_SET);
	if (DEBUG) ShowTEST("power down module sim");
	HAL_Delay(2500);
	hsimcom.pwrkey=0;
	HAL_GPIO_WritePin(SIM_PWRKEY_GPIO_Port, SIM_PWRKEY_Pin, GPIO_PIN_RESET);
	if (DEBUG) ShowTEST("power on module sim");
	HAL_Delay(2000);
	SimStatusFlag=0;
	
//////////////////////////////////////////
	if (Sim_SendCommand("AT\r\n","OK",3000)==-1)
	{
		return -1;
	}
	hsimcom.retry = RETRY_NUM;
////////////////////////////////////////////	
	if (Sim_SendCommand("AT+CPIN?\r\n","+CPIN: READY",5000) == -1 )
	{
		hsimcom.sim_inited = -1;
		return -1 ;
	}
	
	
	HAL_Delay(2000);
	
	hsimcom.sim_wait_time = HAL_GetTick() + 50000;
//////////////////////////////////////////////
		while (1)
	{
		//HAL_Delay(100);
		if (strstr((char*)hsimcom.data_in,"SMS Ready")!=NULL)
		{
			hsimcom.sim_stat = SIM_INSERTED;
			if (DEBUG) {  ShowTEST("SIM_INSERTED"); HAL_Delay(2000);}
			break;
		}
		else if (strstr((char*)hsimcom.data_in, "NOT INSERTED")!=NULL)
		{
			hsimcom.sim_inited = -1;
			if (DEBUG) { ShowTEST("SIM_NOT_INSERT");HAL_Delay(1000); }
			
			hsimcom.sim_stat = SIM_NOT_INSERT;
			return -1;
		}
		else if (hsimcom.sim_wait_time < HAL_GetTick())
		{
//			hsimcom.sim_inited = -1;
//			return;
			break;
		}
	}
////////////////////////////////////////////////////////
	hsimcom.retry = RETRY_NUM;
	if (Sim_SendCommand("AT+CMGF=1\r\n","OK",3000) == 1)
	{
		hsms.sms_inited = 1;   
		//return;
	}
///////Set Echo Mode
//	hsimcom.retry = RETRY_NUM;
//		if (Sim_SendCommand("ATE0\r\n","OK",3000) ==1)
//	{
//		hsms.sms_inited = 1;
//		//return;
//	}

//	if (Sim_SendCommand("AT&W\r\n","OK",3000) == 1)
//	{
//		hsms.sms_inited = 1;
//		//return;
//	}
	return 1;
}
int8_t Sim_Get_Phonenumber(void )
{
	if(hsms.sms_inited == 1)
	{
			hsimcom.retry = RETRY_NUM;
			if (Sim_SendCommand("AT+CUSD=1,\"*101#\"\r\n","ERROR",3000) == 1 )
			{
				hsms.sms_inited = -1;
				return -1 ;
			}
			else 
			{
				HAL_Delay(3000);
				char *p1 = strstr((char*)hsimcom.data_in, "+CUSD: 1,");
				p1=p1+10;
				char *p2 = strstr((char*)hsimcom.data_in, ".");
				if (p2-p1-1 >9)
				{
					memset(IDnumber, 0, 20);
					memcpy(IDnumber, p1,  abs(p2 -p1));
					if (DEBUG) { ShowTEST((char*) IDnumber);HAL_Delay(5000); }
					return 1;
				}
				else return 1;
			
			}
   }
	else return -1;
}
int8_t Sim_Bearer_Configure(void )
{
	hsimcom.sim_stat = SIM_INITED;
	hsimcom.sim_inited = 0;
	hsimcom.retry = RETRY_NUM;
	if (Sim_SendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n","OK",4000) ==1)
	{
		hgprs.gprs_simcom_inited = 0;
		//return;
	}
	else return -1;
	hsimcom.retry = RETRY_NUM;
	if (Sim_SendCommand("AT+SAPBR=3,1,\"APN\",\"v-internet\"\r\n","OK",4000) ==1)
	{
		hgprs.gprs_simcom_inited = 1;
		//return;
	}
	else return -1;
	hsimcom.retry = RETRY_NUM;
	if (Sim_SendCommand("AT+SAPBR=1,1\r\n","OK",4000) ==1)
	{
		hgprs.gprs_simcom_inited = 2;
		//return;
	}
	else return -1;
	hsimcom.retry = RETRY_NUM;
	if (Sim_SendCommand("AT+SAPBR=2,1\r\n","OK",4000) ==1)
	{
		hgprs.gprs_simcom_inited = 3;
		//return;
	}
	else return -1;
	return 1; // config succes
}
int8_t Sim_Bearer_Dis(void )
{
	hsimcom.retry = RETRY_NUM;
	if (Sim_SendCommand("AT+HTTPTERM\r\n","OK",4000) ==1)
	{
			hgprs.http_simcom_inited = -1;
	}	
	if (Sim_SendCommand("AT+SAPBR=0,1\r\n","OK",4000) ==1)
	{
		
		hgprs.gprs_simcom_inited = -1;

		return 1;
	}
	else return -1;
}
int8_t Sim_HTTP_Post(char * s)
{
	hgprs.http_simcom_post =0;
	hsimcom.retry = RETRY_NUM;
	if (Sim_SendCommand("AT+HTTPINIT\r\n","OK",3000) ==1) //init http
	{
		hgprs.http_simcom_inited = 1;
		//return 1;
	}
	hsimcom.retry = RETRY_NUM;
		if (Sim_SendCommand("AT+HTTPPARA=\"CID\",1\r\n","OK",3000) ==1) //set parameters for HTTP session
	{
		hgprs.http_simcom_inited = 2;
		//return 1;
	}
	hsimcom.retry = RETRY_NUM;
		if (Sim_SendCommand("AT+HTTPPARA=\"URL\",\"graduation-20201.herokuapp.com/socketSend\"\r\n","OK",3000) ==1) //connect API
	//if (Sim_SendCommand("AT+HTTPPARA=\"URL\",\"http://dummy.restapiexample.com/api/v1/create\"\r\n","OK",3000) ==1)
	{
		hgprs.http_simcom_inited = 3;
		//return 1;
	}
	hsimcom.retry = RETRY_NUM;
		if (Sim_SendCommand("AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n","OK",3000) ==1) //connect API
	{
		hgprs.http_simcom_inited = 4;
		//return 1;
	}
	for (uint8_t i=0; i<4;i++)
		{
			hsimcom.retry = RETRY_NUM;
			if (Sim_SendCommand("AT+HTTPDATA=400,20000\r\n","DOWNLOAD",10000) ==1) //set parameters for HTTP session
			{
				simcom_puts((uint8_t*)s);
				DeleDatain();
				uint32_t time= HAL_GetTick();
				while (HAL_GetTick()- time <40000)
				{
					if (strstr(hsimcom.data_in,"OK")!=NULL)
					{
						if (DEBUG) { ShowTEST("DOWNLOAD OK");HAL_Delay(1000); }
						break;
					}
					
				}
				hsimcom.retry =1;
			 if (	Sim_SendCommand("AT+HTTPACTION=1\r\n","200",20000) ==1)
			 {
				 if (DEBUG) { ShowTEST("200");HAL_Delay(10000); }
				 hgprs.http_simcom_post =1;		
				 
				 HAL_Delay(3000);
				simcom_puts("AT+HTTPREAD\r\n");
				 return 1;
			 }
			} 
	}
	if (hgprs.http_simcom_post ==0)
	{
		return -1;
	}	
}



