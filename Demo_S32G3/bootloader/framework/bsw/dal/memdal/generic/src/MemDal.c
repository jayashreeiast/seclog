/*===================================================================================================*/
/*
*   @file              MemDal.c
*   project            AUTOSAR 4.4 GOLDVIP
*   platform           CORTEXM
*
*   @brief             Integration Framework - Memory Driver Abstraction Layer component
*   @details           This file implements the MemDAL interface to application layer
*
*   Autosar Version    4.4.0
*   Build Version      S32G3XX_GOLDVIP_24_12_0_EAR_ASR_REL_4_4_REV_0000_20231120
*
*   (c) Copyright 2006-2016 Freescale Semiconductor, Inc. Copyright 2017-2022 NXP
*   All Rights Reserved.
*/
/*===================================================================================================*/

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
#include "MemDal.h"
#include "SysDal.h"
#include "Console.h"

#if (MEMDAL_FEE_ENABLED == STD_ON)
    #include "MemDal_Fee.h"
#endif /* (MEMDAL_FEE_ENABLED == STD_ON) */

#if (MEMDAL_SDHC_ENABLED == STD_ON)
    #include "MemDal_Sdhc.h"
#endif /* (MEMDAL_SDHC_ENABLED == STD_ON) */

#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
    #include "Det.h"
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */

/*===================================================================================================*/
/*                              SOURCE FILE VERSION INFORMATION                                      */
/*===================================================================================================*/
#define MEMDAL_VENDOR_ID_C                                  43

#define MEMDAL_AR_RELEASE_MAJOR_VERSION_C                   4
#define MEMDAL_AR_RELEASE_MINOR_VERSION_C                   4
#define MEMDAL_AR_RELEASE_REVISION_VERSION_C                0

#define MEMDAL_SW_MAJOR_VERSION_C                           24
#define MEMDAL_SW_MINOR_VERSION_C                           12
#define MEMDAL_SW_PATCH_VERSION_C                           0

/*===================================================================================================*/
/*                                      FILE VERSION CHECKS                                          */
/*===================================================================================================*/

/* Check if MemDal source file and header file are of the same vendor */
#if (MEMDAL_VENDOR_ID_C != MEMDAL_VENDOR_ID)
    #error "MemDal.c and MemDal.h have different vendor ids"
#endif

/* Check if MemDal source file and header file are of the same Autosar version */
#if ((MEMDAL_AR_RELEASE_MAJOR_VERSION_C != MEMDAL_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_AR_RELEASE_MINOR_VERSION_C != MEMDAL_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_AR_RELEASE_REVISION_VERSION_C != MEMDAL_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal.c and MemDal.h are different"
#endif

