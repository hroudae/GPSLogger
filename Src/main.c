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

#include "main.h"
#include "utilities.h"
#include "lcd.h"
#include "openlog.h"
#include "fatfs.h"


// SPI Pins for LCD (SPI2)
#define SCK_B 13	// system clock
#define MOSI_B 15 // send data
#define DC_B 5		// mode select
#define RST_B 6		// reset
#define SCE_B 7		// chip select

// I2C Pins for GPS (I2C1)
#define SCL_B 8
#define SDA_B 9

// UART Pins for SD Card (USART3)
#define TX_B 10
#define RX_B 11
#define RTS_B 14


void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_FATFS_Init();

    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable GPIOC clock in RCC for LEDS

    // initialize LEDs
    configGPIOC_output(RED_LED);
    configGPIOC_output(GREEN_LED);
    configGPIOC_output(BLUE_LED);
    configGPIOC_output(ORANGE_LED);

    GPIOC->BSRR = (1 << RED_LED); // Start high
    GPIOC->BRR = (1 << BLUE_LED); // Start low

    // Set up LCD screen
    LCD screen = { SCK_B, MOSI_B, SCE_B, DC_B, RST_B };
    LCD_Setup(&screen);

    // Setup OpenLog
    OPENLOG sdcard = { TX_B, RX_B, RTS_B, 9600 };
    OPENLOG_Setup(&sdcard);

    uint8_t colorOn = 1;

    while (1) {
        LCD_ClearRow(0, 0);
        LCD_Reset();
        switch(colorOn++ % 2) {
            case 0: LCD_PrintStringCentered("BLUE ON", 7); break;
            case 1: LCD_PrintStringCentered("RED ON", 6); break;
            default: LCD_PrintStringCentered("ERROR", 5);
        }

        //LEDs toggle every 200ms
        HAL_Delay(2000); // Delay 200ms
        
        // Toggle the output state
        toggleLED(RED_LED);
        toggleLED(BLUE_LED);
    }
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
