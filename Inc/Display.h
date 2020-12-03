#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "Display.h"
#include "stm32f1xx_hal.h"
#include "LCD.h"
#include "string.h"
#include "i2c.h"
#include "stdio.h"

void ShowSeaWatch(void  );
void ShowPower(void );
void UpdateShowPower(void );
#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_H__ */

