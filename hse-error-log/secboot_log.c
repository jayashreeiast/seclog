#include <stdint.h>
#include <string.h>

#include <common/debug.h>
#include <arch_helpers.h>

#include <platform_def.h>
#include "secboot_log.h"

/* NOR flash driver hooks (already in S32G BSP) */
extern int nor_flash_read(uint32_t addr, void *buf, uint32_t len);
extern int nor_flash_write(uint32_t addr, const void *buf, uint32_t len);
extern int nor_flash_erase_sector(uint32_t addr);

/* ============================================================
 * Log format
 * ============================================================ */

#define SECBOOT_LOG_MAGIC   0x534C4F47U   /* "SLOG" */

typedef struct {
	uint32_t magic;
	uint32_t write_offset;
	uint32_t version;
	uint32_t reserved;
} secboot_log_header_t;

typedef struct {
	uint32_t timestamp;
	uint32_t image_id;
	int32_t  error_code;
} secboot_log_entry_t;

/* ============================================================
 * Internal state
 * ============================================================ */

static secboot_log_header_t log_hdr;
static uint32_t log_base = PLAT_SECBOOT_LOG_BASE;

/* ============================================================
 * Utilities
 * ============================================================ */

static uint32_t get_boot_timestamp(void)
{
	uint64_t cnt;
	__asm__ volatile ("mrs %0, cntvct_el0" : "=r"(cnt));
	return (uint32_t)(cnt & 0xFFFFFFFFU);
}

/* ============================================================
 * API implementation
 * ============================================================ */

void secboot_log_init(void)
{
	nor_flash_read(log_base, &log_hdr, sizeof(log_hdr));

	if ((log_hdr.magic != SECBOOT_LOG_MAGIC) ||
	    (log_hdr.write_offset < sizeof(secboot_log_header_t)) ||
	    (log_hdr.write_offset >= PLAT_SECBOOT_LOG_SIZE)) {

		nor_flash_erase_sector(log_base);

		log_hdr.magic = SECBOOT_LOG_MAGIC;
		log_hdr.version = 1U;
		log_hdr.reserved = 0U;
		log_hdr.write_offset = sizeof(secboot_log_header_t);

		nor_flash_write(log_base, &log_hdr, sizeof(log_hdr));
	}
}

void secboot_log_error(uint32_t image_id, int32_t error_code)
{
	secboot_log_entry_t entry;
	uint32_t write_addr;

	entry.timestamp  = get_boot_timestamp();
	entry.image_id   = image_id;
	entry.error_code = error_code;

	write_addr = log_base + log_hdr.write_offset;

	/* Wrap log if full */
	if ((log_hdr.write_offset + sizeof(entry)) > PLAT_SECBOOT_LOG_SIZE) {
		secboot_log_init();
		write_addr = log_base + log_hdr.write_offset;
	}

	nor_flash_write(write_addr, &entry, sizeof(entry));

	log_hdr.write_offset += sizeof(entry);
	nor_flash_write(log_base, &log_hdr, sizeof(log_hdr));
}

