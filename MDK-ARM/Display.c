#include "Display.h"
//////////////////////////
extern float Temp;
extern float Turbidity ;
extern float PH;
extern float WaveAmplitude;
extern CLCD_I2C_Name LCD1;
////////////////////////
extern char Pdischarge[10];
extern char Pcharge[10];
extern char Icharge[10];


char SeaShow[20];
char PowerShow[20];
uint8_t lenDis;

void ShowSeaWatch(void )
{
	CLCD_I2C_Clear(&LCD1);
	CLCD_I2C_Init(&LCD1,&hi2c1,(0x27<<1),20,4);
	CLCD_I2C_SetCursor(&LCD1, 6, 0);
	CLCD_I2C_WriteString(&LCD1,"SEAWATCH");
	CLCD_I2C_SetCursor(&LCD1, 0, 1);
	sprintf(SeaShow,"Temp:%.2f%c  PH:%.1f",Temp,0xDF,PH);
	lenDis =strlen(SeaShow);
	CLCD_I2C_WriteString(&LCD1,SeaShow);
	
	CLCD_I2C_SetCursor(&LCD1, 0, 2);
	sprintf(SeaShow,"Turbidity: %.0f NTU",Turbidity);
	CLCD_I2C_WriteString(&LCD1,SeaShow);
	
	CLCD_I2C_SetCursor(&LCD1, 0, 3);
	sprintf(SeaShow,"Amplitude: %.1f m",WaveAmplitude);
	CLCD_I2C_WriteString(&LCD1,SeaShow);
	//CLCD_I2C_BlinkOn(&LCD1);
}
void ShowPower(void )
{
	CLCD_I2C_Clear(&LCD1);
	CLCD_I2C_Init(&LCD1,&hi2c1,(0x27<<1),20,4);
	CLCD_I2C_SetCursor(&LCD1, 5, 0);
	CLCD_I2C_WriteString(&LCD1,"SOLARPOWER");
//	Pdischarge = 15.4;
//	Pcharge = 5.2;
//	Icharge = 0.836;
	CLCD_I2C_SetCursor(&LCD1, 0, 1);
//	sprintf(PowerShow,"adsajkdsa");
//	lenDis =strlen(PowerShow);
	
	sprintf(PowerShow,"Pdischarge: %sW",Pdischarge);
	lenDis =strlen(PowerShow);
	CLCD_I2C_WriteString(&LCD1,PowerShow);
	
	CLCD_I2C_SetCursor(&LCD1, 0, 2);
	sprintf(PowerShow,"Pcharge: %sW",Pcharge);
	CLCD_I2C_WriteString(&LCD1,PowerShow);
	
	CLCD_I2C_SetCursor(&LCD1, 0, 3);
	sprintf(PowerShow,"Icharge: %sA",Icharge);
	CLCD_I2C_WriteString(&LCD1,PowerShow);
//	CLCD_I2C_BlinkOn(&LCD1);
//	for (uint8_t i=0;i<=4 ; i++)
//	{
//		HAL_GPIO_TogglePin(GPIOB,LED_TEST_Pin);
//		HAL_Delay(500);
//	}
}
void UpdateShowPower(void )
{
	CLCD_I2C_SetCursor(&LCD1,12 , 1);
	sprintf(PowerShow,"%sW",Pdischarge);
	CLCD_I2C_WriteString(&LCD1,PowerShow);
	
	CLCD_I2C_SetCursor(&LCD1, 9, 2);
	sprintf(PowerShow,"%sW",Pcharge);
	CLCD_I2C_WriteString(&LCD1,PowerShow);
	
	CLCD_I2C_SetCursor(&LCD1, 9, 3);
	sprintf(PowerShow,"%sA",Icharge);
	CLCD_I2C_WriteString(&LCD1,PowerShow);
	
}
