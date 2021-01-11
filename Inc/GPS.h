#ifndef __GPS_H__
#define __GPS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "GPS.h"
#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
#include "math.h"
#include "stdlib.h"
#include "Sim808.h"
#define DEBUG 0
//////////////////////////////////////////

int8_t GPS_TurnOn(void ); // -1 : Fault 1: Done
int8_t GPS_TurnOff(void ); // -1 : Fault 1: Done
int8_t GPS_Read(void );  // 0 : feedback fault 1: have data GPS -1: have not data GPS
/////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#endif /* __GPS_H__ */


