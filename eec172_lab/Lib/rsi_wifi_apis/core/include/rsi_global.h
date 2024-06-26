/**
 * @file     rsi_global.h
 * @version  3.6
 * @date     2013-May-16
 *
 * Copyright(C) Redpine Signals 2013
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief HEADER, GLOBAL, Global Header file, the things that must be almost everywhere 
 *
 * @section Description
 * This is the top level global.h file for data/functions that need to be in every module
 * This file contains all the structures and defines used in the entire application 
 *
 * @section Improvements
 * New structures are added. 
 * The definition for MACRO "RSI_TXDATA_OFFSET_LUDP" from 28 to 26.
 * LTCP Conn Establishment Rcv structure is changed. 
 * rsi_uSleepTimer is changed
 *
 */


#ifndef _RSIGLOBAL_H_
#define _RSIGLOBAL_H_

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Global defines
 */
#define RSI_TRUE            1    
#define RSI_FALSE           0
#ifndef NULL
#define NULL                0
#endif

#define DATA_RX             0
#define DATA_TX             1

//#define PACKED            1 

#define WLAN_MGMT_TYPE  0x4
#define WLAN_DATA_TYPE  0x5
#define ZB_MGMT_TYPE    0x1
#define ZB_DATA_TYPE    0x1
#define BT_MGMT_TYPE    0x2
#define BT_DATA_TYPE    0x2

//!Comment This in case the host MCU is of BIG ENDIAN 
#define RSI_LITTLE_ENDIAN                       0

//#define RSI_FEATSEL_ENABLE                    0

//! Interrupt Mode Selection 
#define RSI_INTERRUPTS

//! Polled Mode selection
//#define RSI_POLLED

/*
 * @ Type Definitions
 */
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef char                int8;
typedef short               int16;
typedef long                int32;


#define ENABLE                      1
#define DISABLE                     0


#ifndef RSI_HWTIMER 
//! need to define this macro if h/w timer is available and it should increment spiTimer2, spiTimer1 
#define RSI_TICKS_PER_SECOND        50000 
#else
#define RSI_TICKS_PER_SECOND        10
#endif


/*@ firmware upgradation timeout */
#define RSI_FWUPTIMEOUT                100 * RSI_TICKS_PER_SECOND
/*@ wireless firmware upgradation timeout */
#define RSI_WIRELESSFWUPTIMEOUT        500 * RSI_TICKS_PER_SECOND
/*@ bootloading timeout */
#define RSI_BLTIMEOUT                  1 * RSI_TICKS_PER_SECOND
/*@ band timeout */
#define RSI_BANDTIMEOUT                1 * RSI_TICKS_PER_SECOND
/*@ Init timeout */
#define RSI_INITTIMEOUT                1 * RSI_TICKS_PER_SECOND
/*@ Query firmware version timeout */
#define RSI_QFWVTIMEOUT                1 * RSI_TICKS_PER_SECOND
/*@ Set Mac address timeout */
#define RSI_SMATIMEOUT                 1 * RSI_TICKS_PER_SECOND
/*@ Scan timeout */
#define RSI_SCANTIMEOUT                12 * RSI_TICKS_PER_SECOND
/*@ Sleep timer timeout */
#define RSI_SLEEPTIMERTIMEOUT          1 * RSI_TICKS_PER_SECOND
/*@ Wepkeys timeout */
#define RSI_WEPKEYSTIMEOUT             1 * RSI_TICKS_PER_SECOND
/*@ Join timeout */
#define RSI_JOINTIMEOUT                12 * RSI_TICKS_PER_SECOND
/*@ Disconnect timeout */
#define RSI_DISCONTIMEOUT              1 * RSI_TICKS_PER_SECOND
/*@ Query connection status timeout */
#define RSI_QCSTIMEOUT                 3 * RSI_TICKS_PER_SECOND
/*@ Query dhcp params timeout */
#define RSI_QDPTIMEOUT                 3 * RSI_TICKS_PER_SECOND
/*@ Query network params timeout */
#define RSI_QNPTIMEOUT                 3 * RSI_TICKS_PER_SECOND
/*@ Ip configuration timeout */
#define RSI_IPPTIMEOUT                 6 * RSI_TICKS_PER_SECOND
/*@ Query RSSI Value timeout */
#define RSI_RSSITIMEOUT                1 * RSI_TICKS_PER_SECOND
/*@ recv timeout */
#define RSI_RECVTIMEOUT                1 * RSI_TICKS_PER_SECOND
/*@ Socket open timeout */
#define RSI_SOPTIMEOUT                 6 * RSI_TICKS_PER_SECOND
/*@ Regread timeout */
#define RSI_REGREADTIMEOUT             1 * RSI_TICKS_PER_SECOND
/*@ Query DNS timeout */
#define RSI_QDNSTIMEOUT                6 * RSI_TICKS_PER_SECOND
/*@ Start token timeout */
#define RSI_START_TOKEN_TIMEOUT        10 * RSI_TICKS_PER_SECOND
/*@ Set Listen interval timeout */
#define RSI_SLITIMEOUT                 1 * RSI_TICKS_PER_SECOND
/*@ Config Enable timeout */
#define RSI_CETIMEOUT                  1 * RSI_TICKS_PER_SECOND
/*@ Config store timeout */
#define RSI_CSTIMEOUT                  1 * RSI_TICKS_PER_SECOND
/*@ Config get timeout */
#define RSI_CGTIMEOUT                  1 * RSI_TICKS_PER_SECOND
/*@ Query BSSID/NW TYPE timeout */
#define RSI_QBSSIDNWTIMEOUT            6 * RSI_TICKS_PER_SECOND
#define RSI_QMACADDTIMEOUT             6 * RSI_TICKS_PER_SECOND
/*@ Query MAC ADDR timeout */
/*@ Get HTTP timeout */
#define RSI_GETHTTPTIMEOUT             40 * RSI_TICKS_PER_SECOND
/*@ Post HTTP timeout */
#define RSI_POSTHTTPTIMEOUT            6 * RSI_TICKS_PER_SECOND
/*@ Mode select timeout */
#define RSI_MODESEL_TIMEOUT            1 * RSI_TICKS_PER_SECOND
/*@ Feature select timeout */
#define RSI_FEATSEL_TIMEOUT            1 * RSI_TICKS_PER_SECOND
/*@ WPS RESPONSE timeout */
#define RSI_WPSRSPTIMEOUT              120 * RSI_TICKS_PER_SECOND
/*@ Power save timeout*/
#define RSI_PWSAVE_TIMEOUT             5 * RSI_TICKS_PER_SECOND
/*@ Command response timeout */
#define RSI_RESPONSE_TIMEOUT(A)        RSI_RESET_TIMER3;                         \
                                           while (rsi_checkPktIrq() != RSI_TRUE) \
                                            {                                    \
                                              if (RSI_INC_TIMER_3 > A)           \
                                               {                                 \
                                                 retval = -1;                    \
                                                 break;                          \
                                               }                                 \
                                            }     



/*=======================================================================================*/
/**
 * Device Parameters
 */
#define RSI_MAXSOCKETS                       8     //@ Maximum number of open sockets

/**
 * Debugging Parameters
 */
#define RSI_DEBUG_DEVICE                    "UART_1"
#define RSI_MAX_PAYLOAD_SIZE                 1600  //@ Maximum data payload size
#define RSI_TCP_MAX_SEND_SIZE                1450
#define RSI_UDP_MAX_SEND_SIZE                1472
#define RSI_AP_SCANNED_MAX                   12    //@ Maximum number of scanned acces points
#define RSI_MAX_WFD_DEV_CNT                  10    //@ Maximum wifi direct device count

/**
 * Things that are needed in this .h file
 */
#define RSI_FRAME_DESC_LEN                  16     //@ Length of the frame descriptor, for both read and write
#define RSI_FRAME_CMD_RSP_LEN               56     //@ Length of the command response buffer/frame
#define RSI_TXDATA_OFFSET_TCP               46     //@ required Tx data offset value for TCP, 46
#define RSI_TXDATA_OFFSET_UDP               34     //@ required Tx data offset value for UDP, 34
#define RSI_RXDATA_OFFSET_TCP_V4            26     //@ required Rx data offset value for TCPV4, 26
#define RSI_RXDATA_OFFSET_TCP_V6            46     //@ required Rx data offset value for TCPV6, 46
#define RSI_RXDATA_OFFSET_UDP_V4            14     //@ required Rx data offset value for UDP_V4, 14
#define RSI_RXDATA_OFFSET_UDP_V6            34     //@ required Rx data offset value for UDP_V6, 34
#define RSI_TXDATA_OFFSET_LUDP              16     //@ required Rx data offset value for LUDP, 26
#define RSI_PSK_LEN                         64     //@ maximum length of PSK
#define RSI_SSID_LEN                        34     //@ maximum length of SSID
#define RSI_BSSID_LEN                       6      //@ BSSID length
#define RSI_IP_ADD_LEN                      4 
#define RSI_MAC_ADD_LEN                     6
#define RSI_MGMT_PKT_TYPE                   0x04
#define RSI_DATA_PKT_TYPE                   0x05 
#define RSI_PMK_LEN                         32     //@PMK maximum length                                   

/**
 * Const declaration
 *
 */
#define RSI_BYTES_3                         3
#define RSI_BYTES_2                         2




