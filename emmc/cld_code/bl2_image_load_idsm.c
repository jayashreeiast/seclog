/*
 * Copyright (c) 2016-2022, ARM Limited and Contributors.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * bl2_image_load_v2.c — AUTOSAR IdsM secure logging, NXP S32G399A
 * HSE_FW_S32G3XX_0_2_64_0, STANDARD FW (HSE_FWTYPE=0)
 *
 * ═══════════════════════════════════════════════════════════════════════
 * DEFINITIVE ROOT CAUSE ANALYSIS (from hse_srv_key_import_export.h):
 *
 * Issue 1 — RSA pub import INVALID_PARAM (0x55a5a399):
 *   global_defs.h NVM group 5 = RSA_PAIR slots ONLY.
 *   Trying to import keyType=HSE_KEY_TYPE_RSA_PUB into an RSA_PAIR slot
 *   fails because the catalog slot type doesn't match.
 *   FIX: Import RSA_PUB as keyType=HSE_KEY_TYPE_RSA_PAIR with pKey[2]=0
 *        (no private exponent → HSE stores only n+e but the slot type matches).
 *        ALTERNATIVELY: skip pub import entirely since we only need SIGN.
 *        We choose to SKIP it — it is non-fatal and we don't use it for logging.
 *
 * Issue 2 — RSA sign 0xa5aa52b4:
 *   hse_mem_alloc() returns pointers into a shared HSE SRAM region that
 *   is NON-CACHED (device memory). Calling flush_dcache_range() on
 *   non-cached memory is undefined behaviour and can corrupt the memory
 *   transaction, producing a garbage response code.
 *   FIX: NEVER call flush_dcache_range() on hse_mem_alloc() buffers.
 *        Only flush regular DRAM buffers (e.g. static s_rec array).
 *        hse_mem_alloc SRAM is already coherent with the HSE M7 DMA.
 *
 * Issue 3 — AES key slot:
 *   Header says pKey[2] = symmetric key (AES/HMAC).
 *   Working patch uses pKey[0] and it works — HSE may accept either.
 *   We use pKey[2] per spec, matching the header comment exactly.
 *
 * ═══════════════════════════════════════════════════════════════════════
 * KEY CATALOG — STANDARD FW NVM (confirmed from global_defs.h):
 *   Group 5: RSA_PAIR, 2 slots, 2048-bit
 *     Slot 0 = NVM_RSA2048_BOOT_COPY_KEY_HANDLE        (system/boot)
 *     Slot 1 = NVM_RSA2048_PAIR_CUSTAUTH_COPY_HANDLE   ← IDSM SIGN key
 *   Group 7: AES, 2 slots, 256-bit
 *     Slot 0 = NVM_AES256_KEY0                          (system)
 *     Slot 1 = free                                    ← IDSM AES key
 *
 * ═══════════════════════════════════════════════════════════════════════
 * hseImportKeySrv_t CONFIRMED LAYOUT (hse_srv_key_import_export.h):
 *   targetKeyHandle           hseKeyHandle_t
 *   pKeyInfo                  HOST_ADDR → hseKeyInfo_t
 *   pKey[3]                   HOST_ADDR[3]
 *     pKey[0] = RSA modulus n
 *     pKey[1] = RSA exponent e
 *     pKey[2] = RSA private d  / AES/HMAC symmetric key
 *   keyLen[3]                 uint16_t[3]
 *   reserved[2]               uint8_t[2]
 *   cipher.cipherKeyHandle    hseKeyHandle_t  → HSE_INVALID_KEY_HANDLE
 *   cipher.cipherScheme       (leave zeroed)
 *   keyContainer.authKeyHandle hseKeyHandle_t → HSE_INVALID_KEY_HANDLE
 *   (all other keyContainer fields → leave zeroed)
 *   keyFormat                 (leave zeroed)
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

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

//#ifdef HSE_SECBOOT
# include <drivers/nxp/s32/hse/hse_core.h>
# include <drivers/nxp/s32/hse/hse_mem.h>
# include <hse_interface.h>
//#endif

/* ================================================================
 * AUTOSAR IdsM PROTOCOL CONSTANTS
 * ================================================================ */
#define IDSM_PROTOCOL_VERSION        0x1U
#define IDSM_HDR_BIT_TIMESTAMP       (1U << 1)
#define IDSM_HDR_BIT_SIGNATURE       (1U << 2)
#define IDSM_HDR_TS_AND_SIG   (IDSM_HDR_BIT_TIMESTAMP | IDSM_HDR_BIT_SIGNATURE)
#define IDSM_HDR_TS_ONLY       IDSM_HDR_BIT_TIMESTAMP
#define IDSM_TS_AUTOSAR              0x00U
#define IDSM_INSTANCE_A_CORE         0x000U
#define IDSM_MODULE_SECBOOT          0x00U

#define IDSM_EVENT_BL2_BOOT_START    0x8000U
#define IDSM_EVENT_BL2_BOOT_OK       0x8001U
#define IDSM_EVENT_BL31_FAIL         0x8010U
#define IDSM_EVENT_BL32_FAIL         0x8011U
#define IDSM_EVENT_BL33_FAIL         0x8012U
#define IDSM_EVENT_KERNEL_FAIL       0x8013U

#define IDSM_LOG_START_SECTOR        60686336U
#define IDSM_LOG_MAX_ENTRIES         64U
#define IDSM_BLOCK_SIZE              512U
#define IDSM_SECTORS_SECURE          1U
#define IDSM_SECTORS_PLAIN           2U

#define IDSM_EVENT_FRAME_BYTES       8U
#define IDSM_TIMESTAMP_BYTES         8U
#define IDSM_GCM_IV_BYTES            12U
#define IDSM_GCM_TAG_BYTES           16U
#define IDSM_RSA_SIG_BYTES           256U
#define IDSM_SIG_LEN_FIELD_BYTES     2U

