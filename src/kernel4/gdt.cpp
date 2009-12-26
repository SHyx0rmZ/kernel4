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

#include "gdt.h"

GDTEntry::GDTEntry(GDTMode mode, GDTType type, GDTRing ring, uintptr_t base, uint32_t limit, GDTGranularity granularity, GDTPresence presence)
{
	this->base_low = (base & 0xFFFF);
	this->base_middle = ((base >> 16) & 0xFF);
	this->base_high = ((base >> 24) & 0xFF);
	this->limit_low = (limit & 0xFFFF);
	this->flags_and_limit_high = ((limit >> 16) & 0x0F) | (((uint8_t)mode << 5) & 0x60) | (((uint8_t)granularity << 7) & 0x80);

	switch(type)
	{
		case GDTType::System:
			this->access = 0x00;
			break;
		case GDTType::Code:
			this->access = 0x1A;
			break;
		case GDTType::Data:
			this->access = 0x12;
			break;
	}

	this->access |= (((uint8_t)ring << 5) & 0x60) | (((uint8_t)presence << 7) & 0x80);
}

GDTEntry::~GDTEntry()
{
}

uintptr_t GDTEntry::GetBase()
{
	return (this->base_low | (this->base_middle << 16) | (this->base_high << 24));
}

GDTGranularity GDTEntry::GetGranularity()
{
	return (GDTGranularity)((this->flags_and_limit_high >> 7) & 1);
}

uint32_t GDTEntry::GetLimit()
{
	return (this->limit_low | ((this->flags_and_limit_high & 0x0F) << 16));
}

GDTMode GDTEntry::GetMode()
{
	return (GDTMode)((this->flags_and_limit_high >> 5) & 3);
}

GDTPresence GDTEntry::GetPresence()
{
	return (GDTPresence)((this->access >> 7) & 1);
}

GDTRing GDTEntry::GetRing()
{
	return (GDTRing)((this->access >> 5) & 3);
}

GDTSystemType32 GDTEntry::GetSystemType32()
{
	if(!this->IsSystemSegment())
	{
		return GDTSystemType32::Invalid;
	}

	return (GDTSystemType32)(this->access & 0x0F);
}

GDTSystemType64 GDTEntry::GetSystemType64()
{
	if(!this->IsSystemSegment())
	{
		return GDTSystemType64::Invalid;
	}

	return (GDTSystemType64)(this->access & 0x0F);
}

GDTType GDTEntry::GetType()
{
	if(((this->access >> 4) & 1) == 0)
	{
		return GDTType::System;
	}
	else if((this->access >> 3) & 1)
	{
		return GDTType::Code;
	}
	else
	{
		return GDTType::Data;
	}
}

bool GDTEntry::IsCodeSegment()
{
	return (!this->IsSystemSegment() && ((this->access >> 3) & 1) == 1);
}

bool GDTEntry::IsConforming()
{
	return (this->IsCodeSegment() && ((this->access >> 2) & 1) == 1);
}

bool GDTEntry::IsDataSegment()
{
	return (!this->IsSystemSegment() && ((this->access >> 3) & 1) == 0);
}

bool GDTEntry::IsExecuteOnly()
{
	return (this->IsCodeSegment() && ((this->access >> 1) & 1) == 0);
}

bool GDTEntry::IsExpandDown()
{
	return (this->IsDataSegment() && ((this->access >> 2) & 1) == 1);
}

bool GDTEntry::IsReadOnly()
{
	return (this->IsDataSegment() && ((this->access >> 1) & 1) == 0);
}

bool GDTEntry::IsSystemSegment()
{
	return (((this->access >> 4) & 1) == 0);
}

void GDTEntry::SetBase(uintptr_t address)
{
	this->base_low = (address & 0xFFFF);
	this->base_middle = ((address >> 16) & 0xFF);
	this->base_high = ((address >> 24) & 0xFF);
}

bool GDTEntry::SetConforming(bool conforming)
{
	if(!this->IsCodeSegment())
	{
		return false;
	}

	this->access &= ~(1 << 2);
	this->access |= (conforming << 2);

	return true;
}

bool GDTEntry::SetExecuteOnly(bool executeonly)
{
	if(!this->IsCodeSegment())
	{
		return false;
	}

	this->access &= ~(1 << 1);
	this->access |= (executeonly << 1);
	
	return true;
}

bool GDTEntry::SetExpandDown(bool expanddown)
{
	if(!this->IsDataSegment())
	{
		return false;
	}

	this->access &= ~(1 << 2);
	this->access |= (expanddown << 2);
	
	return true;
}

void GDTEntry::SetGranularity(GDTGranularity granularity)
{
	this->flags_and_limit_high &= ~(1 << 7);
	this->flags_and_limit_high |= ((uint8_t)granularity << 7);
}