extern const uint8            rsi_frameCmdDebug[RSI_BYTES_3];            
extern const uint8            rsi_frameCmdBootInsn[RSI_BYTES_3];
extern const uint8            rsi_frameCmdUpdateInfo[RSI_BYTES_3];            
extern const uint8            rsi_frameCmdBand[RSI_BYTES_3];            
extern const uint8            rsi_frameCmdInit[RSI_BYTES_3];            
extern const uint8            rsi_frameCmdScan[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdBGScan[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdJoin[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdPsk[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdPower[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdIpparam[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdSocket[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdSocketClose[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdRssi[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdSend[RSI_BYTES_3];  
extern const uint8            rsi_frameCmdCert[RSI_BYTES_3];    
extern const uint8            rsi_frameCmdRecv[RSI_BYTES_3];  
extern const uint8            rsi_frameCmdConnStatus[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdQryNetParms[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdDisconnect[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdQryFwVer[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdSetMacAddr[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdQryMacAddress[RSI_BYTES_3];                
extern const uint8            rsi_frameCmdOperMode[RSI_BYTES_3];
extern const uint8            rsi_frameCmdP2pConfig[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSetEap[RSI_BYTES_3];
extern const uint8            rsi_frameCmdWebServer[RSI_BYTES_3];
extern const uint8            rsi_frameCmdWebFields[RSI_BYTES_3];
extern const uint8            rsi_frameCmdAntSel[RSI_BYTES_3];
extern const uint8            rsi_frameCmdQryGoParms[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSleepTimer[RSI_BYTES_3];
extern const uint8            rsi_frameCmdDnsQuery[RSI_BYTES_3];
extern       uint8            rsi_frameCmdHttpGetReq[RSI_BYTES_3];
extern const uint8            rsi_frameCmdDnsServer[RSI_BYTES_3];
extern       uint8            rsi_frameCmdHttpPostReq[RSI_BYTES_3];
extern const uint8            rsi_frameCmdReset[RSI_BYTES_3];
extern const uint8            rsi_frameCmdCfgSave[RSI_BYTES_3];
extern const uint8            rsi_frameCmdCfgEnable[RSI_BYTES_3];
extern const uint8            rsi_frameCmdCfgGet[RSI_BYTES_3];
extern const uint8            rsi_frameCmdFeatsel[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSnr[RSI_BYTES_3];
extern const uint8            rsi_frameCmdLtcpConnStatus[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSentBytesCount[RSI_BYTES_3];
extern const uint8            rsi_frameCmdAPconf[RSI_BYTES_3];
extern const uint8            rsi_frameCmdWepkey[RSI_BYTES_3];
extern const uint8            rsi_frameCmdUrlRsp[RSI_BYTES_3];
extern const uint8            rsi_frameCmdPingRequest[RSI_BYTES_3];
extern const uint8            rsi_frameCmdGpioconf[RSI_BYTES_3];
//!snmp commands
extern const uint8            rsi_frameCmdSnmpEnable[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSnmpGetRsp[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSnmpGetNextRsp[RSI_BYTES_3];
extern const uint8            rsi_frameCmdSnmpTrap[RSI_BYTES_3];
//!IPV6 ipconfig command resp
extern const uint8            rsi_frameCmdIPconf6[RSI_BYTES_3];
//!Wireless firmware upgrade ok command
extern const uint8            rsi_frameCmdWirelessFwUpgrade[RSI_BYTES_3];
//!Roaming parameters frame 
extern const uint8            rsi_frameCmdRoamParams[RSI_BYTES_3];
//!High throughput Capabilities parameters frame 
extern const uint8            rsi_frameCmdHtCaps[RSI_BYTES_3];
//!WMM PS parameters frame 
extern const uint8            rsi_frameCmdWmmPs[RSI_BYTES_3];
//!WPS Method parameters frame 
extern const uint8            rsi_frameCmdWpsMethod[RSI_BYTES_3];
//!Multicast params 
extern const uint8            rsi_frameCmdMulticast[RSI_BYTES_3]; 
//!PER MODE
extern const uint8            rsi_frameCmdPerMode[RSI_BYTES_3];
//!PER STATS
extern const uint8            rsi_frameCmdPerStats[RSI_BYTES_3];
//!Sleep ack
extern const uint8            rsi_sleepack[RSI_BYTES_3];
//!Multicast filter bitmap
extern const uint8            rsi_frameCmdMcastFilter[RSI_BYTES_3];

//!Management Response list
extern const uint8            rsi_mgmtRspList[];   

//!JSON & Webpage related
extern const uint8            rsi_frameCmdWebpageClearFiles[RSI_BYTES_3];
extern const uint8            rsi_frameCmdWebpageEraseFile[RSI_BYTES_3];
extern const uint8            rsi_frameCmdJSONCreate[RSI_BYTES_3];
extern const uint8            rsi_frameCmdJSONEraseFile[RSI_BYTES_3];

//!Set region 
extern const uint8            rsi_frameSetRegion[RSI_BYTES_3];
extern const uint8            rsi_frameSetRegionAp[RSI_BYTES_3];

//! Rejoin Params response Frame
extern const uint8            rsi_frameCmdRejoinParams[RSI_BYTES_3];

//! User store configuration Frame
extern const uint8			  rsi_frameCmdUserStoreCfg[RSI_BYTES_3];


/*===============================================*/
/**
 * Debug Structures
 */
typedef union {
    struct {
    uint32     assertion_type;
    uint32     assertion_level;
    } debugFrameSnd;
    uint8                 uDebugBuf[8];            //@ byte format to send to the spi interface,8 bytes
} rsi_uDebug;


/*===================================================*/
/**
 * set region
 */

typedef union {
    struct{
        uint8     setregion_code_from_user_cmd;
      /*Enable or disable set region from user:
         1-take from user configuration;
         0-Take from Beacons*/
        uint8     region_code;
       /*region code(1-US,2-EU,3-JP.4-World Domain)*/
        }setRegionFrameSnd;
        uint8 usetRegionBuf[2];
}rsi_usetregion;


/*======================================================*/
/**Set region in AP mode
*
*/
#define COUNTRY_CODE_LENGTH      3

#define MAX_POSSIBLE_CHANNEL     24
typedef union{
    struct{
        uint8     setregion_code_from_user_cmd;
      /*Enable or disable set region from user:
         1-take from user configuration;
         0-Take US or EU or JP*/
        uint8     country_code[3];
        /*region code(1-US,2-EU,3-JP)*/
        uint32 no_of_rules;
        struct{
            uint8   first_channel;
            uint8   no_of_channels;
            uint8   max_tx_power;
        }channel_info[MAX_POSSIBLE_CHANNEL];
    }setRegionApFrameSnd;
        uint8 usetRegionApBuf[80];
}rsi_usetregion_ap_t;


/*===============================================*/
/**
 * Scan Structures
 */

//! The scan command argument union/variables

typedef union {
    struct {
        uint8     channel[4];                             //@ RF channel to scan, 0=All, 1-14 for 2.5GHz channels 1-14
        uint8     ssid[RSI_SSID_LEN];                     //@ uint8[34], ssid to scan         
        uint8     reserved[6];                             //@ uint8[6], reserved fields
        uint8     channel_bit_map_2_4[2];                 //@ uint8[2], channel bit map for 2.4 Ghz
        uint8     channel_bit_map_5[4];                   //@ uint8[4], channel bit map for 5 Ghz
    } scanFrameSnd;
    uint8                 uScanBuf[RSI_SSID_LEN + 16];    //@ byte format to send to the spi interface, 48 bytes
} rsi_uScan;

/*===============================================*/
/**
 * Multicast Structures
 */

//! Multicast command structure
typedef union {
      struct {
        uint8     ip_version[2];
        uint8     req_Type[2]; 
          union
          {
             uint8      ipv4_address[4];
             uint32     ipv6_address[4];
          }group_address;
         }multicastFrameSnd;
        uint8     uMulticastBuf[20];
}rsi_uMulticast;


/*===============================================*/
/**
 * BGScan Structures
 */

//! The BG scan command argument union/variables
typedef union {
     struct {
           uint8    bgscan_enable[2];                //@ enable or disable BG scan        
           uint8    enable_instant_bgscan[2];        //@ Is it instant bgscan or normal bgscan
           uint8    bgscan_threshold[2];             //@ bg scan threshold value
           uint8    rssi_tolerance_threshold[2];     //@ tolerance threshold
           uint8    bgscan_periodicity[2];           //@ periodicity
           uint8    active_scan_duration[2];         //@ sctive scan duration
           uint8    passive_scan_duration[2];        //@ passive scan duration
           uint8    multi_probe;                     //@ multi probe
         } bgscanFrameSnd;
     uint8           ubgScanBuf[15];
     //@ byte format to send to the spi interface, 68 bytes
} rsi_ubgScan;



/*===============================================*/
/**
 * Join Data Frame Structure
 */
typedef union {
    struct {
           uint8    reserved1;                       //@ reserved bytes:Can be used for security Type
           uint8    securityType;                    //@ 0- Open, 1-WPA, 2-WPA2,6-MIXED_MODE
           uint8    dataRate;                        //@ data rate, 0=auto, 1=1Mbps, 2=2Mbps, 3=5.5Mbps, 4=11Mbps, 12=54Mbps
           uint8    powerLevel;                      //@ transmit power level, 0=low (6-9dBm), 1=medium (10-14dBm, 2=high (15-17dBm)
           uint8    psk[RSI_PSK_LEN];                //@ pre-shared key, 63-byte string , last charecter is NULL
           uint8    ssid[RSI_SSID_LEN];              //@ ssid of access point to join to, 34-byte string
           uint8    reserved2[3];                    //@ reserved bytes
           uint8    ssid_len;
    } joinFrameSnd;
    uint8    uJoinBuf[RSI_SSID_LEN + RSI_PSK_LEN + 8];            
    //@ byte format to send to the spi interface, 106 (0x6A) bytes
} rsi_uJoin;


/*===============================================*/
/**
 * PSK Frame Structure
 */
typedef union {
  struct {
    uint8    TYPE;
    uint8    psk_or_pmk[RSI_PSK_LEN];
    uint8    ap_ssid[RSI_SSID_LEN] ;
  } PskFrameSnd;
  uint8 uPskBuf[1 + RSI_PSK_LEN + RSI_SSID_LEN];
} rsi_uPsk;


/*===============================================*/
/**
 * Disconnect Data Frame Structure
 */
typedef struct {
        uint8    mode_flag[2];                       //@ 0- Module in Client mode, 1- AP mode
        uint8    client_mac_addr[6];                 //@ client MAC address, Ignored/Reserved in case of client mode
}rsi_disassoc_t;


/*===============================================*/
/**
 * JSON Structures
 */
#define JSON_BUFFER_SIZE                    512
#define JSON_CHUNK_LEN                      1024
#define RSI_JSON_MAX_CHUNK_LENGTH           1024
typedef struct rsi_jsonCreateObject_s
{
    char    filename[24];
    uint8   total_length[2];
    uint8   current_length[2];
    char    json_data[RSI_JSON_MAX_CHUNK_LENGTH];
} rsi_jsonCreateObject_t;

typedef struct rsi_tfs_clear_files_s
{
    uint8   clear;
} rsi_tfs_clear_files_t;

typedef struct rsi_tfs_erase_file_s
{
    char    filename[24];
} rsi_tfs_erase_file_t;

/*=======================================================*/
/*
 * TCP/IP Configure structures
 */

typedef union {
    struct {
        uint8    dhcpMode;                           //@ 0=Manual, 1=Use DHCP
        uint8    ipaddr[4];                          //@ IP address of this module if in manual mode
        uint8    netmask[4];                         //@ Netmask used if in manual mode
        uint8    gateway[4];                         //@ IP address of default gateway if in manual mode
		    uint8    hostname[31];                       //@ DHCP client host name
    } ipparamFrameSnd;
    uint8                    uIpparamBuf[44];        
    //@ 16 bytes, byte format to send to spi
} rsi_uIpparam;

/*===============================================*/
/*
 * IPV6 Configure
 */
typedef union{
   struct{
    uint8     mode[2];
    uint8     prefixLength[2];
    uint32    ipaddr6[4];
    uint32    gateway6[4];
  }ipconf6FrameSnd;
   uint8 uIpconf6Buf[36];
}rsi_uIPconf6;


/*===================================================*/
/**
 * Socket Configure
 */

#define WEBS_MAX_URL_LEN   51
#define WEBS_MAX_HOST_LEN  51

typedef union {
  struct {
    uint8    ip_version[2];                     //@ ip version4 or 6
    uint8    socketType[2];                     //@ 0=TCP Client, 1=UDP Client, 2=TCP Server (Listening TCP)
    uint8    moduleSocket[2];                   //@ Our local module port number
    uint8    destSocket[2];                     //@ Port number of what we are connecting to
    union{
      uint8     ipv4_address[4];
      uint32    ipv6_address[4];
    }destIpaddr;
    uint8     max_count[2];
    uint8     tos[4];
    uint8     ssl_bitmap;
    uint8     ssl_ciphers;
    uint8     webs_resource_name[WEBS_MAX_URL_LEN];
    uint8     webs_host_name[WEBS_MAX_HOST_LEN];

  } socketFrameSnd;
    uint8                    uSocketBuf[136];        //@ 24 bytes, byte format to send to spi
} rsi_uSocket;


/*===================================================*/
/**
 * Sockets Structure
 * Structure linking socket number to protocol
 */
typedef struct {
         uint8    ip_version[2];                   //@ ip version
         uint8    socketDescriptor[2];             //@ socket descriptor
         uint8    protocol;                        //@ PROTOCOL_UDP, PROTOCOL_TCP or PROTOCOL_UNDEFINED
         uint8    src_port[2];                     //@ socket local port number
         union{
           uint8     ipv4_address[4];              //@ Destination ipv4 address
           uint32    ipv6_address[4];              //@ Destination ipv6 address
         }dest_ip;
         uint8    dest_port[2];                    //@ Destination port number  
} rsi_socketsStr;


/*===================================================*/
/**
 * Sockets Structure Array
 * Array of Structures linking socket number to protocol
 */
typedef struct {
    rsi_socketsStr      socketsArray[RSI_MAXSOCKETS+1];        
    //@ Socket numbers are from 1 to 8
} rsi_sockets;


/*===================================================*/
/**
 * Socket Close
 */
typedef union {
    struct {
        uint8    socketDescriptor[2];                
        uint8    port_number[2];                   //@ uint16, socket descriptor to close
    } socketCloseFrameSnd;
    uint8                 uSocketCloseBuf[2];      //@ byte format to send to the spi interface, 2 bytes
} rsi_uSocketClose;

/*===================================================*/
/**
 * LTCP socket Connection status
 */
typedef union {
    struct {
       uint8    socketDescriptor[2];              //@ uint16, socket descriptor for LTCP socket
    } queryLtcpConnStatusFrameSnd;
    uint8                 uLtcpConnStatusBuf[2];                    
    //@ byte format to send to the spi interface, 2 bytes
 } rsi_uQueryLtcpConnStatus;


/*===================================================*/
/**
 * sent bytes count
 */
typedef union {
    struct {
       uint8    socketDescriptor[2];              //@ uint16, socket descriptor
    } querySentBytesCountFrameSnd;
    uint8                 uSentBytesStatusBuf[2];                    
    //@ byte format to send to the spi interface, 2 bytes
 } rsi_uQuerySentBytesCount;


/**
 * New commads for WiseConnect
 *
 */

/*===================================================*/
/**
 * FIPS Mode 
 *
 */  
typedef union {
  struct {
    uint8 fips_mode_enable[4];
  } fipsModeFrameSnd;
  uint8         uFipsModeBuf[4];        
} rsi_uFipsMode;

/*===============================================*/
/**
 * RECHECK KEY  Frame Structure
 */
typedef union {
  struct {
    uint16    type; /*0- PMK 1- EAP password*/
    uint8    key[128]; /*PMK/EAP password*/
  } RecheckkeyFrameSnd;
  uint8 uRecheckKeyBuf[2 + 128];
} rsi_urecheck_key;

/*===============================================*/
/**
 * FWUPGRADATION KEY  Frame Structure
 */
typedef union {
  struct {
    uint8    key[16]; /*PMK/EAP password*/
  } FwupgradationkeyFrameSnd;
  uint8 uFwupgradationKeyBuf[16];
} rsi_ufwupgradation_key;

#define RSI_RPS_PAYLOAD_LEN 1024
typedef struct rsi_fw_up_frm_host_s{
  uint8 packet_info[4];
  uint8 payload[RSI_RPS_PAYLOAD_LEN];
}rsi_fw_up_t;

/*===================================================*/
/**
 * Operational Mode 
 *
 */  
typedef union {
    struct {
       uint32    oper_mode;                       //@ operating mode 0-client, 1-p2p, 2-EAP, 6-AP, 8-PER
       uint32    feature_bit_map;                 //@ BIT(0)-Open mode security, BIT(1)-PSK security, BIT(2) JSON objects
       uint32    tcp_ip_feature_bit_map;          //@ BIT(0)-tcp/ip bypass, BIT(1)-HTTP server,BIT(2)-DHCPV4 client, 
                                                  //@ BIT(3)-DHCPV6 client, BIT(4)-DHCPV4 server, BIT(5)-DHCPV6 server
       uint32   custom_feature_bit_map; 
    } operModeFrameSnd;
    uint8    uOperModeBuf[16];                
} rsi_uOperMode;

/*===================================================*/
/**
 * Antenna Select 
 *
 */  
typedef union {
    struct {
        uint8    AntennaVal;                       //@ uint8, Antenna value to set    
    } AntennaSelFrameSnd;
    uint8     AnetnnaReqBuf;    
} rsi_uAntenna;

/*===================================================*/
/**
 * Config p2p command 
 *
 */  
typedef union {
    struct {
        uint8    GOIntent[2];                     //@ GO Intent Value 0-15 for P2p GO or client , 16 - Soft AP 
        uint8    deviceName[64];                  //@ name of the device
        uint8    operChannel[2];                  //@ In which channel we are operating after becomes Group owner
        uint8    ssidPostFix[64];                 //@ Postfix SSID
        uint8    psk[64];                         //@ PSK of the device 
    }configP2pFrameSnd;
    uint8    uConfigP2pBuf[196];
}rsi_uConfigP2p;

/*===================================================*/
/**
 * DNS Server command 
 *
 */  
typedef union {
    struct {
        uint8    ip_version[2];
        uint8    DNSMode[2];
        union{
        uint8    ipv4_address[4];       
        uint32   ipv6_address[4];       
    }primary_dns_ip;
    union{
        uint8    ipv4_address[4];    
        uint32   ipv6_address[4];    
    }secondary_dns_ip;
    }dnsServerFrameSnd;
    uint8       uDnsBuf[36];
}rsi_uDnsServer;



/*===================================================*/
/**
 * DNS query command 
 *
 */  

#define MAX_URL_LEN 90
typedef union {
    struct {
        uint8    ip_version[2];
        uint8    aDomainName[MAX_URL_LEN];        
        uint8    uDNSServerNumber[2];            
    }dnsQryFrameSnd;
    uint8    uDnsQryBuf[MAX_URL_LEN + 4];
}rsi_uDnsQry;

 
/*===================================================*/
/**
 * Config EAP command 
 *
 */  
typedef union {
    struct {
        uint8   eapMethod[32];                    //@ EAP method
        uint8   innerMethod[32];                  //@ Inner method
        uint8   userIdentity[64];                 //@ user name
        uint8   password[128];                    //@ Password    
        int8    okc_enable[4];                    //@Opportunistic Key Caching enable
    }setEapFrameSnd;
    uint8   uSetEapBuf[256 + 4];
}rsi_uSetEap;

/*===================================================*/
/**
 * Web server command 
 *
 */  
#define MAX_URL_LENGTH             40
#define MAX_POST_DATA_LENGTH       512

typedef struct
{
   uint8   url_length;
   uint8   url_name[MAX_URL_LENGTH];
   uint8   request_type;
   uint8   post_content_length[2];
   uint8   post_data[MAX_POST_DATA_LENGTH];

}rsi_urlReqFrameRcv; 

#define MAX_WEBPAGE_SEND_SIZE      1024
typedef struct 
{ 
    uint8   filename[24];
    uint8   total_len[2];
    uint8   current_len[2];
    uint8   has_json_data;
    uint8   webpage[MAX_WEBPAGE_SEND_SIZE];
} WebpageSnd_t;

typedef union 
{
    struct {
    WebpageSnd_t    Webpage_info; 
    }webServFrameSnd;
    uint8     uWebServBuf[1024 + 2 + 2 + 1 + 24]; //@ byte format to send to the spi interface, 1026 bytes 
}rsi_uWebServer;


/*===================================================*/
/**
 * Host Web page command 
 *
 */  
#define MAX_HOST_WEBPAGE_SEND_SIZE   1400

typedef struct 
{ 
    uint8   total_len[4];
    uint8   more_chunks;
    uint8   webpage[MAX_HOST_WEBPAGE_SEND_SIZE];
} HostWebpageSnd_t;


/*===================================================*/
/**
 * Web Fields command 
 *
 */
 
#define MAX_NO_OF_FIELDS  10
#ifdef PACKED 
typedef struct __attribute__((packed)){
#else
typedef struct {
#endif
  uint8   field_index;
  uint8   field_val[64];    
}field_st_t;

typedef union {
#ifdef PACKED 
  struct __attribute__((packed)){
#else
  struct {
#endif
    uint8    field_cnt;
    field_st_t   field_st[MAX_NO_OF_FIELDS];
  }webFieldsFrameSnd;
  uint8   uWebFieldBuf[680];                      //@ byte format to send to the spi interface, 680 bytes 
}rsi_uWebFields;

/*===================================================*/
/**
 * Set Mac Address
 */

typedef union
{
    struct {
        uint8    macAddr[6];                      //@ byte array, mac address
    } setMacAddrFrameSnd;
    uint8   setMacAddrBuf[6];                    
} rsi_uSetMacAddr;

/*===================================================*/
/**
 * Feature select
 */

typedef union
{
    struct {
        uint8    featsel_bitmapVal[4];            //@ uint32, feat select bitmap value to set    
    } FeatselFrameSnd;
    uint8   uFeatselBuf[4];                    
} rsi_uFeatsel;

/*===================================================*/
/**
 * Band
 */

typedef union 
{
    struct {          
        uint8    bandVal;                         //@ uint8, band value to set    
    } bandFrameSnd;
    uint8    uBandBuf;                        
} rsi_uBand;

/*===================================================*/
/**
 * Cfg enable
 */

typedef union
{
    struct {
        uint8    cfg_enable;                      //@ uint8, config enable flag    
    } cfgEnableFrameSnd;
    uint8   ucfgEnableBuf;                        
} rsi_uCfgEnable;

/*===================================================*/
/**
 * Sleep timer
 */

typedef union {
    struct {
        uint8   TimeVal[2];                       //@ uint16, sleep timer value to set    
    } SleepTimerFrameSnd;
    uint8   uSleepTimerBuf[2];                        
} rsi_uSleepTimer;

#define HTTP_BUFFER_LEN 1200

/*===================================================*/
/** 
 *  HTTP GET / POST Request
  */
typedef union {
    struct {
        uint8  ip_version[2];                     //@ ip version 4 or 6
        uint8  https_enable[2];                   //@ enable http features
        uint16 http_port;                         //@ server port number
        uint8  buffer[HTTP_BUFFER_LEN];           //@ Username, Password,Hostname, IP address,url,header,data
    } HttpReqFrameSnd;
    uint8   uHttpReqBuf[HTTP_BUFFER_LEN + 6];
} rsi_uHttpReq;

/*===================================================*/
/**
 * HTTP GET/POST Response
 */
typedef struct TCP_EVT_HTTP_Data_t{
  uint32   more; 
  uint32   offset;
  uint32   data_len;
  uint8    data[1400];
} rsi_uHttpRsp;

/*===================================================*/
/**
 * DNS query struct
 */



#define MAX_DNS_REPLY 10 

typedef struct TCP_EVT_DNS_Query_Resp 
{ 
 uint8   ip_version[2];
 uint8   uIPCount[2];  
 union{
   uint8   ipv4_address[4];
   uint8   ipv6_Address[16];
 }aIPaddr[MAX_DNS_REPLY]; 
}TCP_EVT_DNS_Query_Resp; 

/*===================================================*/
/**
 * Power mode
 */
typedef union {
    struct {
        uint8   powerVal;                         //@ uint8, power value to set
    } powerFrameSnd;
    uint8   uPowerBuf;         
} rsi_uPower;

/*===================================================*/
/**
 * Per mode
 */
typedef union {
    struct {
        uint8   per_mode_enable[2];               //@ uint8, enable/disable per mode
        uint8   power[2];                         //@ uint8, per mode power
        uint8   rate[4];                          //@ uint8, per mode rate
        uint8   length[2];                        //@ uint8, per mode length
        uint8   mode[2];                          //@ uint8, per mode mode
        uint8   channel[2];                       //@ uint8, per mode channel
        uint8   rate_flags[2];                    //@ uint8, per mode rate_flags
        uint8   per_ch_bw[2];                     //@ uint8, per mode per_ch_bw
        uint8   aggr_enable[2];                   //@ uint8, per mode aggr_enable
        uint8   aggr_count[2];                    //@ uint8, per mode aggr_count
        uint8   no_of_pkts[2];                    //@ uint8, per mode no_of_pkts
        uint8   delay[4];                         //@ uint8, per mode delay
    } perModeFrameSnd;
    uint8   uPerModeBuf[28];         
}rsi_uPerMode;

/*===================================================*/
/**
 * Per stats
 */
typedef union {
    struct {
      uint8 per_stats_enable[2];
      uint8 per_stats_channel[2];
    } perStatsFrameSnd;
    uint8   uPerStatsBuf[4];         
}rsi_uPerStats;

/*===================================================*/
/**
 * SEND
 */
typedef union {
    struct {
        uint8   ip_version[2];                    //@ ip version 4 or 6
        uint8   socketDescriptor[2];              //@ socket descriptor of the already opened socket connection
        uint8   sendBufLen[4];                    //@ length of the data to be sent
        uint8   sendDataOffsetSize[2];            //@ Data Offset, TCP=46, UDP=34
        uint8   padding[RSI_TXDATA_OFFSET_TCP + RSI_MAX_PAYLOAD_SIZE+60 ];    
        //@ large enough for TCP or UDP frames
    } sendFrameSnd;
    struct {
        uint8   ip_version[2];                    //@ ip version 4 or 6
        uint8   socketDescriptor[2];              //@ socket descriptor of the already opened socket connection
        uint8   sendBufLen[4];                    //@ length of the data to be sent
        uint8   sendDataOffsetSize[2];            //@ Data Offset, TCP=46, UDP=34
        uint8   sendDataOffsetBuf[RSI_TXDATA_OFFSET_UDP];    
        //@ Empty offset buffer, UDP=34
        uint8   sendDataBuf[RSI_MAX_PAYLOAD_SIZE+60]; 
        //@ Data payload buffer, 1400 bytes max
    } sendFrameUdpSnd;
    struct {
        uint8   ip_version[2];                    //@ ip version 4 or 6
        uint8   socketDescriptor[2];              //@ socket descriptor of the already opened socket connection
        uint8   sendBufLen[4];                    //@ length of the data to be sent
        uint8   sendDataOffsetSize[2];            //@ Data Offset, 46 for TCP, 34 for UDP
        uint8   sendDataOffsetBuf[RSI_TXDATA_OFFSET_TCP];  
        //@ Empty offset buffer, 46 for TCP
        uint8   sendDataBuf[RSI_MAX_PAYLOAD_SIZE+100];    
        //@ Data payload buffer, 1400 bytes max
        uint8   padding[2];                
        //@ data length sent should always be in multiples of 4 bytes
    } sendFrameTcpSnd;
    struct {
        uint8   ip_version[2];                    //@ ip version 4 or 6
        uint8   socketDescriptor[2];              //@ socket descriptor of the already opened socket connection
        uint8   sendBufLen[4];                    //@ length of the data to be sent
        uint8   sendDataOffsetSize[2];            //@ Data Offset, TCP=44, UDP=32
        uint8   destPort[2];
        union{
          uint8   ipv4_address[RSI_IP_ADD_LEN];
          uint32  ipv6_address[RSI_IP_ADD_LEN];
    }destIPaddr;
        uint8   sendDataOffsetBuf[RSI_TXDATA_OFFSET_LUDP];    
        //@ Empty offset buffer, UDP=26
        uint8   sendDataBuf[RSI_MAX_PAYLOAD_SIZE+112];            
        //@ Data payload buffer, 1400 bytes max
    } sendFrameLudpSnd;
    uint8   uSendBuf[RSI_MAX_PAYLOAD_SIZE+112 + RSI_TXDATA_OFFSET_TCP + 8];        
    //@ byte format to send to spi, TCP is the larger of the two, 1456 bytes
} rsi_uSend;

/*===================================================*/
/**
 * Frame Descriptor
 */

typedef union {
    struct {
        uint8   dataFrmLenAndQueue[2];                
        //@ Data frame body length. Bits 14:12=queue, 010 for data, Bits 11:0 are the length
        uint8    padding[14];                     //@ Unused, set to 0x00
 } frameDscDataSnd;
    struct {
        uint8   mgmtFrmLenAndQueue[2];            
        //@ Data frame body length. Bits 14:12=queue, 000 for data, Bits 11:0 are the length
        uint8   mgmtRespType;                
        //@ Management frame descriptor response status, 0x00=success, else error
        uint8   padding[9];                       //@ Unused , set to 0x00
        uint8   mgmtFrmDscRspStatus;
        uint8   padding1[3];
    } frameDscMgmtRsp;
    uint8   uFrmDscBuf[RSI_FRAME_DESC_LEN];       //@ byte format for spi interface, 16 bytes
} rsi_uFrameDsc;

/*===================================================*/
/**
 * Roaming Parameters set structure
 */
typedef union {
    struct {
       uint8   roam_enable[4];
       uint8   roam_threshold[4];
       uint8   roam_hysteresis[4];
   }roamParamsFrameSnd;
 uint8   uRoamParamsBuf[12];
}rsi_uRoamParams;

/*===================================================*/
/**
 *Structure for rejoin_params
 */
typedef struct rsi_rejoin_params_s{
      uint8   rsi_max_try[4];
      int8    rsi_scan_interval[4];
      int8    rsi_beacon_missed_count[4];
      uint8   rsi_first_time_retry_enable[4];
} rsi_rejoin_params_t;


/*===================================================*/
/**
 * HT CAPS Parameters set structure
 */
typedef union { 
  struct {
    uint8    mode_11n_enable[2];
    uint8    ht_caps_bitmap[2];
  }htCapsFrameSnd;
  uint8   uHtCapsBuf[4];
}rsi_uHtCaps;


/*===================================================*/
/**
 * WMM PS Parameters set structure
 */
typedef union {
  struct {
    uint8   wmm_ps_enable[2];
    uint8   wmm_ps_type[2];
    uint8   wmm_ps_wakeup_interval[4];
    uint8   wmm_ps_uapsd_bitmap; 
   }wmmPsFrameSnd;
   uint8  uWmmPsBuf[9];
}rsi_uWmmPs;

/*===================================================*/
/**
 * WPS Parameters set structure
 */
#define RSI_WPS_PIN_LEN  8
typedef union { 
  struct {
    uint8   wps_method[2];
    uint8   generate_pin[2];
    uint8   wps_pin[RSI_WPS_PIN_LEN];
  }wpsMethodFrameSnd;
  uint8   uWpsMethodBuf[12];
}rsi_uWpsMethod;

/*===================================================*/
/**
 * Command Response Frame Union
 */
typedef struct {
    uint8   devState;                             //@ If New device  1; Device left 0
    uint8   devName[32];                          //@ Name the device found or left 32 bytes
    uint8   macAddress[6];                        //@ Mac address of the device 
    uint8   devtype[2];                           //@ Type of the device 1st byte inidcates primary device type;
                                                  //@ 2nd byte indicates sub catagory
}rsi_wfdDevInfo;

typedef struct {     
    rsi_wfdDevInfo  devInfo;    
}rsi_wfdDevRsp;

typedef struct {
       uint8   devCount;
       rsi_wfdDevInfo   strWfdDevInfo[RSI_MAX_WFD_DEV_CNT];    
                                                   //@ 32 maximum responses from scan command    
} rsi_wfdDevResponse;


typedef struct {
    uint8   rfChannel;                             //@ rf channel to us, 0=scan for all
    uint8   securityMode;                          //@ security mode, 0=open, 1=wpa1, 2=wpa2, 3=wep
    uint8   rssiVal;                               //@ absolute value of RSSI
    uint8   uNetworkType;
    uint8   ssid[RSI_SSID_LEN];                    //@ 32-byte ssid of scanned access point
    uint8   bssid[RSI_BSSID_LEN];
#ifndef RSI_FEATSEL_ENABLE
    uint8   reserved[2];
#else
    uint8   snr;
    uint8   reserved;
    uint8   ap_name[16];
#endif
} rsi_scanInfo;

typedef struct {
    uint8   nwType;                                //@ network type, 0=Ad-Hoc (IBSS), 1=Infrastructure
    uint8   securityType;                          //@ security type, 0=Open, 1=WPA1, 2=WPA2, 3=WEP
    uint8   dataRate;                              //@ data rate, 0=auto, 1=1Mbps, 2=2Mbps, 3=5.5Mbps, 4=11Mbps, 12=54Mbps
    uint8   powerLevel;                            //@ transmit power level, 0=low (6-9dBm), 1=medium (10-14dBm, 2=high (15-17dBm)
    uint8   psk[RSI_PSK_LEN];                      //@ pre-shared key, 32-byte string
    uint8   ssid[RSI_SSID_LEN];                    //@ ssid of access point to join to, 32-byte string
    uint8   ibssMode;                              //@ Ad-Hoc Mode (IBSS), 0=Joiner, 1=Creator
    uint8   ibssChannel;                           //@ rf channel number for Ad-Hoc (IBSS) mode
    uint8   reserved;
} rsi_joinInfo;

typedef struct {
    uint8   scanCount[4];                          //@ uint32, number of access points found
    uint8   padding[4];
    rsi_scanInfo    strScanInfo[RSI_AP_SCANNED_MAX];    
                                                   //@ 32 maximum responses from scan command
} rsi_scanResponse;

typedef struct {
  uint8   macAddress[6];
}rsi_initResponse;

typedef struct {
    uint8   macAddress[6];
}rsi_qryMacFrameRcv;

typedef struct {
    uint8   operState;    
}rsi_joinResponse;

typedef struct {
    uint8   rssiVal[2];                           //@ uint8, RSSI value for the device the module is currently connected to
} rsi_rssiFrameRcv;

typedef struct {
    uint8   snrVal;                               //@ uint8, RSSI value for the device the module is currently connected to
} rsi_snrFrameRcv;


typedef struct {
    uint8   ip_version[2];                       //@ ip version 4 or 6  
    uint8   socketType[2];                       //@ uint16, type of socket created
    uint8   socketDescriptor[2];                 //@ uinr16, socket descriptor, like a file handle, usually 0x00
    uint8   moduleSocket[2];                     //@ uint16, Port number of our local socket
    union{
      uint8   ipv4_addr[4];                      //@ uint32, Our (module) IPv4 Address
      uint8   ipv6_addr[16];                     //@ uint32, Our (module) IPv6 Address
  }moduleIPaddr;
  uint16   mss;
} rsi_socketFrameRcv;

typedef struct {
    uint8   socketDsc[2];                         //@ uint16, socket that was closed
    uint8   bytesSent[4];                         //@ uint32, sent bytes count
} rsi_socketCloseFrameRcv;

typedef struct {
    uint8    macAddr[6];                          //@ MAC address of this module
    uint8    ipaddr[4];                           //@ Configured IP address
    uint8    netmask[4];                          //@ Configured netmask
    uint8    gateway[4];                          //@ Configured default gateway
} rsi_ipparamFrameRcv;

typedef struct {
    uint8   macAddr[6];                           //@ MAC address of this module
    uint8   ipaddr[4];                            //@ Configured IP address
    uint8   netmask[4];                           //@ Configured netmask
    uint8   gateway[4];                           //@ Configured default gateway
} rsi_recvIpChange;



typedef struct {
    uint8   prefixLength[2];                      //@ prefix length
    uint8   ipaddr6[16];                          //@ Configured IPv address
    uint8   defaultgw6[16];                       //@ Router IPv6 address
} rsi_ipconf6FrameRcv;

typedef struct {
    uint8   object_id[32];                         //@ Object id  
    uint8   length[4];                             //@ Length of set request
    uint8   value[152];                            //@ value to be set
} rsi_snmp_set;

typedef struct {
    uint8   wps_pin[RSI_WPS_PIN_LEN];
}rsi_wpsMethodFrameRcv;

/*Region Code response in set region command*/
typedef struct 
{
  //! region code selected
  uint8 region_code;
}rsi_uSetRegionRsp;

typedef struct {
    uint8   state[2];                             //@ uint16, connection state, 0=Not Connected, 1=Connected
} rsi_conStatusFrameRcv;

typedef struct {

   uint8  sock_handle[2];
   uint8  SentBytes[4];

} rsi_sentBytesRsp;

typedef struct {
    uint8   socketDescriptor[2];
  uint8 ip_version[2];
  union
  {
    uint8   ipv4_address[4];
    uint8   ipv6_address[16];
  }dest_ip;
    uint8   dPort[2];
} rsi_LtcpConnStatusFrameRcv;

typedef struct sock_info_query_t
{
    uint8   sock_id[2];
    uint8   sock_type[2];
    uint8   sPort[2];
    uint8   dPort[2];
    union{
        uint8   ipv4_address[4];
        uint8   ipv6_address[16];
      }destIpaddr;
    
}sock_info_query_t;

#define MN_NUM_SOCKETS             10 
typedef struct {
    uint8   wlanState;                            //@ uint8, 0=NOT Connected, 1=Connected
    uint8   Chn_no;                               //@ channel number of connected AP 
    uint8   psk[64];                              //@ PSK 
    uint8   mac_addr[6];                          //@ Mac address
    uint8   ssid[RSI_SSID_LEN];                   //@ uint8[32], SSID of connected access point
    uint8   connType[2];                          //@ uint16, 0=AdHoc, 1=Infrastructure
    uint8   sec_type;
    uint8   dhcpMode;                             //@ uint8, 0=Manual IP Configuration,1=DHCP
    uint8   ipaddr[4];                            //@ uint8[4], Module IP Address
    uint8   subnetMask[4];                        //@ uint8[4], Module Subnet Mask
    uint8   gateway[4];                           //@ uint8[4], Gateway address for the Module
    uint8   num_open_socks[2];                    //@ number of sockets opened
    uint8   prefix_length[2];                     //@ prefix length for ipv6 address
    uint8   ipv6addr[16];                         //@ modules ipv6 address
    uint8   defaultgw6[16];                       //@ router ipv6 address
    uint8   tcp_stack_used;                       //@ BIT(0) =1 - ipv4, BIT(1)=2 - ipv6, BIT(0) & BIT(1)=3 - BOTH
    sock_info_query_t   socket_info[MN_NUM_SOCKETS];    
} rsi_qryNetParmsFrameRcv;


#define MAX_STA_SUPPORT 4

struct  go_sta_info_s
{
   uint8   ip_version[2];                         //@ IP version if the connected client
   uint8   mac[6];                                //@ Mac Address of the connected client
  union
  {
    uint8   ipv4_address[4];                      //@ IPv4 Address of the Connected client
    uint8   ipv6_address[16];                     //@ IPv6 Address of the Connected client
  }ip_address;
#ifdef PACKED
}__attribute__((packed));                         //@ to avoid padding in the structures
#else
};
#endif

typedef struct {
    uint8   ssid[RSI_SSID_LEN];                   //@ SSID of the P2p GO
    uint8   bssid[6];                             //@ BSSID of the P2p GO
    uint8   channel_number[2];                    //@ Operating channel of the GO 
    uint8   psk[64];                              //@ PSK of the GO
    uint8   ipv4_address[4];                      //@ IPv4 Address of the GO
    uint8   ipv6_address[16];                     //@ IPv6 Address of the GO
    uint8   sta_count[2];                         //@ Number of stations Connected to GO
    struct  go_sta_info_s sta_info[MAX_STA_SUPPORT];
}rsi_qryGOParamsFrameRcv;



typedef struct {
    uint8    fwversion[20];                       //@ uint8[20], firmware version text string, x.y.z as 1.3.0
} rsi_qryFwversionFrameRcv;


typedef struct {
    uint8   ip_version[2];                        //@ uint16, the ip version of the ip address , 4 or 6
    uint8   recvSocket[2];                        //@ uint16, the socket number associated with this read event
    uint8   recvBufLen[4];                        //@ uint32, length of data received
    uint8   recvDataOffsetSize[2];                //@ uint16, offset of data from start of buffer
    uint8   fromPortNum[2];                       //@ uint16, port number of the device sending the data to us
    union{
      uint8   ipv4_address[4];                    //@ uint32, IPv4 Address of the device sending the data to us
      uint8   ipv6_address[16];                   //@ uint32, IPv6 Address of the device sending the data to us
  }fromIPaddr;
    uint8    recvDataOffsetBuf[RSI_RXDATA_OFFSET_UDP_V4]; 
                                                  //@ uint8, empty offset buffer, 14 for UDP, 42 bytes from beginning of buffer
    uint8    recvDataBuf[RSI_MAX_PAYLOAD_SIZE];   //@ uint8, buffer with received data
} rsi_recvFrameUdp;

typedef struct {
      uint8   ip_version[2];                      //@ uint16, the ip version of the ip address , 4 or 6
      uint8   recvSocket[2];                      //@ uint16, the socket number associated with this read event
      uint8   recvBufLen[4];                      //@ uint32, length of data received
      uint8   recvDataOffsetSize[2];              //@ uint16, offset of data from start of buffer
      uint8   fromPortNum[2];                     //@ uint16, port number of the device sending the data to us
    union{
        uint8   ipv4_address[4];                  //@ uint32, IPv4 Address of the device sending the data to us
        uint8   ipv6_address[16];                 //@ uint32, IPv6 Address of the device sending the data to us
  }fromIPaddr;
    uint8   recvDataOffsetBuf[RSI_RXDATA_OFFSET_UDP_V6];
    //@ uint8, empty offset buffer, 14 for UDP, 42 bytes from beginning of buffer
    uint8   recvDataBuf[RSI_MAX_PAYLOAD_SIZE];    //@ uint8, buffer with received data
} rsi_recvFrameUdp6;


typedef struct { 
      uint8   ip_version[2];                      //@ uint16, the ip version of the ip address , 4 or 6
      uint8   recvSocket[2];                      //@ uint16, the socket number associated with this read event
      uint8   recvBufLen[4];                      //@ uint32, length of payload data received
      uint8   recvDataOffsetSize[2];              //@ uint16, offset of data from start of buffer
      uint8   fromPortNum[2];                     //@ uint16, port number of the device sending the data to us
    union{
       uint8   ipv4_address[4];                   //@ uint32, IPv4 Address of the device sending the data to us
       uint8   ipv6_address[16];                  //@ uint32, IPv6 Address of the device sending the data to us
  }fromIPaddr;
    uint8   recvDataOffsetBuf[RSI_RXDATA_OFFSET_TCP_V4]; //@ uint8, empty offset buffer, 26 for TCP
    uint8   recvDataBuf[RSI_MAX_PAYLOAD_SIZE];    //@ uint8, buffer with received data
} rsi_recvFrameTcp;

typedef struct { 
    uint8   ip_version[2];                        //@ uint16, the ip version of the ip address , 4 or 6
    uint8   recvSocket[2];                        //@ uint16, the socket number associated with this read event
    uint8   recvBufLen[4];                        //@ uint32, length of payload data received
    uint8   recvDataOffsetSize[2];                //@ uint16, offset of data from start of buffer
    uint8   fromPortNum[2];                       //@ uint16, port number of the device sending the data to us
    union{
       uint8   ipv4_address[4];                   //@ uint32, IPv4 Address of the device sending the data to us
       uint8   ipv6_address[16];                  //@ uint32, IPv6 Address of the device sending the data to us
  }fromIPaddr;
    uint8   recvDataOffsetBuf[RSI_RXDATA_OFFSET_TCP_V6]; //@ uint8, empty offset buffer, 26 for TCP
    uint8   recvDataBuf[RSI_MAX_PAYLOAD_SIZE];    //@ uint8, buffer with received data
} rsi_recvFrameTcp6;

typedef struct {
    uint8   socket[2];                                //@ uint8, socket handle for the terminated connection
} rsi_recvRemTerm;

typedef struct{
 union{
  uint8   ipv4_address[4];                         //@ primary DNS IPv4
  uint8   ipv6_address[16];                        //@ primary DNS IPv6
 }primary_dns_ip;

 union{
    uint8   ipv4_address[4];                       //@ secondary DNS IPv4
    uint8   ipv6_address[16];                      //@ secondary DNS IPv6
 }secondary_dns_ip;
}rsi_dnsserverResponse;

typedef struct {
    uint8   ip_version[2];
    uint8   sock_id[2];                           //@ uint16, socket handle 
    uint8   fromPortNum[2];                       //@ uint16, remote port number 
    union{
        uint8   ipv4_address[4];                  //@  remote IPv4 Address 
        uint8   ipv6_address[16];                 //@  remote IPv6 Address 
        }dst_ip_address;
    uint8   mss[2];                               //@ uint32, remote IP Address 
} rsi_recvLtcpEst;

/* Certificate loading related macros */
#define MAX_CERT_SEND_SIZE 1400

struct cert_info_s
{
  uint8   total_len[2];
  uint8   CertType;
  uint8   more_chunks;
  uint8   CertLen[2];
  uint8   KeyPwd[128];
#ifdef PACKED
}__attribute__((packed));                         //@packed is used to avoid padding
#else
};
#endif

#define MAX_DATA_SIZE (MAX_CERT_SEND_SIZE - sizeof(struct cert_info_s))

struct SET_CHUNK_S
{ 
    struct  cert_info_s cert_info;    
    uint8   Certificate[MAX_DATA_SIZE];
};


/*==================================================*/
/*This will keep the wepkey params*/

typedef struct {
    uint8   index[2];
    uint8   key[4][32];
}rsi_wepkey;

/*====================================================*/
/*This will keep the AP configuration parameter*/

typedef struct {
    uint8   channel_no[2];    
    uint8   ssid[RSI_SSID_LEN];    
    uint8   security_type;
    uint8   encryp_mode;    
    uint8   psk[RSI_PSK_LEN];
    uint8   beacon_interval[2];    
    uint8   dtim_period[2];    
    uint8   reserved1[2];
    uint8   max_sta_support[2]; // it can be configured from 1 to 4
}rsi_apconfig;

/*SNMP command structure*/
#define MAX_SNMP_VALUE 200
typedef union {
    struct {
        uint8   type;
        uint8   value[MAX_SNMP_VALUE];     
    } snmpFrameSnd;
    uint8   uSnmpBuf[MAX_SNMP_VALUE + 4];
} rsi_uSnmp;

/*SNMP trap structure*/
typedef union {
    struct {
    uint8   snmp_version[4];
    uint8   ip_version[4];
    union{
      uint8   ipv4_address[4];     
      uint32  ipv6_address[4];
    }destIPaddr;
    uint8   community[32];
    uint8   trap_type;
    uint8   elapsed_time[4]; 

    } snmptrapFrameSnd;
    uint8   uSnmptrapBuf[61];
} rsi_uSnmptrap;

/* Structure for SNMP Enable */
typedef union{
  struct{
    uint8   snmpEnable;
 }snmpEnableFrameSnd;
  uint8   uSnmpenableBuf;
}rsi_uSnmpEnable;


/* Ping Response Frame */
typedef struct {
	uint8   ip_version[2];
	uint8   ping_size[2]; 
	union{
		uint8   ipv4_addr[4];                         //@ uint32, Our (module) IPv4 Address
		uint8   ipv6_addr[16];                        //@ uint32, Our (module) IPv6 Address
	}ping_address;
} rsi_uPingRsp;

/*structure for ping request command*/
typedef struct rsi_ping_request_s{
  uint8   ip_version[2];
  uint8   ping_size[2];
  union{
    uint8   ipv4_address[4];
    uint32  ipv6_address[4];
  }ping_address;
}rsi_ping_request_t;

/*PSK response structure*/
typedef struct{
  uint8   pmk[32];
}rsi_PmkResponse;

/*P2P connection request from wi-fi device*/
typedef struct rsi_p2p_conn_req_s{
  uint8   device_name[32];    
}rsi_p2p_conn_req_t;

/* Module state response */
typedef struct rsi_state_notificaton_s{
    uint8   TimeStamp[4];
    uint8   stateCode;
    uint8   reason_code;
    uint8   rsi_channel;
    uint8   rsi_rssi;
    uint8   rsi_bssid[6];
} rsi_state_notificaton_t;


/*User store configuration parameters*/
typedef struct sc_params_s
{
	uint8    cfg_enable;
	uint8    opermode[4];
	uint8    feature_bit_map[4];
	uint8    tcp_ip_feature_bit_map[4];
	uint8    custom_feature_bit_map[4];
	uint8    band;
	uint8    NetworkType;
	uint8    join_ssid[RSI_SSID_LEN];
	uint8    uRate;
	uint8    uTxPower;
	uint8    reserved_1;
	uint8    reserved_2;
	uint8    scan_ssid_len;
	uint8    reserved_3;
	uint8    csec_mode;
	uint8    psk[RSI_PSK_LEN];
	uint8    scan_ssid[RSI_SSID_LEN];
	uint8    scan_cnum;
	uint8    dhcp_enable;
#define IP_ADDRESS_SZ 4
	uint8    ip[IP_ADDRESS_SZ];
	uint8    sn_mask[IP_ADDRESS_SZ];
	uint8    dgw[IP_ADDRESS_SZ];

	uint8    eap_method[32];
	uint8    inner_method[32];
	uint8    user_identity[64];
	uint8    passwd[128];

	uint8    go_intent[2];
	uint8    device_name[64];
	uint8    operating_channel[2];
	uint8    ssid_postfix[64];
	uint8    psk_key[64];
#define  WISE_PMK_LEN 32
	uint8    pmk[WISE_PMK_LEN];
	rsi_apconfig apconfig;
	uint8    module_mac[6];
	uint8    antenna_select[2];
	uint8    reserved_5[2];
	rsi_wepkey wep_key;
	uint8    dhcp6_enable[2];
	uint8    prefix_length[2];
	uint8    ip6[16];
	uint8    dgw6[16];
	uint8    tcp_stack_used;
	uint8    bgscan_magic_code[2];
	uint8    bgscan_enable[2];
	uint8 	 bgscan_threshold[2];
	uint8 	 rssi_tolerance_threshold[2];
	uint8 	 bgscan_periodicity[2];
	uint8 	 active_scan_duration[2];
	uint8 	 passive_scan_duration[2];
	uint8    multi_probe;
	//!Channel bitmap info
	uint8    chan_bitmap_magic_code[2];
	uint8    scan_chan_bitmap_stored_2_4_GHz[4];
	uint8    scan_chan_bitmap_stored_5_GHz[4];
	//!Roaming Params info
	uint8    roam_magic_code[2];
	rsi_uRoamParams  roam_params_stored;
	//!rejoin params info
	uint8    rejoin_magic_code[2];
	rsi_rejoin_params_t rejoin_param_stored;
	uint8     region_request_from_host;
	uint8     rsi_region_code_from_host;
	uint8     region_code;

}rsi_user_store_config_t;


typedef struct {
    uint8   dev_name[32];                         //@ All the characters are part of device name, no Null termination 
} rsi_ConnAcceptRcv;

typedef struct {    
   	uint8    cfg_enable;
	uint8    opermode[4];
	uint8    feature_bit_map[4];
	uint8    tcp_ip_feature_bit_map[4];
	uint8    custom_feature_bit_map[4];
	uint8    band;
	uint8    NetworkType;
	uint8    join_ssid[RSI_SSID_LEN];
	uint8    uRate;
	uint8    uTxPower;
	uint8    reserved_1;
	uint8    reserved_2;
	uint8    scan_ssid_len;
	uint8    reserved_3;
	uint8    csec_mode;
	uint8    psk[RSI_PSK_LEN];
	uint8    scan_ssid[RSI_SSID_LEN];
	uint8    scan_cnum;
	uint8    dhcp_enable;
#define IP_ADDRESS_SZ 4
	uint8    ip[IP_ADDRESS_SZ];
	uint8    sn_mask[IP_ADDRESS_SZ];
	uint8    dgw[IP_ADDRESS_SZ];

	uint8    eap_method[32];
	uint8    inner_method[32];
	uint8    user_identity[64];
	uint8    passwd[128];

	uint8    go_intent[2];
	uint8    device_name[64];
	uint8    operating_channel[2];
	uint8    ssid_postfix[64];
	uint8    psk_key[64];
#define  WISE_PMK_LEN 32
	uint8    pmk[WISE_PMK_LEN];
	rsi_apconfig apconfig;
	uint8    module_mac[6];
	uint8    antenna_select[2];
	uint8    reserved_5[2];
	rsi_wepkey wep_key;
	uint8    dhcp6_enable[2];
	uint8    prefix_length[2];
	uint8    ip6[16];
	uint8    dgw6[16];
	uint8    tcp_stack_used;
	uint8    bgscan_magic_code[2];
	uint8    bgscan_enable[2];
	uint8 	 bgscan_threshold[2];
	uint8 	 rssi_tolerance_threshold[2];
	uint8 	 bgscan_periodicity[2];
	uint8 	 active_scan_duration[2];
	uint8 	 passive_scan_duration[2];
	uint8    multi_probe;
	//!Channel bitmap info
	uint8    chan_bitmap_magic_code[2];
	uint8    scan_chan_bitmap_stored_2_4_GHz[4];
	uint8    scan_chan_bitmap_stored_5_GHz[4];
	//!Roaming Params info
	uint8    roam_magic_code[2];
	rsi_uRoamParams  roam_params_stored;
	//!rejoin params info
	uint8    rejoin_magic_code[2];
	rsi_rejoin_params_t rejoin_param_stored;
	uint8     region_request_from_host;
	uint8     rsi_region_code_from_host;
	uint8     region_code;
} rsi_cfgGetFrameRcv;


/* PER stats response */
typedef struct per_stats_s
{
   //! no. of tx pkts
  uint8 tx_pkts[2];
  //! no. of rx pkts
  uint8 reserved_1[2];
  //! no. of tx retries
  uint8 tx_retries[2];
  //! no. of pkts that pass crc
  uint8 crc_pass[2];
  //! no. of pkts failing crc chk
  uint8 crc_fail[2];
  //! no. of times cca got stuck
  uint8 cca_stk[2];
  //! no of times cca didn't get stuck
  uint8 cca_not_stk[2];
  //! no. of pkt aborts
  uint8 pkt_abort[2];
  //! no. of false rx starts
  uint8 fls_rx_start[2];
  //! cca idle time
  uint8 cca_idle[2];
  //! Reserved fields
  uint8 reserved_2[26];
  //! no. of rx retries
  uint8 rx_retries[2];
  //! rssi value
  uint8 reserved_3[2];
  //! cal_rssi
  uint8 cal_rssi[2];
  //! lna_gain bb_gain
  uint8 reserved_4[4];
	//! xretries pkts dropped
  //! number of tx packets dropped after maximum retries 
	uint8 xretries[2];
	//! consecutive pkts dropped
	uint8 max_cons_pkts_dropped[2];
  uint8 reserved_5[2];

}rsi_uPerStatsRsp;


/* Card ready Response */
typedef struct card_ready_s
{
  //! Boot loader checksum
  uint8 bootloader_checksum[4];

  //! Firmware checksum
  uint8 firmware_checksum[4];
}rsi_card_readyRsp;

/* Store Config checksum Response */
typedef struct store_config_chksum_s
{
  uint8 checksum[20];
}rsi_store_config_checksumRsp;

//! Generate DRBG random
typedef struct generate_drbg_rand_s
{
  uint8 random_number[32];
}rsi_generate_DRBGRandRsp;

/** End of the certificate loading structure **/

typedef struct {
    uint8   rspCode[2];
    uint8   status[2];                  
    //@ 0- For Success ,Non-Zero Value is the Error Code return
    union {
        //@ response payload    
        rsi_qryMacFrameRcv              qryMacaddress;
        rsi_initResponse                initResponse;          
        rsi_scanResponse                scanResponse;
        rsi_joinResponse                joinResponse;
        rsi_PmkResponse                 PmkResponse;   
        rsi_wfdDevResponse              wfdDevResponse;
        rsi_rssiFrameRcv                rssiFrameRcv;
        rsi_socketFrameRcv              socketFrameRcv;
        rsi_socketCloseFrameRcv         socketCloseFrameRcv;
        rsi_ipparamFrameRcv             ipparamFrameRcv;
        rsi_recvIpChange                recvIpchange;
        rsi_ipconf6FrameRcv             ipconf6FrameRcv;
        rsi_conStatusFrameRcv           conStatusFrameRcv;
        rsi_qryNetParmsFrameRcv         qryNetParmsFrameRcv;
        rsi_qryGOParamsFrameRcv         qryGoParamsFrameRcv;
        rsi_qryFwversionFrameRcv        qryFwversionFrameRcv;
        rsi_dnsserverResponse           dnsserverresponse;    
        rsi_recvFrameUdp                recvFrameUdp;
        rsi_recvFrameTcp                recvFrameTcp;  
        rsi_recvFrameUdp6               recvFrameUdp6;
        rsi_recvFrameTcp6               recvFrameTcp6; 
        rsi_recvRemTerm                 recvRemTerm;
        rsi_recvLtcpEst                 recvLtcpEst;
        TCP_EVT_DNS_Query_Resp          dnsqryresponse;
        rsi_snrFrameRcv                 snrFrameRcv;    
        rsi_cfgGetFrameRcv              cfgGetFrameRcv;
        rsi_LtcpConnStatusFrameRcv      LtcpConnStatRcv;
        rsi_urlReqFrameRcv              urlReqRcv;
        rsi_p2p_conn_req_t              dev_req;
        rsi_ConnAcceptRcv               ConnectReqRcv;
        rsi_snmp_set                    snmp_set;
        rsi_wpsMethodFrameRcv           wpsMethodFrameRcv;
        rsi_state_notificaton_t         stateFrameRcv;
        rsi_uHttpRsp                    httpFrameRcv;
        rsi_uSetRegionRsp               setRegFrameRcv;
        rsi_uPingRsp                    rsi_pingFrameRcv;
        rsi_uPerStatsRsp                rsi_perFrameRcv;
        rsi_card_readyRsp               rsi_card_readyRcv;
        rsi_store_config_checksumRsp    rsi_cfg_chksumRcv;
        rsi_generate_DRBGRandRsp        rsi_generateDRBGrandRcv; 
		    rsi_sentBytesRsp                SentBytes;
        uint8                           uCmdRspBuf[RSI_FRAME_CMD_RSP_LEN + RSI_MAX_PAYLOAD_SIZE + RSI_WPS_PIN_LEN + 100];
    }uCmdRspPayLoad;
} rsi_uCmdRsp;

/*==============================================*/
/**
 * Main struct which defines all the paramaters of the API library
 * This structure is defined as a variable, the  pointer to, which is passed
 *  toall the functions in the API
 * The struct is initialized by a set of #defines for default values
 *  or for simple use cases
 */

typedef struct {
    uint8                    band;                //@ uint8, frequency band to use
    rsi_uOperMode            opermode;
    uint8                    powerMode;           //@ uint16, power mode, 0=power mode0, 1=power mode 1, 2=power mode 2
    uint8                    antSel;              //@ 1 for internal antenna 2 for external antenna
    uint8                    macAddress[6];       //@ 6 bytes, mac address
    rsi_uScan                uScanFrame;          //@ frame sent as the scan command
    rsi_ubgScan              ubgScanFrame;        //@ frame sent as the bg scan command
    rsi_uJoin                uJoinFrame;          //@ frame sent as the join command
    rsi_uPsk                 uPskFrame;
    rsi_uIpparam             uIpparamFrame;       //@ frame sent to set the IP parameters
    rsi_uIPconf6             uIpconf6Frame;       //@ frame sent to set the IPV6 parameters
    rsi_uSocket              uSocketFrame;
    rsi_uWebServer           uWebData;
    rsi_uWebFields           uWebField;
    rsi_uSetEap              usetEap;
    rsi_uConfigP2p           uconfigP2p;
    rsi_uDnsServer           uDnsServer;
    rsi_uDnsQry              uDnsQry;
    rsi_uHttpReq             uHttpGetReq;
    rsi_uHttpReq             uHttpPostReq;  
    rsi_uSleepTimer          uSleepTimer;
    uint8                    cfg_enable;
    rsi_apconfig             apconf_data;
    rsi_wepkey               wepkeys;
    rsi_disassoc_t           disassoc_frame; 
    HostWebpageSnd_t         UrlRsp;
    rsi_ping_request_t       ping;
    rsi_uFeatsel             uFeatsel;            //@feature select
    uint8                    snmp_enable;
    rsi_uSnmp                uSnmp;
    rsi_uSnmptrap            uSnmptrap;
    rsi_uRoamParams          uRoamParams;
    rsi_rejoin_params_t      rejoin_param;
    rsi_uHtCaps              uHtCaps;
    rsi_uWmmPs               uWmmPs;
    rsi_uWpsMethod           uWpsMethod;
    rsi_uPerMode             uPerMode;
    rsi_uPerStats            uPerStats;
    rsi_uQueryLtcpConnStatus uQueryLtcpConnStatus;
    rsi_jsonCreateObject_t   json;
    rsi_tfs_clear_files_t    clear_files;
    rsi_uMulticast           uMulticastFrame;
    rsi_uDebug               uDebugFrame;
    rsi_usetregion           SetRegion;
    rsi_usetregion_ap_t      SetRegionAp;
    rsi_user_store_config_t  userstorecfg;
    rsi_fw_up_t              fw_up_params;
    rsi_urecheck_key         re_key;
	  rsi_uFipsMode            uFipsmode;
    rsi_ufwupgradation_key   fwupgradation_key;
} rsi_api;

/*===================================================*/
/**
 * Interrupt Handeling Structure
 */
typedef struct {
    uint8   mgmtPacketPending;                    //@ TRUE for management packet pending in module
    uint8   dataPacketPending;                    //@TRUE for data packet pending in module
    uint8   powerIrqPending;                      //@ TRUE for power interrupt pending in the module
    uint8   bufferFull;                           //@ TRUE=Cannot send data, FALSE=Ok to send data
    uint8   bufferEmpty;                          //@TRUE, Tx buffer empty, seems broken on module
    uint8   isrRegLiteFi;
} rsi_intStatus;




/*==============================================*/

typedef struct {
      uint8    *ptrRecvBuf;                       //@ Location of the payload data
      uint32   recvBufLen;                        //@ Length of the payload data
      int      recvType;                          //@ Whether this receive event was a remote disconnect or read data
      uint16   socketNumber;                      //@ The socket number the event is associated with
} recvArgs;




/*==================================================*/
/**
 * This structure maintain power save state.
 *
 */ 
typedef struct {
    uint8   current_mode;
    uint8   ack_pwsave;
    uint8   sleep_received;
    uint8   ack_sent;
}rsi_powerstate;

//! Debug Print Levels
#define RSI_DEBUG_LVL         0x00ff
//! These bit values may be ored to all different combinations of debug printing
#define RSI_PL0                0xffff
#define RSI_PL1                0x0001
#define RSI_PL2                0x0002
#define RSI_PL3                0x0004
#define RSI_PL4                0x0008
#define RSI_PL5                0x0010
#define RSI_PL6                0x0020
#define RSI_PL7                0x0040
#define RSI_PL8                0x0080
#define RSI_PL9                0x0100
#define RSI_PL10               0x0200
#define RSI_PL11               0x0400
#define RSI_PL12               0x0800
#define RSI_PL13               0x1000
#define RSI_PL14               0x2000
#define RSI_PL15               0x4000
#define RSI_PL16               0x8000

//#define RSI_DPRINT(lvl, fmt, args)              if (lvl & DEBUG_LVL) printf(fmt, ##args)



enum RSI_INTERRUPT_TYPE {
    RSI_TXBUFFER_FULL       = 0x01,
    RSI_TXBUFFER_EMPTY      = 0x02,
    RSI_MGMT_PENDING        = 0x04,
    RSI_DATA_PENDING        = 0x08,
    RSI_PWR_MODE            = 0x10
};

/**
 * Enumerations
 */
enum RSI_PROTOCOL {
    RSI_PROTOCOL_UDP_V4     = 0x00,
    RSI_PROTOCOL_TCP_V4     = 0x01,
    RSI_PROTOCOL_UDP_V6     = 0x02,
    RSI_PROTOCOL_TCP_V6     = 0x03
};

#define BUFFER_FULL_FAILURE -3

/* Status indications */
#define BUFFER_FULL     0x01
#define BUFFER_EMPTY    0x02
#define RX_PKT_PENDING  0x08
#define POWER_SAVE      0x08

#define INDEX_SIZE (3 * 1024)
#define WEBFIELD_SIZE 680


#define RSI_STATUS_OFFSET         12
#define RSI_TWOBYTE_STATUS_OFFSET 12 
#define RSI_RSP_TYPE_OFFSET       2

extern uint8 index_htm[INDEX_SIZE];
extern uint8 web_htm[WEBFIELD_SIZE];
extern struct SET_CHUNK_S set_chunks;
extern rsi_api                    rsi_strApi;
extern rsi_wfdDevResponse wfdDevData;
extern uint32   interrupt_rcvd;

#include "rsi_hal.h"
#include "platform_specific.h"
#include "rsi_config.h"
#include "rsi_api.h"
#ifdef LINUX_PLATFORM
#include "string.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