/* Check if MemDal source file and header file are of the same Software version */
#if ((MEMDAL_SW_MAJOR_VERSION_C != MEMDAL_SW_MAJOR_VERSION) || \
     (MEMDAL_SW_MINOR_VERSION_C != MEMDAL_SW_MINOR_VERSION) || \
     (MEMDAL_SW_PATCH_VERSION_C != MEMDAL_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of MemDal.c and MemDal.h are different"
#endif

/* Check if MemDal source file and configuration header file are of the same vendor */
#if (MEMDAL_VENDOR_ID_C != MEMDAL_CFG_VENDOR_ID)
    #error "MemDal.c and MemDal_Cfg.h have different vendor ids"
#endif

/* Check if MemDal source file and configuration header file are of the same Autosar version */
#if ((MEMDAL_AR_RELEASE_MAJOR_VERSION_C != MEMDAL_CFG_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_AR_RELEASE_MINOR_VERSION_C != MEMDAL_CFG_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_AR_RELEASE_REVISION_VERSION_C != MEMDAL_CFG_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal.c and MemDal_Cfg.h are different"
#endif

/* Check if MemDal source file and configuration header file are of the same software version */
#if ((MEMDAL_SW_MAJOR_VERSION_C != MEMDAL_CFG_SW_MAJOR_VERSION) || \
     (MEMDAL_SW_MINOR_VERSION_C != MEMDAL_CFG_SW_MINOR_VERSION) || \
     (MEMDAL_SW_PATCH_VERSION_C != MEMDAL_CFG_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of MemDal.c and MemDal_Cfg.h are different"
#endif

/* Check if MemDal and SysDal header file are of the same Autosar version */
#if ((MEMDAL_AR_RELEASE_MAJOR_VERSION_C != SYSDAL_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_AR_RELEASE_MINOR_VERSION_C != SYSDAL_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_AR_RELEASE_REVISION_VERSION_C != SYSDAL_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal.c and SysDal.h are different"
#endif

#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
/* Check if MemDal and Det header file are of the same Autosar version */
#if ((MEMDAL_AR_RELEASE_MAJOR_VERSION_C != DET_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_AR_RELEASE_MINOR_VERSION_C != DET_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_AR_RELEASE_REVISION_VERSION_C != DET_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal.c and Det.h are different"
#endif
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */

#if (MEMDAL_FEE_ENABLED == STD_ON)
/* Check if MemDal source file and MemDal_Fee header file are of the same Autosar version */
#if ((MEMDAL_AR_RELEASE_MAJOR_VERSION_C != MEMDAL_FEE_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_AR_RELEASE_MINOR_VERSION_C != MEMDAL_FEE_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_AR_RELEASE_REVISION_VERSION_C != MEMDAL_FEE_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal.c and MemDal_Fee.h are different"
#endif

/* Check if MemDal source file and MemDal_Fee header file are of the same software version */
#if ((MEMDAL_SW_MAJOR_VERSION_C != MEMDAL_FEE_SW_MAJOR_VERSION) || \
     (MEMDAL_SW_MINOR_VERSION_C != MEMDAL_FEE_SW_MINOR_VERSION) || \
     (MEMDAL_SW_PATCH_VERSION_C != MEMDAL_FEE_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of MemDal.c and MemDal_Fee.h are different"
#endif
#endif /* (MEMDAL_FEE_ENABLED == STD_ON) */

#if (MEMDAL_SDHC_ENABLED == STD_ON)
/* Check if MemDal source file and MemDal_Sdhc header file are of the same Autosar version */
#if ((MEMDAL_AR_RELEASE_MAJOR_VERSION_C != MEMDAL_SDHC_AR_RELEASE_MAJOR_VERSION) || \
     (MEMDAL_AR_RELEASE_MINOR_VERSION_C != MEMDAL_SDHC_AR_RELEASE_MINOR_VERSION) || \
     (MEMDAL_AR_RELEASE_REVISION_VERSION_C != MEMDAL_SDHC_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of MemDal.c and MemDal_Sdhc.h are different"
#endif

/* Check if MemDal source file and MemDal_Sdhc header file are of the same software version */
#if ((MEMDAL_SW_MAJOR_VERSION_C != MEMDAL_SDHC_SW_MAJOR_VERSION) || \
     (MEMDAL_SW_MINOR_VERSION_C != MEMDAL_SDHC_SW_MINOR_VERSION) || \
     (MEMDAL_SW_PATCH_VERSION_C != MEMDAL_SDHC_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of MemDal.c and MemDal_Sdhc.h are different"
#endif
#endif /* (MEMDAL_SDHC_ENABLED == STD_ON) */

/*===================================================================================================*/
/*                               LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)                          */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                       DEFINES AND MACROS                                          */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                             LOCAL CONSTANTS                                       */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                             LOCAL VARIABLES                                       */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                            GLOBAL CONSTANTS                                       */
/*===================================================================================================*/

/*===================================================================================================*/
/*                                            GLOBAL VARIABLES                                       */ 
/*===================================================================================================*/

/**
* @brief The component state
*/
static VAR(MemDal_ModuleState, AUTOMATIC) MemDal_eState = MEMDAL_UNINITIALIZED;

/**
* @brief Pointer to global MemDal configuration structure
*/
static P2CONST(MemDal_ConfigType, AUTOMATIC, MEMDAL_APPL_DATA) MemDal_pConfig = NULL_PTR;

/**
* @brief MemDal jobs array
*/
static VAR(MemDal_JobType, AUTOMATIC) MemDal_aJobs[MEMDAL_JOB_QUEUE_SIZE] = {0};

/**
* @brief MemDal jobs queue
*/
static VAR(MemDal_JobsQueueType, AUTOMATIC) MemDal_Queue;

/**
* @brief Last ECU running state read from MemDal_Main
*/
static VAR(SysDal_EcuModeType, AUTOMATIC) MemDal_eLastEcuMode;


/*===================================================================================================*/
/*                                    LOCAL FUNCTION PROTOTYPES                                      */
/*===================================================================================================*/

static FUNC(Std_ReturnType, AUTOMATIC) MemDal_WriteAllBlocks(void);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_JobProcessor(void);
static FUNC(boolean, AUTOMATIC) MemDal_IsJobDone(void);
static FUNC(MemDal_BlockStatusType, AUTOMATIC) MemDal_GetJobResult(void);
static FUNC(MemDal_BlockType*, AUTOMATIC) MemDal_GetBlockById(CONST(MemDal_BlockIdType, AUTOMATIC) BlockId);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_ExecuteJob(P2VAR(MemDal_JobType, AUTOMATIC, MEMDAL_APPL_DATA) pJob);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Job_Read(P2VAR(MemDal_JobType, AUTOMATIC, MEMDAL_APPL_DATA) pJob);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Job_Write(P2VAR(MemDal_JobType, AUTOMATIC, MEMDAL_APPL_DATA) pJob);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Job_Erase(P2VAR(MemDal_JobType, AUTOMATIC, MEMDAL_APPL_DATA) pJob);
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_AddJob
(
    P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock,
    VAR(MemDal_OperationType, AUTOMATIC) eOp
);

/*===================================================================================================*/
/*                                      LOCAL FUNCTIONS                                              */
/*===================================================================================================*/

/*===================================================================================================*/
/**
* @brief      MemDal component initialization function
* @details    
* 
* @param[in]  pConfig - Pointer to MemDal module configuration structure
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) MemDal_Init(P2CONST(MemDal_ConfigType, AUTOMATIC, MEMDAL_APPL_CONST ) pConfig)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;

    if (MEMDAL_INITIALIZED == MemDal_eState)
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_INIT_API_ID, MEMDAL_E_ALREADY_INIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }
    else
    {
        if (NULL_PTR == pConfig)
        {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_INIT_API_ID, MEMDAL_E_PARAM_POINTER);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
        }
        else
        {   
            status = E_OK; /* If SDHC is disabled, initialization should continue for FEE blocks */
#if (MEMDAL_SDHC_ENABLED == STD_ON)
            status = MemDal_Sdhc_Init(pConfig);
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
            if (E_OK != status)
            {
                Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_INIT_API_ID, MEMDAL_E_FAILED);
            }
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
#endif /* (MEMDAL_SDHC_ENABLED == STD_ON) */
        }
    }

    if (E_OK == status)
    {
        /* Save global configuration pointer */
        MemDal_pConfig = pConfig;
        
        /* Initialize the MemDal jobs queue */
        MemDal_Queue.pJobs = MemDal_aJobs;
        MemDal_Queue.u16Counter = 0;
        MemDal_Queue.u16Head = 0;
        MemDal_Queue.u16Tail = 0;

        MemDal_eState = MEMDAL_INITIALIZED;
    }

    CONSOLE_LOG(CONSOLE_INFO, "MemDal_Init done\r\n");
}

/*===================================================================================================*/
/**
* @brief      MemDal component deinitialization function
* @details    
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) MemDal_DeInit(void)
{
#if (MEMDAL_SDHC_ENABLED == STD_ON)
    MemDal_Sdhc_Deinit();
#endif /* (MEMDAL_SDHC_ENABLED == STD_ON) */
    
    MemDal_eState = MEMDAL_UNINITIALIZED;
}

