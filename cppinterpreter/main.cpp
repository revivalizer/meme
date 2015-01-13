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

void IterateDir(const char* const relativePath)
{
	char absolutePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, absolutePath);
	zstrcat(absolutePath, "\\");
	zstrcat(absolutePath, relativePath);
	zstrcat(absolutePath, "*");
	relativePath;

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(absolutePath, &ffd);

	do
	{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			zmsg(ffd.cFileName);
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);
	
	FindClose(hFind);
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
	//IterateDir("..\\test\\unittests");
	IterateDir("");

	size_t outSize = 1024*128;
	uint8_t* out = new uint8_t[outSize];
	BinaryExpression* raw = convertBinaryRepresentation("1", out, outSize);
	BinaryExpression* unpacked = unpack(raw);
	ExtendedBinaryExpression* extended = extend(unpacked);
	atom* result = deserialize(&(extended->treeDescriptors), extended, nullptr);
	result;
}