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

#include "string.h"
#include "stdint.h"
#include "stddef.h"

void *memcpy(void *dest, const void *src, size_t n)
{
	return memmove(dest, src, n);
}

void *memmove(void *dest, const void *src, size_t n)
{
	char *d = (char *)dest;
	const char *s = (const char *)src;

	if(d < s)
	{
		while(n--)
		{
			*(d++) = *(s++);
		}
	}
	else if(d > s)
	{
		s += n;
		d += n;

		while(n--)
		{
			*(d--) = *(s--);
		}
	}

	return dest;
}

void *memset(void *s, uint8_t c, size_t n)
{
	char *d = (char *)s;

	while(n--)
	{
		*(d++) = c;
	}
	
	return s;
}


int32_t strncmp(const char *s1, const char *s2, size_t n)
{
	int32_t difference = 0;

	while(*(s1++) && *(s2++) && (n--) && !difference)
	{
		difference = *s1 - *s2;
	}

	return difference;
}
