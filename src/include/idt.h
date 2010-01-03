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

#ifndef _IDT_H_
#define _IDT_H_

#include <stdint.h>

enum class IDTRing : uint8_t
{
	Ring0 = 0,
	Ring1 = 1,
	Ring2 = 2,
	Ring3 = 3
};

enum class IDTIST : uint8_t
{
	IST0 = 0,
	IST1 = 1,
	IST2 = 2,
	IST3 = 3,
	IST4 = 4,
	IST5 = 5,
	IST6 = 6,
	IST7 = 7
};

enum class IDTType : uint8_t
{
	Interrupt = 0,
	Trap = 1
};

class IDTEntry
{
	public:
		IDTEntry(uintptr_t offset, IDTType type = IDTType::Interrupt, uint16_t selector = 1 << 3, IDTRing ring = IDTRing::Ring0, IDTIST ist = IDTIST::IST0, bool present = true);
		~IDTEntry();

		IDTIST 		GetIST();
		uintptr_t 	GetOffset();
		IDTRing 	GetRing();
		uint16_t 	GetSelector();
		IDTType 	GetType();
		bool 		IsPresent();
		void 		SetIST(IDTIST ist);
		void 		SetOffset(uintptr_t offset);
		void 		SetPresence(bool present);
		void 		SetRing(IDTRing ring);
		void 		SetSelector(uint16_t selector);
		void 		SetType(IDTType type);

		friend bool	operator==(const IDTEntry e1, const IDTEntry e2);

	private:
		uint16_t offset_low;
		uint16_t selector;
		uint16_t flags;
		uint16_t offset_mid;
		uint32_t offset_high;
		uint32_t reserved;
} __attribute__((packed));

class IDTTable
{
	public:
		IDTTable(uint8_t size, uintptr_t position);
		~IDTTable();

		IDTEntry	GetEntry(uint8_t index);
		uint8_t	GetIndex(IDTEntry entry); // -1 if not found
		uint8_t		GetSize();
		bool		IsActive();
		void 		MakeActive();
		void		SetEntry(uint8_t index, IDTEntry entry);

	private:
		uint16_t limit;
		IDTEntry *base;
} __attribute__((packed));

#endif
