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
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#ifdef __GNUC__
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

#define DEBUG_MONITOR_BUFFER_SIZE 	32
#define DEBUG_MONITOR_ARGC_SIZE 	4

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */
osThreadId debugMonitorHandle;

osMessageQId debugMonitorQueueHandle;

/*** Static Variables ***/
static uint8_t s_bufferRx[DEBUG_MONITOR_BUFFER_SIZE];
//static uint8_t s_bufferTx[DEBUG_MONITOR_BUFFER_SIZE];
uint8_t s_bufferRxRp = 0, stopDefaultTask = 0;
uint32_t ProducerValue = 0, ConsumerValue = 0;
static char s_storedCommand[DEBUG_MONITOR_BUFFER_SIZE];
static uint32_t s_storedCommandIndex = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
void debugMonitorTask(void const * argument);
static uint32_t led(char *argv[], uint32_t argc);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
typedef struct {
  char* cmd;
  uint32_t (*func)(char *argv[], uint32_t argc);
} DEBUG_MON_COMMAND;

DEBUG_MON_COMMAND s_debugCommands[] = {
//  {"enc", enc},
//  {"ls", ls},
//  {"fatfs", fatfs},
  {"led",   led},
//  {"cap",   cap},
//  {"mode",  mode},
//  {"test1", test1},
//  {"test2", test2},
  {(void*)0, (void*)0}
};

/* USER CODE BEGIN 0 */
PUTCHAR_PROTOTYPE
{
	if(HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 10) != HAL_OK)
		return -1;
	return ch;
}
//
GETCHAR_PROTOTYPE
{
//	char data[4];
	uint8_t ch = 0; //, len = 1;
//
//	while(HAL_UART_Receive(&huart2, &ch, 1, 10) != HAL_OK)
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
////	HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 10);
	return ch;
}
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
  s_bufferRxRp = 0;

  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

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
  osMessageQDef(debugMonitorQueue, 16, uint16_t);
  debugMonitorQueueHandle = osMessageCreate(osMessageQ(debugMonitorQueue), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osThreadDef(debugMonitor, debugMonitorTask, osPriorityIdle, 0, 512);
  debugMonitorHandle = osThreadCreate(osThread(debugMonitor), NULL);
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
  //UART DMA 수신 데이터 메모리 연결해 주는 역할
  //idle 인터럽트 없이 데이터 수신할 경우
  //HAL_UART_Receive_DMA(&huart2, s_bufferRx, DEBUG_MONITOR_BUFFER_SIZE);
  //or
  //idle 인터럽트 통해서 데이터 수신할 경우
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, s_bufferRx, DEBUG_MONITOR_BUFFER_SIZE);

  //HaftComplte Interrupt 사용하지 않을 경우 비활성화
  __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
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
  HAL_GPIO_WritePin(GPIOE, LDO_3V3_PWR_ON_Pin|GATE_RUN_LED_R_Pin|GATE_RUN_LED_B_Pin|GATE_RUN_LED_G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LDO_3V3_PWR_ON_Pin GATE_RUN_LED_R_Pin GATE_RUN_LED_B_Pin GATE_RUN_LED_G_Pin */
  GPIO_InitStruct.Pin = LDO_3V3_PWR_ON_Pin|GATE_RUN_LED_R_Pin|GATE_RUN_LED_B_Pin|GATE_RUN_LED_G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

static uint32_t led(char *argv[], uint32_t argc)
{
	uint32_t onoff = atoi(argv[0]);

	printf("LED:%ld\r\n", onoff);

	if(onoff == 0){
		HAL_GPIO_WritePin(GATE_RUN_LED_R_GPIO_Port, GATE_RUN_LED_R_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GATE_RUN_LED_G_GPIO_Port, GATE_RUN_LED_G_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GATE_RUN_LED_B_GPIO_Port, GATE_RUN_LED_B_Pin, GPIO_PIN_RESET);
		stopDefaultTask = 1;
	}
	else{
		stopDefaultTask = 0;
	}
	return 1;
}

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
    	HAL_UART_Transmit(&huart2, d, 1, 10);
        //LL_USART_TransmitData8(USART3, *d);
        //while (!LL_USART_IsActiveFlag_TXE(&huart2)) {}
        while(!(READ_BIT(huart2.Instance->SR, USART_SR_TXE) == (USART_SR_TXE))){}
    }
    //while (!LL_USART_IsActiveFlag_TC(&huart2)) {}
    while(!(READ_BIT(huart2.Instance->SR, USART_SR_TC) == (USART_SR_TC))){}
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
	if(((DEBUG_MONITOR_BUFFER_SIZE -__HAL_DMA_GET_COUNTER(&hdma_usart2_rx))&(DEBUG_MONITOR_BUFFER_SIZE-1)) != s_bufferRxRp){
		HAL_UART_Transmit(&huart2, &s_bufferRx[s_bufferRxRp], 1, 10);

		/* check if one line is done */
		if (s_bufferRx[s_bufferRxRp] == '\n' || s_bufferRx[s_bufferRxRp] == '\r' || s_bufferRxRp == DEBUG_MONITOR_BUFFER_SIZE-1) {
			osMessagePut(debugMonitorQueueHandle, ++ProducerValue, 100);
		}
		s_bufferRxRp++;
		s_bufferRxRp &= (DEBUG_MONITOR_BUFFER_SIZE - 1);
	}
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	printf_("HT");
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	printf_("CP");
}

