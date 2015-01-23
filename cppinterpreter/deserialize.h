#pragma once

struct BinaryExpression
{
	uint32_t  totalSize;
	uint16_t  numTreeDescriptors;
	uint16_t  numStrings;
	uint16_t  numSymbols;
	uint16_t  numNumbers;
	uint16_t* treeDescriptors;
	char*     strings;
	char*     symbols;
	double*   numbers;
	uint32_t  dummy; // for 32 byte alignment
};

typedef uint16_t Node;

struct ExtendedBinaryExpression
{
	Node*     treeDescriptors;
	char**    strings;
	char**    symbols;
	double*   numbers;
};

ExtendedBinaryExpression* Unpack(BinaryExpression* blob);
atom_t* Deserialize(ExtendedBinaryExpression* expr);