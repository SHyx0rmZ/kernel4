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
#include <config.h>
#include <task.h>
#include <string.h>

Console console;
MemoryManager memory;
PagingManager paging(0x300000);
GDTTable gdt(5, 0x300000);
TaskManager tasking(NULL);

extern void test();

/**
 * Creates a new instance of the kernel
 *
 * @param multiboot		The multiboot information structure the bootloader created
 */
Kernel::Kernel(MultibootInformation multiboot, uintptr_t paging_structures)
{
	paging = PagingManager(paging_structures);

	console.Clear();

	// Wonderful TUI
	console << ConsoleArea::Top << ConsoleColor::DarkGray << "ASXSoft " << ConsoleColor::Blue << "Nuke " << ConsoleColor::DarkGray << "- " << ConsoleColor::Blue << "Version " << NUKE_VERSION << ConsoleColor::Gray << ConsoleArea::Middle;

	console << "Initializing memory pool... (Note: This can take quite some time)\r\n";

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
			console << ConsoleColor::Green << " WRIN\r\n" << ConsoleColor::Gray;

			memory.Initialize(memstart->address, memstart->length);
		}
		else
		{
			console << ConsoleColor::Red << " FAIL\r\n" << ConsoleColor::Gray;
		}

		// Next memory block
		memstart = (MultibootMemory *)((uintptr_t)memstart + memstart->size + 4);
	}

	console << "Replacing temporary Paging structures...";

	PagingManager *p = (PagingManager *)memory.PAlloc(1);
	uintptr_t pp = memory.PAlloc(4);

	memset(p, 0, 0x1000);
	memset((void *)pp, 0, 0x4000);

	*p = PagingManager(pp);

	for(uintptr_t block = 0; block < (((uintptr_t)p + 0x0FFFFFFF) & ~0x0FFFFFFF);) //block += 0x1000)
	{
		p->Map(block, block);

		block += 0x1000;
	}

	paging = *p;

	paging.Load();

	console << ConsoleColor::Green << "WRIN\r\n" << ConsoleColor::Gray;
	console << "Initializing GDT... ";

	// TODO: Replace address with new()
	// Set up the GDT
	//GDTTable gdt(5, 0x200000 - (5 * sizeof(GDTEntry)));

	gdt = GDTTable(7, memory.VAlloc(7 * sizeof(GDTEntry)));

	gdt.SetEntry(0, GDTEntry(GDTMode::RealMode,		GDTType::Code, GDTRing::Ring0, 0, 0, GDTGranularity::Block, GDTPresence::NonPresent));
	gdt.SetEntry(1, GDTEntry(GDTMode::LongMode,		GDTType::Code, GDTRing::Ring0));
	gdt.SetEntry(2, GDTEntry(GDTMode::ProtectedMode,	GDTType::Data, GDTRing::Ring0));
	gdt.SetEntry(3, GDTEntry(GDTMode::LongMode,		GDTType::Code, GDTRing::Ring3));
	gdt.SetEntry(4, GDTEntry(GDTMode::ProtectedMode,	GDTType::Data, GDTRing::Ring3));
	gdt.SetEntry(5, AvailableTSS64(0, 104).ToGDTEntryPart1());
	gdt.SetEntry(6, AvailableTSS64(0, 104).ToGDTEntryPart2());

	// Load GDT and set up proper segments
	gdt.MakeActive();

	gdt.ReloadSegment(GDTSegmentRegister::CS, 1);
	gdt.ReloadSegment(GDTSegmentRegister::DS, 2);
	gdt.ReloadSegment(GDTSegmentRegister::ES, 2);
	gdt.ReloadSegment(GDTSegmentRegister::FS, 0);
	gdt.ReloadSegment(GDTSegmentRegister::GS, 0);
	gdt.ReloadSegment(GDTSegmentRegister::SS, 2);

	console << ConsoleColor::Green << "WRIN\r\n" << ConsoleColor::Gray;
	console << "Intializing IDT... ";

	// TODO: Replace address with new()
	// Set up IDT
	//IDTTable idt(128, 0x200000 - (5 * sizeof(GDTEntry)) - (128 * sizeof(IDTEntry)));

	IDTTable idt(128, memory.VAlloc(128 * sizeof(IDTEntry)));

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

	idt.SetEntry(81, IDTEntry((uintptr_t)isr_system));

	// Load IDT
	idt.MakeActive();

	console << ConsoleColor::Green << "WRIN\r\n" << ConsoleColor::Gray;
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

	console << ConsoleColor::Green << "WRIN\r\n" << ConsoleColor::Gray;
	console << "Enabling interrupts... ";

	// Set a new frequency for IRQ 0
	uint64_t timer_divisor = 4773; // 249.98 Hz

	out8(0x43, 0x34);
	out8(0x40, (timer_divisor & 0xFF));
	out8(0x40, (timer_divisor >> 8));

	console << ConsoleColor::Green << "WRIN\r\n" << ConsoleColor::Gray;
	console << "Entering endless loop...\r\n";

	test();

	tasking = TaskManager((uintptr_t)Kernel::Idle);

	console << "Activating TSS";

	//uint16_t tss = 5 << 3;

	//asm("ltr %0" : : "m"(tss));

	// Enable Interrupts (Taskswitchs)
	asm("sti");

	while(1)
	{
		asm("hlt");
	}
}

void Tasker1()
{
	while(1)
		console << ConsoleColor::Yellow << "Still Alive... ";
}

void Tasker2()
{
	while(1)
		console << ConsoleColor::Red << "Still Alive... ";
}

void Tasker3()
{
	while(1)
		console << ConsoleColor::Blue << "Still Alive... ";
}

void Tasker4()
{
	while(1)
		console << ConsoleColor::Green << "Still Alive... ";
}

void Kernel::Idle()
{
	console << "Kernel is now idling...\r\n";

	Task tasker1((uintptr_t)Tasker1);
	Task tasker2((uintptr_t)Tasker2);
	Task tasker3((uintptr_t)Tasker3);
	Task tasker4((uintptr_t)Tasker4);

	tasking.tasks.Add(&tasker1);
	tasking.tasks.Add(&tasker2);
	tasking.tasks.Add(&tasker3);
	tasking.tasks.Add(&tasker4);

	while(1)
	{
		asm("hlt");
	}
}

/**
 * The kernel can not be shutdown and instead enters a endless loop
 */
Kernel::~Kernel()
{
	while(1);
}
