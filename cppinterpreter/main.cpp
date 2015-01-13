#include "base/base.h"
#include "atom.h"
#include "deserialize.h"

#pragma function(memset)
void * __cdecl memset(void *pTarget, int value, size_t cbTarget) {
	unsigned char *p = static_cast<unsigned char *>(pTarget);
	while (cbTarget-- > 0) {
		*p++ = static_cast<unsigned char>(value);
	}
	return pTarget;
}

#pragma warning(disable:4800)

BinaryExpression* convertBinaryRepresentation(char* str, uint8_t* out, size_t outSize)
{
	DWORD cbRead;
	BOOL fSuccess = CallNamedPipeA("\\\\.\\pipe\\memeparser", str, zstrlen(str), out, outSize, &cbRead, 100);

	if (fSuccess==0)
	{
		zfatalerror("Could not open named pipe!");
	}
	return (BinaryExpression*)out;
}

int main(int argc,  char** argv)
{
	argv; argc;
	/*
	uint16_t id = 0;
	uint16_t* idp = &id;
	atom* e = deserialize(&idp, extend(unpack(nullptr)), nullptr);
	return int(e) & 2;
*/
	size_t outSize = 1024*128;
	uint8_t* out = new uint8_t[outSize];
	BinaryExpression* raw = convertBinaryRepresentation("1", out, outSize);
	BinaryExpression* unpacked = unpack(raw);
	ExtendedBinaryExpression* extended = extend(unpacked);
	atom* result = deserialize(&(extended->treeDescriptors), extended, nullptr);
	result;
}