/*===================================================================================================*/
/**
* @brief      MemDal component main function
* @details    This function must be called cyclically
* 
* @param[in]  None
* 
* @return     None
*/
/*===================================================================================================*/
FUNC(void, AUTOMATIC) MemDal_Main(void)
{
    VAR(SysDal_EcuModeType, AUTOMATIC) EcuMode = SYSDAL_STATE_RUN;

    if (MEMDAL_INITIALIZED == MemDal_eState)
    {
#if (SYSDAL_LOW_POWER_MODE == STD_ON)
        EcuMode = SysDal_GetCurrentPowerMode();
#endif /* (SYSDAL_LOW_POWER_MODE == STD_ON) */

        /* Transition from ECU run state to sleep state */
        if ((SYSDAL_STATE_RUN == MemDal_eLastEcuMode) && (SYSDAL_STATE_SLEEP == EcuMode))
        {
            MemDal_WriteAllBlocks();
        }

        MemDal_JobProcessor();

        if ((0 == MemDal_Queue.u16Counter) && (SYSDAL_STATE_SLEEP == EcuMode))
        {
            /* Enter sleep mode Not implemented  */
        }
        MemDal_eLastEcuMode = EcuMode;
    }
    else
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_MAINFUNCTION_API_ID, MEMDAL_E_UNINIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }
}

