/**
*   @file    hse_srv_dice.h
*
*   @brief   Open Profile for DICE.
*   @details This file contains the services for the Open-DICE Profile
*
*
*   @addtogroup hse_srv_dice HSE DICE Service
*   @ingroup class_key_management
*   @{
*/

/*==================================================================================================
*
*   Copyright 2019 - 2023 NXP.
*
*   This software is owned or controlled by NXP and may only be used strictly in accordance with
*   the applicable license terms. By expressly accepting such terms or by downloading, installing,
*   activating and/or otherwise using the software, you are agreeing that you have read, and that
*   you agree to comply with and are bound by, such license terms. If you do not agree to
*   be bound by the applicable license terms, then you may not retain, install, activate or
*   otherwise use the software.
==================================================================================================*/
/*==================================================================================================
==================================================================================================*/

#ifndef HSE_SRV_DICE_H
#define HSE_SRV_DICE_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                        INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
==================================================================================================*/

#include "hse_common_types.h"

#define HSE_START_PRAGMA_PACK
#include "hse_compiler_abs.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/

/*==================================================================================================
*                                     FILE VERSION CHECKS
==================================================================================================*/

/*==================================================================================================
*                                          CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
*                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
#ifdef HSE_SPT_DICE
/** @brief   Dice extension field that can be include in the CBOR certificate.
             The CBOR label for this field is defined by the application.
 *  */
typedef struct
{
    /** @brief   The UID value */
    uint8_t                  uid[8U];
    /** @brief   The firmware version */
    hseAttrFwVersion_t       fwVersion;
    /** @brief   The hash of the SYS-IMG */
    uint8_t                  sysImgHash[32U];
    /** @brief   The first 16 bytes of SHA2_224(ADKP)*/
    uint8_t                  hashADKP[16U];
    /** @brief   The hash of IVT: SHA2_256(IVT) */
    uint8_t                  hashIvt[32U];
    /** @brief   The SMR and Core reset status (refer to #hseAttrSmrCoreStatus_t) */
    hseAttrSmrCoreStatus_t   smrCoreResetStatus;
    /** @brief   The firmware partition used during booting: Primary(0x01) or Backup (0x02) (refer to #hseAttrFwPartition_t) */
    hseAttrFwPartition_t     fwPartition;
    /** @brief   The IVT/DCD/ST authentication: no authentication (0x00U), boot authenticated (0x01U); refer to #hseAttrConfigBootAuth_t*/
    hseAttrConfigBootAuth_t  bootAuth;
    /** @brief   The debug authentication more: Password (0x00U), Challenge-Response based (0x01U); refer to #hseAttrDebugAuthMode_t*/
    hseAttrDebugAuthMode_t   authMode;
    /** @brief   ADKP diversified with UID (ADKPm option) is used or not: No (0x00U), Yes (0x01U)  */
    uint8_t                  AdkpmBit;
    /** @brief   OTFAD context status (refer to #hseOtfadCtxStatus_t) */
    hseOtfadCtxStatus_t      otfadContextStatus;
    /** @brief   The OTP rollback counter for SYS-IMG */
    uint8_t                  otpSysImgRollbackCounter;
    /** @brief   The OTP rollback counter for firmware */
    uint8_t                  otpFwRollbackCounter;
    uint8_t                  reserved[2U];
}hseDiceExtensionField_t;


/** @brief    The options used to generate DICE UDS/CDI0/CDIn key pair.*/
typedef uint32_t hseDiceKeyPairGenOptions_t;
#define HSE_DICE_KP_GEN_UDS      ((hseDiceKeyPairGenOptions_t)0U)  /**< @brief Generate UDS key pair (UDS_PRIV, UDS_PUB) */
#define HSE_DICE_KP_GEN_CDI0     ((hseDiceKeyPairGenOptions_t)1U)  /**< @brief Generate CDIO key pair (CDI0_PRIV, CDI0_PUB) */
#define HSE_DICE_KP_GEN_CDIn     ((hseDiceKeyPairGenOptions_t)2U)  /**< @brief Generate CDIn key pair (CDIn_PRIV, CDIn_PUB) */
#define HSE_DICE_KP_GEN_LAST_CDI ((hseDiceKeyPairGenOptions_t)3U)  /**< @brief Generate last CDI key pair (CDIn_PRIV, CDIn_PUB). \
                                                                               The SW layer n can decide not to involve HSE for building the DICE chain for next SW layers (>n). \
                                                                               Refer to pPubKey parameter from #hseDiceGenerateKeyPairSrv_t  */

