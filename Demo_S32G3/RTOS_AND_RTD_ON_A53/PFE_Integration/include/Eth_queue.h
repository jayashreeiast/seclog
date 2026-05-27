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
 
/* Maximum capacity of queue */
#define QUEUE_CAPACITY 1000

/**
 * @brief   Queue element information structure
 */
typedef struct queue_elm_info_tag{
    uint8 ctrl_idx;
    uint8 *phys_addr_ptr;
    Eth_DataType *data_ptr;
    uint16 ether_type;
    uint16 len_byte;
} queue_elm_info_t;

void queue_init(void);
bool_t queue_is_empty(void);
bool_t queue_is_full(void);
void queue_push(queue_elm_info_t queue_elm_data);
queue_elm_info_t queue_pop(void);
