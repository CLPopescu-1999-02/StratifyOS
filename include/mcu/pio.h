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

/*! \addtogroup PIO_DEV PIO
 *
 * \ingroup DEV
 *
 */

#ifndef _MCU_PIO_H_
#define _MCU_PIO_H_

#include "core.h"
#include "sos/dev/pio.h"
#include "sos/fs/devfs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MCU_PACK {
	u32 status;
	u32 rising;
	u32 falling;
} pio_event_data_t;

int mcu_pio_open(const devfs_handle_t * cfg) MCU_PRIV_CODE;
int mcu_pio_read(const devfs_handle_t * cfg, devfs_async_t * rop) MCU_PRIV_CODE;
int mcu_pio_write(const devfs_handle_t * cfg, devfs_async_t * wop) MCU_PRIV_CODE;
int mcu_pio_ioctl(const devfs_handle_t * cfg, int request, void * ctl) MCU_PRIV_CODE;
int mcu_pio_close(const devfs_handle_t * cfg) MCU_PRIV_CODE;

int mcu_pio_getinfo(int port, void * ctl) MCU_PRIV_CODE;
int mcu_pio_setattr(int port, void * ctl) MCU_PRIV_CODE;
int mcu_pio_setaction(int port, void * ctl) MCU_PRIV_CODE;

int mcu_pio_setmask(int port, void * ctl) MCU_PRIV_CODE;
int mcu_pio_clrmask(int port, void * ctl) MCU_PRIV_CODE;
int mcu_pio_get(int port, void * ctl) MCU_PRIV_CODE;
int mcu_pio_set(int port, void * ctl) MCU_PRIV_CODE;


#ifndef __link

static inline int _mcu_i2c_cfg_pio(int port, int pio_port, int scl_pin, int sda_pin) MCU_ALWAYS_INLINE MCU_PRIV_CODE;
int _mcu_i2c_cfg_pio(int port, int pio_port, int scl_pin, int sda_pin){
	if ( _mcu_core_set_pinsel_func(pio_port, sda_pin, CORE_PERIPH_I2C, port) ) return -1;  //Use pins for I2C
	if ( _mcu_core_set_pinsel_func(pio_port, scl_pin, CORE_PERIPH_I2C, port) ) return -1;  //Use pins for I2C
	return 0;
}


static inline int _mcu_uart_cfg_pio(int port, int pio_port, int rx_pin, int tx_pin) MCU_ALWAYS_INLINE MCU_PRIV_CODE;
int _mcu_uart_cfg_pio(int port, int pio_port, int rx_pin, int tx_pin){
	if ( _mcu_core_set_pinsel_func(pio_port, rx_pin, CORE_PERIPH_UART, port) ) return -1;  //Use pin for UART
	if ( _mcu_core_set_pinsel_func(pio_port, tx_pin, CORE_PERIPH_UART, port) ) return -1;  //Use pin for UART
	return 0;
}

static inline int _mcu_spi_cfg_pio(int port, int pio_port, int mosi_pin, int miso_pin, int sck_pin) MCU_ALWAYS_INLINE MCU_PRIV_CODE;
int _mcu_spi_cfg_pio(int port, int pio_port, int mosi_pin, int miso_pin, int sck_pin){
	if ( _mcu_core_set_pinsel_func(pio_port, mosi_pin, CORE_PERIPH_SPI, port) ) return -1;  //Use pin for SPI
	if ( _mcu_core_set_pinsel_func(pio_port, miso_pin, CORE_PERIPH_SPI, port) ) return -1;  //Use pin for SPI
	if ( _mcu_core_set_pinsel_func(pio_port, sck_pin, CORE_PERIPH_SPI, port) ) return -1;  //Use pin for SPI
	return 0;
}

static inline int _mcu_ssp_cfg_pio(int port, int pio_port, int mosi_pin, int miso_pin, int sck_pin) MCU_ALWAYS_INLINE MCU_PRIV_CODE;
int _mcu_ssp_cfg_pio(int port, int pio_port, int mosi_pin, int miso_pin, int sck_pin){
	if ( _mcu_core_set_pinsel_func(pio_port, mosi_pin, CORE_PERIPH_SSP, port) ) return -1;  //Use pin for SPI
	if ( _mcu_core_set_pinsel_func(pio_port, miso_pin, CORE_PERIPH_SSP, port) ) return -1;  //Use pin for SPI
	if ( _mcu_core_set_pinsel_func(pio_port, sck_pin, CORE_PERIPH_SSP, port) ) return -1;  //Use pin for SPI
	return 0;
}

static inline int _mcu_usb_cfg_pio(int usb_port,
		int data_pio_port,
		int datap_pin,
		int datam_pin,
		int vbus_pio_port,
		int vbus_pio_pin) MCU_ALWAYS_INLINE MCU_PRIV_CODE;

int _mcu_usb_cfg_pio(int port,
		int data_pio_port,
		int datap_pin,
		int datam_pin,
		int vbus_pio_port,
		int vbus_pio_pin){
	if ( _mcu_core_set_pinsel_func(data_pio_port, datap_pin, CORE_PERIPH_USB, port) ) return -1;  //Use pin for USB
	if ( _mcu_core_set_pinsel_func(data_pio_port, datam_pin, CORE_PERIPH_USB, port) ) return -1;  //Use pin for USB
	if ( _mcu_core_set_pinsel_func(vbus_pio_port, vbus_pio_pin, CORE_PERIPH_USB, port) ) return -1;  //Use pin for USB
	return 0;
}

#endif

#ifdef __cplusplus
}
#endif



#endif /* _MCU_PIO_H_ */

/*! @} */
