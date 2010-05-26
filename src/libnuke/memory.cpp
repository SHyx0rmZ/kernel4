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

#include "managers.h"

void test()
{
}

void *operator new(size_t size)
{
	if(size > 4000)
	{
		console << ConsoleColor::Red << "You just got pwned by NoobMM!";

		while(1)
		{
			asm("cli;hlt");
		}
	}

	return (void *)memory.VAlloc();
}

void *operator new[](size_t size)
{
	return operator new(size);
}

void operator delete(void *address)
{
	memory.VFree((uintptr_t)address);
}

void operator delete[](void *address)
{
	operator delete(address);
}
