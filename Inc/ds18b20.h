#ifndef _DS18B20_H_
#define _DS18B20_H_

#include "stm32f1xx_hal.h"
uint8_t ds_init (void);
void ds_write (uint8_t data);
uint8_t ds_read(void);
void ReadvalueTemp(void);

#endif
