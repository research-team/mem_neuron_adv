/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <time.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
uint8_t flags=0;
//total boards in chain
uint8_t board_cnt=3;
//current board number
uint8_t board_num=2;
//last spike time
int32_t last_spike=0;
//width of spike storage
uint8_t spike_wid=0;
uint8_t spike_wid_rst=0;
//if channel is excitatory
//bitwise info - all channels are excitatory
uint8_t ex_channels = 0xFF;
//GPIO info for each channel;
//signal level is Apin-Bpin
uint16_t Apin[8]={Q2_Pin,Q4_Pin,Q6_Pin,Q8_Pin,Q10_Pin,Q12_Pin,Q14_Pin,Q16_Pin};
uint16_t Bpin[8]={Q1_Pin,Q3_Pin,Q5_Pin,Q7_Pin,Q9_Pin,Q11_Pin,Q13_Pin,Q15_Pin};
GPIO_TypeDef* Aport[8]={Q2_GPIO_Port,Q4_GPIO_Port,Q6_GPIO_Port,Q8_GPIO_Port,Q10_GPIO_Port,Q12_GPIO_Port,Q14_GPIO_Port,Q16_GPIO_Port};
GPIO_TypeDef* Bport[8]={Q1_GPIO_Port,Q3_GPIO_Port,Q5_GPIO_Port,Q7_GPIO_Port,Q9_GPIO_Port,Q11_GPIO_Port,Q13_GPIO_Port,Q15_GPIO_Port};
//weight collection
int32_t last_spike_time[8]={0,0,0,0,0,0,0,0};
uint16_t weights[8]={0,0,0,0,0,0,0,0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI3_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin== GPIO_PIN_0) {
	  SET_BITN(flags,0);
  }
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	SET_BITN(flags,2);
}
void Hebb_weight_update(uint8_t last_spiked){
	//do some wired math
	//TODO:remake weight
	//use last spike times to do some magic
	uint8_t i=0;
	for(i=0;i<8;i++){
		if (weights[i]>0x03FF){
			weights[i]=0x03FF;
		}
	}
	uint8_t ls_tmp=last_spiked;
	uint8_t cs_data[1];
	uint8_t res_data_tx[2];
	uint8_t res_data_rx[2];
	cs_data[0]=~1;
	while(ls_tmp>0){
		//if this input spiked
		if((ls_tmp&0x01)==1){
			//CS for resistor
			HAL_GPIO_WritePin(CS_CS_GPIO_Port, CS_CS_Pin, GPIO_PIN_RESET);
			HAL_SPI_Transmit(&hspi3, cs_data, 1, 1000);
			HAL_GPIO_WritePin(CS_CS_GPIO_Port, CS_CS_Pin, GPIO_PIN_SET);

			//read datasheet carefully
			//0000 0101 0000 0000
			res_data_tx[0]=0x04|(weights[i]>>8);
			res_data_tx[1]=weights[i]&0xFF;
			HAL_SPI_TransmitReceive(&hspi1, res_data_tx,res_data_rx, 2, 1000);

			cs_data[0]=0xFF;
			HAL_GPIO_WritePin(CS_CS_GPIO_Port, CS_CS_Pin, GPIO_PIN_RESET);
			HAL_SPI_Transmit(&hspi3, cs_data, 1, 1000);
			HAL_GPIO_WritePin(CS_CS_GPIO_Port, CS_CS_Pin, GPIO_PIN_SET);
		}
		else{
			last_spike_time[i]+=last_spike;
			//no need for big times since hebb will be zero anyways
			if(last_spike_time[i]>10000){
				last_spike_time[i]=10000;
			}
		}
		cs_data[0]=~(1<<i);
		ls_tmp>>=1;
		i++;
	}

}
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
  MX_DMA_Init();
  MX_RTC_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_SPI3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //enable O1 and O2
  //0011 0001 0000 0000 0000 0000
  uint8_t data1[3]={0x31,0x00,0x03};
  uint8_t data2[3];
//  HAL_GPIO_WritePin(CSPOW_GPIO_Port, CSPOW_Pin, GPIO_PIN_RESET);
//  HAL_SPI_TransmitReceive(&hspi1, data1,data2, 3, 1000);
//  HAL_GPIO_WritePin(CSPOW_GPIO_Port, CSPOW_Pin, GPIO_PIN_SET);
//  HAL_Delay(1);

  //set trigger level once, never touch again and change mode for weight update
  data1[0]=0xB0;
  data1[1]=0x01;
  data1[2]=0x9A;
  HAL_GPIO_WritePin(CSPOW_GPIO_Port, CSPOW_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, data1,data2, 3, 1000);
  HAL_GPIO_WritePin(CSPOW_GPIO_Port, CSPOW_Pin, GPIO_PIN_SET);
  //change mode for resistor
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK){
	  Error_Handler();
  }

  uint8_t cs_data[1];
  cs_data[0]=0x00;
  HAL_GPIO_WritePin(CS_CS_GPIO_Port, CS_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi3, cs_data, 1, 1000);
  HAL_GPIO_WritePin(CS_CS_GPIO_Port, CS_CS_Pin, GPIO_PIN_SET);

  //read datasheet carefully
  //0000 0100 0000 0000
  uint8_t res_data_rx[2];
  uint8_t res_data_tx[2];
  res_data_tx[0]=0x04;
  res_data_tx[1]=0xFF;
  HAL_SPI_TransmitReceive(&hspi1, res_data_tx,res_data_rx, 2, 1000);

  cs_data[0]=0xFF;
  HAL_GPIO_WritePin(CS_CS_GPIO_Port, CS_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi3, cs_data, 1, 1000);
  HAL_GPIO_WritePin(CS_CS_GPIO_Port, CS_CS_Pin, GPIO_PIN_SET);


  HAL_GPIO_WritePin(GPIOC, LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin, GPIO_PIN_RESET);

  HAL_TIM_Base_Start_IT(&htim1);
  uint8_t rx_data[board_cnt];
  uint8_t tx_data[board_cnt];
  uint8_t data_tmp=0;
  uint8_t ext_info_tmp=0;
  uint8_t i=0;
  for(i=0;i<board_cnt;i++){
	  tx_data[i]=0;
	  rx_data[i]=0;
  }
  HAL_UART_Receive_DMA(&huart1,  rx_data, board_cnt);
