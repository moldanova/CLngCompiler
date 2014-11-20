#pragma once

// Описание выделенной лексемы
class Lexeme
{
public:
	Lexeme();
	Lexeme(int code, std::string text, int line, int col);
	~Lexeme();
	int asInt();
	float asFloat();
	Lexeme& operator = (const Lexeme& l);
	bool operator == (int code);
	bool operator != (int code);

	int code;			// Код лексемы
	std::string text;	// Текст лексемы
	int line;			// Номер строки
	int col;			// Номер символа в строке
};

