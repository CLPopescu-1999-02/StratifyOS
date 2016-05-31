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

/*! \addtogroup STFY
 * @{
 */

/*! \file */

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stratify/stratify.h>
#include <stratify/sysfs.h>
#include "config.h"
#include "mcu/types.h"
#include "mcu/debug.h"
#include "mcu/pio.h"
#include "stratify/link.h"

static int init_fs();
static int startup_fs();

static void priv_check_reset_source(void * args) MCU_PRIV_EXEC_CODE;

void priv_check_reset_source(void * args){
	u8 * src = args;
	core_attr_t attr;
	mcu_core_getattr(0, &attr);
	*src = attr.reset_type;
	switch(attr.reset_type){
	case CORE_RESET_SRC_WDT:
		mcu_priv_debug("wdt rst\n");
		break;
	case CORE_RESET_SRC_POR:
		mcu_priv_debug("por rst\n");
		break;
	case CORE_RESET_SRC_BOR:
		mcu_priv_debug("bor rst\n");
		break;
	case CORE_RESET_SRC_EXTERNAL:
		mcu_priv_debug("ext rst\n");
		break;
	}
}

void check_reset_source(void){
	u8 src;
	mcu_core_privcall(priv_check_reset_source, &src);
	mcu_event(MCU_BOARD_CONFIG_EVENT_INIT, 0);
}

void start_filesystem(void){
	u32 started;
	started = startup_fs();
	mcu_event(MCU_BOARD_CONFIG_EVENT_START_FILESYSTEM, &started);
}

void * initial_thread(void * arg){

	check_reset_source();

	//Initialize the file systems
	if ( init_fs() < 0 ){
		mcu_event(MCU_BOARD_CONFIG_EVENT_ERROR, (void*)"init_fs");
	}

	start_filesystem();

	open("/dev/rtc", O_RDWR);

	mcu_event(MCU_BOARD_CONFIG_EVENT_START_LINK, 0);

	link_update(stfy_board_config.link_transport); 	//Run the link update thread--never returns

	return NULL;
}


int init_fs(){
	int i;
	i = 0;
	while( sysfs_isterminator(&sysfs_list[i]) == false ){
		mcu_debug("init %s\n", sysfs_list[i].mount_path);
		if ( sysfs_list[i].mount( sysfs_list[i].cfg ) < 0 ){
			mcu_debug("failed to init\n");
		}
		i++;
	}
	return 0;
}

int startup_fs(){
	int i;
	i = 0;
	int started;
	int ret;
	started = 0;
	while( sysfs_isterminator(&sysfs_list[i]) == false ){
		ret = sysfs_list[i].startup( sysfs_list[i].cfg );
		if ( ret > 0){
			started += ret;
		}
		i++;
	}
	//the number of processes that have been started
	return started;
}


