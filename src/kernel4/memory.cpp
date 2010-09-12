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

MemoryBitmap::MemoryBitmap()
{
	memset(this->bitmap, 0, 4096);
}

MemoryBitmap::~MemoryBitmap()
{
}

uint64_t MemoryBitmap::Size()
{
	return 0;
}

uintptr_t MemoryBitmap::Find(uint64_t blocks, bool low)
{
	blocks = 0;
	low = false;

	return NULL;
}

MemoryStack::MemoryStack()
{
	this->bottom = NULL;
	this->stack = &this->bottom;
	this->size = 0;
}

MemoryStack::~MemoryStack()
{
}

void MemoryStack::Push(uintptr_t address)
{
	*(uint64_t **)address = this->stack;

	this->stack = (uintptr_t *)address;

	this->size++;
}

uintptr_t MemoryStack::Pop()
{
	uintptr_t page = (uintptr_t)this->stack;

	if(page == (uintptr_t)&this->bottom)
	{
		console << ConsoleColor::Red << "Memory stack is empty! Halting system...";

		while(1)
		{
			asm("cli;hlt;");
		}
	}

	this->stack = *(uintptr_t **)this->stack;

	this->size--;

	return page;
}

uint64_t MemoryStack::Size()
{
	return this->size;
}

MemoryManager::MemoryManager()
{
	//this->tree = SplayTree<MemoryBlock>();

	this->bitmap = MemoryBitmap();
	this->stack = MemoryStack();
	this->virtualizer = 0xFFFFFFFF80000000;
}

MemoryManager::~MemoryManager()
{
}

uint64_t MemoryManager::GetAvailableMemory()
{
	//return memory.GetSize() * 0x1000;

	//return this->tree.Size() * 0x1000;

	return (this->bitmap.Size() + this->stack.Size()) * 0x1000;
}

void MemoryManager::VFree(uintptr_t address)
{
	/*
	//TODO: Replace with actual function
	//address = NULL;
	paging.UnMap(address);

	this->PFree(address);
	address = NULL;
	*/

	if(address == (uintptr_t)NULL)
	{
		return;
	}

	uint64_t pages = address - sizeof(uint64_t);
	address -= sizeof(uint64_t);

	for(uint64_t i = 0; i < pages; i++)
	{
		paging.UnMap(address + i * 0x1000);

		this->PFree(address + i * 0x1000);
	}

	address = NULL;
}

uintptr_t MemoryManager::VAlloc(uint64_t size, bool low)
{
	/*
	//TODO: Replace with actual function
	//return NULL;
	uintptr_t address = this->PAlloc();

	paging.Map(address, address);

	memset((void *)address, 0, 4096);

	return address;
	*/

	uint64_t pages = (size + sizeof(uint64_t) + 0x0FFF) / 0x1000;
	uintptr_t address = NULL;

	for(uint64_t i = 0; i < pages; i++)
	{
		uintptr_t temporary = this->PAlloc(low);

		if(temporary == (uintptr_t)NULL)
		{
			console << ConsoleColor::Red << "Invalid memory block allocated!\r\nHalting system!";
		}

		if(address == (uintptr_t)NULL)
		{
			address = temporary;
		}

		paging.Map(this->next_free_virtual(), temporary);
	}

	memset((void *)address, 0, pages * 0x1000);

	*(uint64_t *)address = pages;

	return address + sizeof(uint64_t);
}

uintptr_t MemoryManager::next_free_virtual()
{
	uintptr_t virtualizer = this->virtualizer;

	this->virtualizer += 0x1000;

	return virtualizer;
}

void MemoryManager::Initialize(uintptr_t address, uint64_t length)
{
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
}

void MemoryManager::PFree(uintptr_t block, uint64_t blocks)
{
	/*
	// Comment out for speed
	//memset((void *)block, 0, 4096);

	SplayTreeNode<MemoryBlock> *node = (SplayTreeNode<MemoryBlock> *)block;
	node->Left = NULL;
	node->Right = NULL;
	node->Data = (MemoryBlock *)(block + sizeof(SplayTreeNode<MemoryBlock>));
	node->Data->Address = (block + sizeof(SplayTreeNode<MemoryBlock>) + sizeof(MemoryBlock));
	node->Data->Size = 4096 - sizeof(SplayTreeNode<MemoryBlock>) - sizeof(MemoryBlock);

	tree.Add(node);
	*/

	if(block < 0x08000000)
	{
		blocks = 0;
	}
	else
	{
	}

	return this->stack.Push(block);
}

uintptr_t MemoryManager::PAlloc(uint64_t blocks, bool low)
{
	if(low || (blocks != 1))
	{
		this->bitmap.Find(blocks, low);

		return NULL;
	}
	else
	{
		if(this->stack.Size() == 0)
		{
			// TODO: Swap pages
			console << ConsoleColor::Red << "No further memory available!\r\nHalting system! (For now, should swap)";

			while(1)
			{
				asm("cli;hlt");
			}
		}

		return this->stack.Pop();
	}

	return NULL;

	/*
	if(blocks_to_allocate == 1)
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
	else if(blocks_to_allocate > 1)
	{
		uint8_t allocated_blocks = 0;

		SplayTreeNode<MemoryBlock> *nodes[256];

		do
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

			nodes[allocated_blocks] = tree.Top();

			tree.Remove(nodes[allocated_blocks]);

			tree.Search(nodes[allocated_blocks]->Data);

			if((allocated_blocks < blocks_to_allocate) && (tree.Top()->Data->Address == (nodes[allocated_blocks]->Data->Address - 0x1000)))
			{
				allocated_blocks++;
			}
			else
			{
				nodes[255] = tree.Top();

				while(allocated_blocks > 0)
				{
					tree.Add(nodes[allocated_blocks]);

					allocated_blocks--;
				}

				tree.Search(nodes[255]->Data);
			}
		}
		while(allocated_blocks < blocks_to_allocate);

		if(allocated_blocks == (blocks_to_allocate))
		{
			//for(allocated_blocks = 0; allocated_blocks < blocks_to_allocate; allocated_blocks++)
			//{
			//	tree.Remove(nodes[allocated_blocks]);
			//}

			return ((uintptr_t)nodes[allocated_blocks - 1]->Data->Address - sizeof(SplayTreeNode<MemoryBlock>) - sizeof(MemoryBlock));
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}*/
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
