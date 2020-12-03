#include "Solar.h"
extern char buffer1[100];
extern char Rx_Buffer1[100];
char Vdischarge[10];
char Idischarge[10];
char Pdischarge[10];
char Icharge[10];
char Pcharge[10];
char Vbattery[10];
uint8_t LightStatus;  // 1 : cho phep bat den chop 0: ko cho phep
uint8_t CurrentSource; // 1: Solar 0: Wave
void PreProcesSOLAR(void )
{
		uint8_t len= strlen(Rx_Buffer1);
	uint8_t StartPos=0;
	uint8_t EndPos=0;
	uint8_t dem=0;
	for(uint8_t i=0; i<len;i++)
	{
	if (Rx_Buffer1[i] =='@')
		{
			 StartPos=i;
			//printf("%d\n\r",StartPos);
		}
		
	}
	for (uint8_t i=StartPos; i<len;i++)
	{
		if (Rx_Buffer1[i] =='!')
			{
				 EndPos=i;
			//printf("%d\n\r",EndPos);
				 break;
			}
	}
	if (EndPos-StartPos+1>2)
	{
		for (uint8_t i=0;i<100;i++) buffer1[i]=0;
		for (uint8_t i=StartPos; i<=EndPos;i++)
		{
			buffer1[dem++]=Rx_Buffer1[i];
		}
	}
	//printf("%s\n\r",buffer1);
}
uint8_t ACproces(void)
{
	return buffer1[5]-48;
}
uint8_t AIproces(void)
{
	char s[100];
	for (uint8_t z=0;z<100;z++) s[z]=0;
	 // ACDischarge
	uint8_t i=7;
	switch(buffer1[5])                           
	{
		
		case '7': 
		{					
							for (uint8_t j=0;j<=2;j++)
							{
									uint8_t count=0;
									while (buffer1[i]!=';')
									{
										s[count]=buffer1[i];
										i++;
										count++;
									}
									i++; // Bo qua dau cham phay
									switch(j)
									{
										case 0: for (uint8_t z=0;z<10;z++) Vdischarge[z]=0;
														for (uint8_t z=0;z<count;z++) Vdischarge[z]=s[z];
														break;
										case 1: for (uint8_t z=0;z<10;z++) Idischarge[z]=0;
														for (uint8_t z=0;z<count;z++) Idischarge[z]=s[z];
														break;
										case 2: for (uint8_t z=0;z<10;z++) Pdischarge[z]=0;
														for (uint8_t z=0;z<count;z++) Pdischarge[z]=s[z];
														break;	
										default:
														break;
									}
								}
							return 7;
			}
		case '8': 
		{					for (uint8_t j=0;j<=1;j++)
							{
									uint8_t count=0;
									while (buffer1[i]!=';')
									{
										s[count]=buffer1[i];
										i++;
										count++;
									}
									i++; // Bo qua dau cham phay
									switch(j)
									{
										case 0: for (uint8_t z=0;z<10;z++) Icharge[z]=0;
														for (uint8_t z=0;z<count;z++) Icharge[z]=s[z];
														break;
										case 1: for (uint8_t z=0;z<10;z++) Pcharge[z]=0;
														for (uint8_t z=0;z<count;z++) Pcharge[z]=s[z];
														break;
										default:
														break;
									}
								}			
						  	return 8;
		}
		case '9': 
		{		
							uint8_t count=0;
							while (buffer1[i]!=';')
							{
								s[count]=buffer1[i];
								i++;
								count++;
							}
							for (uint8_t z=0;z<10;z++) Vbattery[z]=0;
							for (uint8_t z=0;z<count;z++) Vbattery[z]=s[z];
							LightStatus=buffer1[++i]-48;
							CurrentSource=buffer1[i+2]-48;
								return 9;
		}
		default:
							return 0;  // ko co 7 8 9
	}
}

uint8_t MainProcesSOLAR(void)
{
	if (buffer1[2]=='A')
	{
		if (buffer1[3]=='C') return ACproces();
		else if (buffer1[3]=='I') return AIproces();
	}
	return 0;
}
void SetLightTurnOn(void)
{
	printf("@;C;1;!\n\r");
}
void SetLightTurnOff(void)
{
	printf("@;C;2;!\n\r");
}
void SetWaveToSolar(void)
{
	printf("@;C;3;!\n\r");
}
void SetSolarToWave(void)
{
	printf("@;C;4;!\n\r");
}
void SetSavePowerMode(void)
{
	printf("@;C;5;!\n\r");
}
void SetNormalMode(void)
{
	printf("@;C;6;!\n\r");
}
void GetDischarger(void)
{
	printf("@;I;1;!\n\r");
}
void GetCharger(void)
{
	printf("@;I;2;!\n\r");
}
void GetStatus(void)
{
	printf("@;I;3;!\n\r");
}
