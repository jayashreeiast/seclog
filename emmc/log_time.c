/*
 * Copyright (c) 2016-2022, ARM Limited and Contributors.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <stdint.h>

#include <arch.h>
#include <arch_helpers.h>
#include "bl2_private.h"
#include <common/bl_common.h>
#include <common/debug.h>
#include <common/desc_image_load.h>
#include <drivers/auth/auth_mod.h>
#include <plat/common/platform.h>

#include <platform_def.h>
#include <s32cc_storage.h>

/* === ADDED FOR EMMC LOGGING === */
#include <drivers/mmc.h>
#include <string.h>

#define BLOCK_SIZE          512
#define LOG_START_SECTOR    60686336   /* MUST NOT overlap FIP */
#define LOG_MAX_ENTRIES     32
#define LOG_MAGIC           0x424C324C  /* "BL2L" */

/* =======================================================================
 * BL2 Persistent Log Structure
 * ======================================================================= */
typedef struct {
    uint32_t magic;
    uint32_t image_id;
    int32_t  error;
    uint64_t cntvct;
    uint64_t cntfrq;
    uint64_t time_us;
} bl2_log_entry_t;

/* =======================================================================
 * Timer Helpers
 * ======================================================================= */
static inline uint64_t bl2_get_cntvct(void)
{
    return read_cntvct_el0();
}

static inline uint64_t bl2_get_cntfrq(void)
{
    return read_cntfrq_el0();
}

static uint64_t bl2_get_time_us(void)
{
    uint64_t cnt  = bl2_get_cntvct();
    uint64_t freq = bl2_get_cntfrq();

    if (freq == 0)
        return 0;

    return (cnt * 1000000ULL) / freq;
}

/* =======================================================================
 * Persistent eMMC Failure Logger
 * ======================================================================= */
static void bl2_log_failure(uint32_t image_id, int error)
{
    uint8_t block[BLOCK_SIZE];
    bl2_log_entry_t entry;
    int ret;
    int slot;

    memset(&entry, 0, sizeof(entry));

    entry.magic     = LOG_MAGIC;
    entry.image_id  = image_id;
    entry.error     = error;
    entry.cntvct    = bl2_get_cntvct();
    entry.cntfrq    = bl2_get_cntfrq();
    entry.time_us   = bl2_get_time_us();

    NOTICE("BL2 LOG: image=%u err=%d time_us=%llu\n",
           image_id, error, entry.time_us);

    for (slot = 0; slot < LOG_MAX_ENTRIES; slot++) {

        ret = mmc_read_blocks(LOG_START_SECTOR + slot,
                              (uintptr_t)block, 1);
        if (ret != 1) {
            ERROR("BL2 LOG: read failed\n");
            return;
        }

        bl2_log_entry_t *existing = (bl2_log_entry_t *)block;

        if (existing->magic != LOG_MAGIC) {

            memset(block, 0, BLOCK_SIZE);
            memcpy(block, &entry, sizeof(entry));

            ret = mmc_write_blocks(LOG_START_SECTOR + slot,
                                   (uintptr_t)block, 1);
            if (ret != 1) {
                ERROR("BL2 LOG: write failed\n");
            }
            return;
        }
    }

    WARN("BL2 LOG: Log area full\n");
}

/* =======================================================================
 * eMMC Read/Write Test
 * ======================================================================= */
void bl2_emmc_test(void)
{
    uint8_t write_buf[BLOCK_SIZE];
    uint8_t read_buf[BLOCK_SIZE];
    int ret;

    NOTICE("BL2: Starting eMMC test\n");

    memset(write_buf, 0, BLOCK_SIZE);
    strcpy((char *)write_buf, "BL2 TEST LOG FROM S32G");

    ret = mmc_write_blocks(LOG_START_SECTOR - 1,
                           (uintptr_t)write_buf, 512);
    if (ret != 1) {
        ERROR("BL2: mmc_write_blocks failed: %d\n", ret);
        return;
    }

    memset(read_buf, 0, BLOCK_SIZE);

    ret = mmc_read_blocks(LOG_START_SECTOR - 1,
                          (uintptr_t)read_buf, 512);
    if (ret != 1) {
        ERROR("BL2: mmc_read_blocks failed: %d\n", ret);
        return;
    }

    NOTICE("BL2: read OK: %s\n", read_buf);
}

