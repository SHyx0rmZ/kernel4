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

enum GDTRing
{
	Ring0 = 0x00,
	Ring1 = 0x20,
	Ring2 = 0x40,
	Ring3 = 0x60
};

enum GDTMode
{
	RealMode = 0x00,
	LongMode = 0x02,
	ProtectedMode = 0x04
};

class GDTEntry
{
	public:
		GDTEntry(GDTMode mode, uint8_t flags, uint8_t access = 10, uintptr_t base = 0, uint32_t limit = 0xFFFFFFFF);
		~GDTEntry();

		uint8_t GetAccess();
		uintptr_t GetBase();
		uint8_t GetFlags();
		uint32_t GetLimit();
		void SetAccess(uint8_t access);
		void SetBase(uintptr_t address);
		void SetFlags(uint8_t flags);
		void SetLimit(uint32_t limit);

	private:
		uint16_t limit_low;
		uint16_t base_low;
		uint8_t base_middle;
		uint8_t access;
		uint8_t flags_and_limit_high;
		uint8_t base_high;
};

#endif
