#include "pch.h"

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

atom* RunTest(atom* test)
{
	atom* expr = Parse(string(car(test)));
	atom* expected = Parse(string(car(cdr(test))));

	// NEXT
	// both expr and expected should be parsed before sending over

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
				zstrcat(errorDescBuf, ": Test FAILED\n\"");
				zstrcat(errorDescBuf, expr);
				zstrcat(errorDescBuf, "\" did evaluate to \"");
				zstrcat(errorDescBuf, expected);
				zstrcat(errorDescBuf, "\"");

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
}

void RunUnitTests()
{
	PrintVerboseTestResult(RunTestsInDir("..\\test\\unittests\\"));
}
