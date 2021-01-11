#include "main.h"
#include "delay.h"
#include "ds18b20.h"

extern uint8_t check, temp_l, temp_h;
extern uint16_t temp;
extern uint8_t flagTemp;
GPIO_InitTypeDef GPIO_InitStruct;

void ReadvalueTemp(void )
{
	  flagTemp =1;
		ds_init();
		ds_write(0xCC);  // skip ROM
		ds_write(0x44);  // convert t

		HAL_Delay(800);

		ds_init();
		ds_write(0xCC);  // skip ROM
		ds_write(0xBE);  // Read Scratchpad

		temp_l = ds_read();
		temp_h = ds_read();
		temp = (temp_h<<8)|temp_l;
		temp_h = (temp >> 4);
		temp_l = (temp_l & 0xf);
		flagTemp =0;
}

void gpio_set_input (void)
{
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void gpio_set_output (void)
{
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}


uint8_t ds_init (void) {
	gpio_set_output();   // set the pin as output
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);  // pull the pin low
	DWT_Delay_us(480);   // delay according to datasheet
	gpio_set_input();    // set the pin as input
	DWT_Delay_us(80);    // delay according to datasheet

	if (!(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6))) { // if the pin is low i.e the presence pulse is there
		DWT_Delay_us(400);  // wait for 400 us
		return 0;
	} else {
		DWT_Delay_us(400);
		return 1;
	}
}
void ds_write (uint8_t data) {
	gpio_set_output();   // set as output

	for (int i=0; i<8; i++)	{
		if ((data & (1<<i))!=0) {// if the bit is high
			// write 1
			gpio_set_output();  // set as output
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);  // pull the pin LOW
			DWT_Delay_us(1);  // wait for  us
			gpio_set_input();  // set as input
			DWT_Delay_us(60);  // wait for 60 us
		} else { // if the bit is low
			// write 0
			gpio_set_output();
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);  // pull the pin LOW
			DWT_Delay_us(60);  // wait for 60 us
			gpio_set_input ();
		}
	}
}

uint8_t ds_read(void) {
	uint8_t value=0;
	gpio_set_input();

	for (int i=0; i<8; i++) {
		gpio_set_output();   // set as output
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);  // pull the data pin LOW
		DWT_Delay_us(2);  // wait for 2 us
		gpio_set_input();  // set as input
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)) {  // if the pin is HIGH
			value |= 1<<i;  // read = 1
		}
		DWT_Delay_us(60);  // wait for 60 us
	}
	return value;
}



