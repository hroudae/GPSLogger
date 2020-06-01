/*
 * File: openlog.h
 * Purpose: Declares all functions that allow interaction with the OpenLog microSD
 *          reader via USART3
 * https://www.sparkfun.com/products/13712
 */
#ifndef __OPENLOG_H
#define __OPENLOG_H

#include "utilities.h"


#define MAX_FILE_NAME_LENGTH 12

enum FILE_TYPE { ASCII=1, HEX=2, RAW=3};
enum MODE { RST_SEQ, RDY, ENT_CMD, INIT_CMD, CMD_RDY };

// Holds UART pin and baud rate information
typedef struct {        // OpenLog pins
    uint8_t uart_tx;    // RXI
    uint8_t uart_rx;    // TXO
    uint8_t uart_rts;   // GRN
    uint32_t uart_baud;
} OPENLOG;

OPENLOG *thisOpenLog;

void OPENLOG_Setup(OPENLOG *openLog);
void OPENLOG_ResetSequence(void);

// USART3 interrupt handler
void OPENLOG_USART3ReceivedInterrupt(void);

// enter command mode
void OPENLOG_EnterCommandMode(void);

// file manipulation functions
void OPENLOG_NewFile(char* name);
void OPENLOG_AppendFile(char* name, char* msg);
/* TODO
void OPENLOG_WriteFile(char* name, uint8_t len, uint32_t offset, char* msg);
void OPENLOG_RemoveFile(char* name, uint8_t len);
uint32_t OPENLOG_FileSize(char* name, uint8_t len);
char* OPENLOG_ReadFile(char* name, uint8_t len, uint32_t start, uint32_t length, FILE_TYPE type);
*/

// low level functions
void OPENLOG_Init(void);
void OPENLOG_Sync(void);
void OPENLOG_Reset(void);

#endif /* __OPENLOG_H */
