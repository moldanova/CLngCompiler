#pragma once

#include "Lexeme.h"

// ������������ ����� ������
enum
{
	LEX_UNKNOWN = -1,
	LEX_NOT = 0,			// ! 
	LEX_PERSENT,			// %
	LEX_AND,				// &
	LEX_LPAREN,				// (
	LEX_RPAREN,				// )
	LEX_MUL,				// *
	LEX_ADD,				// +
	LEX_COMMA,				// ,
	LEX_SUB,				// -
	LEX_DOT,				// .
	LEX_DIV,				// /
	LEX_COLON,				// :
	LEX_SEMICOLON,			// ;
	LEX_LESS,				// <
	LEX_ASSIGNMENT,			// =
	LEX_GREAT,				// >
	LEX_QUESTION,			// ?
	LEX_LBRACKET,			// [
	LEX_RBRACKET,			// ]
	LEX_XOR,				// ^
	LEX_LBRACE,				// {
	LEX_OR,					// |
	LEX_RBRACE,				// }
	LEX_INVERT,				// ~
	LEX_NOT_EQUAL,			// !=
	LEX_PERSENT_ASSIGNMENT,	// %=
	LEX_LOGICAL_AND,		// &&
	LEX_AND_ASSIGNMENT,		// &=
	LEX_MUL_ASSIGNMENT,		// *=
	LEX_INCREMENT,			// ++
	LEX_ADD_ASSIGNMENT,		// +=
	LEX_DECREMENT,			// --
	LEX_SUB_ASSIGNMENT,		// -=
	LEX_ARROW,				// ->
	LEX_DIV_ASSIGNMENT,		// /=
	LEX_LSHIFT,				// <<
	LEX_LESS_EQUAL,			// <=
	LEX_LSHIFT_ASSIGNMENT,	// <<=
	LEX_EQUAL,				// ==
	LEX_GREAT_EQUAL,		// >=
	LEX_RSHIFT,				// >>
	LEX_RSHIFT_ASSIGNMENT,	// >>=
	LEX_XOR_ASSIGNMENT,		// ^=
	LEX_OR_ASSIGNMENT,		// |=
	LEX_LOGICAL_OR,			// ||
	LEX_ID,					// �������������
	LEX_BREAK,				// break
	LEX_CHAR,				// char
	LEX_CONST,				// const
	LEX_DO,					// do
	LEX_ELSE,				// else
	LEX_FLOAT,				// float
	LEX_FOR,				// for
	LEX_IF,					// if
	LEX_INT,				// int
	LEX_RETURN,				// return
	LEX_SIZEOF,				// sizeof
	LEX_STRUCT,				// struct
	LEX_TYPEDEF,			// typedef
	LEX_VOID,				// void
	LEX_WHILE,				// while	
	LEX_INT_VALUE,			// ������������� ��������
	LEX_FLOAT_VALUE,		// ������������ ��������
	LEX_STRING_VALUE,		// ��������� ���������
	LEX_CHAR_VALUE,			// ���������� ���������
	LEX_EOF,				// ����� �����
	LEX_ERROR				// ������
};

// ����� ������������ �����������
class Lexer
{
public:
	// �����������
	Lexer(const char* fileName);
	// ����������
	~Lexer();
	// �������� ��������� �������
	Lexeme next();
private:
	// �������� ��������� ������
	void getChar();
	// ��������� ������������� �����������
	void parseComment1();
	// ��������� ������������ �����������
	void parseComment2();
	// ��������� �����
	Lexeme parseNumber();
	// ��������� ����� �����
	Lexeme parseOct();	
	// ��������� ����� �����
	Lexeme parseInt();
	// ��������� ����� �����
	Lexeme parseHex();
	// ��������� ������������ � ������
	Lexeme parseFloat();
	// ��������� ������������ � ���������������� �����
	Lexeme parseExp();
	// ��������� ��������� ���������
	Lexeme parseString();
	// ��������� ���������� ���������
	Lexeme parseChar();
	// ��������� �������������
	Lexeme parseId();
	// ��������� ������
	Lexeme parseError();
	// ������������ �������
	Lexeme makeLex(int code);
	bool isOct(char c);
	bool isDigit(char c);
	bool isHex(char c);
	bool isSymbol(char c);

	std::ifstream input;	// ������� ��������� ����
	char ch;				// ������������� ������
	std::string text;		// ����� �������
	int line;				// ������ ������
	int col;				// ������ � ������	
	int saveLine;
	int saveCol;
};

