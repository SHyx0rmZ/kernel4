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

// TODO: Overload some methods
uint16_t *Console::video = (uint16_t *)(0xB8000 + 160);

/**
 * Creates a new textmode console
 */
Console::Console()
{
	// Set video pointer to beginning of ConsoleArea::Middle
	this->video = (uint16_t *)(0xB8000 + 160);

	// Set color to gray on black
	this->attribute = 0x0700;

	// Convert numerical values to Hex by default
	this->state = ConsoleState::Hex;

	// Print in lines 2-24 by default
	this->area = ConsoleArea::Middle;

	// Set last positions of the areas to their beginning
	this->vl_top = (uint16_t *)(0xB8000);
	this->vl_middle = this->video;
	this->vl_bottom = (uint16_t *)(0xB8000 + 160 * 24);
}

/**
 * Destroy the console
 */
Console::~Console()
{
}

/**
 * Clear the console buffer
 *
 * This will clear ConsoleArea::Middle to black and
 * ConsoleArea::Top and ConsoleArea::Bottom to gray.
 */
void Console::Clear()
{
	// This is needed so we don't get the video pointer messed up
	*this << ConsoleArea::Middle;

	// Reset video to beginning of ConsoleArea::Middle
	this->video = (uint16_t *)(0xB8000 + 160);

	// Clear the whole buffer
	memset((void *)0xB8000, 0, 80 * 25 * 2);

	// Clear ConsoleArea::Top and ConsoleArea::Bottom to gray
	// (The printing function will set the background color)
	*this << ConsoleArea::Top
		<< "                                                                                \n"
		<< ConsoleArea::Bottom
		<< "                                                                                \n"
		<< ConsoleArea::Middle;
}

/**
 * Get information on what ConsoleArea is currently drawn own
 *
 * @returns The ConsoleArea currently in use
 */
ConsoleArea Console::GetArea()
{
	return this->area;
}

/**
 * Get the attribute currently in use
 *
 * Note: This will not return 0x7* when ConsoleArea::Top
 * or ConsoleArea::Bottom are in use.
 *
 * @returns The attribute currently in use
 */
uint8_t Console::GetAttribute()
{
	return (this->attribute >> 8);
}

/**
 * Get the current state
 *
 * @returns The current state
 */
ConsoleState Console::GetState()
{
	return this->state;
}

/**
 * Set a ConsoleArea to draw on
 *
 * @param area The new ConsoleArea to draw on
 */
