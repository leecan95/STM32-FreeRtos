/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
#include "math.h"
#include "limits.h"
/* USER CODE BEGIN Includes */
//Register
#define LSM303_ACC_ADDRESS (0x19 << 1) // address of accelerometer: 0011 001x
#define LSM303_ACC_CTRL_REG1_A 0x20 // settings register 1
#define LSM303_ACC_CTRL_REG3_A 0x22 // setting register 3
#define LSM303_ACC_Z_H_A 0x2D // higher byte of Z axis data 
#define LSM303_ACC_Z_L_A 0x2C // lower byte of Z axis data
#define LSM303_ACC_X_L_A 0x28 // lower byte of X axis data

// to read more data at once
#define LSM303_ACC_Z_L_A_MULTI_READ (LSM303_ACC_Z_L_A | 0x80)

// to read more data at once
#define LSM303_ACC_X_L_A_MULTI_READ (LSM303_ACC_X_L_A | 0x80)

// Bit masks
// CTRL_REG1_A = [ODR3][ODR2][ODR1][ODR0][LPEN][ZEN][YEN][XEN]
#define LSM303_ACC_Z_ENABLE 0x04 // 0000 0100
#define LSM303_ACC_XYZ_ENABLE 0x07 // 0000 0111
#define LSM303_ACC_100HZ 0x50 //0101 0000
#define LSM303_ACC_1HZ 0x10 //0001 0000

// CTRL_REG3_A = [CLICK][AOI1][AOI2][DRDY_1][DRDY_2][WTM][OVERRUN][---]
#define LSM303_ACC_I1_DRDY1 0x10 //0001 0000
#define LSM303_ACC_I1_DRDY2 0x08 //0000 1000

#define LSM303_ACC_RESOLUTION 2.0 // Maximum value of measurable acceleration [g]

// Variables
uint8_t Data[6]; // Variable for direct data reading from the accelerometer
int16_t Xaxis = 0; // Contains a transformed form of data read from the X axis
int16_t Yaxis = 0; // Contains a transformed form of data read from the Y axis
int16_t Zaxis = 0; // Contains a transformed form of data read from the Z axis

float Xaxis_g = 0 ; // Contains the acceleration in the X axis, expanded to physical unit [g]
float Yaxis_g = 0; // Contains the acceleration in the Y axis, expanded to physical unit [g]
float Zaxis_g = 0; // Contains the acceleration in the Z axis, expanded to physical unit [g]

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
I2C_HandleTypeDef hi2c1;

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityHigh, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityNormal, 0, 128);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
int falldetect(void)
{
	float at = 0;
	HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_X_L_A_MULTI_READ, 1, Data, 6, 100);

	// Convert received data bytes to int16_t type
	Xaxis = ((Data[1] << 8) | Data[0]);
	Yaxis = ((Data[3] << 8) | Data[2]);
	Zaxis = ((Data[5] << 8) | Data[4]);

	// calculation of acceleration in each axis in the SI unit [g]
	Xaxis_g = ((float) Xaxis * LSM303_ACC_RESOLUTION) / (float) INT16_MAX;
	Yaxis_g = ((float) Yaxis * LSM303_ACC_RESOLUTION) / (float) INT16_MAX;
	Zaxis_g = ((float) Zaxis * LSM303_ACC_RESOLUTION) / (float) INT16_MAX;
  at = sqrt(Xaxis_g*Xaxis_g + Yaxis_g*Yaxis_g + Zaxis_g*Zaxis_g);
	if (at < 0.7)
		return 1;
	else 
		return 0;
}
void StartDefaultTask(void const * argument)
{
      // Fill the configuration variable with the appropriate options
	uint8_t Settings = LSM303_ACC_XYZ_ENABLE | LSM303_ACC_100HZ;
	
  // Enter the configuration in the accelerometer register
	HAL_I2C_Mem_Write(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_CTRL_REG1_A, 1, &Settings, 1, 100);

	Settings = LSM303_ACC_I1_DRDY2;
	HAL_I2C_Mem_Write(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_CTRL_REG3_A, 1, &Settings, 1, 100);  

  /* USER CODE BEGIN 5 */
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
		falldetect();
    osDelay(10);
  }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
	int i = 0;
  /* Infinite loop */
  for(;;)
  {
		while(falldetect() == 1) {
		for	(i = 0;i < 150;i++) {
		HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
    osDelay(50);
		}
		}
		while(falldetect() == 0) {
			HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
      osDelay(250);
			if (falldetect() == 1)
				break;
			
			osDelay(250);
			if (falldetect() == 1)
				break;
			HAL_GPIO_TogglePin(GPIOD,GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
			osDelay(250);
			if (falldetect() == 1)
			break;
			osDelay(250);
			if (falldetect() == 1)
				break;
		}
  }
  /* USER CODE END StartTask02 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM9 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM9) {
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
