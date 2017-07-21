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
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>

#include "sched_flags.h"

typedef struct {
	task_memories_t * mem;
	int tid;
} init_sched_task_t;

static void priv_init_sched_task(init_sched_task_t * task) MCU_PRIV_EXEC_CODE;
static void cleanup_process(void * status);

/*! \details This function creates a new process.
 * \return The thread id or zero if the thread could not be created.
 */
int sched_new_process(void (*p)(char *)  /*! The startup function (crt()) */,
		const char * path_arg /*! Path string with arguments */,
		task_memories_t * mem,
		void * reent /*! The location of the reent structure */){
	int tid;
	init_sched_task_t args;

	//Start a new process
	tid = task_new_process(
			p,
			cleanup_process,
			path_arg,
			mem,
			reent
	);

	if ( tid > 0 ){
		//update the scheduler table using a privileged call
		args.tid = tid;
		args.mem = mem;
		mcu_core_privcall((core_privcall_t)priv_init_sched_task, &args);
	} else {
		return -1;
	}

	return task_get_pid( tid );
}

void priv_init_sched_task(init_sched_task_t * task){
	uint32_t stackguard;
	struct _reent * reent;
	int id = task->tid;
	memset((void*)&sos_sched_table[id], 0, sizeof(sched_task_t));

	PTHREAD_ATTR_SET_IS_INITIALIZED((&(sos_sched_table[id].attr)), 1);
	PTHREAD_ATTR_SET_SCHED_POLICY((&(sos_sched_table[id].attr)), SCHED_OTHER);
	PTHREAD_ATTR_SET_GUARDSIZE((&(sos_sched_table[id].attr)), SCHED_DEFAULT_STACKGUARD_SIZE);
	PTHREAD_ATTR_SET_CONTENTION_SCOPE((&(sos_sched_table[id].attr)), PTHREAD_SCOPE_SYSTEM);
	PTHREAD_ATTR_SET_INHERIT_SCHED((&(sos_sched_table[id].attr)), PTHREAD_EXPLICIT_SCHED);
	PTHREAD_ATTR_SET_DETACH_STATE((&(sos_sched_table[id].attr)), PTHREAD_CREATE_DETACHED);

	sos_sched_table[id].attr.stackaddr = task->mem->data.addr; //Beginning of process data memory
	sos_sched_table[id].attr.stacksize = task->mem->data.size; //Size of the memory (not just the stack)
	sos_sched_table[id].priority = 0; //This is the default starting priority priority
	sos_sched_table[id].attr.schedparam.sched_priority = 0; //This is the priority to revert to after being escalated

	sos_sched_table[id].wake.tv_sec = SCHED_TIMEVAL_SEC_INVALID;
	sos_sched_table[id].wake.tv_usec = 0;
	sched_priv_assert_active(id, 0);
	sched_priv_assert_inuse(id);
	sched_priv_update_on_wake( sos_sched_table[id].priority );
	stackguard = (uint32_t)task->mem->data.addr + task->mem->data.size - 128;
#if USE_MEMORY_PROTECTION > 0
	task_priv_set_stackguard(id, (void*)stackguard, SCHED_DEFAULT_STACKGUARD_SIZE);
#endif

	//Items inherited from parent process

	//Signal mask
	reent = (struct _reent *)task_table[id].reent;
	reent->sigmask = _REENT->sigmask;
}

static void cleanup_process(void * status){
	//Processes should ALWAYS use exit -- this should never get called but is here just in case
	kill(task_get_pid(task_get_current()), SIGKILL);
}