/*===================================================================================================*/
/**
* @brief      Add new job to MemDal
* @details    Add a block operation (read/write/erase) which will be processed in MemDal_Main function
* 
* @param[in]  pBlock - pointer to block descriptor
* @param[in]  eOp - block operation (read/write/erase)
* 
* @return     E_OK - job added
*             E_NOT_OK - job not added
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_AddJob
(
    P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock,
    VAR(MemDal_OperationType, AUTOMATIC) eOp
)
{
    VAR(Std_ReturnType, AUTOMATIC) retCode = E_NOT_OK;

    /* Check to be a valid block before adding to jobs queue */
    if (NULL_PTR != pBlock)
    {
        MemDal_Queue.pJobs[MemDal_Queue.u16Tail].pBlock = pBlock;
        MemDal_Queue.pJobs[MemDal_Queue.u16Tail].eOperation = eOp;
        MemDal_Queue.pJobs[MemDal_Queue.u16Tail].eJobStatus = MEMDAL_JOB_PENDING;

        MemDal_Queue.u16Tail = (MemDal_Queue.u16Tail + 1) % MEMDAL_JOB_QUEUE_SIZE;
        MemDal_Queue.u16Counter++;

        pBlock->eBlockStatus = MEMDAL_BLOCK_STATE_BUSY;

        retCode = E_OK;
    }

    return retCode;
}