/**
* @brief          Generate DICE key pairs.
* @details        This service generates the UDS or CDIn key pairs  ((UDS_PRIV, UDS_PUB) or (CDIn_PRIV, CDIn_PUB))
* @note
*                 - HSE generates the NVM UDS key pair using an Unique Device Secret (UDS).
*                   The NVM UDS key pair is stored in SYS-IMG in the key slot specified in the #hseDiceAttr_t attribute (SYS-IMG must be published).
*                   Note that this is an one-time operation.
*                   If the NVM UDS key pair is already generated (SYS-IMG contains the UDS key pair), HSE returns the status HSE_SRV_RSP_KEY_UPDATE_ERROR.
*                 - The RAM CDI0 key pair is generated using the asset CDI0_attest which is processed by HSE using:
*                       - The Unique Device Secret (UDS) (also used to generate the UDS key pair)
*                       - The fingerprint over a SW image (SHA512(SMR)) that is one of the SMR specified in the secure boot flow of the HSE;
*                         this SMR is defined in the #hseDiceAttr_t attribute (during configuration).
*                       - A set of input parameters fully controlled by the HSE (configuration data, authority data, mode and hidden input)
*                 - The RAM CDIn key pairs (n > 0) are generated using the asset CDIn_attest which is processed by the HSE using:
*                       - The CDI[n-1]_attest of the previous SW layer
*                       - The fingerprint over a SW image (SHA512(App#n)) that is provided as input by the caller
*                       - A set of input parameters controlled by the application: SHA512(App#n), configuration data, mode, authority data.
*                 - The hidden input is not used (64 bytes are set to zero)
                  - When generating the CDIn keypair, this service shall be called from the SW layer n-1.
*
*               Forking the SW attestation flow:
*               - each SW layer can create two or more CDIn_m key pairs from the same CDI[n-1] attest value.
*                 For example, layer 0 can create two CDI key pairs (CDI1_1 and CDI1_2) from the CD0 attest value.
*                 For this reason, the intermediate attest values are stored in the HSE. HSE can store maximum 8 CDIn attest values (8 entries).
*                 If there is no need to generate additional CDIn_m key pairs from CDI[n-1] attest value,
*                 the application can free the CDI[n-1] attest entry when signing the last certificate (by calling the hseDiceGenSignCertSrv_t service with erasePrivKey = TRUE).
*                 E.g. the application can free CDI0 attest entry when signing CDI1_2 certificate.
*
* @return      HSE_SRV_RSP_NOT_ALLOWED - in case the CDI0 SMR is not installed
*              HSE_SRV_RSP_NOT_ENOUGH_SPACE - if there is no empty entry inside HSE
*/