//  if (board_num==0){
//	  tx_data[0]=0x55;
//	  tx_data[1]=0xAA;
//	  tx_data[2]=0xF5;
//  	  HAL_UART_Transmit_DMA(&huart2, tx_data, board_cnt);
//  }
  while (1)
  {
//	  HAL_GPIO_TogglePin(GPIOC, LED1_Pin);

	  //TIM1 (1ms) signal
	  if(READ_BITN(flags,1)==1){
		  RESET_BITN(flags,1);
		  HAL_GPIO_TogglePin(GPIOC, LED3_Pin);
	  }
	  //UART msg received
	  if(READ_BITN(flags,2)==1){
		  RESET_BITN(flags,2);
		  HAL_UART_IRQHandler(&huart2);
		  HAL_GPIO_TogglePin(GPIOC, LED4_Pin);
		  data_tmp=rx_data[board_num];
		  //copy info about input types
		  ext_info_tmp=ex_channels;
		  i=0;
		  while(data_tmp>0){
			  if((data_tmp&0x01)==1){
				  if((ext_info_tmp&0x01)==1){
					  //if is exitatory pull down b port
					  HAL_GPIO_WritePin(Bport[i], Bpin[i], GPIO_PIN_RESET);
				  }
				  else{
					  //if is inhibitory pull down a port
					  HAL_GPIO_WritePin(Aport[i], Apin[i], GPIO_PIN_RESET);
				  }
				  spike_wid=0;
				  spike_wid_rst=1;
			  }
			  i++;
			  ext_info_tmp>>=1;
			  data_tmp>>=1;
		  }

		  while(spike_wid<30){}
		  for(i=0;i<8;i++){
			  HAL_GPIO_WritePin(Aport[i], Apin[i], GPIO_PIN_SET);
		  	  HAL_GPIO_WritePin(Bport[i], Bpin[i], GPIO_PIN_SET);
		  }
		  //output signal generated
		  if(READ_BITN(flags,0)==1){
			  RESET_BITN(flags,0);
			  HAL_GPIO_TogglePin(GPIOC, LED2_Pin);
			  //need to update weight according to wired math and what spiked last time
			  //Hebb_weight_update(rx_data[board_num]);
			  tx_data[0]=rx_data[0];
			  tx_data[1]=rx_data[1];
			  //change last spike time to negative value for emulating refactory period
			  last_spike=-100;
		  }
		  HAL_UART_Transmit(&huart2, tx_data, board_cnt,1000);
		  for(i=0;i<board_cnt;i++){
		  	  tx_data[i]=0;
		  	  rx_data[i]=0;
		  }
		  HAL_UART_Receive_DMA(&huart1,  rx_data, board_cnt);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 13;
  RCC_OscInitStruct.PLL.PLLN = 195;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
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

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
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
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 120;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 100;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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
  huart1.Init.BaudRate = 230400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
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
  huart2.Init.BaudRate = 230400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin
                          |Q7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CSPOW_Pin|CS_CS_Pin|Q4_Pin|Q2_Pin
                          |Q1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Q8_Pin|Q12_Pin|Q10_Pin|Q9_Pin
                          |Q3_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Q6_Pin|Q5_Pin|Q15_Pin|Q16_Pin
                          |Q14_Pin|Q13_Pin|Q11_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin LED4_Pin
                           Q7_Pin Q8_Pin Q12_Pin Q10_Pin
                           Q9_Pin Q3_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin
                          |Q7_Pin|Q8_Pin|Q12_Pin|Q10_Pin
                          |Q9_Pin|Q3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : OUT_Pin */
  GPIO_InitStruct.Pin = OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CSPOW_Pin CS_CS_Pin Q4_Pin Q2_Pin
                           Q1_Pin */
  GPIO_InitStruct.Pin = CSPOW_Pin|CS_CS_Pin|Q4_Pin|Q2_Pin
                          |Q1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : Q6_Pin Q5_Pin Q15_Pin Q16_Pin
                           Q14_Pin Q13_Pin Q11_Pin */
  GPIO_InitStruct.Pin = Q6_Pin|Q5_Pin|Q15_Pin|Q16_Pin
                          |Q14_Pin|Q13_Pin|Q11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  else if (htim->Instance == TIM1) {
	  SET_BITN(flags,1);
	  last_spike+=1;
	  spike_wid+=1;
	  if(spike_wid_rst==1){
		  spike_wid_rst=0;
		  spike_wid=0;
	  }
    }
  /* USER CODE END Callback 1 */
}

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
