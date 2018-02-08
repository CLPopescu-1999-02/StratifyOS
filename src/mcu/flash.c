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

#include "sos/fs/devfs.h"
#include "mcu/flash.h"

DEVFS_MCU_DRIVER_IOCTL_FUNCTION(flash, FLASH_VERSION, I_MCU_TOTAL + I_FLASH_TOTAL, mcu_flash_eraseaddr, mcu_flash_erasepage, mcu_flash_getpage, mcu_flash_getsize, mcu_flash_getpageinfo, mcu_flash_writepage)