/*===================================================================================================*/
/**
* @brief      Write all blocks to external memory
* @details    This function will add write job requests to MemDal jobs queue
* 
* @param[in]  None
* 
* @return     E_OK - write requests added
*             E_NOT_OK - failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_WriteAllBlocks(void)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_OK;
    VAR(uint16, AUTOMATIC) u16Idx;
    P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock;

    pBlock = MemDal_pConfig->pBlocks;

    for (u16Idx = 0; u16Idx < MEMDAL_BLOCKS_NUMBER; u16Idx++)
    {
        if (TRUE == pBlock[u16Idx].bIsUpdated)
        {
            MemDal_AddJob(&pBlock[u16Idx], MEMDAL_OP_WRITE);
        }
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      Process memory operations requests
* @details    This function will check if there are new memory operations requests and call the appropriate
*             driver functions. The next step is to supervise the job execution, mark it as completed once
*             is done, and remove the job from the queue.
* 
* @param[in]  None
* 
* @return     E_OK - write requests added
*             E_NOT_OK - failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_JobProcessor(void)
{
    VAR(Std_ReturnType, AUTOMATIC) retCode = E_OK;
    P2VAR(MemDal_JobType, AUTOMATIC, MEMDAL_APPL_DATA) pJob = NULL_PTR;
    
    pJob = &MemDal_Queue.pJobs[MemDal_Queue.u16Head];

    if (MemDal_Queue.u16Counter > 0)
    {
        if (MEMDAL_JOB_PENDING == pJob->eJobStatus)
        {
            /* Execute memory operation */
            if (E_OK == MemDal_ExecuteJob(pJob))
            {
                pJob->eJobStatus = MEMDAL_JOB_ONGOING;
            }
        }
        
        if (MEMDAL_JOB_ONGOING == pJob->eJobStatus)
        {
            /* Check if memory operation is complete */
            if (TRUE == MemDal_IsJobDone())
            {
                pJob->pBlock->eBlockStatus = MemDal_GetJobResult();

                pJob->eJobStatus = MEMDAL_JOB_COMPLETED;

                MemDal_Queue.u16Counter--;

                MemDal_Queue.u16Head = (MemDal_Queue.u16Head + 1) % MEMDAL_JOB_QUEUE_SIZE;
            }
        }
    }

    return retCode;
}

/*===================================================================================================*/
/**
* @brief      Execute requested job: read/write/erase
* @details    This function will execute the appropriate job based on operation requested
* 
* @param[in]  pJob - job tot be executed
* 
* @return     E_OK - job was launched
*             E_NOT_OK - job failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_ExecuteJob(P2VAR(MemDal_JobType, AUTOMATIC, MEMDAL_APPL_DATA) pJob)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_OK;

    switch(pJob->eOperation)
    {
    case MEMDAL_OP_READ:
        status = MemDal_Job_Read(pJob);
        break;
    
    case MEMDAL_OP_WRITE:
        status = MemDal_Job_Write(pJob);
        break;

    case MEMDAL_OP_ERASE:
        status = MemDal_Job_Erase(pJob);
        break;
    
    default:
        status = E_NOT_OK;
        break;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      Execute read job
* @details    This function will call the appropriate driver read function based on memory type
* 
* @param[in]  pJob - job tot be executed
* 
* @return     E_OK - job was launched
*             E_NOT_OK - job failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Job_Read(P2VAR(MemDal_JobType, AUTOMATIC, MEMDAL_APPL_DATA) pJob)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;

    switch (pJob->pBlock->eMemory)
    {
#if (MEMDAL_FEE_ENABLED == STD_ON)
    case MEMDAL_MEMORY_FLASH:
        status = MemDal_Fee_Read(pJob->pBlock);
        break;
#endif /* (MEMDAL_FEE_ENABLED == STD_ON) */
    
#if (MEMDAL_SDHC_ENABLED == STD_ON)
    case MEMDAL_MEMORY_SDCARD:
        status = MemDal_Sdhc_Read(pJob->pBlock);
        break;
#endif /* (MEMDAL_SDHC_ENABLED == STD_ON) */
    
    default:
        break;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      Execute read job
