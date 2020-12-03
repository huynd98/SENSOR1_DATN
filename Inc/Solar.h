#ifndef __SOLAR_H__
#define __SOLAR_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "Solar.h"
#include "stm32f1xx_hal.h"
#include "string.h"
#include "stdio.h"
//#include "main.h"

void PreProcesSOLAR(void );
void SetLightTurnOn(void);
void SetLightTurnOff(void);
void SetWaveToSolar(void);
void SetSolarToWave(void);
void SetSavePowerMode(void);
void SetNormalMode(void);
void GetDischarger(void);
void GetCharger(void);
void GetStatus(void);
uint8_t MainProcesSOLAR(void);
#ifdef __cplusplus
}
#endif

#endif /* __SOLAR_H__ */
