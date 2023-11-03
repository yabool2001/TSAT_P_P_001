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
#define UART_TIMEOUT 					1000
#define NMEA_3D_FIX						'3'
#define NMEA_MESSAGE_SIZE				250
#define TIM_SECONDS_THS_SYSTEM_RESET	3600
#define ASTRO_MESSAGE_TIMER				60000

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
char* 		hello = "Hello TSAT_P_P_001\n" ;
char		rtc_dt[20] ;

// Lx6
uint8_t		rxd_byte = 0 ;
uint8_t		nmea_message[NMEA_MESSAGE_SIZE] ;
uint8_t		gngll_message[NMEA_MESSAGE_SIZE] ;
uint8_t		i_nmea = 0 ;
char* 		nmea_gngsa_label = "GNGSA" ;
char* 		nmea_gngll_label = "GNGLL" ;
char* 		nmea_rmc_label = "RMC" ;
char 		nmea_coordinates_log[52] ; // Nagłowek + 12 + ew. znak minus + '\0'
char 		nmea_latitude_s[12] ; // 10 + ew. znak minus + '\0'
char 		nmea_longitude_s[12] ; // 10 + ew. znak minus + '\0'
int32_t		astro_geo_wr_latitude ;
int32_t		astro_geo_wr_longitude ;
double		nmea_pdop_ths = 5.1 ;
uint16_t	nmea_max_rmc_time = 60 ;
uint16_t	nmea_max_active_time = 480 ; // Powinien być ten sam typ co tim_seconds // 240: 4 min.,
char		nmea_fixed_mode_s ;
double 		nmea_fixed_pdop_d = 1000.0 ;

// TIM
uint16_t	tim_seconds = 0 ; // Powinien być ten sam typ co nmea_max_active_time
uint32_t	agg_tim_seconds = 0 ;

