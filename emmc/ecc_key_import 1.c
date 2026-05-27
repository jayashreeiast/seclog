#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "libhse.h"
#include "hse_interface.h"

#define ERROR(fmt, ...) printf("[ERROR] " fmt, ##__VA_ARGS__)
#define INFO(fmt, ...)  printf("[INFO] " fmt, ##__VA_ARGS__)

/* * TARGET HANDLE: The Root Key Handle (ECC) 
 * Based on your previous logs: 0x00010401
 */
#define ECC_ROOT_PUB_HANDLE GET_KEY_HANDLE(HSE_KEY_CATALOG_ID_NVM, 4, 1)

/* * ECC P-256 Constants 
 * Size = 32 bytes (X) + 32 bytes (Y) = 64 bytes total
 */
#define ECC_PUB_KEY_SIZE     64 

int main(void)
{
    // Zero out descriptors to avoid garbage data
    hseSrvDescriptor_t srv_desc = {0};
    hseImportKeySrv_t import_key_req = {0};

    // ======================================================================
    // 1. KEY DATA (X and Y Coordinates Concatenated)
    // ======================================================================
    // You must paste the X and Y bytes of your Root CA Public Key here.
    // Do NOT include the '04' header byte if your cert has it. Just X + Y.
    uint8_t ecc_pub_key[ECC_PUB_KEY_SIZE] = {
    0x5a, 0x42, 0x1e, 0xaf, 0x86, 0x45, 0xd1, 0x48, 0xf4, 0xb4, 0x15, 0x41, 0xae, 0xa3, 0x94, 0xb3, 0x63, 
    0xd8, 0x05,0xaa, 0x9d, 0x3e, 0xb5, 0x24, 0x56, 0x77, 0x41, 0xac, 0x8e, 0x93, 0x6a, 0x1b, 0xa9, 0xc6, 
    0xf9, 0x3a, 0x0e, 0xd7, 0x16, 0x32, 0x87, 0x03, 0xb3, 0x09, 0x31, 0x35, 0xb1, 0xe9, 0xed, 0x0a, 0xbc, 
    0x37, 0x20, 0xb4, 0x9a, 0x7c, 0x85, 0xe3, 0xad, 0xef, 0xb0, 0xf5, 0xe6, 0x6d
    }; 

    void *key_data_dma = NULL;
    void *key_info_dma = NULL;
    hseKeyInfo_t *key_info_ptr;
    int err;
    uint32_t status; // Note: hse_check_status returns uint32_t usually

    // Open device
    err = hse_dev_open();
    if (err) {
        ERROR("Failed to open HSE device: %d\n", err);
        return err;
    }

    status = hse_check_status();
    if (!(status & HSE_STATUS_INSTALL_OK)) {
        ERROR("HSE not installed/ready!\n");
        hse_dev_close();
        return -ENODEV;
    }

    // ======================================================================
    // 2. MEMORY ALLOCATION
    // ======================================================================
    
    // Allocate DMA for Key Data
    key_data_dma = hse_mem_alloc(ECC_PUB_KEY_SIZE);
    if (!key_data_dma) {
        ERROR("DMA alloc failed for key data\n");
        hse_dev_close();
        return -ENOMEM;
    }
    // Copy the C-array to the DMA buffer
    hse_memcpy(key_data_dma, ecc_pub_key, ECC_PUB_KEY_SIZE);

    // Allocate DMA for Key Info
    key_info_dma = hse_mem_alloc(sizeof(hseKeyInfo_t));
    if (!key_info_dma) {
        ERROR("DMA alloc failed for key info\n");
        hse_mem_free(key_data_dma);
        hse_dev_close();
        return -ENOMEM;
    }
    hse_memset(key_info_dma, 0, sizeof(hseKeyInfo_t));

    // ======================================================================
    // 3. KEY INFO CONFIGURATION (CRITICAL STEP)
    // ======================================================================
    key_info_ptr = (hseKeyInfo_t *)key_info_dma;

    // A. Key Type: ECC Public
    key_info_ptr->keyType = HSE_KEY_TYPE_ECC_PUB; 
    // B. Key Length: 256 bits (for NIST P-256)
    key_info_ptr->keyBitLen = 256; 
    // C. Flags: VERIFY ONLY (The Fix!)
    // Do NOT add HSE_KF_USAGE_KEY_PROVISION or HSE_KF_USAGE_AUTHORIZATION
    key_info_ptr->keyFlags = HSE_KF_USAGE_VERIFY; 
    // D. Specific Curve ID (NIST P-256)
    // Note: Structure path might vary slightly based on HSE header version.
    // Usually it is inside a union 'specific'.
    key_info_ptr->specific.eccCurveId = HSE_EC_SEC_SECP256R1; 
    key_info_ptr->smrFlags  = 0;
 

    INFO("Configured Key Info: Type=ECC_PUB, Curve=P-256, Flags=VERIFY\n");

    // ======================================================================
    // 4. SERVICE DESCRIPTOR
    // ======================================================================
    srv_desc.srvId = HSE_SRV_ID_IMPORT_KEY;
    
    import_key_req.targetKeyHandle = ECC_ROOT_PUB_HANDLE;
    import_key_req.pKeyInfo        = hse_virt_to_dma(key_info_dma);
    
    // CRITICAL CHANGE: Public Keys go in pKey[0], not pKey[2]!
    import_key_req.pKey[0]    = hse_virt_to_dma(key_data_dma);
    import_key_req.keyLen[0]  = ECC_PUB_KEY_SIZE;

    // Unused fields for Public Key Import
    import_key_req.cipher.cipherKeyHandle = HSE_INVALID_KEY_HANDLE;
    import_key_req.keyContainer.authKeyHandle = HSE_INVALID_KEY_HANDLE;

    srv_desc.hseSrv.importKeyReq = import_key_req;

    // ======================================================================
    // 5. SEND REQUEST
    // ======================================================================
    INFO("Importing ECC Root Key into handle 0x%08X...\n", ECC_ROOT_PUB_HANDLE);

    err = hse_srv_req_sync(HSE_CHANNEL_ANY, &srv_desc, sizeof(srv_desc));

    if (err) {
        ERROR("Key Import FAILED. Error: 0x%x\n", err);
    } else {
        INFO("Key Import SUCCESS! Root Key flags are now fixed.\n");
    }

    // Cleanup
    hse_mem_free(key_data_dma);
    hse_mem_free(key_info_dma);
    hse_dev_close();

    return err;
}