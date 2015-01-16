#include "pch.h"

BinaryExpression* GetBinaryRepresentationFromNamedPipe(const char* const str, uint8_t* out, size_t outSize)
{
	DWORD cbRead;
	BOOL fSuccess = CallNamedPipeA("\\\\.\\pipe\\memeparser", LPVOID(str), zstrlen(str), out, outSize, &cbRead, 100);

	if (fSuccess==0)
	{
		zfatalerror("Could not open named pipe!");
	}
	return (BinaryExpression*)out;
}

BinaryExpression* GetBinaryRepresentationFromNamedPipe(const char* const str)
{
	// Just leak memory for now
	size_t outSize = 1024*128; // just guessing
	uint8_t* out = new uint8_t[outSize];
	BinaryExpression* raw = GetBinaryRepresentationFromNamedPipe(str, out, outSize);
	return raw;
}

atom_t* Parse(const char* const str)
{
	return Deserialize(Unpack(GetBinaryRepresentationFromNamedPipe(str)));
}