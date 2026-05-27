/*
 * Copyright (c) 2016-2022, ARM Limited and Contributors.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * bl2_image_load_idsm.c
 *
 * AUTOSAR IdsM-compliant secure event logging at BL2/ATF level.
 * NXP S32G399A — non-AUTOSAR Linux A53 core.
 *
 * FRAME COMPLIANCE (see idsm_event.h header for full spec table):
 *   Fixed Header     : 8 bytes  — Version(4b) | Header(4b) | Instance(10b)
 *                                 | Module(6b) | EventId(16b) | Count(16b)
 *                                 | Reserve(8b)
 *   Timestamp Frame  : 8 bytes  — type_flag(1B) | ms_value(7B big-endian)
 *   Signature Frame  : 34 bytes — SigLen(2B big-endian=32) | HMAC-SHA256(32B)
 *     HMAC covers: fixed_hdr(8) + enc_timestamp(8) = 16 bytes
 *
 * CRYPTO (HSE_SECBOOT=1):
 *   AES-256-GCM  encrypt the 8-byte Timestamp (AAD = fixed_hdr 8B)
 *   HMAC-SHA256  sign    fixed_hdr(8B) + enc_timestamp(8B)
 *   Both use drivers/nxp/s32/hse/ — same API as s32_crypto_lib.c:
 *     hse_driver_init / hse_mem_alloc / hse_memcpy /
 *     hse_virt_to_phys / hse_srv_req_sync(HSE_CHANNEL_CRYPTO, &desc)
 *
 * FALLBACK (HSE not built or not ready):
 *   Writes plaintext event + timestamp (original 2-sector layout)
 *   so boot is never blocked.
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

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

#include "idsm_event.h"

/* ================================================================
 * MODULE STATE
 * ================================================================ */
static uint32_t s_log_index = 0;   /* monotonic sequence / slot counter */
static int      s_hse_ready = 0;   /* 1 after successful hse_driver_init */

/* ================================================================
 * TIMER — ARM generic timer
 * ================================================================ */

/*
 * get_timestamp_ms() — returns 56-bit ms value split across 7 bytes.
 * We use only the lower 56 bits of (cnt * 1000 / freq).
 * Caller provides a 7-byte big-endian output buffer.
 */
static void get_timestamp_ms(uint8_t out_ms[7])
{
    uint64_t cnt  = read_cntpct_el0();
    uint64_t freq = read_cntfrq_el0();

    NOTICE("IDSM_TIMER: cnt=%lu freq=%lu\n", cnt, freq);

    if (freq == 0U)
        freq = 5000000ULL;   /* 5 MHz fallback */

    uint64_t ms = (cnt * 1000ULL) / freq;

    /* Write big-endian into 7 bytes (56-bit) */
    for (int i = 6; i >= 0; i--) {
        out_ms[i] = (uint8_t)(ms & 0xFFU);
        ms >>= 8;
    }
}

/* ================================================================
 * FIXED HEADER BUILDER
 *
 * Packs the spec-defined bit fields into the 8-byte wire format:
 *
 *   Byte 0  [7:4] Protocol Version  [3:0] Protocol Header flags
 *   Byte 1  Instance Id [9:2]
 *   Byte 2  [7:6] Instance Id [1:0]  [5:0] Module Id
 *   Byte 3  Event Id [15:8]
 *   Byte 4  Event Id [7:0]
 *   Byte 5  Count [15:8]
 *   Byte 6  Count [7:0]
 *   Byte 7  Reserve = 0x00
 * ================================================================ */
static void build_fixed_hdr(idsm_fixed_hdr_t   *hdr,
                             uint16_t            instance_id,   /* 10-bit */
                             uint8_t             module_id,     /*  6-bit */
                             uint8_t             hdr_flags,     /*  4-bit */
                             uint16_t            event_id,
                             uint16_t            count)
{
    hdr->ver_hdr     = (uint8_t)(((IDSM_PROTOCOL_VERSION & 0xFU) << 4) |
                                  (hdr_flags & 0xFU));

    hdr->inst_hi     = (uint8_t)((instance_id >> 2) & 0xFFU);
    hdr->inst_lo_mod = (uint8_t)(((instance_id & 0x3U) << 6) |
                                  (module_id   & 0x3FU));

    hdr->event_hi    = (uint8_t)((event_id >> 8) & 0xFFU);
    hdr->event_lo    = (uint8_t)( event_id        & 0xFFU);

    hdr->count_hi    = (uint8_t)((count >> 8) & 0xFFU);
    hdr->count_lo    = (uint8_t)( count        & 0xFFU);

    hdr->reserve     = 0x00U;
}

