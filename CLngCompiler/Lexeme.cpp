#include "stdafx.h"
#include "Lexeme.h"

// �����������
Lexeme::Lexeme()
{
	this->code = -1;
	this->text = "";
	this->line = -1;
	this->col = -1;
}

// �����������
Lexeme::Lexeme(int code, std::string text, int line, int col)
{
	this->code = code;
	this->text = text;
	this->line = line;
	this->col = col;
}

// ����������
Lexeme::~Lexeme()
{
}

// �������� ��� ����� �����
int Lexeme::asInt()
{
	return atoi(text.c_str());
}

// �������� ��� ������������ �����
float Lexeme::asFloat()
{
	return (float)atof(text.c_str());
}

// �������� ������������
Lexeme& Lexeme::operator = (const Lexeme& l)
{
	code = l.code;
	text = l.text;
	line = l.line;
	col = l.col;
	return *this;
}

// �������� ��� �������
bool Lexeme::operator == (int code)
{
	return this->code == code;
}

// �������� ��� �������
bool Lexeme::operator != (int code)
{
	return this->code != code;
}