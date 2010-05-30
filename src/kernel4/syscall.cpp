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

#include <stdint.h>

void SyscallHandler()
{
	uint64_t rcx, r11;
	asm("mov %%r11, %%rax" : "=c" (rcx), "=a" (r11));

	//TODO: Do what ever has to be done :D

	asm("mov %%rax, %%r11" : : "c" (rcx), "a" (r11));
	asm("sysret");
}

void SetupSyscalls(uint16_t segment, uintptr_t address)
{
	asm(
		"mov $0xC0000084, %%ecx \n"
		"xor %%eax, %%eax \n"
		"wrmsr \n"
		"mov $0xC0000081, %%ecx \n"
		"mov %0, %%dx \n"
		"wrmsr \n"
		"mov $0xC0000082, %%ecx \n"
		"mov %1, %%eax \n"
		"mov %2, %%edx \n"
		"wrmsr \n"
		"mov $0xC0000080, %%ecx \n"
		"rdmsr \n"
		"or $1, %%eax \n"
		"wrmsr \n"
		: : "b" ((uint16_t)((segment << 3) & 0xFFFF)), "S" ((uint32_t)(address & 0xFFFFFFFF)), "D" ((uint32_t)(address >> 32))
	);
}
