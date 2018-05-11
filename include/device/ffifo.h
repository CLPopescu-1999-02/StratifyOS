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


#ifndef DEVICE_FFIFO_H_
#define DEVICE_FFIFO_H_


#include "device/fifo.h"
#include "sos/dev/ffifo.h"
#include "sos/fs/devfs.h"


/*! \details This is the state for a framed FIFO (ffifo)
 *
 */
typedef struct MCU_PACK {
    volatile fifo_atomic_position_t atomic_position;
    devfs_transfer_handler_t transfer_handler;
	u32 o_flags;
} ffifo_state_t;

/*! \details This is the configuration for a framed FIFO (ffifo)
 *
 */
typedef struct MCU_PACK {
	u16 count /*! The number of frames in the buffer */;
	u16 frame_size /*! The size of each frame */;
	char * buffer /*! A pointer to the buffer */;
} ffifo_config_t;

int ffifo_open(const devfs_handle_t * handle);
int ffifo_ioctl(const devfs_handle_t * handle, int request, void * ctl);
int ffifo_read(const devfs_handle_t * handle, devfs_async_t * async);
int ffifo_write(const devfs_handle_t * handle, devfs_async_t * async);
int ffifo_close(const devfs_handle_t * handle);

int ffifo_open_local(const ffifo_config_t * config, ffifo_state_t * state);
int ffifo_close_local(const ffifo_config_t * config, ffifo_state_t * state);
int ffifo_ioctl_local(const ffifo_config_t * config, ffifo_state_t * state, int request, void * ctl);
int ffifo_write_local(const ffifo_config_t * config, ffifo_state_t * state, devfs_async_t * async, int allow_callback);
int ffifo_read_local(const ffifo_config_t * config, ffifo_state_t * state, devfs_async_t * async, int allow_callback);

//returns a pointer to the next place to write
void * ffifo_get_head(const ffifo_config_t * config, ffifo_state_t * state);
void * ffifo_get_tail(const ffifo_config_t * config, ffifo_state_t * state);

int ffifo_read_buffer(const ffifo_config_t * config, ffifo_state_t * state, char * buf, int len);
int ffifo_write_buffer(const ffifo_config_t * config, ffifo_state_t * state, const char * buf, int len);

char * ffifo_get_frame(const ffifo_config_t * config, u16 frame);

//helper functions for implementing FIFOs
void ffifo_flush(ffifo_state_t * state);
int ffifo_getinfo(ffifo_info_t * info, const ffifo_config_t * config, ffifo_state_t * state);

void ffifo_inc_head(ffifo_state_t * state, u16 count);
void ffifo_inc_tail(ffifo_state_t * state, u16 count);
int ffifo_is_write_ok(ffifo_state_t * state, u16 count, int writeblock);

int ffifo_is_writeblock(ffifo_state_t * state);
void ffifo_set_writeblock(ffifo_state_t * state, int value);

int ffifo_is_notify_read(ffifo_state_t * state);
void ffifo_set_notify_read(ffifo_state_t * state, int value);

int ffifo_is_notify_write(ffifo_state_t * state);
void ffifo_set_notify_write(ffifo_state_t * state, int value);

int ffifo_is_overflow(ffifo_state_t * state);
void ffifo_set_overflow(ffifo_state_t * state, int value);

void ffifo_data_transmitted(const ffifo_config_t * cfgp, ffifo_state_t * state);
void ffifo_data_received(const ffifo_config_t * cfgp, ffifo_state_t * state);

void ffifo_cancel_rop(ffifo_state_t * state);
void ffifo_cancel_wop(ffifo_state_t * state);


#define FFIFO_DEFINE_CONFIG(ffifo_frame_count, ffifo_frame_size, ffifo_buffer) .count = ffifo_frame_count, .frame_size = ffifo_frame_size, .buffer = ffifo_buffer

#define FFIFO_DECLARE_CONFIG_STATE(ffifo_name,\
    ffifo_frame_count, \
    ffifo_frame_size ) \
    ffifo_state_t ffifo_name##_state MCU_SYS_MEM; \
    char ffifo_name##_buffer[ffifo_frame_size * ffifo_frame_count]; \
    const ffifo_config_t ffifo_name##_config = { FFIFO_DEFINE_CONFIG(ffifo_frame_count, ffifo_frame_size, ffifo_name##_buffer) }


#endif /* DEVICE_FFIFO_H_ */

