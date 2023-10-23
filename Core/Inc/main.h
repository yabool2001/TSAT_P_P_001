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
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
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
void send_debug_logs ( char* ) ;
void reset_astronode ( void ) ;
void send_astronode_request ( uint8_t* , uint32_t ) ;
bool is_astronode_character_received ( uint8_t* ) ;
uint32_t get_systick ( void ) ;
bool is_systick_timeout_over ( uint32_t , uint16_t ) ;
void my_astro_on ( void ) ;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define L86_RXD_Pin GPIO_PIN_11
#define L86_RXD_GPIO_Port GPIOC
#define L86_RST_Pin GPIO_PIN_12
#define L86_RST_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOF
#define ASTRO_PWR_SW_Pin GPIO_PIN_3
#define ASTRO_PWR_SW_GPIO_Port GPIOC
#define DBG_TXD_Pin GPIO_PIN_2
#define DBG_TXD_GPIO_Port GPIOA
#define DBG_RXD_Pin GPIO_PIN_3
#define DBG_RXD_GPIO_Port GPIOA
#define LDG_Pin GPIO_PIN_5
#define LDG_GPIO_Port GPIOA
#define ASTRO_PWR_SWA6_Pin GPIO_PIN_6
#define ASTRO_PWR_SWA6_GPIO_Port GPIOA
#define ASTRO_TXD_Pin GPIO_PIN_9
#define ASTRO_TXD_GPIO_Port GPIOA
#define ASTRO_RST_Pin GPIO_PIN_7
#define ASTRO_RST_GPIO_Port GPIOC
#define ASTRO_RXD_Pin GPIO_PIN_10
#define ASTRO_RXD_GPIO_Port GPIOA
#define ASTRO_RSTA11_Pin GPIO_PIN_11
#define ASTRO_RSTA11_GPIO_Port GPIOA
#define ASTRO_EVENT_Pin GPIO_PIN_12
#define ASTRO_EVENT_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define ASTRO_WAKEUP_Pin GPIO_PIN_15
#define ASTRO_WAKEUP_GPIO_Port GPIOA
#define L86_PWR_SW_Pin GPIO_PIN_8
#define L86_PWR_SW_GPIO_Port GPIOC
#define LIS_SCL_Pin GPIO_PIN_6
#define LIS_SCL_GPIO_Port GPIOB
#define LIS_SDA_Pin GPIO_PIN_7
#define LIS_SDA_GPIO_Port GPIOB
#define LIS_INT1_EXTI8_Pin GPIO_PIN_8
#define LIS_INT1_EXTI8_GPIO_Port GPIOB
#define LIS_INT2_EXTI9_Pin GPIO_PIN_9
#define LIS_INT2_EXTI9_GPIO_Port GPIOB
#define L86_TXD_Pin GPIO_PIN_10
#define L86_TXD_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */
#define HUART_ASTRO						&huart1
#define HUART_DBG						&huart2
#define HUART_L86						&huart3
#define UART_TX_MAX_BUFF_SIZE			250
#define UART_TX_TIMEOUT					100
#define UART_ASTRO_RX_MAX_BUFF_SIZE		100
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
