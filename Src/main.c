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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SEAWATCH_CHANNEL		1
#define POWER_CHANNEL	2
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t Xcursor,Ycursor;
extern CLCD_I2C_Name LCD1;

float Temp=37.2;
float Turbidity = 400.1;
float PH=6.5;
float WaveAmplitude=2.45;

extern char Vdischarge[10];
extern char Idischarge[10];
extern char Pdischarge[10];
extern char Icharge[10];
extern char Pcharge[10];
extern char Vbattery[10];
extern uint8_t LightStatus;  // 1 : cho phep bat den chop 0: ko cho phep
extern uint8_t CurrentSource; // 1: Solar 0: Wave

uint8_t CHANNEL,ButtonFlag;

uint8_t countT2=0;

char buffer1[100]; //Khai bao mang buffer 100 phan tu
int len1; //Khai bao bien len tuong ung voi chieu dai
char Rx_indx1, Rx_data1[2], Transfer_cplt1=0;
char Rx_Buffer1[100]; 
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
		}
		HAL_UART_Receive_IT(&huart1 , (uint8_t *)&Rx_data1, 1); //Kich hoat Uart nhan du lieu ngat moi luc
	}
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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(GPIOB,Power_Sensor_Pin,GPIO_PIN_SET);
	
	CHANNEL =SEAWATCH_CHANNEL;
	ButtonFlag =0;
	//ShowPower();
	//CLCD_I2C_Init(&LCD1,&hi2c1,(0x27<<1),20,4);
	CLCD_I2C_Init(&LCD1,&hi2c1,(0x27<<1),20,4);
	ShowSeaWatch();
	//CLCD_I2C_Init(&LCD1,&hi2c1,(0x27<<1),20,4);
	HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_data1,1); // Kich hoat ngat uart khi nhan duoc 1 byte
	HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	//	CLCD_I2C_SetCursor(&LCD1, 18, 2);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(ButtonFlag)
		{		
				//GetDischarger();
				if (CHANNEL == SEAWATCH_CHANNEL) 
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
				ButtonFlag=0;
		}
		if (Transfer_cplt1)
		{
			//HAL_GPIO_TogglePin(GPIOB,LED_TEST_Pin);
			PreProcesSOLAR();
				HAL_GPIO_TogglePin(GPIOB,LED_TEST_Pin);
			MainProcesSOLAR();
//			uint8_t check ;
//			if( check == 7 ) {ShowPower(); };
			if (CHANNEL == POWER_CHANNEL) UpdateShowPower();
			Transfer_cplt1=0;
		}
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
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
		}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == Button_Pin)
		{
		  //HAL_GPIO_TogglePin(GPIOB,LED_TEST_Pin);
			ButtonFlag= 1;
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
