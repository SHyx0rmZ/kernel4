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

#include "stddef.h"
#include "gdt.h"

/**
 * Creates a new segment descriptor
 *
 * @param mode		Which operating mode the segment is for (possible values are: Real Mode, Protected Mode and Long Mode)
 * @param ring		The privilege ring the segment requests
 * @param base		The base address of the segment
 * @param limit		The size of the segment
 * @param granularity		If set to GDTGranularity::Block, limit gives the size in blocks of 4 KiB
 * @param presence		If the segment is present or not
 */
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

/**
 * Get the base address of the segment
 *
 * @returns		The base address of the segment
 */
uintptr_t GDTEntry::GetBase()
{
	return (this->base_low | (this->base_middle << 16) | (this->base_high << 24));
}

/**
 * Get the granularity of the segment
 *
 * @returns		The granularity of the segment
 */
GDTGranularity GDTEntry::GetGranularity()
{
	return (GDTGranularity)((this->flags_and_limit_high >> 7) & 1);
}

/**
 * Get the size of the segment
 *
 * @returns		The size of the segment
 */
uint32_t GDTEntry::GetLimit()
{
	return (this->limit_low | ((this->flags_and_limit_high & 0x0F) << 16));
}

/**
 * Get the operating mode of the segment
 *
 * @returns		The operating mode of the segment
 */
GDTMode GDTEntry::GetMode()
{
	return (GDTMode)((this->flags_and_limit_high >> 5) & 3);
}

/**
 * Get the presence of the segment
 *
 * @returns		If the segment is present or not
 */
GDTPresence GDTEntry::GetPresence()
{
	return (GDTPresence)((this->access >> 7) & 1);
}

/**
 * Get the privilege level of the segment
 *
 * @returns		The privilege level of the segment
 */
GDTRing GDTEntry::GetRing()
{
	return (GDTRing)((this->access >> 5) & 3);
}

/**
 * Get the type of system segment the segment is
 *
 * Will not return GDTSystemType32::Invalid if called for a 64-Bit segment
 *
 * @returns		The type of system segment the segment is
 */
GDTSystemType32 GDTEntry::GetSystemType32()
{
	if(!this->IsSystemSegment())
	{
		return GDTSystemType32::Invalid;
	}

	return (GDTSystemType32)(this->access & 0x0F);
}

/**
 * Get the type of system segment the segment is
 *
 * Will not return GDTSystemType64::Invalid if called for a 32-Bit segment
 *
 * @returns		The type of system segment the segment is
 */
GDTSystemType64 GDTEntry::GetSystemType64()
{
	if(!this->IsSystemSegment())
	{
		return GDTSystemType64::Invalid;
	}

	return (GDTSystemType64)(this->access & 0x0F);
}

/**
 * Get the type of the segment
 *
 * @returns		The type of segment
 */
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

/**
 * Check if the segment is a code segment
 *
 * @returns		true if the segment is a code segment, false otherwise
 */
bool GDTEntry::IsCodeSegment()
{
	return (!this->IsSystemSegment() && ((this->access >> 3) & 1) == 1);
}

/**
 * Check if the segment is a conforming code segment
 *
 * @returns		true if the segment is a conforming code segment, false otherwise
 */
bool GDTEntry::IsConforming()
{
	return (this->IsCodeSegment() && ((this->access >> 2) & 1) == 1);
}

/**
 * Check if the segment is a data segment
 *
 * @returns		true if the segment is a data segment, false otherwise
 */
bool GDTEntry::IsDataSegment()
{
	return (!this->IsSystemSegment() && ((this->access >> 3) & 1) == 0);
}

/**
 * Check if the segment is an execute only code segment
 *
 * @returns		true if the segment is an execute only code segment, false otherwise
 */
bool GDTEntry::IsExecuteOnly()
{
	return (this->IsCodeSegment() && ((this->access >> 1) & 1) == 0);
}

/**
 * Check if the segment is an expand down data segment
 *
 * @returns		true if the segment is an expand down data segment, false otherwise
 */
bool GDTEntry::IsExpandDown()
{
	return (this->IsDataSegment() && ((this->access >> 2) & 1) == 1);
}

/**
 * Check if the segment is a read only data segment
 *
 * @returns		true if the segment is a read only data segment, false otherwise
 */
