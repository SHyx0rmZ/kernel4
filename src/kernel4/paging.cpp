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
#include <paging.h>
#include <managers.h>

PagingManager::PagingManager(uintptr_t address)
{
	this->page_map_level_4 = (PageMapLevel4Entry *)address;	
	this->dynamic_page = (PageTableEntry *)0x305FF8LL;
	this->static_pointer = (PagingStructure *)0xFFFFFFFFFFFFF000LL;
}

PagingManager::~PagingManager()
{
}

void PagingManager::Load()
{
	asm(
		"mov %0, %%rax \n"
		"mov %%rax, %%cr3 \n"
		: : "g" (*this)
	);
}

void PagingManager::Invalidate(uintptr_t virtual_address)
{
	asm(
		"invlpg %0 \n"
		: : "m" (*(char *)virtual_address)
	);
}

void PagingManager::Invalidate(PagingStructure *page)
{
	this->Invalidate((uintptr_t)page);
}

void PagingManager::Map(uintptr_t virtual_address, uintptr_t physical_address)
{
	this->UpdateIndexes(virtual_address & 0x000FFFFFFFFFF000LL);

	this->dynamic_page->SetAddress((uintptr_t)page_map_level_4);

	this->Invalidate(this->static_pointer);

	PageMapLevel4Entry *pml4e = (PageMapLevel4Entry *)(&this->static_pointer[pml4i]);

	if(pml4e->IsPresent() == false)
	{
		pml4e->Clear();
		pml4e->SetAccess(PageAccess::UserWritable);
		pml4e->SetAddress(memory.PAlloc());
		pml4e->SetCachability(PageCachability::WriteThroughCachable);
		pml4e->SetPresence(true);
	}

	this->dynamic_page->SetAddress(pml4e->GetAddress());

	this->Invalidate(this->static_pointer);

	PageDirectoryPointerEntry *pdpe = (PageDirectoryPointerEntry *)(&this->static_pointer[pdpi]);

	if(pdpe->IsPresent() == false)
	{
		pdpe->Clear();
		pdpe->SetAccess(PageAccess::UserWritable);
		pdpe->SetAddress(memory.PAlloc());
		pdpe->SetCachability(PageCachability::WriteThroughCachable);
		pdpe->SetPresence(true);
	}

#if PAGESIZE == 4

	this->dynamic_page->SetAddress(pdpe->GetAddress());

	this->Invalidate(this->static_pointer);

	PageDirectoryEntry *pde = (PageDirectoryEntry *)(&this->static_pointer[pdi]);

	if(pde->IsPresent() == false)
	{
		pde->Clear();
		pde->SetAccess(PageAccess::UserWritable);
		pde->SetAddress(memory.PAlloc());
		pde->SetCachability(PageCachability::WriteThroughCachable);
		pde->SetPresence(true);
	}

	this->dynamic_page->SetAddress(pde->GetAddress());

	this->Invalidate(this->static_pointer);

	PageTableEntry *pte = (PageTableEntry *)(&this->static_pointer[pti]);

	pte->Clear();
	pte->SetAccess(PageAccess::UserWritable);
	pte->SetAddress(physical_address);
	pte->SetCachability(PageCachability::WriteThroughCachable);
	pte->SetPresence(true);

	asm(
		"invlpg %0 \n"
		: : "m" (*(char *)virtual_address)
	);

#elif PAGESIZE == 2

	this->dynamic_page->SetAddress(pdpe->GetAddress());

	this->Invalidate(this->static_pointer);

	PageDirectoryEntry *pde = (PageDirectoryEntry *)(&this->static_pointer[pdi]); 

	pde->Clear();
	pde->SetAccess(PageAccess::UserWritable);
	pde->SetAddress(physical_address);
	pde->SetCachability(PageCachability::WriteThroughCachable);
	pde->SetPresence(true);

	this->Invalidate(virtual_address);

#endif
}

void PagingManager::UnMap(uintptr_t virtual_address)
{
	this->UpdateIndexes(virtual_address & 0x000FFFFFFFFFF000LL);	

	this->dynamic_page->SetAddress((uintptr_t)page_map_level_4);

	this->Invalidate(this->static_pointer);

	PageMapLevel4Entry *pml4e = (PageMapLevel4Entry *)(&this->static_pointer[pml4i]);
	//PageMapLevel4Entry *pml4e = (PageMapLevel4Entry *)(page_map_level_4[pml4i].GetAddress());

	if(pml4e->IsPresent() == false)
	{
		return;
	}

	this->dynamic_page->SetAddress(pml4e->GetAddress());

	this->Invalidate(this->static_pointer);

	PageDirectoryPointerEntry *pdpe = (PageDirectoryPointerEntry *)(&this->static_pointer[pdpi]);
	//PageDirectoryPointerEntry *pdpe = (PageDirectoryPointerEntry *)(pml4e[pdpi].GetAddress());

	if(pdpe->IsPresent() == false)
	{
		return;
	}

#if PAGESIZE == 4

	this->dynamic_page->SetAddress(pdpe->GetAddress());

	this->Invalidate(this->static_pointer);

	PageDirectoryEntry *pde = (PageDirectoryEntry *)(&this->static_pointer[pdi]);
	//PageDirectoryEntry *pde = (PageDirectoryEntry *)(pdpe[pdi].GetAddress());

	if(pde->IsPresent() == false)
	{
		return;
	}

	this->dynamic_page->SetAddress(pde->GetAddress());

	this->Invalidate(this->static_pointer);

	PageTableEntry *pte = (PageTableEntry *)(&this->static_pointer[pti]);
	//PageTableEntry *pte = (PageTableEntry *)(pde[pti].GetAddress());

	pte->SetPresence(false);

#elif PAGESIZE == 2

	this->dynamic_page->SetAddress(pdpe->GetAddress());

	this->Invalidate(this->static_pointer);

	PageDirectoryEntry *pde = (PageDirectoryEntry *)(&this->static_pointer[pdi]);
	//PageDirectoryEntry *pde = (PageDirectoryEntry *)(pdpe[pdi].GetAddress());

	pde->SetPresence(false);

#endif

	this->Invalidate(virtual_address);
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
	//uintptr_t old = this->GetAddress();

	this->information &= 0xFFF0000000000FFF;
	this->information |= (~0xFFF0000000000FFF & address);

	//asm("invlpg %0 \n" : : "g" (address));
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

#if PAGESIZE == 2

	this->information = 0x80;

#endif

}

PageDirectoryEntry::~PageDirectoryEntry()
{
}

void PageDirectoryEntry::Clear()
{

#if PAGESIZE == 2

	this->information = 0x80;

#endif

}

PageTableEntry::PageTableEntry()
{

#if PAGESIZE == 4

	this->information = 0x80;

#endif

}

PageTableEntry::~PageTableEntry()
{
}

void PageTableEntry::Clear()
{

#if PAGESIZE == 4

	this->information = 0x80;

#endif

}
