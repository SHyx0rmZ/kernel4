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

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stdint.h>

enum class ConsoleState
{
	Decimal,
	Hex,
	Octal,
	HexFixed
};

enum class ConsoleColor
{
	Black = 0,
	Gray = 7,
	DarkGray = 8,
	Blue = 9,
	Green = 10,
	Red = 12,
	Yellow = 14,
	White = 15
};

enum class ConsoleArea
{
	Top = 0,
	Middle = 1,
	Bottom = 2,
};

class Console
{
	public:
		Console();
		~Console();

		void Clear();
		ConsoleArea GetArea();
		uint8_t GetAttribute();
		ConsoleState GetState();
		void SetArea(ConsoleArea area);
		void SetAttribute(uint8_t attribute);
		void SetState(ConsoleState state);

		void Convert(char *buffer, uint16_t buffersize, uint64_t number, uint16_t base);

		Console &operator<<(const char *text);
		Console &operator<<(uint64_t number);
		Console &operator<<(ConsoleColor color);
		Console &operator<<(ConsoleState state);
		Console &operator<<(ConsoleArea area);
		Console &operator<<(uint64_t *pointer);

	private:
		static uint16_t *video;

		uint16_t attribute;
		ConsoleState state;
		ConsoleArea area;
		uint16_t *vl_top, *vl_middle, *vl_bottom;
};

#endif
