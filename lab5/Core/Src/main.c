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
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
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

/* USER CODE BEGIN PV */
uint8_t RxBuffer[2];
uint8_t TxBuffer[100];

uint8_t Main[500] =
				"|-----------------------------------------------------|\r\n"
				"|                    Welcome to Menu                  |\r\n"
				"|               Press the number 1 or 0               |\r\n"
				"|-----------------------------------------------------|\r\n"
				"|<<<<<<<<<<<<<<< 0 for LED Control  >>>>>>>>>>>>>>>>>>|\r\n"
				"|<<<<<<<<<<<<<<< 1 for Check Button >>>>>>>>>>>>>>>>>>|\r\n"
				"|-----------------------------------------------------|\r\n";
uint8_t PageLED[700] =
				"|-----------------------------------------------------|\r\n"
				"|               Welcome to Led control                |\r\n"
		        "|               Select a,s,d,x for menu               |\r\n"
				"|-----------------------------------------------------|\r\n"
				"|                a : Speed +1 Hz                      |\r\n"
				"|                s : Speed -1 Hz                      |\r\n"
				"|                d : On / Off                         |\r\n"
		        "|                x : Go to Menu                       |\r\n"
				"|-----------------------------------------------------|\r\n";

uint8_t PageButton[300] =
				"|-----------------------------------------------------|\r\n"
				"|              Welcome to Check Button                |\r\n"
				"|         Press 'x' to get back to Menu               |\r\n"
				"|-----------------------------------------------------|\r\n";

int8_t Hz = 0;
int8_t BefHz = 0;
uint16_t mils = 0;
uint8_t PageState = 0;
uint8_t LedNow = 0;
uint8_t Button = 0;
uint8_t PreButton = 0;
uint8_t Press[20] = "ButtonPressed\r\n";
uint8_t unPress[20] = "ButtonUnpressed\r\n";

uint8_t OneTimePress = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

void UARTInterruptConfig();
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
  /* USER CODE BEGIN 2 */

	UARTInterruptConfig();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		static uint32_t timestamp = 0;
		if (HAL_GetTick() > timestamp) {

			if (Hz > 0) {
				mils = 500 / Hz;
			} else {
				mils = 0;
			}
			timestamp = HAL_GetTick() + mils;

			if(LedNow == 0 || Hz == 0){
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, RESET);
			}
			else{
				HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			}
		}

		switch (PageState) {

		case 0:

			HAL_UART_Transmit_IT(&huart2, Main, strlen((char*) Main));
			PageState = 1;
			break;
		case 1:
			if(RxBuffer[0] == '0'){
				RxBuffer[0] = 0;
				PageState = 2;
			}
			else if(RxBuffer[0] == '1'){
				RxBuffer[0] = 0;
				PageState = 4;
			}
			break;


		case 2:

			HAL_UART_Transmit_IT(&huart2, PageLED, strlen((char*) PageLED));
			PageState = 3;
			break;
		case 3:
			if (RxBuffer[0] == 'a') {
				Hz += 1;
				BefHz += 1;

				sprintf((char*) TxBuffer, " BlinkHz %d\r\n", Hz);
				HAL_UART_Transmit_IT(&huart2, TxBuffer, strlen((char*) TxBuffer));
				PageState = 3;
				RxBuffer[0] = ' ';
			}
			else if (RxBuffer[0] == 's') {
				Hz -= 1;
				if(Hz <= 0){
					Hz = 0;
				}
				BefHz -= 1;
				if(BefHz <= 0){
					BefHz = 0;
				}
				sprintf((char*) TxBuffer, " BlinkHz %d\r\n", BefHz);
				HAL_UART_Transmit_IT(&huart2, TxBuffer, strlen((char*) TxBuffer));
				PageState = 3;
				RxBuffer[0] = ' ';

			}
			else if(RxBuffer[0] == 'd'){
				if(LedNow){
					LedNow = 0;
					RxBuffer[0] = ' ';
					Hz = 0;
					sprintf((char*) TxBuffer, " LED OFF \r\n");
					HAL_UART_Transmit_IT(&huart2, TxBuffer, strlen((char*) TxBuffer));
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, SET);
				}
				else{
					LedNow = 1;
					RxBuffer[0] = ' ';
					Hz = BefHz;
					sprintf((char*) TxBuffer, " LED ON \r\n");
					HAL_UART_Transmit_IT(&huart2, TxBuffer, strlen((char*) TxBuffer));
				}
			}
			else if(RxBuffer[0] == 'x'){
				RxBuffer[0] = 0;
				PageState = 0;
			}
			break;


		case 4:

			HAL_UART_Transmit_IT(&huart2, PageButton, strlen((char*) PageButton));
			PageState = 5;
			break;
		case 5:
			Button = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
			if(Button == 1 && PreButton == 0){
				HAL_UART_Transmit_IT(&huart2, unPress, strlen((char*) unPress));
				OneTimePress = 0;
			}
			else if(Button == 0 && PreButton == 0){
				if(!OneTimePress){
					HAL_UART_Transmit_IT(&huart2, Press, strlen((char*) Press));
					OneTimePress = 1;
				}
			}
			else if (RxBuffer[0] == 'x'){
				RxBuffer[0] = 0;
				PageState = 0;
			} /*
			else if(RxBuffer[0] != 0){
				HAL_Delay(5);
				HAL_UART_Transmit_IT(&huart2, wrong, strlen((char*) wrong));
				RxBuffer[0] = 0;
				PageState = 5;
			} */

			PreButton = Button;
			break;
		}

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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  huart2.Init.BaudRate = 230400;
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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void UARTInterruptConfig() {
	HAL_UART_Receive_IT(&huart2, RxBuffer, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == &huart2) {
		RxBuffer[1] = '\0';

		sprintf((char*) TxBuffer, "Enter : %s\r\n", RxBuffer);
		HAL_UART_Transmit(&huart2, TxBuffer, strlen((char*) TxBuffer), 100);

		HAL_UART_Receive_IT(&huart2, RxBuffer, 1);
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
	while (1) {
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
