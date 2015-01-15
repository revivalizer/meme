#include "pch.h"

#pragma function(memset)
void * __cdecl memset(void *pTarget, int value, size_t cbTarget) {
	unsigned char *p = static_cast<unsigned char *>(pTarget);
	while (cbTarget-- > 0) {
		*p++ = static_cast<unsigned char>(value);
	}
	return pTarget;
}

#pragma warning(disable:4800)

BinaryExpression* convertBinaryRepresentation(const char* const str, uint8_t* out, size_t outSize)
{
	DWORD cbRead;
	BOOL fSuccess = CallNamedPipeA("\\\\.\\pipe\\memeparser", LPVOID(str), zstrlen(str), out, outSize, &cbRead, 100);

	if (fSuccess==0)
	{
		zfatalerror("Could not open named pipe!");
	}
	return (BinaryExpression*)out;
}

char* LoadFile(const char* const path)
{
	HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	char* data = nullptr;

	if (hFile==INVALID_HANDLE_VALUE)
		zfatalerror("Couldn't open file %s", path);

	LARGE_INTEGER size;
	GetFileSizeEx(hFile, &size);

	// Just leak memory for now
	data = (char*)zalignedalloc(size.LowPart+1, 4);
	((char*)data)[size.LowPart] = '\0'; // append zero for easy text handling

	DWORD  dwBytesRead = 0;

	ReadFile(hFile, data, size.LowPart, &dwBytesRead, NULL);

	CloseHandle(hFile);
	return data;
}

atom* Parse(const char* const str)
{
	// Just leak memory for now
	size_t outSize = 1024*128;
	uint8_t* out = new uint8_t[outSize];
	BinaryExpression* raw = convertBinaryRepresentation(str, out, outSize);
	BinaryExpression* unpacked = unpack(raw);
	ExtendedBinaryExpression* extended = extend(unpacked);
	atom* result = deserialize(&(extended->treeDescriptors), extended, nullptr);
	return result;
}

atom* Eval(atom* expr)
{
	if (isnumber(expr)) 
		return expr;
	else if (isstring(expr))
		return expr;

	return nil;
}

bool StructuralEquality(atom* expr1, atom* expr2)
{
	if (type(expr1)!=type(expr2))
		return false;

	if (isnumber(expr1) && number(expr1)==number(expr2))
		return true;

	if (isstring(expr1) && zstrequal(string(expr1), string(expr2)))
		return true;

	return false;
}

char errorDescBuf[2048];

atom* RunTest(atom* test)
{
	atom* expr = car(test);
	atom* expected = car(cdr(test));

	atom* result = Eval(expr);

	bool success = StructuralEquality(result, expected);

	/*if (!success)
	{
		errorDescBuf[0] = '\0';
		zstrcat(errorDescBuf, "Test FAILED in ");
		zstrcat(errorDescBuf, filename);
		zstrcat(errorDescBuf, "\n");
		zstrcat(errorDescBuf, string(expr));
		zstrcat(errorDescBuf, " did not result in ");
		zstrcat(errorDescBuf, string());
	}*/
	
	return cons(new_boolean(success), cons(expr, cons(expected, cons(result, nil))));
}

atom* RunTestsInFile(const char* const path)
{
	char* data = LoadFile(path);
	atom* tests = Parse(data);

	iter testiter(tests);

	bool accSuccess = true;

	while (atom* test = testiter())
	{
		if (test==nil)
			return nil;

		bool success = RunTest(test);
		accSuccess &= success;

		if (!success)
		{
			errorDescBuf[0] = '\0';
		}
	}

	return nil;
}

void RunTestsInDir(const char* const relativePath)
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
			GetCurrentDirectory(MAX_PATH, absolutePath);
			zstrcat(absolutePath, "\\");
			zstrcat(absolutePath, relativePath);
			zstrcat(absolutePath, ffd.cFileName);

			RunTestsInFile(absolutePath);
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
	RunTestsInDir("..\\test\\unittests\\");

	size_t outSize = 1024*128;
	uint8_t* out = new uint8_t[outSize];
	BinaryExpression* raw = convertBinaryRepresentation("1", out, outSize);
	BinaryExpression* unpacked = unpack(raw);
	ExtendedBinaryExpression* extended = extend(unpacked);
	atom* result = deserialize(&(extended->treeDescriptors), extended, nullptr);
	result;
}