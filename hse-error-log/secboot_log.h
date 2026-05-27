#ifndef SECBOOT_LOG_H
#define SECBOOT_LOG_H

#include <stdint.h>

/*
 * Initialize secure boot log storage.
 * Must be called once in BL2.
 */
void secboot_log_init(void);

/*
 * Log secure boot authentication failure.
 * image_id  - BL3x image ID
 * error_code - TF-A / HSE error code
 */
void secboot_log_error(uint32_t image_id, int32_t error_code);

#endif /* SECBOOT_LOG_H */

