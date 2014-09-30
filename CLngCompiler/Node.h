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
	// ����������
	virtual ~Node();
	// ������� ���� �� �����
	virtual void print();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
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
	// ������� ���� �� �����
	virtual void print();
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
	// ������� ���� �� �����
	virtual void print();
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
	// ������� ���� �� �����
	virtual void print();
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
	// �����������
	BinaryOpNode(Node* left, Lexeme op, Node* right);
	// ����������
	virtual ~BinaryOpNode();
	// ������� ���� �� �����
	virtual void print();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ����� ����
	Node* left;
	// ������ ����
	Node* right;
	// ��������
	Lexeme op;
};

//-------------------------------------------------------------------------

// ����� ������� ��������
class UnaryOpNode : public Node
{
public:
	// �����������
	UnaryOpNode(Node* node, Lexeme op, bool postfix);
	// ����������
	virtual ~UnaryOpNode();
	// ������� ���� �� �����
	virtual void print();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ���������
	Node* node;
	// ��������
	Lexeme op;
	// ��������
	bool postfix;
};

//-------------------------------------------------------------------------

// ����� ���������
class ValueNode : public Node
{
public:
	// �����������
	ValueNode(Lexeme value);
	// ����������
	virtual ~ValueNode();
	// ������� ���� �� �����
	virtual void print();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ��� ����������
	Lexeme value;
};

//-------------------------------------------------------------------------

// ����� ��������������
class IdentifierNode : public Node
{
public:
	// �����������
	IdentifierNode(Lexeme name);
	// ����������
	virtual ~IdentifierNode();
	// ������� ���� �� �����
	virtual void print();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// �������������
	Lexeme name;
};

//-------------------------------------------------------------------------

// ����� ��������� � �������� �������
class ArrayNode : public Node
{
public:
	// �����������
	ArrayNode(Node* arr, Node* idx);
	// ����������
	virtual ~ArrayNode();
	// ������� ���� �� �����
	virtual void print();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ������
	Node* arr;
	// ������ �������
	Node* idx;
};

//-------------------------------------------------------------------------

// ����� ������ �������
class FuncCallNode : public Node
{
public:
	// �����������
	FuncCallNode(Node* func);
	// ����������
	virtual ~FuncCallNode();
	// ������� ���� �� �����
	virtual void print();
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
