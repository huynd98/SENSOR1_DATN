/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LCD.h"
#include "string.h"
#include "stdio.h"
#include "Display.h"
#include "Solar.h"
#include "Lora.h"
#include "Sim808.h"
#include "GPS.h"
#include "w25qxx.h"
#include "delay.h"
#include "ds18b20.h"
#include "Turbidity.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SEAWATCH_CHANNEL		1
#define POWER_CHANNEL	2
#define INFOR_CHANNEL 3
//#define DEBUG 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t Xcursor,Ycursor;
extern CLCD_I2C_Name LCD1;

extern RTC_TimeTypeDef sTime ;
extern RTC_DateTypeDef DateToUpdate ;

float Temp=37.2;
float Turbidity = 400.1;
float PH=6.5;
float WaveAmplitude=2.45;

float tesetmem;

/* Sim variables */
char Sim_response[500] = {0};
char Sim_Rxdata[2] = {0};

extern char Vdischarge[10];
extern char Idischarge[10];
extern char Pdischarge[10];
extern char Icharge[10];
extern char Pcharge[10];
extern char Vbattery[10];
extern uint8_t LightStatus;  // 1 : cho phep bat den chop 0: ko cho phep
extern uint8_t CurrentSource; // 1: Solar 0: Wave

extern  Simcom_TypeDef hsimcom;
extern  GPS_Simcom_TypeDef hgps;

typedef struct
{
	float Pdischarge;
	float PchargeWave;
	float PchargeSolar;
	float Idischarge;
	float IchargeWave;
	float IchargeSolar;
	double Longtitude;
	double Latitude;
	float  Temperature;
	float  Turbidity;
	float  PH;
	float  DO;
	float  WaveAmplitude;
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
} Data ;
Data SeaWatch;
Data SubSeaWatch[16];
Data SeaWatch1;
volatile char IDnumber[20]; 
volatile uint8_t LenIDnumber;
uint16_t CurrentPage;

uint16_t ADC[2];

uint8_t flash1[8] = {1,2,3,4,5,6,7,8};
uint8_t flash2[20] ;
uint8_t  L;
uint8_t second;


uint8_t CHANNEL,ButtonFlag,SimStatusFlag=0;


uint8_t countT2=0;
 
char httpbuffer[400];

char buffer1[100], buffer2[100]; //Khai bao mang buffer 100 phan tu
int len1; //Khai bao bien len tuong ung voi chieu dai
char Rx_indx1, Rx_data1[2], Transfer_cplt1=0;
char Rx_indx2, Rx_data2[2], Transfer_cplt2=0;
char Rx_Buffer1[100],Rx_Buffer2[255]; 

