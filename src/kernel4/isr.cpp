#include <isr.h>
#include <managers.h>
#include <task.h>
#include <io.h>

uint64_t timer = 0; // Uptime
uint64_t freemem = 0; // Amount of free memory

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
			while(1);

			break;
		// IRQ 0
		case 32:
			timer += 1000;

			freemem = memory.GetAvailableMemory();

			console << ConsoleArea::Bottom << ConsoleColor::Blue << "\rTime: " << ConsoleState::Decimal << (timer / 250000) / 60 << " min " << (timer / 250000) % 60 << " s  Free Memory: " << (freemem / (1024 * 1024 * 1024)) % 1024 << " GiB " << (freemem / (1024 * 1024)) % 1024 << " MiB " << (freemem / 1024) % 1024 << "KiB   " << ConsoleColor::Gray << ConsoleArea::Middle;
			if(timer % 400 == 0)
			{
				timer += 1;
			}

			break;
		case 33:
			uint8_t c = in8(0x60);
			console << ConsoleState::Hex << c;
			break;
	}

	if(task->interrupt >= 0x20 && task->interrupt <= 0x2F)
	{
		if(task->interrupt >= 0x28)
		{
			out8(0xA0, 0x20);
		}

		out8(0x20, 0x20);

		//asm("int $0");
		uint64_t *b = new uint64_t;
		console << ConsoleColor::Green << "new " << ConsoleColor::Gray << "variable @ " << ConsoleState::HexFixed << ConsoleColor::Blue << (uint64_t)b << " ";
	}
	else
	{
		console << ConsoleColor::Red << "\r\nOMGWTFBBQ?! The kernel encountered an exception and cannot continue execution!" << ConsoleColor::Blue << "\r\nRegister Dump:";
		console	<< ConsoleState::HexFixed;
		console << ConsoleColor::Blue << "\r\n    SS: " << ConsoleColor::Gray << task->rsp << ConsoleColor::Blue << "    RSP: " << ConsoleColor::Gray << task->rsp << ConsoleColor::Blue << " RFLAGS: " << ConsoleColor::Gray << task->rflags;
		console << ConsoleColor::Blue << "\r    CS: " << ConsoleColor::Gray << task->cs << ConsoleColor::Blue << "    RIP: " << ConsoleColor::Gray << task->rip;
		console << ConsoleColor::Blue << "\r\n   INT: " << ConsoleColor::Gray << task->interrupt << ConsoleColor::Blue << "  ERROR: " << ConsoleColor::Gray << task->error;
		console << ConsoleColor::Blue << "\r\n    DS: " << ConsoleColor::Gray << task->ds << ConsoleColor::Blue << "     ES: " << ConsoleColor::Gray << task->es;
		console << ConsoleColor::Blue << "\r\n    FS: " << ConsoleColor::Gray << task->fs << ConsoleColor::Blue << "     GS: " << ConsoleColor::Gray << task->gs;
		console << ConsoleColor::Blue << "\r\n   RAX: " << ConsoleColor::Gray << task->rax << ConsoleColor::Blue << "    RBX: " << ConsoleColor::Gray << task->rbx << ConsoleColor::Blue << "    RCX: " << ConsoleColor::Gray << task->rcx;
		console << ConsoleColor::Blue << "\r   RDX: " << ConsoleColor::Gray << task->rdx << ConsoleColor::Blue << "    RDI: " << ConsoleColor::Gray << task->rdi << ConsoleColor::Blue << "    RSI: " << ConsoleColor::Gray << task->rsi;
		console << ConsoleColor::Blue << "\r   RBP: " << ConsoleColor::Gray << task->rbp << ConsoleColor::Blue << "     R8: " << ConsoleColor::Gray << task->r8 << ConsoleColor::Blue << "     R9: " << ConsoleColor::Gray << task->r9;
		console << ConsoleColor::Blue << "\r   R10: " << ConsoleColor::Gray << task->r10 << ConsoleColor::Blue << "    R11: " << ConsoleColor::Gray << task->r10 << ConsoleColor::Blue << "    R12: " << ConsoleColor::Gray << task->r12;
		console << ConsoleColor::Blue << "\r   R13: " << ConsoleColor::Gray << task->r13 << ConsoleColor::Blue << "    R14: " << ConsoleColor::Gray << task->r14 << ConsoleColor::Blue << "    R15: " << ConsoleColor::Gray << task->r15;

		while(1) { asm("hlt"); }
	}
}
