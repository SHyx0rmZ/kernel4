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

#include "stdint.h"
#include "print.h"

uint8_t *videomemory = (uint8_t *)0xB8000;

void clear_screen()
{
	uint16_t *video;

	for(video = (uint16_t *)0xB8000; video < (uint16_t *)0xB8FA0; video++)
	{
		*video = 0x0720;
	}

	videomemory = (uint8_t *)0xB8000;
}

void print(const char *text, uint8_t flags)
{
	uint8_t *c;

	for(c = (uint8_t *)&text[0]; *c != 0; c++)
	{
		while(videomemory < (uint8_t *)0xB8000)
		{
			videomemory += 0xA0;
		}

		if(*c == '\n')
		{
			videomemory += 0xA0;
		}

		if(*c == '\r')
		{
			videomemory = (uint8_t *)(videomemory - ((uint32_t)(videomemory - 0xB8000) % 0xA0));
		}

		while(videomemory > (uint8_t *)0xB8FA0)
		{
			uint8_t *videosrc = (uint8_t *)0xB80A0;
			uint8_t *videodest = (uint8_t *)0xB8000;

			while(videosrc < (uint8_t *)0xB8FA0)
			{
				*videodest++ = *videosrc++;
			}

			videomemory -= 0xA0;
		}
		
		if(*c != '\r' && *c != '\n')
		{
			*videomemory++ = *c;
			*videomemory++ = flags; 
		}
	}
}
