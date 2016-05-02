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

/*! \addtogroup UNI_FS
 * @{
 */

/*! \file */

#include  "unistd_fs.h"

int _unlink(const char * name){
	const sysfs_t * fs;

	if ( sysfs_ispathinvalid(name) == true ){
		return -1;
	}

	fs = sysfs_find(name, true);
	if ( fs != NULL ){
		return fs->unlink(fs->cfg,
				sysfs_stripmountpath(fs, name)
				);
	}
	errno = ENOENT;
	return -1;
}

/*! @} */

