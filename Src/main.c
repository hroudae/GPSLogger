/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "fatfs.h"


#define redLED 6
#define blueLED 7
#define orangeLED 8
#define greenLED 9


void SystemClock_Config(void);

void configLED(uint8_t x);
void toggleLED(uint8_t x);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_FATFS_Init();

  RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable GPIOC clock in RCC for LEDS

  configLED(redLED); 
	configLED(blueLED);

  GPIOC->BSRR = (1 << redLED); // Start high
	GPIOC->BRR = (1 << blueLED); // Start low

  while (1) {
    //LEDs toggle every 200ms
		HAL_Delay(200); // Delay 200ms
	
		// Toggle the output state
		toggleLED(redLED);
		toggleLED(blueLED);
  }
}

/*
 * Generic LED configuration function
 * Pass in the pin number, x, of the LED on PCx
 * Configures pin to general-pupose output mode, push-pull output,
 * low-speed, and no pull-up/down resistors
 */
void configLED(uint8_t x) {
	// Set General Pupose Output
	GPIOC->MODER |= (1 << (2*x));
	GPIOC->MODER &= ~(1 << ((2*x)+1));
	// Set to Push-pull
	GPIOC->OTYPER &= ~(1 << x);
	// Set to Low speed
	GPIOC->OSPEEDR &= ~((1 << (2*x)) | (1 << ((2*x)+1)));
	// Set to no pull-up/down
	GPIOC->PUPDR &= ~((1 << (2*x)) | (1 << ((2*x)+1)));
}

/*
 * Toogle LED at PCx
 */
void toggleLED(uint8_t x) {
	if (GPIOC->ODR & (1 << x)) // PCx is high
			GPIOC->BRR = (1 << x);
		else	// PCx is low
			GPIOC->BSRR = (1 << x);
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
