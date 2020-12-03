/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SIM_DTR_Pin GPIO_PIN_1
#define SIM_DTR_GPIO_Port GPIOA
#define Power_Sensor_Pin GPIO_PIN_0
#define Power_Sensor_GPIO_Port GPIOB
#define SIM_PWRKEY_Pin GPIO_PIN_2
#define SIM_PWRKEY_GPIO_Port GPIOB
#define FLASH_NSS_Pin GPIO_PIN_12
#define FLASH_NSS_GPIO_Port GPIOB
#define LORA_NSS_Pin GPIO_PIN_8
#define LORA_NSS_GPIO_Port GPIOA
#define SIM_STATUS_Pin GPIO_PIN_11
#define SIM_STATUS_GPIO_Port GPIOA
#define LED_TEST_Pin GPIO_PIN_4
#define LED_TEST_GPIO_Port GPIOB
#define ACCELO_INT_Pin GPIO_PIN_5
#define ACCELO_INT_GPIO_Port GPIOB
#define ACCELO_INT_EXTI_IRQn EXTI9_5_IRQn
#define Button_Pin GPIO_PIN_9
#define Button_GPIO_Port GPIOB
#define Button_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
