/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern UART_HandleTypeDef huart1;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_0_Pin GPIO_PIN_13
#define LED_0_GPIO_Port GPIOC
#define LED_1_Pin GPIO_PIN_14
#define LED_1_GPIO_Port GPIOC
#define LED_STATUS_Pin GPIO_PIN_15
#define LED_STATUS_GPIO_Port GPIOC
#define LED_CAN_BUSY_Pin GPIO_PIN_0
#define LED_CAN_BUSY_GPIO_Port GPIOH
#define ADC_SCL_Pin GPIO_PIN_0
#define ADC_SCL_GPIO_Port GPIOC
#define ADC_SDA_Pin GPIO_PIN_1
#define ADC_SDA_GPIO_Port GPIOC
#define Vpanel__Pin GPIO_PIN_2
#define Vpanel__GPIO_Port GPIOC
#define IPanel__Pin GPIO_PIN_0
#define IPanel__GPIO_Port GPIOA
#define Vbat_comp_Pin GPIO_PIN_2
#define Vbat_comp_GPIO_Port GPIOA
#define Vbat__Pin GPIO_PIN_3
#define Vbat__GPIO_Port GPIOA
#define TempDiode_Pin GPIO_PIN_4
#define TempDiode_GPIO_Port GPIOA
#define TempMos_Pin GPIO_PIN_5
#define TempMos_GPIO_Port GPIOA
#define Rad__Pin GPIO_PIN_6
#define Rad__GPIO_Port GPIOA
#define Bat_OverV_Out_Pin GPIO_PIN_7
#define Bat_OverV_Out_GPIO_Port GPIOA
#define Vbat_comp_in_EXT_Pin GPIO_PIN_4
#define Vbat_comp_in_EXT_GPIO_Port GPIOC
#define Vbat_comp_EXT_Pin GPIO_PIN_5
#define Vbat_comp_EXT_GPIO_Port GPIOC
#define Bat_OverV_Out_EXT_Pin GPIO_PIN_0
#define Bat_OverV_Out_EXT_GPIO_Port GPIOB
#define TImer_break_Pin GPIO_PIN_12
#define TImer_break_GPIO_Port GPIOB
#define PWM_N_Pin GPIO_PIN_13
#define PWM_N_GPIO_Port GPIOB
#define DB_1_Pin GPIO_PIN_14
#define DB_1_GPIO_Port GPIOB
#define DB_0_Pin GPIO_PIN_15
#define DB_0_GPIO_Port GPIOB
#define PWM_Pin GPIO_PIN_8
#define PWM_GPIO_Port GPIOA
#define ID_0_Pin GPIO_PIN_4
#define ID_0_GPIO_Port GPIOB
#define ID_1_Pin GPIO_PIN_5
#define ID_1_GPIO_Port GPIOB
#define ID_2_Pin GPIO_PIN_6
#define ID_2_GPIO_Port GPIOB
#define ID_3_Pin GPIO_PIN_7
#define ID_3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
