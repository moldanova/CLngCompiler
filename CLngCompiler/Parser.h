#pragma once

#include "Lexer.h"
#include "Node.h"
#include "Symbols.h"

// ����� ��������������� �����������
class Parser
{
public:
	// �����������
	Parser(Lexer& lexer);
	// ����������
	~Parser();
	// ��������� ������
	void parse();
	// ��������� ���������
	Node* parseExpression();
	// ������� ������� ������
	bool hasErrors();
	// �������� ����������� ���������
	ProgramNode& getProgram() { return prog; }
	// �������� ������� ��������
	SymbolsTable& getSymbols() { return symbols; }
private:	
	// ��������� ������������
	Node* parseAssignmentExpression();
	// ��������� �������� ���������
	Node* parseConditionalExpression(Node* left);
	// ��������� ���������� ���
	Node* parseLogicalOrExpression(Node* left);
	// ��������� ���������� �
	Node* parseLogicalAndExpression(Node* left);
	// ��������� ��������� ���
	Node* parseInclusiveOrExpression(Node* left);
	// ��������� ��������� ����������� ���
	Node* parseExclusiveOrExpression(Node* left);
	// ��������� ��������� �
	Node* parseAndExpression(Node* left);
	// ��������� ���������
	Node* parseEqualityExpression(Node* left);
	// ��������� ���������
	Node* parseRelationalExpression(Node* left);
	// ��������� �����
	Node* parseShiftExpression(Node* left);
	// �������� ��������
	Node* parseAdditiveExpression(Node* left);
	// ��������� ���������
	Node* parseMultiplicativeExpression(Node* left);
	// ��������� ���������� ����
	Node* parseCastExpression();
	// ��������� ������� ��������
	Node* parseUnaryExpression();
	// ��������� ����������� ��������
	Node* parsePostfixExpression();
	// ��������� �������
	Node* parsePrimaryExpression();
	// ��������� �������
	Node* parseFunction();
	// ��������� ���
	TypeSymbol* parseType();
	// ��������� ���������
	TypeSymbol* parseStruct();	
	// ��������� ����������
	VariableSymbol* parseVariable(TypeSymbol* type);
	

	// ���������, ��� ������� �������� ������� ���������
	bool isUnaryOperator();
	// ��������� ��� ������� �������� �������������
	bool isAssignmentOperator();
	// ���������, ��� ������� �������� ������ ����
	bool isTypeName();

	// �������� ��������� �������
	void next();
	// ������� ������� �����
	void back();
	// �������� �����
	void commit();

	// ��������� � �������������� ������
	void OnSyntaxError(const char* msg);

	Lexer& lexer;	// ����������� ����������
	Lexeme lex;		// ������������� �������
	std::vector<Lexeme> buffer;		// ����� ������������ ������
	std::vector<Lexeme> stack;		// ���� �������� ������
	ProgramNode prog;
	SymbolsTable symbols;			// ������� ��������
};

