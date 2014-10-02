#pragma once

#include "Lexeme.h"

// Класс синтаксической ошибки
class SyntaxException : public std::exception
{
public:
	static std::string makeMessage(const char* msg, int line, int col);
	SyntaxException(const char* msg, int line, int col);
	SyntaxException(const char* msg, Lexeme lex);
};

