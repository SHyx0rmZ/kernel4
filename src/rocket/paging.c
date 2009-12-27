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

uint64_t *paging_initialize()
{
	uint64_t *pml4e = (uint64_t *)0x300000LL;
	*pml4e = 0x30100BLL;
	uint64_t *pdpe = (uint64_t *)0x301000LL;
	*pdpe = 0x30200BLL;
	uint64_t *pd1 = (uint64_t *)0x302000LL;
	uint64_t *pd2 = (uint64_t *)0x302008LL;
	uint64_t *pd3 = (uint64_t *)0x302010LL;
	uint64_t *pd4 = (uint64_t *)0x302018LL;
	uint64_t *pd5 = (uint64_t *)0x302020LL;
	uint64_t *pd6 = (uint64_t *)0x302028LL;
	uint64_t *pd7 = (uint64_t *)0x302030LL;
	uint64_t *pd8 = (uint64_t *)0x302038LL;
	uint64_t *pd9 = (uint64_t *)0x302040LL;
	uint64_t *pd0 = (uint64_t *)0x302048LL;
	
	*pd1 = 0x000018FLL;
	*pd2 = 0x020018FLL;
	*pd3 = 0x040018FLL;
	*pd4 = 0x060018FLL;
	*pd5 = 0x080018FLL;
	*pd6 = 0x0A0018FLL;
	*pd7 = 0x0C0018FLL;
	*pd8 = 0x0E0018FLL;
	*pd9 = 0x100018FLL;
	*pd0 = 0x120018FLL;

	return (uint64_t *)0x300000LL;
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