/* ================================================================
 * TIMESTAMP FRAME BUILDER
 *
 *   Byte[0]  Bit[7] = type (IDSM_TS_AUTOSAR / IDSM_TS_OEM)
 *            Bit[6:0] = reserved (0) for AUTOSAR
 *   Bytes[1..7]  56-bit ms value, big-endian
 * ================================================================ */
static void build_timestamp_frame(idsm_timestamp_frame_t *ts, uint8_t type_flag)
{
    memset(ts, 0, sizeof(*ts));
    ts->type_and_rsvd = type_flag & 0x80U;   /* only Bit[7] is meaningful */
    get_timestamp_ms(ts->ts_ms);
}

/* ================================================================
 * SIGNATURE FRAME BUILDER (spec layout, sig_data zeroed initially)
 *
 *   Bytes[0..1]  Signature Length (big-endian) = IDSM_HMAC_SHA256_BYTES
 *   Bytes[2..33] Signature data   (32-byte HMAC, filled later)
 * ================================================================ */
static void init_signature_frame(idsm_signature_frame_t *sf)
{
    uint16_t sig_len = (uint16_t)IDSM_HMAC_SHA256_BYTES;

    sf->sig_len_hi = (uint8_t)((sig_len >> 8) & 0xFFU);
    sf->sig_len_lo = (uint8_t)( sig_len        & 0xFFU);
    memset(sf->sig_data, 0, IDSM_HMAC_SHA256_BYTES);
}

/* ================================================================
 * IV BUILDER
 *
 * 4-byte device prefix | 4-byte zero pad | 4-byte big-endian seq.
 * Replace prefix with OTP-burned device nonce for production.
 * ================================================================ */
static void build_gcm_iv(uint8_t iv[IDSM_GCM_IV_BYTES], uint32_t seq)
{
    static const uint8_t dev_prefix[4] = { 0xA5, 0x5A, 0xC3, 0x3C };

    memcpy(iv, dev_prefix, 4U);
    memset(iv + 4, 0, 4U);
    iv[8]  = (uint8_t)((seq >> 24) & 0xFFU);
    iv[9]  = (uint8_t)((seq >> 16) & 0xFFU);
    iv[10] = (uint8_t)((seq >>  8) & 0xFFU);
    iv[11] = (uint8_t)( seq        & 0xFFU);
}

/* ================================================================
 * DUMP HELPERS  (compile out for non-DEBUG builds)
 * ================================================================ 
#ifdef DEBUG
static void dump_fixed_hdr(const idsm_fixed_hdr_t *h)
{
    uint16_t instance = (uint16_t)(((uint16_t)h->inst_hi << 2) |
                                    ((h->inst_lo_mod >> 6) & 0x3U));
    uint8_t  module   = h->inst_lo_mod & 0x3FU;
    uint16_t event    = ((uint16_t)h->event_hi << 8) | h->event_lo;
    uint16_t count    = ((uint16_t)h->count_hi << 8) | h->count_lo;

    NOTICE("IDSM HDR: ver=%u flags=0x%x inst=%u mod=%u "
           "event=0x%04X count=%u\n",
           (h->ver_hdr >> 4), (h->ver_hdr & 0xF),
           instance, module, event, count);
}

static void dump_sig_frame(const idsm_signature_frame_t *sf)
{
    uint16_t len = ((uint16_t)sf->sig_len_hi << 8) | sf->sig_len_lo;
    NOTICE("IDSM SIG LEN: %u  DATA: ", len);
    for (int i = 0; i < 16; i++) NOTICE("%02X", sf->sig_data[i]);
    NOTICE("...\n");
}
#else
# define dump_fixed_hdr(h)   ((void)(h))
# define dump_sig_frame(sf)  ((void)(sf))
#endif
*/
/* ================================================================
 * HSE INITIALISATION  (mirrors s32_crypto_lib.c init())
 * ================================================================ */
