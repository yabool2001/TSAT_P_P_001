/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NMEA_3D_FIX						'3'
#define NMEA_MESSAGE_SIZE				250

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
char* 		hello = "\nHello TSAT_P_P_001\n\n" ;
char		dbg_buff[100] ;
char		rtc_dt_s[20] ;
uint32_t	current_ts = 0 ;

// Lx6
int32_t		astro_geo_wr_latitude ;
int32_t		astro_geo_wr_longitude ;
double		nmea_pdop_ths = PDOP_THS ;
uint16_t	my_lx6_gnss_active_time_ths = GNSS_ACTIVE_TIME_THS ; // Powinien być ten sam typ co tim_seconds // 240: 4 min.,
double 		nmea_fixed_pdop_d = 101 ;
char 		nmea_fixed_pdop_s[NMEA_FIX_PDOP_STRING_BUFF_SIZE] = {0} ; // 4 znaki wartości i kropka XX.X + '\0'
uint32_t	last_fix_ts = 0 ; // 0 timestamp (ts) is 1970.01.01 00:00:00

// TIM
uint16_t	tim_seconds = 0 ; // Powinien być ten sam typ co my_lx6_gnss_active_time_ths
uint32_t	agg_tim_gnss_seconds = 0 ;

// Astrocast
uint32_t	astro_log_loop_timer = 0 ;
uint16_t	astro_payload_id_counter = 1 ;
char		payload[ASTRONODE_APP_PAYLOAD_MAX_LEN_BYTES] = {0}; // 160 bajtów
char 		astro_payload_log[ASTRONODE_APP_PAYLOAD_MAX_LEN_BYTES+21] ; // Nagłowek + 12 + ew. znak minus + '\0'

// ACC
stmdev_ctx_t my_lis2dw12_ctx ;

// RTC
RTC_TimeTypeDef rtc_t ;
RTC_DateTypeDef rtc_d ;