////temp
uint8_t temp_l, temp_h;
uint16_t temp;
uint8_t flagTemp;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t i;
	//static char data=0;
	if (huart-> Instance == USART1) // UART hien tai la uart3
	{ 
		if (Rx_indx1==0) {for (i=0;i<100;i++) Rx_Buffer1[i]=0;} // xoa Rx_Buffer truoc khi nhan du lieu
		
		if (Rx_data1[0] !=13) // Neu du lieu nhan duoc khac voi 13 ( enter )
		{
			Rx_Buffer1[Rx_indx1++]=Rx_data1[0];
//			if( (data=='@')&& (Rx_data[0]!='@')) Rx_Buffer[Rx_indx++]=Rx_data[0];
//			if ( (data==0)&& (Rx_data[0]=='@')) { data='@';} // Them du lieu vao Rx_Buffer
		}
		else // Neu phihm enter duoc nhan
		{
			Rx_indx1 =0 ;  // Dua indx ve 0
			Transfer_cplt1 =1 ; // Qua tring truyen hoan tat, du lieu da san sang de doc
			PreProcesSOLAR();
			MainProcesSOLAR();
			
			Transfer_cplt1=0;
		}
		HAL_UART_Receive_IT(&huart1 , (uint8_t *)&Rx_data1, 1); //Kich hoat Uart nhan du lieu ngat moi luc
	}
	else if (huart-> Instance == USART2) // UART hien tai la uart2 SIM
	{ 
//		if (Rx_indx2==0) {for (i=0;i<100;i++) Rx_Buffer2[i]=0;} // xoa Rx_Buffer truoc khi nhan du lieu
//		
//		if (Rx_data2[0] !=10) // Neu du lieu nhan duoc khac voi 13 ( enter )
//		{
//			Rx_Buffer2[Rx_indx2++]=Rx_data2[0];
////			if( (data=='@')&& (Rx_data[0]!='@')) Rx_Buffer[Rx_indx++]=Rx_data[0];
////			if ( (data==0)&& (Rx_data[0]=='@')) { data='@';} // Them du lieu vao Rx_Buffer
//		}
//		else // Neu phim enter duoc nhan
//		{
//			Rx_indx2 =0 ;  // Dua indx ve 0
//			Transfer_cplt2 =1 ; // Qua tring truyen hoan tat, du lieu da san sang de doc
//		}
//		HAL_UART_Receive_IT(&huart2 , (uint8_t *)&Rx_data2, 1); //Kich hoat Uart nhan du lieu ngat moi luc
			simcom_receive_data_it();
	}
}
void BlinkLed( void )
{
	for (int8_t i=0; i<4;i++)
	{
		HAL_GPIO_TogglePin(GPIOB,LED_TEST_Pin);
		HAL_Delay(200);
	}
	
}
void ShowTEST(char * abc)
{
	CLCD_I2C_Clear(&LCD1);
	CLCD_I2C_SetCursor(&LCD1, 0, 0);
	CLCD_I2C_WriteString(&LCD1,abc);
}
void ReadCurrentPage(void ) // Sector 1
{
	uint8_t A[4];
	W25qxx_ReadSector(A,1,0,4);
	CurrentPage = A[0]+10*A[1]+100*A[2]+1000*A[3];
}
void WriteCurrentPage( uint16_t N ) // Sector 1
{
	uint16_t G= N;
	uint8_t a;
	W25qxx_EraseSector(1);
	for (uint8_t i=1;i<=4;i++)
			{
				a=G%10;
				G=G/10;
				W25qxx_WriteSector(&a,1,i-1,1);
			}			
}
void WriteSeaWatch(void )
{
	CurrentPage++;
	W25qxx_WritePage((uint8_t *)&SeaWatch,CurrentPage,0,sizeof(SeaWatch));
	WriteCurrentPage(CurrentPage);
}
void ReadSeaWatch()
{
	W25qxx_ReadPage((uint8_t *)&SeaWatch,CurrentPage,0,sizeof(SeaWatch));
	if (CurrentPage%16==0) //vua chuyen sang sector moi
	{
		// xoa Sector
		W25qxx_EraseSector(CurrentPage/16);
	}
	CurrentPage--;
	WriteCurrentPage(CurrentPage);
}

uint8_t PushSeaWatch(void )
{
	ReadSeaWatch();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_RTC_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(GPIOB,Power_Sensor_Pin,GPIO_PIN_SET);   // Turn on 5V for sensor and LCD
	
	CHANNEL =SEAWATCH_CHANNEL;
	ButtonFlag =0;
	SimStatusFlag=0;
	flagTemp =0;
  /////////////////////////////////////////
	CLCD_I2C_Init(&LCD1,&hi2c1,(0x27<<1),20,4);
	SaveCCG();
	ShowSeaWatch();
	DWT_Delay_Init();
	HAL_ADC_Start(&hadc2);
	HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_data1,1); // Kich hoat ngat uart khi nhan duoc 1 byte	
	HAL_TIM_Base_Start_IT(&htim2);
