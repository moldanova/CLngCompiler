#include "stdafx.h"
#include "ConstExpressionCalc.h"


// �����������
ConstExpressionCalc::ConstExpressionCalc()
{
}

// ����������
ConstExpressionCalc::~ConstExpressionCalc()
{
}

// ���������
bool ConstExpressionCalc::calculate(Node* node)
{
	return true;
}

// ������
std::string ConstExpressionCalc::errorMsg()
{
	return "";
}

// ��� ����������
TypeSymbol* ConstExpressionCalc::getType()
{
	return new TypeSymbol("int", 4);
}

// �������� ���������
char ConstExpressionCalc::asChar()
{
	return 10;
}

// �������� ���������
int ConstExpressionCalc::asInt()
{
	return 10;
}

// �������� ���������
float ConstExpressionCalc::asFloat()
{
	return 10;
}

// �������� ���������
void* ConstExpressionCalc::asPtr()
{
	return NULL;
}
