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

#include <task.h>
#include <memory.h>
#include <paging.h>
#include <managers.h>
#include <string.h>

Task::Task(uintptr_t entry) : paging(memory.PAlloc())
{
	memset((void *)&this->state, 0, sizeof(TaskState));

	this->state.rip = entry;
	this->state.rsp = memory.PAlloc();
	this->state.rflags = 0x202;
	this->state.cs = gdt.GetDescriptor(GDTEntry(GDTMode::LongMode, GDTType::Code, GDTRing::Ring3));
	this->state.ds = gdt.GetDescriptor(GDTEntry(GDTMode::ProtectedMode, GDTType::Data, GDTRing::Ring3));
	this->state.es = gdt.GetDescriptor(GDTEntry(GDTMode::ProtectedMode, GDTType::Data, GDTRing::Ring3));
	this->state.ss = gdt.GetDescriptor(GDTEntry(GDTMode::ProtectedMode, GDTType::Data, GDTRing::Ring3));

	if(!this->state.cs || !this->state.ds || !this->state.es || !this->state.fs)
	{
		while(1)
		{
			asm("cli;hlt");
		}
	}

	this->paging.Map(0xFFFFFFFC00000000, this->paging.Address());
	this->paging.Map(0xFFFFFFFC00001000, this->state.rsp);

	this->paging.Map(this->state.rip & ~0x0FFF, this->state.rip & ~0x0FFF);
}

Task::~Task()
{
}

TaskManager::TaskManager(uintptr_t kernel)
{
	if(kernel != (uintptr_t)NULL)
	{
		this->running = NULL;
		this->active = false;

		// Setup idle task
		Task *idle = (Task *)memory.PAlloc();

		memset((void *)idle, 0, 4096);

		idle->paging = paging;
		idle->state.rip = kernel;
		idle->state.rsp = memory.PAlloc();
		idle->state.rflags = 0x202;
		idle->state.cs = gdt.GetDescriptor(GDTEntry(GDTMode::LongMode, GDTType::Code, GDTRing::Ring0));
		idle->state.ds = gdt.GetDescriptor(GDTEntry(GDTMode::ProtectedMode, GDTType::Data, GDTRing::Ring0));
		idle->state.es = gdt.GetDescriptor(GDTEntry(GDTMode::ProtectedMode, GDTType::Data, GDTRing::Ring0));
		idle->state.ss = gdt.GetDescriptor(GDTEntry(GDTMode::ProtectedMode, GDTType::Data, GDTRing::Ring0));

		if(!idle->state.cs || !idle->state.ds || !idle->state.es || !idle->state.ss)
		{
			while(1)
			{
				asm("cli;hlt");
			}
		}

		this->tasks.Add(idle);
	}
}

TaskManager::~TaskManager()
{
}

void TaskManager::Schedule(TaskState *state)
{
	if(this->active)
	{
		memcpy((void *)(&this->running->Data->state), (const void *)state, sizeof(TaskState));
	}
	else
	{
		this->running = this->tasks.End();
		this->active = true;
	}

	if(this->running->Next == NULL)
	{
		this->running = this->tasks.Start();
	}
	else
	{
		this->running = this->running->Next;
	}

	memcpy((void *)state, (const void *)(&this->running->Data->state), sizeof(TaskState));
}