void GDTEntry::SetLimit(uint32_t limit)
{
	this->limit_low = (limit & 0xFFFF);
	this->flags_and_limit_high &= 0xF0;
	this->flags_and_limit_high |= ((limit >> 16) & 0x0F);
}

void GDTEntry::SetMode(GDTMode mode)
{
	this->flags_and_limit_high &= ~(3 << 5);
	this->flags_and_limit_high |= ((uint8_t)mode << 5);
}

void GDTEntry::SetPresence(GDTPresence presence)
{
	this->access &= ~(1 << 7);
	this->access |= ((uint8_t)presence << 7);
}

bool GDTEntry::SetReadOnly(bool readonly)
{
	if(!this->IsDataSegment())
	{
		return false;
	}

	this->access &= ~(1 << 1);
	this->access |= (readonly << 1);
	
	return true;
}

void GDTEntry::SetRing(GDTRing ring)
{
	this->access &= ~(3 << 5);
	this->access|= ((uint8_t)ring << 5);
}

bool GDTEntry::SetSystemType(GDTSystemType32 systemtype)
{
	if(!this->IsSystemSegment() || (systemtype == GDTSystemType32::Invalid))
	{
		return false;
	}

	this->access &= 0xF0;
	this->access |= ((uint8_t)systemtype & 0x0F);
	
	return true;
}

bool GDTEntry::SetSystemType(GDTSystemType64 systemtype)
{
	if(!this->IsSystemSegment() || (systemtype == GDTSystemType64::Invalid))
	{
		return false;
	}

	this->access &= 0xF0;
	this->access |= ((uint8_t)systemtype & 0x0F);
	
	return true;
}

bool GDTEntry::SetSystemType32(GDTSystemType32 systemtype)
{
	if(!this->IsSystemSegment() || (systemtype == GDTSystemType32::Invalid))
	{
		return false;
	}

	this->access &= 0xF0;
	this->access |= ((uint8_t)systemtype & 0x0F);
	
	return true;
}

bool GDTEntry::SetSystemType64(GDTSystemType64 systemtype)
{
	if(!this->IsSystemSegment() || (systemtype == GDTSystemType64::Invalid))
	{
		return false;
	}

	this->access &= 0xF0;
	this->access |= ((uint8_t)systemtype & 0x0F);
	
	return true;
}

void GDTEntry::SetType(GDTType type)
{
	switch(type)
	{
		case GDTType::System:
			this->access &= ~0x10;
			break;
		case GDTType::Code:
			this->access |= 0x1A;
			break;
		case GDTType::Data:
			this->access |= 0x12;
			break;
	}
}

GDTTable::GDTTable(uint8_t size, uintptr_t position)
{
	if(size == 0)
	{
		this->limit = 0;
	}
	else
	{
		this->limit = ((size) * 8) - 1;
	}

	this->base = (GDTEntry *)position;
}

GDTTable::~GDTTable()
{
}


GDTEntry GDTTable::GetEntry(uint8_t index)
{
	return this->base[index];
}

uint8_t GDTTable::GetSize()
{
	return (this->limit + 1) / 8;
}

bool GDTTable::IsActive()
{
	uint16_t temp_limit = this->limit;
	GDTEntry *temp_base = this->base;

	asm(
		"sgdt %0 \n"
		: : "m" (*this)
	);

	bool same = ((this->limit == temp_limit) && (this->base == temp_base));

	this->limit = temp_limit;
	this->base = temp_base;

	return same;
}

void GDTTable::MakeActive()
{
	asm(
		"lgdt %0 \n"

		: : "g" (*this)
	);
}

void GDTTable::ReloadSegment(GDTSegmentRegister segment, uint8_t index)
{
	switch(segment)
	{
		case GDTSegmentRegister::CS:
			asm(
				"push %%rax \n"
				"push $1f \n"
				"rex.w retf \n"
				"1: \n"
				: : "a" (index << 3)
			);
			break;
		case GDTSegmentRegister::DS:
			asm(
				"mov %0, %%ds \n"
				: : "r" (index << 3)
			);
			break;
		case GDTSegmentRegister::ES:
			asm(
				"mov %0, %%es \n"
				: : "r" (index << 3)
			);
			break;
		case GDTSegmentRegister::FS:
			asm(
				"mov %0, %%fs \n"
				: : "r" (index << 3)
			);
			break;
		case GDTSegmentRegister::GS:
			asm(
				"mov %0, %%gs \n"
				: : "r" (index << 3)
			);
			break;
		case GDTSegmentRegister::SS:
			asm(
				"mov %0, %%ss \n"
				: : "r" (index << 3)
			);
			break;
	}
}

void GDTTable::SetEntry(uint8_t index, GDTEntry entry)
{
	this->base[index] = entry;
}
