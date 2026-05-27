/*
 * Copyright (c) 2016-2022, ARM Limited and Contributors.
 * All rights reserved.
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
#include <drivers/io/io_storage.h>

#include <plat/common/platform.h>
#include <platform_def.h>

/* ============================================================
 * SIMPLE IO READ/WRITE TEST (DEBUG VERSION)
 * ============================================================ */
static void bl2_simple_io_test(void)
{
	uintptr_t dev_handle;
	uintptr_t image_spec;
	uintptr_t image_handle;
	size_t rw_size;
	int rc;

	uint32_t write_val = 0xA5A5A5A5;
	uint32_t read_val  = 0x00000000;

	NOTICE("\n");
	NOTICE("=================================================\n");
	NOTICE("BL2 IO TEST: START\n");
	NOTICE("=================================================\n");

	/* Step 1: Resolve image source */
	NOTICE("BL2 IO TEST: Resolving image source (BL33)\n");

	rc = plat_get_image_source(BL33_IMAGE_ID,
				   &dev_handle,
				   &image_spec);
	if (rc != 0) {
		ERROR("BL2 IO TEST: plat_get_image_source FAILED (%d)\n", rc);
		return;
	}

	NOTICE("BL2 IO TEST: dev_handle = 0x%lx\n", dev_handle);
	NOTICE("BL2 IO TEST: image_spec = 0x%lx\n", image_spec);

	/* Step 2: Open image */
	NOTICE("BL2 IO TEST: Opening image\n");

	rc = io_open(dev_handle, image_spec, &image_handle);
	if (rc != 0) {
		ERROR("BL2 IO TEST: io_open FAILED (%d)\n", rc);
		return;
	}

	NOTICE("BL2 IO TEST: image_handle = 0x%lx\n", image_handle);

	/* Step 3: Seek (EXPECTED TO FAIL for FIP) */
	NOTICE("BL2 IO TEST: Attempting io_seek to offset 0x400\n");

	rc = io_seek(image_handle, IO_SEEK_SET, 0x400);
	if (rc != 0) {
		ERROR("BL2 IO TEST: io_seek FAILED (%d) — continuing anyway\n", rc);
	} else {
		NOTICE("BL2 IO TEST: io_seek OK\n");
	}

	/* Step 4: Write (EXPECTED TO FAIL for FIP) */
	NOTICE("BL2 IO TEST: Attempting io_write\n");

	rc = io_write(image_handle,
		      (uintptr_t)&write_val,
		      sizeof(write_val),
		      &rw_size);
	if (rc != 0) {
		ERROR("BL2 IO TEST: io_write FAILED (%d)\n", rc);
	} else {
		NOTICE("BL2 IO TEST: io_write OK size=%zu\n", rw_size);
	}

	/* Step 5: Read (may work sequentially) */
	NOTICE("BL2 IO TEST: Attempting io_read\n");

	rc = io_read(image_handle,
		     (uintptr_t)&read_val,
		     sizeof(read_val),
		     &rw_size);
	if (rc != 0) {
		ERROR("BL2 IO TEST: io_read FAILED (%d)\n", rc);
	} else {
		NOTICE("BL2 IO TEST: io_read OK size=%zu\n", rw_size);
		NOTICE("BL2 IO TEST: read_val = 0x%08x\n", read_val);
	}

	NOTICE("BL2 IO TEST: Closing image\n");
	io_close(image_handle);

	NOTICE("=================================================\n");
	NOTICE("BL2 IO TEST: END\n");
	NOTICE("=================================================\n");
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

		err = bl2_plat_handle_pre_image_load(
				bl2_node_info->image_id);
		if (err != 0) {
			ERROR("BL2: Failure in pre image load handling (%i)\n", err);
			plat_error_handler(err);
		}

		if ((bl2_node_info->image_info->h.attr &
		     IMAGE_ATTRIB_SKIP_LOADING) == 0U) {

			INFO("BL2: Loading image id %u\n",
			     bl2_node_info->image_id);

			err = load_auth_image(
				bl2_node_info->image_id,
				bl2_node_info->image_info);

			if (err != 0) {
				ERROR("BL2: Failed to load image id %u (%i)\n",
				      bl2_node_info->image_id, err);
				plat_error_handler(err);
			}
		} else {
			INFO("BL2: Skip loading image id %u\n",
			     bl2_node_info->image_id);
		}

		err = bl2_plat_handle_post_image_load(
				bl2_node_info->image_id);
		if (err != 0) {
			ERROR("BL2: Failure in post image load handling (%i)\n", err);
			plat_error_handler(err);
		}

		/* Go to next image */
		bl2_node_info = bl2_node_info->next_load_info;
	}

	/* ===============================
	 * SIMPLE IO TEST EXECUTED HERE
	 * =============================== */
	bl2_simple_io_test();

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


