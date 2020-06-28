/*
 * File: openlog.c
 * Purpose: Defines functions that allow interaction with the OpenLog microSD
 *          reader via USART3
 * https://www.sparkfun.com/products/13712
 */
#include "openlog.h"
#include "lcd.h"
#include <stdio.h>

volatile enum MODE mode;

uint8_t rst_seq_frame;
const char *RST_SEQ_OK = "12<";

uint8_t ent_cmd_md_frame;
const char *ENT_CMD_MD_OK = "~>";

uint8_t init_cmd_md_frame;
const char *INIT_CMD_MD_OK = "\r\n<";

uint8_t rm_rply_md_frame;
const char *RM_RPLY_MD_ERR = "\r\n!>";

/*
 * Setup the USART3 subsytem and the GPIO pins
 */
void OPENLOG_Setup(OPENLOG *openLog) {
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; //Enable USART3 clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  // Enable GPIOB clock

    thisOpenLog = openLog;
    mode = RST_SEQ;

    configPinB_AF4(thisOpenLog->uart_tx);
    configPinB_AF4(thisOpenLog->uart_rx);
    configGPIOB_output(thisOpenLog->uart_rts);  // Should this be a general output instead?
    GPIOB->BSRR = (1 << thisOpenLog->uart_rts);

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

    HAL_Delay(100);
    OPENLOG_ResetSequence();
}

/*
 * Reset OpenLog to a kown state
 */
 void OPENLOG_ResetSequence(void) {
    mode = RST_SEQ;
    rst_seq_frame = 0;

    GPIOB->BRR = (1 << thisOpenLog->uart_rts);
    HAL_Delay(10);
    GPIOB->BSRR = (1 << thisOpenLog->uart_rts);

    while (mode != RDY);
 }

/*
 * USART3 or 4 interrupt request handler
 */
void OPENLOG_USART3ReceivedInterrupt() {
    if (!(USART3->ISR & USART_ISR_RXNE_Msk)) return;
    uint8_t recvValue = USART3->RDR;

    // if in reset sequence, a received 12< means OpenLog is ready to receive data
    // if it's not that, something has gone wrong so spin
    if (mode == RST_SEQ && recvValue != RST_SEQ_OK[rst_seq_frame]) {LCD_ClearDisplay();LCD_PrintStringCentered("RST SEQ ERROR");while(1);}
    else if (mode == RST_SEQ && rst_seq_frame < 2) {
        rst_seq_frame++;
    }
    else if (mode == RST_SEQ && rst_seq_frame == 2) {
        setLED(GREEN_LED);
        mode = RDY;
    }
    // if trying to enter command mode, a ~> means OpenLog is now in command mode and
    // ready to receive commands
    else if (mode == ENT_CMD && recvValue != ENT_CMD_MD_OK[ent_cmd_md_frame]) {LCD_ClearDisplay();LCD_PrintStringCentered("ENT CMD MD ERROR");while(1);}
    else if (mode == ENT_CMD && ent_cmd_md_frame < 1) {
        ent_cmd_md_frame++;
    }
    else if (mode == ENT_CMD && ent_cmd_md_frame == 1) {
        setLED(ORANGE_LED);
        mode = CMD_RDY;
    }
    else if (mode == INIT_CMD && recvValue != INIT_CMD_MD_OK[init_cmd_md_frame]) {LCD_ClearDisplay();LCD_PrintStringCentered("INIT CMD MD ERROR");while(1);}
    else if (mode == INIT_CMD && init_cmd_md_frame < 2) {
        init_cmd_md_frame++;
    }
    else if (mode == INIT_CMD && init_cmd_md_frame == 2) {
        mode = RDY;
    }
    else if (mode == RM_RPLY && recvValue != RM_RPLY_MD_ERR[rm_rply_md_frame]) {LCD_ClearDisplay();LCD_PrintStringCentered("RM RPLY ERR");while(1);}
    else if (mode == RM_RPLY && rm_rply_md_frame < 3) {
        rm_rply_md_frame++;
    }
    else if (mode == RM_RPLY && rm_rply_md_frame == 3) {
        mode = RDY;
    }
}

/*
 * Enter Command Mode for OpenLog by sending the CTRL+z character three times
 */
void OPENLOG_EnterCommandMode() {
    char ctrlZ[4] = { 26, 26, 26, '\0' };
    mode = ENT_CMD;
    ent_cmd_md_frame = 0;
    USART3_SendStr(ctrlZ);
}

/*
 * Create a new file on the microSD card
 */
void OPENLOG_NewFile(char* name) {
    if (mode != CMD_RDY) {
        OPENLOG_EnterCommandMode();
        while (mode != CMD_RDY);
    }

    mode = INIT_CMD;
    init_cmd_md_frame = 0;
    char cmd[32];
    sprintf(cmd, "new %s\r", name);
    USART3_SendStr(cmd);
}

/*
 * Remove a file from the SD card
 */
void OPENLOG_RemoveFile(char* name) {
    if (mode != CMD_RDY) {
        OPENLOG_EnterCommandMode();
        while (mode != CMD_RDY);
    }

    mode = RM_RPLY; // OpenLog replies \r\n!> need to wait for it
    rm_rply_md_frame = 0;
    char cmd[32];
    sprintf(cmd, "rm %s\r", name);
    USART3_SendStr(cmd);

    while (mode != RDY);
}

/*
 * Append text to the end of a file. If the file does not exist, it is created. msg needs to be null terminated
 */
void OPENLOG_AppendFile(char* name, char* msg) {
    if (mode != CMD_RDY) {
        OPENLOG_EnterCommandMode();
        while (mode != CMD_RDY);
    }

    mode = INIT_CMD;
    init_cmd_md_frame = 0;
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
    init_cmd_md_frame = 0;
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
    init_cmd_md_frame = 0;
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
    init_cmd_md_frame = 0;
    char *cmd = "reset\r";

    USART3_SendStr(cmd);
}
