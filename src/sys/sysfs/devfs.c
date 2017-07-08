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

#include <reent.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>
#include "sos/fs/sysfs.h"

#include "mcu/mcu.h"
#include "../unistd/unistd_fs.h"
#include "../unistd/unistd_flags.h"
#include "../sched/sched_flags.h"

#include "sos/fs/devfs.h"
#include "../unistd/unistd_flags.h"

typedef struct {
	int err;
	const devfs_device_t * device;
} priv_args_t;


static void priv_open_device(void * args) MCU_PRIV_EXEC_CODE;
static void priv_devfs_close(void * args) MCU_PRIV_EXEC_CODE;

typedef struct {
	int err;
	const void * cfg;
	void * handle;
	int request;
	void * ctl;
} ioctl_priv_t;
void ioctl_priv(void * args) MCU_PRIV_EXEC_CODE;

static int get_total(const devfs_device_t * list){
	int total;
	total = 0;
	while( list[total].driver.open != NULL ){
		total++;
	}
	return total;
}

static const devfs_device_t * load(const devfs_device_t * list, const char * device_name){
	int i;
	i = 0;
	while( devfs_is_terminator(&(list[i])) == false ){
		if ( strcmp(device_name, list[i].name) == 0 ){
			return &list[i];
		}
		i++;
	}
	return NULL;
}

/*
static int find(const device_t * list, const device_t * dev){
	int i;
	i = 0;
	while( list[i].name != NULL ){
		if (&list[i] == dev){
			return i;
		}
		i++;
	}
	return -1;
}
*/

void priv_open_device(void * args){
	priv_args_t * p = (priv_args_t*)args;
	p->err = p->device->driver.open( &(p->device->handle) );
}

int devfs_init(const void * cfg){
	//no initialization is required
	return 0;
}

int devfs_opendir(const void * cfg, void ** handle, const char * path){
	const devfs_device_t * dev;
	const devfs_device_t * list = (const devfs_device_t*)cfg;

	if ( strcmp(path, "") != 0 ){
		//there is only one valid folder (the top)
			dev = load(list, path);
		if ( dev == NULL ){
			errno = ENOENT;
		} else {
			errno = ENOTDIR;
		}
		return -1;
	}
	*handle = NULL;
	return 0;
}

int devfs_readdir_r(const void * cfg, void * handle, int loc, struct dirent * entry){
	int total;
	const devfs_device_t * dev_list = (const devfs_device_t*)cfg; //the cfg value is the device list
	//this populates the entry for the loc position
	total = get_total(dev_list);
	if ( (uint32_t)loc < total ){
		strcpy(entry->d_name, dev_list[loc].name);
		entry->d_ino = loc;
		return 0;
	}
	errno = ENOENT;
	return -1;
}

int devfs_closedir(const void * cfg, void ** handle){
	//This doesn't need to do anything
	return 0;
}

int devfs_open(const void * cfg, void ** handle, const char * path, int flags, int mode){
	priv_args_t args;
	const devfs_device_t * list = (const devfs_device_t*)cfg;

	//check the flags O_CREAT, O_APPEND, O_TRUNC are not supported
	if ( (flags & O_APPEND) | (flags & O_CREAT) | (flags & O_TRUNC) ){
		errno = ENOTSUP;
		return -1;
	}


	//Check to see if the device is in the list
	args.device = load(list, path);
	if ( args.device != NULL ){
		mcu_core_privcall(priv_open_device, &args);
		if ( args.err < 0 ){
			return args.err;
		}
		*handle = (void*)args.device;
		return 0;
	}
	errno = ENOENT;
	return -1;
}

int devfs_fstat(const void * cfg, void * handle, struct stat * st){
	int num;
	const devfs_device_t * dev = handle;
	//populate the characteristics
	//num = find(list, dev);
	num = 0;
	st->st_dev = 0;
	st->st_rdev = num;
	st->st_ino = num;
	st->st_size = 0;
	st->st_uid = dev->uid;
	st->st_mode = dev->mode;
	return 0;
}

int devfs_stat(const void * cfg, const char * path, struct stat * st){
	//populate the characteristics of the device
	const devfs_device_t * list = (const devfs_device_t*)cfg;
	const devfs_device_t * dev;

	dev = load(list, path);
	if ( dev == NULL ){
		errno = ENOENT;
		return -1;
	}
	return devfs_fstat(cfg, (void*)dev, st);
}

int devfs_read_async(const void * cfg, void * handle, devfs_async_t * op){
	const devfs_device_t * dev = (const devfs_device_t*)(handle);
	return dev->driver.read(&dev->handle, op);
}

int devfs_write_async(const void * cfg, void * handle, devfs_async_t * op){
	const devfs_device_t * dev = (const devfs_device_t*)handle;
	return dev->driver.write(&dev->handle, op);
}

int devfs_ioctl(const void * cfg, void * handle, int request, void * ctl){
	ioctl_priv_t args;
	args.cfg = cfg;
	args.handle = handle;
	args.request = request;
	args.ctl = ctl;
	args.err = 0;
	mcu_cortexm_svcall(ioctl_priv, &args);
	return args.err;
}

void ioctl_priv(void * args){
	ioctl_priv_t * p = (ioctl_priv_t*)args;
	const devfs_device_t * dev = (const devfs_device_t*)p->handle;
	p->err = dev->driver.ioctl(&dev->handle, p->request, p->ctl);
}

void priv_devfs_close(void * args){
	priv_args_t * p = (priv_args_t*)args;
	p->err = p->device->driver.close(&p->device->handle);
}

int devfs_close(const void * cfg, void ** handle){
	priv_args_t args;
	args.err = 0;
	args.device = *handle;
	mcu_core_privcall(priv_devfs_close, &args);
	*handle = NULL;
	return args.err;
}