/* ================================================================
 * KEY HANDLES — STANDARD FW NVM catalog
 *
 * AES-256:   NVM group 7, slot 1
 * RSA_PAIR:  NVM group 5, slot 1  (SIGN only — we do NOT import pub separately)
 * ================================================================ */
#define IDSM_NVM_AES256_KEY_HANDLE \
    ((hseKeyHandle_t)GET_KEY_HANDLE(HSE_KEY_CATALOG_ID_NVM, 7U, 1U))

#define IDSM_NVM_RSA_PAIR_KEY_HANDLE \
    ((hseKeyHandle_t)GET_KEY_HANDLE(HSE_KEY_CATALOG_ID_NVM, 5U, 1U))

/* ================================================================
 * KEY MATERIAL — real keys from openssl
 * ================================================================ */

/* AES-256 key (32 bytes) */
static const uint8_t IDSM_AES256_KEY_BYTES[32] = {
    0x2c, 0x08, 0x1a, 0xb4, 0x50, 0x61, 0xe8, 0x3a,
    0x20, 0x6d, 0x09, 0x5d, 0xbd, 0x0d, 0x9d, 0xa9,
    0xad, 0x4c, 0xd7, 0xf8, 0xac, 0x4a, 0x66, 0x26,
    0x03, 0xbc, 0xc1, 0x80, 0x80, 0xc2, 0xab, 0x11
};

/* RSA-2048 modulus n (256 bytes, big-endian) */
static const uint8_t IDSM_RSA_N[256] = {
    0xe0, 0x52, 0xfd, 0xef, 0xb1, 0x8b, 0xf0, 0x18,
    0xe1, 0xbd, 0xd1, 0x25, 0x7d, 0x7f, 0xca, 0xbe,
    0x4e, 0x36, 0x98, 0xc3, 0x00, 0xcd, 0xd4, 0x79,
    0xe8, 0x2e, 0xf5, 0x71, 0xb5, 0xa6, 0x11, 0xc0,
    0xb2, 0xae, 0x76, 0xbc, 0xbc, 0xa6, 0x8f, 0xc7,
    0x84, 0x41, 0x42, 0xb8, 0x27, 0x0f, 0x94, 0xc3,
    0xd8, 0x3f, 0x07, 0x2b, 0x67, 0x79, 0x26, 0x77,
    0xa1, 0xb9, 0xf0, 0xcd, 0x37, 0xab, 0x5c, 0x63,
    0x01, 0xff, 0x77, 0xcd, 0x8e, 0xa9, 0x36, 0x90,
    0x08, 0x3a, 0xdc, 0xe6, 0x76, 0x26, 0xa2, 0xc1,
    0x5b, 0xb2, 0xa4, 0x5a, 0xb6, 0x95, 0x2b, 0x94,
    0xbb, 0x0e, 0xdc, 0xb1, 0xe9, 0x3b, 0x72, 0x18,
    0x98, 0x5e, 0x84, 0x63, 0x4e, 0x2f, 0xdd, 0x7b,
    0x13, 0x3b, 0xbd, 0x9c, 0x75, 0x50, 0x96, 0xe1,
    0xa8, 0xdf, 0x68, 0x39, 0xf4, 0x25, 0x2e, 0x51,
    0xe5, 0x07, 0x8d, 0xbc, 0x10, 0xf3, 0x4e, 0x38,
    0x4d, 0xa7, 0x6f, 0x77, 0x8f, 0xa5, 0xc5, 0x0a,
    0xc7, 0x93, 0x30, 0x58, 0x4b, 0xab, 0xed, 0x07,
    0xfb, 0xea, 0xf5, 0xaa, 0x04, 0xb6, 0xc5, 0xc2,
    0x3f, 0x2a, 0xd8, 0x90, 0xc5, 0x28, 0xd5, 0x06,
    0x75, 0xc0, 0xf6, 0x58, 0x5a, 0x06, 0x46, 0x38,
    0x86, 0x6c, 0xcb, 0x1b, 0x01, 0xe2, 0xe7, 0x6f,
    0x08, 0x00, 0x9c, 0x26, 0xac, 0x6b, 0xcb, 0xf7,
    0x2a, 0xcc, 0x6c, 0x7e, 0x66, 0xd2, 0xb1, 0x7d,
    0xd4, 0x4e, 0x37, 0x6b, 0x3a, 0x09, 0x22, 0x68,
    0xcb, 0x96, 0x19, 0x36, 0xb7, 0xc1, 0x3c, 0x9f,
    0x42, 0x37, 0xc1, 0x86, 0xec, 0xf5, 0xe9, 0xa3,
    0xee, 0xb8, 0x1d, 0x6e, 0xc8, 0x37, 0x3a, 0xea,
    0xde, 0x50, 0x62, 0xa6, 0x79, 0xa2, 0x43, 0x58,
    0x2e, 0x64, 0x50, 0x4d, 0x5b, 0x08, 0xed, 0xc3,
    0x3b, 0xa2, 0xdf, 0x7a, 0xf8, 0xe3, 0x57, 0x2d,
    0x86, 0xb2, 0x07, 0x1b, 0x2b, 0x42, 0x79, 0x1b
};

/* RSA public exponent e = 65537 */
static const uint8_t IDSM_RSA_E[3] = { 0x01, 0x00, 0x01 };

