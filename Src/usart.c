/*
 * File: usart.c
 * Purpose: Define the utility functions pertaining to the USART peripheral
 */
 #include "utilities.h"
 #include "openlog.h"


/*
 * Set baud rate of USART3 to rate using the HCLK
 */
void USART3_SetBaudRate(uint32_t rate) {
  uint32_t fclk = HAL_RCC_GetHCLKFreq();
  uint16_t usartdiv = fclk / rate;
  USART3->BRR = usartdiv;
}

 /*
 * Send char on USART3
 */
void USART3_SendChar(char c) {
	// wait until transmit data register is empty. bit 7 will be set when empty
	while ((USART3->ISR & (1 << 7)) == 0) {}
	
	// Transmit data register is now empty, write new char to send
	USART3->TDR = c;
}


/*
 * Send a null-terminated string on USART3
 */
void USART3_SendStr(char* s) {
	uint32_t i = 0;
	while (s[i] != '\0') USART3_SendChar(s[i++]);
}

/*
 * USART3 and 4 interrupt handler
 */
void USART3_4_IRQHandler(void) {
    OPENLOG_USART3ReceivedInterrupt();
}