bool GDTEntry::IsReadOnly()
{
	return (this->IsDataSegment() && ((this->access >> 1) & 1) == 0);
}

/**
 * Check if the segment is a system segment
 *
 * @returns		true if the segment is a system segment, false otherwise
 */
bool GDTEntry::IsSystemSegment()
{
	return (((this->access >> 4) & 1) == 0);
}

/**
 * Set the base address of the segment
 *
 * @param address		The new base address of the segment
 */
void GDTEntry::SetBase(uintptr_t address)
{
	this->base_low = (address & 0xFFFF);
	this->base_middle = ((address >> 16) & 0xFF);
	this->base_high = ((address >> 24) & 0xFF);
}

/**
 * Set the conforming bit of the segment
 *
 * @param conforming		Is the conforming bit to be set?
 * @returns		true if the segment is a code segment and the bit could be set, false otherwise
 */
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

/**
 * Set the execute only bit of the segment
 *
 * @param executeonly		Is the execute only bit to be set?
 * @returns		true if the segment is a code segment and the bit could be set, false otherwise
 */
bool GDTEntry::SetExecuteOnly(bool executeonly)
{
	if(!this->IsCodeSegment())
	{
		return false;
	}

	this->access &= ~(1 << 1);
	this->access |= (!executeonly << 1);

	return true;
}

/**
 * Set the expand down bit of the segment
 *
 * @param expanddown		Is the expand down bit to be set?
 * @returns		true if the segment is a data segment and the bit could be set, false otherwise
 */
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

/**
 * Set the granularity of the segment
 *
 * If GDTGranularity::Block, limit gives the size of the segment in blocks of 4 KiB, in bytes otherwise
 *
 * @param granularity		The granularity of the segment
 */
void GDTEntry::SetGranularity(GDTGranularity granularity)
{
	this->flags_and_limit_high &= ~(1 << 7);
	this->flags_and_limit_high |= ((uint8_t)granularity << 7);
}

/**
 * Set the size of the segment
 *
 * @param limit		The new size of the segment
 */
void GDTEntry::SetLimit(uint32_t limit)
{
	this->limit_low = (limit & 0xFFFF);
	this->flags_and_limit_high &= 0xF0;
	this->flags_and_limit_high |= ((limit >> 16) & 0x0F);
}

/**
 * Set the operating mode of the segment
 *
 * @param mode		The operating mode of the segment
 */
void GDTEntry::SetMode(GDTMode mode)
{
	this->flags_and_limit_high &= ~(3 << 5);
	this->flags_and_limit_high |= ((uint8_t)mode << 5);
}

/**
 * Set the presence of the segment
 *
 * @param presence		The presence of the segment
 */
void GDTEntry::SetPresence(GDTPresence presence)
{
	this->access &= ~(1 << 7);
	this->access |= ((uint8_t)presence << 7);
}

/**
 * Set the read only bit of the segment
 *
 * @param readonly		Is the read only bit to be set?
 * @returns		true if the segment is a data segment and the bit could be set, false otherwise
 */
bool GDTEntry::SetReadOnly(bool readonly)
{
	if(!this->IsDataSegment())
	{
		return false;
	}

	this->access &= ~(1 << 1);
	this->access |= (!readonly << 1);

	return true;
}

/**
 * Set the privilege level of the segment
 *
 * @param ring		The privilege level of the segment
 */
void GDTEntry::SetRing(GDTRing ring)
{
	this->access &= ~(3 << 5);
	this->access|= ((uint8_t)ring << 5);
}

/**
 * Set the system segment type of the segment
 *
 * @param systemtype		The system segment type of the segment
 * @returns		true if the segment is a system segment and the bit could be set, false otherwise
 */
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

/**
 * Set the system segment type of the segment
 *
 * @param systemtype		The system segment type of the segment
 * @returns		true if the segment is a system segment and the bit could be set, false otherwise
 */
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

/**
 * Set the system segment type of the segment
 *
 * @param systemtype		The system segment type of the segment
 * @returns		true if the segment is a system segment and the bit could be set, false otherwise
 */
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

/**
 * Set the system segment type of the segment
 *
 * @param systemtype		The system segment type of the segment
 * @returns		true if the segment is a system segment and the bit could be set, false otherwise
 */
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