/* RSA-2048 private exponent d (256 bytes, big-endian) */
static const uint8_t IDSM_RSA_D[256] = {
    0x1e, 0x24, 0x48, 0xa6, 0xba, 0x6e, 0x99, 0x36,
    0x5e, 0xae, 0xc8, 0xb9, 0x86, 0xe1, 0x72, 0xf6,
    0x9b, 0x3f, 0xce, 0xca, 0xf5, 0x02, 0xb3, 0x38,
    0x26, 0x30, 0x99, 0x1e, 0x4c, 0x50, 0x22, 0x36,
    0x88, 0x4b, 0xd4, 0xfc, 0x8b, 0x44, 0xbe, 0x31,
    0x0a, 0xde, 0x51, 0xfe, 0xb8, 0x90, 0xe9, 0x66,
    0x5f, 0x5a, 0x28, 0xb6, 0x76, 0x52, 0xdb, 0x0e,
    0x5c, 0x16, 0xc5, 0xfa, 0x6d, 0x2e, 0x83, 0x5d,
    0x72, 0x59, 0x47, 0x59, 0xc1, 0x7f, 0x8f, 0x2b,
    0x4f, 0x4f, 0xc9, 0x7a, 0xd6, 0x09, 0x5e, 0xf9,
    0x28, 0x3f, 0x1a, 0xc2, 0x6d, 0xa2, 0x21, 0x96,
    0xfc, 0x83, 0x84, 0xb8, 0x0b, 0x24, 0xc7, 0xb1,
    0xf2, 0x57, 0x50, 0xe6, 0x9f, 0x0c, 0xe3, 0x6f,
    0xb4, 0x27, 0xe4, 0xd4, 0x6f, 0x17, 0x03, 0x0f,
    0x13, 0x47, 0xe1, 0x93, 0x57, 0xf0, 0xdf, 0x06,
    0x76, 0x05, 0xcb, 0xcf, 0xa7, 0x5f, 0x69, 0xf4,
    0xfa, 0x52, 0x90, 0x26, 0xf0, 0x79, 0x41, 0xab,
    0x31, 0x88, 0xa2, 0xdf, 0x93, 0x91, 0x75, 0x78,
    0x47, 0xd5, 0xb7, 0x15, 0x76, 0x91, 0x10, 0x1a,
    0xe3, 0x8f, 0x52, 0xdf, 0x06, 0x10, 0x4f, 0x46,
    0x3a, 0xec, 0xf6, 0xcb, 0x02, 0xa8, 0xa0, 0x43,
    0xa3, 0x7f, 0x94, 0x45, 0x2d, 0x3f, 0x13, 0x2e,
    0x27, 0x34, 0xc1, 0x63, 0xe5, 0x38, 0xf6, 0x2c,
    0x4e, 0xd1, 0x79, 0xee, 0x56, 0x2a, 0x44, 0x17,
    0x3d, 0xab, 0x88, 0x8d, 0x42, 0xc4, 0x10, 0x49,
    0x85, 0x3b, 0x83, 0xdd, 0x05, 0x00, 0xae, 0x1c,
    0xea, 0x6f, 0x0a, 0x13, 0x9b, 0x77, 0x83, 0xa4,
    0xf6, 0x82, 0x67, 0xa3, 0x93, 0x0a, 0xb2, 0xf5,
    0xef, 0x50, 0xe4, 0x24, 0xe9, 0x4f, 0x11, 0xad,
    0x29, 0x6d, 0xf6, 0x4c, 0x43, 0x8b, 0xd5, 0xa6,
    0xe7, 0xef, 0x08, 0x61, 0xa8, 0xc9, 0x0c, 0x18,
    0x64, 0x6f, 0x16, 0xe5, 0xa0, 0xed, 0x7c, 0x31
};

/* ================================================================
 * AUTOSAR IdsM WIRE FORMAT STRUCTS
 * ================================================================ */
typedef struct {
    uint8_t ver_hdr;
    uint8_t inst_hi;
    uint8_t inst_lo_mod;
    uint8_t event_hi;
    uint8_t event_lo;
    uint8_t count_hi;
    uint8_t count_lo;
    uint8_t reserve;
} __attribute__((packed)) idsm_event_frame_t;

typedef struct {
    uint8_t type_flags;
    uint8_t ts_ms[7];
} __attribute__((packed)) idsm_timestamp_frame_t;

typedef struct {
    uint8_t sig_len_hi;
    uint8_t sig_len_lo;
    uint8_t sig_data[IDSM_RSA_SIG_BYTES];
} __attribute__((packed)) idsm_signature_frame_t;

#define IDSM_RECORD_PAD \
    (IDSM_BLOCK_SIZE \
     - IDSM_EVENT_FRAME_BYTES \
     - IDSM_GCM_IV_BYTES \
     - IDSM_GCM_TAG_BYTES \
     - IDSM_TIMESTAMP_BYTES \
     - (IDSM_SIG_LEN_FIELD_BYTES + IDSM_RSA_SIG_BYTES))

typedef struct {
    idsm_event_frame_t      event_frame;
    uint8_t                 gcm_iv[IDSM_GCM_IV_BYTES];
    uint8_t                 gcm_tag[IDSM_GCM_TAG_BYTES];
    idsm_timestamp_frame_t  enc_timestamp;
    idsm_signature_frame_t  sig_frame;
    uint8_t                 pad[IDSM_RECORD_PAD];
} __attribute__((packed)) idsm_secure_record_t;

typedef char _idsm_sz[(sizeof(idsm_secure_record_t) == 512) ? 1 : -1];

/* ================================================================
 * MODULE STATE
 * ================================================================ */
static uint32_t s_log_index  = 0;
static int      s_hse_ready  = 0;
static int      s_keys_ready = 0;

/* ================================================================
 * TIMER
 * ================================================================ */