static void idsm_hse_init(void)
{
//#ifdef HSE_SECBOOT
    int ret;

    if (s_hse_ready)
        return;

    ret = hse_driver_init();
    if (ret != 0) {
        ERROR("IDSM: hse_driver_init failed (%d) "
              "— plaintext fallback active\n", ret);
        return;
    }

    s_hse_ready = 1;
    NOTICE("IDSM: HSE driver ready\n");
//#else
    NOTICE("IDSM: HSE_SECBOOT not set — plaintext fallback active\n");
//#endif
}

/* ================================================================
 * HSE AES-256-GCM  — encrypt the 8-byte Timestamp frame.
 *
 * AAD  = fixed_hdr (8 bytes)  — authenticated but not encrypted,
 *        so the fixed header (EventId, flags …) is tamper-evident
 *        even without decryption.
 *
 * Pattern mirrors s32_crypto_lib.c:
 *   alloc → memcpy → virt_to_phys → fill desc → hse_srv_req_sync
 * ================================================================ */
//#ifdef HSE_SECBOOT
static int idsm_encrypt_timestamp(
        const idsm_timestamp_frame_t  *plain_ts,
        const idsm_fixed_hdr_t        *aad_hdr,
        const uint8_t                  iv[IDSM_GCM_IV_BYTES],
        idsm_timestamp_frame_t        *out_enc_ts,
        uint8_t                        out_tag[IDSM_GCM_TAG_BYTES])
{
    hseSrvDescriptor_t  desc     = {0};
    hseAeadSrv_t       *aead     = &desc.hseSrv.aeadReq;

    void    *iv_buf   = NULL,  *aad_buf  = NULL;
    void    *in_buf   = NULL,  *out_buf  = NULL;
    void    *tag_buf  = NULL,  *tlen_buf = NULL;
    uint32_t tag_len  = (uint32_t)IDSM_GCM_TAG_BYTES;
    uint32_t data_len = (uint32_t)IDSM_TIMESTAMP_BYTES;
    uint32_t aad_len  = (uint32_t)IDSM_FIXED_HDR_BYTES;
    int      ret      = -ENOMEM;

    iv_buf   = hse_mem_alloc(IDSM_GCM_IV_BYTES);
    aad_buf  = hse_mem_alloc(aad_len);
    in_buf   = hse_mem_alloc(data_len);
    out_buf  = hse_mem_alloc(data_len);
    tlen_buf = hse_mem_alloc(sizeof(uint32_t));
    tag_buf  = hse_mem_alloc(tag_len);

    if (!iv_buf || !aad_buf || !in_buf ||
        !out_buf || !tlen_buf || !tag_buf) {
        ERROR("IDSM: GCM alloc OOM\n");
        goto cleanup;
    }

    hse_memcpy(iv_buf,   iv,       IDSM_GCM_IV_BYTES);
    hse_memcpy(aad_buf,  aad_hdr,  aad_len);
    hse_memcpy(in_buf,   plain_ts, data_len);
    hse_memcpy(tlen_buf, &tag_len, sizeof(uint32_t));

    desc.srvId              = HSE_SRV_ID_AEAD;
    aead->accessMode        = HSE_ACCESS_MODE_ONE_PASS;
    aead->authCipherMode    = HSE_AUTH_CIPHER_MODE_GCM;
    aead->cipherDir         = HSE_CIPHER_DIR_ENCRYPT;
    aead->keyHandle         = IDSM_AES_KEY_HANDLE;
    aead->ivLength          = (uint32_t)IDSM_GCM_IV_BYTES;
    aead->pIV               = hse_virt_to_phys(iv_buf);
    aead->aadLength         = aad_len;
    aead->pAAD              = hse_virt_to_phys(aad_buf);
    aead->inputLength       = data_len;
    aead->pInput            = hse_virt_to_phys(in_buf);
    aead->pOutput           = hse_virt_to_phys(out_buf);
    aead->tagLength         = tag_len;
    //aead->pTagLength        = hse_virt_to_phys(tlen_buf);
    aead->pTag              = hse_virt_to_phys(tag_buf);

    ret = hse_srv_req_sync(HSE_CHANNEL_CRYPTO, &desc);
    if (ret != 0) {
        ERROR("IDSM: GCM encrypt failed (%d)\n", ret);
        goto cleanup;
    }

    hse_memcpy(out_enc_ts, out_buf,  data_len);
    hse_memcpy(out_tag,    tag_buf,  tag_len);

cleanup:
    if (tag_buf)  hse_mem_free(tag_buf);
    if (tlen_buf) hse_mem_free(tlen_buf);
    if (out_buf)  hse_mem_free(out_buf);
    if (in_buf)   hse_mem_free(in_buf);
    if (aad_buf)  hse_mem_free(aad_buf);
    if (iv_buf)   hse_mem_free(iv_buf);
    return ret;
}
//#endif /* HSE_SECBOOT */

