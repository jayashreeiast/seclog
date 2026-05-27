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

#include <drivers/delay_timer.h>
#include <nxp_timer.h>

/* === ADDED FOR EMMC TEST === */
#include <drivers/mmc.h>
#include <string.h>

#define TEST_SECTOR   60686336
#define BLOCK_SIZE    512

/* =======================================================================
 * Timestamp helper using ARM Generic Timer
 * ======================================================================= */
static uint64_t get_time_ms(void)
{
    uint64_t cnt;
    uint64_t freq;

    isb();
    cnt  = read_cntpct_el0();
    freq = read_cntfrq_el0();

    return (cnt * 1000ULL) / freq;
}

static void print_timestamp(const char *event)
{
    uint64_t time_ms = get_time_ms();
    NOTICE("TIME [%s] = %llu ms\n", event, time_ms);
}

/* =======================================================================
 * eMMC Read/Write Test with Timestamp Logging
 * ======================================================================= */
void bl2_emmc_test(void)
{
    uint8_t write_buf[BLOCK_SIZE];
    uint8_t read_buf[BLOCK_SIZE];
    int ret;

    uint64_t t1, t2;

    NOTICE("BL2: Starting eMMC test\n");

    memset(write_buf, 0, BLOCK_SIZE);
    strcpy((char *)write_buf, "BL2 TEST LOG FROM S32G");

    /* ===== WRITE START ===== */
    print_timestamp("eMMC WRITE START");
    t1 = get_time_ms();

    ret = mmc_write_blocks(TEST_SECTOR, (uintptr_t)write_buf, 1);

    t2 = get_time_ms();
    print_timestamp("eMMC WRITE END");

    if (ret != 1) {
        ERROR("BL2: mmc_write_blocks failed: %d\n", ret);
        return;
    }

    NOTICE("BL2: write OK (Latency = %llu ms)\n", (t2 - t1));

    memset(read_buf, 0, BLOCK_SIZE);

    /* ===== READ START ===== */
    print_timestamp("eMMC READ START");
    t1 = get_time_ms();

    ret = mmc_read_blocks(TEST_SECTOR, (uintptr_t)read_buf, 1);

    t2 = get_time_ms();
    print_timestamp("eMMC READ END");

    if (ret != 1) {
        ERROR("BL2: mmc_read_blocks failed: %d\n", ret);
        return;
    }

    NOTICE("BL2: read OK: %s\n", read_buf);
    NOTICE("BL2: read latency = %llu ms\n", (t2 - t1));
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
            ERROR("BL2: Failure in pre image load handling (%i)\n", err);
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
                ERROR("BL2: Failed to load image id %u (%i)\n",
                      bl2_node_info->image_id, err);

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
            ERROR("BL2: Failure in post image load handling (%i)\n", err);
            if (!retried_with_secondary) {
                plat_switch_to_secondary_fip();
                retried_with_secondary = true;
                INFO("BL2: retry (post) from secondary FIP\n");
                goto restart_from_head;
            }
            ERROR("BL2: Both primary and secondary FIP failed (post) for id %u\n",
                  bl2_node_info->image_id);
            plat_error_handler(err);
        }

		/* Go to next image */
		bl2_node_info = bl2_node_info->next_load_info;
	}

    /* ================= RUN EMMC TEST HERE ================= */
    NOTICE("BL2: Running eMMC read/write test BEFORE handoff\n");
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

