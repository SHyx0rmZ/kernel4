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
#include <idt.h>

/**
 * Creates a new IDTEntry
 *
 * @param offset 		The offset to the interrupt service routine
 * @param type 		The type this IDTEntry represents, either a Interrupt Gate or a Trap
 * @param selector 		The GDT selector of the code segment the interrupt service routine is in
 * @param ring 		The privilege level this IDTEntry has
 * @param ist 		The interrupt stack to use
 * @param present 		If the IDTEntry is present
 */
IDTEntry::IDTEntry(uintptr_t offset, IDTType type, uint16_t selector, IDTRing ring, IDTIST ist, bool present)
{
	this->offset_low = (offset & 0xFFFF);
	this->selector = selector;
	this->flags = (present << 15) | ((uint8_t)ring << 13) | ((0xE | (uint8_t)type) << 8) | ((uint8_t)ist);
	this->offset_mid = ((offset >> 16) & 0xFFFF);
	this->offset_high = ((offset >> 32) & 0xFFFFFFFF);
	this->reserved = 0;
}

/**
 * Destroys the IDTEntry
 */
IDTEntry::~IDTEntry()
{
}

/**
 * Get the interrupt stack this IDTEntry uses
 *
 * @returns		 The interrupt stack used by this IDTEntry
 */
IDTIST IDTEntry::GetIST()
{
	return (IDTIST)(this->flags & 0x7);
}

/**
 * Get the offset to the interrupt service routine
 *
 * @returns		 The offset to the interrupt service routine
 */
uintptr_t IDTEntry::GetOffset()
{
	return (((uint64_t)this->offset_high << 32) | ((uint64_t)this->offset_mid << 16) | ((uint64_t)this->offset_low));
}

/**
 * Get the privilege level of this IDTEntry
 *
 * @returns		 The privilege level of this IDTEntry
 */
IDTRing IDTEntry::GetRing()
{
	return (IDTRing)((this->flags >> 13) & 3);
}

/**
 * Get the GDT selector
 *
 * @returns		 the GDT selector
 */
uint16_t IDTEntry::GetSelector()
{
	return this->selector;
}

/**
 * Get the type of this IDTEntry
 *
 * @returns		 The type of this IDTEntry
 */
IDTType IDTEntry::GetType()
{
	return (IDTType)((this->flags >> 8) & 1);
}

/**
 * Is this IDTEntry present?
 *
 * @returns		 true if the IDTEntry is present, false otherwise
 */
bool IDTEntry::IsPresent()
{
	return (this->flags & (1 << 15)) ? true : false;
}

/**
 * Set the interrupt stack
 *
 * @param ist 		The stack the interrupt routine should use
 */
void IDTEntry::SetIST(IDTIST ist)
{
	this->flags &= ~7;
	this->flags |= ((uint8_t)ist & 7);
}

/**
 * Set offset to interrupt routine
 *
 * @param offset 		Offset to interrupt routine
 */
void IDTEntry::SetOffset(uintptr_t offset)
{
	this->offset_low = (offset & 0xFFFF);
	this->offset_mid = ((offset >> 16) & 0xFFFF);
	this->offset_high = ((offset >> 32) & 0xFFFFFFFF);
}

/**
 * Make the IDTEntry present or not present
 *
 * @param present 		Shall the IDTEntry be marked present?
 */
void IDTEntry::SetPresence(bool present)
{
	this->flags &= ~(1 << 15);
	this->flags |= ((present & 1) << 15);
}

/**
 * Set the privilege level
 *
 * @param ring 		The privilege level
 */
void IDTEntry::SetRing(IDTRing ring)
{
	this->flags &= ~(3 << 13);
	this->flags |= (((uint8_t)ring & 3) << 13);
}

/**
 * Set code segment selector
 *
 * @param selector 		The new code segment selector
 */
void IDTEntry::SetSelector(uint16_t selector)
{
	this->selector = selector;
}

/**
 * Set the interrupt type
 *
 * @param type 		The interrupt type
 */
void IDTEntry::SetType(IDTType type)
{
	this->flags &= ~(1 << 8);
	this->flags |= (((uint8_t)type & 1) << 8);
}

/**
 * Compare two IDTEntries
 *
 * @param e1 		The first IDTEntry
 * @param e2 		The second IDTEntry
 * @returns		 true if both IDTEntries are equal, false otherwise
 */
bool operator ==(const IDTEntry e1, const IDTEntry e2)
{
	if((e1.offset_low == e2.offset_low) && (e1.offset_mid == e2.offset_mid && (e1.flags == e2.flags))
			&& (e1.offset_high == e2.offset_high) && (e1.selector == e2.selector))
	{
		return true;
	}

	return false;
}

/**
 * Create a new IDTTable
 *
 * @param size 		The number of IDTEntries that fit into this IDTTable
 * @param position 		The address of the IDTTable (or rather the IDTEntries)
 */
IDTTable::IDTTable(uint8_t size, uintptr_t position)
{
	if(size == 0)
	{
		this->limit = 0;
	}
	else
	{
		this->limit = ((size) * sizeof(IDTEntry)) - 1;
	}

	this->base = (IDTEntry *)position;
}

/**
 * Destroys the IDTTable
 */
IDTTable::~IDTTable()
{
}

/**
 * Get an IDTEntry from the IDTTable
 *
 * @param index 		The index of the IDTEntry
 * @returns		 The IDTEntry at index index, NULL if index is invalid
 */
IDTEntry IDTTable::GetEntry(uint8_t index)
{
	if(index >= this->GetSize())
	{
		return NULL;
	}

	return this->base[index];
}

/**
 * Get the index of a specific IDTEntry
 *
 * @param entry 		The IDTEntry to search in the IDTTable
 * @returns		 The index of the searched IDTEntry, -1 if not found
 */
uint8_t IDTTable::GetIndex(IDTEntry entry)
{
	for(uint8_t i = 0; i < this->GetSize(); i++)
	{
		if(this->base[i] == entry)
		{
			return i;
		}
	}

	return -1;
}

/**
 * Get the number of IDTEntries that fit into this IDTTable
 *
 * @returns		 The number of IDTEntries
 */
uint8_t IDTTable::GetSize()
{
	return (this->limit + 1) / sizeof(IDTEntry);
}

/**
 * Check if this IDTTable is the currently active one
 *
 * @returns		 true if this IDTTable is the currently active one, false otherwise
 */
bool IDTTable::IsActive()
{
	// Backup values
	uint16_t temp_limit = this->limit;
	IDTEntry *temp_base = this->base;

	// Get the values of the currently active IDTTable
	asm(
		"sidt %0 \n"
		: : "m" (*this)
	);

	// Compare to backup values
	bool same = ((this->limit == temp_limit) && (this->base == temp_base));

	// Restore backup values
	this->limit = temp_limit;
	this->base = temp_base;

	return same;
}

/**
 * Make this IDTTable the currently active one
 */
void IDTTable::MakeActive()
{
	// Load this IDTTable into IDTR
	asm(
		"lidt %0 \n"
		: : "g" (*this)
	);
}

/**
 * Set the IDTEntry at index index
 *
 * This function does simply nothing if index is invalid
 *
 * @param index 		The index of the IDTEntry to be set
 * @param entry 		The IDTEntry to be set
 */
void IDTTable::SetEntry(uint8_t index, IDTEntry entry)
{
	if(index < this->GetSize())
	{
		this->base[index] = entry;
	}
}
