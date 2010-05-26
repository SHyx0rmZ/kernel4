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

#include <config.h>
#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <console.h>
#include <managers.h>
#include <string.h>

extern const uintptr_t start_kernel;
extern const uintptr_t end_kernel;

MemoryManager::MemoryManager()
{
	this->tree = SplayTree<MemoryBlock>();
}

MemoryManager::~MemoryManager()
{
}

uint64_t MemoryManager::GetAvailableMemory()
{
	//return memory.GetSize() * 0x1000;

	return this->tree.Size() * 0x1000;
}

void MemoryManager::VFree(uintptr_t address)
{
	//TODO: Replace with actual function
	//address = NULL;
	paging.UnMap(address);

	this->PFree(address);
	address = NULL;
}

uintptr_t MemoryManager::VAlloc()
{
	//TODO: Replace with actual function
	//return NULL;
	uintptr_t address = this->PAlloc();

	paging.Map(address, address);

	return address;
}

void MemoryManager::Initialize(uintptr_t address, uint64_t length)
{


#if PAGESIZE == 4

	uintptr_t bs = ((address + 0x0FFF) & ~0xFFF);
	uintptr_t be = ((address + length) & ~0xFFF);

	if((bs < (uintptr_t)&start_kernel && be < (uintptr_t)&start_kernel && bs < paging.Address() && be < paging.Address()) || (bs > (uintptr_t)&end_kernel && be > (uintptr_t)&end_kernel && bs > paging.Address() + 0x8000 && be > paging.Address() + 0x8000))
	{
		while(bs < be && (be - bs) >= 0x1000)
		{
			if(bs >= 0x10000000)
				paging.Map(bs, bs);

			this->PFree(bs);

			bs += 0x1000;
		}
	}
	else
	{
		while(bs < be && (be - bs) >= 0x1000)
		{
			if((bs < (uintptr_t)&start_kernel && (bs + 0x1000) < (uintptr_t)&start_kernel && bs < paging.Address() && be < paging.Address()) || (bs > (uintptr_t)&end_kernel && (bs + 0x1000) > (uintptr_t)&end_kernel && bs > paging.Address() + 0x8000 && be > paging.Address() + 0x8000))
			{
				if(bs >= 0x10000000)
					paging.Map(bs, bs);

				this->PFree(bs);
			}

			bs += 0x1000;
		}
	}

#elif PAGESIZE == 2

	//TODO: BUGFIX
	asijg39zulajsd

	uintptr_t bs = ((address + 0x1FFFFF) & ~0x1FFFFF);
	uintptr_t be = ((address + length) & ~0x1FFFFF);

	if((bs < start_kernel && be < start_kernel) || (bs > end_kernel && be > end_kernel))
	{
		while(bs < be && (be - bs) >= 0x200000)
		{
			this->PFree(bs);

			bs += 0x200000;
		}
	}
	else
	{
		while(bs < be && (be - bs) >= 0x200000)
		{
			if((bs < start_kernel && (bs + 0x200000) < start_kernel) || (bs > end_kernel && (bs + 0x200000) > end_kernel))
			{
				this->PFree(bs);
			}

			bs += 0x200000;
		}
	}

#endif

}

void MemoryManager::PFree(uintptr_t block)
{
	memset((void *)block, 0, 4096);

	SplayTreeNode<MemoryBlock> *node = (SplayTreeNode<MemoryBlock> *)block;
	node->Left = NULL;
	node->Right = NULL;
	node->Data = (MemoryBlock *)(block + sizeof(SplayTreeNode<MemoryBlock>));
	node->Data->Address = (block + sizeof(SplayTreeNode<MemoryBlock>) + sizeof(MemoryBlock));
	node->Data->Size = 4096 - sizeof(SplayTreeNode<MemoryBlock>) - sizeof(MemoryBlock);

	tree.Add(node);
}

uintptr_t MemoryManager::PAlloc()
{
	if(tree.Size() == 0)
	{
		Console console;

		console << ConsoleColor::Red << "No further memory available!\r\nHalting system!";

		while(1)
		{
			asm("cli;hlt");
		}
	}

	SplayTreeNode<MemoryBlock> *node = tree.Top();

	tree.Remove(node);

	return ((uintptr_t)node->Data->Address - sizeof(SplayTreeNode<MemoryBlock>) - sizeof(MemoryBlock));
}

MemoryBlock::MemoryBlock()
{
	this->Address = NULL;
	this->Size = 0;
}

MemoryBlock::~MemoryBlock()
{
}

bool MemoryBlock::operator==(const MemoryBlock m)
{
	return (this->Address == m.Address && this->Size == m.Size);
}

bool MemoryBlock::operator>(const MemoryBlock m)
{
	return (this->Address > m.Address);
}

bool MemoryBlock::operator<(const MemoryBlock m)
{
	return (this->Address < m.Address);
}
