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

#ifndef _SPLAY_H_
#define _SPLAY_H_

#include <stdint.h>

template <class T>
class SplayTreeNode
{
	public:
		SplayTreeNode();
		SplayTreeNode(T *data);
		~SplayTreeNode();

		SplayTreeNode<T> *Left;
		SplayTreeNode<T> *Right;
		T *Data;
};

template <class T>
class SplayTree
{
	public:
		SplayTree();
		~SplayTree();

		void Add(T *data);
		void Add(SplayTreeNode<T> *data);
		void Remove(T *data);
		void Remove(SplayTreeNode<T> *data);
		SplayTreeNode<T> *Search(T *data);
		SplayTreeNode<T> *Search(T *data, SplayTreeNode<T> *root);
		uint64_t Size();
		SplayTreeNode<T> Top();

	private:
		void Splay(T *data);
		void Splay(SplayTreeNode<T> *data);

		SplayTreeNode<T> *Root;
		uint64_t Elements;
};

template <class T>
SplayTreeNode<T>::SplayTreeNode()
{
	this->Left = NULL;
	this->Right = NULL;
	this->Data = NULL;
}

template <class T>
SplayTreeNode<T>::SplayTreeNode(T *data)
{
	this->Left = NULL;
	this->Right = NULL;
	this->Data = data;
}

template <class T>
SplayTreeNode<T>::~SplayTreeNode()
{
}

template <class T>
SplayTree<T>::SplayTree()
{
	this->Root = NULL;
	this->Elements = 0;
}

template <class T>
SplayTree<T>::~SplayTree()
{
}

template <class T>
void SplayTree<T>::Add(T *data)
{
	if(this->Search(data) == NULL)
	{
		this->Add(new SplayTreeNode<T>(data));
	}
}

template <class T>
void SplayTree<T>::Add(SplayTreeNode<T> *data)
{
	if(this->Root == NULL)
	{
		this->Root = data;
	}
	else
	{
		this->Splay(data->Data);

		if(*data->Data == *this->Root->Data)
		{
			return;
		}
		else if(*data->Data < *this->Root->Data)
		{
			data->Left = this->Root->Left;
			data->Right = this->Root;
			this->Root->Left = NULL;
		}
		else
		{
			data->Right = this->Root->Right;
			data->Left = this->Root;
			this->Root->Right = NULL;
		}
		
		this->Root = data;
		this->Elements++;
	}
}

template <class T>
void SplayTree<T>::Remove(T *data)
{
	if(this->Root != NULL && *this->Search(data)->Data == *data)
	{
		SplayTreeNode<T> *d = new SplayTreeNode<T>(data);
		SplayTreeNode<T> *n = this->Root;

		this->Remove(d);

		delete d;
		delete n;
	}
}

template <class T>
void SplayTree<T>::Remove(SplayTreeNode<T> *data)
{
	if(this->Root == NULL)
	{
		return;
	}

	this->Splay(data->Data);

	if(*this->Root->Data == *data->Data)
	{
		if(this->Root->Left == NULL)
		{
			this->Root = this->Root->Right;
		}
		else
		{
			SplayTreeNode<T> *n = this->Root;
			this->Root = this->Root->Left;
			this->Splay(data->Data);
			n->Right = this->Root->Right;
		}

		this->Elements--;	
	}
}

template <class T>
SplayTreeNode<T> *SplayTree<T>::Search(T *data)
{
	if(this->Root == NULL)
	{
		return NULL;
	}

	this->Splay(data);

	if(*this->Root->Data == *data)
	{
		return this->Root;
	}
	else
	{
		return NULL;
	}
}

template <class T>
uint64_t SplayTree<T>::Size()
{
	return this->Elements;
}

template <class T>
void SplayTree<T>::Splay(T *data)
{
	if(this->Root == NULL)
	{
		return;
	}

	SplayTreeNode<T> n = SplayTreeNode<T>();
	SplayTreeNode<T> *l, *r, *y;

	l = r = &n;

	while(1)
	{
		if(*data < *this->Root->Data)
		{
			if(this->Root->Left == NULL)
			{
				break;
			}

			if(*data < *this->Root->Left->Data)
			{
				y = this->Root->Left;
				this->Root->Left = y->Right;
				y->Right = this->Root;
				this->Root = y;

				if(this->Root->Left == NULL)
				{
					break;
				}
			}

			r->Left = this->Root;
			r = this->Root;
			this->Root = this->Root->Left;
		}
		else if(*data > *this->Root->Data)
		{
			if(this->Root->Right == NULL)
			{
				break;
			}

			if(*data > *this->Root->Right->Data)
			{
				y = this->Root->Right;
				this->Root->Right = y->Left;
				y->Left = this->Root;
				this->Root = y;

				if(this->Root->Right == NULL)
				{
					break;
				}
			}

			l->Right = this->Root;
			l = this->Root;
			this->Root = this->Root->Right;
		}
		else
		{
			break;
		}
	}

	l->Right = this->Root->Left;
	r->Left = this->Root->Right;
	this->Root->Left = n.Right;
	this->Root->Right = n.Left;
}

template <class T>
void SplayTree<T>::Splay(SplayTreeNode<T> *data)
{
	this->Splay(data->Data);
}

template <class T>
SplayTreeNode<T> SplayTree<T>::Top()
{
	return this->Root;
}

#endif
