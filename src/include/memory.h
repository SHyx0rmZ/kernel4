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

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>
#include <splay.h>

class MemoryManager
{
	public:
		MemoryManager();
		~MemoryManager();

		uint64_t GetAvailableMemory();
		void Initialize(uintptr_t address, uint64_t length);
		uintptr_t PAlloc();
		void PFree(uintptr_t block);
		uintptr_t VAlloc();
		void VFree(uintptr_t address);

	private:
		SplayTree<uint64_t> tree;
};

#endif
