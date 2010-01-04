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

#ifndef _TASK_H_
#define _TASK_H_

#include <memory.h>

class TaskState
{
	public:
		uint64_t address, length;
		uint64_t ds, es, fs, gs, rax, rbx, rcx, rdx, rsi, rdi, rbp; 
		uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
		uint64_t interrupt, error, rip, cs, rflags, rsp, ss;
} __attribute__((packed));

/*class ExtendedTaskState
{
	public:
		PageBlock *page_blocks;
		uint64_t
		VirtualBlock *virtual_blocks;
};*/

/*class CompleteTaskState
{
	public:
		TaskState *cpu;
		//ExtendedTaskState *process;
}:*/

class TaskManager
{
	public:
		TaskManager(uintptr_t position, uint16_t maximum_task_number);
		~TaskManager();

		void Schedule(TaskState *state);
		void Create(uintptr_t entry);
		void DestroyCurrent();
		uint16_t GetCurrent();

	private:
		static uintptr_t base;
		static uintptr_t position;
		TaskState *tasks;
		uint16_t limit;
		uint16_t current;
		uint16_t number;
		uint64_t running[1024];
};

#endif