static void get_timestamp_ms(uint8_t out_ms[7])
{
    uint64_t cnt  = read_cntpct_el0();
    uint64_t freq = read_cntfrq_el0();

    NOTICE("IDSM_TIMER: cnt=%lu freq=%lu\n", cnt, freq);

    if (freq == 0U) {
#ifdef COUNTER_FREQUENCY
        freq = (uint64_t)COUNTER_FREQUENCY;
#else
        freq = 5000000ULL;
#endif
    }
    uint64_t ms = (cnt * 1000ULL) / freq;
    for (int i = 6; i >= 0; i--) {
        out_ms[i] = (uint8_t)(ms & 0xFFU);
        ms >>= 8;
    }
}

/* ================================================================
 * FRAME BUILDERS
 * ================================================================ */
static void build_event_frame(idsm_event_frame_t *ef,
                               uint16_t instance_id, uint8_t module_id,
                               uint8_t hdr_flags,
                               uint16_t event_id, uint16_t count)
{
    ef->ver_hdr     = (uint8_t)(((IDSM_PROTOCOL_VERSION & 0xFU) << 4) |
                                  (hdr_flags & 0xFU));
    ef->inst_hi     = (uint8_t)((instance_id >> 2) & 0xFFU);
    ef->inst_lo_mod = (uint8_t)(((instance_id & 0x3U) << 6) |
                                  (module_id & 0x3FU));
    ef->event_hi    = (uint8_t)((event_id >> 8) & 0xFFU);
    ef->event_lo    = (uint8_t)( event_id        & 0xFFU);
    ef->count_hi    = (uint8_t)((count >> 8) & 0xFFU);
    ef->count_lo    = (uint8_t)( count        & 0xFFU);
    ef->reserve     = 0x00U;
}

static void build_timestamp_frame(idsm_timestamp_frame_t *ts)
{
    memset(ts, 0, sizeof(*ts));
    ts->type_flags = IDSM_TS_AUTOSAR;
    get_timestamp_ms(ts->ts_ms);
}

static void build_gcm_iv(uint8_t iv[IDSM_GCM_IV_BYTES], uint32_t seq)
{
    static const uint8_t pfx[4] = { 0xA5, 0x5A, 0xC3, 0x3C };
    memcpy(iv, pfx, 4U);
    memset(iv + 4U, 0, 4U);
    iv[8]  = (uint8_t)((seq >> 24) & 0xFFU);
    iv[9]  = (uint8_t)((seq >> 16) & 0xFFU);
    iv[10] = (uint8_t)((seq >>  8) & 0xFFU);
    iv[11] = (uint8_t)( seq        & 0xFFU);
}

/* ================================================================
 * STEP 1 — HSE DRIVER INIT
 * ================================================================ */
static void idsm_hse_init(void)
{
//#ifdef HSE_SECBOOT
    int ret;

    if (s_hse_ready)
        return;

    ret = hse_driver_init();
    if (ret != 0) {
        ERROR("IDSM: hse_driver_init failed (%d)\n", ret);
        return;
    }

    s_hse_ready = 1;
    NOTICE("IDSM: HSE driver ready\n");
    return;
//#else
    NOTICE("IDSM: HSE_SECBOOT not set\n");
//#endif
}

/* ================================================================
 * STEP 2A — IMPORT AES-256 KEY → NVM (7,1)
 *
 * Per hse_srv_key_import_export.h:
 *   pKey[2] = "The symmetric key (e.g AES, HMAC)"
 *   keyLen[2] = 32
 *   pKey[0] = 0, pKey[1] = 0  (unused for symmetric)
 *
 * cipher.cipherKeyHandle = HSE_INVALID_KEY_HANDLE  (plain, no wrapping)
 * keyContainer.authKeyHandle = HSE_INVALID_KEY_HANDLE (no auth container)
 *
 * NOTE: Do NOT call flush_dcache_range on hse_mem_alloc buffers.
 *       HSE SRAM is non-cached device memory — already coherent.
 * ================================================================ */
static int idsm_import_aes256_key(void)
{
    hseSrvDescriptor_t  desc = {0};
    hseImportKeySrv_t  *imp  = &desc.hseSrv.importKeyReq;
    hseKeyInfo_t        ki   = {0};
    void  *ki_buf  = NULL;
    void  *key_buf = NULL;
    int    ret     = -ENOMEM;

    ki_buf  = hse_mem_alloc(sizeof(hseKeyInfo_t));
    key_buf = hse_mem_alloc(32U);

    if ((ki_buf == NULL) || (key_buf == NULL)) {
        ERROR("IDSM: AES alloc failed\n");
        goto done;
    }

    ki.keyType   = HSE_KEY_TYPE_AES;
    ki.keyBitLen = 256U;
    ki.keyFlags  = (uint16_t)(HSE_KF_USAGE_ENCRYPT | HSE_KF_USAGE_DECRYPT);

    hse_memcpy(ki_buf,  &ki,                   sizeof(hseKeyInfo_t));
    hse_memcpy(key_buf,  IDSM_AES256_KEY_BYTES, 32U);

    desc.srvId           = HSE_SRV_ID_IMPORT_KEY;
    imp->targetKeyHandle = IDSM_NVM_AES256_KEY_HANDLE;
    imp->pKeyInfo        = hse_virt_to_phys(ki_buf);

    /* Symmetric key → pKey[2] per header spec */
    imp->pKey[2]   = hse_virt_to_phys(key_buf);
    imp->keyLen[2] = 32U;

    imp->cipher.cipherKeyHandle     = HSE_INVALID_KEY_HANDLE;
    imp->keyContainer.authKeyHandle = HSE_INVALID_KEY_HANDLE;

    ret = hse_srv_req_sync(HSE_CHANNEL_CRYPTO, &desc);
    if (ret != 0)
        ERROR("IDSM: AES import failed ret=%d\n", ret);
    else
        NOTICE("IDSM: AES-256 imported → NVM (7,1)\n");

done:
    if (key_buf) hse_mem_free(key_buf);
    if (ki_buf)  hse_mem_free(ki_buf);
    return ret;
}

