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

#include <string.h>
#include "sos/sos.h"
#include "scheduler_local.h"

void scheduler_fault_build_trace_string(char * dest){
	sprintf(dest, "F%X:0x%lX",
			m_scheduler_fault.fault.num,
			(u32)m_scheduler_fault.fault.addr
	);
	return;
	char hex_buffer[9];
	strcpy(dest, "F");
	htoa(hex_buffer, m_scheduler_fault.fault.num);
	strcat(dest, hex_buffer);
	strcat(dest, ":A");
	htoa(hex_buffer, (u32)m_scheduler_fault.fault.handler_pc);
	strcat(dest, hex_buffer);
	strcat(dest, ":C");
	htoa(hex_buffer, (u32)m_scheduler_fault.fault.caller);
	strcat(dest, "\n");
}

void scheduler_fault_build_string(char * dest){
	sprintf(dest, "F%X:A%lX:PC%lX:C%lX:HA%lX:HC%lX\n",
			m_scheduler_fault.fault.num,
			(u32)m_scheduler_fault.fault.addr,
			(u32)m_scheduler_fault.fault.pc,
			(u32)m_scheduler_fault.fault.caller,
			(u32)m_scheduler_fault.fault.handler_pc,
			(u32)m_scheduler_fault.fault.handler_caller
	);
	return;
	char hex_buffer[9];
	strcpy(dest, "F");
	htoa(hex_buffer, m_scheduler_fault.fault.num);
	strcat(dest, hex_buffer);
	strcat(dest, ":A");
	htoa(hex_buffer, (u32)m_scheduler_fault.fault.handler_pc);
	strcat(dest, hex_buffer);
	strcat(dest, ":C");
	htoa(hex_buffer, (u32)m_scheduler_fault.fault.caller);
	strcat(dest, "\n");
}

void htoa(char * dest, int num){
	char nibble;
	int i;
	for(i=0; i < 8; i++){
		nibble = num & 0xF;
		num >>= 4;
		dest[7 - i] = htoc(nibble);
	}
	dest[8] = 0;
}
