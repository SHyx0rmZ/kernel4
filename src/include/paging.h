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

#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdint.h>
#include <stddef.h>

enum class PageAccess : uint8_t
{
	UserReadable = 0,
	UserWritable = 1,
	SystemReadable = 2,
	SystemWritable = 3
};

enum class PageCachability : uint8_t
{
	WriteBackCachable = 0,
	WriteThroughCachable = 1,
	WriteBackUnachable = 2,
	WriteThroughUncachable = 3
};

class PagingStructure
{
	public:
		PagingStructure();
		~PagingStructure();

		void Clear();
		PageAccess GetAccess();
		uintptr_t GetAddress();
		PageCachability GetCachability();
		bool IsPresent();
		void SetAccess(PageAccess access);
		void SetAddress(uintptr_t address);
		void SetCachability(PageCachability cachability);
		void SetPresence(bool present);

	protected:
		uint64_t information;
};

class PageMapLevel4Entry : public PagingStructure
{
	public:
		PageMapLevel4Entry();
		~PageMapLevel4Entry();
};

class PageDirectoryPointerEntry : public PagingStructure
{
	public:
		PageDirectoryPointerEntry();
		~PageDirectoryPointerEntry();
};

class PageDirectoryEntry : public PagingStructure
{
	public:
		PageDirectoryEntry();
		~PageDirectoryEntry();

		void Clear();
		bool IsBottom();
};

class PageTableEntry : public PagingStructure
{
	public:
		PageTableEntry();
		~PageTableEntry();
};

class PagingManager
{
	public:
		PagingManager(uintptr_t address);
		~PagingManager();

		void UnMap(uintptr_t virtual_address);
		void Map(uintptr_t virtual_address, uintptr_t physical_address);
		void Load();
		void Invalidate(uintptr_t virtual_address);
		void Invalidate(PagingStructure *page);
		uintptr_t Address();

		static uintptr_t GetActive();

	private:
		PageMapLevel4Entry *page_map_level_4;
		uint16_t pml4i, pdpi, pdi, pti;
		PageTableEntry *dynamic_page;
		PagingStructure *static_pointer;

		void UpdateIndexes(uintptr_t address);
};

#endif
