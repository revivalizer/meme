#include "pch.h"

uint8_t* programMem = nullptr;
uint8_t* emitPos = nullptr;

void exec_alloc()
{
	if (programMem==nullptr)
	{
		programMem = (uint8_t*)VirtualAlloc(NULL, 1024*1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		emitPos = programMem;
	}
}

void exec_emit(uint8_t byte)
{
	*emitPos++ = byte;
}

typedef void (*pfunc)(void);

void exec_run()
{
	pfunc exec = (pfunc)programMem;
	exec();
}

void exec_dealloc()
{
	emitPos = programMem;
}