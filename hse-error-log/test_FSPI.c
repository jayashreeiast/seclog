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

/* ============================
 * FSPI / XSPI INCLUDES (S32G)
 * ============================ */
#include <flash_info.h>
#include <fspi_api.h>

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

	/* ============================================================
	 * FSPI NOR FLASH SANITY TEST (SAFE, CORRECT FOR S32G)
	 * ============================================================ */
	{
		uint32_t test_offset = 0x400;        /* Offset inside NOR */
		uint32_t write_val  = 0xA5A5A5A5;
		uint32_t read_val   = 0x0;

		NOTICE("BL2 FSPI TEST: offset=0x%08x\n", test_offset);

		/* Erase sector containing test_offset */
		xspi_sector_erase(
			test_offset - (test_offset % F_SECTOR_ERASE_SZ),
			F_SECTOR_ERASE_SZ);

		/* Write value using IP command */
		xspi_write(test_offset, &write_val, sizeof(write_val));

		/* Read back using IP read */
		xspi_ip_read(test_offset, &read_val, sizeof(read_val));

		if (read_val == write_val) {
			NOTICE("FSPI TEST PASS: wrote=0x%08x read=0x%08x\n",
			       write_val, read_val);
		} else {
			ERROR("FSPI TEST FAIL: wrote=0x%08x read=0x%08x\n",
			      write_val, read_val);
		}
	}
	/* ============================================================
	 * END FSPI TEST
	 * ============================================================ */

	/*
	 * Get information about the images to load.
	 */
	bl2_load_info = plat_get_bl_image_load_info();
	assert(bl2_load_info != NULL);
	assert(bl2_load_info->head != NULL);
	assert(bl2_load_info->h.type == PARAM_BL_LOAD_INFO);
	assert(bl2_load_info->h.version >= VERSION_2);
	bl2_node_info = bl2_load_info->head;

	while (bl2_node_info != NULL) {

		/*
		 * Platform setup (once)
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
				plat_error_handler(err);
			}
		} else {
			INFO("BL2: Skip loading image id %u\n", bl2_node_info->image_id);
		}

		err = bl2_plat_handle_post_image_load(bl2_node_info->image_id);
		if (err != 0) {
			ERROR("BL2: Failure in post image load handling (%i)\n", err);
			plat_error_handler(err);
		}

		bl2_node_info = bl2_node_info->next_load_info;
	}

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
	}

	/* Flush parameters to memory */
	plat_flush_next_bl_params();

	return bl2_to_next_bl_params->head->ep_info;
}

