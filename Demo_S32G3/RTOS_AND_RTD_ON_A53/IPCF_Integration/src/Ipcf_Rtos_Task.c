/*
 *
 * IPC Shared Memory Driver sample application
 *
 * Copyright 2020-2025 NXP
 * All Rights Reserved.
 *
 * NXP Confidential. This software is owned or controlled by NXP and may only be
 * used strictly in accordance with the applicable license terms. By expressly
 * accepting such terms or by downloading, installing, activating and/or otherwise
 * using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms. If you do not agree to be
 * bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 *
 */
#if defined(__cplusplus)
extern "C"{
#endif

/* shm driver includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "ipc-shm.h"
#include "Ipcf_Rtos_Task.h"
#include "ipc-os.h"
#include "gic.h"

#define SAMPLE_NUM_MSGS	100

#define CTRL_CHAN_ID 0
#define CTRL_CHAN_SIZE 64
#define MAX_SAMPLE_MSG_LEN 32


uint8 localSemaphoreHandle;
typedef volatile uint8 *SemaphoreHandle_t;
#define xSemaphoreCreateCounting(max, init_val) (&localSemaphoreHandle)
#define xSemaphoreGive(psema) (*(psema))++

static sint8 sharpstr[] = "#";
static sint8 hellostr[] = " HELLO WORLD! from CORE ";
static sint8 send_buf[] = "SENDING MESSAGES: ";
static sint8 repl_buf[] = "REPLIED MESSAGES: ";

extern QueueHandle_t xQueue;

/**
 * struct ipc_sample_app - sample app private data
 * @ctrl_shm:		control channel local shared memory
 * @rx_count:		number of received messaged
 * @tx_count:		number of transmitted messaged
 * @expected_msgs:	number of expected messages
 * @last_rx_no_msg:	last number of received message
 * @last_rx_ch:		last Rx channel id
 */
static struct ipc_sample_app {
	sint8 *ctrl_shm;
	volatile uint16 rx_count;
	volatile uint16 tx_count;
	uint16 expected_msgs;
	uint16 last_rx_no_msg;
	uint8 last_rx_ch;
} app;

/* link with generated variables */
const void *rx_cb_arg = &app;

/* variable used for synchronization */
static SemaphoreHandle_t shm_event_rx;

/* alternative implementation of strtol */
uint32 ipc_strtol(sint8 *src)
{
	uint32 res = 0;

	while ((*src >= '0') && (*src <= '9')) {
		res = res*10 + (*(src++) - '0');
	}

	return res;
}

/* alternative implementation of strlen */
uint32 ipc_strlen(const sint8 *src)
{
	uint32 i;

	for (i = 0; src[i] != '\0'; i++)
		;
	return i;
}

/* alternative implementation of strcpy */
void ipc_strcpy(sint8 *dst, const sint8 *src)
{

	while ((*dst++ = *src++) != '\0')
		;
}

/* alternative implementation of strrchr */
sint8 *ipc_strrchr(const sint8 *s, int c)
{
	sint8 *rtnval = 0;

	do {
		if (*s == c)
			rtnval = (sint8 *) s;
	} while (*s++);
	return rtnval;
}

/* convert positive base 10 number to string */
static void ipc_numtostr(uint16 num, sint8 *str)
{
	uint16 tmp = num;

	/* if is only zero must return 0 */
	if (num == 0) {
		*++str = '0';
	}

	/* advance string to least significant digit position */
	while (tmp) {
		++str;
		tmp = tmp / 10;
	}

	*++str = '\0';

	/* insert digits backwards */
	while (num) {
		*--str = num % 10 + '0';
		num = num / 10;
	}
}

/*
 * data channel rx callback: increment rx counter, release buffer
 */
void data_chan_rx_cb(void *arg, const uint8 instance, uint8 chan_id, void *buf,
		uint32 size)
{
	sint8 err;
	struct ipc_sample_app *cb_arg_sample =
		(struct ipc_sample_app *)(*((uintptr *)arg));

	if (cb_arg_sample != &app || size > MAX_SAMPLE_MSG_LEN) {
		handle_error(-IPC_SHM_E_INVAL);
	}

	/* consume received data: get number of message */
	/* Note: without being copied locally */
	cb_arg_sample->last_rx_no_msg = ipc_strtol((sint8 *)buf + ipc_strlen(sharpstr));

	/* check if the chan_id is in sync with no_msg */
	if (chan_id != ((cb_arg_sample->last_rx_no_msg % 2) + 1)) {
		handle_error(-IPC_SHM_E_INVAL);
	}

	cb_arg_sample->last_rx_ch = chan_id;
	cb_arg_sample->rx_count++;

	/* release the buffer */
	err = ipc_shm_release_buf(instance, chan_id, buf);
	if (err) {
		handle_error(err);
	}
	/* notify demo function it can resume sending another message */
	xSemaphoreGive(shm_event_rx);
}

/* control channel Rx callback: get number of expected messages */
void ctrl_chan_rx_cb(void *arg, const uint8 instance, uint8 chan_id, void *mem)
{
	struct ipc_sample_app *cb_arg_sample =
		(struct ipc_sample_app *)(*((uintptr *)arg));

	if (cb_arg_sample != &app || ipc_strlen(mem) > MAX_SAMPLE_MSG_LEN ||
		chan_id != CTRL_CHAN_ID || IPCF_INSTANCE != instance) {
		handle_error(-IPC_SHM_E_INVAL);
	}

	/* get number of expected messages */
	cb_arg_sample->expected_msgs =
		ipc_strtol((sint8 *)mem + ipc_strlen(send_buf));

	/* reset counters */
	cb_arg_sample->rx_count = 0;
	cb_arg_sample->tx_count = 0;
}

/* send control message with number of data messages to be sent */
static int send_ctrl_msg(const uint8_t instance, int num_msgs)
{
	/* last channel is control channel */
	const int chan_id = CTRL_CHAN_ID;
	int err;

	/* write number of messages to be sent in control channel memory */
	ipc_strcpy(app.ctrl_shm, send_buf);
	ipc_numtostr(num_msgs, ipc_strrchr(app.ctrl_shm, ' '));

	/* notify remote */
	err = ipc_shm_unmanaged_tx(instance, chan_id);
	if (err) {
		return err;
	}

	return 0;
}


/* send ACK message back on control channel */
static void reply_ctrl_msg(const uint8 instance, uint8 chan_id)
{
	sint8 err;

	ipc_strcpy(app.ctrl_shm, repl_buf);
	ipc_numtostr(app.rx_count, ipc_strrchr(app.ctrl_shm, ' '));

	err = ipc_shm_unmanaged_tx(instance, chan_id);
	if (err)
		handle_error(err);
}

void Ipcf_Rtos_Prepare(void)
{
	sint8 err = -IPC_SHM_E_INVAL;
	
	/* Initialize the semaphore for shm_event_rx */
	shm_event_rx = xSemaphoreCreateCounting(0xFFu, 0);

	/* init variables before use */
	app.tx_count = 0;
	app.rx_count = 0;
	/* INT1_IRQn interrupt initialized*/
	vGicRegisterIsrHandler(MSCM_INT1_IRQn, ipc_shm_hardirq);
    vGicSetIntPriority(MSCM_INT1_IRQn, 10);
    vGicEnableIntID(MSCM_INT1_IRQn);

	/* wait until initialized or error */
	do {
		err = ipc_shm_init(&ipcf_shm_instances_cfg);
	} while (err == -IPC_SHM_E_REMOTE_INIT_IN_PROGRESS);
	if (err)
		handle_error(err);

	/* wait for remote core to initialize the shm driver */
	while (ipc_shm_is_remote_ready(IPCF_INSTANCE) != 0) {
		/* busy wait */
	}

	/* acquire control channel memory once (unmanaged channel) */
	app.ctrl_shm = ipc_shm_unmanaged_acquire(IPCF_INSTANCE, CTRL_CHAN_ID);
	if (!app.ctrl_shm)
		handle_error(-IPC_SHM_E_NOMEM);

		/* signal number of messages to remote via control channel */
	err = send_ctrl_msg(IPCF_INSTANCE, SAMPLE_NUM_MSGS);
	if (err)
		handle_error(err);

	app.tx_count = 0;
}
/**
 * shm_demo() - shared memory demo
 *
 * Echoes back received messages in an endless loop.
 */
void Ipcf_Send_Task(void)
{
	sint8 *buf = NULL;
	int chan_id = 0;
	sint8 err = -IPC_SHM_E_INVAL;

	TickType_t xNextWakeTime;
    const uint32_t ulValueToSend = 100UL;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

	for( ;; )
    {
        /* Place this task in the blocked state until it is time to run again. */
        vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

        /* Send to the queue - causing the queue receive task to unblock.  
		0 is used as the block time so the sending operation will not block - 
		it shouldn't need to block as the queue should always
        be empty at this point in the code. */
        xQueueSend( xQueue, &ulValueToSend, portMAX_DELAY );

        /* alternate MANAGE channel */
		chan_id = (app.tx_count % 2) + 1;

		/* acquire buffer for "small" messages */
		buf = ipc_shm_acquire_buf(IPCF_INSTANCE, chan_id, MAX_SAMPLE_MSG_LEN);
		
		if (!buf)
			handle_error(-IPC_SHM_E_NOMEM);

		/* generate the messages */
		ipc_strcpy(buf, sharpstr);
		ipc_numtostr(app.tx_count, ipc_strrchr(buf, '#'));
		ipc_strcpy(buf + ipc_strlen(buf), hellostr);
		ipc_numtostr(OsIf_GetCoreID(), ipc_strrchr(buf, ' '));

		if (ipc_strlen(buf) > MAX_SAMPLE_MSG_LEN) {
			handle_error(-IPC_SHM_E_NOMEM);
		}

		/* send IPCF messages */
		err = ipc_shm_tx(IPCF_INSTANCE, chan_id, buf, ipc_strlen(buf));
		if (err) {
			handle_error(err);
		}

		app.tx_count++;

    }

}

/**
 * handle_error() - callback for handling IPCF Shared Memory Driver errors
 * @error: ipc-shm driver error code
 *
 * Because we don't have iostream or any logging available we stop here to
 * allow manual debugging by attaching with debug probe.
 */
void handle_error(sint8 error)
{
	/* silence compiler warning  */
	(void)error;

	while (1) {
		/* block indefinitely */
	}
}

#if defined(__cplusplus)
}
#endif
