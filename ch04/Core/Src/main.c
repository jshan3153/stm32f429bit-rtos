/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
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
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */
osThreadId DebugMonitorHandle;

#define BUFFER_SIZE 16

/*** Static Variables ***/
static UART_HandleTypeDef *sp_huart;
static volatile uint8_t s_bufferRx[BUFFER_SIZE];
static volatile uint8_t s_bufferTx[BUFFER_SIZE];
uint8_t s_bufferRxRp = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
	if(HAL_UART_Transmit(sp_huart, (uint8_t *)&ch, 1, 10) != HAL_OK)
		return -1;
	return ch;
}
//
//int __io_getchar(void)
//{
//	char data[4];
//	uint8_t ch, len = 1;
//
//	while(HAL_UART_Receive(sp_huart, &ch, 1, 10) != HAL_OK)
//	{
//	}
//
//	memset(data, 0x00, 4);
//	switch(ch)
//	{
//		case '\r':
//		case '\n':
//			len = 2;
//			sprintf(data, "\r\n");
//			break;
//
//		case '\b':
//		case 0x7F:
//			len = 3;
//			sprintf(data, "\b \b");
//			break;
//
//		default:
//			data[0] = ch;
//			break;
//	}
////	HAL_UART_Transmit(sp_huart, (uint8_t *)data, len, 10);
//	return ch;
//}
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  sp_huart = &huart2;
  //HAL_UART_Receive(sp_huart, &uart2RxData, 1, 100);
  s_bufferRxRp = 0;

  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  HAL_UART_Receive_DMA(sp_huart, s_bufferRx, BUFFER_SIZE);

  printf("stm32f429bit-rtos\r\n");
//
//  printf("Enter Ch:");
//  scanf("%c", &ch);
//  printf("%c\r\n", ch);

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
//  osThreadDef(DebugMonitor, debugMonitor_task, osPriorityIdle, 0, 512);
//  DebugMonitorHandle = osThreadCreate(osThread(DebugMonitor), NULL);
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LDO_3V3_PWR_ON_Pin|GATE_RUN_LED_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LDO_3V3_PWR_ON_Pin GATE_RUN_LED_R_Pin */
  GPIO_InitStruct.Pin = LDO_3V3_PWR_ON_Pin|GATE_RUN_LED_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void usart_process_data(const void* data, size_t len)
{
    const uint8_t* d = data;

    /*
     * This function is called on DMA TC or HT events, and on UART IDLE (if enabled) event.
     *
     * For the sake of this example, function does a loop-back data over UART in polling mode.
     * Check ringbuff RX-based example for implementation with TX & RX DMA transfer.
     */

    for (; len > 0; --len, ++d) {
    	HAL_UART_Transmit(sp_huart, d, 1, 10);
        //LL_USART_TransmitData8(USART3, *d);
        //while (!LL_USART_IsActiveFlag_TXE(sp_huart)) {}
        while(!(READ_BIT(sp_huart->Instance->SR, USART_SR_TXE) == (USART_SR_TXE))){}
    }
    //while (!LL_USART_IsActiveFlag_TC(sp_huart)) {}
    while(!(READ_BIT(sp_huart->Instance->SR, USART_SR_TC) == (USART_SR_TC))){}
}

/**
 * \brief           Send string to USART
 * \param[in]       str: String to send
 */
void printf_(const char* str) {
    usart_process_data(str, strlen(str));
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	printf_("EC");
}
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	printf_("HT");
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	printf_("CP");
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
	char ch = 0;
	uint8_t bufferRxWp = 0;
	printf("StartDefaultTask\r\n");
	HAL_GPIO_WritePin(LDO_3V3_PWR_ON_GPIO_Port, LDO_3V3_PWR_ON_Pin, GPIO_PIN_SET);
  /* Infinite loop */
  for(;;)
  {
	  bufferRxWp = (BUFFER_SIZE - hdma_usart2_rx.Instance->NDTR)&(BUFFER_SIZE-1);

	  if(bufferRxWp != s_bufferRxRp){
		  ch = s_bufferRx[s_bufferRxRp++];
		  HAL_UART_Transmit(sp_huart, &ch, 1, 10);
		  if(s_bufferRxRp>BUFFER_SIZE)
			  s_bufferRxRp = 0;
	  }


	osDelay(1);
//	HAL_GPIO_WritePin(GATE_RUN_LED_R_GPIO_Port, GATE_RUN_LED_R_Pin, GPIO_PIN_SET);
//	osDelay(1000);
//	HAL_GPIO_WritePin(GATE_RUN_LED_R_GPIO_Port, GATE_RUN_LED_R_Pin, GPIO_PIN_RESET);
  }
  /* USER CODE END 5 */
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