* @details    This function will call the appropriate driver write function based on memory type
* 
* @param[in]  pJob - job tot be executed
* 
* @return     E_OK - job was launched
*             E_NOT_OK - job failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Job_Write(P2VAR(MemDal_JobType, AUTOMATIC, MEMDAL_APPL_DATA) pJob)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;

    switch (pJob->pBlock->eMemory)
    {
#if (MEMDAL_FEE_ENABLED == STD_ON)
    case MEMDAL_MEMORY_FLASH:
        status = MemDal_Fee_Write(pJob->pBlock);
        break;
#endif /* (MEMDAL_FEE_ENABLED == STD_ON) */
    
#if (MEMDAL_SDHC_ENABLED == STD_ON)
    case MEMDAL_MEMORY_SDCARD:
        status = MemDal_Sdhc_Write(pJob->pBlock);
        break;
#endif /* (MEMDAL_SDHC_ENABLED == STD_ON) */ 
    
    default:
        break;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      Execute read job
* @details    This function will call the appropriate driver erase function based on memory type
* 
* @param[in]  pJob - job tot be executed
* 
* @return     E_OK - job was launched
*             E_NOT_OK - job failed
*/
/*===================================================================================================*/
static FUNC(Std_ReturnType, AUTOMATIC) MemDal_Job_Erase(P2VAR(MemDal_JobType, AUTOMATIC, MEMDAL_APPL_DATA) pJob)
{
    VAR(Std_ReturnType, AUTOMATIC) status = E_NOT_OK;

    switch (pJob->pBlock->eMemory)
    {
#if (MEMDAL_FEE_ENABLED == STD_ON)
    case MEMDAL_MEMORY_FLASH:
        status = MemDal_Fee_Erase(pJob->pBlock);
        break;
#endif /* (MEMDAL_FEE_ENABLED == STD_ON) */
    
#if (MEMDAL_SDHC_ENABLED == STD_ON)
    case MEMDAL_MEMORY_SDCARD:
        status = MemDal_Sdhc_Erase(pJob->pBlock);
        break;
#endif /* (MEMDAL_SDHC_ENABLED == STD_ON) */
    
    default:
        break;
    }

    return status;
}

/*===================================================================================================*/
/**
* @brief      Check if current MemDal job is finished
* @details    This function will check the appropriate memory driver status (FEE/SDHC)
* 
* @param[in]  None
* 
* @return     TRUE - job completed
*             FALSE - job not completed
*/
/*===================================================================================================*/
static FUNC(boolean, AUTOMATIC) MemDal_IsJobDone(void)
{
    VAR(boolean, AUTOMATIC) bJobIsDone = FALSE;
    VAR(MemDal_MemoryType, AUTOMATIC) BlockMem;

    BlockMem = MemDal_Queue.pJobs[MemDal_Queue.u16Head].pBlock->eMemory;

    switch(BlockMem)
    {
#if (MEMDAL_FEE_ENABLED == STD_ON)
    case MEMDAL_MEMORY_FLASH:
        bJobIsDone = MemDal_Fee_IsJobDone();
        break;
#endif /* (MEMDAL_FEE_ENABLED == STD_ON) */
    
#if (MEMDAL_SDHC_ENABLED == STD_ON)
    case MEMDAL_MEMORY_SDCARD:
        bJobIsDone = MemDal_Sdhc_IsJobDone();
        break;
#endif /* (MEMDAL_SDHC_ENABLED == STD_ON) */

    default:
        break;
    }

    return bJobIsDone;
}

/*===================================================================================================*/
/**
* @brief      Get the result of a MemDal read/write/erase operation
* @details    This function will mainly check if the memory operation was successful or not
* 
* @param[in]  None
* 
* @return     MEMDAL_BLOCK_STATE_ERROR - block operation error
*             MEMDAL_BLOCK_STATE_READY - block operation succesful
*             MEMDAL_BLOCK_STATE_BUSY - operation on going
*/
/*===================================================================================================*/
static FUNC(MemDal_BlockStatusType, AUTOMATIC) MemDal_GetJobResult(void)
{
    VAR(MemDal_BlockStatusType, AUTOMATIC) jobResult = MEMDAL_BLOCK_STATE_ERROR;
    
    switch (MemDal_Queue.pJobs[MemDal_Queue.u16Head].pBlock->eMemory)
    {
#if (MEMDAL_FEE_ENABLED == STD_ON)
    case MEMDAL_MEMORY_FLASH:
        jobResult = MemDal_Fee_GetJobResult();
        break;
#endif /* (MEMDAL_FEE_ENABLED == STD_ON) */
    
#if (MEMDAL_SDHC_ENABLED == STD_ON)
    case MEMDAL_MEMORY_SDCARD:
        jobResult = MemDal_Sdhc_GetJobResult();
        break;
#endif /* (MEMDAL_SDHC_ENABLED == STD_ON) */

    default:
        break;
    }

    return jobResult;
}

/*===================================================================================================*/
/**
* @brief      Get MemDal block by ID
* @details    This function will return the pointer to the MemDal block descriptor structure
* 
* @param[in]  BlockId - block ID
* 
* @return     Pointer to MemDal block
*/
/*===================================================================================================*/
static FUNC(MemDal_BlockType*, AUTOMATIC) MemDal_GetBlockById(CONST(MemDal_BlockIdType, AUTOMATIC) BlockId)
{
    P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock = NULL_PTR;
    VAR(uint16, AUTOMATIC) u16Idx;

    for (u16Idx = 0; u16Idx < MEMDAL_BLOCKS_NUMBER; u16Idx++)
    {
        if (BlockId == MemDal_pConfig->pBlocks[u16Idx].BlockId)
        {
            pBlock = &MemDal_pConfig->pBlocks[u16Idx];
            break;
        }
    }

    return pBlock;
}

/*===================================================================================================*/
/**
* @brief      MemDal read block
* @details    This function will set the destination address of the RAM buffer and start the read operation.
*             The caller has to ensure that size of the RAM buffer is large enough to fit the entire MemDal block.
*
* @param[in]  BlockId - ID of the block to be read
* @param[in]  u32DestinationAddress - the address of the destination RAM buffer
* 
* @return     E_OK - read operation started
*             E_NOT_OK - read operation rejected
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_ReadBlock
(
    CONST(MemDal_BlockIdType, AUTOMATIC) BlockId,
    VAR(uint32, AUTOMATIC) u32DestinationAddress
)
{
    VAR(Std_ReturnType, AUTOMATIC) readStatus = E_NOT_OK;
    P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock = NULL_PTR;

    if (MEMDAL_INITIALIZED == MemDal_eState)
    {
        pBlock = MemDal_GetBlockById(BlockId);

        /* For invalid block ID we get a NULL block pointer */
        if (NULL_PTR != pBlock)
        {
            if (MEMDAL_BLOCK_STATE_BUSY != pBlock->eBlockStatus)
            {
                pBlock->u32RamBuffer = u32DestinationAddress;
                readStatus = MemDal_AddJob(pBlock, MEMDAL_OP_READ);
            }
        }
        else
        {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_READ_API_ID, MEMDAL_E_INVALID_BLOCK);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
        }
    }
    else
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_READ_API_ID, MEMDAL_E_UNINIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return readStatus;
}

