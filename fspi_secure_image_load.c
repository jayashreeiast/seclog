/*
 * Copyright (c) 2016-2022, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <arch.h>
#include <arch_helpers.h>
#include "bl2_private.h"
#include <common/bl_common.h>
#include <common/debug.h>
#include <common/desc_image_load.h>
#include <drivers/auth/auth_mod.h>
#include <plat/common/platform.h>

#include <platform_def.h>

/* --- TEST ADDITION START --- */
#include "fspi.h"

void run_fspi_test(void)
{
	char *test_msg = "FSPI_TEST_OK_ADDR_0x03F00000";
	char read_back[64] = {0};
	uint32_t log_addr = 0x03F00000; /* Use a safe high-address for testing */

	INFO("BL2: Starting Explicit FSPI Test...\n");

	/* Initialize hardware */
	fspi_init();

	/* 1. Erase */
	if (fspi_erase_sector(log_addr) != 0) {
		ERROR("FSPI Test: Erase Failed\n");
		return;
	}

	/* 2. Write */
	if (fspi_write(log_addr, (uint8_t *)test_msg, strlen(test_msg)) != 0) {
		ERROR("FSPI Test: Write Failed\n");
		return;
	}

	/* 3. Read back and Verify */
	fspi_read(log_addr, (uint8_t *)read_back, strlen(test_msg));

	if (strncmp(test_msg, read_back, strlen(test_msg)) == 0) {
		INFO("BL2: FSPI Test SUCCESS! Data: %s\n", read_back);
	} else {
		ERROR("BL2: FSPI Test FAILED! Read: %s\n", read_back);
	}
}
/* --- TEST ADDITION END --- */

struct entry_point_info *bl2_load_images(void)
{
	bl_params_t *bl2_to_next_bl_params;
	bl_load_info_t *bl2_load_info;
	const bl_load_info_node_t *bl2_node_info;
	int plat_setup_done = 0;
	int err;

	/* --- EXPLICIT TEST CALL START --- */
	/* We call this here so it runs every time BL2 executes */
	run_fspi_test();
	/* --- EXPLICIT TEST CALL END --- */

	bl2_load_info = plat_get_bl_image_load_info();
	assert(bl2_load_info != NULL);
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

		/* Allow platform to handle image information. */
		err = bl2_plat_handle_post_image_load(bl2_node_info->image_id);
		if (err != 0) {
			ERROR("BL2: Failure in post image load handling (%i)\n", err);
			plat_error_handler(err);
		}

		/* Go to next image */
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

	/* Flush the parameters to be passed to next image */
	plat_flush_next_bl_params();

	return bl2_to_next_bl_params->head->ep_info;
}