/**
 * Set the type of the segment
 *
 * @param type		The type of the segment
 */
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

/**
 * Check if two GDTEntries are the same
 *
 * @returns		true if both GDTEntries are the same, false otherwise
 */
bool operator ==(GDTEntry e1, GDTEntry e2)
{
	if(e1.GetType() != e2.GetType())
	{
		return false;
	}

	if(e1.GetType() == GDTType::System)
	{
		if(e1.access == e2.access)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if((e1.limit_low == e2.limit_low) && (e1.base_low == e2.base_low) && (e1.access == e2.access)
			&& (e1.flags_and_limit_high == e2.flags_and_limit_high)
			&& (e1.base_middle == e2.base_middle) && (e1.base_high == e2.base_high))
	{
		return true;
	}

	return false;
}

/**
 * Creates a new GDT (or rather, a structure to manage it)
 *
 * @param size		The number of entries the GDTTable can hold
 * @param position		A pointer to a GDTEntry array
 */
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

uint16_t GDTTable::GetDescriptor(GDTEntry entry)
{
	return this->GetDescriptor(this->GetIndex(entry));
}

uint16_t GDTTable::GetDescriptor(uint8_t index)
{
	return ((index << 3) | (uint8_t)(this->GetEntry(index).GetRing()));
}

GDTEntry GDTTable::GetEntry(GDTEntry entry)
{
	return this->GetEntry(this->GetIndex(entry));
}

/**
 * Get a GDTEntry at a specific index
 *
 * @param The		index of the GDTEntry
 * @returns		The GDTEntry indexed by index, NULL if index is invalid
 */
GDTEntry GDTTable::GetEntry(uint8_t index)
{
	if(index >= this->GetSize())
	{
		GDTEntry *e = NULL;
		return *e;
	}

	return this->base[index];
}

/**
 * Get the index of a GDTEntry
 *
 * @param The		GDTEntry to get the index of
 * @returns		The index of the GDTEntry represented by entry if found, 0 otherwise
 */
uint8_t GDTTable::GetIndex(GDTEntry entry)
{
	for(uint8_t i = 0; i < this->GetSize(); i++)
	{
		if(this->base[i] == entry)
		{
			return i;
		}
	}

	return 0;
}

/**
 * Get the size of the GDTTable
 *
 * @returns		The size of the GDTTable
 */
uint8_t GDTTable::GetSize()
{
	return (this->limit + 1) / 8;
}

/**
 * Check if the GDTTable is the currently active GDT
 *
 * @ŗeturns true if the GDTTable is the currently active GDT, false otherwise
 */
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

/**
 * Make the GDTTable the currently active GDT
 */
void GDTTable::MakeActive()
{
	asm(
		"lgdt %0 \n"
		: : "g" (*this)
	);
}

/**
 * Reload a segment register
 *
 * @param segment		The segment register to reload
 * @param index		The index of the GDTEntry to reload the segment register with
 */
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
				: : "a" (this->GetDescriptor(index))
			);
			break;
		case GDTSegmentRegister::DS:
			asm(
				"mov %0, %%ds \n"
				: : "r" (this->GetDescriptor(index))
			);
			break;
		case GDTSegmentRegister::ES:
			asm(
				"mov %0, %%es \n"
				: : "r" (this->GetDescriptor(index))
			);
			break;
		case GDTSegmentRegister::FS:
			asm(
				"mov %0, %%fs \n"
				: : "r" (this->GetDescriptor(index))
			);
			break;
		case GDTSegmentRegister::GS:
			asm(
				"mov %0, %%gs \n"
				: : "r" (this->GetDescriptor(index))
			);
			break;
		case GDTSegmentRegister::SS:
			asm(
				"mov %0, %%ss \n"
				: : "r" (this->GetDescriptor(index))
			);
			break;
	}
}

/**
 * Set the GDTEntry indexed by index
 *
 * @param index		The index of the GDTEntry
 * @param entry		The new value of the GDTEntry indexed by index
 */
void GDTTable::SetEntry(uint8_t index, GDTEntry entry)
{
	if(index < this->GetSize())
	{
		this->base[index] = entry;
	}
}
