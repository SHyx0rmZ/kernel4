/*
 *   ASXSoft Nuke - Operating System
 *   Copyright (C) 2009 Patrick Pokatilo
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

#ifndef _MULTIBOOT_H_
#define _MULTIBOOT_H_

#include "stdint.h"

enum class MultibootMemoryType : uint32_t
{
	Available = 1,
	System = 2,
	Reclaim = 3,
	ACPI = 4
};

class MultibootMemory
{
	public:
		uint32_t size;
		uintptr_t address;
		uint64_t length;
		MultibootMemoryType type;
} __attribute__((packed));

class MultibootModule
{
	public:
		void *module_start;
		void *module_end;
		char *name;
		uint8_t reserved[4];
} __attribute__((packed));

class MultibootInformation
{
	public:
		uint32_t flags;
		uint32_t memory_lower;
		uint32_t memory_upper;
		uint32_t boot_device;
		char *cmdline;
		uint32_t modules_count;
		MultibootModule *modules_address;
		uint64_t memory_length;
		MultibootMemory *memory_address;
		// TODO: drives, apm
} __attribute__((packed));

struct MultibootInformationSpecification
{
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint32_t elfshdr_num;
	uint32_t elfshdr_size;
	uint32_t elfshdr_addr;
	uint32_t elfshdr_shndx;
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint32_t vbe_interface_seg;
	uint32_t vbe_interface_off;
	uint32_t vbe_interface_len;
	uint32_t size;
} __attribute__((packed));

#endif
