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

#ifndef APPFS_LOCAL_H_
#define APPFS_LOCAL_H_

#include "config.h"
#include <stdbool.h>
#include "appfs.h"
#include "sos/dev/mem.h"
#include "mcu/debug.h"
#include "sos/link/link.h"
#include "sos/fs/sysfs.h"


typedef struct {
	u32 code_start /*! the new value */;
	u32 code_size;
	u32 data_start /*! the new data start */;
	u32 data_size;
	u32 rewrite_mask;
	u32 kernel_symbols_total;
} appfs_util_handle_t;

#define APPFS_MEMPAGETYPE_FREE 0
#define APPFS_MEMPAGETYPE_SYS 1
#define APPFS_MEMPAGETYPE_USER 2
#define APPFS_MEMPAGETYPE_MASK 0x03

typedef struct {
	int beg_addr /*! the address of the beginning of the file */;
	int size /*! the current size of the file */;
	int page /*! the start page of the open file */;
	int type /*! the type of memory (flash or RAM) */;
	u32 mode /*! \brief File mode 0444 for data and 0555 for executables */;
} appfs_reg_handle_t;


typedef struct {
	u8 is_install /*! boolean for the .install file */;
	union {
		appfs_util_handle_t install;
		appfs_reg_handle_t reg;
	} type;
} appfs_handle_t;

typedef struct {
	int ret;
	const devfs_device_t * dev;
	mem_pageinfo_t pageinfo;
	appfs_file_t fileinfo;
} priv_load_fileinfo_t;


//page utilities
int appfs_util_getpagetype(appfs_hdr_t * info, int page, int type);
int appfs_util_getflashpagetype(appfs_hdr_t * info);

//file utilities
int appfs_util_lookupname(const void * cfg, const char * path, priv_load_fileinfo_t * args, int type, int * size);

//memory access utilities
void appfs_util_priv_closeinstall(void * args) MCU_PRIV_EXEC_CODE;
int appfs_util_priv_writeinstall(const devfs_device_t * dev, appfs_handle_t * h, appfs_installattr_t * attr) MCU_PRIV_EXEC_CODE;
int appfs_util_priv_create(const devfs_device_t * dev, appfs_handle_t * h, appfs_installattr_t * attr) MCU_PRIV_EXEC_CODE;
int appfs_util_priv_free_ram(const devfs_device_t * dev, appfs_handle_t * h) MCU_PRIV_EXEC_CODE;
int appfs_util_priv_reclaim_ram(const devfs_device_t * dev, appfs_handle_t * h) MCU_PRIV_EXEC_CODE;
int appfs_util_getfileinfo(priv_load_fileinfo_t * info, const devfs_device_t * dev, int page, int type, int * size);
int appfs_util_erasepages(const devfs_device_t * dev, int start_page, int end_page);
int appfs_util_getpageinfo(const devfs_device_t * dev, mem_pageinfo_t * pageinfo);

//ram access
#define APPFS_RAM_PAGES ((DEVICE_MAX_RAM-DEVICE_SYS_RAM)/DEVICE_RAM_PAGE_SIZE)
#define APPFS_RAM_USAGE_BITS_SIZE (APPFS_RAM_PAGES*2) //size of usage in bits (2 bits per page)
#define APPFS_RAM_USAGE_WORDS (APPFS_RAM_USAGE_BITS_SIZE/16)
#define APPFS_RAM_USAGE_BYTES (APPFS_RAM_USAGE_WORDS*4)

int appfs_ram_getusage(int page);
int appfs_ram_setusage(int page, int size, int type);
int appfs_priv_ram_setusage(int page, int size, int type) MCU_PRIV_EXEC_CODE;
void appfs_ram_priv_saveusage(void * args) MCU_PRIV_EXEC_CODE;
void appfs_ram_setrange(u32 * buf, int page, int size, int usage);
extern u32 appfs_ram_usagetable[APPFS_RAM_USAGE_WORDS];






#endif /* APPFS_LOCAL_H_ */
