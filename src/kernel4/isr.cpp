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

#include <isr.h>
#include <managers.h>
#include <task.h>
#include <io.h>

uint64_t timer = 0; // Uptime
uint64_t freemem = 0; // Amount of free memory

void dump_info(TaskState *task)
{
	uint64_t cr0, cr2, cr3, cr4, cr8;

	asm(
		"mov %%cr0, %%rax \n"
		"mov %%rax, %0 \n"
		"mov %%cr2, %%rax \n"
		"mov %%rax, %1 \n"
		"mov %%cr3, %%rax \n"
		"mov %%rax, %2 \n"
		"mov %%cr4, %%rax \n"
		"mov %%rax, %3 \n"
		"mov %%cr8, %%rax \n"
		"mov %%rax, %4 \n"
		: "=g" (cr0), "=g" (cr2), "=g" (cr3), "=g" (cr4), "=g" (cr8)
	);

	console << ConsoleColor::Blue << "Register Dump:" << ConsoleState::HexFixed;
	console << ConsoleColor::Blue << "\r\n    SS: " << ConsoleColor::Gray << task->rsp << ConsoleColor::Blue << "    RSP: " << ConsoleColor::Gray << task->rsp << ConsoleColor::Blue << " RFLAGS: " << ConsoleColor::Gray << task->rflags;
	console << ConsoleColor::Blue << "\r    CS: " << ConsoleColor::Gray << task->cs << ConsoleColor::Blue << "    RIP: " << ConsoleColor::Gray << task->rip << ConsoleColor::Blue << "    CR0: " << ConsoleColor::Gray << cr0;
	console << ConsoleColor::Blue << "\r   INT: " << ConsoleColor::Gray << task->interrupt << ConsoleColor::Blue << "  ERROR: " << ConsoleColor::Gray << task->error << ConsoleColor::Blue << "    CR2: " << ConsoleColor::Gray << cr2;
	console << ConsoleColor::Blue << "\r    DS: " << ConsoleColor::Gray << task->ds << ConsoleColor::Blue << "     ES: " << ConsoleColor::Gray << task->es << ConsoleColor::Blue << "    CR3: " << ConsoleColor::Gray << cr3;
	console << ConsoleColor::Blue << "\r    FS: " << ConsoleColor::Gray << task->fs << ConsoleColor::Blue << "     GS: " << ConsoleColor::Gray << task->gs << ConsoleColor::Blue << "    CR4: " << ConsoleColor::Gray << cr4;
	console << ConsoleColor::Blue << "\r   RAX: " << ConsoleColor::Gray << task->rax << ConsoleColor::Blue << "    RBX: " << ConsoleColor::Gray << task->rbx << ConsoleColor::Blue << "    RCX: " << ConsoleColor::Gray << task->rcx;
	console << ConsoleColor::Blue << "\r   RDX: " << ConsoleColor::Gray << task->rdx << ConsoleColor::Blue << "    RDI: " << ConsoleColor::Gray << task->rdi << ConsoleColor::Blue << "    RSI: " << ConsoleColor::Gray << task->rsi;
	console << ConsoleColor::Blue << "\r   RBP: " << ConsoleColor::Gray << task->rbp << ConsoleColor::Blue << "     R8: " << ConsoleColor::Gray << task->r8 << ConsoleColor::Blue << "     R9: " << ConsoleColor::Gray << task->r9;
	console << ConsoleColor::Blue << "\r   R10: " << ConsoleColor::Gray << task->r10 << ConsoleColor::Blue << "    R11: " << ConsoleColor::Gray << task->r10 << ConsoleColor::Blue << "    R12: " << ConsoleColor::Gray << task->r12;
	console << ConsoleColor::Blue << "\r   R13: " << ConsoleColor::Gray << task->r13 << ConsoleColor::Blue << "    R14: " << ConsoleColor::Gray << task->r14 << ConsoleColor::Blue << "    R15: " << ConsoleColor::Gray << task->r15;
	console << ConsoleColor::Blue << "\rStack Dump:";
	console << ConsoleColor::Blue << "\r\n SS(0): " << ConsoleColor::Gray << *((uint64_t *)task->rsp) << ConsoleColor::Blue << "  SS(1): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 8) << ConsoleColor::Blue << "  SS(2): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 16);
	console << ConsoleColor::Blue << "\r SS(3): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 24) << ConsoleColor::Blue << "  SS(4): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 32) << ConsoleColor::Blue << "  SS(5): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 40);
	console << ConsoleColor::Blue << "\r SS(6): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 48) << ConsoleColor::Blue << "  SS(7): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 56) << ConsoleColor::Blue << "  SS(8): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 64);
	console << ConsoleColor::Blue << "\r SS(9): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 72) << ConsoleColor::Blue << " SS(10): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 80) << ConsoleColor::Blue << " SS(11): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 88);
	console << ConsoleColor::Blue << "\rSS(12): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 96) << ConsoleColor::Blue << " SS(13): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 104) << ConsoleColor::Blue << " SS(14): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 112);
	console << ConsoleColor::Blue << "\rSS(15): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 120) << ConsoleColor::Blue << " SS(16): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 128) << ConsoleColor::Blue << " SS(17): " << ConsoleColor::Gray << *((uint64_t *)task->rsp - 136);
}

/**
 * Handle interrupts
 *
 * @param task		The TaskState of the current task
 */
void handle_interrupt(TaskState *task)
{
	asm("cli");

	console << ConsoleColor::DarkGray << "Interrupt " << ConsoleState::Decimal << task->interrupt << "! " << ConsoleColor::Gray;

	switch(task->interrupt)
	{
		case 8:
		{
			while(1);

			break;
		}

		// IRQ 0
		case 32:
		{
			timer += 1000;

			freemem = memory.GetAvailableMemory();

			console << ConsoleArea::Bottom << ConsoleColor::Blue << "\rTime: " << ConsoleState::Decimal << (timer / 250000) / 60 << " min " << (timer / 250000) % 60 << " s  Free Memory: " << (freemem / (1024 * 1024 * 1024)) % 1024 << " GiB " << (freemem / (1024 * 1024)) % 1024 << " MiB " << (freemem / 1024) % 1024 << " KiB " << (freemem % 1024) << " B    " << ConsoleColor::Gray << ConsoleArea::Middle;
			if(timer % 400 == 0)
			{
				timer += 1;
			}

			break;
		}

		case 33:
		{
			uint8_t c = in8(0x60);
			console << ConsoleState::Hex << c;
			break;
		}

		case 81:
		{
			console << ConsoleColor::Yellow <<  "System Interrupt\r\n";
			dump_info(task);
			break;
		}
	}

	if(task->interrupt >= 0x20 && task->interrupt <= 0x2F)
	{
		if(task->interrupt >= 0x28)
		{
			out8(0xA0, 0x20);
		}

		out8(0x20, 0x20);

		//asm("int $0");
		console << ConsoleColor::Green << "new " << ConsoleColor::Gray << "variable @ ";
		uint64_t *b = new uint64_t;
		console << ConsoleState::HexFixed << ConsoleColor::Blue << (uint64_t)b << " ";
	}
	else if(task->interrupt < 0x20)
	{
		console << ConsoleColor::Red << "\r\nOMGWTFBBQ?! The kernel encountered an exception and cannot continue execution!\r\n";

		dump_info(task);

		while(1) { asm("hlt"); }
	}
}