void Console::SetArea(ConsoleArea area)
{
	// First, save the last video pointer position of
	// the current ConsoleArea into the correct vl_* variable
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

	// Set the new ConsoleArea
	this->area = area;

	// Reset the video pointer position to the last known
	// position in this ConsoleArea
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

/**
 * Set an attribute to use
 *
 * @param attribute The new attribute byte
 */
void Console::SetAttribute(uint8_t attribute)
{
	this->attribute = (attribute << 8);
}

/**
 * Set a new ConsoleState
 *
 * @param state The new ConsoleState
 */
void Console::SetState(ConsoleState state)
{
	this->state = state;
}

/**
 * Convert a number into a string, with variable radix
 *
 * @param buffer The buffer to write the string into
 * @param buffersize The size of the buffer
 * @param number The number to convert
 * @param base The radix
 */
void Console::Convert(char *buffer, uint16_t buffersize, uint64_t number, uint16_t base)
{
	// Reserve an internal buffer
	char c[256];

	int16_t i;

	// Highest allowed radix is 35
	base %= 36;

	// Highest allowed buffersize is 256
	buffersize %= 257;

	if(this->state == ConsoleState::HexFixed && base == 16)
	{
		// Convert the number to a string
		for(i = 255; i > 255 - 16; i--)
		{
			// Convert to character
			c[i] = (number % base) + 0x30;

			// Add 7 if we have to use a character
			if(c[i] > 0x39)
			{
				c[i] += 7;
			}

			// Divide the number by the radix
			number /= base;
		}
	}
	else
	{
		// If number is 0 time can be saved
		if(number == 0)
		{
			buffer[0] = '0';
			buffer[1] = 0;

			return;
		}

		// Convert the number to a string
		// The string is reversed!
		for(i = 255; i > 0 && number > 0; i--)
		{
			// Convert to character
			c[i] = (number % base) + 0x30;

			// Add 7 if we have to use a character
			if(c[i] > 0x39)
			{
				c[i] += 7;
			}

			// Divide the number by the radix
			number /= base;
		}
	}

	i++;
	uint8_t j;

	// Now reverse the string and copy it to the buffer
	for(j = 0; j < (buffersize - 1) && i < 256; i++, j++)
	{
		buffer[j] = c[i];
	}

	// Terminate the buffer string with \0
	buffer[j] = 0;
}

/**
 * Print a string to the console buffer
 *
 * @param text The string to print
 * @returns A reference to the console
 */
Console &Console::operator<<(const char *text)
{
	// Which area to we have to draw on
	// (Yes, there are many bytes "wasted" here, but
	// this also does speed up the loop significantly)
	switch(this->GetArea())
	{
		case ConsoleArea::Middle:
			// Read each char of the string until \0
			while(*text)
			{
				switch(*text)
				{
					// If the current char is \n advance
					// the video pointer to the row below
					case '\n':
						video = (uint16_t *)((uint64_t)video + 160);
						break;

					// If the current char is \r reset the video
					// pointer to the beginning of the current line
					case '\r':
						video = (uint16_t *)((uint64_t)video - (((uint64_t)video - 0xB8000) % 160));
						break;

					// Print the character
					default:
						// Scroll if behind end of area
						if((uint64_t)video >= 0xB8000 + 160 * 24)
						{
							memmove((void *)(0xB8000 + 160), (void*)(0xB8000 + 160 * 2), 160 * 22);
							memset((void *)(0xB8000 + 160 * 23), 0, 160);
							video = (uint16_t *)((uint64_t)video - 160);
						}

						// Here the character gets printed
						*(video++) = *text | this->attribute;
						break;
				}

				// Advance to next character in string
				text++;
			}
			break;
		case ConsoleArea::Top:
			// Read each char of the string until \0
			while(*text)
			{
				switch(*text)
				{
					// If the current char is \n or \r reset
					// the video pointer to beginning of line
					case '\n':
					case '\r':
						video = (uint16_t *)0xB8000;
						break;

					default:
						// Scroll if behind end of area
						if((uint64_t)video >= 0xB8000 + 160)
						{
							memmove((void *)0xB8000, (void *)(0xB8000 + 2), 158);
							memset((void *)(0xB8000 + 158), 0, 2);
							video = (uint16_t *)((uint64_t)video - 2);
						}

						// Here the character gets printed
						*(video++) = *text | this->attribute | (0x70 << 8);
						break;
				}

				// Advance to next character in string
				text++;
			}
			break;
		case ConsoleArea::Bottom:
			// Read each char of the string until \0
			while(*text)
			{
				switch(*text)
				{
					// If the current char is \n or \r reset
					// the video pointer to beginning of line
					case '\n':
					case '\r':
						video = (uint16_t *)(0xB8000 + 160 * 24);
						break;

					// Print the character
					default:
						// Scroll if behind end of area
						if((uint64_t)video >= 0xB8000 + 160 * 25)
						{
							memmove((void *)(0xB8000 + 160 * 24), (void *)(0xB8000 + 2 + 160 * 24), 158);
							memset((void *)(0xB8000 + 158 + 160 * 24), 0, 2);
							video = (uint16_t *)((uint64_t)video - 2);
						}

						// Here the character gets printed
						*(video++) = *text | this->attribute | (0x70 << 8);
						break;
				}

				// Advance to next character in string
				text++;
			}
			break;
	}

	return *this;
}

/**
 * Prints a number to the console buffer
 *
 * @param number The number to print
 * @returns A reference to the console
 */
Console &Console::operator<<(uint64_t number)
{
	// Create a buffer for the converted number
	char converted[256];

	// Convert the number with the correct radix
	switch(this->state)
	{
		case ConsoleState::Decimal:
			this->Convert(converted, 256, number, 10);
			break;
		case ConsoleState::Hex:
		case ConsoleState::HexFixed:
			this->Convert(converted, 256, number, 16);
			*this << "0x";
			break;
		case ConsoleState::Octal:
			this->Convert(converted, 256, number, 8);
			*this << "0";
			break;
	}

	// Print and return
	return (*this << converted);
}

/**
 * Sets a new foreground color
 *
 * @param color The new foreground color
 * @returns A reference to the console
 */
Console &Console::operator<<(ConsoleColor color)
{
	// Set the attribute of the console
	this->SetAttribute((uint8_t)color);

	return *this;
}

/**
 * Set a new ConsoleState
 *
 * @param state The new ConsoleState
 * @returns A reference to the console
 */
Console &Console::operator<<(ConsoleState state)
{
	// Set the state of the console
	this->SetState(state);

	return *this;
}

/**
 * Change the ConsoleArea to draw on
 *
 * @param area The ConsoleArea to draw on
 * @returns A reference to the consoel
 */
Console &Console::operator<<(ConsoleArea area)
{
	// Set the ConsoleArea
	this->SetArea(area);

	return *this;
}