/*******************************************************************************
 * This function loads SCP_BL2/BL3x images and returns the ep_info for
 * the next executable image.
 ******************************************************************************/
struct entry_point_info *bl2_load_images(void)
{
    bl_params_t *bl2_to_next_bl_params;
    bl_load_info_t *bl2_load_info;
    const bl_load_info_node_t *bl2_node_info;
    int plat_setup_done = 0;
    int err;
    bool retried_with_secondary = false;

	/*
	 * Get information about the images to load.
	 */
restart_from_head:
    bl2_load_info = plat_get_bl_image_load_info();
    assert(bl2_load_info != NULL);
    assert(bl2_load_info->head != NULL);
	assert(bl2_load_info->h.type == PARAM_BL_LOAD_INFO);
	assert(bl2_load_info->h.version >= VERSION_2);
    bl2_node_info = bl2_load_info->head;

    while (bl2_node_info != NULL) {
		/*
		 * Perform platform setup before loading the image,
		 * if indicated in the image attributes AND if NOT
		 * already done before.
		 */
        if ((bl2_node_info->image_info->h.attr &
             IMAGE_ATTRIB_PLAT_SETUP) != 0U) {
			if (plat_setup_done != 0) {
				WARN("BL2: Platform setup already done!!\n");
			} else {
                INFO("BL2: Doing platform setup\n");
                bl2_platform_setup();
                plat_setup_done = 1;
            }
        }

        err = bl2_plat_handle_pre_image_load(bl2_node_info->image_id);
        if (err != 0) {

            bl2_log_failure(bl2_node_info->image_id, err);

            if (!retried_with_secondary) {
                plat_switch_to_secondary_fip();
                retried_with_secondary = true;
                INFO("BL2: retry (pre) from secondary FIP\n");
                goto restart_from_head;
            }
            ERROR("BL2: Both primary and secondary FIP failed (pre) for id %u\n",
                  bl2_node_info->image_id);
            plat_error_handler(err);
        }

        if ((bl2_node_info->image_info->h.attr &
             IMAGE_ATTRIB_SKIP_LOADING) == 0U) {

			INFO("BL2: Loading image id %u\n", bl2_node_info->image_id);
            err = load_auth_image(bl2_node_info->image_id,
                                  bl2_node_info->image_info);

            if (err != 0) {

                bl2_log_failure(bl2_node_info->image_id, err);

                if (!retried_with_secondary) {
                    plat_switch_to_secondary_fip();
                    plat_setup_done = 0;
                    retried_with_secondary = true;
					INFO("BL2: retry (load) from secondary FIP\n");
                    goto restart_from_head;
                }

				ERROR("BL2: Both primary and secondary FIP failed (load) for id %u\n",
				      bl2_node_info->image_id);
                plat_error_handler(err);
            }
		} else {
			INFO("BL2: Skip loading image id %u\n", bl2_node_info->image_id);
		}

        err = bl2_plat_handle_post_image_load(bl2_node_info->image_id);
        if (err != 0) {

            bl2_log_failure(bl2_node_info->image_id, err);

            if (!retried_with_secondary) {
                plat_switch_to_secondary_fip();
                retried_with_secondary = true;
                goto restart_from_head;
            }

            plat_error_handler(err);
        }

        bl2_node_info = bl2_node_info->next_load_info;
    }

    NOTICE("BL2: Running eMMC test BEFORE handoff\n");
    bl2_emmc_test();

	/*
	 * Get information to pass to the next image.
	 */
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

