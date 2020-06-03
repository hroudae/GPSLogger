/*
 * File: nmea.h
 * Purpose: Structs and defines used for NMEA messages
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

// Number of Fields in messages, including address, checksum, and end string; DOES NOT INCLUDE START
#define NMEA_DTM 11 // Output; datum reference
#define NMEA_GBQ 4  // Poll Request; poll a standard message (Talker GB)
#define NMEA_GBS 13 // Output; GNSS satellite fault detection
#define NMEA_GGA 17 // Output; global positioning system fix data
#define NMEA_GLL 10 // Output; latitude and longitude, with time of position fix and status
#define NMEA_GLQ 4  // Poll Request; poll a standard message (Talker GL)
#define NMEA_GNQ 4  // Poll Request; poll a standard message (Talker GN)
#define NMEA_GNS 16 // Output; GNSS fix data
#define NMEA_GPQ 4  // Poll Request; poll a standard message (Talker GP)
#define NMEA_GRS 19 // Output; GNSS range residuals
#define NMEA_GSA 21 // Output; GNSS DOP and active satellites
#define NMEA_GST 11 // Output; GNSS pseduorange error statisitics
#define NMEA_GSV 16 // Output; GNSS satellites in view
#define NMEA_RMS 16 // Output; Recommended minimum data
#define NMEA_TXT 7  // Output; Text Transmission
#define NMEA_VLW 11 // Output; Dual ground/water distance
#define NMEA_VTG 12 // Output; Course over ground and ground speed
#define NMEA_ZDA 9  // Output; Time and date

typedef struct {
    char[2] talker_id;
    char[3] sentence;
} NMEA_ADDR;

typedef struct {
    char start;         // '$'
    NMEA_ADDR addr;     // talker ID and sentence formatter
    char* data;         // data delimited by ','; varried length; starts with ','
    char[3] checksum;   // '*' followed by two characters - XOR of all characaters in addr and data
    char[2] end;        // <CR><LF>
} NMEA_MSG;




#endif /* __NMEA_H */
