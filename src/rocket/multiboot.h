/*
 *   ASXSoft Nuke - Operating System
 *   Copyright (C) 2009  Max Reitz
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

#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include "stdint.h"

#define FLAG_ISSET(flags, bit) ((flags) & (1 << (bit)))

#define MULTIBOOT_INFO_MAGIC            0x2BADB002
#define MULTIBOOT_INFO_HAS_MEMORY       0x00000001
#define MULTIBOOT_INFO_HAS_BOOT_DEVICE  0x00000002
#define MULTIBOOT_INFO_HAS_CMDLINE      0x00000004
#define MULTIBOOT_INFO_HAS_MODS         0x00000008
#define MULTIBOOT_INFO_HAS_AOUT_SYMS    0x00000010
#define MULTIBOOT_INFO_HAS_ELF_SYMS     0x00000020
#define MULTIBOOT_INFO_HAS_MMAP         0x00000040
#define MULTIBOOT_INFO_HAS_DRIVES       0x00000080
#define MULTIBOOT_INFO_HAS_CONFIG_TABLE 0x00000100
#define MULTIBOOT_INFO_HAS_LOADER_NAME  0x00000200
#define MULTIBOOT_INFO_HAS_APM_TABLE    0x00000400
#define MULTIBOOT_INFO_HAS_VBE          0x00000800

struct multiboot_module
{
    void *mod_start;
    void *mod_end;
    char *string;
    uint8_t reserved[4];
} __attribute__((packed));

struct apm_table
{
    uint16_t version;
    uint16_t cseg;
    void *offset;
    uint16_t cseg16;
    uint16_t flags;
    uint16_t cseg_len;
    uint16_t cseg16_len;
    uint16_t dseg_len;
} __attribute__((packed));

struct multiboot_info
{
    uint32_t mi_flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    char *cmdline;
    uint32_t mods_count;
    struct multiboot_module *mods_addr;
    uint32_t elfshdr_num;
    uint32_t elfshdr_size;
    void *elfshdr_addr;
    uint32_t elfshdr_shndx;
    uint32_t mmap_length;
    void *mmap_addr;
    uint32_t drives_length;
    void *drives_addr;
    void *config_table;
    char *boot_loader_name;
    struct apm_table *apm_table;
    void *vbe_control_info;
    void *vbe_mode_info;
    uint16_t vbe_mode;
    uint32_t vbe_interface_seg;
    void *vbe_interface_off;
    uint32_t vbe_interface_len;
    uint32_t size;
} __attribute__((packed));

struct multiboot_memory_map
{
	uint32_t size;
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
} __attribute__((packed));

#endif