/* ================================================================
 * STEP 2B — IMPORT RSA-2048 PRIVATE KEY PAIR → NVM (5,1)
 *
 * Per hse_srv_key_import_export.h:
 *   "A RSA private key should always be imported together with
 *    the public key."
 *   pKey[0] = modulus n       (256 bytes)
 *   pKey[1] = exponent e      (3 bytes)
 *   pKey[2] = private exp d   (256 bytes)  ← present = RSA_PAIR
 *
 * keyType  = HSE_KEY_TYPE_RSA_PAIR
 * keyFlags = HSE_KF_USAGE_SIGN
 *            (this is what allows HSE_SRV_ID_SIGN to use this key)
 *
 * We do NOT import a separate RSA_PUB key because:
 *   - NVM group 5 = RSA_PAIR slots only (no RSA_PUB group in STANDARD FW)
 *   - The RSA_PUB import is non-fatal and not needed for IDSM logging
 *   - Trying HSE_KEY_TYPE_RSA_PUB into an RSA_PAIR slot → INVALID_PARAM
 * ================================================================ */
static int idsm_import_rsa_pair_key(void)
{
    hseSrvDescriptor_t  desc = {0};
    hseImportKeySrv_t  *imp  = &desc.hseSrv.importKeyReq;
    hseKeyInfo_t        ki   = {0};
    void *ki_buf = NULL, *n_buf = NULL, *e_buf = NULL, *d_buf = NULL;
    int   ret    = -ENOMEM;

    ki_buf = hse_mem_alloc(sizeof(hseKeyInfo_t));
    n_buf  = hse_mem_alloc(256U);
    e_buf  = hse_mem_alloc(sizeof(IDSM_RSA_E));
    d_buf  = hse_mem_alloc(256U);

    if ((ki_buf == NULL) || (n_buf == NULL) ||
        (e_buf  == NULL) || (d_buf == NULL)) {
        ERROR("IDSM: RSA pair alloc failed\n");
        goto done;
    }

    ki.keyType   = HSE_KEY_TYPE_RSA_PAIR;
    ki.keyBitLen = 2048U;
    ki.keyFlags  = (uint16_t)HSE_KF_USAGE_SIGN;

    hse_memcpy(ki_buf, &ki,         sizeof(hseKeyInfo_t));
    hse_memcpy(n_buf,   IDSM_RSA_N, 256U);
    hse_memcpy(e_buf,   IDSM_RSA_E, sizeof(IDSM_RSA_E));
    hse_memcpy(d_buf,   IDSM_RSA_D, 256U);

    desc.srvId           = HSE_SRV_ID_IMPORT_KEY;
    imp->targetKeyHandle = IDSM_NVM_RSA_PAIR_KEY_HANDLE;
    imp->pKeyInfo        = hse_virt_to_phys(ki_buf);

    imp->pKey[0]   = hse_virt_to_phys(n_buf);       /* modulus n  */
    imp->keyLen[0] = 256U;
    imp->pKey[1]   = hse_virt_to_phys(e_buf);       /* exponent e */
    imp->keyLen[1] = (uint16_t)sizeof(IDSM_RSA_E);
    imp->pKey[2]   = hse_virt_to_phys(d_buf);       /* private d  */
    imp->keyLen[2] = 256U;

    imp->cipher.cipherKeyHandle     = HSE_INVALID_KEY_HANDLE;
    imp->keyContainer.authKeyHandle = HSE_INVALID_KEY_HANDLE;

    ret = hse_srv_req_sync(HSE_CHANNEL_CRYPTO, &desc);
    if (ret != 0)
        ERROR("IDSM: RSA pair import failed ret=%d\n", ret);
    else
        NOTICE("IDSM: RSA-2048 pair imported → NVM (5,1) [SIGN enabled]\n");

done:
    if (d_buf)  hse_mem_free(d_buf);
    if (e_buf)  hse_mem_free(e_buf);
    if (n_buf)  hse_mem_free(n_buf);
    if (ki_buf) hse_mem_free(ki_buf);
    return ret;
}

/* ================================================================
 * STEP 2 — PROVISION ALL KEYS (once per boot)
 * ================================================================ */
static void idsm_provision_keys(void)
{
    if (s_keys_ready)
        return;

    NOTICE("IDSM: Provisioning keys into HSE NVM catalog...\n");

    if (idsm_import_aes256_key() != 0) {
        ERROR("IDSM: AES key provision failed\n");
        return;
    }

    if (idsm_import_rsa_pair_key() != 0) {
        ERROR("IDSM: RSA pair provision failed\n");
        return;
    }

    s_keys_ready = 1;
    NOTICE("IDSM: Keys provisioned — secure logging active\n");
}

/* ================================================================
 * STEP 3A — AES-256-GCM ENCRYPT TIMESTAMP
 *
 * tagLength = uint32_t BY VALUE in FW 0_2_64_0 (confirmed by compiler).
 * NO flush_dcache_range on hse_mem_alloc buffers — HSE SRAM is
 * non-cached device memory, already coherent with HSE M7 DMA.
 * ================================================================ */
