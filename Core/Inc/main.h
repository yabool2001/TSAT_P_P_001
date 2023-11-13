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
  * Devices:
  * ACC: ST LIS2DW12
  * SATCOM: ASTROCAST S+
  * GNSS: L86
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

#include "my_rtc.h"
#include "my_nmea.h"
#include "astronode_definitions.h"
#include "astronode_application.h"
#include "my_astronode.h"
#include "my_lx6_gnss.h"
#include "my_lis2dw12.h"
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
bool is_evt_pin_high ( void ) ;
void my_ldg_on ( void ) ;
void my_ldg_off ( void ) ;
void my_lx6_on ( void ) ;
void my_lx6_off ( void ) ;
int32_t	my_lis2dw12_platform_write ( void* , uint8_t , const uint8_t* , uint16_t ) ;
int32_t	my_lis2dw12_platform_read ( void* , uint8_t , uint8_t* , uint16_t ) ;
bool enqueue_hello_payload ( void ) ;
bool is_system_initialized ( void ) ;
bool enqueue_payload ( void ) ;

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define L86_RXD_Pin GPIO_PIN_11
#define L86_RXD_GPIO_Port GPIOC
#define L86_RST_Pin GPIO_PIN_12
#define L86_RST_GPIO_Port GPIOC
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOF
#define DBG_TXD_Pin GPIO_PIN_2
#define DBG_TXD_GPIO_Port GPIOA
#define DBG_RXD_Pin GPIO_PIN_3
#define DBG_RXD_GPIO_Port GPIOA
#define LDG_Pin GPIO_PIN_5
#define LDG_GPIO_Port GPIOA
#define ASTRO_PWR_SW_Pin GPIO_PIN_6
#define ASTRO_PWR_SW_GPIO_Port GPIOA
#define LIS_SPI1_CS_Pin GPIO_PIN_14
#define LIS_SPI1_CS_GPIO_Port GPIOB
#define ASTRO_TXD_Pin GPIO_PIN_9
#define ASTRO_TXD_GPIO_Port GPIOA
#define ASTRO_RXD_Pin GPIO_PIN_10
#define ASTRO_RXD_GPIO_Port GPIOA
#define ASTRO_RST_Pin GPIO_PIN_11
#define ASTRO_RST_GPIO_Port GPIOA
#define ASTRO_EVT_Pin GPIO_PIN_12
#define ASTRO_EVT_GPIO_Port GPIOA
#define ASTRO_EVT_EXTI_IRQn EXTI4_15_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define ASTRO_WAKEUP_Pin GPIO_PIN_15
#define ASTRO_WAKEUP_GPIO_Port GPIOA
#define L86_PWR_SW_Pin GPIO_PIN_8
#define L86_PWR_SW_GPIO_Port GPIOC
#define LIS_SPI1_SCK_Pin GPIO_PIN_3
#define LIS_SPI1_SCK_GPIO_Port GPIOB
#define LIS_SPI1_MISO_Pin GPIO_PIN_4
#define LIS_SPI1_MISO_GPIO_Port GPIOB
#define LIS_SPI1_MOSI_Pin GPIO_PIN_5
#define LIS_SPI1_MOSI_GPIO_Port GPIOB
#define LIS_INT1_EXTI8_Pin GPIO_PIN_8
#define LIS_INT1_EXTI8_GPIO_Port GPIOB
#define LIS_INT1_EXTI8_EXTI_IRQn EXTI4_15_IRQn
#define LIS_INT2_EXTI9_Pin GPIO_PIN_9
#define LIS_INT2_EXTI9_GPIO_Port GPIOB
#define LIS_INT2_EXTI9_EXTI_IRQn EXTI4_15_IRQn
#define L86_TXD_Pin GPIO_PIN_10
#define L86_TXD_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */
#define FIRMWARE_RELEASE_YEAR			2023
#define FIRMWARE_VERSION				0.0.15
#define HUART_ASTRO						&huart1
#define HUART_DBG						&huart2
//#define HUART_Lx6						&huart3
#define UART_TIMEOUT 					1000
#define UART_TX_MAX_BUFF_SIZE			250
#define UART_TX_TIMEOUT					100
#define UART_ASTRO_RX_MAX_BUFF_SIZE		100
#define HSPI1							&hspi1

#define TIM_SECONDS_THS_SYSTEM_RESET	900		// 900 s = 15 min.
#define ASTRO_LOG_TIMER					60000 	// 60 000 ms = 60 s

#define SECONDS_IN_1_HOUR				3600
#define HAL_STD_TIME_OPS_THS			2		// 2 s

#define GNSS_ACTIVE_TIME_THS			60 // Więcej niż 60 s nie ma sensu, bo to oznacza, że nie będzie jak wysłać do satelite Astrocast
#define PDOP_THS						5.1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
