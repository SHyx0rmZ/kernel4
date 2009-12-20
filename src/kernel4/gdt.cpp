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

GDTEntry::GDTEntry(GDTMode mode, uint8_t flags, uint8_t access, uintptr_t base, uint32_t limit)
{
	// Bogus code, to enable compilation :D
	this->base_low = base;
	this->flags_and_limit_high = flags;
	this->access = access;
	this->base_low = (uint8_t)mode;
	this->limit_low = limit;
}
