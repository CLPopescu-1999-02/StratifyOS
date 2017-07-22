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


#if !defined __link

#include "mcu/debug.h"
#include "sos/dev/uart.h"
#include "mcu/uart.h"
#include "mcu/core.h"
#include "mcu/mcu.h"


#if MCU_DEBUG

char mcu_debug_buffer[MCU_DEBUG_BUFFER_SIZE];

int mcu_debug_init(){
	devfs_handle_t handle;

	//Open the debugging UART
	handle.port = mcu_board_config.debug_uart_port;
	if( mcu_uart_open(&handle) < 0 ){
		return -1;
	}

	/*
	memset(&attr.pin_assignment, 0xff, sizeof(uart_pin_assignment_t));
	attr.pin_assignment.rx = mcu_board_config.debug_uart_pin_assignment.rx;
	attr.pin_assignment.tx = mcu_board_config.debug_uart_pin_assignment.tx;
	attr.freq = 115200;
	attr.o_flags = UART_FLAG_IS_PARITY_NONE | UART_FLAG_IS_STOP1;
	attr.width = 8;
	*/

	return mcu_uart_setattr(handle.port, (void*)&mcu_board_config.debug_uart_attr);
}


void mcu_priv_write_debug_uart(void * args){
	int nbyte;
	int i;
	nbyte = strnlen(mcu_debug_buffer, MCU_DEBUG_BUFFER_SIZE);

	for(i=0; i < nbyte; i++){
		mcu_uart_put(mcu_board_config.debug_uart_port, (void*)(u32)mcu_debug_buffer[i]);
	}

}

#endif

#endif






