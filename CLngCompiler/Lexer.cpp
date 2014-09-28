#include "stdafx.h"
#include "Lexer.h"

#define PARSELEX(c1, lex1) \
	if (ch == c1) \
	{ \
		getChar(); \
		return makeLex(lex1); \
	}
#define PARSELEX2(c1, lex1, c2, lex2) \
	if (ch == c1) \
	{ \
		getChar(); \
		if (ch == c2) \
		{ \
			getChar(); \
			return makeLex(lex2); \
		} \
		else \
			return makeLex(lex1); \
	}
#define PARSELEX3(c1, lex1, c2, lex2, c3, lex3) \
	if (ch == c1) \
	{ \
		getChar(); \
		if (ch == c2) \
		{ \
			getChar(); \
			return makeLex(lex2); \
		} \
		else if (ch == c3) \
		{ \
			getChar(); \
			return makeLex(lex3); \
		} \
		else \
			return makeLex(lex1); \
	}
// Конструктор
Lexer::Lexer(const char* fileName)
	: input(fileName)
	, line(1)
	, col(0)
	, ch(-1)
{
	// Проверка входного файла
	if (!input)
		throw std::exception("Illegal filename");
}

// деструктор
Lexer::~Lexer()
{
}

// Получить следующую лексему
Lexeme Lexer::next()
{
	if (ch == -1)
		getChar();

	saveLine = line;
	saveCol = col;

	// Бесконечный цикл, чтобы можно было пропускать пробелы и комментарии
	while (true)
	{
		if (ch == -1)
			return makeLex(LEX_EOF);
		if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
		{
			getChar();
			saveLine = line;
			saveCol = col;
			continue;
		}
		PARSELEX2('!', LEX_NOT, '=', LEX_NOT_EQUAL)
		PARSELEX2('%', LEX_PERSENT, '=', LEX_PERSENT_ASSIGNMENT)
		PARSELEX3('&', LEX_AND, '=', LEX_AND_ASSIGNMENT, '&', LEX_LOGICAL_AND)
		PARSELEX('(', LEX_LPAREN)
		PARSELEX(')', LEX_RPAREN)
		PARSELEX2('*', LEX_MUL, '=', LEX_MUL_ASSIGNMENT)
		if (ch == '+')
		{
			getChar();
			PARSELEX('=', LEX_ADD_ASSIGNMENT)
			PARSELEX('+', LEX_INCREMENT)
			if (ch >= '0' && ch <= '9')
				return parseNumber();
			return makeLex(LEX_ADD);
		}
		PARSELEX(',', LEX_COMMA)
		if (ch == '-')
		{
			getChar();
			PARSELEX('=', LEX_SUB_ASSIGNMENT)
			PARSELEX('-', LEX_DECREMENT)
			PARSELEX('>', LEX_ARROW)
			if (ch >= '0' && ch <= '9')
				return parseNumber();
			return makeLex(LEX_SUB);
		}
		PARSELEX('.', LEX_DOT)
		if (ch == '/')
		{
			getChar();
			PARSELEX('=', LEX_DIV_ASSIGNMENT);
			if (ch == '*')
				parseComment1();
			else if (ch == '/')
				parseComment2();
			else
				return makeLex(LEX_DIV);
			continue;
		}
		PARSELEX(':', LEX_COLON)
		PARSELEX(';', LEX_SEMICOLON)
		if (ch == '<')
		{
			getChar();
			PARSELEX('=', LEX_LESS_EQUAL)
			PARSELEX2('<', LEX_LSHIFT, '=', LEX_LSHIFT_ASSIGNMENT)
			return makeLex(LEX_LESS);
		}
		PARSELEX2('=', LEX_ASSIGNMENT, '=', LEX_EQUAL)
		if (ch == '>')
		{
			getChar();
			PARSELEX('=', LEX_GREAT_EQUAL)
			PARSELEX2('>', LEX_RSHIFT, '=', LEX_RSHIFT_ASSIGNMENT)
			return makeLex(LEX_GREAT);
		}
		PARSELEX('?', LEX_QUESTION)
		PARSELEX('[', LEX_LBRACKET)
		PARSELEX(']', LEX_RBRACKET)
		PARSELEX2('^', LEX_XOR, '=', LEX_XOR_ASSIGNMENT)
		PARSELEX('{', LEX_LBRACE)
		PARSELEX3('|', LEX_OR, '=', LEX_OR_ASSIGNMENT, '|', LEX_LOGICAL_OR)
		PARSELEX('}', LEX_RBRACE)
		PARSELEX('~', LEX_INVERT)
		if (ch == '\"')
			return parseString();
		if (ch == '\'')
			return parseChar();
		if (ch >= '0' && ch <= '9')
			return parseNumber();
		if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
			return parseId();
	}
}

