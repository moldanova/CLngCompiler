#pragma once

#include "Symbols.h"

class Value
{
public:
	Value();
	Value operator = (Value val);
	// Привести типы значений
	void checkType(Value& val);
	// тип значения
	TypeSymbol* type;
	// Значение
	char asChar;
	int asInt;
	float asFloat;
};

Value operator || (Value l, Value r);
Value operator && (Value l, Value r);
Value operator | (Value l, Value r);
Value operator ^ (Value l, Value r);
Value operator & (Value l, Value r);
Value operator == (Value l, Value r);
Value operator != (Value l, Value r);
Value operator < (Value l, Value r);
Value operator <= (Value l, Value r);
Value operator >= (Value l, Value r);
Value operator > (Value l, Value r);
Value operator << (Value l, Value r);
Value operator >> (Value l, Value r);
Value operator - (Value l, Value r);
Value operator + (Value l, Value r);
Value operator * (Value l, Value r);
Value operator / (Value l, Value r);
Value operator % (Value l, Value r);

Value operator - (Value r);
Value operator ~ (Value r);
Value operator ! (Value r);
Value operator ++ (Value r);
Value operator -- (Value r);
Value operator ++ (Value r, int);
Value operator -- (Value r, int);