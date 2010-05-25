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
