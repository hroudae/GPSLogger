/*
 * File: openlog.c
 * Purpose: Defines functions that allow interaction with the OpenLog microSD
 *          reader via USART3
 * https://www.sparkfun.com/products/13712
 */
#include "openlog.h"
#include "stdio.h"

volatile enum MODE mode = RST_SEQ;

/*
 * Setup the USART3 subsytem and the GPIO pins
 */
void OPENLOG_Setup(OPENLOG *openLog) {
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; //Enable USART3 clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  // Enable GPIOB clock

    thisOpenLog = openLog;

    configPinB_AF4(thisOpenLog->uart_tx);
    configPinB_AF4(thisOpenLog->uart_rx);
    configPinB_AF4(thisOpenLog->uart_rts);  // Should this be a general output instead?

    USART3_SetBaudRate(thisOpenLog->uart_baud);

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
 * Reset OpenLogk to a kown state
 */
 void resetOpenLog(void) {
    mode = RST_SEQ;

    GPIOB->BRR = (1 << thisOpenLog->uart_rts);
    HAL_Delay(10);
    GPIOB->BSRR = (1 << thisOpenLog->uart_rts);
 }

/*
 * USART3 or 4 interrupt request handler
 */
void OPENLOG_USART3ReceivedInterrupt() {
    if (!(USART3->ISR & USART_ISR_RXNE_Msk)) return;

    uint8_t recvValue = USART3->RDR;

    // if in reset sequence, a received < means OpenLog is ready to receive data
    // if it's not that, something has gone wrong so spin
    // TODO will this actually be a 12<?
    if (mode == RST_SEQ && recvValue != '<') while(1);
    else if (mode == RST_SEQ) {
        setLED(GREEN_LED);
        mode = RDY;
    }
    // if trying to enter command mode, a > means OpenLog is now in command mode and
    // ready to receive commands
    else if (mode == INIT_CMD && (recvValue != '>' || recvValue != '<')) while(1);
    else if (mode == INIT_CMD && recvValue == '>') {
        setLED(ORANGE_LED);
        mode = CMD_RDY;
    }
    else if (mode == INIT_CMD && recvValue == '<') {
        mode = RDY;
    }
}

/*
 * Enter Command Mode for OpenLog by sending the CTRL+z character three times
 */
void OPENLOG_EnterCommandMode() {
    char ctrlZ[3] = { 26, 26, 26 };
    mode = INIT_CMD;
    USART3_SendStr(ctrlZ);
}

/*
 * Create a new file on the microSD card
 */
void OPENLOG_NewFile(char* name, uint8_t len) {
    if (mode != CMD_RDY) {
        OPENLOG_EnterCommandMode();
        while (mode != CMD_RDY);
    }

    mode = INIT_CMD;
    char cmd[32];
    sprintf(cmd, "new %s\r", name);
    USART3_SendStr(cmd);

    
}

/*
 * Append text to the end of a file. If the file does not exist, it is created. msg needs to be null terminated
 */
void OPENLOG_AppendFile(char* name, uint8_t len, char* msg) {
    if (mode != CMD_RDY) {
        OPENLOG_EnterCommandMode();
        while (mode != CMD_RDY);
    }

    mode = INIT_CMD;
    char cmd[32];
    sprintf(cmd, "append %s\r", name);
    USART3_SendStr(cmd);

    while (mode != RDY);

    USART3_SendStr(msg);
}

/*
 * Reinitializes the system and reopens the SD card - hepful if SD card stops responding
 */
void OPENLOG_Init(void) {
    if (mode != CMD_RDY) {
        OPENLOG_EnterCommandMode();
        while (mode != CMD_RDY);
    }

    mode = INIT_CMD;
    char *cmd = "init\r";

    USART3_SendStr(cmd);
}

/*
 * Syncs the current buffer to the SD card
 */
void OPENLOG_Sync(void) {
    if (mode != CMD_RDY) {
        OPENLOG_EnterCommandMode();
        while (mode != CMD_RDY);
    }

    mode = INIT_CMD;
    char *cmd = "sync\r";

    USART3_SendStr(cmd);
}

/*
 * jumps OpenLog to location zero, reruns bootloader, and then init code. Helpful if changes to config file nees to take affect
 */
void OPENLOG_Reset(void){
    if (mode != CMD_RDY) {
        OPENLOG_EnterCommandMode();
        while (mode != CMD_RDY);
    }

    mode = INIT_CMD;
    char *cmd = "reset\r";

    USART3_SendStr(cmd);
}
