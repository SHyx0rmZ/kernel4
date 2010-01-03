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
#include <paging.h>
#include <managers.h>

PagingManager::PagingManager(uintptr_t address)
{
	this->page_map_level_4 = (PageMapLevel4Entry *)address;
}

PagingManager::~PagingManager()
{
}

void PagingManager::Map(uintptr_t virtual_address, uintptr_t physical_address)
{
	this->UpdateIndexes(virtual_address & 0x000FFFFFFFFFF000LL);

	PageMapLevel4Entry *pml4e = (PageMapLevel4Entry *)(page_map_level_4[pml4i].GetAddress());

	if(pml4e->IsPresent() == false)
	{
		pml4e->Clear();
		pml4e->SetAccess(PageAccess::UserWritable);
		pml4e->SetAddress(memory.PAlloc());
		pml4e->SetCachability(PageCachability::WriteThroughCachable);
		pml4e->SetPresence(true);
	}

	PageDirectoryPointerEntry *pdpe = (PageDirectoryPointerEntry *)(pml4e[pdpi].GetAddress());

	if(pdpe->IsPresent() == false)
	{
		pdpe->Clear();
		pdpe->SetAccess(PageAccess::UserWritable);
		pdpe->SetAddress(memory.PAlloc());
		pdpe->SetCachability(PageCachability::WriteThroughCachable);
		pdpe->SetPresence(true);
	}

	PageDirectoryEntry *pde = (PageDirectoryEntry *)(pdpe[pdi].GetAddress());

	if(pde->IsPresent() == false)
	{
		pde->Clear();
		pde->SetAccess(PageAccess::UserWritable);
		pde->SetAddress(memory.PAlloc());
		pde->SetCachability(PageCachability::WriteThroughCachable);
		pde->SetPresence(true);
	}

	PageTableEntry *pte = (PageTableEntry *)(pde[pti].GetAddress());

	pte->Clear();
	pte->SetAccess(PageAccess::UserWritable);
	pte->SetAddress(physical_address);
	pte->SetCachability(PageCachability::WriteThroughCachable);
	pte->SetPresence(true);
}

void PagingManager::UnMap(uintptr_t virtual_address)
{
	this->UpdateIndexes(virtual_address & 0x000FFFFFFFFFF000LL);	
	
	PageMapLevel4Entry *pml4e = (PageMapLevel4Entry *)(page_map_level_4[pml4i].GetAddress());

	if(pml4e->IsPresent() == false)
	{
		return;
	}

	PageDirectoryPointerEntry *pdpe = (PageDirectoryPointerEntry *)(pml4e[pdpi].GetAddress());

	if(pdpe->IsPresent() == false)
	{
		return;
	}

	PageDirectoryEntry *pde = (PageDirectoryEntry *)(pdpe[pdi].GetAddress());

	if(pde->IsPresent() == false)
	{
		return;
	}

	PageTableEntry *pte = (PageTableEntry *)(pde[pti].GetAddress());

	pte->SetPresence(false);
}

void PagingManager::UpdateIndexes(uintptr_t address)
{
	this->pml4i = (address >> 39) & 511;
	this->pdpi = (address >> 30) & 511;
	this->pdi = (address >> 21) & 511;
	this->pti = (address >> 12) & 511;
}

PagingStructure::PagingStructure()
{
	this->information = 0;
}

PagingStructure::~PagingStructure()
{
}

void PagingStructure::Clear()
{
	this->information = 0;
}

PageAccess PagingStructure::GetAccess()
{
	return (PageAccess)((this->information >> 1) >> 3);
}

uintptr_t PagingStructure::GetAddress()
{
	return (this->information & ~0xFFF0000000000FFFLL);
}

PageCachability PagingStructure::GetCachability()
{
	return (PageCachability)((this->information >> 3) & 3);
}

bool PagingStructure::IsPresent()
{
	return (this->information & 1);
}

void PagingStructure::SetAccess(PageAccess access)
{
	this->information &= ~(3 << 1);
	this->information |= ((uint8_t)access << 1);
}

void PagingStructure::SetAddress(uintptr_t address)
{
	this->information &= 0xFFF0000000000FFF;
	this->information |= (~0xFFF0000000000FFF & address);
}

void PagingStructure::SetCachability(PageCachability cachability)
{
	this->information &= ~(3 << 3);
	this->information |= ((uint8_t)cachability << 3);
}

void PagingStructure::SetPresence(bool present)
{
	this->information &= ~1;
	this->information |= (present & 1);
}

PageMapLevel4Entry::PageMapLevel4Entry() : PagingStructure()
{
}

PageMapLevel4Entry::~PageMapLevel4Entry()
{
}

PageDirectoryPointerEntry::PageDirectoryPointerEntry() : PagingStructure()
{
}

PageDirectoryPointerEntry::~PageDirectoryPointerEntry()
{
}

PageDirectoryEntry::PageDirectoryEntry() : PagingStructure()
{
}

PageDirectoryEntry::~PageDirectoryEntry()
{
}

PageTableEntry::PageTableEntry()
{
}

PageTableEntry::~PageTableEntry()
{
}