///////////////////////////////////////////////////////////////////
 
		if (W25qxx_Init()==true)
		{
			
			// W25qxx_EraseChip();
			W25qxx_ReadPage(&L,0,0,1);
					
			if ((L<=9)||(L>=12))
			{
					if (Sim_init() !=-1)
	        {
						if  (Sim_Get_Phonenumber()==1)
						{							
							
							W25qxx_EraseSector(0);
							LenIDnumber = strlen((char*)IDnumber);
							W25qxx_WritePage(&LenIDnumber,0,0,1);
							W25qxx_WritePage((uint8_t*)IDnumber,0,1,LenIDnumber);
							
						}	
					}
			}
			else 
			{
							W25qxx_ReadPage((uint8_t* )IDnumber,0,1,L);
							//ShowTEST("Da co ID trong FLASH");HAL_Delay(2000);
	
			}	
	
////			W25qxx_EraseSector(2);
////			WriteCurrentPage(31);
////  		ReadCurrentPage();	
		}

///////////////////////////////////////////////////////////////////////	 	 
//	Init_Sim_Parameter();


///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

// Day du lieu HTTP
///////////////////////////////////////////////////////
		if (Sim_init()==1)
		{
//		HAL_Delay(10000);
//// Doc thong so Power
//		PreProcesSOLAR();
//		MainProcesSOLAR();
	  SeaWatch.Pdischarge = atof(Pdischarge);
		SeaWatch.PchargeSolar = atof(Pcharge);
		SeaWatch.Idischarge = atof(Idischarge);
		SeaWatch.IchargeSolar = atof(Icharge);
//Doc thong so thoi gian
		HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc,&DateToUpdate,RTC_FORMAT_BIN);
		second = sTime.Seconds;
		SeaWatch.Minute=sTime.Minutes;
		SeaWatch.Hour = sTime.Hours;
		SeaWatch.Day = DateToUpdate.Date;
		SeaWatch.Month = DateToUpdate.Month;
		if (SeaWatch.Month >=10) SeaWatch.Month-=6;
		SeaWatch.Year = DateToUpdate.Year;
// Doc Sensor
		ReadValueTurbidity();
		SeaWatch.Turbidity = Turbidity;
		ReadvalueTemp();
		char  ch[7];
		sprintf(ch,"%d.%d",	temp_h,temp_l);	
		SeaWatch.Temperature = atof(ch)	;
// Doc GPS
///////////////////////////////////////////////////////
		if (GPS_TurnOn()==1)
		{
			if (GPS_Read()==1)
			{
			 SeaWatch.Latitude=	hgps.GPS_latitude;
			 SeaWatch.Longtitude= hgps.GPS_longtitude; 
			}
				else
					{
						SeaWatch.Latitude=0;
						SeaWatch.Longtitude=0;
					}
					
			GPS_TurnOff();
		}
