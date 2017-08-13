/* Copyright 2011-2017 Tyler Gilbert;
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
#include "sos/fs/sysfs.h"

static void update_loc(sysfs_file_t * file, int adjust);

int sysfs_file_open(sysfs_file_t * file, const char * name, int mode){
	int ret;
	struct stat st;
	const sysfs_t * fs = file->fs;
	ret = fs->open(
			fs->config,
			&(file->handle),
			name,
			file->flags,
			mode);

	if( ret < 0 ){
		return -1;
	}

	if( fs->fstat(fs->config, file->handle, &st) == 0 ){
		if( st.st_mode & S_IFCHR ){
			file->flags |= O_CHAR;
		}
	}

	return ret;
}

int sysfs_file_read(sysfs_file_t * file, void * buf, int nbyte){
	const sysfs_t * fs = file->fs;
	int bytes;
	bytes = fs->read(fs->config, file->handle, file->flags, file->loc, buf, nbyte);
	update_loc(file, bytes);
	return bytes;
}

int sysfs_file_write(sysfs_file_t * file, const void * buf, int nbyte){
	const sysfs_t * fs = file->fs;
	int bytes;
	bytes = fs->write(fs->config, file->handle, file->flags, file->loc, buf, nbyte);
	update_loc(file, bytes);
	return bytes;
}


int sysfs_file_close(sysfs_file_t * file){
	const sysfs_t * fs = file->fs;
	return fs->close(fs->config, &file->handle);
}

int sysfs_file_ioctl(sysfs_file_t * file, int request, void * ctl){
	const sysfs_t * fs = file->fs;
	return fs->ioctl(fs->config, file->handle, request, ctl);
}

void update_loc(sysfs_file_t * file, int adjust){
	//if not a char device, increment the location
	if ( (adjust > 0) && ((file->flags & O_CHAR) == 0) ){
		file->loc += adjust;
	}
}


