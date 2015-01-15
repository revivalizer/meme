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

atom* RunTest(atom* test)
{
	atom* expr = car(test);
	atom* expected = car(cdr(test));

	atom* result = Eval(expr);

	bool success = StructuralEquality(result, expected);

	return cons(new_boolean(success), cons(expr, cons(expected, cons(result, nil))));
}

atom* RunTestsInFile(const char* const path)
{
	char* data = LoadFile(path);
	atom* tests = Parse(data);

	iter testiter(tests);

	atom* result = nil;

	while (atom* test = testiter())
	{
		result = cons(RunTest(test), result);
	}

	return ReverseInPlace(result);
}

atom* RunTestsInDir(const char* const relativePath)
{
	char absolutePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, absolutePath);
	zstrcat(absolutePath, "\\");
	zstrcat(absolutePath, relativePath);
	zstrcat(absolutePath, "*");
	relativePath;

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(absolutePath, &ffd);

	atom* result = nil;

	do
	{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			GetCurrentDirectory(MAX_PATH, absolutePath);
			zstrcat(absolutePath, "\\");
			zstrcat(absolutePath, relativePath);
			zstrcat(absolutePath, ffd.cFileName);

			result = cons(cons(new_string(ffd.cFileName), RunTestsInFile(absolutePath)), result);
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);
	
	FindClose(hFind);

	return ReverseInPlace(result);
}

void PrintVerboseTestResult(atom* result)
{
	char errorDescBuf[2048];

	auto fileiter = iter(result);

	bool dirSuccess = true;

	// For all files
	while (auto file = fileiter())
	{
		auto filename = string(car(file));
		auto fileresults = cdr(file);

		auto testiter = iter(fileresults);

		bool fileSuccess = true;

		// For all tests
		while (auto test = testiter())
		{
			// Get results
			bool success = boolean(car(test));
			auto expr = string(car(cdr(test)));
			auto expected = string(car(cdr(cdr(test))));

			dirSuccess &= success;
			fileSuccess &= success;

			// Print error if neccesary
			if (!success)
			{
				errorDescBuf[0] = '\0';
				zstrcat(errorDescBuf, filename);
				zstrcat(errorDescBuf, ": Test FAILED\n");
				zstrcat(errorDescBuf, expr);
				zstrcat(errorDescBuf, " did not match ");
				zstrcat(errorDescBuf, expected);

				zmsg(errorDescBuf);
			}
		}

		if (fileSuccess)
			zmsg("%s: all tests passed", filename);
	}

	if (dirSuccess)
		zmsg("ALL TESTS PASSED.");
	else
		zmsg("SOME TESTS FAILED.");

	zmsg("Done.");
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
	PrintVerboseTestResult(RunTestsInDir("..\\test\\unittests\\"));

	size_t outSize = 1024*128;
	uint8_t* out = new uint8_t[outSize];
	BinaryExpression* raw = convertBinaryRepresentation("1", out, outSize);
	BinaryExpression* unpacked = unpack(raw);
	ExtendedBinaryExpression* extended = extend(unpacked);
	atom* result = deserialize(&(extended->treeDescriptors), extended, nullptr);
	result;
}