// Получить очередной символ
void Lexer::getChar()
{
	if (ch != -1 && ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t')
		text += ch;
	ch = input.get();		
	if (ch == '\n')
	{
		line++;
		col = 0;
	}
	else if (ch == '\t')
		col += 4;
	else
		col++;	
}

// разобрать многострочный комментарий
void Lexer::parseComment1()
{
	while (ch != -1)
	{
		getChar();
		if (ch == '*')
		{
			getChar();
			if (ch == '/')
				break;
		}
	}
	getChar();
	saveLine = line;
	saveCol = col;
	text.clear();
}

// разобрать однострочный комментарий
void Lexer::parseComment2()
{
	while (ch != -1)
	{
		getChar();
		if (ch == '\n')
			break;
	}
	getChar();
	saveLine = line;
	saveCol = col;
	text.clear();
}

// разобрать число
Lexeme Lexer::parseNumber()
{
	if (ch == '0')
	{
		getChar();
		if (isOct(ch))
			return parseOct();
		if (isDigit(ch))
			return parseInt();
		if (ch == 'x' || ch == 'X')
			return parseHex();
		if (ch == '.')
			return parseFloat();
		if (ch == 'e' || ch == 'E')
			return parseExp();
		return makeLex(LEX_INT_VALUE);
	}
	return parseInt();
}

// разобрать целое число
Lexeme Lexer::parseOct()
{
	while (isOct(ch))
		getChar();
	if (isDigit(ch))
		return parseInt();
	if (ch == '.')
		return parseFloat();
	if (ch == 'e' || ch == 'E')
		return parseExp();
	if (isalpha(ch))
		return parseError();

	// Проверяем число
	__int64 val = 0;
	for (int i = 0; i < text.size(); i++)
		val = val * 8 + (text[i] - '0');
	if (val < -2147483648i64 || val > 2147483647i64)
		return makeLex(LEX_ERROR);

	return makeLex(LEX_INT_VALUE);
}

// разобрать целое число
Lexeme Lexer::parseInt()
{
	while (isDigit(ch))
		getChar();
	if (ch == '.')
		return parseFloat();
	if (ch == 'e' || ch == 'E')
		return parseExp();
	if (isalpha(ch))
		return parseError();

	// Проверяем число
	__int64 val = _atoi64(text.c_str());
	if (val < -2147483648i64 || val > 2147483647i64)
		return makeLex(LEX_ERROR);

	return makeLex(LEX_INT_VALUE);
}

// разобрать целое число
Lexeme Lexer::parseHex()
{
	getChar();
	if (!isHex(ch))
		return parseError();
	while (isHex(ch))
		getChar();
	if (isSymbol(ch))
		return parseError();

	// Проверяем число
	__int64 val = 0;
	for (int i = 2; i < text.size(); i++)
	{
		char c = text[i];
		if (c >= '0' && c <= '9')
			val = val * 16 + (c - '0');
		else if (c >= 'a' && c <= 'f')
			val = val * 16 + (c - 'a' + 10);
		else if (c >= 'A' && c <= 'F')
			val = val * 16 + (c - 'A' + 10);
	}
	if (val < -2147483648i64 || val > 2147483647i64)
		return makeLex(LEX_ERROR);

	return makeLex(LEX_INT_VALUE);
}

// разобрать вещественное с точкой
Lexeme Lexer::parseFloat()
{
	getChar();
	while (isDigit(ch))
		getChar();
	if (ch == 'e' || ch == 'E')
		return parseExp();
	if (isSymbol(ch) || ch == '.')
		return parseError();

	return makeLex(LEX_FLOAT_VALUE);
}

// разобрать вещественное в экспоненциальной форме
Lexeme Lexer::parseExp()
{
	getChar();
	if (ch == '-' || ch == '+')
		getChar();
	if (!isDigit(ch))
		return parseError();
	while (isDigit(ch))
		getChar();
	if (isSymbol(ch) || ch == '.')
		parseError();
	return makeLex(LEX_FLOAT_VALUE);
}

// разобрать строковую константу
Lexeme Lexer::parseString()
{	
	while (ch != -1)
	{
		getChar();
		if (ch == '"')
		{
			getChar();
			return makeLex(LEX_STRING_VALUE);
		}
		if (ch == '\\')
			getChar();
		if (ch == '\n' || ch == '\r')
			return makeLex(LEX_ERROR);
	}
	return makeLex(LEX_ERROR);
}

// разобрать символьную константу
Lexeme Lexer::parseChar()
{
	getChar();
	if (ch == '\n' || ch == '\r')
		return makeLex(LEX_ERROR);
	if (ch == '\\')
		getChar();
	getChar();
	if (ch == '\'')
	{
		getChar();
		return makeLex(LEX_CHAR_VALUE);
	}
	return parseError();
}

// разобрать идентификатор
Lexeme Lexer::parseId()
{
	getChar();
	while (isSymbol(ch) || isDigit(ch))
		getChar();
	char* keywords[] = {"break", "char", "const", "do", "else", "float", "for", "if", "int", "return", "sizeof", "struct", "typedef", "void", "while", };
	for (int i = 0; i < sizeof(keywords)/sizeof(keywords[0]); ++i)
		if (text == keywords[i])
		{
			return makeLex(LEX_ID + i + 1);
			break;
		}
	return makeLex(LEX_ID);
}

// разобрать ошибку
Lexeme Lexer::parseError()
{
	while (isSymbol(ch) || isDigit(ch))
		getChar();
	return makeLex(LEX_ERROR);
}

// Сформировать лексему
Lexeme Lexer::makeLex(int code)
{
	Lexeme l(code, text, saveLine, saveCol);
	saveLine = line;
	saveCol = col;
	text.clear();
	return l;
}

bool Lexer::isOct(char c)
{
	return c >= '0' && c <= '7';
}

bool Lexer::isDigit(char c)
{
	return c >= '0' && c <= '9';
}

bool Lexer::isHex(char c)
{
	return isDigit(c) || (c >= 'a' && c <= 'f') || (c > 'A' && c <= 'F');
}

bool Lexer::isSymbol(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}