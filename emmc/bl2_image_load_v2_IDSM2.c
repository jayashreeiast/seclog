/*
 * Copyright (c) 2016-2022, ARM Limited
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <arch.h>
#include <arch_helpers.h>
#include "bl2_private.h"

#include <common/debug.h>
#include <common/desc_image_load.h>
#include <drivers/auth/auth_mod.h>
#include <plat/common/platform.h>

#include <platform_def.h>
#include <s32cc_storage.h>
#include <drivers/mmc.h>

#include "idsm_event.h"

/* ============================================================
 * STATIC STATE (Circular Buffer Index)
 * ============================================================ */
static uint32_t idsm_log_index = 0;

/* ============================================================
 * TIMER (ARM GENERIC TIMER)
 * ============================================================ */
static uint64_t get_time_ms(void)
{
    uint64_t cnt = read_cntpct_el0();
    uint64_t freq = read_cntfrq_el0();

    NOTICE("IDSM_TIMER: cnt=%lu freq=%lu\n", cnt, freq);

    if (freq == 0)
        freq = 5000000;

    return (cnt * 1000ULL) / freq;
}

/* ============================================================
 * FRAME PRINT HELPERS
 * ============================================================ */
static void dump_event_frame(const idsm_event_frame_t *f)
{
    NOTICE("IDSM_EVENT_FRAME: ");
    for (int i = 0; i < 8; i++)
        NOTICE("%02X ", f->data[i]);
    NOTICE("\n");
}

static void dump_timestamp_frame(const idsm_timestamp_frame_t *t)
{
    NOTICE("IDSM_TS_FRAME: ");
    for (int i = 0; i < 8; i++)
        NOTICE("%02X ", t->data[i]);
    NOTICE("\n");
}

/* ============================================================
 * BUILD EVENT FRAME
 * ============================================================ */
static void build_event_frame(idsm_event_frame_t *f,
                              uint8_t instance,
                              uint16_t event_id)
{
    f->data[0] = ((IDSM_PROTOCOL_VERSION & 0xF) << 4) |
                 (IDSM_PROTOCOL_HEADER & 0xF);

    f->data[1] = ((instance & 0x3F) << 2) |
                 ((IDSM_MODULE_SECBOOT >> 4) & 0x3);

    f->data[2] = (IDSM_MODULE_SECBOOT & 0xF) << 4;

    f->data[3] = (event_id >> 8) & 0xFF;
    f->data[4] = event_id & 0xFF;

    f->data[5] = 0;
    f->data[6] = 0;
    f->data[7] = 0;
}

/* ============================================================
 * BUILD TIMESTAMP FRAME
 * ============================================================ */
static void build_timestamp_frame(idsm_timestamp_frame_t *t)
{
    uint64_t ts = get_time_ms();

    for (int i = 0; i < 8; i++)
        t->data[7 - i] = (ts >> (i * 8)) & 0xFF;
}

/* ============================================================
 * MAIN IDSM LOGGER
 * ============================================================ */
void idsm_log_event(uint16_t event_id)
{
    idsm_event_frame_t event;
    idsm_timestamp_frame_t ts;

    uint8_t block_buf[IDSM_BLOCK_SIZE];
    int ret;

    uint32_t slot = idsm_log_index % IDSM_LOG_MAX_ENTRIES;

    uint32_t event_sector = IDSM_LOG_START_SECTOR + (slot * 2);
    uint32_t ts_sector    = event_sector + 1;

    /* ==== DEBUG SECTOR INFO ==== */
    NOTICE("IDSM_LOG: EVENT=0x%x SLOT=%u\n", event_id, slot);
    NOTICE("IDSM_LOG: event_sector=%u ts_sector=%u\n",
           event_sector, ts_sector);

    /* ==== Dump MMC Info ==== 
    extern mmc_device_info_t mmc_dev_info;
    NOTICE("IDSM_MMC: block_size=%u erase_size=%u rca=%u\n",
           mmc_dev_info.block_size,
           mmc_dev_info.erase_size,
           mmc_dev_info.rca); */

    /* ==== Build Frames ==== */
    build_event_frame(&event, IDSM_INSTANCE_A_CORE, event_id);
    build_timestamp_frame(&ts);

    dump_event_frame(&event);
    dump_timestamp_frame(&ts);

    /* ==== Write EVENT frame ==== */
    memset(block_buf, 0, IDSM_BLOCK_SIZE);
    memcpy(block_buf, &event, sizeof(event));

    NOTICE("IDSM: Writing EVENT frame...\n");
    ret = mmc_write_blocks(event_sector, (uintptr_t)block_buf, 1);
    NOTICE("IDSM: mmc_write(EVENT) ret=%d\n", ret);

    if (ret != 1) {
        ERROR("IDSM: Event write FAILED at sector %u ret=%d\n",
              event_sector, ret);
        return;
    }

    /* ==== Write TIMESTAMP frame ==== */
    memset(block_buf, 0, IDSM_BLOCK_SIZE);
    memcpy(block_buf, &ts, sizeof(ts));

    NOTICE("IDSM: Writing TS frame...\n");
    ret = mmc_write_blocks(ts_sector, (uintptr_t)block_buf, 1);
    NOTICE("IDSM: mmc_write(TS) ret=%d\n", ret);

    if (ret != 1) {
        ERROR("IDSM: Timestamp write FAILED at sector %u ret=%d\n",
              ts_sector, ret);
        return;
    }

    NOTICE("IDSM: SUCCESS Logged event 0x%x at slot %u\n",
           event_id, slot);

    idsm_log_index++;
}