/*===================================================================================================*/
/**
* @brief      MemDal write block
* @details    This function will set the source address of the RAM buffer and start the write operation.
*             The caller has to ensure data integrity of RAM buffer while data is copyed to external memory.
*             If block write type is ONREQUEST the transfer will be done in the next MemDal_Main() call.
*             If block write type is ONCLOSING the transfer will be done on ECU transition to sleep mode.
*
* @param[in]  BlockId - ID of the block to be written
* @param[in]  u32SourceAddress - the address of the source RAM buffer
* 
* @return     E_OK - write operation started
*             E_NOT_OK - write operation rejected
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_WriteBlock
(
    CONST(MemDal_BlockIdType, AUTOMATIC) BlockId,
    VAR(uint32, AUTOMATIC) u32SourceAddress
)
{
    VAR(Std_ReturnType, AUTOMATIC) writeStatus = E_NOT_OK;
    P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock = NULL_PTR;

    if (MEMDAL_INITIALIZED == MemDal_eState)
    {
        pBlock = MemDal_GetBlockById(BlockId);

        /* For invalid block ID we get a NULL block pointer */
        if (NULL_PTR != pBlock)
        {
            if (MEMDAL_BLOCK_STATE_BUSY != pBlock->eBlockStatus)
            {
                pBlock->u32RamBuffer = u32SourceAddress;

                if (MEMDAL_WRITE_ONREQUEST == pBlock->eWrite)
                {
                    writeStatus = MemDal_AddJob(pBlock, MEMDAL_OP_WRITE);
                }
                else if (MEMDAL_WRITE_ONCLOSING == pBlock->eWrite)
                {
                    pBlock->bIsUpdated = TRUE;
                    writeStatus = E_OK;
                }
            }
        }
        else
        {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_WRITE_API_ID, MEMDAL_E_INVALID_BLOCK);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
        }
    }
    else
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_WRITE_API_ID, MEMDAL_E_UNINIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return writeStatus;
}