typedef struct
{
    /** @brief   INPUT: The option used to generate DICE key pairs (UDS/CDI0/CDIn key pair). Refer to #hseDiceKeyPairGenOptions_t.
     *                 @note
     *                 - If keyPairGenOption == HSE_DICE_KP_GEN_UDS, the targetKeyHandleKeyPair, srcKeyHandleKeyPair,
     *                   pAppHash, inputSize, pConfig, mode and pAuthorityHash parameters are ignored.
     *                   In this case, the key slot provided in the DICE attribute #hseDiceAttr_t is used to generate the UDS key.
     *                   The MU on which the service is triggered must match the MU of the UDS key pair (see #hseDiceAttr_t).
     *                 - If keyPairGenOption == HSE_DICE_KP_GEN_CDI0, the srcKeyHandleKeyPair, pAppHash, inputSize, pConfig and pAuthorityHash parameters are ignored.
     *                 - If keyPairGenOption == HSE_DICE_KP_GEN_LAST_CDI, the pAppHash, inputSize, pConfig and pAuthorityHash parameters are ignored. The pPubKey is provided by the application.
     */
    hseDiceKeyPairGenOptions_t keyPairGenOption;

    /** @brief   INPUT: - Specifies the key slot where to generate the CDI0 or CDIn key pair.
     *                    The targeted key slot must be in RAM catalog and must be empty.
     *                  @note
     *                    If keyPairGenOption == HSE_DICE_KP_GEN_UDS, this parameter is ignored (refer to keyPairGenOption).
     */
    hseKeyHandle_t  targetKeyHandleKeyPair;

    /** @brief   INPUT: Specifies the handle of CDI[n-1] key pair which is generated by the previous SW layer.
     *                  @note
     *                  - When the SW layer n-1 calls the service to generate the CDIn ECC key pair, the MU on which the service is triggered
     *                    must match the MU of the srcKeyHandleKeyPair key slot (CDI[n-1]).
     *                  - If keyPairGenOption == HSE_DICE_KP_GEN_UDS or HSE_DICE_KP_GEN_CDI0, this parameter is ignored.
     */
    hseKeyHandle_t  srcKeyHandleKeyPair;

    /** @brief   INPUT: Pointer to codeHAsh: SHA512(App#n) (64bytes)
     *                  @note Ignored when generating the UDS, CDI0 and last CDI key pairs.
     */
    HOST_ADDR       pAppHash;

    /** @brief   INPUT: Points to the optional configurationDescriptor (64 bytes).
     *                  @note Ignored when generating the UDS, CDI0 and last CDI key pairs.
     */
    HOST_ADDR       pConfig;

    /** @brief   INPUT: Points to 64-byte authorityHash (SHA512 hash over the public key used to verify the App#n image).
     *                  @note Ignored when generating the UDS, CDI0 and last CDI key pairs.
     */
    HOST_ADDR       pAuthorityHash;

    /** @brief   INPUT: The mode value provided by the application (1 byte)
     *                  @note Ignored when generating the UDS, CDI0 and last CDI key pairs.
     */
    uint8_t         mode;

    uint8_t         reserved1[3U];

    /** @brief    Points to a RAM buffer which contains the public key value
     *            OUTPUT: keyPairGenOption != HSE_DICE_KP_GEN_LAST_CDI, the public key value of the created key pair is exported if the pPubKey != NULL
     *            INPUT/OUTPUT: If keyPairGenOption == HSE_DICE_KP_GEN_LAST_CDI, the an initial public key value is provided as input by the application.
     *                          The generated CDIn public key (see the note below) is provided in pPubKey overwritten the buffer (overwritten the initial  public key).
     *                   @note  If the HSE_DICE_KP_GEN_LAST_CDI option is used, the SW layer n can decide not to involve HSE for building the DICE chain for next SW layers (>n).
     *                          To achieve this, HSE generates the CDIn public key (CDIn_PUB) having as inputs the CDI[n-1]_priv (srcKeyHandleKeyPair) and
     *                          an initial public key provided by the application (pPubKey).
     *                          The CDIn public key is generated as follow:
     *                          1. P(x,y) = CDI[n-1]_priv * pPubKey (ECDH), where pPubKey is provided by the application
     *                          2. Generate CDIn_PUB according to FIPS 186-4 (section B.5.1):
     *                             - d = c mod(p-1) + 1, where:
     *                                - c - P.x-coordinate concatenated with 64 bits from P.y-coordinate (size of c = key bit length+64bits)
     *                                - p - order of the curve generator G
     *                             - CDIn_PUB = d ∙ G
     *                          The same operations should be done by App SW layer n+1 to compute the CDIn key pair: CDIn_PRIV (d), CDIn_PUB.
     *                          Once the CDIn key pair is computed, the DICE chain can be built from SW layer n, without involving HSE.
     *                    @note The srcKeyHandleKeyPair provided when generating the last CDIn keypair cannot be used anymore to generate new ECC key pairs
     *                          as the CDI[n-1] attestation gets erased upon successful last CDIn keypair generation.
     */
    HOST_ADDR       pPubKey;

    uint8_t         reserved2[16U];
}hseDiceGenerateKeyPairSrv_t;



/** @brief    The options used to generate DICE secrets (CDI0/CDIn secrets).*/
typedef uint32_t hseDiceSecretGenOptions_t;
#define HSE_DICE_SECRET_GEN_CDI0    ((hseDiceSecretGenOptions_t)0U)  /**< @brief Generate CDI0 secret */
#define HSE_DICE_SECRET_GEN_CDIn    ((hseDiceSecretGenOptions_t)1U)  /**< @brief Generate CDIn secret */