/* ================================================================
 * HSE HMAC-SHA256 — sign fixed_hdr + enc_timestamp.
 *
 * AUTOSAR spec: "Signature calculated with EventFrame +
 *   Optional Timestamp + Optional Context Data"
 * We sign: fixed_hdr(8B) + enc_timestamp(8B) = 16 bytes.
 *
 * Pattern is identical to hash/sign calls in s32_crypto_lib.c.
 * ================================================================ */
//#ifdef HSE_SECBOOT
static int idsm_hmac_sign(
        const idsm_fixed_hdr_t       *hdr,
        const idsm_timestamp_frame_t *enc_ts,
        uint8_t                       out_tag[IDSM_HMAC_SHA256_BYTES])
{
    hseSrvDescriptor_t  desc    = {0};
    hseMacSrv_t        *mac_req = &desc.hseSrv.macReq;

    /*
     * Build contiguous input buffer: [fixed_hdr(8) | enc_timestamp(8)]
     * The AUTOSAR Signature Frame covers everything before it on the wire.
     */
    uint8_t  sign_input[IDSM_FIXED_HDR_BYTES + IDSM_TIMESTAMP_BYTES];
    uint32_t sign_len = (uint32_t)sizeof(sign_input);
    uint32_t tag_len  = (uint32_t)IDSM_HMAC_SHA256_BYTES;

    memcpy(sign_input,                       hdr,    IDSM_FIXED_HDR_BYTES);
    memcpy(sign_input + IDSM_FIXED_HDR_BYTES, enc_ts, IDSM_TIMESTAMP_BYTES);

    void *in_buf      = hse_mem_alloc(sign_len);
    void *tlen_buf    = hse_mem_alloc(sizeof(uint32_t));
    void *tag_buf     = hse_mem_alloc(tag_len);
    int   ret         = -ENOMEM;

    if (!in_buf || !tlen_buf || !tag_buf) {
        ERROR("IDSM: HMAC alloc OOM\n");
        goto cleanup;
    }

    hse_memcpy(in_buf,   sign_input, sign_len);
    hse_memcpy(tlen_buf, &tag_len,   sizeof(uint32_t));

    desc.srvId                               = HSE_SRV_ID_MAC;
    mac_req->accessMode                      = HSE_ACCESS_MODE_ONE_PASS;
    mac_req->macScheme.macAlgo               = HSE_MAC_ALGO_HMAC;
    mac_req->macScheme.sch.hmac.hashAlgo     = HSE_HASH_ALGO_SHA2_256;
    mac_req->keyHandle                       = IDSM_HMAC_KEY_HANDLE;
    mac_req->sgtOption                       = HSE_SGT_OPTION_NONE;
    mac_req->inputLength                     = sign_len;
    mac_req->pInput                          = hse_virt_to_phys(in_buf);
    //mac_req->pTagLength                      = hse_virt_to_phys(tlen_buf);
    mac_req->pTag                            = hse_virt_to_phys(tag_buf);

    ret = hse_srv_req_sync(HSE_CHANNEL_CRYPTO, &desc);
    if (ret != 0) {
        ERROR("IDSM: HMAC sign failed (%d)\n", ret);
        goto cleanup;
    }

    hse_memcpy(out_tag, tag_buf, tag_len);

cleanup:
    if (tag_buf)  hse_mem_free(tag_buf);
    if (tlen_buf) hse_mem_free(tlen_buf);
    if (in_buf)   hse_mem_free(in_buf);
    return ret;
}
//#endif /* HSE_SECBOOT */

/* ================================================================
 * SECURE WRITE PATH
 *
 * Builds a spec-compliant idsm_secure_record_t (512 bytes):
 *
 *   1. Fixed header  → HDR flags = TS | SIG (0x06)
 *   2. GCM IV        → counter-based nonce
 *   3. AES-256-GCM   → encrypt Timestamp, produce GCM tag
 *   4. HMAC-SHA256   → sign hdr+enc_ts, write into sig_frame
 *   5. mmc_write_blocks (1 sector)
 * ================================================================ */
