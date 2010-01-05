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

#include <stdint.h>
#include <stddef.h>
#include <task.h>
#include <managers.h>
#include <string.h>

TaskManager::TaskManager(uintptr_t position, uint16_t maximum_task_number)
{
	this->tasks = (TaskState *)position;
	this->limit = maximum_task_number;
	this->current = 0;

	for(uint16_t i = 0; i < 1024; i++)
	{
		this->running[i] = 0;
	}

	uintptr_t e;

	asm(
		"movq -0x10(%%rbp), %0 \n"
		: "=r" (e)
	);
}

TaskManager::~TaskManager()
{
}

uint16_t TaskManager::GetCurrent()
{
	return current;
}

void TaskManager::Create(uintptr_t entry)
{
	uint16_t i;

	for(i = 0; i < 1024; i++)
	{
		if(running[i] != 0xFFFFFFFFFFFFFFFFLL)
		{
			break;
		}
	}

	uint8_t j;

	for(j = 0; j < 64; j++)
	{
		if((running[i] & (1 << j)) == 0)
		{
			break;
		}
	}

	uint16_t d = (i * 64) + j;

	if(d >= this->limit)
	{
		console << ConsoleColor::Red << "Can't create task!";

		while(1) { asm("hlt"); }
	}

	memset((void *)&tasks[d], 0, sizeof(TaskState));

	tasks[d].rip = entry;
	tasks[d].rsp = memory.PAlloc();
	tasks[d].rflags = 0x202;
	tasks[d].cs = 8;
	tasks[d].ds = 16;
	tasks[d].es = 16;
	tasks[d].ss = 16;

	running[i] |= 1 << j;

	this->number++;
}

void TaskManager::Schedule(TaskState *state)
{
	memmove((void *)&this->tasks[this->current], (void *)state, sizeof(TaskState));
	this->current = (this->current + 1) % this->number;
	memmove((void *)state, (void *)&this->tasks[this->current], sizeof(TaskState));
}

void TaskManager::DestroyCurrent()
{
	console << ConsoleColor::Red << "TaskManager::DestroyCurrent() not yet implemented!";
	while(1) { asm("hlt"); }
}
