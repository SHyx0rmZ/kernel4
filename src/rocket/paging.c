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
#include "paging.h"

uint32_t *paging_initialize(uint32_t address)
{
	address += 0x0FFF;
	address &= 0xFFFF1000;

	uint64_t *pml4e = (uint64_t *)address;
	*pml4e = address + 0x100BLL;
	uint64_t *pdpe = (uint64_t *)(address + 0x1000);
	*pdpe = address + 0x200BLL;
	
	uint64_t *pdd = (uint64_t *)(address + 0x2000);

	for(uint8_t i = 0; i < 128; i++)
	{
		*(pdd++) = i * 0x200000 + 0x018FLL;
	}

	uint64_t *pml4ex = (uint64_t *)(address + 0x0FF8);
	*pml4ex = address + 0x300BLL;
	uint64_t *pdpex = (uint64_t *)(address + 0x3FF8);
	*pdpex = address + 0x400BLL;
	uint64_t *pdx = (uint64_t *)(address + 0x4FF8);
	*pdx = address + 0x500BLL;
	uint64_t *ptx = (uint64_t *)(address + 0x5FF8);
	*ptx = address + 0x018FLL;

	return (uint32_t *)address;
}

void paging_activate(uint64_t *pml4)
{
	asm(
		"push %%eax \n"
		"push %%ecx \n"

		"mov %%cr4, %%eax \n"
		"or $0xA0, %%eax \n"
		"mov %%eax, %%cr4 \n"

		"mov %0, %%eax \n"
		"mov %%eax, %%cr3 \n"

		"mov $0xC0000080, %%ecx \n"
		"rdmsr \n"
		"or $0x0100, %%eax \n"
		"wrmsr \n"

		"mov %%cr0, %%eax \n"
		"or $0x80000000, %%eax \n"
		"mov %%eax, %%cr0 \n"
		
		"pop %%ecx \n"
		"pop %%eax \n"
		:
		: "g"(pml4)
	   );
}
