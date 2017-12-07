/*! \file */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "mcu/debug.h"

#if MCU_DEBUG
#define VERSION "3.2.0d"
#else
#define VERSION "3.2.0"
#endif

#define BCD_VERSION 0x320

#include <stdint.h>

#ifdef __armv7m
#define ARCH "armv7m"
#define SCHED_USECOND_TMR_RESET_OC 0
#define SCHED_USECOND_TMR_SLEEP_OC 1
#define PTHREAD_DEFAULT_STACK_SIZE 1536
#define MALLOC_CHUNK_SIZE 64
#define MALLOC_SBRK_JUMP_SIZE 128
#define SCHED_FIRST_THREAD_STACK_SIZE 2048
#define SCHED_DEFAULT_STACKGUARD_SIZE 32
#endif

#if defined __armv7em
#if __FPU_USED
#define ARCH "armv7em-fpu"
#else
#define ARCH "armv7em"
#endif
#define SCHED_USECOND_TMR_RESET_OC 0
#define SCHED_USECOND_TMR_SLEEP_OC 1
#define PTHREAD_DEFAULT_STACK_SIZE 1536
#define MALLOC_CHUNK_SIZE 64
#define MALLOC_SBRK_JUMP_SIZE 128
#define SCHED_FIRST_THREAD_STACK_SIZE 2048
#define SCHED_DEFAULT_STACKGUARD_SIZE 32
#endif


#define PTHREAD_MAX_LOCKS 1024
#define PTHREAD_MUTEX_PRIO_CEILING 0
#define PTHREAD_STACK_MIN 128
#define SCHED_LOWEST_PRIORITY 0
#define SCHED_HIGHEST_PRIORITY 31
#define SCHED_DEFAULT_PRIORITY 0
#define SCHED_RR_DURATION 10

#define USE_STDIO 1

#endif
