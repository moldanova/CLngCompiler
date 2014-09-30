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
	Node* parse();
	// ��������� ���������
	Node* parseExpression();
	// ������� ������� ������
	bool hasErrors();
private:	
	// ��������� ������� ����������
	Node* parseUnit();
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
	// ��������� ���
	TypeSymbol* parseType();
	// ��������� ���������
	TypeSymbol* parsePointer(TypeSymbol* type);
	// ��������� ���������
	TypeSymbol* parseStruct();	
	// ��������� �����������
	ItemSymbol* parseDeclarator(TypeSymbol* type);

	// �������� ��������� �������
	void next();
	// ������� ������� �����
	void back();
	// ���������, ��� ������� �������� ������� ���������
	bool isUnaryOperator();
	// ��������� ��� ������� �������� �������������
	bool isAssignmentOperator();
	// ���������, ��� ������� �������� ������ ����
	bool isTypeName();
	// ����� �������
	Symbol* findSymbolByName(std::string name, bool forceAll = true);
	// �����  ������� ��� ��������
	Symbol* findOrAddSymbol(Symbol* symbol, bool forceAll = true);
	// �������� ������
	void addSymbol(Symbol* symbol);

	// ��������� � �������������� ������
	void OnSyntaxError(const char* msg);

	Lexer& lexer;	// ����������� ����������
	Lexeme lex;		// ������������� �������
	std::vector<Lexeme> buffer;		// ����� ������������ ������
	std::vector<Lexeme> backStack;		// ���� �������� ������
	std::vector<SymbolsTable*> tables;	// ���� ������ ��������
};

