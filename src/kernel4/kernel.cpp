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

#include "kernel.h"
#include "multiboot.h"
#include "stdint.h"
#include "stddef.h"
#include "gdt.h"

/**
 * Creates a new instance of the kernel
 * 
 * \param multiboot The multiboot information structure the bootloader created
 */
Kernel::Kernel(MultibootInformation multiboot)
{
	GDTTable gdt(5, 0x200000 - (5 * sizeof(GDTEntry)));

	gdt.SetEntry(0, GDTEntry(GDTMode::RealMode, 		GDTType::Code, GDTRing::Ring0, 0, 0, GDTGranularity::Block, GDTPresence::NonPresent));
	gdt.SetEntry(1, GDTEntry(GDTMode::LongMode, 		GDTType::Code, GDTRing::Ring0));
	gdt.SetEntry(2, GDTEntry(GDTMode::ProtectedMode, 	GDTType::Data, GDTRing::Ring0));
	gdt.SetEntry(3, GDTEntry(GDTMode::LongMode, 		GDTType::Code, GDTRing::Ring3));
	gdt.SetEntry(4, GDTEntry(GDTMode::ProtectedMode, 	GDTType::Data, GDTRing::Ring3));

	gdt.MakeActive();

	gdt.ReloadSegment(GDTSegmentRegister::CS, 1);
	gdt.ReloadSegment(GDTSegmentRegister::DS, 2);
	gdt.ReloadSegment(GDTSegmentRegister::ES, 2);
	gdt.ReloadSegment(GDTSegmentRegister::FS, 0);
	gdt.ReloadSegment(GDTSegmentRegister::GS, 0);
	gdt.ReloadSegment(GDTSegmentRegister::SS, 2);

	while(multiboot.flags);
}

/**
 * The kernel can not be shutdown and instead enters a endless loop
 */
Kernel::~Kernel()
{
	while(1);
}
