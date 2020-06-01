/*
 * File: gps.c
 * Purpose: Defines functions that allow interaction with the SAM-M8Q GPS
 *          via I2C1
 */
#include "gps.h"
#include "string.h"

volatile enum PROTOCOL currentProtocol;

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
void GPS_GetData(char *buf) {
    // can either get number of bytes available, or poll the data stream register and 0xff means no data
    // if leave off reg addr, will automatically inc until 0xff; default is 0xff so can omit reg addr

    // Get available bytes
    uint32_t available_bytes = 0;
    char available_high[2], available_low[2];
    if (I2C1_ReadStr(GPS_I2C_ADDR, AVAIL_BYTES_HIGH_REG, available_high, 1)) return; // something went wrong
    if (I2C1_ReadStr(GPS_I2C_ADDR, AVAIL_BYTES_LOW_REG, available_low, 1)) return; // something went wrong
    available_bytes = ((uint32_t)available_high[0] << 8) | (uint32_t)available_low[1];
    
    char data_stream[available_bytes+1];
    if (I2C1_ReadStr(GPS_I2C_ADDR, DATA_STREAM_REG, data_stream, available_bytes)) return;

    GPS_ParseData(data_stream);

    buf = data_stream;
}

/*
 * Parse received data sent with NMEA or UBX protocol
 * NMEA Frame: | $ | <address> | {,<value} | <checksum> | <CR><LF> |
 *      ie: $GPGLL,4717.11634,N,00833.91297,E,124923.00,A,A*6E
 *          $xxDTM,datum,subDatum,lat,NS,lon,EW,alt,refDatum*cs<CR><LF>
 */
void GPS_ParseData(char* data) {
    uint32_t len = strlen(data);

    for (uint32_t i = 0; i < len; i++) {
        switch(data[i]) {
            case '$': currentProtocol = NMEA; NMEA_ByteNumber = 1; break;
            case 0xB5: currentProtocol = UBX; UBX_ByteNumber = 1; break;
            case 0xD3: currentProtocol = RTCM; RTCM_ByteNumber = 1; break;

            // TODO PARSE DATA
        };
    }
}