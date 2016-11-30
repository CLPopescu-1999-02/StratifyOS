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

#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include "mcu/debug.h"
#include "iface/dev/fifo.h"
#include "dev/fifo.h"


void fifo_inc_tail(fifo_state_t * state, int size){
	state->tail++;
	if ( state->tail == size ){
		state->tail = 0;
	}
}

void fifo_inc_head(fifo_state_t * state, int size){
	state->head++;
	if ( state->head == size ){
		state->head = 0;
	}

	if ( state->head == state->tail ){
		state->tail++;
		if ( state->tail == size ){
			state->tail = 0;
		}
		fifo_set_overflow(state, 1);
	}
}

int fifo_is_write_ok(fifo_state_t * state, u16 size, int writeblock){
	int ret = 1;
	if( ((state->tail == 0) && (state->head == size -1)) ||
			((state->head) + 1 == state->tail)
	){
		if( writeblock ){
			//cannot write anymore data at this time
			ret = 0;
		} else {
			//OK to write but it will cause an overflow
			fifo_inc_tail(state, size);
			fifo_set_overflow(state, 1);
		}
	}

	//return OK or not to write 1 = OK, 0 = write is blocked
	return ret;
}

int fifo_is_writeblock(fifo_state_t * state){
	return ((state->o_flags & FIFO_FLAGS_WRITEBLOCK) != 0);
}

void fifo_set_writeblock(fifo_state_t * state, int value){
	if( value ){
		state->o_flags |= FIFO_FLAGS_WRITEBLOCK;
	} else {
		state->o_flags &= ~FIFO_FLAGS_WRITEBLOCK;
	}
}

int fifo_is_overflow(fifo_state_t * state){
	return ((state->o_flags & FIFO_FLAGS_OVERFLOW) != 0);
}

void fifo_set_overflow(fifo_state_t * state, int value){
	if( value ){
		state->o_flags |= FIFO_FLAGS_OVERFLOW;
	} else {
		state->o_flags &= ~FIFO_FLAGS_OVERFLOW;
	}
}

int fifo_read_buffer(const fifo_cfg_t * cfgp, fifo_state_t * state, char * buf){
	int i;
	u16 size = cfgp->size;
	for(i=0; i < state->rop_len; i++){
		if ( state->head == state->tail ){ //check for data in the fifo buffer
			//there is no more data in the buffer
			break;
		} else {
			buf[i] = cfgp->buffer[state->tail];
			fifo_inc_tail(state, size);
		}
	}
	return i; //number of bytes read
}


int fifo_write_buffer(const fifo_cfg_t * cfgp, fifo_state_t * state, const char * buf){
	int i;
	int size = cfgp->size;
	int writeblock = fifo_is_writeblock(state);
	for(i=0; i < state->wop_len; i++){
		if( fifo_is_write_ok(state, size, writeblock) ){
			cfgp->buffer[state->head] = buf[i];
			fifo_inc_head(state, size);
		} else {
			break;
		}
	}
	return i; //number of bytes written
}

void fifo_flush(fifo_state_t * state){
	state->head = 0;
	state->tail = 0;
	fifo_set_overflow(state, 0);
}

void fifo_getattr(fifo_attr_t * attr, const fifo_cfg_t * cfgp, fifo_state_t * state){
	attr->size = cfgp->size - 1;
	if( state->head >= state->tail ){
		attr->used = state->head - state->tail;
	} else {
		attr->used = cfgp->size - state->tail + state->head;
	}
	attr->overflow = fifo_is_overflow(state);
	fifo_set_overflow(state, 0);
}

void fifo_data_received(const fifo_cfg_t * cfgp, fifo_state_t * state){
	int bytes_read;

	if( state->rop != NULL ){
		state->rop->nbyte = state->rop_len; //update the number of bytes read??
		if( (bytes_read = fifo_read_buffer(cfgp, state, state->rop->buf)) > 0 ){
			state->rop->nbyte = bytes_read;
			if ( state->rop->callback(state->rop->context, (mcu_event_t)NULL) == 0 ){
				state->rop = NULL;
			}
		}
	}
}

void fifo_cancel_rop(fifo_state_t * state){
	if( state->rop != NULL ){
		state->rop->nbyte = -1;
		if ( state->rop->callback(state->rop->context, MCU_EVENT_SET_CODE(MCU_EVENT_OP_CANCELLED)) == 0 ){
			state->rop = NULL;
		}
	}
}

