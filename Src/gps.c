/*
 * File: gps.c
 * Purpose: Defines functions that allow interaction with the SAM-M8Q GPS
 *          via I2C1
 */
#include "gps.h"
#include "string.h"
#include "lcd.h"
#include "stdio.h"

volatile PROTOCOL currentProtocol;

uint32_t UBX_ByteNumber, NMEA_ByteNumber, RTCM_ByteNumber;

/*
 * Setup the I2C1 subsytem and the GPIO pins
 */
void GPS_Setup(GPS *gps) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;	// Enable GPIOB clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // Enable I2C2 clock

    thisGPS = gps;
    currentProtocol = NONE;
    UBX_ByteNumber = 0;
    NMEA_ByteNumber = 0;
    RTCM_ByteNumber = 0;

    configPinB_AF1(thisGPS->i2c_scl);
    configPinB_AF1(thisGPS->i2c_sda);
    
    I2C1_Config();

    // Enable I2C1 peripheral using PE bit in CR1
	I2C1->CR1 |= 1 << I2C_CR1_PE_Pos;

    // Interrupt?
}

/*
 * Get data from the GPS
 */
NMEA_MSG GPS_GetData_NMEA() {
    // can either get number of bytes available, or poll the data stream register and 0xff means no data
    // if leave off reg addr, will automatically inc until 0xff; default is 0xff so can omit reg addr
    NMEA_MSG msg;

    // Get available bytes
    uint32_t available_bytes = 0;
    char available_high[2], available_low[2];
    if (I2C1_ReadStr(GPS_I2C_ADDR, AVAIL_BYTES_HIGH_REG, available_high, 1)) { // something went wrong
        snprintf(msg.status, NMEA_RMC_LEN_STATUS+1, "%s", commError);
        return msg;
    }
    if (I2C1_ReadStr(GPS_I2C_ADDR, AVAIL_BYTES_LOW_REG, available_low, 1)) { // something went wrong
        snprintf(msg.status, NMEA_RMC_LEN_STATUS+1, "%s", commError);
        return msg;
    }
    available_bytes = ((uint32_t)available_high[0] << 8) | (uint32_t)available_low[0];

    if (available_bytes == 0) { // no data available
        snprintf(msg.status, NMEA_RMC_LEN_STATUS+1, "%s", noData);
        return msg;
    }
    
    char data_stream[available_bytes+1];
    if (I2C1_ReadStr(GPS_I2C_ADDR, DATA_STREAM_REG, data_stream, available_bytes)) {
        snprintf(msg.status, NMEA_RMC_LEN_STATUS+1, "%s", commError);
        return msg;
    }

    data_stream[available_bytes] = '\0';
    // LCD_ClearDisplay(); LCD_PrintStringCentered("parse");
    return GPS_ParseData_NMEA(data_stream);
}

/*
 * Parse received data sent with NMEA or UBX protocol
 * NMEA Frame: | $ | <address> | {,<value} | <checksum> | <CR><LF> |
 *      ie: $GPGLL,4717.11634,N,00833.91297,E,124923.00,A,A*6E
 *          $xxDTM,datum,subDatum,lat,NS,lon,EW,alt,refDatum*cs<CR><LF>
 */
NMEA_MSG GPS_ParseData_NMEA(char* data) {
    NMEA_MSG msg;

    if (data[0] != '$') {
        snprintf(msg.status, NMEA_RMC_LEN_STATUS+1, "%s", commError);
        return msg;
    }
    
    return NMEA_ParseData(data);
}

/*
 * Send poll request for msgid to gps
 */
void GPS_PollData(PROTOCOL prot, char* msgid) {
    switch (prot) {
        case NMEA: NMEA_PollGNQ(msgid, GPS_I2C_ADDR); break;
        case NONE:
        default:
            return;
    };
}

/*
 * Set rate of NMEA messages
 */
void GPS_SetRateNMEA(char* msgid, GPS_INTERFACE port, unsigned int rate) {
    NMEA_SetRate(msgid, port, rate, GPS_I2C_ADDR);
}
