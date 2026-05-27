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
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "Eth_43_PFE.h"
#include "Eth_queue.h"

static void queue_clear_elm(int queue_elm_index);
static int how_many(void);

queue_elm_info_t queue[QUEUE_CAPACITY];

/* Pointer to next index of queue to push */
int queue_push_ptr;

/* Pointer to next index of queue to pop */
int queue_pop_ptr;

/* TRUE if queue_push_ptr exceeds QUEUE_CAPACITY, then queue_push_ptr point to queue[0] */
bool queueIndexIsOutOfBound;

/**
 * @brief   Clear queue element with index
 */
static void queue_clear_elm(int queue_elm_index){
    queue[queue_elm_index].ctrl_idx = 0;
    queue[queue_elm_index].phys_addr_ptr = NULL;
    queue[queue_elm_index].data_ptr = NULL;
    queue[queue_elm_index].ether_type = 0;
    queue[queue_elm_index].len_byte = 0;
}

/**
 * @brief   How many elements in the queue?
 * @return  Number of elements in queue
 */
static int how_many(void){
    return (queue_push_ptr + QUEUE_CAPACITY * queueIndexIsOutOfBound - queue_pop_ptr);
}

/**
 * @brief   Clear all queue element, push and pop pointer
 */
void queue_init(void){
    int index;

    queue_push_ptr = 0;
    queue_pop_ptr = 0;
    queueIndexIsOutOfBound = FALSE;

    for(index = 0 ; index < QUEUE_CAPACITY; index++){
        queue_clear_elm(index);
    }
}

/**
 * @brief   Check queue is Empty or not
 * @return  TRUE if queue is empty
 *          FALSE if queue is not empty
 */
bool_t queue_is_empty(void) {
    return !(how_many());
}

/**
 * @brief   Check queue is full or not
 * @return  TRUE if queue is full
 *          FALSE if queue is not full
 */
bool_t queue_is_full(void) {
    return (QUEUE_CAPACITY == how_many());
}

/**
 * @brief   Push data to queue
 */
void queue_push(queue_elm_info_t queue_elm_data) {
    queue[queue_push_ptr].ctrl_idx = queue_elm_data.ctrl_idx;
    queue[queue_push_ptr].phys_addr_ptr = queue_elm_data.phys_addr_ptr;
    queue[queue_push_ptr].data_ptr = queue_elm_data.data_ptr;
    queue[queue_push_ptr].ether_type = queue_elm_data.ether_type;
    queue[queue_push_ptr].len_byte = queue_elm_data.len_byte;
    queue_push_ptr++;
    if (queue_push_ptr > QUEUE_CAPACITY-1) {
        queue_push_ptr = 0;
        queueIndexIsOutOfBound = TRUE;
    }
}

/**
 * @brief   Get element from queue
 * @return  Data
 */
queue_elm_info_t queue_pop(void) {
    queue_elm_info_t ret;

    ret = queue[queue_pop_ptr];
    queue_clear_elm(queue_pop_ptr);
    queue_pop_ptr++;
    if (queue_pop_ptr > QUEUE_CAPACITY-1) {
        queue_pop_ptr = 0;
        queueIndexIsOutOfBound = FALSE;
    }
    return ret;
}
