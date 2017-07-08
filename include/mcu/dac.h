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

/*! \addtogroup DAC_DEV DAC
 * @{
 * \ingroup DEV
 */

#ifndef _MCU_DAC_H_
#define _MCU_DAC_H_

#include "sos/dev/dac.h"

#include "sos/fs/devfs.h"

#ifdef __cplusplus
extern "C" {
#endif

int mcu_dac_open(const devfs_handle_t * cfg) MCU_PRIV_CODE;
int mcu_dac_read(const devfs_handle_t * cfg, devfs_async_t * rop) MCU_PRIV_CODE;
int mcu_dac_write(const devfs_handle_t * cfg, devfs_async_t * wop) MCU_PRIV_CODE;
int mcu_dac_ioctl(const devfs_handle_t * cfg, int request, void * ctl) MCU_PRIV_CODE;
int mcu_dac_close(const devfs_handle_t * cfg) MCU_PRIV_CODE;

int mcu_dac_getinfo(int port, void * ctl) MCU_PRIV_CODE;
int mcu_dac_setattr(int port, void * ctl) MCU_PRIV_CODE;
int mcu_dac_setaction(int port, void * ctl) MCU_PRIV_CODE;
int mcu_dac_get(int port, void * ctl) MCU_PRIV_CODE;
int mcu_dac_set(int port, void * ctl) MCU_PRIV_CODE;

//DMA version of the driver
int mcu_dac_dma_open(const devfs_handle_t * cfg) MCU_PRIV_CODE;
int mcu_dac_dma_read(const devfs_handle_t * cfg, devfs_async_t * rop) MCU_PRIV_CODE;
int mcu_dac_dma_write(const devfs_handle_t * cfg, devfs_async_t * wop) MCU_PRIV_CODE;
int mcu_dac_dma_ioctl(const devfs_handle_t * cfg, int request, void * ctl) MCU_PRIV_CODE;
int mcu_dac_dma_close(const devfs_handle_t * cfg) MCU_PRIV_CODE;

int mcu_dac_dma_setattr(int port, void * ctl) MCU_PRIV_CODE;
int mcu_dac_dma_setaction(int port, void * ctl) MCU_PRIV_CODE;


#ifdef __cplusplus
}
#endif


#endif // _MCU_DAC_H_

/*! @} */


