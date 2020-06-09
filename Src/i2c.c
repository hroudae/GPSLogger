/*
 * File: usart.c
 * Purpose: Define the utility functions pertaining to the I2C1 peripheral
 */
#include "utilities.h"
#include "string.h"

/*
 * Configures the I2C1 peripheral to 100kHz
 * TODO: up this to 400kHz
 */
void I2C1_Config() {
	// Configure bus timing using I2Cx_TIMINGR for 100 kHz
	// PRESC = 1, SCLL = 0x13, SCLH = 0xF, SDADEL = 0x2, SCLDEL = 0x4
	// Clear the register
	I2C1->TIMINGR = 0x00000000;
	// Set everything
	I2C1->TIMINGR |= (0x1 << I2C_TIMINGR_PRESC_Pos);
	I2C1->TIMINGR |= (0x4 << I2C_TIMINGR_SCLDEL_Pos);
	I2C1->TIMINGR |= (0x2 << I2C_TIMINGR_SDADEL_Pos);
	I2C1->TIMINGR |= (0xF << I2C_TIMINGR_SCLH_Pos);
	I2C1->TIMINGR |= (0x13 << I2C_TIMINGR_SCLL_Pos);
}

/*
 * Set up a transaction by setting the info need in the CR2 Reg
 * addr is slave address, bytes is the number of bytes, rdwr is read or write transaction
 */
void I2C1_TransactionSetup(uint8_t addr, uint8_t bytes, uint8_t rdwr) {
	// Set slave address
	I2C1->CR2 &= ~(0x3FF << I2C_CR2_SADD_Pos); // clear the bits
	I2C1->CR2 |= addr << (I2C_CR2_SADD_Pos + 1); // only using 7 bit addresses
	// Set number of data bytes
	I2C1->CR2 &= ~(0xFF << I2C_CR2_NBYTES_Pos);
	I2C1->CR2 |= bytes << I2C_CR2_NBYTES_Pos;
	// Set the RD_WRN bit indicating direction of communication
	if (rdwr > 0) // set to read
		I2C1->CR2 |= 1 << I2C_CR2_RD_WRN_Pos;
	else // set to write
		I2C1->CR2 &= ~(1 << I2C_CR2_RD_WRN_Pos);
	// Set START to begin address frame
	I2C1->CR2 |= 1 << I2C_CR2_START_Pos;
}


/*
 * Set the STOP bit in CR2 to send stop condition on I2C1
 */
void I2C1_SendStopCondition() {
	I2C1->CR2 |= 1 << I2C_CR2_STOP_Pos;
}

/*
 * Send a char over I2C1
 */
void I2C1_SendChar(char c) {
	I2C1->TXDR = c;
}

/*
 * Send a string of null-terminated data over I2C1. No stop condition is sent to allow restart condition
 */
uint8_t I2C1_WriteStr(uint32_t addr, uint8_t reg, char *str) {
    uint32_t len = strlen(str) + 1;

    I2C1_TransactionSetup(addr, len, WRITE);

    for (uint32_t i = 0; i < len; i++) {
        // Check for TXIS or NACK flag set
        while (!(I2C1->ISR & I2C_ISR_TXIS_Msk) && !(I2C1->ISR & I2C_ISR_NACKF_Msk));
        
        if (I2C1->ISR & I2C_ISR_NACKF_Msk) {
            return 1; // FAILURE
        }
		
        if (i == 0) { // send register number to write to
            I2C1_SendChar(reg);
        }
        else { // send the message
            I2C1_SendChar(str[i-1]);
		}
    }

    return 0;
}

/*
 * Send a string of null-terminated data over I2C1 to no register. No stop condition is sent to allow restart condition
 */
uint8_t I2C1_WriteStrNoReg(uint32_t addr, char *str) {
    uint32_t len = strlen(str);

    I2C1_TransactionSetup(addr, len, WRITE);

    for (uint32_t i = 0; i < len; i++) {
        // Check for TXIS or NACK flag set
        while (!(I2C1->ISR & I2C_ISR_TXIS_Msk) && !(I2C1->ISR & I2C_ISR_NACKF_Msk));
        
        if (I2C1->ISR & I2C_ISR_NACKF_Msk) {
            return 1; // FAILURE
        }

        I2C1_SendChar(str[i]);
    }

    return 0;
}

/*
 * Read a string of data over I2C1 into the provided str buffer. No stop condition is sent to allow restart condition
 */
uint8_t I2C1_ReadStr(uint32_t addr, uint8_t reg, char str[], uint8_t bytes) {
    // write register to read from
    if (I2C1_WriteStr(addr, reg, "\0")) return 1;

    //LCD_ClearDisplay();
    //LCD_PrintStringCentered("reg sent");

    I2C1_TransactionSetup(addr, bytes, READ);

    //LCD_ClearDisplay();
    //LCD_PrintStringCentered("reading data");

    for (uint32_t i = 0; i < bytes; i++) {
        // Check for RXNE or NACK flag set
        while (!(I2C1->ISR & I2C_ISR_RXNE_Msk) && !(I2C1->ISR & I2C_ISR_NACKF_Msk));
		
        if (I2C1->ISR & I2C_ISR_NACKF_Msk) {
            return 1; // FAILURE
        }
		
        // RXNE flag was set
        str[i] = I2C1->RXDR;
    }

    return 0;
}
