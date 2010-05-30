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

	if(!this->state.cs | !this->state.ds | !this->state.es | !this->state.fs)
		while(1){asm("cli;hlt");}
}

Task::~Task()
{
}

TaskManager::TaskManager(uintptr_t kernel)
{
	
}
