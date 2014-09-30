#pragma once

#include "Node.h"
#include "Symbols.h"

// ����� ��� ���������� ����������� ���������
class ConstExpressionCalc
{
public:
	// �����������
	ConstExpressionCalc();
	// ����������
	virtual ~ConstExpressionCalc();
	// ���������
	bool calculate(Node* node);
	// ������
	std::string errorMsg();
	// ��� ����������
	TypeSymbol* getType();
	// �������� ���������
	char asChar();
	// �������� ���������
	int asInt();
	// �������� ���������
	float asFloat();
	// �������� ���������
	void* asPtr();
};

