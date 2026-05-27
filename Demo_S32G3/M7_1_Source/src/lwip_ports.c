/*
 *  Copyright 2025 NXP
 *
 *  NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only
 *  be used strictly in accordance with the applicable license terms. By
 *  expressly accepting such terms or by downloading, installing, activating
 *  and/or otherwise using the software, you are agreeing that you have read,
 *  and that you agree to comply with and are bound by, such license terms. If
 *  you do not agree to be bound by the applicable license terms, then you may
 *  not retain, install, activate or otherwise use the software.
 *
 *  This file contains sample code only. It is not part of the production code deliverables.
 */

#include <lwip/sys.h>
#include <lwip/sio.h>

extern u32_t g_sys_milliseconds;

u32_t sys_now(void)
{
    return g_sys_milliseconds;
}

sio_fd_t sio_open(u8_t devnum)
{
    (void)devnum;
    return NULL;
}

void sio_send(u8_t c, sio_fd_t fd)
{
    (void)c;
    (void)fd;
}

u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len)
{
    (void)fd;
    (void)*data;
    (void)len;
    return 0;
}