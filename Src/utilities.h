/*
 * File: utilities.h
 * Purpose: Declare common functions and defines
 */
#ifndef __UTILITIES_H
#define __UTILITIES_H

#include "stm32f0xx_hal.h"


// LED Pins on GPIOC
#define RED_LED 6
#define BLUE_LED 7
#define ORANGE_LED 8
#define GREEN_LED 9

// I2C rd/wr directions
#define READ 1
#define WRITE 0


typedef enum {
    GPS_DDC,    // I2C, U-Blox calls it DDC in documentation
    GPS_USART1,
    GPS_USART2,
    GPS_USB,
    GPS_SPI
} GPS_INTERFACE;


// utilities.c
void setLED(uint8_t led);
void clearLED(uint8_t led);
void toggleLED(uint8_t led);

uint8_t uintToStr(char* buf, uint16_t number);
float strToFloat(char* str);
void floatToStr(char* buf, float num, int numplaces);

void configUserButtonInterrupt(void);

// usart.c
void USART3_SetBaudRate(uint32_t rate);
void USART3_SendChar(char c);
void USART3_SendStr(char* str);

// i2c.c
void I2C1_Config(void);
void I2C1_TransactionSetup(uint8_t addr, uint8_t bytes, uint8_t rdwr);
void I2C1_SendStopCondition(void);
void I2C1_SendChar(char c);
uint8_t I2C1_WriteStr(uint32_t addr, uint8_t reg, char *str);
uint8_t I2C1_WriteStrNoReg(uint32_t addr, char *str);
uint8_t I2C1_ReadStr(uint32_t addr, uint8_t reg, char str[], uint8_t bytes);

// pincofing.c
// GPIOA pins
void configUserButton(void); // PA0
// GPIOB pins
void configGPIOB_output(uint8_t pin);
void configPinB_AF0(uint8_t x);
void configPinB_AF1(uint8_t x);
void configPinB_AF4(uint8_t x);
// GPIOC pins
void configGPIOC_output(uint8_t pin);

#endif /* __UTILITIES_H */