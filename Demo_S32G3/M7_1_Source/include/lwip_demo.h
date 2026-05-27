/*
 *  Copyright 2023-2025 NXP
 *
 *  NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only
 *  be used strictly in accordance with the applicable license terms. By
 *  expressly accepting such terms or by downloading, installing, activating
 *  and/or otherwise using the software, you are agreeing that you have read,
 *  and that you agree to comply with and are bound by, such license terms. If
 *  you do not agree to be bound by the applicable license terms, then you may
 *  not retain, install, activate or otherwise use the software.
 *
 *  This file contains sample code only. It is not part of the production code deliverables.
 */

#ifndef LWIP_DEMO_H
#define LWIP_DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/snmp.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/apps/httpd.h"
#include "lwip/timeouts.h"
#include "lwip/apps/lwiperf.h"

#define PFE_NETIF_NUMBER 3U

typedef struct 
{
    int has_dhcp;                          /* Variable containing information whether dhcp is enabled or not */
    int has_auto_ip;                       /* Variable containing information whether auto ip is enabled or not */
    int has_IPv6;                          /* Variable containing information whether ipv6 is enabled or not */
    int default_netif;
    const char *ip_addr;                        /* Ip address of board needs to be set if dhcp is turned off. */
    const char *netmask;                        /* Netmask of board needs to be set if dhcp is turned off. */
    const char *gw;                             /* Network gateway of board needs to be set if dhcp is turned off. */
    const char *hostname;                   /* Hostname of board used for dns. */
    char name[2];                           /* Interface name */
} netif_custom_t;

int demo_lwip_init(void);
void demo_lwip_main(void);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_DEMO_H */