/*******************************************************************************
 * BL2 LOAD IMAGES
 ******************************************************************************/
struct entry_point_info *bl2_load_images(void)
{
    bl_params_t *bl2_to_next_bl_params;
    bl_load_info_t *bl2_load_info;
    const bl_load_info_node_t *bl2_node_info;
    int plat_setup_done = 0;
    int err;
    bool retried = false;

    /* >>>>>>>>>>>>>>>>>> LOG BOOT START <<<<<<<<<<<<<<<<<< */
    idsm_log_event(IDSM_EVENT_BL31_FAIL);

restart:
    bl2_load_info = plat_get_bl_image_load_info();
    bl2_node_info = bl2_load_info->head;

    while (bl2_node_info != NULL) {

        if ((bl2_node_info->image_info->h.attr &
             IMAGE_ATTRIB_PLAT_SETUP) && !plat_setup_done) {
            bl2_platform_setup();
            plat_setup_done = 1;
        }

        /* PRE LOAD */
        err = bl2_plat_handle_pre_image_load(bl2_node_info->image_id);

        if (err != 0) {
            idsm_log_event(IDSM_EVENT_BL31_FAIL);
            if (!retried) {
                plat_switch_to_secondary_fip();
                retried = true;
                goto restart;
            }
            plat_error_handler(err);
        }

        /* LOAD IMAGE */
        if (!(bl2_node_info->image_info->h.attr & IMAGE_ATTRIB_SKIP_LOADING)) {

            err = load_auth_image(
                    bl2_node_info->image_id,
                    bl2_node_info->image_info);

            if (err != 0) {
                idsm_log_event(IDSM_EVENT_BL31_FAIL);
                if (!retried) {
                    plat_switch_to_secondary_fip();
                    retried = true;
                    goto restart;
                }
                plat_error_handler(err);
            }
        }

        /* POST LOAD */
        err = bl2_plat_handle_post_image_load(bl2_node_info->image_id);

        if (err != 0) {
            idsm_log_event(IDSM_EVENT_BL31_FAIL);
            if (!retried) {
                plat_switch_to_secondary_fip();
                retried = true;
                goto restart;
            }
            plat_error_handler(err);
        }

        bl2_node_info = bl2_node_info->next_load_info;
    }

    /* >>>>>>>>>>>>>>>>>> LOG BL LOADING COMPLETE <<<<<<<<<<<<<<<<<< */
    idsm_log_event(IDSM_EVENT_BL31_FAIL);

    /* NEXT BL PARAMS */
    bl2_to_next_bl_params = plat_get_next_bl_params();
	assert(bl2_to_next_bl_params != NULL);
	assert(bl2_to_next_bl_params->head != NULL);
	assert(bl2_to_next_bl_params->h.type == PARAM_BL_PARAMS);
	assert(bl2_to_next_bl_params->h.version >= VERSION_2);
	assert(bl2_to_next_bl_params->head->ep_info != NULL);

	/* Populate arg0 for the next BL image if not already provided */
	if (bl2_to_next_bl_params->head->ep_info->args.arg0 == (u_register_t)0)
		bl2_to_next_bl_params->head->ep_info->args.arg0 =
					(u_register_t)bl2_to_next_bl_params;

	/* Flush the parameters to be passed to next image */
	plat_flush_next_bl_params();
    return bl2_to_next_bl_params->head->ep_info;
}
