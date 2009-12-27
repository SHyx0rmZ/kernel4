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
#include "string.h"
#include "multiboot.h"
#include "elf64.h"

extern gdt_pointer_t gdt_pointer;

void start_rocket_engine(struct multiboot_info *info)
{
	clear_screen();

	print("ASXSoft", COLOR_GRAY);
	print(" Rocket", COLOR_BLUE);
	print(" - Loading Kernel...\r\n================================================================================", COLOR_GRAY); 

	print("Initializing GDT...", COLOR_GRAY);

	gdt_initialize();
	gdt_load();
	gdt_flush_registers(0x08, 0x18, 0x18, 0x00, 0x00, 0x18);

	ewrin();
	print("Checking for Long Mode support...", COLOR_GRAY);

	cpuid_result_t *cpuid;

	cpuid = cpuid_extended(0x80000001);
	
	if((cpuid->edx & 0x20000000) == 0)
	{
		efail();
		print("Your CPU does not support Long Mode", COLOR_RED);
		reboot();
	}

	cpuid = cpuid_standard(0x00000001);

	if((cpuid->edx & 0x40) == 0)
	{
		efail();
		print("Your CPU doe not support Physical Adress Extension", COLOR_RED);
		reboot();
	}

	ewrin();
	print("Backing up Multiboot information...", COLOR_GRAY);

	struct multiboot_info *multiboot = (struct multiboot_info *)0x280000;
	memcpy(multiboot, info, sizeof(struct multiboot_info));
	multiboot->size = sizeof(struct multiboot_info);

	if(FLAG_ISSET(multiboot->mi_flags, 2))
	{
		memcpy((void *)((uint32_t)multiboot + multiboot->size), multiboot->cmdline, strlen(multiboot->cmdline));
		multiboot->cmdline = (char *)((uint32_t)multiboot + multiboot->size);
		multiboot->size += strlen(multiboot->cmdline);
	}

	if(FLAG_ISSET(multiboot->mi_flags, 3))
	{
		memcpy((void *)((uint32_t)multiboot + multiboot->size), multiboot->mods_addr, multiboot->mods_count * sizeof(struct multiboot_module));
		multiboot->mods_addr = (struct multiboot_module *)((uint32_t)multiboot + multiboot->size);
		multiboot->size += multiboot->mods_count * sizeof(struct multiboot_module);

		for(uint32_t mods = 0; mods < multiboot->mods_count; mods++)
		{
			memcpy((void *)((uint32_t)multiboot + multiboot->size), multiboot->mods_addr[mods].string, strlen(multiboot->mods_addr[mods].string));
			multiboot->mods_addr[mods].string = (char *)((uint32_t)multiboot + multiboot->size);
			multiboot->size += strlen(multiboot->mods_addr[mods].string);
		}
	}

	if(FLAG_ISSET(multiboot->mi_flags, 4) || FLAG_ISSET(multiboot->mi_flags, 5))
	{
		multiboot->mi_flags &= ~((1 << 4) | (1 << 5));
		memset(&multiboot->elfshdr_num, 0, 16);
	}

	if(FLAG_ISSET(multiboot->mi_flags, 6))
	{
		struct multiboot_memory_map *mem_s = (struct multiboot_memory_map *)multiboot->mmap_addr;
		struct multiboot_memory_map *mem_e = (struct multiboot_memory_map *)((uint32_t)multiboot->mmap_addr + (uint32_t)multiboot->mmap_length);

		multiboot->mmap_addr = (struct multiboot_memory_map *)((uint32_t)multiboot + multiboot->size);

		while(mem_s < mem_e)
		{
			memcpy((void *)((uint32_t)multiboot + multiboot->size), mem_s, mem_s->size + 4);
			multiboot->size += mem_s->size + 4;
			mem_s = (struct multiboot_memory_map *)((uint32_t)mem_s + mem_s->size + 4);
		}
	}

	// TODO: This will surely be needed one day
	if(FLAG_ISSET(multiboot->mi_flags, 7))
	{
		multiboot->mi_flags &= ~(1 << 7);
		memset(&multiboot->drives_length, 0, 8);
	}

	if(FLAG_ISSET(multiboot->mi_flags, 8))
	{
		multiboot->mi_flags &= ~(1 << 8);
		memset(&multiboot->config_table, 0, 4);
	}

	// TODO: This will surely be needed one day
	if(FLAG_ISSET(multiboot->mi_flags, 10))
	{
		multiboot->mi_flags &= ~(1 << 10);
		memset(&multiboot->apm_table, 0, 4);
	}

	// TODO: This may be needed one day
	if(FLAG_ISSET(multiboot->mi_flags, 11))
	{
		multiboot->mi_flags &= ~(1 << 11);
		memset(&multiboot->vbe_control_info, 0, 22);
	}

	ewrin();
	print("Searching for kernel...", COLOR_GRAY);

	uint32_t kernel = 0xFFFFFFFF;

	for(uint32_t mod = 0; kernel == 0xFFFFFFFF && mod < multiboot->mods_count; mod++)
	{
		uint32_t i = 0;

		for(; multiboot->mods_addr[mod].string[i] && multiboot->mods_addr[mod].string[i] != ' '; i++);

		while(kernel == 0xFFFFFFFF && multiboot->mods_addr[mod].string[i])
		{
			if(strncmp(&multiboot->mods_addr[mod].string[i], "--kernel", 8) == 0)
			{
				kernel = mod;
			}

			i++;
		}
	}

	if(kernel == 0xFFFFFFFF)
	{
		efail();
		print("No kernel was found", COLOR_RED);
		reboot();
	}

	ewrin();

	print("Loading kernel \"", COLOR_GRAY);
	memcpy((void *)0x2f0000, multiboot->mods_addr[kernel].string, strlen(multiboot->mods_addr[kernel].string));
	memset((void *)(0x2f0000 + strfind((const char *)0x2f0000, ' ')), 0, 1);
	print((const char *)0x2f0000, COLOR_YELLOW);
	print("\"...", COLOR_GRAY);

        Elf64_Ehdr *hdr = (Elf64_Ehdr *)multiboot->mods_addr[kernel].mod_start;
        Elf64_Phdr *phdr;

        if ((hdr->e_ident[0] != 0x7F) ||
	    (hdr->e_ident[1] != 'E')  ||
	    (hdr->e_ident[2] != 'L')  ||
	    (hdr->e_ident[3] != 'F'))
        {
                print("Kernel is no valid ELF executable", COLOR_RED);
                reboot();
        }

        if ((hdr->e_ident[EI_CLASS] != ELFCLASS64)  ||
            (hdr->e_ident[EI_DATA]  != ELFDATA2LSB) ||
	    (hdr->e_machine         != EM_X86_64)   ||
	    (hdr->e_type            != ET_EXEC))
        {
                print("Kernel is no x86-64 LSB executable", COLOR_RED);
                reboot();
        }

        phdr = (Elf64_Phdr *)((uintptr_t)multiboot->mods_addr[kernel].mod_start + (uintptr_t)hdr->e_phoff);
        for (int i = 0; i < hdr->e_phnum; i++)
        {
                if (phdr[i].p_type != PT_LOAD)
                        continue;
                memset((void *)(uintptr_t)phdr[i].p_vaddr, 0, phdr[i].p_memsz);
                if (!phdr[i].p_filesz)
                        continue;
                memcpy((void *)(uintptr_t)phdr[i].p_vaddr, (const void *)((uintptr_t)multiboot->mods_addr[kernel].mod_start + (uintptr_t)phdr[i].p_offset), phdr[i].p_filesz);
        }

	ewrin();
	print("Initializing Paging...", COLOR_GRAY);

	uint64_t *pml4 = paging_initialize();
	
	ewrin();
	print("Switching to Long Mode...", COLOR_GRAY);

	paging_activate(pml4);

	ewrin();

	print("Starting kernel execution...", COLOR_GREEN);

	asm(
		"push $0x10 \n"
		"push %0 \n"
		"retf \n"
		: : "g" (hdr->e_entry) , "D" (multiboot), "S" (&gdt_pointer)
	   );
}
