#include "stdafx.h"
#include "ConstExpressionCalc.h"


// Конструктор
ConstExpressionCalc::ConstExpressionCalc()
{
}

// Деструктор
ConstExpressionCalc::~ConstExpressionCalc()
{
}

// Вычислить
bool ConstExpressionCalc::calculate(Node* node)
{
	return true;
}

// Ошибка
std::string ConstExpressionCalc::errorMsg()
{
	return "";
}

// Тип результата
TypeSymbol* ConstExpressionCalc::getType()
{
	return new TypeSymbol("int", 4);
}

// Получить результат
char ConstExpressionCalc::asChar()
{
	return 10;
}

// Получить результат
int ConstExpressionCalc::asInt()
{
	return 10;
}

// Получить результат
float ConstExpressionCalc::asFloat()
{
	return 10;
}

// Получить результат
void* ConstExpressionCalc::asPtr()
{
	return NULL;
}
