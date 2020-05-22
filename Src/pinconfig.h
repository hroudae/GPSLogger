/*
 * File: pinconfig.h
 * Purpose: Declares all functions that configure GPIO pins in various ways
 */
#ifndef __PINCONFIG_H
#define __PINCONFIG_H

#include "stm32f0xx_hal.h"

// GPIOB pins
void configGPIOB_output(uint8_t pin);
void configPinB_AF0(uint8_t x);
void configPinB_AF4(uint8_t x);

// GPIOC pins
void configGPIOC_output(uint8_t pin);

#endif /* __PINCONFIG_H */
