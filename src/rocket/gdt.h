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

#ifndef _GDT_H_
#define _GDT_H_

#include "stdint.h"

/*
 * Variables
 */

#define GDT_LIMIT			0xFFFFFFFF

// Access
#define GDT_PRESENT	 		0x80
#define GDT_RING_3			0x60
#define GDT_RING_2			0x40
#define GDT_RING_1			0x20
#define GDT_RING_0			0x00
#define GDT_SEGMENT			0x10
#define GDT_EXECUTABLE			0x08
#define GDT_DIRECTION_DOWN		0x04
#define GDT_DIRECTION_UP		0x00
#define GDT_CONFORMING			0x04
#define GDT_READABLE			0x02
#define GDT_WRITABLE			0x02
#define GDT_ACCESSED			0x01

// Flags
#define GDT_GRANULAR	 		0x08
#define GDT_PROTECTED_MODE		0x04
#define GDT_REAL_MODE			0x00
#define GDT_LONG_MODE	 		0x02
#define GDT_AVAILABLE	 		0x01

/*
 * Structs
 */

// gdt_entry_t
typedef struct
{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t flags_and_limit_high;
	uint8_t base_high;
} 
__attribute__((packed)) 
gdt_entry_t;

// gdt_pointer_t
typedef struct
{
	uint16_t limit;
	uintptr_t base;
} 
__attribute__((packed)) 
gdt_pointer_t;

/*
 * Functions
 */

void gdt_flush_registers(uint16_t cs, uint16_t ds, uint16_t es, uint16_t fs, uint16_t gs, uint16_t ss);
void gdt_load();
void gdt_set_entry(uint16_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void gdt_initialize();

#endif
