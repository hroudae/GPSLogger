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
#include "gps.h"
#include "fatfs.h"
#include "stdio.h"
#include "string.h"


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

    // Set up LCD screen
    LCD screen = { SCK_B, MOSI_B, SCE_B, DC_B, RST_B };
    LCD_Setup(&screen);

    GPS gps = { SCL_B, SDA_B };
    GPS_Setup(&gps);

    // Setup OpenLog
    OPENLOG sdcard = { TX_B, RX_B, RTS_B, 9600 };
    OPENLOG_Setup(&sdcard);

    LCD_ClearDisplay();
    LCD_PrintStringCentered("Here we go");

    //char *file = "gps.txt";
    //char *data = "I AM WRITING THIS DATA TO AN SD CARD!\n";
    //OPENLOG_AppendFile(file, data);
    int numnodata = 0;

    while (1) {
        HAL_Delay(1000);
        //if (numnodata < 5)
        //    GPS_PollData(NMEA, NMEA_RMC);
        //else {
        //    numnodata = 0;
        //    GPS_PollData(NMEA, NMEA_GLL);
       //}

        NMEA_MSG rmc = GPS_GetData_NMEA();
        if (strcmp(rmc.status, commError) == 0) {
            //LCD_ClearDisplay();
            //LCD_PrintStringCentered("Error getting data!");
            clearLED(ORANGE_LED);
            clearLED(BLUE_LED);
            setLED(RED_LED);
            continue;
        }
        else if (strcmp(rmc.status, noData) == 0) {
            //LCD_ClearDisplay();
            //LCD_PrintStringCentered("no data");
            clearLED(RED_LED);
            clearLED(BLUE_LED);
            setLED(ORANGE_LED);
            numnodata++;
            continue;
        }
        else {
            clearLED(RED_LED);
            clearLED(ORANGE_LED);
            setLED(BLUE_LED);
            LCD_ClearDisplay();

            //char first[32];
            //snprintf(first, 32, "%02x%02x%02x%02x%02x%02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
            LCD_PrintString("TIME: "); LCD_PrintString(rmc.time);
            LCD_PrintString(" LAT: "); LCD_PrintString(rmc.lat); LCD_PrintString(rmc.ns);
            LCD_PrintString(" LON: "); LCD_PrintString(rmc.lon); LCD_PrintString(rmc.ew);

            USART3_SendStr(rmc.time);
        }
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
