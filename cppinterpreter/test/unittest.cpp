#include "pch.h"

char* LoadFile(const char* const path)
{
	HANDLE hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	char* data = nullptr;

	if (hFile==INVALID_HANDLE_VALUE)
	{
		zfatalerror("Couldn't open file %s", path);
	}

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

atom_t* RunTest(atom_t* test)
{
	atom_t* exprStr = car(test);
	atom_t* expectedStr = car(cdr(test));
	atom_t* expr = Parse(string(exprStr));
	atom_t* expected = Parse(string(expectedStr));

	atom_t* result = Eval(expr, CreateGlobalEnvironment());

	bool success = StructuralEquality(result, expected);

	return cons(new_boolean(success), cons(exprStr, cons(expectedStr, cons(result, nil))));
}

atom_t* RunTestsInFile(const char* const path)
{
	char* data = LoadFile(path);
	atom_t* tests = Parse(data);

	iter nextTest(tests);

	atom_t* result = nil;

	while (atom_t* test = nextTest())
	{
		result = cons(RunTest(test), result);
	}

	return ReverseInPlace(result);
}

atom_t* RunTestsInDir(const char* const relativePath)
{
	char absolutePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, absolutePath);
	zstrcat(absolutePath, "\\");
	zstrcat(absolutePath, relativePath);
	zstrcat(absolutePath, "*");
	relativePath;

	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(absolutePath, &ffd);

	atom_t* result = nil;

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

	gc_mark(result);
	gc_sweep();
	
	FindClose(hFind);

	return ReverseInPlace(result);
}

void PrintVerboseTestResult(atom_t* result)
{
	char errorDescBuf[2048];

	auto nextFile = iter(result);

	bool dirSuccess = true;

	// For all files
	while (auto file = nextFile())
	{
		auto filename = string(car(file));
		auto fileresults = cdr(file);

		auto nextTest = iter(fileresults);

		bool fileSuccess = true;

		// For all tests
		while (auto test = nextTest())
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
				zstrcat(errorDescBuf, ": test FAILED\n\"");
				zstrcat(errorDescBuf, expr);
				zstrcat(errorDescBuf, "\" did not evaluate to \"");
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
	PrintVerboseTestResult(RunTestsInDir("..\\unittests\\"));
}
