/*===================================================================================================*/
/*
*   @file              MemDal_Types.h
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - Types file for MemDal component
*   @details           This file implements the MemDal Types
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

#ifndef MEMDAL_TYPES_H
#define MEMDAL_TYPES_H

#ifdef __cplusplus
extern "C"{
#endif

/*===================================================================================================*/
/*                                         INCLUDE FILES                                             
   1) system and project includes
   2) needed interfaces from external units
   3) internal and external interfaces from this unit
*/   
/*===================================================================================================*/
#include "Std_Types.h"

/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define MEMDAL_TYPES_VENDOR_ID                          43

#define MEMDAL_TYPES_AR_RELEASE_MAJOR_VERSION           4
#define MEMDAL_TYPES_AR_RELEASE_MINOR_VERSION           4
#define MEMDAL_TYPES_AR_RELEASE_REVISION_VERSION        0

#define MEMDAL_TYPES_SW_MAJOR_VERSION                   24
#define MEMDAL_TYPES_SW_MINOR_VERSION                   12
#define MEMDAL_TYPES_SW_PATCH_VERSION                   0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                 STRUCTURES AND OTHER TYPEDEFS                                     */
/*===================================================================================================*/

/**
* @brief     The component state
* @details   Used for coding the possible states of the component.
*/
typedef enum
{
    MEMDAL_UNINITIALIZED,                   /**< @brief uninitialized (initial state) */
    MEMDAL_INITIALIZED,                     /**< @brief initialized */
    MEMDAL_DEINITIALIZED                    /**< @brief deinitialized */
} MemDal_ModuleState;

/**
* @brief          MemDal memory block status type
* @details        
*/
typedef enum
{
    MEMDAL_BLOCK_STATE_ERROR = 0,           /**< @brief block operations error */
    MEMDAL_BLOCK_STATE_READY,               /**< @brief block ready for operations */
    MEMDAL_BLOCK_STATE_BUSY,                /**< @brief operations on going */
    MEMDAL_BLOCK_STATE_UNINIT               /**< @brief block not initialized  */
} MemDal_BlockStatusType;

/**
* @brief          MemDal block memory type
* @details        This lists the types of external memory devices
*/
typedef enum
{
    MEMDAL_MEMORY_FLASH = 0,                /**< @brief Flash memory type */
    MEMDAL_MEMORY_SDCARD                    /**< @brief Sdhc memory type */
} MemDal_MemoryType;

/**
* @brief          MemDal block write mode
* @details        Describe at what moment the block will be written
*/
typedef enum
{
    MEMDAL_WRITE_ONREQUEST = 0,             /**< @brief Write block on request */
    MEMDAL_WRITE_ONCLOSING                  /**< @brief Write block on closing */
} MemDal_WriteType;

/**
* @brief          MemDal block operation
* @details        Describe the types of operations performed on a memory block
*/
typedef enum
{
    MEMDAL_OP_READ = 0,                     /**< @brief Read operation */
    MEMDAL_OP_WRITE,                        /**< @brief Write operation */
    MEMDAL_OP_ERASE                         /**< @brief Erase operation */
} MemDal_OperationType;

/**
* @brief          MemDal jobs status
* @details        Describe the list of possible states for a MemDal job while is performing block operations
*/
typedef enum
{
    MEMDAL_JOB_ERROR = 0,                   /**< @brief MemDal job error */
    MEMDAL_JOB_PENDING,                     /**< @brief MemDal job waiting to be processed */
    MEMDAL_JOB_ONGOING,                     /**< @brief MemDal job on going */
    MEMDAL_JOB_COMPLETED                    /**< @brief MemDal job was precessed */
} MemDal_JobStatusType;

/**
* @brief          MemDal memory block ID type
* @details        
*/
typedef uint16 MemDal_BlockIdType;

/**
* @brief          MemDal memory block address type
* @details        
*/
typedef uint32 MemDal_AddressType;

/**
* @brief          MemDal SDHC file descriptor type
* @details        This structure contains a file path and corresponding access options
*/
typedef struct
{
    /**
    * @brief Absolute path of the file
    */
    P2VAR(char, AUTOMATIC, MEMDAL_APPL_DATA) pFilePath;

    /**
    * @brief Pointer to block descriptor
    */
    VAR(uint8, AUTOMATIC) u8Options;
} MemDal_Sdhc_FileDescriptorType;

