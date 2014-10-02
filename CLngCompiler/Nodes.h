#pragma once

#include "Lexeme.h"
#include "Symbols.h"

class INodeVisitor;

//-------------------------------------------------------------------------

// ����� ���� ��������������� ������
class Node
{
public:
	// �����������
	Node();
	// �����������
	Node(Symbol* symbol);
	// �����������
	Node(Lexeme lex, Symbol* symbol);
	// ����������
	virtual ~Node();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// �������� ��� ��������
	TypeSymbol* getType();
	// ������� ����
	Lexeme lex;
	// ��� ����
	Symbol* symbol;
};

//-------------------------------------------------------------------------

// ����� ���� ���������
class ProgramNode : public Node
{
public:
	// �����������
	ProgramNode();
	// ����������
	virtual ~ProgramNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// �������� ����
	void addNode(Node* node);
	// ������ ����� ���������
	std::vector<Node*> nodes;
	// ������� ���������� ��������
	SymbolsTable global;
};

//-------------------------------------------------------------------------

// ����� ���������
class ExpressionNode : public Node
{
public:
	// �����������
	ExpressionNode(Node* node);
	// ����������
	virtual ~ExpressionNode();
	// �������� ����
	void addAssignment(Node* node);
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ������ ���������
	std::vector<Node*> nodes;	
};

//-------------------------------------------------------------------------

// �������� ���������
class ConditionalNode : public Node
{
public:
	// �����������
	ConditionalNode(Node* left, Node* first, Node* second);
	// ����������
	virtual ~ConditionalNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ����� ����
	Node* left;
	// ������ ����
	Node* first;
	// ������ ����
	Node* second;
};

//-------------------------------------------------------------------------

// ����� �������� ��������
class BinaryOpNode : public Node
{
public:
	// ��������� ��� ����������
	static TypeSymbol* makeType(Lexeme lex, Node* left, Node* right);
	// �����������
	BinaryOpNode(Lexeme lex, Symbol* symbol, Node* left, Node* right);
	// ����������
	virtual ~BinaryOpNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ����� ����
	Node* left;
	// ������ ����
	Node* right;	
};

//-------------------------------------------------------------------------

// ����� ������� ��������
class UnaryOpNode : public Node
{
public:
	// ��������� ��� ����������
	static TypeSymbol* makeType(Lexeme lex, Node* right, bool postfix);
	// �����������
	UnaryOpNode(Lexeme lex, Symbol* symbol,  Node* right, bool postfix);
	// ����������
	virtual ~UnaryOpNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ���������
	Node* right;	
	// ��������
	bool postfix;
};

//-------------------------------------------------------------------------

// ����� ���������
class ValueNode : public Node
{
public:
	// �����������
	ValueNode(Lexeme lex, Symbol* symbol);
	// ����������
	virtual ~ValueNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
};

//-------------------------------------------------------------------------

// ����� ��������������
class IdentifierNode : public Node
{
public:
	// �����������
	IdentifierNode(Lexeme lex, Symbol* symbol);
	// ����������
	virtual ~IdentifierNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
};

//-------------------------------------------------------------------------

// ����� ��������� � �������� �������
class ArrayNode : public Node
{
public:
	// �����������
	ArrayNode(Symbol* symbol, Node* var, Node* idx);
	// ����������
	virtual ~ArrayNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ������
	Node* var;
	// ������ �������
	Node* idx;
};

//-------------------------------------------------------------------------

// ����� ������ �������
class FuncCallNode : public Node
{
public:
	// �����������
	FuncCallNode(Symbol* symbol, Node* func);
	// ����������
	virtual ~FuncCallNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// �������� ����
	void addArgument(Node* node);
	// ������ �������
	Node* func;
	// ������ ���������
	std::vector<Node*> nodes;	
};

//-------------------------------------------------------------------------

// ��������� ���������� ��� ����� ������
class INodeVisitor
{
public:
	// �������� ����
	virtual void OnNode(Node* node) = 0;
	// �������� ���� ���������
	virtual void OnNode(ProgramNode* node) = 0;
	// �������� ���� ���������
	virtual void OnNode(ExpressionNode* node) = 0;
	// �������� ���� ��������� ����������
	virtual void OnNode(ConditionalNode* node) = 0;
	// �������� ���� �������� ��������
	virtual void OnNode(BinaryOpNode* node) = 0;
	// �������� ���� ������� ��������
	virtual void OnNode(UnaryOpNode* node) = 0;	
	// �������� ���� ��������
	virtual void OnNode(ValueNode* node) = 0;
	// �������� ���� ��������������
	virtual void OnNode(IdentifierNode* node) = 0;
	// �������� ���� ��������� � �������
	virtual void OnNode(ArrayNode* node) = 0;
	// �������� ���� ������ �������
	virtual void OnNode(FuncCallNode* node) = 0;	
};
