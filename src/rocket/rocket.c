/*
 *   ASXSoft Nuke - Operating System
 *   Copyright (C) 2009  Patrick Pokatilo
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdint.h"
#include "stddef.h"
#include "gdt.h"
#include "paging.h"
#include "reboot.h"
#include "print.h"
#include "cpuid.h"

extern gdt_pointer_t gdt_pointer;

void start_rocket_engine()
{
	print("ASXSoft", COLOR_GRAY);
	print(" Rocket", COLOR_BLUE);
	print(" - Loading Kernel...\r\n", COLOR_GRAY); 

	gdt_initialize();
	gdt_load();
	gdt_flush_registers(0x08, 0x18, 0x18, 0x00, 0x00, 0x18);

	cpuid_result_t *cpuid;

	cpuid = cpuid_extended(0x80000001);
	
	if((cpuid->edx & 0x20000000) == 0)
	{
		print("Your CPU does not support Long Mode", COLOR_RED);
		reboot();
	}

	cpuid = cpuid_standard(0x00000001);

	if((cpuid->edx & 0x40) == 0)
	{
		print("Your CPU doe not support Physical Adress Extension", COLOR_RED);
		reboot();
	}

	uint64_t *pml4 = paging_initialize();
	
	paging_activate(pml4);

	while(1);
}
