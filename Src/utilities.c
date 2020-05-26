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

/*
 * convert an unsigned int to a string and return the number of characters
 */ 
uint8_t uintToStr(char *buf, uint16_t number) {
    uint8_t charsWritten = 0;
    // get the character for each digit
    do {
        buf[charsWritten++] = '0' + (number % 10);
        number /= 10;
    } while (number > 0);
    
    // need to reverse the order in the array
    for (int i = 0, j = charsWritten-1; i < j; i++, j--) {
        char temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
    }
    
    return charsWritten;
}
