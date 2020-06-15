/*
 * File: ubx.h
 * Purpose: Structs, defines, and functions used for UBX messages to and from the GPS
 */
#ifndef __UBX_H
#define __UBX_H

#include "utilities.h"

// all ubx messages start with these two sync characters
#define UBX_SYNC_1 0xb5
#define UBX_SYNC_2 0x62

// length of some elements of UBX message
#define UBX_MAX_LEN_MSGLEN 2

// Class value of the various message types
#define UBX_CLASS_NAV  0x01
#define UBX_CLASS_RXM  0x02
#define UBX_CLASS_INF  0x04
#define UBX_CLASS_ACK  0x05
#define UBX_CLASS_CFG  0x06
#define UBX_CLASS_UPD  0x09
#define UBX_CLASS_MON  0x0a
#define UBX_CLASS_AID  0x0b
#define UBX_CLASS_TIM  0x0d
#define UBX_CLASS_ESF  0x10
#define UBX_CLASS_MGA  0x13
#define UBX_CLASS_LOG  0x21
#define UBX_CLASS_SEC  0x27
#define UBX_CLASS_HNR  0x28
#define UBX_CLASS_NMEA 0xF0

// ID value of various message types
// NMEA messages
#define UBX_ID_NMEA_DTM 0x0a
#define UBX_ID_NMEA_GBQ 0x44
#define UBX_ID_NMEA_GBS 0x09
#define UBX_ID_NMEA_GGA 0x00
#define UBX_ID_NMEA_GLL 0x01
#define UBX_ID_NMEA_GLQ 0x43
#define UBX_ID_NMEA_GNQ 0x42
#define UBX_ID_NMEA_GNS 0x0d
#define UBX_ID_NMEA_GPQ 0x40
#define UBX_ID_NMEA_GRS 0x06
#define UBX_ID_NMEA_GSA 0x02
#define UBX_ID_NMEA_GST 0x07
#define UBX_ID_NMEA_GSV 0x03
#define UBX_ID_NMEA_RMC 0x04
#define UBX_ID_NMEA_TXT 0x41
#define UBX_ID_NMEA_VLW 0x0F
#define UBX_ID_NMEA_VTG 0x05
#define UBX_ID_NMEA_ZDA 0x08
// PUBX messages
#define UBX_ID_NMEA_PUBX_CONFIG   0x41
#define UBX_ID_NMEA_PUBX_POSITION 0x00
#define UBX_ID_NMEA_PUBX_RATE     0x40
#define UBX_ID_NMEA_PUBX_SVSTATUS 0x03
#define UBX_ID_NMEA_PUBX_TIME     0x04

typedef struct {
    char msgclass;
    char msgid;
    char msglen[UBX_MAX_LEN_MSGLEN+1];
    char ck_a;
    char ck_b;
} UBX_MSG;


void UBX_checksum(char *buf, char* data);

#endif /* __UBX_H */
