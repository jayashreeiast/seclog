/*
 *  Copyright 2025 NXP
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

#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H

#define NO_SYS                          1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define SYS_LIGHTWEIGHT_PROT            0

#define LWIP_IPV6                       0
#define IPV6_FRAG_COPYHEADER            0
#define LWIP_IPV6_DUP_DETECT_ATTEMPTS   0

/* Enable some protocols to test them */
#define LWIP_DHCP                       0
#define LWIP_AUTOIP                     0

#define LWIP_IGMP                       1
#define LWIP_DNS                        1

#define LWIP_ALTCP                      1

/* Turn off checksum verification of fuzzed data */
#define CHECKSUM_CHECK_IP               0
#define CHECKSUM_CHECK_UDP              0
#define CHECKSUM_CHECK_TCP              0
#define CHECKSUM_CHECK_ICMP             0
#define CHECKSUM_CHECK_ICMP6            0

#define TCP_MSS                         1460
#define MEM_SIZE                        32768
#define TCP_SND_QUEUELEN                (2* TCP_SND_BUF / TCP_MSS)
#define MEMP_NUM_TCP_SEG                TCP_SND_QUEUELEN
#define TCP_OVERSIZE                    1
#define TCP_SND_BUF                     (64 * TCP_MSS)
#define TCP_WND                         (64 * TCP_MSS)
#define LWIP_WND_SCALE                  1
#define TCP_RCV_SCALE                   2
#define PBUF_POOL_SIZE                  200 

#define ETHARP_SUPPORT_STATIC_ENTRIES   1

#define LWIP_NUM_NETIF_CLIENT_DATA      1
#define LWIP_SNMP                       1
#define MIB2_STATS                      1
#define LWIP_MDNS_RESPONDER             1

#define MEM_ALIGNMENT                   4

#endif /* LWIP_LWIPOPTS_H */