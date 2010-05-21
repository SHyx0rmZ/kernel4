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

#include "multiboot.h"
#include "kernel.h"

typedef void (*constructor)();

extern "C" void entry(struct MultibootInformationSpecification *multiboot_info_32bit, uint32_t page_tables);
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

void entry(struct MultibootInformationSpecification *multiboot_info_32bit, uint32_t page_tables)
{
	MultibootInformation multiboot;

	multiboot.flags = multiboot_info_32bit->flags;
	multiboot.memory_lower = multiboot_info_32bit->mem_lower;
	multiboot.memory_upper = multiboot_info_32bit->mem_upper;
	multiboot.boot_device = multiboot_info_32bit->boot_device;
	multiboot.cmdline = (char *)multiboot_info_32bit->cmdline;
	multiboot.modules_count = multiboot_info_32bit->mods_count;
	multiboot.modules_address = (MultibootModule *)multiboot_info_32bit->mods_addr;
	multiboot.memory_length = multiboot_info_32bit->mmap_length;
	multiboot.memory_address = (MultibootMemory *)multiboot_info_32bit->mmap_addr;


	for(constructor *i = &start_ctors; i != &end_ctors; ++i)
	{
		(*i)();
	}

	Kernel kernel(multiboot, (0x0000LL + page_tables));

	while(1);
}
