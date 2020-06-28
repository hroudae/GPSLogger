/*
 * File: nmea.c
 * Purpose: Functions used for NMEA messages to and from the GPS
 * https://en.wikipedia.org/wiki/NMEA_0183#Message_structure
 */
#include "nmea.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char start = '$';
char csstart = '*';
char end[3] = { 0x0D, 0x0A, '\0' }; // <CR><LF>

char invalidStatus[NMEA_RMC_LEN_STATUS+1] = "V";
char noData[NMEA_RMC_LEN_STATUS+1] = "Z";
char commError[NMEA_RMC_LEN_STATUS+1] = "Q";

/*
 * Create and send the poll message defined in the struct
 */
void NMEA_PollMsg(NMEA_POLL_MSG* msgstruct, uint8_t i2caddr) {
    char msg[NMEA_MAX_LEN+1];
    snprintf(msg, NMEA_MAX_LEN+1, \
        "%c%s,%s%c%02X%s", start, msgstruct->addr, msgstruct->data, csstart, msgstruct->checksum, end);
    I2C1_WriteStrNoReg(i2caddr, msg);
}

/*
 * Send a poll message for GN data: $xxGNQ,msgId*cs<CR><LF>
 */
void NMEA_PollGNQ(char* msgid, uint8_t i2caddr) {
    char addr[NMEA_MAX_LEN_ADDR+1];
    snprintf(addr, NMEA_MAX_LEN_ADDR+1, "%s%s", TALKER_ID_MICROPROC, NMEA_GNQ);
    int cs = NMEA_Checksum(addr, msgid);
    NMEA_POLL_MSG msgstruct = { addr, msgid, cs };
    NMEA_PollMsg(&msgstruct, i2caddr);
}

/*
 * Send a poll message for GP data: $xxGPQ,msgId*cs<CR><LF>
 */
void NMEA_PollGPQ(char* msgid, uint8_t i2caddr) {
    char addr[NMEA_MAX_LEN_ADDR+1];
    snprintf(addr, NMEA_MAX_LEN_ADDR+1, "%s%s", TALKER_ID_MICROPROC, NMEA_GPQ);
    int cs = NMEA_Checksum(addr, msgid);
    NMEA_POLL_MSG msgstruct = { addr, msgid, cs };
    NMEA_PollMsg(&msgstruct, i2caddr);
}

/*
 * Parse the received data
 */
NMEA_MSG NMEA_ParseData(char* data) {
    char id[NMEA_MAX_LEN_TALKERID+1];
    char format[NMEA_MAX_LEN_SENTENCE+1];

    for (int i = 1, j = 0; i < 3; i++, j++) {
        id[j] = data[i];
    }
    for (int i = 3, j = 0; i < 6; i++, j++) {
        format[j] = data[i];
    }
    id[NMEA_MAX_LEN_TALKERID] = '\0';
    format[NMEA_MAX_LEN_SENTENCE] = '\0';

    if (strcmp(format, NMEA_RMC) == 0) {
        return NMEA_ParseRMCData(data);
    }
    else if (strcmp(format, NMEA_GLL) == 0) {
        return NMEA_ParseGLLData(data);
    }
    else {
        NMEA_MSG msg;
        snprintf(msg.status, NMEA_RMC_LEN_STATUS+1, "%s", noData);
        return msg;
    }
}

/*
 * Parse received RMC data
 */
NMEA_MSG NMEA_ParseRMCData(char* data) {
    int len = strlen(data);
    int fieldno = 0;
    int charnum = 0;
    NMEA_MSG rmc;
    char checksum[NMEA_MAX_LEN_CHECKSUM+1];

    for (int i = 1; i < len-NMEA_MAX_LEN_END; i++) {
        if (data[i] == ',' || data[i] == '*') {
            fieldno++; charnum = 0; continue;
        }

        switch(fieldno) {
            case  0: rmc.addr[charnum] = data[i]; rmc.addr[charnum+1] = '\0'; break;
            case  1: rmc.time[charnum] =  data[i]; rmc.time[charnum+1] = '\0'; break;
            case  2: rmc.status[charnum] =  data[i]; rmc.status[charnum+1] = '\0'; break;
            case  3: rmc.lat[charnum] =  data[i]; rmc.lat[charnum+1] = '\0'; break;
            case  4: rmc.ns[charnum] =  data[i]; rmc.ns[charnum+1] = '\0'; break;
            case  5: rmc.lon[charnum] =  data[i]; rmc.lon[charnum+1] = '\0'; break;
            case  6: rmc.ew[charnum] =  data[i]; rmc.ew[charnum+1] = '\0'; break;
            case  7: rmc.spd[charnum] =  data[i]; rmc.spd[charnum+1] = '\0'; break;
            case  8: rmc.cog[charnum] =  data[i]; rmc.cog[charnum+1] = '\0'; break;
            case  9: rmc.date[charnum] =  data[i]; rmc.date[charnum+1] = '\0'; break;
            case 10: rmc.mv[charnum] =  data[i]; rmc.mv[charnum+1] = '\0'; break;
            case 11: rmc.mvew[charnum] =  data[i]; rmc.mvew[charnum+1] = '\0'; break;
            case 12: rmc.posmode[charnum] =  data[i]; rmc.posmode[charnum+1] = '\0'; break;
            case 13: rmc.navstatus[charnum] =  data[i]; rmc.navstatus[charnum+1] = '\0'; break;
            case 14: checksum[charnum] =  data[i]; checksum[charnum+1] = '\0'; break;
        };

        charnum++;
    }

    int cs = (int) strtol(&checksum[1], NULL, 16);
    rmc.checksum = cs;

    return rmc;
}

