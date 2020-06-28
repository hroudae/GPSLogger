/*
 * File: pinconfig.c
 * Purpose: Defines all functions that configure GPIO pins in various ways
 */

#include "utilities.h"


/*
 * Set up the USER button on pin PA0
 * Configures pin to input, low-speed, and pull-down resistor
 */
void configUserButton(void) {
	// Set to digital input
	GPIOA->MODER &= ~((1) | (1 << 1));
	// Set to low speed
	GPIOA->OSPEEDR &= ~((1) | (1 << 1));
	// Set to pull-down resistor
	GPIOA->PUPDR |= (1 << 1);
	GPIOA->PUPDR &= ~(1);
}

/*
 * Generic GPIOB configuration function
 * Pass in the pin number, x, of the GPIO on PBx
 * Configures pin to general-pupose output mode, push-pull output,
 * low-speed, and no pull-up/down resistors
 */
void configGPIOB_output(uint8_t pin) {
    uint32_t shift2x = 2*pin;
    uint32_t shift2xp1 = shift2x+1;
  
    // Set General Pupose Output
    GPIOB->MODER |= (1 << shift2x);
    GPIOB->MODER &= ~(1 << shift2xp1);
    // Set to Push-pull
    GPIOB->OTYPER &= ~(1 << pin);
    // Set to Low speed
    GPIOB->OSPEEDR &= ~((1 << shift2x) | (1 << shift2xp1));
    // Set to no pull-up/down
    GPIOB->PUPDR &= ~((1 << shift2x) | (1 << shift2xp1));
}

/*
 * GPIOB Pin configuration function
 * Pass in the pin number, x
 * Configures pin to alternate function mode, push-pull output,
 * low-speed, no pull-up/down resistors, and AF0
 */
void configPinB_AF0(uint8_t x) {
    // Set to Alternate function mode, 10
    GPIOB->MODER &= ~(1 << (2*x));
    GPIOB->MODER |= (1 << ((2*x)+1));
    // Set to Push-pull
    GPIOB->OTYPER &= ~(1 << x);
    // Set to Low speed
    GPIOB->OSPEEDR |= ((1 << (2*x)) | (1 << ((2*x)+1)));
    // Set to no pull-up/down
    GPIOB->PUPDR &= ~((1 << (2*x)) | (1 << ((2*x)+1)));
    // Set alternate functon to AF0
    if (x < 8) {  // use AFR low register
        GPIOB->AFR[0] &= ~(0xF << (4*x));
    }
    else {  // use AFR high register
        GPIOB->AFR[1] &= ~(0xF << (4*(x-8)));
    }
}

/*
 * GPIOB Pin configuration function
 * Pass in the pin number, x
 * Configures pin to alternate function mode, push-pull output,
 * low-speed, no pull-up/down resistors, and AF1
 */
void configPinB_AF1(uint8_t x) {
    // Set to Alternate function mode, 10
    GPIOB->MODER &= ~(1 << (2*x));
    GPIOB->MODER |= (1 << ((2*x)+1));
    // Set to Push-pull
    GPIOB->OTYPER &= ~(1 << x);
    // Set to Low speed
    GPIOB->OSPEEDR |= ((1 << (2*x)) | (1 << ((2*x)+1)));
    // Set to no pull-up/down
    GPIOB->PUPDR &= ~((1 << (2*x)) | (1 << ((2*x)+1)));
    // Set alternate functon to AF1
    if (x < 8) {  // use AFR low register
        GPIOB->AFR[0] &= ~(0xF << (4*x));
        GPIOB->AFR[0] |= (0x1 << (4*x));
    }
    else {  // use AFR high register
        GPIOB->AFR[1] &= ~(0xF << (4*(x-8)));
        GPIOB->AFR[1] |= (0x1 << (4*(x-8)));
    }
}

/*
 * GPIOB Pin configuration function
 * Pass in the pin number, x
 * Configures pin to alternate function mode, push-pull output,
 * low-speed, no pull-up/down resistors, and AF4
 */
void configPinB_AF4(uint8_t x) {
    // Set to Alternate function mode, 10
    GPIOB->MODER &= ~(1 << (2*x));
    GPIOB->MODER |= (1 << ((2*x)+1));
    // Set to Push-pull
    GPIOB->OTYPER &= ~(1 << x);
    // Set to Low speed
    GPIOB->OSPEEDR &= ~((1 << (2*x)) | (1 << ((2*x)+1)));
    // Set to no pull-up/down
    GPIOB->PUPDR &= ~((1 << (2*x)) | (1 << ((2*x)+1)));
    // Set alternate functon to AF4
    if (x < 8) {  // use AFR low register
        GPIOB->AFR[0] &= ~(0xF << (4*x));
        GPIOB->AFR[0] |= (0x4 << (4*x));
    }
    else {  // use AFR high register
        GPIOB->AFR[1] &= ~(0xF << (4*(x-8)));
        GPIOB->AFR[1] |= (0x4 << (4*(x-8)));
    }
}

/*
 * Generic GPIOC configuration function
 * Pass in the pin number, x, of the GPIO on PCx
 * Configures pin to general-pupose output mode, push-pull output,
 * low-speed, and no pull-up/down resistors
 */
void configGPIOC_output(uint8_t pin) {
    uint32_t shift2x = 2*pin;
    uint32_t shift2xp1 = shift2x+1;
    
    // Set General Pupose Output
    GPIOC->MODER |= (1 << shift2x);
    GPIOC->MODER &= ~(1 << shift2xp1);
    // Set to Push-pull
    GPIOC->OTYPER &= ~(1 << pin);
    // Set to Low speed
    GPIOC->OSPEEDR &= ~((1 << shift2x) | (1 << shift2xp1));
    // Set to no pull-up/down
    GPIOC->PUPDR &= ~((1 << shift2x) | (1 << shift2xp1));
}