#pragma once

struct BinaryExpression
{
	uint32_t totalSize;
	uint16_t numTreeDescriptors;
	uint16_t numStrings;
	uint16_t numSymbols;
	uint16_t numNumbers;
	uint16_t* treeDescriptors;
	char*     strings;
	char*     symbols;
	double*   numbers;
	uint32_t dummy; // for alignment
};

struct ExtendedBinaryExpression
{
	uint16_t* treeDescriptors;
	char**     strings;
	char**     symbols;
	double*   numbers;
};


typedef struct atom atom_t;

struct Environment;
typedef uint16_t Node;


BinaryExpression* unpack(BinaryExpression* expr);
ExtendedBinaryExpression* extend(BinaryExpression* expr);
atom_t* deserialize(Node** node, ExtendedBinaryExpression* expr, Environment* env);