/*
 * Parse received GLL data
 */
NMEA_MSG NMEA_ParseGLLData(char *data) {
    int len = strlen(data);
    int fieldno = 0;
    int charnum = 0;
    NMEA_MSG gll;
    char checksum[NMEA_MAX_LEN_CHECKSUM+1];

    for (int i = 1; i < len-NMEA_MAX_LEN_END; i++) {
        if (data[i] == ',' || data[i] == '*') {
            fieldno++; charnum = 0; continue;
        }

        switch(fieldno) {
            case 0: gll.addr[charnum] = data[i]; gll.addr[charnum+1] = '\0'; break;
            case 1: gll.lat[charnum] =  data[i]; gll.lat[charnum+1] = '\0'; break;
            case 2: gll.ns[charnum] =  data[i]; gll.ns[charnum+1] = '\0'; break;
            case 3: gll.lon[charnum] =  data[i]; gll.lon[charnum+1] = '\0'; break;
            case 4: gll.ew[charnum] =  data[i]; gll.ew[charnum+1] = '\0'; break;
            case 5: gll.time[charnum] =  data[i]; gll.time[charnum+1] = '\0'; break;
            case 6: gll.status[charnum] =  data[i]; gll.status[charnum+1] = '\0'; break;
            case 7: gll.posmode[charnum] =  data[i]; gll.posmode[charnum+1] = '\0'; break;
            case 8: checksum[charnum] =  data[i]; checksum[charnum+1] = '\0'; break;
        };

        charnum++;
    }

    int cs = (int) strtol(&checksum[1], NULL, 16);
    gll.checksum = cs;

    return gll;
}

/*
 * Find checksum for NMEA message. Checksum is over address and data.
 * Checksum is the XOR of all characters and should be sent as its value in 2 hex characters
 */
int NMEA_Checksum(char* addr, char* data) {
    int checksum = 0;
    char csrange[NMEA_MAX_LEN+1];
    snprintf(csrange, NMEA_MAX_LEN+1, "%s,%s", addr, data);

    int i = 0;
    while (csrange[i] != '\0') {
        checksum ^= csrange[i++];
    }

    return checksum;
}

/*
 * Set rate of NMEA messages using the PUBX command RATE
 */
void NMEA_SetRate(char* msgid, GPS_INTERFACE port, unsigned int rate, uint8_t i2caddr) {
    char msg[NMEA_MAX_LEN+1];
    char *addr = "PUBX";
    char rates[NMEA_MAX_LEN_DATA+1];
    char payload[NMEA_MAX_LEN_DATA+1] = "40,";

    strncat(payload, msgid, 4);

    switch (port) {
        case GPS_DDC:    snprintf(rates, NMEA_MAX_LEN_DATA, ",%u,,,,,", rate); break;
        case GPS_USART1: snprintf(rates, NMEA_MAX_LEN_DATA, ",,%u,,,,", rate); break;
        case GPS_USART2: snprintf(rates, NMEA_MAX_LEN_DATA, ",,,%u,,,", rate); break;
        case GPS_USB:    snprintf(rates, NMEA_MAX_LEN_DATA, ",,,,%u,,", rate); break;
        case GPS_SPI:    snprintf(rates, NMEA_MAX_LEN_DATA, ",,,,,%u,", rate); break;
        default:         snprintf(rates, NMEA_MAX_LEN_DATA, ",,,,,,"); break;
    }

    strncat(payload, rates, NMEA_MAX_LEN_DATA);
    strncat(payload, "0", 2); // reserved character, always 0

    int checksum = NMEA_Checksum(addr, payload);
    
    snprintf(msg, NMEA_MAX_LEN, "%c%s,%s%c%02x%s", start, addr, payload, csstart, checksum, end);

    I2C1_WriteStrNoReg(i2caddr, msg);
}