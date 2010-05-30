
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

#ifndef _LIST_H_
#define _LIST_H_

#include <stdint.h>
#include <stddef.h>

template <class T>
class ListNode
{
	public:
		ListNode(T *data = NULL);
		~ListNode();

		ListNode<T> *Previous;
		ListNode<T> *Next;
		T *Data;
};

template <class T>
class List
{
	public:
		List();
		~List();

		void Add(T *data, uint64_t index = -1);
		void Add(ListNode<T> *data, uint64_t index = -1);
		void Remove(T *data);
		void Remove(ListNode<T> *data);
		void Remove(uint64_t index);
		ListNode<T> *Search(T *data);
		uint64_t Size();

	private:
		ListNode<T> *start;
		ListNode<T> *end;
		uint64_t elements;
}

template <class T>
ListNode<T>::ListNode(T *data)
{
	this->Previous = NULL;
	this->Next = NULL;
	this->Data = data;
}

template <class T>
ListNode<T>::~ListNode()
{
}

template <class T>
List<T>::List()
{
	this->start = NULL;
	this->end = NULL;
	this->elements = 0;
}

template <class T>
List<T>::~List()
{
}

template <class T>
void List<T>::Add(T *data, uint64_t index);
{
	this->Add(new ListNode<T>(data), index);
}

template <class T>
void List<T>::Add(ListNode<T> *data, uint64_t index);
{
	if(this->start == NULL)
	{
		this->start = data;
		this->end = data;
	}
	else
	{
		if(index == -1)
		{
			this->end->Next = data;
			data->Previous = this->end;
			this->end = data;
		}
		else
		{
			ListNode<T> *I;
			uint64_t i;

			if(index <= ((this->elements - 1) / 2))
			{
				for(i = 0, I = this->start; i < index && I->Next != NULL; i++, I = I->Next);
			}
			else
			{
				for(i = this->elements - 1, I = this->end; i > index && I->Previous != NULL; i--, I = I->Previous);
			}

			data->Previous = I->Previous;
			I->Previous->Next = data
			I->Previous = data;
			data->Next = I;
		}
	}
}

void List<T>::Remove(T *data);
void List<T>::Remove(ListNode<T> *data);
void List<T>::Remove(uint64_t index);
ListNode<T> *List<T>::Search(T *data);
uint64_t List<T>::Size();

#endif
