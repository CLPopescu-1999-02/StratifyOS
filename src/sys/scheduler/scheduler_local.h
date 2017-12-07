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


#ifndef SCHED_FLAGS_H_
#define SCHED_FLAGS_H_

#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <limits.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdint.h>
#include "cortexm/fault.h"
#include "cortexm/task.h"
#include "mcu/debug.h"
#include "mcu/core.h"
#include "sos/sos.h"
#include "trace.h"

#include "scheduler_flags.h"
#include "scheduler_timing.h"
#include "scheduler_fault.h"
#include "scheduler.h"


#define SCHEDULER_NUM_SIGNALS 32

extern volatile s8 m_scheduler_current_priority;
extern volatile s8 m_scheduler_status_changed;
extern volatile scheduler_fault_t m_scheduler_fault;


static inline int scheduler_priority(int id) MCU_ALWAYS_INLINE;
int scheduler_priority(int id){ return sos_sched_table[id].priority; };

static inline trace_id_t scheduler_trace_id(int id) MCU_ALWAYS_INLINE;
trace_id_t scheduler_trace_id(int id){ return sos_sched_table[id].trace_id; };


static inline int scheduler_current_priority() MCU_ALWAYS_INLINE;
int scheduler_current_priority(){ return m_scheduler_current_priority; };

static inline int scheduler_status_change_asserted() MCU_ALWAYS_INLINE;
inline int scheduler_status_change_asserted(){
	return m_scheduler_status_changed;
}


int scheduler_check_tid(int id);

int scheduler_prepare();
void scheduler();

int scheduler_create_thread(void *(*p)(void*)  /*! The function to execute for the task */,
		void * arg /*! The thread's single argument */,
		void * mem_addr /*! The address for the new thread memory */,
		int mem_size /*! The stack size in bytes */,
		pthread_attr_t * attr);


int scheduler_create_process(void (*p)(char *),
		const char * path_arg,
		task_memories_t * mem,
		void * reent);



int scheduler_switch_context(void * args);
int scheduler_get_highest_priority_blocked(void * block_object);




#define SCHED_DEBUG 0
#define scheduler_debug(...) do { if ( SCHED_DEBUG == 1 ){ mcu_debug_user_printf("%s:", __func__); mcu_debug_user_printf(__VA_ARGS__); } } while(0)
#define scheduler_root_debug(...) do { if ( SCHED_DEBUG == 1 ){ mcu_debug_printf("%s:", __func__); mcu_debug_root_printf(__VA_ARGS__); } } while(0)





#endif /* SCHED_FLAGS_H_ */
