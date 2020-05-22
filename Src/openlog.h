/*
 * File: openlog.h
 * Purpose: Declares all functions that allow interaction with the OpenLog microSD
 *          reader via USART3
 */
#ifndef __OPENLOG_H
#define __OPENLOG_H

#include "pinconfig.h"

// Holds UART pin and baud rate information
typedef struct {
    uint8_t uart_tx;
    uint8_t uart_rx;
    uint8_t uart_rts;
    uint32_t uart_baud;
} OPENLOG;

OPENLOG *thisOpenLog;

void OPENLOG_Setup(OPENLOG *openLog);
void OPENLOG_SetBaudRate(uint32_t rate);

#endif /* __OPENLOG_H */
