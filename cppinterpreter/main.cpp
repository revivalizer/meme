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

int main(int argc,  char** argv)
{
	argv; argc;
	/*
	uint16_t id = 0;
	uint16_t* idp = &id;
	atom* e = deserialize(&idp, extend(unpack(nullptr)), nullptr);
	return int(e) & 2;
*/

	bool fSuccess;
	DWORD  cbRead, cbToWrite, cbWritten, dwMode; 
	cbToWrite; cbWritten; cbRead; dwMode;

	char* message = "Hello World!\n";

	/*HANDLE pipe = CreateFile("\\\\.\\pipe\\memeparser", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	fSuccess = (bool)WriteFile(pipe, message, zstrlen(message), &cbWritten, NULL); 
	FlushFileBuffers(pipe);
	CloseHandle(pipe);
	*/

	/*char buf[50];
	fSuccess = CallNamedPipeA("\\\\.\\pipe\\memeparser", message, zstrlen(message)+1, buf, 49, &cbRead, 100);
	return GetLastError();
	*/

	HANDLE pipe = CreateFile("\\\\.\\pipe\\memeparser", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (pipe == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	// The pipe connected; change to message-read mode. 

	dwMode = PIPE_READMODE_MESSAGE; 
	fSuccess = SetNamedPipeHandleState( 
		pipe,    // pipe handle 
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!fSuccess) 
	{
		return GetLastError();
	}

	uint8_t* buffer = new uint8_t[1024*1024];

	DWORD numWritten;
	fSuccess = (bool)WriteFile(pipe, message, zstrlen(message)+1, &numWritten, NULL); 

	if (!fSuccess)
	{
		return GetLastError();
	}

	fSuccess = FlushFileBuffers(pipe);

	// Loop until no more data
	char* tbuf = (char*)buffer;
	do 
	{ 
		// Read from the pipe. 

		fSuccess = ReadFile( 
			pipe,    // pipe handle 
			tbuf,    // buffer to receive reply 
			(1024*1024)*sizeof(TCHAR),  // size of buffer 
			&cbRead,  // number of bytes read 
			NULL);    // not overlapped 
		
		tbuf += cbRead;

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA )
			break; 

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

}