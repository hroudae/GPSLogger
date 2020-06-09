/*
 * File: nmea.h
 * Purpose: Structs, defines, and functions used for NMEA messages to and from the GPS
 * https://en.wikipedia.org/wiki/NMEA_0183#Message_structure
 */
#ifndef __NMEA_H
#define __NMEA_H

#include "utilities.h"

// Talker IDs - https://www.nmea.org/Assets/20190303%20nmea%200183%20talker%20identifier%20mnemonics.pdf
#define TALKER_ID_GPS       "GP"
#define TALKER_ID_SBAS      "GP"
#define TALKER_ID_QZSS      "GP"
#define TALKER_ID_GLONASS   "GL"
#define TALKER_ID_GALILEO   "GA"
#define TALKER_ID_BEIDOU    "GB"
#define TALKER_ID_COMBO     "GN"
#define TALKER_ID_MICROPROC "UP"

// Sentence Format names
#define NMEA_DTM "DTM"
#define NMEA_GBQ "GBQ"
#define NMEA_GBS "GBS"
#define NMEA_GGA "GGA"
#define NMEA_GLL "GLL"
#define NMEA_GLQ "GLQ"
#define NMEA_GNQ "GNQ"
#define NMEA_GNS "GNS"
#define NMEA_GPQ "GPQ"
#define NMEA_GRS "GRS"
#define NMEA_GSA "GSA"
#define NMEA_GST "GST"
#define NMEA_GSV "GSV"
#define NMEA_RMC "RMC"
#define NMEA_TXT "TXT"
#define NMEA_VLW "VLW"
#define NMEA_VTG "VTG"
#define NMEA_ZDA "ZDA"

// Number of Fields in messages, including address, checksum, and end string; DOES NOT INCLUDE START
#define NMEA_FRAME_COUNT_DTM 11 // Output; datum reference
#define NMEA_FRAME_COUNT_GBQ 4  // Poll Request; poll a standard message (Talker GB)
#define NMEA_FRAME_COUNT_GBS 13 // Output; GNSS satellite fault detection
#define NMEA_FRAME_COUNT_GGA 17 // Output; global positioning system fix data
#define NMEA_FRAME_COUNT_GLL 10 // Output; latitude and longitude, with time of position fix and status
#define NMEA_FRAME_COUNT_GLQ 4  // Poll Request; poll a standard message (Talker GL)
#define NMEA_FRAME_COUNT_GNQ 4  // Poll Request; poll a standard message (Talker GN)
#define NMEA_FRAME_COUNT_GNS 16 // Output; GNSS fix data
#define NMEA_FRAME_COUNT_GPQ 4  // Poll Request; poll a standard message (Talker GP)
#define NMEA_FRAME_COUNT_GRS 19 // Output; GNSS range residuals
#define NMEA_FRAME_COUNT_GSA 21 // Output; GNSS DOP and active satellites
#define NMEA_FRAME_COUNT_GST 11 // Output; GNSS pseduorange error statisitics
#define NMEA_FRAME_COUNT_GSV 16 // Output; GNSS satellites in view
#define NMEA_FRAME_COUNT_RMC 16 // Output; Recommended minimum data
#define NMEA_FRAME_COUNT_TXT 7  // Output; Text Transmission
#define NMEA_FRAME_COUNT_VLW 11 // Output; Dual ground/water distance
#define NMEA_FRAME_COUNT_VTG 12 // Output; Course over ground and ground speed
#define NMEA_FRAME_COUNT_ZDA 9  // Output; Time and date

// Max length of each part of the message, total max length of 82 characters
#define NMEA_MAX_LEN          82
#define NMEA_MAX_LEN_START    1
#define NMEA_MAX_LEN_TALKERID 2
#define NMEA_MAX_LEN_SENTENCE 3
#define NMEA_MAX_LEN_ADDR     NMEA_MAX_LEN_TALKERID+NMEA_MAX_LEN_SENTENCE
#define NMEA_MAX_LEN_DATA     71
#define NMEA_MAX_LEN_CHECKSUM 3 // includes the '*'
#define NMEA_MAX_LEN_END      2

#define NMEA_RMC_LEN_TIME      9
#define NMEA_RMC_LEN_STATUS    1
#define NMEA_RMC_LEN_LAT       10
#define NMEA_RMC_LEN_NS        1
#define NMEA_RMC_LEN_LON       11
#define NMEA_RMC_LEN_EW        1
#define NMEA_RMC_LEN_SPD       NMEA_MAX_LEN_DATA
#define NMEA_RMC_LEN_COG       NMEA_MAX_LEN_DATA
#define NMEA_RMC_LEN_DATE      6
#define NMEA_RMC_LEN_MV        NMEA_MAX_LEN_DATA
#define NMEA_RMC_LEN_MVEW      1
#define NMEA_RMC_LEN_POSMODE   1
#define NMEA_RMC_LEN_NAVSTATUS 1


typedef struct {
    char* addr;     // talker ID and sentence formatter
    char* data;      // data delimited by ','; varried length; starts with ','
    int checksum;   // '*' followed by two characters - XOR of all characaters in addr and data
} NMEA_POLL_MSG;

typedef struct {
    char* addr;      // talker ID and sentence formatter
    char* time;      // UTC time - HHMMSS.SS
    char* status;    // data validitiy status
    char* lat;       // latitude - ddmm.mmmmm
    char* ns;        // North/South indicator
    char* lon;       // longitude - dddmm.mmmmm
    char* ew;        // East/West indicator
    char* spd;       // speed over ground
    char* cog;       // course over ground
    char* date;      // date in DDMMYY format
    char* mv;        // magnetic variation value
    char* mvew;      // magnetic variation E/W indicator
    char* posmode;   // mode indicator
    char* navstatus; // navigational status
    int   checksum;  // '*' followed by two characters - XOR of all characaters in addr and data
} NMEA_RMC_MSG;


void NMEA_PollMsg(NMEA_POLL_MSG* msgstruct, uint8_t i2caddr);
void NMEA_PollGNQ(char* msgid, uint8_t i2caddr);

uint8_t NMEA_ParseData(char* data, NMEA_RMC_MSG *buf);
void NMEA_ParseRMCData(char* data, NMEA_RMC_MSG *buf);

int NMEA_Checksum(char* addr, char* msg);

#endif /* __NMEA_H */
