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

#include "kernel.h"
#include "multiboot.h"
#include <stdint.h>
#include <stddef.h>
#include <gdt.h>
#include <io.h>
#include <console.h>
#include <memory.h>
#include <managers.h>
#include <idt.h>
#include <isr.h>

Console console;
//MemoryStack memoryb(0x1000000);
//MemoryStack memory(0x1100000);
MemoryManager memory;
PagingManager pages(0xF00000);
TaskManager scheduler(0xE00000, 4000);
extern void test();
/**
 * Creates a new instance of the kernel
 * 
 * @param multiboot		The multiboot information structure the bootloader created
 */
Kernel::Kernel(MultibootInformation multiboot)
{
	console.Clear();

	// Wonderful TUI
	console << ConsoleArea::Top << ConsoleColor::DarkGray << "ASXSoft " << ConsoleColor::Blue << "Nuke " << ConsoleColor::DarkGray << "- " << ConsoleColor::Blue << "Version 0.4" << ConsoleColor::Gray << ConsoleArea::Middle;

	console << "Initializing memory pool...\r\n";

	// If Bit 6 is cleared there is no memory map, so abort
	if((multiboot.flags & (1 << 6)) == 0)
	{
		console << "EPIC FAIL";
		while(1);
	}

	MultibootMemory *memstart = multiboot.memory_address;
	MultibootMemory *memend = (MultibootMemory *)((uintptr_t)multiboot.memory_address + multiboot.memory_length);

	// Scan each memory block in the memory map
	while(memstart < memend)
	{
		// Print some information
		console << "Memory block @ " << ConsoleState::HexFixed << memstart->address << " - " << memstart->address + memstart->length << " (" << ConsoleState::Decimal << ConsoleColor::Blue;
		
		uint64_t memsize = memstart->length / 1024;

		if(memsize > 1024)
		{
			console << memsize / 1024 << " MiB";
		}
		else
		{
			console << memsize << " KiB";
		}

		console << ConsoleColor::Gray << ") ";

		// If it is free memory, add it to the memory ppol
		if(memstart->type == MultibootMemoryType::Available)
		{
			console << ConsoleColor::Green << " OK\r\n" << ConsoleColor::Gray;

			memory.Initialize(memstart->address, memstart->length);
		}
		else
		{
			console << ConsoleColor::Red << " FAIL\r\n" << ConsoleColor::Gray;
		}

		// Next memory block
		memstart = (MultibootMemory *)((uintptr_t)memstart + memstart->size + 4);
	}

	console << "Initializing GDT... ";

	// TODO: Replace address with new()
	// Set up the GDT
	GDTTable gdt(5, 0x200000 - (5 * sizeof(GDTEntry)));

	gdt.SetEntry(0, GDTEntry(GDTMode::RealMode, 		GDTType::Code, GDTRing::Ring0, 0, 0, GDTGranularity::Block, GDTPresence::NonPresent));
	gdt.SetEntry(1, GDTEntry(GDTMode::LongMode, 		GDTType::Code, GDTRing::Ring0));
	gdt.SetEntry(2, GDTEntry(GDTMode::ProtectedMode, 	GDTType::Data, GDTRing::Ring0));
	gdt.SetEntry(3, GDTEntry(GDTMode::LongMode, 		GDTType::Code, GDTRing::Ring3));
	gdt.SetEntry(4, GDTEntry(GDTMode::ProtectedMode, 	GDTType::Data, GDTRing::Ring3));

	// Load GDT and set up proper segments
	gdt.MakeActive();

	gdt.ReloadSegment(GDTSegmentRegister::CS, 1);
	gdt.ReloadSegment(GDTSegmentRegister::DS, 2);
	gdt.ReloadSegment(GDTSegmentRegister::ES, 2);
	gdt.ReloadSegment(GDTSegmentRegister::FS, 0);
	gdt.ReloadSegment(GDTSegmentRegister::GS, 0);
	gdt.ReloadSegment(GDTSegmentRegister::SS, 2);

	console << ConsoleColor::Green << "OK\r\n" << ConsoleColor::Gray;
	console << "Intializing IDT... ";

	// TODO: Replace address with new()
	// Set up IDT
	IDTTable idt(128, 0x200000 - (5 * sizeof(GDTEntry)) - (128 * sizeof(IDTEntry)));

	// Install dummy handlers for every interrupt
	for(uint16_t i = 0; i < idt.GetSize(); i++)
	{
		idt.SetEntry(i, IDTEntry((uintptr_t)isr_null, IDTType::Trap));
	}

	// Set the handlers of interrupts we want to handle
	idt.SetEntry(0, IDTEntry((uintptr_t)exception_stub_0));
	idt.SetEntry(1, IDTEntry((uintptr_t)exception_stub_1));
	idt.SetEntry(2, IDTEntry((uintptr_t)exception_stub_2));
	idt.SetEntry(3, IDTEntry((uintptr_t)exception_stub_3, IDTType::Trap));
	idt.SetEntry(4, IDTEntry((uintptr_t)exception_stub_4, IDTType::Trap));
	idt.SetEntry(5, IDTEntry((uintptr_t)exception_stub_5));
	idt.SetEntry(6, IDTEntry((uintptr_t)exception_stub_6));
	idt.SetEntry(7, IDTEntry((uintptr_t)exception_stub_7));

	idt.SetEntry(8, IDTEntry((uintptr_t)exception_stub_8));
	idt.SetEntry(10, IDTEntry((uintptr_t)exception_stub_10));
	idt.SetEntry(11, IDTEntry((uintptr_t)exception_stub_11));
	idt.SetEntry(12, IDTEntry((uintptr_t)exception_stub_12));
	idt.SetEntry(13, IDTEntry((uintptr_t)exception_stub_13));
	idt.SetEntry(14, IDTEntry((uintptr_t)exception_stub_14));

	idt.SetEntry(16, IDTEntry((uintptr_t)exception_stub_16));
	idt.SetEntry(17, IDTEntry((uintptr_t)exception_stub_17));
	idt.SetEntry(18, IDTEntry((uintptr_t)exception_stub_18));
	idt.SetEntry(19, IDTEntry((uintptr_t)exception_stub_19));

	idt.SetEntry(32, IDTEntry((uintptr_t)irq_stub_0, IDTType::Trap));
	idt.SetEntry(33, IDTEntry((uintptr_t)irq_stub_1, IDTType::Trap));
	idt.SetEntry(34, IDTEntry((uintptr_t)irq_stub_2, IDTType::Trap));
	idt.SetEntry(35, IDTEntry((uintptr_t)irq_stub_3, IDTType::Trap));
	idt.SetEntry(36, IDTEntry((uintptr_t)irq_stub_4, IDTType::Trap));
	idt.SetEntry(37, IDTEntry((uintptr_t)irq_stub_5, IDTType::Trap));
	idt.SetEntry(38, IDTEntry((uintptr_t)irq_stub_6, IDTType::Trap));
	idt.SetEntry(39, IDTEntry((uintptr_t)irq_stub_7, IDTType::Trap));

	idt.SetEntry(40, IDTEntry((uintptr_t)irq_stub_8, IDTType::Trap));
	idt.SetEntry(41, IDTEntry((uintptr_t)irq_stub_9, IDTType::Trap));
	idt.SetEntry(42, IDTEntry((uintptr_t)irq_stub_10, IDTType::Trap));
	idt.SetEntry(43, IDTEntry((uintptr_t)irq_stub_11, IDTType::Trap));
	idt.SetEntry(44, IDTEntry((uintptr_t)irq_stub_12, IDTType::Trap));
	idt.SetEntry(45, IDTEntry((uintptr_t)irq_stub_13, IDTType::Trap));
	idt.SetEntry(46, IDTEntry((uintptr_t)irq_stub_14, IDTType::Trap));
	idt.SetEntry(47, IDTEntry((uintptr_t)irq_stub_15, IDTType::Trap));

	// Load IDT
	idt.MakeActive();

	console << ConsoleColor::Green << "OK\r\n" << ConsoleColor::Gray;
	console << "Remapping IRQs... ";

	// Initialize the PIC
	out8(0x20, 0x11); // Initialize Master
	out8(0xA0, 0x11); // Initialize Slave

	out8(0x21, 0x20); // Master Vector
	out8(0xA1, 0x28); // Slave Vector
	
	out8(0x21, 0x04); // Slave is on 0x22
	out8(0xA1, 0x02); // Master is on 0x29

	out8(0x21, 0x01); // 8086
	out8(0xA1, 0x01); // 8086

	out8(0x21, 0x00); // Masking
	out8(0xA1, 0x00); // Masking

	console << ConsoleColor::Green << "OK\r\n" << ConsoleColor::Gray;
	console << "Enabling interrupts... ";

	// Set a new frequency for IRQ 0
	uint64_t timer_divisor = 4773; // 249.98 Hz

	out8(0x43, 0x34);
	out8(0x40, (timer_divisor & 0xFF));
	out8(0x40, (timer_divisor >> 8));
	
	// Enable Interrupts (Taskswitchs)
	asm("sti");

	console << ConsoleColor::Green << "OK\r\n" << ConsoleColor::Gray;
	console << "Entering endless loop...\r\n";

	// Idle
	while(1)
	{
		asm("hlt");
	}

	test();
}

/**
 * The kernel can not be shutdown and instead enters a endless loop
 */
Kernel::~Kernel()
{
	while(1);
}
