/*
 * File: gps.h
 * Purpose: Declares functions that allow interaction with the SAM-M8Q GPS
 *          via I2C1
 * Datasheet: https://www.u-blox.com/sites/default/files/products/documents/u-blox8-M8_ReceiverDescrProtSpec_%28UBX-13003221%29.pdf
 */
#ifndef __GPS_H
#define __GPS_H

#include "utilities.h"
#include "nmea.h"

#define GPS_I2C_ADDR 0x42

// I2C defined messages
#define NO_DATA 0xFF

// SAM-M8Q Registers
#define AVAIL_BYTES_HIGH_REG 0xFD
#define AVAIL_BYTES_LOW_REG  0xFE
#define DATA_STREAM_REG      0xFF

enum PROTOCOL { NONE, NMEA, UBX, RTCM};

typedef struct {
    uint8_t i2c_scl;
    uint8_t i2c_sda;
} GPS;

GPS *thisGPS;


void GPS_Setup(GPS *gps);

void GPS_GetData(char* buf);
void GPS_ParseData(char* data);

#endif /* __GPS_H */
