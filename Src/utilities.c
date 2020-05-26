/*
 * File: utilities.c
 * Purpose: Define common functions
 */
#include "utilities.h"


/*
 * Set the sepcified LED
 */
void setLED(uint8_t led) {
    GPIOC->BSRR = (1 << led);
}

/*
 * Set the sepcified LED
 */
void clearLED(uint8_t led) {
    GPIOC->BRR = (1 << led);
}

/*
 * Toggle the sepcified LED
 */
void toggleLED(uint8_t led) {
    if (GPIOC->ODR & (1 << led)) // PCx is high
        GPIOC->BRR = (1 << led);
    else	// PCx is low
        GPIOC->BSRR = (1 << led);
}