// Flags
bool		is_system_already_initialized = false ; // Recognize if system has successful GNSS contact and has real time, Based on rtc settings.
bool		is_acc_int1_wkup_flag = false ;
bool		is_astro_evt_flag = false ;
bool		is_rtc_alarm_a_flag = false ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM6_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  // System hello
  HAL_UART_Transmit ( &huart2 , (uint8_t*) hello , strlen (hello) , UART_TIMEOUT ) ;
  __HAL_TIM_CLEAR_IT ( &htim6 , TIM_IT_UPDATE ) ;

  // Is system initialized?
  send_debug_logs ( "Start initialization process" ) ;
  if ( ! is_system_initialized () )
  {
	  // ASTRO INIT
	  if ( !my_astro_init () )
	  {
		  HAL_NVIC_SystemReset () ;
	  }

	  // ACC INIT
	  my_lis2dw12_ctx.write_reg = my_lis2dw12_platform_write ;
	  my_lis2dw12_ctx.read_reg = my_lis2dw12_platform_read ;
	  my_lis2dw12_ctx.handle = HSPI1 ;
	  my_lis2dw12_init ( &my_lis2dw12_ctx ) ;

	  // RTC INIT
  }

  // Preparation process
  send_debug_logs ( "Start preparation process" ) ;
  while ( !enqueue_hello_payload () )
  {
	  my_lis2dw12_int1_wu_enable ( &my_lis2dw12_ctx ) ;
	  send_debug_logs ( "main.c - preparation sm: Enter STOPMode during preparation process" ) ;
	  HAL_SuspendTick () ; // Jak nie wyłączę to mnie przerwanie SysTick od razu wybudzi!!!
	  HAL_PWR_EnterSTOPMode ( PWR_LOWPOWERREGULATOR_ON , PWR_STOPENTRY_WFE ) ;
	  HAL_ResumeTick () ;
	  if ( is_acc_int1_wkup_flag )
	  {
		  send_debug_logs ( "main.c - preparation sm: lis2dw12_int1 wake up after STOPMode" ) ;
		  // Turn off next int1_wkups for the end of procedure
		  my_lis2dw12_int1_wu_disable ( &my_lis2dw12_ctx ) ;
		  is_acc_int1_wkup_flag = false ;
		  HAL_Delay ( 500 ) ; // docelowo 2000
	  }
	  my_lis2dw12_int1_wu_enable ( &my_lis2dw12_ctx ) ;
	  //break ; // Comment it later! It's only necessary for indoor test
  }
  // Ciągle w ramach preparation process otrzyaj chociaż 1 potwierdzenie wysłania wiadomosci, żeby Astro miało czas rtc do NCO
  // Poniżej zrób kod do tego



  // TEST STOP SYSTEM
  /*
  HAL_SuspendTick () ; // Jak nie wyłączę to mnie przerwanie SysTick od razu wybudzi!!!
  HAL_PWR_EnterSTOPMode ( PWR_LOWPOWERREGULATOR_ON , PWR_STOPENTRY_WFE ) ;
  HAL_ResumeTick () ;
  my_lis2dw12_int1_wu_disable ( &my_lis2dw12_ctx ) ;
  */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  // RUNNING STATE MACHINE
  while ( 1 )
  {
	  if ( is_evt_pin_high() )
	  {
		  send_debug_logs ( "main.c - running sm: is_evt_pin_high" ) ;
		  my_astro_read_evt_reg () ;
	  }
	  else
	  {
		  send_debug_logs ( "main.c - running sm: no is_evt_pin_high" ) ;
		  send_debug_logs ( "main.c - preparation sm: Enter STOPMode during preparation process" ) ;
		  HAL_SuspendTick () ; // Jak nie wyłączę to mnie przerwanie SysTick od razu wybudzi!!!
		  HAL_PWR_EnterSTOPMode ( PWR_LOWPOWERREGULATOR_ON , PWR_STOPENTRY_WFE ) ;
		  HAL_ResumeTick () ;
	  }
	  if ( is_astro_evt_flag )
	  {
		  send_debug_logs ( "main.c - running sm: is_astro_evt_flag" ) ;
		  my_astro_read_evt_reg () ;
		  is_astro_evt_flag = false ;
	  }
	  if ( is_acc_int1_wkup_flag )
	  {
		  // Turn off next int1_wkups for the end of procedure
		  my_lis2dw12_int1_wu_disable ( &my_lis2dw12_ctx ) ;
		  is_acc_int1_wkup_flag = false ;

		  send_debug_logs ( "main.c - running sm: lis2dw12_int1 wake up after STOPMode" ) ;
		  my_astro_log ();


		  my_rtc_get_dt ( &rtc_d , &rtc_t ) ;
		  current_ts = my_conv_rtc2timestamp ( &rtc_d , &rtc_t ) ;
		  /*
		  dbg_buff[0] = 0 ;
		  sprintf ( dbg_buff , "Seconds elapsed from last fix: %lu" , current_ts - last_fix_ts ) ;
		  send_debug_logs ( dbg_buff ) ;
		  */
		  if ( ( current_ts - last_fix_ts ) > SECONDS_IN_1_HOUR ) // If more than 1 hour has elapsed from last fix.
		  {
			  if ( astronode_send_rtc_rr () && !astronode_send_nco_rr () ) // If Astro's RC know time and has opportunity to contact SV
			  {
				  enqueue_payload () ;
			  }
		  }
		  // Turn on int1_wkup
		  my_astro_log ();
		  my_lis2dw12_int1_wu_enable ( &my_lis2dw12_ctx ) ;
	  }
	  if ( is_rtc_alarm_a_flag )
	  {
		  send_debug_logs ( "main.c - running sm: rtc alarm A event" ) ;
		  enqueue_payload () ;
		  my_astro_log () ;
		  my_rtc_set_alarm ( SECONDS_IN_1_HOUR ) ;
		  is_rtc_alarm_a_flag = false ;
	  }
	  //HAL_GPIO_ReadPin ( GPIOB , LIS_INT1_EXTI8_Pin ) ;
	  //dbg_buff[0] = 0 ;
	  //sprintf ( dbg_buff , "INT status on LIS_INT1_EXTI8_Pin: %d!\n" , HAL_GPIO_ReadPin ( GPIOB , LIS_INT1_EXTI8_Pin ) ) ;
	  //send_debug_logs ( dbg_buff ) ;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.SubSeconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 16000-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, L86_RST_Pin|L86_PWR_SW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LDG_Pin|ASTRO_PWR_SW_Pin|ASTRO_RST_Pin|ASTRO_WAKEUP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LIS_SPI1_CS_GPIO_Port, LIS_SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : L86_RST_Pin L86_PWR_SW_Pin */
  GPIO_InitStruct.Pin = L86_RST_Pin|L86_PWR_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LDG_Pin ASTRO_PWR_SW_Pin ASTRO_RST_Pin ASTRO_WAKEUP_Pin */
  GPIO_InitStruct.Pin = LDG_Pin|ASTRO_PWR_SW_Pin|ASTRO_RST_Pin|ASTRO_WAKEUP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LIS_SPI1_CS_Pin */
  GPIO_InitStruct.Pin = LIS_SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LIS_SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ASTRO_EVT_Pin */
  GPIO_InitStruct.Pin = ASTRO_EVT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ASTRO_EVT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LIS_INT1_EXTI8_Pin LIS_INT2_EXTI9_Pin */
  GPIO_InitStruct.Pin = LIS_INT1_EXTI8_Pin|LIS_INT2_EXTI9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
bool enqueue_payload ( void )
{
	astro_geo_wr_latitude = 0 ;
	astro_geo_wr_longitude = 0 ;
	if ( my_lx6_get_coordinates ( my_lx6_gnss_active_time_ths , nmea_pdop_ths , &nmea_fixed_pdop_d , &astro_geo_wr_latitude , &astro_geo_wr_longitude ) )
	{
		my_astro_write_coordinates ( astro_geo_wr_latitude , astro_geo_wr_longitude ) ;

		// Update ts of last fix
		my_rtc_get_dt ( &rtc_d , &rtc_t ) ;
		last_fix_ts = my_conv_rtc2timestamp ( &rtc_d , &rtc_t ) ;

		dbg_buff[0] = 0 ;
		sprintf ( dbg_buff , "Last fix timestap: %lu" , last_fix_ts ) ;
		send_debug_logs ( dbg_buff ) ;

		my_rtc_get_time_s ( rtc_dt_s ) ;
		send_debug_logs ( rtc_dt_s ) ;

		if ( nmea_fixed_pdop_d < 100.0 )
		{
			snprintf ( nmea_fixed_pdop_s , NMEA_FIX_PDOP_STRING_BUFF_SIZE , "%.1f", nmea_fixed_pdop_d );
		}

		agg_tim_gnss_seconds = agg_tim_gnss_seconds + tim_seconds  ;

		sprintf ( payload , "%s,%d,%lu" , nmea_fixed_pdop_s , tim_seconds , agg_tim_gnss_seconds ) ;
		sprintf ( astro_payload_log , "Astronode payload: %s" , payload ) ;
		send_debug_logs ( astro_payload_log ) ;
		// astronode_send_pld_fr () ; // Don't clear entire payload queue because it's worth having a travel history, even if sent with delay.
		my_astro_add_payload_2_queue ( astro_payload_id_counter++ , payload ) ;
		if ( astro_payload_id_counter == 0 ) // Avoid id = 0 to avoid collision with control test payload that is always 0
		{
			astro_payload_id_counter++ ;
		}
		return true ;
	}

	return false ;
}

bool enqueue_hello_payload ( void )
{
	bool r = false ;
	const char* fv = "0.1.0" ;

	int32_t astro_geo_wr_latitude = 0 , astro_geo_wr_longitude = 0 ;

	if ( my_lx6_get_coordinates ( my_lx6_gnss_active_time_ths , nmea_pdop_ths , &nmea_fixed_pdop_d , &astro_geo_wr_latitude , &astro_geo_wr_longitude ) )
	{
		my_astro_write_coordinates ( astro_geo_wr_latitude , astro_geo_wr_longitude ) ;

		// Update ts of last fix
		my_rtc_get_dt ( &rtc_d , &rtc_t ) ;
		last_fix_ts = my_conv_rtc2timestamp ( &rtc_d , &rtc_t ) ;

		dbg_buff[0] = 0 ;
		sprintf ( dbg_buff , "Hello payload and first fix timestap: %lu" , last_fix_ts ) ;
		send_debug_logs ( dbg_buff ) ;

		my_rtc_get_time_s ( rtc_dt_s ) ;
		send_debug_logs ( rtc_dt_s ) ;

		snprintf ( nmea_fixed_pdop_s , NMEA_FIX_PDOP_STRING_BUFF_SIZE , "%.1f", nmea_fixed_pdop_d );

		agg_tim_gnss_seconds = agg_tim_gnss_seconds + tim_seconds  ;

		sprintf ( payload , "%s,%d,%lu;%s" , nmea_fixed_pdop_s , tim_seconds , agg_tim_gnss_seconds , fv ) ;
		sprintf ( astro_payload_log , "Astronode payload: %s" , payload ) ;
		send_debug_logs ( astro_payload_log ) ;
		// astronode_send_pld_fr () ; // Don't clear entire payload queue because it's worth having a travel history, even if sent with delay.
		my_astro_add_payload_2_queue ( 0 , payload ) ;

		return true ;
	}

	return r ;
}

void send_debug_logs ( char* p_tx_buffer )
{
    uint32_t length = strlen ( p_tx_buffer ) ;

    if ( length > UART_TX_MAX_BUFF_SIZE )
    {
        HAL_UART_Transmit ( HUART_DBG , ( uint8_t* ) "[ERROR] UART buffer reached max length.\n" , 42 , 1000 ) ;
        length = UART_TX_MAX_BUFF_SIZE;
    }

    HAL_UART_Transmit ( HUART_DBG , ( uint8_t* ) p_tx_buffer , length , 1000 ) ;
    HAL_UART_Transmit ( HUART_DBG , ( uint8_t* ) "\n" , 1 , 1000 ) ;
}
void reset_astronode ( void )
{
    HAL_GPIO_WritePin ( ASTRO_RST_GPIO_Port , ASTRO_RST_Pin , GPIO_PIN_SET ) ;
    HAL_Delay ( 1 ) ;
    HAL_GPIO_WritePin ( ASTRO_RST_GPIO_Port , ASTRO_RST_Pin , GPIO_PIN_RESET ) ;
    HAL_Delay ( 250 ) ;
}
void send_astronode_request ( uint8_t* p_tx_buffer , uint32_t length )
{
    send_debug_logs ( "Message sent to the Astronode --> " ) ;
    send_debug_logs ( ( char* ) p_tx_buffer ) ;

    HAL_UART_Transmit ( HUART_ASTRO , p_tx_buffer , length , 1000 ) ;
}
bool is_astronode_character_received ( uint8_t* p_rx_char )
{
    return ( HAL_UART_Receive ( HUART_ASTRO , p_rx_char , 1 , 100 ) == HAL_OK ? true : false ) ;
}
bool is_evt_pin_high ( void )
{
	return ( HAL_GPIO_ReadPin ( GPIOA , ASTRO_EVT_Pin ) == GPIO_PIN_SET ? true : false);
}

uint32_t get_systick ( void )
{
    return HAL_GetTick() ;
}
bool is_systick_timeout_over ( uint32_t starting_value , uint16_t duration )
{
    return ( get_systick () - starting_value > duration ) ? true : false ;
}
void my_ldg_on ( void )
{
	HAL_GPIO_WritePin ( GPIOA , LDG_Pin , GPIO_PIN_SET ) ;
}
void my_ldg_off ( void )
{
	HAL_GPIO_WritePin ( GPIOA , LDG_Pin , GPIO_PIN_RESET ) ;
}
void my_lx6_on ( void )
{
	HAL_GPIO_WritePin ( GPIOC , L86_PWR_SW_Pin , GPIO_PIN_SET ) ;
	HAL_GPIO_WritePin ( GPIOC , L86_RST_Pin , GPIO_PIN_SET ) ;
	MX_USART3_UART_Init () ;
}
void my_lx6_off ( void )
{
	HAL_GPIO_WritePin ( GPIOC , L86_PWR_SW_Pin , GPIO_PIN_RESET ) ;
	HAL_GPIO_WritePin ( GPIOC , L86_RST_Pin , GPIO_PIN_RESET ) ;
	HAL_UART_DeInit ( HUART_Lx6 ) ;
}
int32_t my_lis2dw12_platform_write ( void *handle , uint8_t reg , const uint8_t *bufp , uint16_t len )
{
	HAL_GPIO_WritePin	( LIS_SPI1_CS_GPIO_Port , LIS_SPI1_CS_Pin , GPIO_PIN_RESET ) ;
	HAL_Delay ( 20 ) ;
	HAL_SPI_Transmit	( handle , &reg , 1 , 1000 ) ;
	HAL_SPI_Transmit	( handle , (uint8_t*) bufp , len , 1000 ) ;
	HAL_GPIO_WritePin	( LIS_SPI1_CS_GPIO_Port , LIS_SPI1_CS_Pin , GPIO_PIN_SET) ;

	return 0;
}
int32_t my_lis2dw12_platform_read ( void *handle , uint8_t reg , uint8_t *bufp , uint16_t len )
{
	reg |= 0x80;
	HAL_GPIO_WritePin ( LIS_SPI1_CS_GPIO_Port , LIS_SPI1_CS_Pin , GPIO_PIN_RESET) ;
	HAL_Delay ( 20 ) ;
	HAL_SPI_Transmit ( handle , &reg , 1 , 1000 ) ;
	HAL_SPI_Receive ( handle , bufp , len , 1000 ) ;
	HAL_GPIO_WritePin ( LIS_SPI1_CS_GPIO_Port , LIS_SPI1_CS_Pin , GPIO_PIN_SET) ;

	return 0;
}
bool is_system_initialized ( void )
{
	uint16_t yyyy ;

	uint32_t commn_ts = astronode_send_rtc_rr () ;

	yyyy = my_rtc_get_time_s ( rtc_dt_s ) ;
	send_debug_logs ( rtc_dt_s ) ;
	if ( yyyy >= FIRMWARE_RELEASE_YEAR || commn_ts != 0 )
	{
		return true ;
	}
	return false ;
}

void HAL_TIM_PeriodElapsedCallback ( TIM_HandleTypeDef *htim )
{
	if ( htim->Instance == TIM6 )
	{
		tim_seconds++ ;
		if ( tim_seconds > TIM_SECONDS_THS_SYSTEM_RESET )
		  {
			  HAL_NVIC_SystemReset () ;
		  }
	}
}

void HAL_GPIO_EXTI_Rising_Callback ( uint16_t GPIO_Pin )
{
	if ( GPIO_Pin == 0x100 ) // LIS_INT1_EXTI8_Pin = 0x100
	{
		is_acc_int1_wkup_flag = true ;
		/*
		dbg_buff[0] = 0 ;
		sprintf ( dbg_buff , "INT on GPIO_Pin %04x detected!\n" , GPIO_Pin ) ;
		send_debug_logs ( dbg_buff ) ;
		*/
	}
	//zamiast poniższego może lepiej wrócić do pierwotnego przeglądania GPIO w trybie input bez przerwania
	if ( GPIO_Pin == 0x1000 ) // ASTRO_EVT_Pin = 0x1000
	{
		is_astro_evt_flag = true ;
	}
}
void HAL_RTC_AlarmAEventCallback ( RTC_HandleTypeDef *hrtc )
{
	is_rtc_alarm_a_flag = true ;
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
