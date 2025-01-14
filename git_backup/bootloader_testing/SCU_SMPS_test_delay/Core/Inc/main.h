/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32h5xx_hal.h"

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
#define UART_RX4_Pin GPIO_PIN_2
#define UART_RX4_GPIO_Port GPIOE
#define UART_TX4_Pin GPIO_PIN_3
#define UART_TX4_GPIO_Port GPIOE
#define GPI_3_Pin GPIO_PIN_4
#define GPI_3_GPIO_Port GPIOE
#define GPI_4_Pin GPIO_PIN_5
#define GPI_4_GPIO_Port GPIOE
#define GPI_5_Pin GPIO_PIN_6
#define GPI_5_GPIO_Port GPIOE
#define CS_1_Pin GPIO_PIN_6
#define CS_1_GPIO_Port GPIOF
#define SCKL_1_Pin GPIO_PIN_7
#define SCKL_1_GPIO_Port GPIOF
#define MISO_1_Pin GPIO_PIN_8
#define MISO_1_GPIO_Port GPIOF
#define MOSI_1_Pin GPIO_PIN_9
#define MOSI_1_GPIO_Port GPIOF
#define START_1_Pin GPIO_PIN_10
#define START_1_GPIO_Port GPIOF
#define DRDY_1_Pin GPIO_PIN_1
#define DRDY_1_GPIO_Port GPIOC
#define DRDY_1_EXTI_IRQn EXTI1_IRQn
#define RESET_1_Pin GPIO_PIN_2
#define RESET_1_GPIO_Port GPIOC
#define RS422_CTRL_Pin GPIO_PIN_0
#define RS422_CTRL_GPIO_Port GPIOA
#define RS485_CTRL1_Pin GPIO_PIN_1
#define RS485_CTRL1_GPIO_Port GPIOA
#define RS485_CTRL2_Pin GPIO_PIN_2
#define RS485_CTRL2_GPIO_Port GPIOA
#define Pwr_Ctrl1_Pin GPIO_PIN_5
#define Pwr_Ctrl1_GPIO_Port GPIOA
#define Pwr_Ctrl2_Pin GPIO_PIN_6
#define Pwr_Ctrl2_GPIO_Port GPIOA
#define GPO_1_Pin GPIO_PIN_0
#define GPO_1_GPIO_Port GPIOB
#define GPO_2_Pin GPIO_PIN_1
#define GPO_2_GPIO_Port GPIOB
#define RTC_RST_Pin GPIO_PIN_11
#define RTC_RST_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_11
#define LED1_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_12
#define LED2_GPIO_Port GPIOE
#define LED3_Pin GPIO_PIN_13
#define LED3_GPIO_Port GPIOE
#define LED4_Pin GPIO_PIN_14
#define LED4_GPIO_Port GPIOE
#define LED5_Pin GPIO_PIN_15
#define LED5_GPIO_Port GPIOE
#define CS_2_Pin GPIO_PIN_11
#define CS_2_GPIO_Port GPIOB
#define SCKL_2_Pin GPIO_PIN_13
#define SCKL_2_GPIO_Port GPIOB
#define MISO_2_Pin GPIO_PIN_14
#define MISO_2_GPIO_Port GPIOB
#define MOSI_2_Pin GPIO_PIN_15
#define MOSI_2_GPIO_Port GPIOB
#define START_2_Pin GPIO_PIN_6
#define START_2_GPIO_Port GPIOG
#define DRDY_2_Pin GPIO_PIN_7
#define DRDY_2_GPIO_Port GPIOG
#define DRDY_2_EXTI_IRQn EXTI7_IRQn
#define RESET_2_Pin GPIO_PIN_8
#define RESET_2_GPIO_Port GPIOG
#define START_3_Pin GPIO_PIN_10
#define START_3_GPIO_Port GPIOA
#define DRDY_3_Pin GPIO_PIN_11
#define DRDY_3_GPIO_Port GPIOA
#define DRDY_3_EXTI_IRQn EXTI11_IRQn
#define RESET_3_Pin GPIO_PIN_12
#define RESET_3_GPIO_Port GPIOA
#define SCKL_3_Pin GPIO_PIN_10
#define SCKL_3_GPIO_Port GPIOC
#define MISO_3_Pin GPIO_PIN_11
#define MISO_3_GPIO_Port GPIOC
#define MOSI_3_Pin GPIO_PIN_12
#define MOSI_3_GPIO_Port GPIOC
#define GPI_1_Pin GPIO_PIN_2
#define GPI_1_GPIO_Port GPIOD
#define GPI_2_Pin GPIO_PIN_3
#define GPI_2_GPIO_Port GPIOD
#define CS_3_Pin GPIO_PIN_4
#define CS_3_GPIO_Port GPIOD
#define UART_RX3_Pin GPIO_PIN_5
#define UART_RX3_GPIO_Port GPIOB
#define UART_TX3_Pin GPIO_PIN_6
#define UART_TX3_GPIO_Port GPIOB
#define UART_RX2_Pin GPIO_PIN_8
#define UART_RX2_GPIO_Port GPIOB
#define UART_TX2_Pin GPIO_PIN_9
#define UART_TX2_GPIO_Port GPIOB
#define UART_RX1_Pin GPIO_PIN_0
#define UART_RX1_GPIO_Port GPIOE
#define UART_TX1_Pin GPIO_PIN_1
#define UART_TX1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