//#ifdef HSE_SECBOOT
static void idsm_write_secure(uint16_t event_id, uint16_t count,
                               uint32_t sector)
{
    /* Static to avoid BL2 stack overflow — aligned for cache ops */
    static idsm_secure_record_t    s_rec  __attribute__((aligned(64)));
    idsm_timestamp_frame_t         plain_ts;
    int ret;

    memset(&s_rec, 0, sizeof(s_rec));

    /* ---- 1. Fixed header (HDR_TS_AND_SIG = Bit[1]|Bit[2] = 0x06) ---- */
    build_fixed_hdr(&s_rec.fixed_hdr,
                    (uint16_t)IDSM_INSTANCE_A_CORE,
                    (uint8_t) IDSM_MODULE_SECBOOT,
                    IDSM_HDR_TS_AND_SIG,
                    event_id,
                    count);
    //dump_fixed_hdr(&s_rec.fixed_hdr);

    /* ---- 2. GCM IV ---- */
    build_gcm_iv(s_rec.gcm_iv, s_log_index);

    /* ---- 3. Build plaintext timestamp then AES-GCM encrypt it ---- */
    build_timestamp_frame(&plain_ts, IDSM_TS_AUTOSAR);

    ret = idsm_encrypt_timestamp(
            &plain_ts,
            &s_rec.fixed_hdr,   /* AAD */
            s_rec.gcm_iv,
            &s_rec.enc_timestamp,
            s_rec.gcm_tag);

    if (ret != 0) {
        ERROR("IDSM: GCM failed (event=0x%x) — plaintext fallback\n",
              event_id);
        /* graceful degradation — log at original offsets */
        uint32_t ev_sec = sector;
        uint32_t ts_sec = sector + 1U;

        /* write raw event block */
        uint8_t blk[IDSM_BLOCK_SIZE] = {0};
        memcpy(blk, &s_rec.fixed_hdr, IDSM_FIXED_HDR_BYTES);
        mmc_write_blocks(ev_sec, (uintptr_t)blk, 512);

        /* write raw timestamp block */
        memset(blk, 0, IDSM_BLOCK_SIZE);
        memcpy(blk, &plain_ts, IDSM_TIMESTAMP_BYTES);
        mmc_write_blocks(ts_sec, (uintptr_t)blk, 512);
        return;
    }

    NOTICE("IDSM: AES-GCM OK\n");

    /* ---- 4. Init signature frame layout, then HMAC-sign ---- */
    init_signature_frame(&s_rec.sig_frame);

    ret = idsm_hmac_sign(&s_rec.fixed_hdr,
                          &s_rec.enc_timestamp,
                          s_rec.sig_frame.sig_data);
    if (ret != 0) {
        ERROR("IDSM: HMAC failed (event=0x%x) — skipping write\n", event_id);
        return;
    }

    NOTICE("IDSM: HMAC-SHA256 OK\n");
    //dump_sig_frame(&s_rec.sig_frame);

    /* ---- 5. Flush + write ---- */
    flush_dcache_range((uintptr_t)&s_rec, sizeof(s_rec));

    ret = mmc_write_blocks(sector, (uintptr_t)&s_rec, 512);
    if (ret != 512) {
        ERROR("IDSM: mmc_write_blocks failed ret=%d sector=%u\n",
              ret, sector);
        return;
    }

    NOTICE("IDSM: secured record written sector=%u seq=%u event=0x%04X\n",
           sector, s_log_index, event_id);
}
//#endif /* HSE_SECBOOT */

/* ================================================================
 * PLAINTEXT FALLBACK WRITE
 *
 * Original 2-sector layout preserved for compatibility during
 * HSE bring-up or when HSE_SECBOOT is not enabled.
 *
 * Sector N+0: fixed_hdr (8 bytes at start of 512-byte block)
 * Sector N+1: plain timestamp frame (8 bytes at start of block)
 * ================================================================ */
