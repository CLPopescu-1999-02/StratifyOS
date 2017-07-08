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


#ifndef SFFS_DEV_H_
#define SFFS_DEV_H_

#include <pthread.h>

#include "sos/fs/sffs.h"

int sffs_dev_open(const void * cfg);
int sffs_dev_write(const void * cfg, int loc, const void * buf, int nbyte);
int sffs_dev_read(const void * cfg, int loc, void * buf, int nbyte);
int sffs_dev_close(const void * cfg);

static inline int sffs_dev_getsize(const void * cfg){
	const sffs_cfg_t * cfgp = cfg;
	return cfgp->state->dattr.num_write_blocks * cfgp->state->dattr.write_block_size;
}


static inline int sffs_dev_geterasesize(const void * cfg){
	const sffs_cfg_t * cfgp = cfg;
	return cfgp->state->dattr.erase_block_size;
}

static inline int sffs_dev_getblocksize(const void * cfg){
	const sffs_cfg_t * cfgp = cfg;
	return cfgp->state->dattr.write_block_size;
}

int sffs_dev_erase(const void * cfg);
int sffs_dev_erasesection(const void * cfg, int loc);

int sysfs_access(int file_mode, int file_uid, int file_gid, int amode);
int sffs_sys_geteuid();
int sffs_sys_getegid();

int sffs_dev_getlist_block(const void * cfg);
void sffs_dev_setlist_block(const void * cfg, int list_block);
int sffs_dev_getserialno(const void * cfg);
void sffs_dev_setserialno(const void * cfg, int serialno);

void sffs_dev_setdelay_mutex(pthread_mutex_t * mutex);

#endif /* SFFS_DEV_H_ */
