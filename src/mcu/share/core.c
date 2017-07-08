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

#include "local.h"
#include "mcu/core.h"

//These functions are device specific
extern void _mcu_core_dev_power_on(int port) MCU_PRIV_CODE;
extern void _mcu_core_dev_power_off(int port) MCU_PRIV_CODE;
extern int _mcu_core_dev_powered_on(int port) MCU_PRIV_CODE;

int (* const _mcu_core_ioctl_func_table[I_MCU_TOTAL + I_CORE_TOTAL])(int, void*) = {
		mcu_core_getinfo,
		mcu_core_setattr,
		mcu_core_setaction,
		mcu_core_setpinfunc,
		mcu_core_sleep,
		mcu_core_reset,
		mcu_core_invokebootloader,
		mcu_core_setclkout,
		mcu_core_setclkdivide,
		mcu_core_getmcuboardconfig
};


int mcu_core_open(const devfs_handle_t * cfg){
	return mcu_open(cfg,
			_mcu_core_dev_powered_on,
			_mcu_core_dev_power_on);
}

int mcu_core_ioctl(const devfs_handle_t * cfg, int request, void * ctl){
	return mcu_ioctl(cfg,
			request,
			ctl,
			_mcu_core_dev_powered_on,
			_mcu_core_ioctl_func_table,
			I_MCU_TOTAL + I_CORE_TOTAL);
}



int mcu_core_read(const devfs_handle_t * cfg, devfs_async_t * rop){
	errno = ENOTSUP;
	return -1;
}


int mcu_core_write(const devfs_handle_t * cfg, devfs_async_t * wop){
	errno = ENOTSUP;
	return -1;
}

int mcu_core_close(const devfs_handle_t * cfg){
	return mcu_close(cfg, _mcu_core_dev_powered_on, _mcu_core_dev_power_off);
}


