//*****************************************************************************
//
// uip-conf.h - uIP Project Specific Configuration File
//
// Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the DK-LM3S9D96 Firmware Package.
//
//*****************************************************************************

#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

#include "uipopt.h"
#include "stm32_eth.h"

//
// 8 bit datatype
// This typedef defines the 8-bit type used throughout uIP.
//
typedef unsigned char u8_t;

//
// 16 bit datatype
// This typedef defines the 16-bit type used throughout uIP.
//
typedef unsigned short u16_t;

//
// 32 bit datatype
// This typedef defines the 32-bit type used throughout uIP.
//
typedef unsigned long u32_t;

//
// Statistics datatype
// This typedef defines the dataype used for keeping statistics in
// uIP.
//
typedef unsigned short uip_stats_t;

//
// Ping IP address assignment
// Use first incoming "ping" packet to derive host IP address
//
#define UIP_CONF_PINGADDRCONF       1  //2016.12.20

//
// UDP support on or off
//
#define UIP_CONF_UDP                1	

//
// UDP checksums on or off
// (not currently supported ... should be 0)
//
#define UIP_CONF_UDP_CHECKSUMS      1	

//
// UDP Maximum Connections
//
#define UIP_CONF_UDP_CONNS          4

//
// Maximum number of TCP connections.
//
#define UIP_CONF_MAX_CONNECTIONS    8

//
// Maximum number of listening TCP ports.
//
#define UIP_CONF_MAX_LISTENPORTS    5

//
// Size of advertised receiver's window
//
//#define UIP_CONF_RECEIVE_WINDOW     400

//
// Size of ARP table
//
#define UIP_CONF_ARPTAB_SIZE        8

//
// uIP buffer size.
//
#define UIP_CONF_BUFFER_SIZE        (ETH_HEADER + ETH_PAYLOAD)

//
// uIP buffer is defined in the application.
//
#define UIP_CONF_EXTERNAL_BUFFER    0
//
// uIP statistics on or off
//
#define UIP_CONF_STATISTICS         0

//
// Logging on or off
//
#define UIP_CONF_LOGGING            0

//
// Broadcast Support
//
#define UIP_CONF_BROADCAST          1   //2016.12.20

//
// Link-Level Header length
//
#define UIP_CONF_LLH_LEN            14

//
// CPU byte order.
//
#define UIP_CONF_BYTE_ORDER         LITTLE_ENDIAN

//
// Here we include the header file for the application we are using in
// this example
//
#include "eth_main.h"

//
// Define the uIP Application State type, based on the httpd.h state variable.
//
typedef struct Port_state uip_tcp_appstate_t;
typedef struct Port_state uip_udp_appstate_t;

//
// UIP_APPCALL: the name of the application function. This function
// must return void and take no arguments (i.e., C type "void
// appfunc(void)").
//
#ifndef UIP_APPCALL
#define UIP_APPCALL     my_appcall
#endif

#ifndef UIP_UDP_APPCALL
#define UIP_UDP_APPCALL	my_udp_appcall
#endif


//
// Here we include the header file for the DPCP client we are using in
// this example
//
#include "dhcpc.h"

//连接等待次数
#define UIP_WAITTIMES                30  //600改为30 2017.07.19




#endif // __UIP_CONF_H_
