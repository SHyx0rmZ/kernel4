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
#include <memory.h>
#include <console.h>

extern const uintptr_t start_kernel;
extern const uintptr_t end_kernel;

MemoryStack::MemoryStack(uintptr_t address)
{
	this->size = 0;
	this->stack = (uintptr_t *)address;
}

MemoryStack::~MemoryStack()
{
}

uint64_t MemoryStack::GetSize()
{
	return this->size;
}

bool MemoryStack::IsEmpty()
{
	return (this->size == 0);
}

uintptr_t MemoryStack::Pop()
{
	if(this->size == 0)
	{
		return NULL;
	}

	return this->stack[size--];
}

void MemoryStack::Push(uintptr_t block)
{
	stack[size++] = block;
}

MemoryBitmap::MemoryBitmap(uintptr_t address, uint16_t size)
{
	this->bitmap = (uint64_t *)address;
	this->size = size;
}

MemoryBitmap::~MemoryBitmap()
{
}

MemoryBitmap MemoryManager::lowmemory = MemoryBitmap(0x1000000, 512);
MemoryStack MemoryManager::memory = MemoryStack(0x1100000);

MemoryManager::MemoryManager()
{
}

MemoryManager::~MemoryManager()
{
}

uint64_t MemoryManager::GetAvailableMemory()
{
	return memory.GetSize() * 0x1000;
}

void MemoryManager::VFree(uintptr_t address)
{
	address = NULL;
}

uintptr_t MemoryManager::VAlloc()
{
	return NULL;
}

void MemoryManager::Initialize(uintptr_t address, uint64_t length)
{
	uintptr_t bs = ((address + 0x0FFF) & ~0xFFF);
	uintptr_t be = ((address + length) & ~0xFFF);

	if(bs < start_kernel && be > end_kernel)
	{
		while(bs < be && (be - bs) >= 0x1000)
		{
			this->PFree(bs);

			bs += 0x1000;
		}
	}
	else
	{
		while(bs < be && (be - bs) >= 0x1000)
		{
			if(bs < start_kernel && (bs + 0x1000) > end_kernel)
			{
				this->PFree(bs);
			}

			bs += 0x1000;
		}
	}
}

void MemoryManager::PFree(uintptr_t block)
{
	memory.Push(block);
}

uintptr_t MemoryManager::PAlloc()
{
	if(memory.IsEmpty())
	{
		Console console;

		console << ConsoleColor::Red << "No further memory available!\r\nHalting system!";

		while(1);
	}

	return memory.Pop();
}