////////////////////////////////////////////////////////
		if (Sim_Bearer_Configure()==1)
			
		{ 
			sprintf(httpbuffer,"{\"pDisc\":\"%.3f\",\"pCharW\":\"0\",\"pCharS\":\"%.3f\",\"iDisc\":\"%.3f\",\"iCharW\":\"0\",\"iCharS\":\"%.4f\",\"long\":\"%lf\",\"lat\":\"%lf\",\"temp\":\"%.1f\",\"ph\":\"%.1f\",\"do\":\"%.1f\",\"turbidity\":\"%.1f\",\"waveAmp\":\"%.2f\",\"year\":\"%d\",\"month\":\"%d\",\"day\":\"%d\",\"hour\":\"%d\",\"min\":\"%d\"}",SeaWatch.Pdischarge,SeaWatch.PchargeSolar,SeaWatch.Idischarge,SeaWatch.IchargeSolar,SeaWatch.Longtitude,SeaWatch.Latitude,SeaWatch.Temperature,SeaWatch.PH,SeaWatch.DO,SeaWatch.Turbidity,SeaWatch.WaveAmplitude,SeaWatch.Year,SeaWatch.Month,SeaWatch.Day,SeaWatch.Hour,SeaWatch.Minute);
			//if (Sim_HTTP_Post("{\"pDischarge\":\"12.12\"}")==1)   
			//if (Sim_HTTP_Post("{\"name\":\"test\",\"salary\":\"123\",\"age\":\"2\"}")==1)
				if (Sim_HTTP_Post(httpbuffer)==1)
				{
//					ReadCurrentPage();
//					for (uint16_t i=CurrentPage;i>31;i--)
//					{
//							
//							if (PushSeaWatch()==-1)  // Day lan luot tung Page len HTTP
//									{
//										CurrentPage ++;
//										uint8_t Sector=CurrentPage/16;
//										uint8_t OffsetPage= CurrentPage%16;
//										for (uint8_t i=0;i<=OffsetPage;i++)
//										{	
//											W25qxx_ReadPage((uint8_t *)&SeaWatch1,CurrentPage-OffsetPage+i,0,sizeof(SeaWatch1));  // Luu du lieu vao bo nho dem				
//										  SubSeaWatch[i]=SeaWatch1;
//										}
//			
//										W25qxx_EraseSector(Sector);		// xao  du lieu
//		  							for (uint8_t i=0;i<=OffsetPage;i++)
//										{																								
//												W25qxx_WritePage((uint8_t*)&SubSeaWatch[i],CurrentPage-OffsetPage+i,0,sizeof(SeaWatch1)); // Luu du lieu tu bo nho dem vao FLASH
//										}
//										break;
//									}
//							
//					}
				}
				else 
				{
					//WriteSeaWatch();//Luu vao o nho
				}
				Sim_Bearer_Dis();
		}
	}
		
	//	HAL_GPIO_WritePin(Power_Sensor_GPIO_Port,Power_Sensor_Pin,1);
	
//		ReadvalueTemp();
//		HAL_Delay(36000);
//		HAL_GPIO_WritePin(LED_TEST_GPIO_Port,LED_TEST_Pin,1);
//		DWT_Delay_us(400);
//		HAL_GPIO_WritePin(LED_TEST_GPIO_Port,LED_TEST_Pin,0);
//		DWT_Delay_us(400);
//		if (DEBUG) 	ShowTEST("da ve ham main while");
//		HAL_Delay(300);
	HAL_Delay(10000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
	_ARMABI int fputc(int c, FILE *stream)
{
		HAL_UART_Transmit(&huart1, (uint8_t *) &c,1,100);
		return 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (flagTemp!=1)
	{
		if  (htim->Instance == TIM2)
		{
			countT2++;
			switch(countT2)
			{		
				case 1 :GetDischarger();
								break;
				case 2 :GetCharger();
								break;
				case 3 :GetStatus();
								break;
				case 4: countT2=0;
								break;
				default: break;
			}   
			if (countT2%2==0)
			{
				ReadvalueTemp();
				ReadValueTurbidity();
			}
			if (CHANNEL == INFOR_CHANNEL) UpdateInfor();
			if (CHANNEL == POWER_CHANNEL) UpdateShowPower();
			if (CHANNEL == SEAWATCH_CHANNEL) UpdateSeaWatch();
		}
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (flagTemp!=1)
	{
		if (GPIO_Pin == Button_Pin)
			{		
					HAL_GPIO_TogglePin(GPIOB,LED_TEST_Pin);
					if (CHANNEL == INFOR_CHANNEL) 
					{ 
						CHANNEL = POWER_CHANNEL; 
						ShowPower();
						//HAL_Delay(2000);	
					}
					else if (CHANNEL == POWER_CHANNEL) 
					{ 
						CHANNEL = SEAWATCH_CHANNEL;
						ShowSeaWatch();
						//HAL_Delay(2000);
					} 
					else if (CHANNEL == SEAWATCH_CHANNEL) 
					{ 
						CHANNEL = INFOR_CHANNEL;
						ShowInfor();
						//HAL_Delay(2000);
					} 
				//	ButtonFlag=0;
			}
		else if (GPIO_Pin == SIM_STATUS_Pin)
			{
				if (hsimcom.pwrkey==0) NVIC_SystemReset();
			}
		}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
