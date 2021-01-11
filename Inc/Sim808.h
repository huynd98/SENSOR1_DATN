#ifndef __SIM808_H__
#define __SIM808_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "Sim808.h"
#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
#include "math.h"
//#include "main.h"
#define SIMCOM_DATA_IN_MAX_SIZE 256
#define SIMCOM_DATA_CMD_MAX_SIZE 32
#define RETRY_NUM 4

#define DEBUG 0
///////////////////
typedef enum {SIM_UNKOWN, SIM_NOT_INSERT, SIM_INSERTED, SIM_INITED, SIM_CONNECTED} Enum_Simcom_Stat;
////////////////////////////////////////////////////////
typedef struct
{
	uint8_t phone_eep[12];
	uint8_t data_in_tmp;
	uint16_t data_in_index;
	uint32_t sim_wait_time;
	int8_t sim_inited;
	int8_t pwrkey;
	uint8_t retry;
	Enum_Simcom_Stat sim_stat;
	char data_in[SIMCOM_DATA_IN_MAX_SIZE];
	char data_cmd[SIMCOM_DATA_CMD_MAX_SIZE];
} Simcom_TypeDef;
typedef struct
{
	uint8_t sms_inited;      // Su dung de kiem tra trang thai khi gui tin
}
Sms_TypeDef;
typedef struct
{
 	int8_t gprs_simcom_inited;
	uint8_t data_grps_simcom[50];
	int8_t http_simcom_inited;
	int8_t http_simcom_post;
	int32_t GSM_latitude;
	int32_t GSM_longtitude;
	uint8_t year, mth, day, hour, min, sec;
} GPRS_Simcom_TypeDef;


typedef struct
{
 	int8_t gps_simcom_inited;
	int8_t gps_simcom_update_data;
	double GPS_latitude;
	double GPS_longtitude;
} GPS_Simcom_TypeDef;
/////////////////////////////////////////////
void Init_Sim_Parameter(void );
void simcom_puts(uint8_t* pData);
void Init_Sim_Parameter(void );
void deleteBuffer(char * buf);
void simcom_receive_data_it(void);
int8_t Sim_sendCommand(char* ,char* ,uint32_t ); // -1: ko phan hoi || phan hoi sai  1: phan hoi dung
int8_t Sim_init(void); // -1 : Khoi tao khong thanh cong
int8_t  Sim_Bearer_Configure(void ); //-1 fault  1: Config succes             
int8_t Sim_Bearer_Dis(void ); // -1: couldn't dis 1: dis oK
int8_t Sim_HTTP_Post(char * s); //-1: Fault 1: Succes 
int8_t Sim_Get_Phonenumber(void ); //-1: sms not init; 1 : get snumber succes
//////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* __SIM808_H__ */