/*===================================================================================================*/
/**
* @brief      MemDal erase block
* @details    This function will trigger an erase operation of the external memory block. MemDal will not
*             affect the content of RAM buffer during this operation, this being application responsibility.
*
* @param[in]  BlockId - ID of the block to be erased
* 
* @return     E_OK - erase operation started
*             E_NOT_OK - erase operation rejected
*/
/*===================================================================================================*/
FUNC(Std_ReturnType, AUTOMATIC) MemDal_EraseBlock(CONST(MemDal_BlockIdType, AUTOMATIC) BlockId)
{
    VAR(Std_ReturnType, AUTOMATIC) eraseStatus = E_NOT_OK;
    P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock = NULL_PTR;

    if (MEMDAL_INITIALIZED == MemDal_eState)
    {
        pBlock = MemDal_GetBlockById(BlockId);

        /* For invalid block ID we get a NULL block pointer */
        if (NULL_PTR != pBlock)
        {
            eraseStatus = MemDal_AddJob(pBlock, MEMDAL_OP_ERASE);
        }
        else
        {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_ERASE_API_ID, MEMDAL_E_INVALID_BLOCK);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
        }
    }
    else
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_ERASE_API_ID, MEMDAL_E_UNINIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return eraseStatus;
}


/*===================================================================================================*/
/**
* @brief      Get the block status
* @details    The block will start in the UNINIT state and will switch between BUSY and READY status
*             during read/write operations.
*
* @param[in]  BlockId - ID of the block
* 
* @return     MEMDAL_BLOCK_STATE_ERROR - block operations error
*             MEMDAL_BLOCK_STATE_READY - block ready for operations
*             MEMDAL_BLOCK_STATE_BUSY - operations on going
*             MEMDAL_BLOCK_STATE_UNINIT -  block not initialized
*/
/*===================================================================================================*/
FUNC(MemDal_BlockStatusType, AUTOMATIC) MemDal_GetBlockStatus
(
    CONST(MemDal_BlockIdType, AUTOMATIC) BlockId
)
{
    VAR(MemDal_BlockStatusType, AUTOMATIC) eBlockStatus = MEMDAL_BLOCK_STATE_ERROR;
    P2VAR(MemDal_BlockType, AUTOMATIC, MEMDAL_APPL_DATA) pBlock = NULL_PTR;

    if (MEMDAL_INITIALIZED == MemDal_eState)
    {
        pBlock = MemDal_GetBlockById(BlockId);

        /* For invalid block ID we get a NULL block pointer */
        if (NULL_PTR != pBlock)
        {
            eBlockStatus = pBlock->eBlockStatus;
        }
        else
        {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
            Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_GETBLOCKSTATUS_API_ID, MEMDAL_E_INVALID_BLOCK);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
        }
    }
    else
    {
#if (MEMDAL_DEV_ERROR_DETECT == STD_ON)
        Det_ReportError(MEMDAL_MODULE_ID, 0U, MEMDAL_GETBLOCKSTATUS_API_ID, MEMDAL_E_UNINIT);
#endif /* MEMDAL_DEV_ERROR_DETECT == STD_ON */
    }

    return eBlockStatus;
}


/*===================================================================================================*/
#ifdef __cplusplus
}
#endif
