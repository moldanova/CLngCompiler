#pragma once

// Описание выделенной лексемы
class Lexeme
{
public:
	// Конструктор
	Lexeme();
	// Конструктор
	Lexeme(int code, std::string text, int line, int col);
	// Деструктор
	~Lexeme();
	// Получить как целое число
	int asInt();
	// Получить как вещественное число
	float asFloat();
	// Оператор присваивания
	Lexeme& operator = (const Lexeme& l);
	// Сравнить код лексемы
	bool operator == (int code);
	// Сравнить код лексемы
	bool operator != (int code);

	int code;			// Код лексемы
	std::string text;	// Текст лексемы
	int line;			// Номер строки
	int col;			// Номер символа в строке
};

