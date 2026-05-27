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

#include <common/bl_common.h>
#include <common/debug.h>
#include <common/desc_image_load.h>

#include <drivers/auth/auth_mod.h>
#include <plat/common/platform.h>
#include <drivers/nxp/flexspi/nor/fspi.h>
#include <platform_def.h>

/* ================= XSPI / NOR APIs ================= */
/* These come from drivers/nxp/flexspi/nor/fspi.c */

int xspi_read(uint32_t offset, void *buf, size_t len);
int xspi_write(uint32_t offset, const void *buf, size_t len);
int xspi_erase(uint32_t offset, size_t len);

/* ================= NOR test config ================= */

#define NOR_TEST_OFFSET     0x400
#define NOR_TEST_SIZE       4

/* =================================================== */

struct entry_point_info *bl2_load_images(void)
{
	bl_params_t *bl2_to_next_bl_params;
	bl_load_info_t *bl2_load_info;
	const bl_load_info_node_t *bl2_node_info;
	int plat_setup_done = 0;
	int err;

	uint32_t write_val = 0xA5A5A5A5;
	uint32_t read_val  = 0;
	int rc;

	NOTICE("\n=================================================\n");
	NOTICE("BL2 XSPI NOR DIRECT ACCESS TEST (NO FIP)\n");
	NOTICE("NOR OFFSET = 0x%x\n", NOR_TEST_OFFSET);
	NOTICE("=================================================\n");

	/* -------- ERASE -------- */
	NOTICE("BL2 NOR: Erasing sector...\n");
	rc = xspi_erase(NOR_TEST_OFFSET, NOR_TEST_SIZE);
	if (rc != 0) {
		ERROR("BL2 NOR: erase FAILED (%d)\n", rc);
	} else {
		NOTICE("BL2 NOR: erase OK\n");
	}

	/* -------- WRITE -------- */
	NOTICE("BL2 NOR: Writing value 0x%08x\n", write_val);
	rc = xspi_write(NOR_TEST_OFFSET, &write_val, sizeof(write_val));
	if (rc != 0) {
		ERROR("BL2 NOR: write FAILED (%d)\n", rc);
	} else {
		NOTICE("BL2 NOR: write OK\n");
	}

	/* -------- READ -------- */
	NOTICE("BL2 NOR: Reading back value\n");
	rc = xspi_read(NOR_TEST_OFFSET, &read_val, sizeof(read_val));
	if (rc != 0) {
		ERROR("BL2 NOR: read FAILED (%d)\n", rc);
	} else {
		NOTICE("BL2 NOR: read OK, value = 0x%08x\n", read_val);
	}

	NOTICE("=================================================\n");
	NOTICE("BL2 NOR TEST DONE – continuing normal boot\n");
	NOTICE("=================================================\n");

	/* ================= NORMAL BL2 FLOW ================= */

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
