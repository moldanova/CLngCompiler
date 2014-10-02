#pragma once

#include "Lexer.h"
#include "Symbols.h"
#include "Nodes.h"


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
private:	
	// ��������� ������� ����������
	void parseUnit(ProgramNode* prog);
	// ��������� ���
	TypeSymbol* parseType();
	// ��������� �����������
	Node* parseDeclarator(TypeSymbol* type);
	// ��������� ���������
	TypeSymbol* parsePointer(TypeSymbol* type);
	// ��������� ������������
	Node* parseAssignmentExpression();
	// ��������� �������� ���������
	Node* parseConditionalExpression(Node* left);
	// ��������� ��������
	Node* Parser::parseOperation(Node* left, int priority);
	// ��������� ���������� ����
	Node* parseCastExpression();
	// ��������� ������� ��������
	Node* parseUnaryExpression();
	// ��������� ����������� ��������
	Node* parsePostfixExpression();
	// ��������� �������
	Node* parsePrimaryExpression();	
	
	
	// ��������� ���������
	TypeSymbol* parseStruct();	
	

	// �������� ��������� �������
	void next();
	// ������� ������� �����
	void back();
	// ��������� � �������������� ������
	void check(bool cond, const char* msg, Lexeme l);
	// ���������, ��� ������� �������� ������� ���������
	bool isUnaryOperator();
	// ��������� ��� ������� �������� �������������
	bool isAssignmentOperator();
	// ���������, ��� ������� �������� ������ ����
	bool isTypeName();


	// ��������� ������
	void checkSymbol(std::string name, bool forceAll = true);
	// ����� ���
	TypeSymbol* getTypeSymbol(std::string name);
	// �������� ���
	TypeSymbol* addTypeSymbol(std::string name, int length);
	// �������� ���
	TypeSymbol* addTypeSymbol(TypeSymbol* baseType, int mode);
	// �������� ���������
	AliasSymbol* addAliasSymbol(TypeSymbol* baseType, std::string name);
	// �������� ������
	ArraySymbol* addArraySymbol(TypeSymbol* baseType, int count);
	// �������� ��������� ��� �������
	Symbol* getSymbol(std::string name);
	// �������� ����������
	VariableSymbol* addVariableSymbol(std::string name, TypeSymbol* type);
	// �������� �������
	FunctionSymbol* addFunctionSymbol(std::string name, TypeSymbol* type);

	Lexer& lexer;	// ����������� ����������
	Lexeme lex;		// ������������� �������
	std::vector<Lexeme> buffer;		// ����� ������������ ������
	std::vector<Lexeme> backStack;		// ���� �������� ������
	std::vector<SymbolsTable*> tables;	// ���� ������ ��������
};