static int data_transmitted(const device_cfg_t * cfg){
	int bytes_written;
	const fifo_cfg_t * cfgp = cfg->dcfg;
	fifo_state_t * state = cfg->state;

	if( state->wop != NULL ){
		if( (bytes_written = fifo_write_buffer(cfgp, state, state->wop->cbuf)) > 0 ){
			state->wop->nbyte = bytes_written;
			if ( state->wop->callback(state->wop->context, NULL) == 0 ){
				state->wop = NULL;
			}
		}
	}

	return 1; //leave the callback in place
}




int fifo_open(const device_cfg_t * cfg){
	//const fifo_cfg_t * cfgp = cfg->dcfg;
	//fifo_state_t * state = cfg->state;
	return 0;
}

int fifo_ioctl(const device_cfg_t * cfg, int request, void * ctl){
	fifo_attr_t * attr = ctl;
	const fifo_cfg_t * cfgp = cfg->dcfg;
	fifo_state_t * state = cfg->state;
	switch(request){
	case I_FIFO_GETATTR:
		fifo_getattr(attr, cfgp, state);
		return 0;
	case I_FIFO_INIT:
		state->rop = NULL;
		state->wop = NULL;
		/* no break */
	case I_FIFO_FLUSH:
		fifo_flush(state);
		data_transmitted(cfg); //something might be waiting to write the fifo
		return 0;
	case I_FIFO_SETWRITEBLOCK:

		fifo_set_writeblock(state, (int)ctl);
		if( fifo_is_writeblock(state) ){
			//make sure the FIFO is not currently blocked
			data_transmitted(cfg);
		}
		return 0;
	}
	errno = EINVAL;
	return -1;
}


int fifo_read_local(const fifo_cfg_t * cfgp, fifo_state_t * state, device_transfer_t * rop){
	int bytes_read;

	if ( state->rop != NULL ){
		errno = EAGAIN; //the device is temporarily unavailable
		return -1;
	}

	state->rop_len = rop->nbyte;
	bytes_read = fifo_read_buffer(cfgp, state, rop->buf); //see if there are bytes in the buffer
	if ( bytes_read == 0 ){
		if( (rop->flags & O_NONBLOCK) ){
			errno = EAGAIN;
			bytes_read = -1;
		} else {
			state->rop = rop;
			state->rop_len = rop->nbyte; //total number of bytes to read
			rop->nbyte = 0; //number of bytes read so far
		}
	}

	return bytes_read;
}

int fifo_read(const device_cfg_t * cfg, device_transfer_t * rop){
	const fifo_cfg_t * cfgp = cfg->dcfg;
	fifo_state_t * state = cfg->state;
	int bytes_read;

	if ( state->rop != NULL ){
		rop->nbyte = 0;
		return 0; //caller will block until data is read
	}

	bytes_read = fifo_read_local(cfgp, state, rop);
	if( bytes_read > 0 ){
		//see if anything needs to write the FIFO
		data_transmitted(cfg);
	}

	if( cfgp->notify_on_read ){
		cfgp->notify_on_read(bytes_read);
	}

	return bytes_read;
}

int fifo_write_local(const fifo_cfg_t * cfgp, fifo_state_t * state, device_transfer_t * wop){
	int bytes_written;

	if ( state->wop != NULL ){
		wop->nbyte = 0;
		return 0; //caller will block until FIFO is ready to write
	}

	state->wop_len = wop->nbyte;
	bytes_written = fifo_write_buffer(cfgp, state, wop->cbuf); //see if there are bytes in the buffer
	if ( bytes_written == 0 ){
		if( wop->flags & O_NONBLOCK ){
			errno = EAGAIN;
			bytes_written = -1;
		} else {
			state->wop = wop;
			state->wop_len = wop->nbyte;
			wop->nbyte = 0;
		}
	}

	if( cfgp->notify_on_write ){
		cfgp->notify_on_write(bytes_written);
	}

	return bytes_written;
}

int fifo_write(const device_cfg_t * cfg, device_transfer_t * wop){
	const fifo_cfg_t * cfgp = cfg->dcfg;
	fifo_state_t * state = cfg->state;
	int bytes_written;

	bytes_written = fifo_write_local(cfgp, state, wop);
	if( bytes_written > 0 ){
		fifo_data_received(cfgp, state);
	}

	return bytes_written;
}

int fifo_close(const device_cfg_t * cfg){
	return 0;
}

