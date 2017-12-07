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

/*! \addtogroup TIME
 * @{
 */

/*! \file */


#include "config.h"
#include <errno.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "mcu/arch.h"
#include "mcu/mcu.h"
#include "mcu/wdt.h"
#include "sos/dev/rtc.h"

#include "../scheduler/scheduler_local.h"

static int set_alarm(int seconds);
static void priv_powerdown(void * args) MCU_ROOT_EXEC_CODE;
static void priv_hibernate(void * args) MCU_ROOT_EXEC_CODE;

void priv_powerdown(void * args){
	mcu_core_execsleep(0, (void*)CORE_DEEPSLEEP_STANDBY);
}

void priv_hibernate(void * args){
	int * seconds = (int*)args;
	u16 save_priority;

	//set the WDT to reset in args seconds
	if( (sos_board_config.o_sys_flags & SYS_FLAG_IS_WDT_DISABLED) == 0 ){
		if( *seconds != 0 ){
			mcu_wdt_setinterval((*seconds+1)*2000);
		} else {
			//set WDT timer to one minute
			mcu_wdt_setinterval((60)*1000);
		}
	}

	//The WDT only runs in hibernate on certain clock sources
	mcu_wdt_priv_reset(NULL);

	//elevate task prio of caller so that nothing executes until prio is restored
	save_priority = sos_sched_table[ task_get_current() ].priority;
	sos_sched_table[ task_get_current() ].priority = SCHED_HIGHEST_PRIORITY+1;

	mcu_core_set_nvic_priority(SVCall_IRQn, mcu_config.irq_middle_prio*2-1); //allow interrrupts to wake from sleep
	mcu_core_execsleep(0, (void*)CORE_DEEPSLEEP);
	mcu_core_set_nvic_priority(SVCall_IRQn, mcu_config.irq_middle_prio-1); //restore SV call interrupt priority

	//restore task prio
	sos_sched_table[ task_get_current() ].priority = save_priority;


	//reinitialize the Clocks
	mcu_core_initclock(1); //Set the main clock
	mcu_core_setusbclock(mcu_board_config.core_osc_freq); //set the USB clock

	//Set WDT to previous value (it only runs in deep sleep with certain clock sources)
	mcu_wdt_priv_reset(NULL);

	if( (sos_board_config.o_sys_flags & SYS_FLAG_IS_WDT_DISABLED) == 0 ){
		if( *seconds != 0 ){
			mcu_wdt_setinterval(SCHED_RR_DURATION * 10 * sos_board_config.task_total + 5);
		}
	}
}

int set_alarm(int seconds){
	int fd;
	rtc_attr_t attr;
	int ret;

	fd = open("/dev/rtc", O_RDWR);
	if ( fd >= 0 ){
		attr.time.time_t = time(NULL);
		attr.time.time_t += seconds;
		gmtime_r((time_t*)&attr.time.time_t, (struct tm*)&attr.time.time);
		attr.o_flags = RTC_FLAG_IS_ALARM_ONCE | RTC_FLAG_ENABLE_ALARM;

		//set the alarm for "seconds" from now
		ret = ioctl(fd, I_RTC_SETATTR, &attr);
		close(fd);
		return ret;
	}
	return -1;
}

int hibernate(int seconds){
	if ( seconds > 0 ){
		set_alarm(seconds);
	}
	mcu_board_execute_event_handler(MCU_BOARD_CONFIG_EVENT_HIBERNATE, &seconds);
	cortexm_svcall(priv_hibernate, &seconds);
	mcu_board_execute_event_handler(MCU_BOARD_CONFIG_EVENT_WAKEUP_FROM_HIBERNATE, &seconds);
	return 0;
}


void powerdown(int seconds){
	if ( seconds > 0 ){
		set_alarm(seconds);
	}
	mcu_board_execute_event_handler(MCU_BOARD_CONFIG_EVENT_POWERDOWN, &seconds);
	cortexm_svcall(priv_powerdown, NULL);
	//device will reset after a powerdown event
}

/*! @} */
