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

/**
 * The granularities a segment can have
 */
enum class GDTGranularity : uint8_t
{
	Byte = 0,
	Block = 1
};

/**
 * The different operating modes a segment can have
 */
enum class GDTMode : uint8_t
{
	RealMode = 0,
	LongMode = 1,
	ProtectedMode = 2
};

/**
 * The presence states a segment can have
 */
enum class GDTPresence : uint8_t
{
	NonPresent = 0,
	Present = 1
};

/**
 * The privilege levels a segment can have
 */
enum class GDTRing : uint8_t
{
	Ring0 = 0,
	Ring1 = 1,
	Ring2 = 2,
	Ring3 = 3
};

/**
 * The different types of system segments in Protected Mode
 */
enum class GDTSystemType32 : uint8_t
{
	TaskStateSegment16Available = 1,
	LocalDescriptorTable = 2,
	TaskStateSegment16Busy = 3,
	CallGate16 = 4,
	TaskGate = 5,
	InterruptGate16 = 6,
	TrapGate16 = 7,
	TaskStateSegmentAvailable = 9,
	TaskStateSegmentBusy = 11,
	CallGate = 12,
	InterruptGate = 14,
	TrapGate = 15,
	Invalid = 255
};

/**
 * The different types of system segments in Long Mode
 */
enum class GDTSystemType64 : uint8_t
{
	UpperHalf16 = 0,
	LocalDescriptorTable = 2,
	TaskStateSegmentAvailable = 9,
	TaskStateSegmentBusy = 11,
	CallGate = 12,
	InterruptGate = 14,
	TrapGate = 15,
	Invalid = 255
};

/**
 * The different types a segment can have
 */
enum class GDTType : uint8_t
{
	System = 0,
	Data = 1,
	Code = 2
};

/**
 * The different segment register there are on a x86_64
 */
enum class GDTSegmentRegister : uint8_t
{
	CS = 0,
	DS = 1,
	ES = 2,
	FS = 3,
	GS = 4,
	SS = 5
};

/**
 * A GDTEntry to be used in constructing a valid GDT
 */
class GDTEntry
{
	public:
		GDTEntry(GDTMode mode, GDTType type, GDTRing ring, uintptr_t base = 0, uint32_t limit = 0xFFFFFFFF, GDTGranularity granularity = GDTGranularity::Block, GDTPresence presence = GDTPresence::Present);
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

		friend bool 	operator==(const GDTEntry e1, const GDTEntry e2);

	private:
		uint16_t limit_low;
		uint16_t base_low;
		uint8_t base_middle;
		uint8_t access;
		uint8_t flags_and_limit_high;
		uint8_t base_high;
} __attribute__((packed));

/**
 * A structure to manage multiple GDTEntries that form a GDT
 */
class GDTTable
{
	public:
		GDTTable(uint8_t size, uintptr_t position);
		~GDTTable();

		uint16_t	GetDescriptor(GDTEntry entry);
		uint16_t	GetDescriptor(uint8_t index);
		GDTEntry	GetEntry(GDTEntry entry);
		GDTEntry 	GetEntry(uint8_t index);
		uint8_t		GetIndex(GDTEntry entry);
		uint8_t 	GetSize();
		bool 		IsActive();
		void 		MakeActive();
		void 		ReloadSegment(GDTSegmentRegister segment, uint8_t index);
		void 		SetEntry(uint8_t index, GDTEntry entry);

	private:
		uint16_t limit;
		GDTEntry *base;
} __attribute__((packed));

class AvailableTSS64
{
	public:
		AvailableTSS64(uintptr_t position, uint32_t size);
		~AvailableTSS64();

		GDTEntry	ToGDTEntryPart1();
		GDTEntry	ToGDTEntryPart2();

	private:
		uint64_t lower;
		uint64_t upper;
} __attribute__((packed));

#endif