/**
* @brief          Generate DICE secrets.
* @details        This service generates the optional RAM CDIn secret keys and shall be called from the software layer n-1.
* @note
*                 -  The RAM CDI0 secret is generated using the asset CDI0_seal which is processed by HSE using:
*                       - The Unique Device Secret (UDS) (also used to generate the UDS key pair)
*                       - A set of parameters fully controlled by the HSE (authority, mode, hidden input)
*                 - The RAM CDIn secrets (n > 0) are generated using the asset CDIn_seal which is processed by HSE using:
*                       - The CDI[n-1]_seal of the previous SW layer
*                       - A set of input parameters controlled by the application: authority data and mode.
*                 - The hidden input is not used (64 bytes are set to zero).
*
*               Forking CDIn secrets:
*               - each SW layer can create two or more CDIn_m secret from the same CDI[n-1] seal value.
*                 For example, layer 0 can create two CDI secrets (CDI1_1 and CDI1_2) from CD0 seal value.
*                 For this reason, the intermediate seal values are stored in the HSE. HSE can store maximum 8 CDIn seal values (8 entries).
*                 If there is no need to generate additional CDIn_m secrets from CDI[n-1] seal value,
*                 the application can free the CDI[n-1] seal entry by setting the eraseSrcKeySeal parameter to TRUE.
*
* @return        HSE_SRV_RSP_NOT_ALLOWED - in case the CDI0 SMR is not installed
*                HSE_SRV_RSP_NOT_ENOUGH_SPACE - if there is no empty entry inside HSE
*/
typedef struct
{
    /** @brief   INPUT: The option used to generate DICE secrets (CDI0/CDIn secrets). Refer to #hseDiceSecretGenOptions_t.
     *                 @note
     *                 - If secretGenOption == HSE_DICE_SECRET_GEN_CDI0, the srcKeyHandleSecret, pAuthorityHash, mode and
     *                   eraseSrcKeySeal parameters are ignored. In this case, the MU on which the service is
     *                   triggered must match the MU of the UDS key pair (see #hseDiceAttr_t).
     */
    hseDiceSecretGenOptions_t secretGenOption;

    /** @brief   INPUT: Specifies the key information of targetKeyHandleSecret key:
     *                  keyType must be HSE_KEY_TYPE_AES, keyBitLen must be 256bits and it can not be a provision key (HSE_KF_USAGE_KEY_PROVISION flag is not set)*/
    hseKeyInfo_t    keyInfo;

    /** @brief   INPUT: Specifies the handle to the CDIn secret key to be generated (CDIn_Secret).
     *                  The targeted key slot must be in RAM catalog and must be empty.
     */
    hseKeyHandle_t  targetKeyHandleSecret;

    /** @brief   INPUT: It specifies the handle to the CDI[n-1] secret key generated by the previous SW layer
     *                 @note - When the SW layer n-1 calls the service to generate the CDIn secret key, the MU on which the service is triggered
     *                         must match the MU of the srcKeyHandleSecret key slot (CDI[n-1]).
     *                       - If secretGenOption == HSE_DICE_SECRET_GEN_CDI0, this parameter is ignored.
     */
    hseKeyHandle_t  srcKeyHandleSecret;

    /** @brief   INPUT: Specifies the pointer to authorityHash: SHA512 hash over the authority (64 bytes)
     *                  @note Ignored when generating CDI0_Secret.
     */
    HOST_ADDR       pAuthorityHash;

    /** @brief   INPUT: The mode value provided by the application (1 byte)
     *                  @note Ignored when generating CDI0_Secret
     */
    uint8_t         mode;

    /** @brief   INPUT: Erase or not the CDI[n-1] seal (TRUE or FALSE) of the previous SW layer after the key generation
     *                  (disables further CDI secrets creation).
     *           @note Ignored when generating CDI0_Secret.
     */
    bool_t          eraseSrcKeySeal;

    uint8_t         reserved[18U];
} hseDiceGenerateSecretSrv_t;


/** @brief    The options used to generate the CDI0_PUB certificate or sign the CDIn_PUB (n>0) certificates.*/
typedef uint32_t hseDiceCertOptions_t;
#define HSE_DICE_CERT_CDI0_PUB    ((hseDiceCertOptions_t)0U)  /**< @brief Generate the CDI0_PUB certificate */
#define HSE_DICE_CERT_CDIn_PUB    ((hseDiceCertOptions_t)1U)  /**< @brief Sign the CDIn_PUB (n>0) certificates */

