/*
 * File: nmea.h
 * Purpose: Structs, defines, and functions used for NMEA messages to and from the GPS
 * https://en.wikipedia.org/wiki/NMEA_0183#Message_structure
 */
#ifndef __NMEA_H
#define __NMEA_H

// Talker IDs
#define TALKER_ID_GPS     "GP"
#define TALKER_ID_SBAS    "GP"
#define TALKER_ID_QZSS    "GP"
#define TALKER_ID_GLONASS "GL"
#define TALKER_ID_GALILEO "GA"
#define TALKER_ID_BEIDOU  "GB"
#define TALKER_ID_COMBO   "GN"

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
#define NMEA_MAX_LEN_DATA     71
#define NMEA_MAX_LEN_CHECKSUM 3 // includes the '*'
#define NMEA_MAX_LEN_END      2

typedef struct {
    char talker_id[2];  // depends on GNSS used
    char sentence[3];   // message format
} NMEA_ADDR;

typedef struct {
    char start;         // '$'
    NMEA_ADDR addr;     // talker ID and sentence formatter
    char data[71];      // data delimited by ','; varried length; starts with ','
    char checksum[3];   // '*' followed by two characters - XOR of all characaters in addr and data
    char end[2];        // <CR><LF>
} NMEA_MSG;


#endif /* __NMEA_H */
