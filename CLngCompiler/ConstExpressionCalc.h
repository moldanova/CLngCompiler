#pragma once

#include "Node.h"
#include "Symbols.h"

// Класс для вычисления константных выражений
class ConstExpressionCalc
{
public:
	// Конструктор
	ConstExpressionCalc();
	// Деструктор
	virtual ~ConstExpressionCalc();
	// Вычислить
	bool calculate(Node* node);
	// Ошибка
	std::string errorMsg();
	// Тип результата
	TypeSymbol* getType();
	// Получить результат
	char asChar();
	// Получить результат
	int asInt();
	// Получить результат
	float asFloat();
	// Получить результат
	void* asPtr();
};

