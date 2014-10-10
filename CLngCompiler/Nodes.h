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
	// ������� ������� ����
	virtual bool isEmpty() { return false; }
	// ������� ����
	Lexeme lex;
	// ��� ����
	Symbol* symbol;
};

//-------------------------------------------------------------------------

// ����� ����� ������� ����� ��������� ������ ��������� �����
class NodesArrayNode : public Node
{
public:
	// �����������
	NodesArrayNode();
	// �����������
	NodesArrayNode(Symbol* symbol);
	// �����������
	NodesArrayNode(Lexeme lex, Symbol* symbol);
	// ����������
	virtual ~NodesArrayNode();
	// �������� ����
	void addNode(Node* node);
	// ������ ����� ���������
	std::vector<Node*> nodes;
};

//-------------------------------------------------------------------------

// ����� ������� ����. ���� ����� �����, ����� ������� ��� ��������
class EmptyNode : public Node
{
public:
	// �����������
	EmptyNode(Symbol* symbol) : Node(symbol) {}
	// ������� ������� ����
	virtual bool isEmpty() { return true; }
};

//-------------------------------------------------------------------------

// ����� ���� ���������
class ProgramNode : public NodesArrayNode
{
public:
	// �����������
	ProgramNode();
	// ����������
	virtual ~ProgramNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ������� ���������� ��������
	SymbolsTable globals;
};

//-------------------------------------------------------------------------

// ����� ���������
class ExpressionNode : public NodesArrayNode
{
public:
	// �����������
	ExpressionNode(Node* node);
	// ����������
	virtual ~ExpressionNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
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
class FuncCallNode : public NodesArrayNode
{
public:
	// �����������
	FuncCallNode(Symbol* symbol, Node* func);
	// ����������
	virtual ~FuncCallNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ���� �������
	Node* func;
};

//-------------------------------------------------------------------------

// ������ ����������
class CompoundNode : public NodesArrayNode
{
public:
	// �����������
	CompoundNode();
	// ����������
	virtual ~CompoundNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ������ �����������
	SymbolsTable locals;
};

//-------------------------------------------------------------------------

// ����� �������
class FunctionNode : public Node
{
public:
	// �����������
	FunctionNode(Symbol* symbol);
	// ����������
	virtual ~FunctionNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ������ ����������
	SymbolsTable params;
	// ���� �������
	Node* statement;
};

//-------------------------------------------------------------------------

// ����� ��������� ���������
class IfNode : public Node
{
public:
	// �����������
	IfNode(Node* expr, Node* statement1, Node* statement2);
	// ����������
	virtual ~IfNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// �������� ��������
	Node* expr;
	// ���������
	Node* statement1;
	Node* statement2;
};

//-------------------------------------------------------------------------

// ����� �����
class WhileNode : public Node
{
public:
	// �����������
	WhileNode(Node* expr, Node* statement);
	// ����������
	virtual ~WhileNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// �������� ��������
	Node* expr;
	// ���������
	Node* statement;
};

//-------------------------------------------------------------------------

// ����� �����
class DoNode : public Node
{
public:
	// �����������
	DoNode(Node* expr, Node* statement);
	// ����������
	virtual ~DoNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// �������� ��������
	Node* expr;
	// ���������
	Node* statement;
};

//-------------------------------------------------------------------------

// ����� �����
class ForNode : public Node
{
public:
	// �����������
	ForNode(Node* expr1, Node* expr2, Node* expr3, Node* statement);
	// ����������
	virtual ~ForNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// �������� ��������
	Node* expr1;
	Node* expr2;
	Node* expr3;
	// ���������
	Node* statement;
};

//-------------------------------------------------------------------------

class BreakNode : public Node
{
public:
	// �����������
	BreakNode();
	// ����������
	virtual ~BreakNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
};

//-------------------------------------------------------------------------

class ContinueNode : public Node
{
public:
	// �����������
	ContinueNode();
	// ����������
	virtual ~ContinueNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
};

//-------------------------------------------------------------------------

class ReturnNode : public Node
{
public:
	// �����������
	ReturnNode(Node* expr);
	// ����������
	virtual ~ReturnNode();
	// �������� ����
	virtual void visit(INodeVisitor* visitor);
	// ������������ ��������
	Node* expr;
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
	// �������� ����
	virtual void OnNode(CompoundNode* node) = 0;
	// �������� ���� �������
	virtual void OnNode(FunctionNode* node) = 0;
	// �������� ����
	virtual void OnNode(IfNode* node) = 0;
	// �������� ����
	virtual void OnNode(WhileNode* node) = 0;
	// �������� ����
	virtual void OnNode(DoNode* node) = 0;
	// �������� ����
	virtual void OnNode(ForNode* node) = 0;
	// �������� ����
	virtual void OnNode(BreakNode* node) = 0;
	// �������� ����
	virtual void OnNode(ContinueNode* node) = 0;
	// �������� ����
	virtual void OnNode(ReturnNode* node) = 0;
};
