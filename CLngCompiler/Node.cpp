#include "stdafx.h"
#include "Node.h"
#include "Lexer.h"

//---------------------------------------------------------------------------

// �����������
Node::Node()
{
}

// ����������
Node::~Node(void)
{
}

// ������� ���� �� �����
void Node::print()
{
}

// �������� ����
void Node::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//-------------------------------------------------------------------------

// �����������
ProgramNode::ProgramNode()
{
	global.addSymbol(new TypeSymbol("void", 0));
	global.addSymbol(new TypeSymbol("char", 1));
	global.addSymbol(new TypeSymbol("int", 4));
	global.addSymbol(new TypeSymbol("float", 4));
}

// ����������
ProgramNode::~ProgramNode()
{
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	nodes.clear();
}

// ������� ���� �� �����
void ProgramNode::print()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i]->print();
		std::cout << std::endl;
	}
}

// �������� ����
void ProgramNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

// �������� ����
void ProgramNode::addNode(Node* node)
{
	nodes.push_back(node);
}

//---------------------------------------------------------------------------

// �����������
ExpressionNode::ExpressionNode(Node* node)
{
	addAssignment(node);
}

// ����������
ExpressionNode::~ExpressionNode()
{
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	nodes.clear();
}

// ������� ���� �� �����
void ExpressionNode::print()
{
	if (!nodes.empty())
	{
		nodes[0]->print();
		for (int i = 1; i < nodes.size(); i++)
		{
			std::cout << ", ";
			nodes[i]->print();
		}
	}
}

// �������� ����
void ExpressionNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

// �������� ����
void ExpressionNode::addAssignment(Node* node)
{
	nodes.push_back(node);
}

//---------------------------------------------------------------------------

// �����������
ConditionalNode::ConditionalNode(Node* left, Node* first, Node* second)
{
	this->left = left;
	this->first = first;
	this->second = second;
}

// ����������
ConditionalNode::~ConditionalNode()
{
	delete left;
	delete first;
	delete second;
}

// ������� ���� �� �����
void ConditionalNode::print()
{
	std::cout << "(";
	left->print();
	std::cout << "?";
	first->print();
	std::cout << ":";
	second->print();
	std::cout << ")";
}

// �������� ����
void ConditionalNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// �����������
BinaryOpNode::BinaryOpNode(Node* left, Lexeme op, Node* right)
{
	this->left = left;
	this->op = op;
	this->right = right;
}

// ����������
BinaryOpNode::~BinaryOpNode()
{
	delete left;
	delete right;
}

// ������� ���� �� �����
void BinaryOpNode::print()
{
	std::cout << "(";
	left->print();
	std::cout << op.text;
	right->print();
	std::cout << ")";
}

// �������� ����
void BinaryOpNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// �����������
UnaryOpNode::UnaryOpNode(Node* node, Lexeme op, bool postfix)
{
	this->node = node;
	this->op = op;
	this->postfix = postfix;
}

// ����������
UnaryOpNode::~UnaryOpNode()
{
	delete node;
}

// ������� ���� �� �����
void UnaryOpNode::print()
{
	if (op == LEX_SIZEOF)
	{
		std::cout << "sizeof(";
		node->print();
		std::cout << ")";
	}
	else if (postfix)
	{
		std::cout << "(";
		node->print();
		std::cout << op.text << ")";
	}
	else
	{
		std::cout << "(" << op.text;
		node->print();
		std::cout << ")";
	}
}

// �������� ����
void UnaryOpNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// �����������
ValueNode::ValueNode(Lexeme value)
{
	this->value = value;
}

// ����������
ValueNode::~ValueNode()
{
}

// ������� ���� �� �����
void ValueNode::print()
{
	std::cout << value.text;
}

// �������� ����
void ValueNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// �����������
IdentifierNode::IdentifierNode(Lexeme name)
{
	this->name = name;
}
	
// ����������
IdentifierNode::~IdentifierNode()
{
}
	
// ������� ���� �� �����
void IdentifierNode::print()
{
	std::cout << name.text;
}

// �������� ����
void IdentifierNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// �����������
ArrayNode::ArrayNode(Node* arr, Node* idx)
{
	this->arr = arr;
	this->idx = idx;
}

// ����������
ArrayNode::~ArrayNode()
{
	delete arr;
	delete idx;
}

// ������� ���� �� �����
void ArrayNode::print()
{
	arr->print();
	std::cout << "[";
	idx->print();
	std::cout << "]";
}

// �������� ����
void ArrayNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// �����������
FuncCallNode::FuncCallNode(Node* func)
{
	this->func = func;
}

// ����������
FuncCallNode::~FuncCallNode()
{
	delete func;
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	nodes.clear();
}

// ������� ���� �� �����
void FuncCallNode::print()
{
	func->print();
	std::cout << "(";
	if (!nodes.empty())
	{
		nodes[0]->print();
		for (int i = 1; i < nodes.size(); i++)
		{
			std::cout << ", ";
			nodes[i]->print();
		}
	}
	std::cout << ")";
}

// �������� ����
void FuncCallNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

// �������� ����
void FuncCallNode::addArgument(Node* node)
{
	nodes.push_back(node);
}