static int idsm_aes_gcm_encrypt(
        const idsm_timestamp_frame_t  *plain_ts,
        const idsm_event_frame_t      *aad_ef,
        const uint8_t                  iv[IDSM_GCM_IV_BYTES],
        idsm_timestamp_frame_t        *out_enc_ts,
        uint8_t                        out_tag[IDSM_GCM_TAG_BYTES])
{
    hseSrvDescriptor_t  desc  = {0};
    hseAeadSrv_t       *aead  = &desc.hseSrv.aeadReq;
    void    *iv_buf  = NULL, *aad_buf = NULL;
    void    *in_buf  = NULL, *out_buf = NULL;
    void    *tag_buf = NULL;
    int      ret     = -ENOMEM;

    iv_buf   = hse_mem_alloc((uint32_t)IDSM_GCM_IV_BYTES);
    aad_buf  = hse_mem_alloc((uint32_t)IDSM_EVENT_FRAME_BYTES);
    in_buf   = hse_mem_alloc((uint32_t)IDSM_TIMESTAMP_BYTES);
    out_buf  = hse_mem_alloc((uint32_t)IDSM_TIMESTAMP_BYTES);
    tag_buf  = hse_mem_alloc((uint32_t)IDSM_GCM_TAG_BYTES);

    if (!iv_buf || !aad_buf || !in_buf || !out_buf || !tag_buf) {
        ERROR("IDSM: GCM alloc OOM\n");
        goto done;
    }

    hse_memcpy(iv_buf,  iv,       (uint32_t)IDSM_GCM_IV_BYTES);
    hse_memcpy(aad_buf, aad_ef,   (uint32_t)IDSM_EVENT_FRAME_BYTES);
    hse_memcpy(in_buf,  plain_ts, (uint32_t)IDSM_TIMESTAMP_BYTES);

    desc.srvId           = HSE_SRV_ID_AEAD;
    aead->accessMode     = HSE_ACCESS_MODE_ONE_PASS;
    aead->authCipherMode = HSE_AUTH_CIPHER_MODE_GCM;
    aead->cipherDir      = HSE_CIPHER_DIR_ENCRYPT;
    aead->keyHandle      = IDSM_NVM_AES256_KEY_HANDLE;
    aead->ivLength       = (uint32_t)IDSM_GCM_IV_BYTES;
    aead->pIV            = hse_virt_to_phys(iv_buf);
    aead->aadLength      = (uint32_t)IDSM_EVENT_FRAME_BYTES;
    aead->pAAD           = hse_virt_to_phys(aad_buf);
    aead->inputLength    = (uint32_t)IDSM_TIMESTAMP_BYTES;
    aead->pInput         = hse_virt_to_phys(in_buf);
    aead->pOutput        = hse_virt_to_phys(out_buf);
    aead->tagLength      = (uint32_t)IDSM_GCM_TAG_BYTES;  /* BY VALUE */
    aead->pTag           = hse_virt_to_phys(tag_buf);

    ret = hse_srv_req_sync(HSE_CHANNEL_CRYPTO, &desc);
    if (ret != 0) {
        ERROR("IDSM: GCM encrypt failed ret=%d\n", ret);
        goto done;
    }

    hse_memcpy(out_enc_ts, out_buf, (uint32_t)IDSM_TIMESTAMP_BYTES);
    hse_memcpy(out_tag,    tag_buf, (uint32_t)IDSM_GCM_TAG_BYTES);
    NOTICE("IDSM: AES-256-GCM encrypt OK\n");

done:
    if (tag_buf) hse_mem_free(tag_buf);
    if (out_buf) hse_mem_free(out_buf);
    if (in_buf)  hse_mem_free(in_buf);
    if (aad_buf) hse_mem_free(aad_buf);
    if (iv_buf)  hse_mem_free(iv_buf);
    return ret;
}

/* ================================================================
 * STEP 3B — RSA-2048-PKCS1v15-SHA256 SIGN
 *
 * Uses IDSM_NVM_RSA_PAIR_KEY_HANDLE (NVM,5,1) with SIGN flag.
 * Signs: event_frame(8B) + enc_timestamp(8B) = 16 bytes.
 *
 * hseSignSrv_t required fields:
 *   accessMode    = HSE_ACCESS_MODE_ONE_PASS
 *   streamId      = 0
 *   authDir       = HSE_AUTH_DIR_GENERATE  ← CRITICAL: missing this = 0
 *                   (default zero = HSE_AUTH_DIR_VERIFY, which fails
 *                    because key has SIGN flag not VERIFY flag)
 *   keyHandle     = RSA_PAIR key with HSE_KF_USAGE_SIGN
 *   signScheme    = RSASSA_PKCS1_V15 / SHA2_256
 *   bInputIsHashed= FALSE (HSE hashes internally)
 *   pSignatureLength[0] = phys addr of uint32 (max in, actual out)
 *   pSignature[0]       = phys addr of 256-byte output buffer
 *
 * DO NOT call flush_dcache_range on hse_mem_alloc buffers —
 * HSE SRAM is non-cached device memory, already coherent.
 * ================================================================ */
