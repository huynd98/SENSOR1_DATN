#include "Display.h"
//////////////////////////
extern uint8_t temp_l, temp_h;
extern float Turbidity ;
extern float PH;
extern float WaveAmplitude;
extern CLCD_I2C_Name LCD1;
////////////////////////
extern char Pdischarge[10];
extern char Pcharge[10];
extern char Icharge[10];

extern volatile char IDnumber[20]; 

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
	sprintf(SeaShow,"Temp:%d.%d%c   pH:%.1f",temp_h,temp_l,0xDF,PH);
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

void ShowInfor(void )
{
	CLCD_I2C_Clear(&LCD1);
	CLCD_I2C_Init(&LCD1,&hi2c1,(0x27<<1),20,4);
	CLCD_I2C_SetCursor(&LCD1, 7, 0);
	CLCD_I2C_WriteString(&LCD1,"INFOR");
	CLCD_I2C_SetCursor(&LCD1, 0, 1);
	sprintf(PowerShow,"ID: %s",IDnumber);
	CLCD_I2C_WriteString(&LCD1,PowerShow);
	CLCD_I2C_SetCursor(&LCD1, 0, 2);
	sprintf(PowerShow,"VIETTEL: ");
	CLCD_I2C_WriteString(&LCD1,PowerShow);
	CLCD_I2C_SetCursor(&LCD1, 0, 3);
	sprintf(PowerShow,"BATTERY: ");
	CLCD_I2C_WriteString(&LCD1,PowerShow);
	CCG();
//	CLCD_I2C_WriteString(&LCD1,PowerShow);
}
void UpdateInfor(void)
{
		CLCD_I2C_SetCursor(&LCD1,9,3);  
	if (atof(Icharge)>=0.01)
	{
		CLCD_I2C_WriteChar(&LCD1, 0x06);
		CLCD_I2C_WriteChar(&LCD1, 0x07);
	}
	else
	{
		CLCD_I2C_WriteChar(&LCD1, 0x04);
		CLCD_I2C_WriteChar(&LCD1, 0x05);
	}
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
void UpdateSeaWatch(void)
{
	CLCD_I2C_SetCursor(&LCD1, 5, 1);
	sprintf(SeaShow,"%d.%d%c ",temp_h,temp_l,0xDF);
	CLCD_I2C_WriteString(&LCD1,SeaShow);
	for (uint8_t i=10;i<20;i++)
	{
		CLCD_I2C_SetCursor(&LCD1, i, 2);
		sprintf(SeaShow," ");
		CLCD_I2C_WriteString(&LCD1,SeaShow);
	}
	CLCD_I2C_SetCursor(&LCD1, 10, 2);
	sprintf(SeaShow,"%.1f NTU",Turbidity);
	CLCD_I2C_WriteString(&LCD1,SeaShow);

}
