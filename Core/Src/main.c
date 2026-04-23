/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
typedef struct
{
    uint8_t servo_id;
    int8_t direction;
} ServoCmd_t;
uint8_t rx_data;
uint8_t system_state = 0;
uint8_t press_count = 0;
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
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* Definitions for SERVOTask */
osThreadId_t SERVOTaskHandle;
const osThreadAttr_t SERVOTask_attributes = {
  .name = "SERVOTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MyCmds */
osMessageQueueId_t MyCmdsHandle;
const osMessageQueueAttr_t MyCmds_attributes = {
  .name = "MyCmds"
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
void StartSERVOTask(void *argument);

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
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
   HAL_UART_Receive_IT(&huart2, &rx_data, 1);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of MyCmds */
  MyCmdsHandle = osMessageQueueNew (5, sizeof(ServoCmd_t), &MyCmds_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of SERVOTask */
  SERVOTaskHandle = osThreadNew(StartSERVOTask, NULL, &SERVOTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 84-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 20000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t last_press = 0;

    if(GPIO_Pin == GPIO_PIN_13)
    {
        if(HAL_GetTick() - last_press < 200) return;
        last_press = HAL_GetTick();

        press_count++;

        if(press_count == 1)
        {
            system_state = 1; // ON
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // LED ON
        }
        else if(press_count == 2)
        {
            system_state = 0; // OFF
            press_count = 0;
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // LED OFF
        }
     }
}
/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	   ServoCmd_t cmd;

	    if(huart->Instance == USART2)
	    {
	        if(system_state == 0)
	        {
	            HAL_UART_Receive_IT(&huart2, &rx_data, 1);
	            return;
	        }

	        if(rx_data == 'A') { cmd.servo_id = 1; cmd.direction = 1; }
	        else if(rx_data == 'B') { cmd.servo_id = 1; cmd.direction = -1; }
	        else if(rx_data == 'C') { cmd.servo_id = 2; cmd.direction = 1; }
	        else if(rx_data == 'D') { cmd.servo_id = 2; cmd.direction = -1; }
	        else
	        {
	            HAL_UART_Receive_IT(&huart2, &rx_data, 1);
	            return;
	        }

	        osMessageQueuePut(MyCmdsHandle, &cmd, 0, 0);   // ✅ FIXED

	        HAL_UART_Receive_IT(&huart2, &rx_data, 1);
	    }
	}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartSERVOTask */
/**
  * @brief  Function implementing the SERVOTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartSERVOTask */
void StartSERVOTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
	  ServoCmd_t cmd;

	    uint16_t current1 = 1500;
	    uint16_t current2 = 1500;

	    int8_t dir1 = 0;
	    int8_t dir2 = 0;

	    for(;;)
	    {
	        if(osMessageQueueGet(MyCmdsHandle, &cmd, NULL, 0) == osOK)   // ✅ FIXED
	        {
	            if(cmd.servo_id == 1) dir1 = cmd.direction;
	            else if(cmd.servo_id == 2) dir2 = cmd.direction;
	        }

	        if(system_state == 1)
	        {
	            if(dir1 == 1 && current1 < 2500) current1 += 10;
	            else if(dir1 == -1 && current1 > 500) current1 -= 10;

	            if(dir2 == 1 && current2 < 2500) current2 += 10;
	            else if(dir2 == -1 && current2 > 500) current2 -= 10;
	        }

	        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, current1);
	        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, current2);

	        osDelay(20);


	    }
	    /* USER CODE END 5 */
	  }

	  /**
	    * @brief  Period elapsed callback in non blocking mode
	    * @note   This function is called  when TIM6 interrupt took place, inside
	    * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
	    * a global variable "uwTick" used as application time base.
	    * @param  htim : TIM handle
	    * @retval None
	    */
	  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
	  {
	    /* USER CODE BEGIN Callback 0 */

	    /* USER CODE END Callback 0 */
	    if (htim->Instance == TIM6)
	    {
	      HAL_IncTick();
	    }
	    /* USER CODE BEGIN Callback 1 */

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
	  #ifdef USE_FULL_ASSERT
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