static int idsm_rsa_pkcs1_sign(
        const idsm_event_frame_t     *ef,
        const idsm_timestamp_frame_t *enc_ts,
        uint8_t                       out_sig[IDSM_RSA_SIG_BYTES])
{
    hseSrvDescriptor_t  desc   = {0};
    hseSignSrv_t       *sign   = &desc.hseSrv.signReq;

    uint8_t  sign_input[IDSM_EVENT_FRAME_BYTES + IDSM_TIMESTAMP_BYTES];
    uint32_t sign_len = (uint32_t)sizeof(sign_input);
    uint32_t sig_len  = (uint32_t)IDSM_RSA_SIG_BYTES;

    memcpy(sign_input,                         ef,     IDSM_EVENT_FRAME_BYTES);
    memcpy(sign_input + IDSM_EVENT_FRAME_BYTES, enc_ts, IDSM_TIMESTAMP_BYTES);

    void *in_buf   = hse_mem_alloc(sign_len);
    void *sig_buf  = hse_mem_alloc(sig_len);
    void *slen_buf = hse_mem_alloc(sizeof(uint32_t));
    int   ret      = -ENOMEM;

    if (!in_buf || !sig_buf || !slen_buf) {
        ERROR("IDSM: RSA sign alloc OOM\n");
        goto done;
    }

    hse_memcpy(in_buf,   sign_input, sign_len);
    hse_memcpy(slen_buf, &sig_len,   sizeof(uint32_t));
    /* Zero the output buffer in HSE SRAM — no dcache flush needed */
    memset(sig_buf, 0, sig_len);

    desc.srvId                                = HSE_SRV_ID_SIGN;
    sign->accessMode                          = HSE_ACCESS_MODE_ONE_PASS;
    sign->streamId                            = 0U;
    sign->authDir                             = HSE_AUTH_DIR_GENERATE; /* GENERATE=sign, VERIFY=check */
    sign->keyHandle                           = IDSM_NVM_RSA_PAIR_KEY_HANDLE;
    sign->signScheme.signSch                  = HSE_SIGN_RSASSA_PKCS1_V15;
    sign->signScheme.sch.rsaPkcs1v15.hashAlgo = HSE_HASH_ALGO_SHA2_256;
    sign->bInputIsHashed                      = FALSE;
    sign->inputLength                         = sign_len;
    sign->pInput                              = hse_virt_to_phys(in_buf);
    sign->pSignatureLength[0]                 = hse_virt_to_phys(slen_buf);
    sign->pSignature[0]                       = hse_virt_to_phys(sig_buf);
    sign->pSignatureLength[1]                 = 0U;
    sign->pSignature[1]                       = 0U;

    ret = hse_srv_req_sync(HSE_CHANNEL_CRYPTO, &desc);
    if (ret != 0) {
        ERROR("IDSM: RSA sign failed ret=%d\n", ret);
        goto done;
    }

    hse_memcpy(out_sig, sig_buf, sig_len);
    NOTICE("IDSM: RSA-2048-PKCS1v15-SHA256 sign OK\n");

done:
    if (slen_buf) hse_mem_free(slen_buf);
    if (sig_buf)  hse_mem_free(sig_buf);
    if (in_buf)   hse_mem_free(in_buf);
    return ret;
}

/* ================================================================
 * SECURE WRITE PATH
 * ================================================================ */
//#ifdef HSE_SECBOOT
static void idsm_write_secure(uint16_t event_id, uint16_t count,
                               uint32_t sector)
{
    static idsm_secure_record_t s_rec __attribute__((aligned(64)));
    idsm_timestamp_frame_t plain_ts;
    int ret;

    memset(&s_rec, 0, sizeof(s_rec));

    build_event_frame(&s_rec.event_frame,
                      (uint16_t)IDSM_INSTANCE_A_CORE,
                      (uint8_t) IDSM_MODULE_SECBOOT,
                      (uint8_t) IDSM_HDR_TS_AND_SIG,
                      event_id, count);

    build_gcm_iv(s_rec.gcm_iv, s_log_index);
    build_timestamp_frame(&plain_ts);

    ret = idsm_aes_gcm_encrypt(&plain_ts, &s_rec.event_frame,
                                s_rec.gcm_iv,
                                &s_rec.enc_timestamp, s_rec.gcm_tag);
    if (ret != 0) {
        ERROR("IDSM: GCM failed — plaintext fallback\n");
        uint8_t blk[IDSM_BLOCK_SIZE] = {0};
        memcpy(blk, &s_rec.event_frame, IDSM_EVENT_FRAME_BYTES);
        mmc_write_blocks(sector, (uintptr_t)blk, 512);
        memset(blk, 0, IDSM_BLOCK_SIZE);
        memcpy(blk, &plain_ts, IDSM_TIMESTAMP_BYTES);
        mmc_write_blocks(sector + 1U, (uintptr_t)blk, 512);
        return;
    }

    s_rec.sig_frame.sig_len_hi = (uint8_t)((IDSM_RSA_SIG_BYTES >> 8) & 0xFFU);
    s_rec.sig_frame.sig_len_lo = (uint8_t)( IDSM_RSA_SIG_BYTES        & 0xFFU);

    ret = idsm_rsa_pkcs1_sign(&s_rec.event_frame, &s_rec.enc_timestamp,
                               s_rec.sig_frame.sig_data);
    if (ret != 0) {
        ERROR("IDSM: RSA sign failed — dropping record\n");
        return;
    }

    /*
     * s_rec is a static in regular DRAM → flush is correct here.
     * DO NOT flush hse_mem_alloc buffers (done inside idsm_rsa_pkcs1_sign
     * and idsm_aes_gcm_encrypt — those are HSE SRAM, non-cached).
     */
    flush_dcache_range((uintptr_t)&s_rec, sizeof(s_rec));

    ret = mmc_write_blocks(sector, (uintptr_t)&s_rec, 512);
    if (ret != 512) {
        ERROR("IDSM: mmc_write_blocks failed ret=%d\n", ret);
        return;
    }

    NOTICE("IDSM: secured record written sector=%u seq=%u event=0x%04X\n",
           sector, s_log_index, event_id);
}
//#endif

/* ================================================================
 * PLAINTEXT FALLBACK WRITE
 * ================================================================ */
static void idsm_write_plain(uint16_t event_id, uint16_t count,
                              uint32_t base_sector)
{
    idsm_event_frame_t     ef;
    idsm_timestamp_frame_t ts;
    uint8_t                blk[IDSM_BLOCK_SIZE];
    int                    ret;

    build_event_frame(&ef,
                      (uint16_t)IDSM_INSTANCE_A_CORE,
                      (uint8_t) IDSM_MODULE_SECBOOT,
                      (uint8_t) IDSM_HDR_TS_ONLY,
                      event_id, count);
    build_timestamp_frame(&ts);

    memset(blk, 0, IDSM_BLOCK_SIZE);
    memcpy(blk, &ef, IDSM_EVENT_FRAME_BYTES);
    ret = mmc_write_blocks(base_sector, (uintptr_t)blk, 512);
    if (ret != 512) {
        ERROR("IDSM_PT: event write failed sector=%u\n", base_sector);
        return;
    }

    memset(blk, 0, IDSM_BLOCK_SIZE);
    memcpy(blk, &ts, IDSM_TIMESTAMP_BYTES);
    ret = mmc_write_blocks(base_sector + 1U, (uintptr_t)blk, 512);
    if (ret != 512) {
        ERROR("IDSM_PT: ts write failed sector=%u\n", base_sector + 1U);
        return;
    }

    NOTICE("IDSM_PT: plain event=0x%04X sector=%u\n", event_id, base_sector);
}

