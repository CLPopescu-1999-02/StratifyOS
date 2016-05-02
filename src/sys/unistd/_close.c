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

/*! \addtogroup UNI_FILE_ACCESS
 * @{
 */

/*! \file
 */

#include <stratify/sysfs.h>
#include "mcu/mcu.h"
#include "unistd_fs.h"
#include "unistd_flags.h"
#include "mcu/debug.h"

/*! \details This function closes the file associated
 * with the specified descriptor.
 * \param fildes The File descriptor \a fildes.
 * \return Zero on success or -1 on error with errno (see \ref ERRNO) set to:
 *  - EBADF:  Invalid file descriptor
 */
int close(int fildes);

int _close(int fildes) {
	//Close the file if it's open
	int ret;
	void ** handle_ptr;
	const sysfs_t * fs;

	fildes = u_fildes_is_bad(fildes);
	if ( fildes < 0 ){
		return -1;
	}

	handle_ptr = get_handle_ptr(fildes);
	fs = get_fs(fildes);
	ret = fs->close(fs->cfg, handle_ptr);
	u_reset_fildes(fildes);
	return ret;
}


/*! @} */

