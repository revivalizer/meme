#include "pch.h"

template <typename T, typename U>
T* ZUnpack(T* a, U* b)
{
	return (T*)((uintptr_t)a+(uintptr_t)b);
}

BinaryExpression* unpack(BinaryExpression* expr)
{
	expr->treeDescriptors = ZUnpack(expr->treeDescriptors, expr);
	expr->strings         = ZUnpack(expr->strings, expr);
	expr->symbols         = ZUnpack(expr->symbols, expr);
	expr->numbers         = ZUnpack(expr->numbers, expr);

	return expr;
}

enum NodeType // this must match F# enum, of course
{
	EOC = 0,
	List,
	String,
	Symbol,
	Number,
};

Node next(Node** node)
{
	return *((*node)++);
}

atom_t* deserialize(Node** node, ExtendedBinaryExpression* expr)
{
	switch (next(node))
	{
		case EOC:
			return nullptr;
			break;
		case List:
		{
			atom* n;
			atom_t *cur = nil;

			while ((n = deserialize(node, expr)) != nullptr)
			{
				cur = cons(n, cur);
			}

			return ReverseInPlace(cur);
			break;
		}
		case String:
			return new_string(expr->strings[next(node)]);
			break;
		case Symbol:
			return new_symbol(expr->symbols[next(node)]);
			break;
		case Number:
			return new_number(expr->numbers[next(node)]);
			break;
	}

	return nullptr;
}

void generate_string_list(char* str, char** strList, uint16_t num)
{
	// unpacks an array of zero terminated strings into a list of string pointers
	for (uint16_t i=0; i<num; i++)
	{
		// invariant: str always points to the next string
		strList[i] = str;

		while (*str)
			str++;

		str++;
	}
}

ExtendedBinaryExpression* extend(BinaryExpression* expr)
{
	auto newexpr = new ExtendedBinaryExpression;
	newexpr->treeDescriptors = expr->treeDescriptors;
	newexpr->numbers = expr->numbers;
	newexpr->strings = new char*[expr->numStrings];
	newexpr->symbols = new char*[expr->numSymbols];

	generate_string_list(expr->strings, newexpr->strings, expr->numStrings);
	generate_string_list(expr->symbols, newexpr->symbols, expr->numSymbols);

	return newexpr;
}

ExtendedBinaryExpression* Unpack(BinaryExpression* blob)
{
	return extend(unpack(blob));
}

atom_t* Deserialize(ExtendedBinaryExpression* expr)
{
	Node* nodes = expr->treeDescriptors;
	return deserialize(&nodes, expr);
}
