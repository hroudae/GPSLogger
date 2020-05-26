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


// utilities.c
void setLED(uint8_t led);
void clearLED(uint8_t led);
void toggleLED(uint8_t led);

// pincofing.c
// GPIOB pins
void configGPIOB_output(uint8_t pin);
void configPinB_AF0(uint8_t x);
void configPinB_AF4(uint8_t x);
// GPIOC pins
void configGPIOC_output(uint8_t pin);

#endif /* __UTILITIES_H */