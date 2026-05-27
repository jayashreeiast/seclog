/*
 * Copyright 2025 NXP.
 */
#ifndef PFE_RTOS_TASK_H
#define PFE_RTOS_TASK_H

#include "Platform_Types.h"
#include "Eth_43_PFE.h"

/* Hardware type */
#define ARP_HARDWARE_TYPE_ETH   0x0001U
/* Protocol type */
#define ARP_PROTOCOL_TYPE_IPV4  0x0800U
/* Hardware address length */
#define ARP_HARDWARE_ADDR_LEN   0x06U
/* Protocol address length */
#define ARP_PROTOCOL_ADDR_LEN   0x04U
/* Opcode arp reply */
#define ARP_OPCODE_ARP_REPLY    0x02U
/* Echo reply type */
#define ICMP_TYPE_ECHO_REPLY    0U
/* Ethernet type IPv4 */
#define ETHER_TYPE_IPV4         0x0800U
/* Ethernet type ARP */
#define ETHER_TYPE_ARP          0x0806U
/* Protocol type ICMP */
#define PROTOCOL_TYPE_ICMP      0x01U
/* IPv4 Version */
#define IPV4_VERSION            0x04U
/* IPv4 header length */
#define IPV4_HEADER_LENGTH      0x05U
/* IPv4 type of service */
#define IPV4_TYPE_OF_SERVICE    0U

#define IPV6_VERSION                0x06U
#define ETHER_TYPE_IPV6             0x86ddU
#define PROTOCOL_TYPE_ICMPV6        0x3aU
#define NEIGHBOR_SOLICITATION_TYPE  0x87U
#define NEIGHBOR_ADVERTISEMENT_TYPE  0x88U
#define ICMPV6_TYPE_ECHO_REPLY    0x81U
#define ICMPV6_TYPE_ECHO_REQUEST    0x80U

#define GPT_TIMER_VALUE (800000000UL / 6UL / 1000UL)
#define GPT_SLAVE_CHANNEL 0

#define NUM_CONTROLLER_USED                     (ETH_43_PFE_NUM_CONTROLLER_CFG)
#if (defined(PFE_CFG_FCI_ENABLE) && (defined(FCI_L2BR_TEST) || defined(FCI_RTABLE_TEST)))
    #define NUM_TX_CONTROLLER_USED                     (ETH_43_PFE_NUM_CONTROLLER_CFG)
#else
    #define NUM_TX_CONTROLLER_USED                     (ETH_43_PFE_NUM_CONTROLLER_CFG - 1U)
#endif

#define PFE_MASTER_READY_ADDRESS         0x343FFFF0
#define PFE_MASTER_READY_VALUE           0x123

/**
 * @brief ARP packet
 **/
typedef struct __attribute__((packed, aligned(4)))
{
   uint16 hrd;                /* Hardware type */
   uint16 pro;                /* Protocol type */
   uint8 hln;                 /* Hardware address length */
   uint8 pln;                 /* Protocol address length */
   uint16 op;                 /* Opcode, 1 = ARP request, 2 = ARP reply */
   uint8 sha[6];              /* Sender hardware address */
   uint32 spa;                /* Sender protocol address */
   uint8 tha[6];              /* Target hardware address */
   uint32 tpa;                /* Target protocol address */
} arp_packet_t;

typedef struct __attribute__((packed, aligned(4)))
{
   uint8 header_length : 4;   /* Internet header length */
   uint8 version : 4;         /* Version */
   uint8 type_of_service;     /* Type of service */
   uint16 total_length;       /* Total length */
   uint16 identification;     /* Indentification */
   uint16 fragment_offset;    /* Fragment offset */
   uint8 time_to_live;        /* Time to live */
   uint8 protocol;            /* Protocol */
   uint16 header_checksum;    /* Header checksum */
   uint32 src_addr;           /* Source IP adress */
   uint32 dest_addr;          /* Destination address */
   uint8 options[];           /* Options */
} ipv4_header_t;

typedef struct __attribute__((packed, aligned(4)))
{
   uint8 traffic_class1 : 4;
   uint8 version : 4;
   uint8 traffic_class2 : 4;
   uint16 flow_label;
   uint16 payload_length;
   uint8 next_header;
   uint8 hop_limit;
   uint32 src_addr[4];
   uint32 dest_addr[4];
} ipv6_header_t;

typedef struct
{
   uint8 type;                /* Type of message */
   uint8 code;                /* Code */
   uint16 checksum;           /* Checksum */
   uint16 identifier;         /* Identifier */
   uint16 sequence_number;    /* Sequence number */
   uint8 payload[];           /* The payload of packet */
} icmp_echo_message_t;

typedef struct
{
   uint8 type;
   uint8 code;
   uint16 checksum;
   uint32 Reserved;
   uint32 target_address[4];
   uint8 src_link_address;
   uint8 length;
   uint8 mac[6];
} icmp_message_v6_t;



uint32 ipv4_string_to_int(const char *p_ip_addr);

void ipv6_string_to_int(const char *p_ip_addr, uint32 *ipv6);
#if defined(PING_TEST)
boolean send_frame(uint8 *data, uint16 ether_type, uint16 data_length, uint8 iface, uint8 *target_mac);

void rx_process(uint8 ctrl_idx, uint8 *phys_addr_ptr, Eth_DataType *p_rx_data, uint16 len_byte);

void rx_callback(uint8 ctrl_idx, Eth_FrameType frame_type, boolean is_broadcast,
                    uint8 *phys_addr_ptr, Eth_DataType *p_rx_data, uint16 len_byte);
#endif

void EthSwt_Tst_PitNotification(void);

void Pit_Init_Irq(void);

void Pfe_Rtos_PrePare(void);

void Pfe_Slave_Ping_Task(void );


#endif
