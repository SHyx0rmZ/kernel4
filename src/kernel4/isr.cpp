#include <isr.h>
#include <managers.h>
#include <task.h>
#include <io.h>

uint64_t timer = 0;

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
			timer++;
			break;
		case 33:
			uint8_t c = in8(0x60);
			console << ConsoleState::Hex << c;
			if(c == 0x39)
				console << ConsoleArea::Bottom << ConsoleColor::Blue << "\rTime: " << ConsoleState::Decimal << timer << ConsoleColor::Gray << ConsoleArea::Middle;
			break;
	}

	if(task->interrupt >= 0x20 && task->interrupt <= 0x2F)
	{
		if(task->interrupt >= 0x28)
		{
			out8(0xA0, 0x20);
		}

		out8(0x20, 0x20);
	}
}
