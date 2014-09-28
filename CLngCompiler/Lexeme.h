#pragma once

// �������� ���������� �������
class Lexeme
{
public:
	// �����������
	Lexeme();
	// �����������
	Lexeme(int code, std::string text, int line, int col);
	// ����������
	~Lexeme();
	// �������� ��� ����� �����
	int asInt();
	// �������� ��� ������������ �����
	float asFloat();
	// �������� ������������
	Lexeme& operator = (const Lexeme& l);
	// �������� ��� �������
	bool operator == (int code);
	// �������� ��� �������
	bool operator != (int code);

	int code;			// ��� �������
	std::string text;	// ����� �������
	int line;			// ����� ������
	int col;			// ����� ������� � ������
};

