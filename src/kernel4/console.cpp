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

#include <stdint.h>
#include <console.h>
#include <string.h>

uint16_t *Console::video = (uint16_t *)(0xB8000 + 160);

Console::Console()
{
	this->video = (uint16_t *)(0xB8000 + 160);
	this->attribute = 0x0700;
	this->state = ConsoleState::Hex;
	this->area = ConsoleArea::Middle;
	this->vl_top = (uint16_t *)(0xB8000);
	this->vl_middle = this->video;
	this->vl_bottom = (uint16_t *)(0xB8000 + 160 * 24);
}

Console::~Console()
{
}

void Console::Clear()
{
	*this << ConsoleArea::Middle;
	this->video = (uint16_t *)(0xB8000 + 160);
	memset((void *)0xB8000, 0, 80 * 25 * 2);
	*this << ConsoleArea::Top << "                                                                                \n" << ConsoleArea::Bottom << "                                                                                                    \n" << ConsoleArea::Middle;
}

ConsoleArea Console::GetArea()
{
	return this->area;
}

uint8_t Console::GetAttribute()
{
	return (this->attribute >> 8);
}

ConsoleState Console::GetState()
{
	return this->state;
}

void Console::SetArea(ConsoleArea area)
{
	switch(this->area)
	{
		case ConsoleArea::Top:
			this->vl_top = this->video;
			break;
		case ConsoleArea::Middle:
			this->vl_middle = this->video;
			break;
		case ConsoleArea::Bottom:
			this->vl_bottom = this->video;
			break;
	}

	this->area = area;

	switch(area)
	{
		case ConsoleArea::Top:
			this->video = this->vl_top;
			break;
		case ConsoleArea::Middle:
			this->video = this->vl_middle;
			break;
		case ConsoleArea::Bottom:
			this->video = this->vl_bottom;
			break;
	}
}

void Console::SetAttribute(uint8_t attribute)
{
	this->attribute = (attribute << 8);
}

void Console::SetState(ConsoleState state)
{
	this->state = state;
}

void Console::Convert(char *buffer, uint16_t buffersize, uint64_t number, uint16_t base)
{
	char c[256];

	int16_t i;

	base %= 35;
	buffersize %= 257;

	if(number == 0)
	{
		buffer[0] = '0';
		buffer[1] = 0;

		return;
	}

	for(i = 255; i > 0 && number > 0; i--)
	{
		c[i] = (number % base) + 0x30;

		if(c[i] > 0x39)
		{
			c[i] += 7;
		}

		number /= base;
	}

	i++;
	uint8_t j;

	for(j = 0; j < (buffersize - 1) && i < 256; i++, j++)
	{
		buffer[j] = c[i];
	}

	buffer[j] = 0;
}

Console &Console::operator<<(const char *text)
{
	switch(this->GetArea())
	{
		case ConsoleArea::Middle:
			while(*text)
			{
				switch(*text)
				{
					case '\n':
						video = (uint16_t *)((uint64_t)video + 160);
						break;
					case '\r':
						video = (uint16_t *)((uint64_t)video - (((uint64_t)video - 0xB8000) % 160));
						break;
					default:
						if((uint64_t)video >= 0xB8000 + 160 * 24)
						{
							memmove((void *)(0xB8000 + 160), (void*)(0xB8000 + 160 * 2), 160 * 22);
							memset((void *)(0xB8000 + 160 * 23), 0, 160);
							video = (uint16_t *)((uint64_t)video - 160);
						}

						*(video++) = *text | this->attribute;
						break;
				}

				text++;
			}
			break;
		case ConsoleArea::Top:
			while(*text)
			{
				switch(*text)
				{
					case '\n':
					case '\r':
						video = (uint16_t *)0xB8000;
						break;
					default:
						if((uint64_t)video >= 0xB8000 + 160)
						{
							memmove((void *)0xB8000, (void *)(0xB8000 + 2), 158);
							memset((void *)(0xB8000 + 158), 0, 2);
							video = (uint16_t *)((uint64_t)video - 2);
						}

						*(video++) = *text | this->attribute | (0x70 << 8);
						break;
				}

				text++;
			}
			break;
		case ConsoleArea::Bottom:
			while(*text)
			{
				switch(*text)
				{
					case '\n':
					case '\r':
						video = (uint16_t *)(0xB8000 + 160 * 24);
						break;
					default:
						if((uint64_t)video >= 0xB8000 + 160 * 25)
						{
							memmove((void *)(0xB8000 + 160 * 24), (void *)(0xB8000 + 2 + 160 * 24), 158);
							memset((void *)(0xB8000 + 158 + 160 * 24), 0, 2);
							video = (uint16_t *)((uint64_t)video - 2);
						}

						*(video++) = *text | this->attribute | (0x70 << 8);
						break;
				}

				text++;
			}
			break;
	}

	return *this;
}

Console &Console::operator<<(uint64_t number)
{
	char converted[256];

	switch(this->state)
	{
		case ConsoleState::Decimal:
			this->Convert(converted, 256, number, 10);
			break;
		case ConsoleState::Hex:
			this->Convert(converted, 256, number, 16);
			*this << "0x";
			break;
		case ConsoleState::Octal:
			this->Convert(converted, 256, number, 8);
			*this << "0";
			break;
	}

	return (*this << converted);
}

Console &Console::operator<<(ConsoleColor color)
{
	this->SetAttribute((uint8_t)color);

	return *this;
}

Console &Console::operator<<(ConsoleState state)
{
	this->SetState(state);

	return *this;
}

Console &Console::operator<<(ConsoleArea area)
{
	this->SetArea(area);

	return *this;
}
