#ifndef IDSM_EVENT_H
#define IDSM_EVENT_H

#include <stdint.h>

/* ============================================================
 * CONFIG
 * ============================================================ */
#define IDSM_PROTOCOL_VERSION        0x1

/* Protocol Header Bits */
#define IDSM_HDR_NO_CONTEXT          (0 << 0)
#define IDSM_HDR_TIMESTAMP_PRESENT   (1 << 1)
#define IDSM_HDR_NO_SIGNATURE        (0 << 2)

#define IDSM_PROTOCOL_HEADER \
    (IDSM_HDR_NO_CONTEXT | IDSM_HDR_TIMESTAMP_PRESENT | IDSM_HDR_NO_SIGNATURE)

/* Instance */
#define IDSM_INSTANCE_A_CORE         0x0

/* Module */
#define IDSM_MODULE_SECBOOT          0x0

/* ============================================================
 * EVENT IDs
 * ============================================================ */
#define IDSM_EVENT_BL31_FAIL         0x0801
#define IDSM_EVENT_BL32_FAIL         0x0802
#define IDSM_EVENT_BL33_FAIL         0x0803
#define IDSM_EVENT_KERNEL_FAIL       0x0804

/* ============================================================
 * STORAGE CONFIG (Circular Buffer)
 * ============================================================ */
#define IDSM_LOG_START_SECTOR        60686336
#define IDSM_LOG_MAX_ENTRIES         64
#define IDSM_BLOCK_SIZE              512

/* ============================================================
 * STRUCTURES
 * ============================================================ */

/* Event Frame (8 bytes) */
typedef struct {
    uint8_t data[8];
} idsm_event_frame_t;

/* Timestamp Frame (8 bytes) */
typedef struct {
    uint8_t data[8];
} idsm_timestamp_frame_t;

/* ============================================================
 * APIs
 * ============================================================ */
void idsm_log_event(uint16_t event_id);

#endif
