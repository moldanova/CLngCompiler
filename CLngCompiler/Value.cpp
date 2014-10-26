#include "stdafx.h"
#include "Value.h"


// Конструктор
Value::Value()
{
	type = NULL;
	asChar = 0;
	asInt = 0;
	asFloat = 0;
}

// Оператор присваивания
Value Value::operator = (Value val)
{
	type = val.type;
	asChar = val.asChar;
	asInt = val.asInt;
	asFloat = val.asFloat;
	return *this;
}

// Привести типы значений
void Value::checkType(Value& val)
{
	if (type->isChar() && val.type->isInt())
	{
		if (val.type->isInt())
		{
			type = val.type;
			asInt = asChar;
			asChar = 0;
		}
		else if (val.type->isFloat())
		{
			type = val.type;
			asFloat = asChar;
			asChar = 0;
		}
	}
	else if (type->isInt())
	{
		if (val.type->isFloat())
		{
			type = val.type;
			asFloat = asInt;
			asInt = 0;
		}
	} 
}

Value operator || (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = new TypeSymbol("int");

	if (v.type->isChar())
		v.asChar = l.asChar || r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt || r.asInt;
	else
		v.asFloat = l.asFloat || r.asFloat;

	return v;
}

Value operator && (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = new TypeSymbol("int");

	if (v.type->isChar())
		v.asChar = l.asChar && r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt && r.asInt;
	else
		v.asFloat = l.asFloat && r.asFloat;

	return v;
}

Value operator | (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = l.type;

	if (v.type->isChar())
		v.asChar = l.asChar | r.asChar;
	else
		v.asInt = l.asInt | r.asInt;

	return v;
}

Value operator ^ (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = l.type;

	if (v.type->isChar())
		v.asChar = l.asChar ^ r.asChar;
	else
		v.asInt = l.asInt ^ r.asInt;

	return v;
}

Value operator & (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = l.type;

	if (v.type->isChar())
		v.asChar = l.asChar & r.asChar;
	else
		v.asInt = l.asInt & r.asInt;

	return v;
}

Value operator == (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = new TypeSymbol("int");

	if (v.type->isChar())
		v.asChar = l.asChar == r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt == r.asInt;
	else
		v.asFloat = l.asFloat == r.asFloat;

	return v;
}

Value operator != (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = new TypeSymbol("int");

	if (v.type->isChar())
		v.asChar = l.asChar != r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt != r.asInt;
	else
		v.asFloat = l.asFloat != r.asFloat;

	return v;
}

Value operator < (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = new TypeSymbol("int");

	if (v.type->isChar())
		v.asChar = l.asChar < r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt < r.asInt;
	else
		v.asFloat = l.asFloat < r.asFloat;

	return v;
}

Value operator <= (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = new TypeSymbol("int");

	if (v.type->isChar())
		v.asChar = l.asChar <= r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt <= r.asInt;
	else
		v.asFloat = l.asFloat <= r.asFloat;

	return v;
}

Value operator >= (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = new TypeSymbol("int");

	if (v.type->isChar())
		v.asChar = l.asChar >= r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt >= r.asInt;
	else
		v.asFloat = l.asFloat >= r.asFloat;

	return v;
}

Value operator > (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = new TypeSymbol("int");

	if (v.type->isChar())
		v.asChar = l.asChar > r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt > r.asInt;
	else
		v.asFloat = l.asFloat > r.asFloat;

	return v;
}

Value operator << (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = l.type;

	if (v.type->isChar())
		v.asChar = l.asChar << r.asChar;
	else
		v.asInt = l.asInt << r.asInt;

	return v;
}

Value operator >> (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = l.type;

	if (v.type->isChar())
		v.asChar = l.asChar >> r.asChar;
	else
		v.asInt = l.asInt >> r.asInt;

	return v;
}

Value operator - (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = l.type;

	if (v.type->isChar())
		v.asChar = l.asChar - r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt - r.asInt;
	else
		v.asFloat = l.asFloat - r.asFloat;

	return v;
}

Value operator + (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = l.type;

	if (v.type->isChar())
		v.asChar = l.asChar + r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt + r.asInt;
	else
		v.asFloat = l.asFloat + r.asFloat;

	return v;
}

Value operator * (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = l.type;

	if (v.type->isChar())
		v.asChar = l.asChar * r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt * r.asInt;
	else
		v.asFloat = l.asFloat * r.asFloat;

	return v;
}

Value operator / (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = l.type;

	if (v.type->isChar())
		v.asChar = l.asChar / r.asChar;
	else if (v.type->isInt())
		v.asInt = l.asInt / r.asInt;
	else
		v.asFloat = l.asFloat / r.asFloat;

	return v;
}

Value operator % (Value l, Value r)
{
	l.checkType(r);
	r.checkType(l);

	Value v;
	v.type = l.type;

	if (v.type->isChar())
		v.asChar = l.asChar % r.asChar;
	else 
		v.asInt = l.asInt % r.asInt;

	return v;
}

Value operator - (Value r)
{
	Value v;
	v.type = r.type;
	if (v.type->isChar())
		v.asChar = -r.asChar;
	else if (v.type->isInt())
		v.asInt = - r.asInt;
	else
		v.asFloat = -r.asFloat;
	return v;
}

Value operator ~ (Value r)
{
	Value v;
	v.type = r.type;
	if (v.type->isChar())
		v.asChar = ~r.asChar;
	else
		v.asInt = ~r.asInt;

	return v;
}

Value operator ! (Value r)
{
	Value v;
	v.type = r.type;
	if (v.type->isChar())
		v.asChar = !r.asChar;
	else if (v.type->isInt())
		v.asInt = !r.asInt;
	else
		v.asFloat = !r.asFloat;
	return v;
}

Value operator ++ (Value r)
{
	Value v;
	v.type = r.type;
	if (v.type->isChar())
		v.asChar = ++r.asChar;
	else if (v.type->isInt())
		v.asInt = ++r.asInt;
	else
		v.asFloat = ++r.asFloat;
	return v;
}

Value operator -- (Value r)
{
	Value v;
	v.type = r.type;
	if (v.type->isChar())
		v.asChar = --r.asChar;
	else if (v.type->isInt())
		v.asInt = --r.asInt;
	else
		v.asFloat = --r.asFloat;
	return v;
}

Value operator ++ (Value r, int)
{
	Value v;
	v.type = r.type;
	if (v.type->isChar())
		v.asChar = r.asChar++;
	else if (v.type->isInt())
		v.asInt = r.asInt++;
	else
		v.asFloat = r.asFloat++;
	return v;
}

Value operator -- (Value r, int)
{
	Value v;
	v.type = r.type;
	if (v.type->isChar())
		v.asChar = r.asChar--;
	else if (v.type->isInt())
		v.asInt = r.asInt--;
	else
		v.asFloat = r.asFloat--;
	return v;
}