/**
* @brief          MemDal memory block type
* @details        MemDal structure for memory blocks configuration
*/
typedef struct
{
    /**
    * @brief MemDal block ID
    */
    CONST(MemDal_BlockIdType, AUTOMATIC) BlockId;
    /**
    * @brief MemDal block size in bytes
    */
    CONST(uint32, AUTOMATIC) u32BlockSize;
    /**
    * @brief External memory driver block id
    */
    CONST(uint16, AUTOMATIC) u16DrvBlockId;
    /**
    * @brief Block status
    */
    VAR(MemDal_BlockStatusType, AUTOMATIC) eBlockStatus;
    /**
    * @brief The type of memory
    */
    CONST(MemDal_MemoryType, AUTOMATIC) eMemory;
    /**
    * @brief Block write mode
    */
    CONST(MemDal_WriteType, AUTOMATIC) eWrite;
    /**
    * @brief Address of the RAM buffer
    */
    VAR(uint32, AUTOMATIC) u32RamBuffer;
    /**
    * @brief Block write mode
    */
    VAR(boolean, AUTOMATIC) bIsUpdated;
} MemDal_BlockType;

/**
* @brief          MemDal job type
* @details        This structure defines a block operation: read/write/erase
*/
typedef struct
{
    /**
    * @brief Pointer to block descriptor
    */
    P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock;
    /**
    * @brief Job status
    */
    VAR(MemDal_JobStatusType, AUTOMATIC) eJobStatus;
    /**
    * @brief Operation type
    */
    VAR(MemDal_OperationType, AUTOMATIC) eOperation;
} MemDal_JobType;

/**
* @brief          MemDal jobs queue type
* @details        This structure encapsulates the jobs queue data
*/
typedef struct
{
    /**
    * @brief Pointer to the jobs array
    */
    P2VAR(MemDal_JobType, AUTOMATIC, MEMDAL_APPL_DATA) pJobs;
    /**
    * @brief The queue head
    */
    VAR(uint16, AUTOMATIC) u16Head;
    /**
    * @brief The queue tail
    */
    VAR(uint16, AUTOMATIC) u16Tail;
    /**
    * @brief Number of pending jobs in queue
    */
    VAR(uint16, AUTOMATIC) u16Counter;
} MemDal_JobsQueueType;

/**
* @brief          MemDal Config Type
* @details        MemDal module initialization data structure
*/
typedef struct
{
    /**
    * @brief Number of FEE blocks
    */
    CONST( uint16, AUTOMATIC) u16FeeBlocksCount;
    /**
    * @brief Number of SDHC blocks
    */
    CONST( uint16, AUTOMATIC) u16SdhcBlocksCount;
    /**
    * @brief List of MemDal SDHC files for card fatfs mode access
    */
    P2VAR(MemDal_Sdhc_FileDescriptorType, AUTOMATIC, MEMDAL_APPL_DATA) pSdhcFiles;
    /**
    * @brief List of MemDal SDHC blocks address for card raw data mode access
    */
    P2VAR(MemDal_AddressType, AUTOMATIC, MEMDAL_APPL_DATA) pSdhcBlockAddress;
    /**
    * @brief List of MemDal blocks
    */
    P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlocks;
} MemDal_ConfigType;

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/

/**
* @brief     Development error codes
*/
/**< @brief Component used without being initialized first */
#define MEMDAL_E_UNINIT                     0xE0U

/**< @brief Component is already initialized */
#define MEMDAL_E_ALREADY_INIT               0xE1U

/**< @brief Invalid block requested */
#define MEMDAL_E_INVALID_BLOCK              0xE2U

/**< @brief Invalid pointer used as argument (i.e. NULL pointer) */
#define MEMDAL_E_PARAM_POINTER              0xE3U

/**< @brief Invalid action for the current session state */
#define MEMDAL_E_INVALID_ACTION             0xE4U

/**< @brief The action failed due to an internal reason */
#define MEMDAL_E_FAILED                     0xE6U

/**
* @brief     API service IDs
*/
/**< @brief Component/sub-component init function */
#define MEMDAL_INIT_API_ID                  0x00U 

/**< @brief Component/sub-component deinit function */
#define MEMDAL_DEINIT_API_ID                0x01U

/**< @brief Component/sub-component main function */
#define MEMDAL_MAINFUNCTION_API_ID          0x02U

/**< @brief MemDal get block status function */
#define MEMDAL_GETBLOCKSTATUS_API_ID        0x03U

/**< @brief MemDal read function */
#define MEMDAL_READ_API_ID                  0x0AU

/**< @brief MemDal write function */
#define MEMDAL_WRITE_API_ID                 0x0BU

/**< @brief MemDal erase function */
#define MEMDAL_ERASE_API_ID                 0x0CU

/**< @brief SDHC sub-component init function */
#define MEMDAL_SDHC_INIT_API_ID             0x0EU

/**< @brief SDHC sub-component deinit function */
#define MEMDAL_SDHC_DEINIT_API_ID           0x0FU

/**< @brief SDHC sub-component read function */
#define MEMDAL_SDHC_READ_API_ID             0x11U

/**< @brief SDHC sub-component write function */
#define MEMDAL_SDHC_WRITE_API_ID            0x12U

/**< @brief SDHC sub-component erase function */
#define MEMDAL_SDHC_ERASE_API_ID            0x13U

#ifdef __cplusplus
}
#endif

#endif /* MEMDAL_TYPES_H */
