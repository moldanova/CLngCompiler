#include "stdafx.h"
#include "Lexeme.h"

// Конструктор
Lexeme::Lexeme()
{
	this->code = -1;
	this->text = "";
	this->line = -1;
	this->col = -1;
}

// Конструктор
Lexeme::Lexeme(int code, std::string text, int line, int col)
{
	this->code = code;
	this->text = text;
	this->line = line;
	this->col = col;
}

// Деструктор
Lexeme::~Lexeme()
{
}

// Получить как целое число
int Lexeme::asInt()
{
	return atoi(text.c_str());
}

// Получить как вещественное число
float Lexeme::asFloat()
{
	return (float)atof(text.c_str());
}

// Оператор присваивания
Lexeme& Lexeme::operator = (const Lexeme& l)
{
	code = l.code;
	text = l.text;
	line = l.line;
	col = l.col;
	return *this;
}

// Сравнить код лексемы
bool Lexeme::operator == (int code)
{
	return this->code == code;
}

// Сравнить код лексемы
bool Lexeme::operator != (int code)
{
	return this->code != code;
}