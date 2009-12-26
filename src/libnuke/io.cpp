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
#include <io.h>

uint8_t in8(uint16_t port)
{
	uint8_t data;

	asm(
		"inb %1 \n"
		: "=a" (data) : "d" (port)
	);

	return data;
}

uint16_t in16(uint16_t port)
{
	uint16_t data;

	asm(
		"inw %1 \n"
		: "=a" (data) : "d" (port)
	);

	return data;
}

uint32_t in32(uint16_t port)
{
	uint32_t data;

	asm(
		"inl %1 \n"
		: "=a" (data) : "d" (port)
	);

	return data;
}

void out8(uint16_t port, uint8_t data)
{
	asm(
		"outb %0 \n"
		: : "d" (port), "a" (data)
	);
}

void out16(uint16_t port, uint16_t data)
{
	asm(
		"outw %0 \n"
		: : "d" (port), "a" (data)
	);
}

void out32(uint16_t port, uint32_t data)
{
	asm(
		"outl %0 \n"
		: : "d" (port), "a" (data)
	);
}
