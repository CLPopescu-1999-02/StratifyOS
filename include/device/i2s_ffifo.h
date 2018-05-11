/* Copyright 2011-2016 Tyler Gilbert; 
 * This file is part of Stratify OS.
 *
 * Stratify OS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Stratify OS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stratify OS.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 */

/*! \addtogroup USB_FIFO
 * @{
 * \ingroup IFACE_DEV
 *
 */

/*! \file  */

#ifndef DEVICE_I2S_FFIFO_H_
#define DEVICE_I2S_FFIFO_H_

#include "sos/dev/i2s_ffifo.h"
#include "mcu/i2s.h"
#include "ffifo.h"



typedef struct MCU_PACK {
    ffifo_attr_t tx;
    ffifo_attr_t rx;
} i2s_ffifo_attr_t;

typedef struct MCU_PACK {
    ffifo_info_t ffifo;
    u32 count;
    s32 error;
} i2s_ffifo_channel_info_t;

typedef struct MCU_PACK {
    i2s_ffifo_channel_info_t rx;
    i2s_ffifo_channel_info_t tx;
} i2s_ffifo_info_t;


typedef struct {
    ffifo_state_t ffifo;
    devfs_async_t i2s_async;
    u32 count;
    s32 error;
} i2s_ffifo_channel_state_t;

typedef struct {
    i2s_ffifo_channel_state_t rx;
    i2s_ffifo_channel_state_t tx;
} i2s_ffifo_state_t;

typedef struct {
    i2s_config_t i2s;
    ffifo_config_t tx;
    ffifo_config_t rx;
} i2s_ffifo_config_t;


int i2s_ffifo_open(const devfs_handle_t * handle);
int i2s_ffifo_ioctl(const devfs_handle_t * handle, int request, void * ctl);
int i2s_ffifo_read(const devfs_handle_t * handle, devfs_async_t * async);
int i2s_ffifo_write(const devfs_handle_t * handle, devfs_async_t * async);
int i2s_ffifo_close(const devfs_handle_t * handle);

int i2s_spi_ffifo_open(const devfs_handle_t * handle);
int i2s_spi_ffifo_ioctl(const devfs_handle_t * handle, int request, void * ctl);
int i2s_spi_ffifo_read(const devfs_handle_t * handle, devfs_async_t * async);
int i2s_spi_ffifo_write(const devfs_handle_t * handle, devfs_async_t * async);
int i2s_spi_ffifo_close(const devfs_handle_t * handle);




#endif /* DEVICE_I2S_FFIFO_H_ */


/*! @} */
