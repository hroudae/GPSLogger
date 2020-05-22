/*
 * File: openlog.c
 * Purpose: Defines functions that allow interaction with the OpenLog microSD
 *          reader via USART3
 */
#include "openlog.h"


/*
 * Setup the USART3 subsytem and the GPIO pins
 */
void OPENLOG_Setup(OPENLOG *openLog) {
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; //Enable USART3 clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  // Enable GPIOB clock

    thisOpenLog = openLog;

    configPinB_AF4(thisOpenLog->uart_tx);
    configPinB_AF4(thisOpenLog->uart_rx);
    configPinB_AF4(thisOpenLog->uart_rts);

    OPENLOG_SetBaudRate(thisOpenLog->uart_baud);

    // enable transmitter and reciever hardware
    USART3->CR1 |= USART_CR1_RE_Msk | USART_CR1_TE_Msk;
    // enable Recieved Register Not Empty interrupt
    USART3->CR1 |= USART_CR1_RXNEIE_Msk;
    // enable peripheral
    USART3->CR1 |= USART_CR1_UE_Msk;
  
	// enable the interrupt and set it to highest priority
    NVIC_EnableIRQ(USART3_4_IRQn);
	NVIC_SetPriority(USART3_4_IRQn, 0);
}

/*
 * Set baud rate of USART3 to rate using the HCLK
 */
void OPENLOG_SetBaudRate(uint32_t rate) {
  uint32_t fclk = HAL_RCC_GetHCLKFreq();
  uint16_t usartdiv = fclk / rate;
  USART3->BRR = usartdiv;
}

/*
 * USART3 or 4 interrupt request handler
 */
void USART3_4_IRQHandler(void) {

}