/**
* @brief          DICE CBOR certificate signing.
* @details        This service can be used to:
*                   - fill the CBOR objects (fields) for codeHash, configurationDescriptor, authorityHash, mode and subjectPublicKey in the certificate and sign the CDI0_PUB certificate
*                   - fill the CBOR object for subjectPublicKey in the certificate and sign the CDIn_PUB (n>0) certificates
* @note
*                 -  Each CBOR object usually (with some exceptions) has the following format: 0x3a 0x00 || CBOR label (4bytes) || length || field value
*                 -  The certificate is provided by the caller without filling CBOR objects specified above. It can include other CBOR objects defined by the application.
*                    Based on the provided parameters (offsets), HSE fills these CBOR objects and sign the CBOR certificate using the private key (previously generated)
*                 -  The certificate format is specified by the DICE open source specification provided by pigweed
*                 -  If the SMR specified in #hseDiceAttr_t attribute must be previously installed, otherwise the service will return  HSE_SRV_RSP_NOT_ALLOWED
* @note
*                 - The payload of the certificate consists of a series of map items.
*                 - Each CBOR map item is being filled by the HSE from the specified offset.
*                 - If (certOption == HSE_DICE_CERT_CDI0_PUB):
*                      - offset[0]: where the CBOR map item containing the COSE Key object of the subjectPublicKey (label -4670552) is filled by HSE
*                                  The required size for the COSE Key object placeholder is:
*                                       87 bytes for ECDSA P-256 with SHA-256
*                                       88 bytes for ECDSA P-256 with SHA-384/SHA-512
*                                       119 bytes for ECDSA P-384 with SHA-256
*                                       120 bytes for ECDSA P-384 with SHA-384/SHA-512
*                                   The Key objects structure is described in the RFC9052 standard
*                                   The algorithm used can be one of ES256, ES384 or ES512 depending on the certSignHashAlgo from #hseDiceAttr_t
*                                   The algorithms are computed based on the CBOR Object Signing and Encryption values (COSE)
*                      - offset[1]: where the CBOR map item (71 bytes) of the codeHash (label -4670545) is filled by HSE:
                                    - if DICE SMR is verified: SHA512 of the SMR
                                    - if DICE SMR is not verified: all bytes are zeros
*                      - offset[2]: where the CBOR map item (71 bytes) of the configurationDescriptor (label -4670548 ) is filled by HSE:
                                    - if DICE SMR is verified: 0x55U, 0xAA, 0xAA, 0x55U, 0x00..0x00 (64bytes)
                                    - if DICE SMR is not verified: 0xFE, 0xCA, 0xAD, 0xAB, 0x00..0x00 (64bytes)
*                      - offset[3]: where the CBOR map item (71 bytes) of the authorityHash (label -4670549) is filled by HSE  (SHA512 of public key used to verify the SMR)
*                      - offset[4]: where the CBOR map item (7 bytes) of the mode (label -4670551) is filled by HSE:
                                    - if HSE_STATUS_HOST_DEBUGGER_ACTIVE == 1, mode = 2 (debug)
                                    - if HSE_STATUS_HOST_DEBUGGER_ACTIVE == 0:
                                        - mode = 0 (not configured), if LC = CUST_DEL/OEM_PROD/SIMULATED_OEM_PROD
                                        - mode = 1 (normal), if LC = IN_FIELD/SIMULATED_IN_FIELD
*                      - offset[5] (optional): where the #hseDiceExtensionField_t field is filled by HSE; if set to zero, this offset is not used.
*                                              @note    This option does not return a CBOR object. It is just filling the value inside the CBOR object.
*                                                       The application must format the CBOR map item (label/value pair) and allocate a CBOR label (e.g 0x3a || label (4bytes) || 0x58 || value size (1byte) || value )
*                 - If (certOption == HSE_DICE_CERT_CDIn_PUB):
*                        - offset[0]: where the the COSE Key object of the subjectPublicKey is filled by HSE  (87 bytes for ECC256 or 120 bytes for ECC384)
*                        - the remaining offsets are ignored
*
*                 - A CBOR map item is a pair of CBOR data items which consists from a label and it's associated value
* Below is a codeHash CBOR map item encoding example:
*
* |CBOR Label obj              |                CBOR Value obj                 |
* |---------------------------:|:---------------------------------------------:|
* | 0x3A  0x00 0x47 0x44 0x50  |  0x58 0x40 followed by 64 bytes for codeHash  |
*
* Where the initial byte meaning is:
* 0x3A - negative integer -1-n (four-byte uint32_t for n follows)
* 0x58 - byte string (one-byte uint8_t for n, and then n bytes follow)
* The CBOR initial byte table is described in the RFC8949.
* @return         HSE_SRV_RSP_NOT_ALLOWED - in case the CDI0 SMR is not installed
*                 HSE_SRV_RSP_KEY_NOT_AVAILABLE - in case the signing cannot be done due to the missing private key.
*/

