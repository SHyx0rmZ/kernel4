#include "managers.h"

void test()
{
	MemoryStack *a = new MemoryStack(0x0000);
	MemoryStack *b = new MemoryStack[2] { MemoryStack(0x1000), MemoryStack(0x2000) };
	a->Push(0x2);
	delete a;
	delete[] b;
}

void *operator new(size_t size)
{
	asm(
		"mov 8(%rbp), %r10 \n"
	);

	size = size;

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
