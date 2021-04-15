/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "stm32f2xx_hal.h"

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
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_2
#define LED3_GPIO_Port GPIOC
#define LED4_Pin GPIO_PIN_3
#define LED4_GPIO_Port GPIOC
#define OUT_Pin GPIO_PIN_0
#define OUT_GPIO_Port GPIOA
#define OUT_EXTI_IRQn EXTI0_IRQn
#define CSPOW_Pin GPIO_PIN_1
#define CSPOW_GPIO_Port GPIOA
#define CS_CS_Pin GPIO_PIN_4
#define CS_CS_GPIO_Port GPIOA
#define Q7_Pin GPIO_PIN_4
#define Q7_GPIO_Port GPIOC
#define Q8_Pin GPIO_PIN_5
#define Q8_GPIO_Port GPIOC
#define Q6_Pin GPIO_PIN_0
#define Q6_GPIO_Port GPIOB
#define Q5_Pin GPIO_PIN_1
#define Q5_GPIO_Port GPIOB
#define Q15_Pin GPIO_PIN_10
#define Q15_GPIO_Port GPIOB
#define Q16_Pin GPIO_PIN_12
#define Q16_GPIO_Port GPIOB
#define Q14_Pin GPIO_PIN_13
#define Q14_GPIO_Port GPIOB
#define Q13_Pin GPIO_PIN_14
#define Q13_GPIO_Port GPIOB
#define Q11_Pin GPIO_PIN_15
#define Q11_GPIO_Port GPIOB
#define Q12_Pin GPIO_PIN_6
#define Q12_GPIO_Port GPIOC
#define Q10_Pin GPIO_PIN_7
#define Q10_GPIO_Port GPIOC
#define Q9_Pin GPIO_PIN_8
#define Q9_GPIO_Port GPIOC
#define Q3_Pin GPIO_PIN_9
#define Q3_GPIO_Port GPIOC
#define Q4_Pin GPIO_PIN_8
#define Q4_GPIO_Port GPIOA
#define Q2_Pin GPIO_PIN_11
#define Q2_GPIO_Port GPIOA
#define Q1_Pin GPIO_PIN_15
#define Q1_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
#define SET_BITN(BYTE, BIT) SET_BIT(BYTE, (1<<BIT))
#define RESET_BITN(BYTE, BIT) CLEAR_BIT(BYTE, (1<<BIT))
#define READ_BITN(BYTE, BIT) READ_BIT(BYTE, (1<<BIT))>>BIT
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