/* ================================================================
 * PUBLIC API
 * ================================================================ */
void idsm_log_event(uint16_t event_id, uint16_t count)
{
    uint32_t slot        = s_log_index % IDSM_LOG_MAX_ENTRIES;
    uint32_t base_sector = IDSM_LOG_START_SECTOR + (slot * IDSM_SECTORS_SECURE);

//#ifdef HSE_SECBOOT
    NOTICE("IDSM_LOG: event=0x%04X count=%u slot=%u sector=%u hse=%d keys=%d\n",
           event_id, count, slot, base_sector, s_hse_ready, s_keys_ready);

    if (s_hse_ready && s_keys_ready) {
        idsm_write_secure(event_id, count, base_sector);
    } else {
        slot        = s_log_index % IDSM_LOG_MAX_ENTRIES;
        base_sector = IDSM_LOG_START_SECTOR
                      + (IDSM_LOG_MAX_ENTRIES * IDSM_SECTORS_SECURE)
                      + (slot * IDSM_SECTORS_PLAIN);
        idsm_write_plain(event_id, count, base_sector);
    }
//#endif

    s_log_index++;
}

/* ================================================================
 * BL2 LOAD IMAGES
 * ================================================================ */
struct entry_point_info *bl2_load_images(void)
{
    bl_params_t               *bl2_to_next_bl_params;
    bl_load_info_t            *bl2_load_info;
    const bl_load_info_node_t *bl2_node_info;
    int   plat_setup_done = 0;
    int   err;
    bool  retried = false;

    idsm_hse_init();
    if (s_hse_ready)
        idsm_provision_keys();

    idsm_log_event(IDSM_EVENT_BL2_BOOT_START, 1U);

restart:
    bl2_load_info = plat_get_bl_image_load_info();
    assert(bl2_load_info != NULL);
    assert(bl2_load_info->head != NULL);
    assert(bl2_load_info->h.type == PARAM_BL_LOAD_INFO);
    assert(bl2_load_info->h.version >= VERSION_2);

    bl2_node_info = bl2_load_info->head;

    while (bl2_node_info != NULL) {

        if (((bl2_node_info->image_info->h.attr &
              IMAGE_ATTRIB_PLAT_SETUP) != 0U) &&
            (plat_setup_done == 0)) {
            INFO("BL2: Doing platform setup\n");
            bl2_platform_setup();
            plat_setup_done = 1;
        }

        err = bl2_plat_handle_pre_image_load(bl2_node_info->image_id);
        if (err != 0) {
            ERROR("BL2: pre-load fail id=%u err=%d\n",
                  bl2_node_info->image_id, err);
            idsm_log_event(IDSM_EVENT_BL31_FAIL, 1U);
            if (!retried) {
                plat_switch_to_secondary_fip();
                plat_setup_done = 0;
                retried = true;
                goto restart;
            }
            plat_error_handler(err);
        }

        if ((bl2_node_info->image_info->h.attr &
             IMAGE_ATTRIB_SKIP_LOADING) == 0U) {
            INFO("BL2: Loading image id %u\n", bl2_node_info->image_id);
            err = load_auth_image(bl2_node_info->image_id,
                                  bl2_node_info->image_info);
            if (err != 0) {
                ERROR("BL2: load fail id=%u err=%d\n",
                      bl2_node_info->image_id, err);
                idsm_log_event(IDSM_EVENT_BL31_FAIL, 1U);
                if (!retried) {
                    plat_switch_to_secondary_fip();
                    plat_setup_done = 0;
                    retried = true;
                    goto restart;
                }
                plat_error_handler(err);
            }
        } else {
            INFO("BL2: Skip loading image id %u\n",
                 bl2_node_info->image_id);
        }

        err = bl2_plat_handle_post_image_load(bl2_node_info->image_id);
        if (err != 0) {
            ERROR("BL2: post-load fail id=%u err=%d\n",
                  bl2_node_info->image_id, err);
            idsm_log_event(IDSM_EVENT_BL31_FAIL, 1U);
            if (!retried) {
                plat_switch_to_secondary_fip();
                retried = true;
                goto restart;
            }
            plat_error_handler(err);
        }

        bl2_node_info = bl2_node_info->next_load_info;
    }

    idsm_log_event(IDSM_EVENT_BL2_BOOT_OK, 1U);

    bl2_to_next_bl_params = plat_get_next_bl_params();
    assert(bl2_to_next_bl_params != NULL);
    assert(bl2_to_next_bl_params->head != NULL);
    assert(bl2_to_next_bl_params->h.type == PARAM_BL_PARAMS);
    assert(bl2_to_next_bl_params->h.version >= VERSION_2);
    assert(bl2_to_next_bl_params->head->ep_info != NULL);

    if (bl2_to_next_bl_params->head->ep_info->args.arg0 == (u_register_t)0)
        bl2_to_next_bl_params->head->ep_info->args.arg0 =
            (u_register_t)bl2_to_next_bl_params;

    plat_flush_next_bl_params();
    return bl2_to_next_bl_params->head->ep_info;
}
