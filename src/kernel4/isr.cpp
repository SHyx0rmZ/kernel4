#include <isr.h>
#include <managers.h>
#include <task.h>
#include <io.h>

uint64_t timer = 0;
uint64_t freemem = 0;

void handle_interrupt(TaskState *task)
{
	asm("cli");

	console << ConsoleColor::DarkGray << "Interrupt " << ConsoleState::Decimal << task->interrupt << "! " << ConsoleColor::Gray;

	switch(task->interrupt)
	{
		case 8:
			while(1);

			break;
		case 32:
			timer += 1000;

			freemem = memory.GetAvailableMemory();

			console << ConsoleArea::Bottom << ConsoleColor::Blue << "\rTime: " << ConsoleState::Decimal << (timer / 250000) / 60 << " min " << (timer / 250000) % 60 << " s  Free Memory: " << (freemem / (1024 * 1024 * 1024)) % 1024 << " GiB " << (freemem / (1024 * 1024)) % 1024 << " MiB " << (freemem / 1024) % 1024 << "KiB" << ConsoleColor::Gray << ConsoleArea::Middle;
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

		asm("int $0");
	}
	else
	{
		console << ConsoleColor::Red << "OMGWTFBBQ?! The kernel encountered an exception and cannot continue execution!";

		while(1) { asm("hlt"); }
	}
}