typedef struct
{
    /** @brief   INPUT:  The options used to sign the CDI0_PUB certificate or the CDIn_PUB (n>0) certificates. Refer to #hseDiceSecretGenOptions_t.
     */
    hseDiceCertOptions_t certOption;

    /** @brief   INPUT: Specifies the handle to the key used to sign the CBOR certificate.
     *                  - If (certOption == HSE_DICE_CERT_CDI0_PUB), this parameter is ignored;  keyHandleNvmUDSKeyPair is used from #hseDiceAttr_t attribute.
     *                  @note
     *                  - The MU on which the service is sent must match the MU of the keyHandleKeyPair key (configured in the key catalog).
     */
    hseKeyHandle_t  keyHandleKeyPair;

    /** @brief   INPUT: Specifies the handle to the public key that must be filled in the CBOR certificate. */
    hseKeyHandle_t  keyHandleSubjectPublicKey;

    /** @brief   INPUT: The address of the CBOR encoded Sig_structure defined by the application and described in the RFC9052, Sigining and Verification Process Chapter:
     *                      Sig_structure = [
     *                        context : "Signature" / "Signature1",
     *                        body_protected : empty_or_serialized_map,
     *                        ? sign_protected : empty_or_serialized_map,
     *                        external_aad : bstr,
     *                        payload : bstr
     *                      ]
     *                  The application shall provide the header bytes of the encoded Sig_structure represented by the first bytes until the payload CBOR object starts.
     *                  The HSE copies internally the header of the CBOR encoded Sig_strucure and concatenates it with the pPayload buffer.
     *                  @note The application may use a dummy payload bstr field when performing CBOR encoding of the Sig_structure, as the payload of the Sig_structure is not considered by the HSE.
     */
    HOST_ADDR       pSigStructureHeader;

    /** @brief   INPUT: The size of the CBOR encoded Sig_structure header defined as the number of the first bytes of the encoded Sig_structure until the payload CBOR object starts.
     */
    uint16_t        sigStructureHeaderSize;

    uint8_t         reserved1[2U]; /**< @brief Reserved bytes  */

    /** @brief   INPUT/OUTPUT:  Specifies the address of the payload CBOR object used inside the COSE_Sign1 certificate structure documented in the RFC9052.
     *                          HSE will fill the map items placeholders inside the payload defined by the offset table.
     *                          HSE will concatenate the CBOR encoded Sig_structure header bytes with the filled payload CBOR object and sign it.
     */
    HOST_ADDR       pPayload;

    /** @brief   INPUT: The size of the CBOR payload object of the CBOR_Sign1 certificate structure.
     *                  @note The size of the CBOR payload object + sigStructureHeaderSize shall be maximum 1500 bytes.
     */
    uint16_t        payloadSize;

    uint8_t         reserved2[2U]; /**< @brief Reserved bytes  */

    /** @brief   INPUT: The offsets of the CBOR map items inside the certificate payload which needs to be filled by HSE.
     *                  @note For details see the description of this service.
     */
    uint16_t        offset[6U];

    /** @brief   OUTPUT: Where to generate the signature (64 bytes for ECC256 or 96 bytes for ECC384).
     */
    HOST_ADDR       pSignature;

    /** @brief   INPUT: Erase or not (TRUE or FALSE) the private key (except the keyHandleNvmUDSKeyPair) once the certificate is signed; The private key can not be used for further signing.
     *                  This option is also erasing the attest value correspondng to keyHandlePrivKey.
     *                  No further ECC key pairs can be generated using hseDiceGenerateKeyPairSrv_t service having srcKeyHandleKeyPair = keyHandlePrivKey.
     */
    bool_t          erasePrivKey;

    uint8_t         reserved3[19U]; /**< @brief Reserved bytes  */
} hseDiceGenSignCertSrv_t;


#endif /* HSE_SPT_DICE */



/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

#define HSE_STOP_PRAGMA_PACK
#include "hse_compiler_abs.h"

#ifdef __cplusplus
}
#endif

#endif /* HSE_SRV_DICE_H */

/** @} */
