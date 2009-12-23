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

#ifndef _GDT_H_
#define _GDT_H_

#include <stdint.h>

enum GDTGranularity
{
	Byte = 0,
	Block = 1
};

enum GDTMode
{
	RealMode = 0,
	LongMode = 1,
	ProtectedMode = 2
};

/*enum GDTType
{
	DataReadOnly = 0,
	DataReadOnlyAccessed = 1,
	DataReadWrite = 2,
	DataReadWriteAccessed = 3,
	DataReadOnlyExpandDown = 4,
	DataReadOnlyExpandDownAccessed = 5,
	DataReadWriteExpandDown = 6,
	DataReadWriteExpandDownAccessed = 7,
	CodeExecuteOnly = 8,
	CodeExecuteOnlyAccessed = 9,
	CodeExecuteRead = 10,
	CodeExecuteReadAccessed = 11,
	CodeExecuteOnlyConforming = 12,
	CodeExecuteOnlyConformingAccessed = 13,
	CodeExecuteReadConforming = 14,
	CodeExecuteReadConformingAccessed = 15
};*/
enum GDTPresence
{
	NonPresent = 0,
	Present = 1
};

enum GDTRing
{
	Ring0 = 0,
	Ring1 = 1,
	Ring2 = 2,
	Ring3 = 3
};

enum GDTSystemType32
{
	TaskStateSegment16Available = 1,
	LocalDescriptorTable32 = 2,
	TaskStateSegment16Busy = 3,
	CallGate16 = 4,
	TaskGate = 5,
	InterruptGate16 = 6,
	TrapGate16 = 7,
	TaskStateSegment32Available = 9,
	TaskStateSegment32Busy = 11,
	CallGate32 = 12,
	InterruptGate32 = 14,
	TrapGate32 = 15,
	Invalid32 = 255
};

enum GDTSystemType64
{
	UpperHalf16 = 0,
	LocalDescriptorTable64 = 2,
	TaskStateSegment64Available = 9,
	TaskStateSegment64Busy = 11,
	CallGate64 = 12,
	InterruptGate64 = 14,
	TrapGate64 = 15,
	Invalid64 = 255
};

enum GDTType
{
	System = 0,
	Data = 1,
	Code = 2
};

class GDTEntry
{
	public:
		//GDTEntry(GDTMode mode, uint8_t flags, uint8_t access = 10, uintptr_t base = 0, uint32_t limit = 0xFFFFFFFF);
		GDTEntry(GDTMode mode, GDTType type, GDTRing ring, uintptr_t base = 0, uint32_t limit = 0xFFFFFFFF, GDTGranularity granularity = Block, GDTPresence presence = Present);
		~GDTEntry();

		uintptr_t 	GetBase();
		GDTGranularity	GetGranularity();
		uint32_t 	GetLimit();
		GDTMode 	GetMode();
		GDTPresence 	GetPresence();
		GDTRing 	GetRing();
		GDTSystemType32 GetSystemType32();
		GDTSystemType64 GetSystemType64();
		GDTType 	GetType();
		bool 		IsCodeSegment();
		bool 		IsConforming();
		bool 		IsDataSegment();
		bool 		IsExecuteOnly();
		bool 		IsExpandDown();
		bool 		IsReadOnly();
		bool 		IsSystemSegment();
		void 		SetBase(uintptr_t address);
		bool 		SetConforming(bool conforming);
		bool		SetExecuteOnly(bool executeonly);
		bool 		SetExpandDown(bool expanddown);
		void		SetGranularity(GDTGranularity granularity);
		void 		SetLimit(uint32_t limit);
		void 		SetMode(GDTMode mode);
		void 		SetPresence(GDTPresence presence);
		bool 		SetReadOnly(bool readonly);
		void 		SetRing(GDTRing ring);
		bool 		SetSystemType(GDTSystemType32 systemtype);
		bool 		SetSystemType(GDTSystemType64 systemtype);
		bool 		SetSystemType32(GDTSystemType32 systemtype);
		bool 		SetSystemType64(GDTSystemType64 systemtype);
		void 		SetType(GDTType type);
		//TODO: implement
		//char 		*ToString();

	private:
		uint16_t limit_low;
		uint16_t base_low;
		uint8_t base_middle;
		uint8_t access;
		uint8_t flags_and_limit_high;
		uint8_t base_high;
};

class GDTTable
{
	public:
		GDTTable(uint8_t size, uintptr_t position);
		~GDTTable();

		GDTEntry GetEntry(uint8_t index);
		uint8_t GetSize();
		void SetEntry(uint8_t index, GDTEntry entry);

	private:
		uint16_t limit;
		GDTEntry *base;
};

#endif
