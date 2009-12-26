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

#include <string.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	return memmove(dest, src, n);
}

void *memmove(void *dest, const void *src, size_t n)
{
	char *d = (char *)dest;
	char *s = (char *)src;

	if(dest > src)
	{
		d += n - 1;
		s += n - 1;
	}

	size_t b = n;

	if((signed long)((unsigned long)dest - (unsigned long)src) < -(signed long)sizeof(unsigned long) 
		|| (signed long)((unsigned long)dest - (unsigned long)src) > (signed long)sizeof(unsigned long))
	{
		b %= sizeof(unsigned long);

		n -= b;
		n /= sizeof(unsigned long);
	}
	else
	{
		n = 0;
	}

	unsigned long *db;
	unsigned long *sb;

	if(dest <= src)
	{
		while(b--)
		{
			*(d++) = *(s++);
		}

		db = (unsigned long *)d;
		sb = (unsigned long *)s;

		while(n--)
		{
			*(db++) = *(sb++);
		}
	}
	else
	{
		while(b--)
		{
			*(d--) = *(s--);
		}

		db = (unsigned long *)d;
		sb = (unsigned long *)s;

		while(n--)
		{
			*(db--) = *(sb--);
		}
	}

	return dest;
}

void *memset(void *dest, int c, size_t n)
{
	char *d = (char *)dest;
	char v = (c & 0xFF);

	size_t b = n % sizeof(unsigned long);
	n -= b;
	n /= sizeof(unsigned long);

	while(b--)
	{
		*(d++) = v;
	}

	unsigned long *db = (unsigned long *)d;
	unsigned long vb = 0;

	for(unsigned int i = 0; i < sizeof(unsigned long); i++)
	{
		vb <<= 1;
		vb += v;
	}	

	while(n--)
	{
		*(db++) = vb;
	}

	return dest;
}
