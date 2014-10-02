#include "stdafx.h"
#include "Exceptions.h"

std::string SyntaxException::makeMessage(const char* msg, int line, int col)
{
	std::string s;
	s = "Syntax error: ";
	s += std::to_string(line);
	s += ":";
	s += std::to_string(col);
	s += " ";
	s += msg;
	return s;
}

SyntaxException::SyntaxException(const char* msg, int line, int col)
	: std::exception(makeMessage(msg, line, col).c_str())
{
}

SyntaxException::SyntaxException(const char* msg, Lexeme lex)
	: std::exception(makeMessage(msg, lex.line, lex.col).c_str())
{
}