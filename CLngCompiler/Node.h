#pragma once

#include "Lexeme.h"

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
};

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
private:
	std::vector<Node*> nodes;	// ������ ���������
};

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
	// ���������
	Node* node;
	// ��������
	Lexeme op;
	// ��������
	bool postfix;
};

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
	// ����� ����
	Node* left;
	// ������ ����
	Node* right;
	// ��������
	Lexeme op;
};

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
	// ����� ����
	Node* left;
	// ������ ����
	Node* first;
	// ������ ����
	Node* second;
};

// ����� ���������
class ConstantValue : public Node
{
public:
	// �����������
	ConstantValue(Lexeme value);
	// ����������
	virtual ~ConstantValue();
	// ������� ���� �� �����
	virtual void print();
	// ��� ����������
	Lexeme value;
};

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
	// �������������
	Lexeme name;
};

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
	// ������
	Node* arr;
	// ������ �������
	Node* idx;
};

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
	void addArgument(Node* node);
	// ������ �������
	Node* func;
private:
	std::vector<Node*> nodes;	// ������ ���������
};

// ����� ���������
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
	void addNode(Node* node);
private:
	// ������ ����� ���������
	std::vector<Node*> nodes;
};

