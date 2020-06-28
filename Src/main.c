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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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

volatile int recorddata = 0;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_FATFS_Init();

    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable GPIOC clock in RCC for LEDS
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable GPIOA clock in RCC for User button

    // initialize LEDs
    configGPIOC_output(RED_LED);
    configGPIOC_output(GREEN_LED);
    configGPIOC_output(BLUE_LED);
    configGPIOC_output(ORANGE_LED);
    
    // set upt user button and interrupt
    configUserButton();
    configUserButtonInterrupt();
    

    // Set up LCD screen
    LCD screen = { SCK_B, MOSI_B, SCE_B, DC_B, RST_B };
    LCD_Setup(&screen);

    GPS gps = { SCL_B, SDA_B };
    GPS_Setup(&gps);

    // Setup OpenLog
    OPENLOG sdcard = { TX_B, RX_B, RTS_B, 9600 };
    OPENLOG_Setup(&sdcard);

    // Disable unwanted messages
    GPS_SetRateNMEA("DTM", GPS_DDC, 0);
    GPS_SetRateNMEA("GBS", GPS_DDC, 0);
    GPS_SetRateNMEA("GGA", GPS_DDC, 0);
    GPS_SetRateNMEA("GLL", GPS_DDC, 0);
    GPS_SetRateNMEA("GNS", GPS_DDC, 0);
    GPS_SetRateNMEA("GRS", GPS_DDC, 0);
    GPS_SetRateNMEA("GSA", GPS_DDC, 0);
    GPS_SetRateNMEA("GST", GPS_DDC, 0);
    GPS_SetRateNMEA("GSV", GPS_DDC, 0);
    GPS_SetRateNMEA("TXT", GPS_DDC, 0);
    GPS_SetRateNMEA("VLW", GPS_DDC, 0);
    GPS_SetRateNMEA("VTG", GPS_DDC, 0);
    GPS_SetRateNMEA("ZDA", GPS_DDC, 0);

    // enable the RMC message once per epoch
    GPS_SetRateNMEA("RMC", GPS_DDC, 1);

    LCD_ClearDisplay();
    LCD_PrintStringCentered("Ready! Press button to record.");

    while (1) {
        while (recorddata == 0);
        HAL_Delay(5000);
        toggleLED(GREEN_LED);
        
        GPS_PollData(NMEA, NMEA_RMC);

        NMEA_MSG rmc = GPS_GetData_NMEA();
        if (strcmp(rmc.status, commError) == 0) {
            clearLED(ORANGE_LED);
            clearLED(BLUE_LED);
            setLED(RED_LED);
            continue;
        }

        // wait for data to be available
        while (strcmp(rmc.status, noData) == 0 || strcmp(rmc.status, invalidStatus) == 0) {
            clearLED(RED_LED);
            clearLED(BLUE_LED);
            setLED(ORANGE_LED);
            HAL_Delay(100);
            rmc = GPS_GetData_NMEA();
        }

        clearLED(RED_LED);
        clearLED(ORANGE_LED);
        setLED(BLUE_LED);

        // save to SD card
        char latmin[3];
        char latdeg[9];
        char lonmin[4];
        char londeg[9];
        char buff[2048];

        snprintf(latmin, 3, "%c%c", rmc.lat[0], rmc.lat[1]);
        snprintf(latdeg, 9, "%c%c%c%c%c%c%c%c", rmc.lat[2], rmc.lat[3], rmc.lat[4], rmc.lat[5], rmc.lat[6], rmc.lat[7], rmc.lat[8], rmc.lat[9]);

        snprintf(lonmin, 4, "%c%c%c", rmc.lon[0], rmc.lon[1], rmc.lon[2]);
        snprintf(londeg, 9, "%c%c%c%c%c%c%c%c", rmc.lon[3], rmc.lon[4], rmc.lon[5], rmc.lon[6], rmc.lon[7], rmc.lon[8], rmc.lon[9], rmc.lon[10]);

        float latdec = atof(latdeg) / 60.0;
        float londec = atof(londeg) / 60.0;

        char latstr[12], lonstr[12];
        snprintf(latstr, 2048, "%d.%07u", rmc.ns[0] == 'N' ? atoi(latmin) : -1*atoi(latmin), (int) ((latdec - (int) latdec) * 10000000));
        snprintf(lonstr, 2048, "%d.%07u", rmc.ew[0] == 'E' ? atoi(lonmin) : -1*atoi(lonmin), (int) ((londec - (int) londec) * 10000000));

        snprintf(buff, 2048, "\t\t<trkpt lat=\"%s\" lon=\"%s\"><ele>%s</ele><time>20%c%c-%c%c-%c%cT%c%c:%c%c:%c%cZ</time></trkpt>\n",
                latstr, lonstr, "0", rmc.date[0], rmc.date[1], rmc.date[2], rmc.date[3], rmc.date[4], rmc.date[5], \
                rmc.time[0], rmc.time[1], rmc.time[2], rmc.time[3], rmc.time[4], rmc.time[5]);
        USART3_SendStr(buff);

        // Print to screen
        LCD_ClearDisplay();
        LCD_PrintString("TIME: "); LCD_PrintString(rmc.time);
        LCD_PrintString(" LAT: "); LCD_PrintString(latstr);
        LCD_PrintString(" LON: "); LCD_PrintString(lonstr);
    }
}

/*
 * EXTI0 and EXTI1 interrupt request handler for User Button
 */
void EXTI0_1_IRQHandler(void) {
    if (recorddata == 0) { // first time through, delete file and write start of track
        char *file = "trail.gpx";
        char *setupText = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                          "<gpx version=\"1.0\">\n"
                          "\t<name>track 1 gpx</name>\n"
                          "\t<trk><name>track 1</name><number>1</number><trkseg>\n";
        OPENLOG_RemoveFile(file); // TODO wait for OpenLog reply
        OPENLOG_AppendFile(file, setupText);

        recorddata = 1;

        LCD_ClearDisplay();
        LCD_PrintStringCentered("Waiting for sats!");
    }
    else { // stop recordering data and end track
        char *endtags = "\t</trkseg></trk>\n"
                        "</gpx>\n";
        USART3_SendStr(endtags);
        recorddata = 0;

        LCD_ClearDisplay();
        LCD_PrintStringCentered("Recording ended!");
        while(1);
    }

    EXTI->PR |= 1;
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
