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

int main(int argc,  char** argv)
{
	argv; argc;
	/*
	uint16_t id = 0;
	uint16_t* idp = &id;
	atom* e = deserialize(&idp, extend(unpack(nullptr)), nullptr);
	return int(e) & 2;
*/

	HANDLE pipe = CreateFile("\\\\.\\pipe\\memeparser", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (pipe == INVALID_HANDLE_VALUE)
	{
	    return GetLastError();
	}

	//string message = "Hi";

	//cout << message.length();

	char* message = "Hello World!";

	DWORD numWritten;
	WriteFile(pipe, message, zstrlen(message), &numWritten, NULL); 
}