// Astrocast
uint32_t	astro_log_loop_timer = 0 ;
uint16_t	g_payload_id_counter = 0 ;
char		payload[ASTRONODE_APP_PAYLOAD_MAX_LEN_BYTES] = {0}; // 160 bajtów
char 		astro_payload_log[ASTRONODE_APP_PAYLOAD_MAX_LEN_BYTES+21] ; // Nagłowek + 12 + ew. znak minus + '\0'
// Flags
bool 		seek_fix_loop_flag = false ;
bool		received_nmea_rmc_flag = false ;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM6_Init(void);
static void MX_RTC_Init(void);
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
  MX_I2C1_Init();
  MX_TIM6_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Transmit ( &huart2 , (uint8_t*) hello , strlen (hello) , UART_TIMEOUT ) ;
  if ( !my_astro_init () )
  {
	  HAL_NVIC_SystemReset () ;
  }

  my_lx6_on () ;
  astro_geo_wr_latitude = 0 ;
  astro_geo_wr_longitude = 0 ;
  nmea_latitude_s[0] = 0 ;
  nmea_longitude_s[0] = 0 ;
  gngll_message[0] = 0 ;
  nmea_fixed_pdop_d = 1000.0 ;
  received_nmea_rmc_flag = false ;
  tim_seconds = 0 ;
  HAL_TIM_Base_Start_IT ( &htim6 ) ;
  while ( tim_seconds < nmea_max_active_time ) // 1200 = 10 min.
  {
	  HAL_UART_Receive ( HUART_Lx6 , &rxd_byte , 1 , UART_TIMEOUT ) ;
	  //HAL_UART_Receive ( HUART_DBG , &rxd_byte , 1 , UART_TIMEOUT ) ; // Receive nmea from DBG "sim_nmea_uart" python script
	  //HAL_UART_Transmit ( HUART_DBG , &rxd_byte , 1 , UART_TIMEOUT ) ; // Transmit all nmea to DBG
	  if ( rxd_byte )
	  {
		  if ( my_nmea_message ( &rxd_byte , nmea_message , &i_nmea ) == 2 )
		  {
			  if ( is_my_nmea_checksum_ok ( (char*) nmea_message ) )
			  {
				  if ( strstr ( (char*) nmea_message , nmea_rmc_label ) && !received_nmea_rmc_flag )
				  {
					  set_my_rtc_from_nmea_rmc ( (char*) nmea_message ) ;
					  received_nmea_rmc_flag = true ;
				  }
				  if ( strstr ( (char*) nmea_message , nmea_gngsa_label ) )
				  {
					  nmea_fixed_mode_s = get_my_nmea_gngsa_fixed_mode_s ( (char*) nmea_message ) ;
					  nmea_fixed_pdop_d = get_my_nmea_gngsa_pdop_d ( (char*) nmea_message ) ;
				  }
				  if ( strstr ( (char*) nmea_message , nmea_gngll_label ) /*&& nmea_fixed_pdop_d <= nmea_pdop_ths */)
				  {
					  if ( nmea_fixed_pdop_d <= nmea_pdop_ths )
					  {
						  get_my_nmea_gngll_coordinates ( (char*) nmea_message , nmea_latitude_s , nmea_longitude_s , &astro_geo_wr_latitude , &astro_geo_wr_longitude ) ; // Nie musze nic kombinować z przenoszeniem tej operacji, bo po niej nie będzie już dalej odbierania wiadomości tylko wyjście
					  }
					  else
					  {
						  memcpy ( gngll_message , nmea_message , NMEA_MESSAGE_SIZE ) ; // Zapisuję, żeby potem, jak nie osiągnę jakości nmea_pdop_ths to wykorzystać coordinates do payload
					  }
				  }
			  }
		  }
	  }
	  rxd_byte = 0 ;
	  if ( tim_seconds > nmea_max_rmc_time && !received_nmea_rmc_flag )
	  {
		  break ;
	  }
	  if ( nmea_fixed_pdop_d <= nmea_pdop_ths )
	  {
		  if ( nmea_latitude_s[0] != 0 )
		  {
			  if ( nmea_fixed_mode_s == NMEA_3D_FIX )
			  {
				  if ( received_nmea_rmc_flag )
				  {
					  break ;
				  }
			  }
		  }
	  }
  }
  HAL_TIM_Base_Stop_IT ( &htim6 ) ;
  my_lx6_off () ;
  if ( nmea_latitude_s[0] == 0 && gngll_message[0] != 0 ) // Jeśli nie masz współrzędnych pdop to wykorzystaja gorsze i zrób ich backup
  {
	  get_my_nmea_gngll_coordinates ( (char*) gngll_message , nmea_latitude_s , nmea_longitude_s , &astro_geo_wr_latitude , &astro_geo_wr_longitude ) ;
  }
  get_my_rtc_time ( rtc_dt ) ;
  send_debug_logs ( rtc_dt ) ;
  sprintf ( payload , "%.1f,%d,%lu" , nmea_fixed_pdop_d , tim_seconds , agg_tim_seconds ) ;
  sprintf ( astro_payload_log , "Astronode payload: %s" , payload ) ;
  sprintf ( nmea_coordinates_log , "NMEA coordinates: %s,%s" , nmea_latitude_s , nmea_longitude_s ) ;
  my_astro_write_coordinates ( astro_geo_wr_latitude , astro_geo_wr_longitude ) ;
  send_debug_logs ( astro_payload_log ) ;
  send_debug_logs ( nmea_coordinates_log ) ;
  my_astro_add_payload_2_queue ( payload ) ;
  //HAL_PWR_EnterSTOPMode ( PWR_LOWPOWERREGULATOR_ON , PWR_STOPENTRY_WFE ) ;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  astro_log_loop_timer = get_systick () ;
  while (1)
  {
	  if ( is_evt_pin_high() )
	  {
		  my_astro_read_evt_reg () ;
	  }
	  if ( get_systick () - astro_log_loop_timer >  ASTRO_MESSAGE_TIMER )
	  {
		  my_astro_log ();
		  astro_log_loop_timer = get_systick () ;
		  astronode_send_pld_er ( g_payload_id_counter , payload , strlen ( payload ) ) ;
	  }

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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00303D5B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.SubSeconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x24;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  HAL_GPIO_WritePin(GPIOC, L86_RST_Pin|ASTRO_RST_Pin|L86_PWR_SW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LDG_Pin|ASTRO_PWR_SW_Pin|ASTRO_RSTA11_Pin|ASTRO_EVENT_Pin
                          |ASTRO_WAKEUP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : L86_RST_Pin ASTRO_RST_Pin L86_PWR_SW_Pin */
  GPIO_InitStruct.Pin = L86_RST_Pin|ASTRO_RST_Pin|L86_PWR_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LDG_Pin ASTRO_PWR_SW_Pin ASTRO_RSTA11_Pin ASTRO_EVENT_Pin
                           ASTRO_WAKEUP_Pin */
  GPIO_InitStruct.Pin = LDG_Pin|ASTRO_PWR_SW_Pin|ASTRO_RSTA11_Pin|ASTRO_EVENT_Pin
                          |ASTRO_WAKEUP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LIS_INT1_EXTI8_Pin LIS_INT2_EXTI9_Pin */
  GPIO_InitStruct.Pin = LIS_INT1_EXTI8_Pin|LIS_INT2_EXTI9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
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
	return ( HAL_GPIO_ReadPin ( GPIOA , ASTRO_EVENT_Pin ) == GPIO_PIN_SET ? true : false);
}
void astro_manage_evt ( void )
{
	send_debug_logs ( "Evt pin is high." ) ;
	astronode_send_evt_rr () ;
	if (is_sak_available () )
	{
		astronode_send_sak_rr () ;
		astronode_send_sak_cr () ;
		send_debug_logs ( "Message has been acknowledged." ) ;
		astronode_send_per_rr () ;
	}
	if ( is_astronode_reset () )
	{
		send_debug_logs ( "Terminal has been reset." ) ;
		astronode_send_res_cr () ;
	}
	if ( is_command_available () )
	{
		send_debug_logs ( "Unicast command is available" ) ;
		astronode_send_cmd_rr () ;
		astronode_send_cmd_cr () ;
	}
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
void my_astro_on ( void )
{
	HAL_GPIO_WritePin ( GPIOA , ASTRO_PWR_SW_Pin , GPIO_PIN_SET ) ;
	MX_USART1_UART_Init () ;
}
void my_astro_off ( void )
{
	HAL_GPIO_WritePin ( GPIOA , ASTRO_PWR_SW_Pin , GPIO_PIN_RESET ) ;
	HAL_UART_DeInit ( HUART_ASTRO ) ;
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