void debugMonitorShow()
{
	printf("Command List");

	for(uint32_t i = 0; s_debugCommands[i].cmd != (void*)0; i++){
		printf("%s\r\n", s_debugCommands[i].cmd);
	}
}

void debugMonitorTask(void const * argument)
{
	osEvent event;

	printf("debugMonitorTask\r\n");

	/* Infinite loop */
	for(;;)
	{
	    /* Get the message from the queue */
	    event = osMessageGet(debugMonitorQueueHandle, osWaitForever);

	    if(event.status == osEventMessage){
	    	if(event.value.v != ConsumerValue){
	    		/* Catch-up. */
	            ConsumerValue = event.value.v;

	            /* Simply call processing function */
	            memcpy(s_storedCommand, s_bufferRx, s_bufferRxRp);
	            s_storedCommandIndex = s_bufferRxRp;

	            /* split input command */
				char *argv[DEBUG_MONITOR_ARGC_SIZE] = {0};
				uint32_t argc = 0;

				argv[argc++] = &s_storedCommand[0];

				for(uint32_t i = 2; i <= s_storedCommandIndex; i++){
					if(s_storedCommand[i] == '\r' || s_storedCommand[i] == '\n'){
						s_storedCommand[i] = '\0';
						break;
					}
					if(s_storedCommand[i] == ' '){
						s_storedCommand[i] = '\0';
						argv[argc++] = &s_storedCommand[i+1];

						if(argc == DEBUG_MONITOR_ARGC_SIZE){
							break;
						}
					}
				}

				/* call corresponding debug command */
				uint32_t ret = 0;
				for (uint32_t i = 0; s_debugCommands[i].cmd != (void*)0; i++) {
					if (strcmp(s_debugCommands[i].cmd, argv[0]) == 0) {
						ret = s_debugCommands[i].func(&argv[1], argc-1);
//						printf(">");
					}
				}

				if(ret != 1){
					debugMonitorShow();
				}

				s_storedCommandIndex = 0;
				memset(s_storedCommand, '0', DEBUG_MONITOR_BUFFER_SIZE);
	    	}
	    	else{
	    		/* Increment the value we expect to remove from the queue next time round. */
	    		++ConsumerValue;
	    		printf_("CV+");
	    	}
	    }
		osDelay(1);
	}
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
	printf("StartDefaultTask\r\n");
	HAL_GPIO_WritePin(LDO_3V3_PWR_ON_GPIO_Port, LDO_3V3_PWR_ON_Pin, GPIO_PIN_SET);
  /* Infinite loop */
  for(;;)
  {
	  if(!stopDefaultTask){
		  HAL_GPIO_WritePin(GATE_RUN_LED_R_GPIO_Port, GATE_RUN_LED_R_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GATE_RUN_LED_G_GPIO_Port, GATE_RUN_LED_G_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GATE_RUN_LED_B_GPIO_Port, GATE_RUN_LED_B_Pin, GPIO_PIN_RESET);
		  osDelay(500);
		  HAL_GPIO_WritePin(GATE_RUN_LED_R_GPIO_Port, GATE_RUN_LED_R_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GATE_RUN_LED_G_GPIO_Port, GATE_RUN_LED_G_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(GATE_RUN_LED_B_GPIO_Port, GATE_RUN_LED_B_Pin, GPIO_PIN_RESET);
		  osDelay(500);
		  HAL_GPIO_WritePin(GATE_RUN_LED_R_GPIO_Port, GATE_RUN_LED_R_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GATE_RUN_LED_G_GPIO_Port, GATE_RUN_LED_G_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(GATE_RUN_LED_B_GPIO_Port, GATE_RUN_LED_B_Pin, GPIO_PIN_SET);
		  osDelay(500);
	  }

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
