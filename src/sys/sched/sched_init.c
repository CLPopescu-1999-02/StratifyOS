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

/*! \addtogroup SCHED
 * @{
 *
 */

/*! \file */
#include "config.h"
#include "sched_flags.h"

#include "mcu/mcu.h"
#include "mcu/core.h"
#include "mcu/debug.h"

volatile sched_fault_t sched_fault MCU_SYS_MEM;

#if SINGLE_TASK != 0
static void * (*start_function)(void*) MCU_SYS_MEM;
static int start_single();
#endif

/*! \details This function initializes the peripheral hardware needed
 * by the scheduler specifically the microsecond timer.
 * \return Zero on success or an error code (see \ref caoslib_err_t)
 */
int sched_init(){

#if SINGLE_TASK == 0
	task_total = sos_board_config.task_total;
	task_table = sos_task_table;

	sched_current_priority = SCHED_LOWEST_PRIORITY - 1;
	sched_status_changed = 0;

	memset((void*)task_table, 0, sizeof(task_t) * task_total);
	memset((void*)sos_sched_table, 0, sizeof(sched_task_t) * task_total);

	//Do basic init of task 0 so that memory allocation can happen before the scheduler starts
	task_table[0].reent = _impure_ptr;
	task_table[0].global_reent = _global_impure_ptr;
#endif

	return 0;
}

/* \details This function initializes the scheduler.  It should be
 * called after all peripherals are initialized and interrupts are on.
 * \return Zero on success or an error code
 */
int sched_start(void * (*init)(void*), int priority){

#if SINGLE_TASK == 0

	sos_sched_table[0].init = init;
	sos_sched_table[0].priority = priority;
	sos_sched_table[0].attr.stackaddr = &_data;
	sos_sched_table[0].attr.stacksize = sos_board_config.sys_memory_size;

	//Start the scheduler in a new thread
	if ( task_init(SCHED_RR_DURATION,
			scheduler, //run the scheduler
			NULL, //Let the task init function figure out where the stack needs to be and the heap size
			sos_board_config.sys_memory_size)
	){
		return -1;
	}
#else

	start_function = init;
	//Enter thread mode
	task_init_single(start_single,
			NULL,
			sos_board_config.sys_memory_size);
#endif
	//Program never gets to this point
	return -1;
}


int sched_prepare(){

	if ( mcu_debug_init() ){
		mcu_cortexm_priv_disable_interrupts(NULL);
		mcu_board_execute_event_handler(MCU_BOARD_CONFIG_EVENT_PRIV_FATAL, (void*)"dbgi");
	}

	mcu_debug("MCU Debug start\n");

#if SCHED_USECOND_TMR_SLEEP_OC > -1
	if ( sched_timing_init() ){
		return -1;
	}
#endif

	mcu_debug("Load MCU Faults\n");




	//Load any possible faults from the last reset
	mcu_fault_load((fault_t*)&sched_fault.fault);

	mcu_debug("Init MPU\n");


#if USE_MEMORY_PROTECTION > 0
	if ( task_init_mpu(&_data, sos_board_config.sys_memory_size) < 0 ){
		sched_debug("Failed to initialize memory protection\n");
		mcu_board_execute_event_handler(MCU_BOARD_CONFIG_EVENT_PRIV_FATAL, (void*)"tski");
	}
#endif

	mcu_cortexm_priv_set_unprivileged_mode(); //Enter unpriv mode
	return 0;
}

#if SINGLE_TASK != 0
int start_single(){
	sched_prepare();
	start_function(NULL);
}
#endif




/*! @} */

