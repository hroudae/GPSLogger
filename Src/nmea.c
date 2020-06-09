/*
 * File: nmea.c
 * Purpose: Functions used for NMEA messages to and from the GPS
 * https://en.wikipedia.org/wiki/NMEA_0183#Message_structure
 */
#include "nmea.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

 char start = '$';
 char csstart = '*';
 char end[3] = { 0x0D, 0x0A, '\0' }; // <CR><LF>

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

uint8_t NMEA_ParseData(char* data, NMEA_RMC_MSG *buf) {
    char id[NMEA_MAX_LEN_TALKERID+1];
    char format[NMEA_MAX_LEN_SENTENCE+1];

    for (int i = 1; i < 3; i++)
        snprintf(id, NMEA_MAX_LEN_TALKERID+1, "%c", data[i]);
    for (int i = 3; i < 6; i++)
        snprintf(format, NMEA_MAX_LEN_SENTENCE+1, "%c", data[i]);

    if (strcmp(format, NMEA_RMC) == 0) {
        NMEA_ParseRMCData(data, buf);
        return 0;
    }
    else return 1;
}

void NMEA_ParseRMCData(char* data, NMEA_RMC_MSG *buf) {
    int len = strlen(data);
    int fieldno = 0;
    char addr[NMEA_MAX_LEN_ADDR+1];
    char time[NMEA_RMC_LEN_TIME+1];
    char status[NMEA_RMC_LEN_STATUS+1];
    char lat[NMEA_RMC_LEN_LAT+1];
    char ns[NMEA_RMC_LEN_NS+1];
    char lon[NMEA_RMC_LEN_LON+1];
    char ew[NMEA_RMC_LEN_EW+1];
    char spd[NMEA_RMC_LEN_SPD+1];
    char cog[NMEA_RMC_LEN_COG+1];
    char date[NMEA_RMC_LEN_DATE+1];
    char mv[NMEA_RMC_LEN_MV+1];
    char mvew[NMEA_RMC_LEN_MV+1];
    char posmode[NMEA_RMC_LEN_POSMODE+1];
    char navstatus[NMEA_RMC_LEN_NAVSTATUS+1];
    char checksum[NMEA_MAX_LEN_CHECKSUM];

    for (int i = 1; i < len-NMEA_MAX_LEN_END; i++) {
        if (data[i] == ',' || data[i] == '*') {
            fieldno++; continue;
        }

        switch(fieldno) {
            case  0: snprintf(addr, 2, "%c", data[i]); break;
            case  1: snprintf(time, 2, "%c", data[i]); break;
            case  2: snprintf(status, 2, "%c", data[i]); break;
            case  3: snprintf(lat, 2, "%c", data[i]); break;
            case  4: snprintf(ns, 2, "%c", data[i]); break;
            case  5: snprintf(lon, 2, "%c", data[i]); break;
            case  6: snprintf(ew, 2, "%c", data[i]); break;
            case  7: snprintf(spd, 2, "%c", data[i]); break;
            case  8: snprintf(cog, 2, "%c", data[i]); break;
            case  9: snprintf(date, 2, "%c", data[i]); break;
            case 10: snprintf(mv, 2, "%c", data[i]); break;
            case 11: snprintf(mvew, 2, "%c", data[i]); break;
            case 12: snprintf(posmode, 2, "%c", data[i]); break;
            case 13: snprintf(navstatus, 2, "%c", data[i]); break;
            case 14: snprintf(checksum, 2, "%c", data[i]); break;
        };
    }

    int cs = (int) strtol(&checksum[1], NULL, 16);

    NMEA_RMC_MSG rmc = { addr, time, status, lat, ns, lon, ew, spd, cog, date, mv, mvew, posmode, navstatus, cs };
    buf = &rmc;
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