static void idsm_write_plain(uint16_t event_id, uint16_t count,
                              uint32_t base_sector)
{
    idsm_fixed_hdr_t       hdr;
    idsm_timestamp_frame_t ts;
    uint8_t                blk[IDSM_BLOCK_SIZE];
    int                    ret;

    build_fixed_hdr(&hdr,
                    (uint16_t)IDSM_INSTANCE_A_CORE,
                    (uint8_t) IDSM_MODULE_SECBOOT,
                    IDSM_HDR_TS_ONLY,
                    event_id,
                    count);

    build_timestamp_frame(&ts, IDSM_TS_AUTOSAR);

    /* Event sector */
    memset(blk, 0, IDSM_BLOCK_SIZE);
    memcpy(blk, &hdr, IDSM_FIXED_HDR_BYTES);
    ret = mmc_write_blocks(base_sector, (uintptr_t)blk, 512);
    if (ret != 512) {
        ERROR("IDSM_PT: event write FAILED sector=%u ret=%d\n",
              base_sector, ret);
        return;
    }

    /* Timestamp sector */
    memset(blk, 0, IDSM_BLOCK_SIZE);
    memcpy(blk, &ts, IDSM_TIMESTAMP_BYTES);
    ret = mmc_write_blocks(base_sector + 1U, (uintptr_t)blk, 512);
    if (ret != 512) {
        ERROR("IDSM_PT: ts write FAILED sector=%u ret=%d\n",
              base_sector + 1U, ret);
        return;
    }

    NOTICE("IDSM_PT: plain event=0x%04X written sector=%u\n",
           event_id, base_sector);
}

/* ================================================================
 * PUBLIC API — idsm_log_event()
 * ================================================================ */
void idsm_log_event(uint16_t event_id, uint16_t count)
{
    uint32_t slot;
    uint32_t base_sector;

//#ifdef HSE_SECBOOT
    /* Secure path: 1 sector per entry */
    slot        = s_log_index % IDSM_LOG_MAX_ENTRIES;
    base_sector = IDSM_LOG_START_SECTOR + (slot * IDSM_SECTORS_SECURE);

    NOTICE("IDSM_LOG: event=0x%04X count=%u slot=%u sector=%u hse=%d\n",
           event_id, count, slot, base_sector, s_hse_ready);

    if (s_hse_ready) {
        idsm_write_secure(event_id, count, base_sector);
    } else {
        /* HSE not ready yet — plain uses 2 sectors, separate range */
        slot        = s_log_index % IDSM_LOG_MAX_ENTRIES;
        base_sector = IDSM_LOG_START_SECTOR +
                      (IDSM_LOG_MAX_ENTRIES * IDSM_SECTORS_SECURE) +
                      (slot * IDSM_SECTORS_PLAIN);
        idsm_write_plain(event_id, count, base_sector);
    }
//#else
    /* Plain path: 2 sectors per entry */
    slot        = s_log_index % IDSM_LOG_MAX_ENTRIES;
    base_sector = IDSM_LOG_START_SECTOR + (slot * IDSM_SECTORS_PLAIN);

    NOTICE("IDSM_LOG: event=0x%04X count=%u slot=%u sector=%u [plaintext]\n",
           event_id, count, slot, base_sector);

    idsm_write_plain(event_id, count, base_sector);
//#endif

    s_log_index++;
}

/*******************************************************************************
 * BL2 LOAD IMAGES
 ******************************************************************************/
struct entry_point_info *bl2_load_images(void)
{
    bl_params_t               *bl2_to_next_bl_params;
    bl_load_info_t            *bl2_load_info;
    const bl_load_info_node_t *bl2_node_info;
    int   plat_setup_done = 0;
    int   err;
    bool  retried = false;

    /* ================================================================
     * HSE init — must run before first idsm_log_event().
     * Same placement as s32_crypto_lib.c:  init() called once
     * at the start of the first crypto operation.
     * ================================================================ */
    idsm_hse_init();

    /* >>>>>> LOG: BL2 entry <<<<< */
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

        /* ---- PRE LOAD ---- */
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

        /* ---- LOAD IMAGE ---- */
        if ((bl2_node_info->image_info->h.attr &
             IMAGE_ATTRIB_SKIP_LOADING) == 0U) {
            INFO("BL2: Loading image id %u\n", bl2_node_info->image_id);
            err = load_auth_image(bl2_node_info->image_id,
                                  bl2_node_info->image_info);
            if (err != 0) {
                ERROR("BL2: load_auth_image fail id=%u err=%d\n",
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

        /* ---- POST LOAD ---- */
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

    /* >>>>>> LOG: all images loaded <<<<< */
    idsm_log_event(IDSM_EVENT_BL2_BOOT_OK, 1U);

    /* ---- NEXT BL PARAMS ---- */
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
