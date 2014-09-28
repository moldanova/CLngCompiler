#include "stdafx.h"
#include "Node.h"
#include "Lexer.h"

// Конструктор
Node::Node()
{
}

// Деструктор
Node::~Node(void)
{
}

// Вывести узел на экран
void Node::print()
{
}

// Конструктор
ExpressionNode::ExpressionNode(Node* node)
{
	addAssignment(node);
}

// Деструктор
ExpressionNode::~ExpressionNode()
{
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	nodes.clear();
}

// Вывести узел на экран
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

// Добавить узел
void ExpressionNode::addAssignment(Node* node)
{
	nodes.push_back(node);
}

// Конструктор
UnaryOpNode::UnaryOpNode(Node* node, Lexeme op, bool postfix)
{
	this->node = node;
	this->op = op;
	this->postfix = postfix;
}

// Деструктор
UnaryOpNode::~UnaryOpNode()
{
	delete node;
}

// Вывести узел на экран
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

// Конструктор
BinaryOpNode::BinaryOpNode(Node* left, Lexeme op, Node* right)
{
	this->left = left;
	this->op = op;
	this->right = right;
}

// Деструктор
BinaryOpNode::~BinaryOpNode()
{
	delete left;
	delete right;
}

// Вывести узел на экран
void BinaryOpNode::print()
{
	std::cout << "(";
	left->print();
	std::cout << op.text;
	right->print();
	std::cout << ")";
}

// Конструктор
ConditionalNode::ConditionalNode(Node* left, Node* first, Node* second)
{
	this->left = left;
	this->first = first;
	this->second = second;
}

// Деструктор
ConditionalNode::~ConditionalNode()
{
	delete left;
	delete first;
	delete second;
}

// Вывести узел на экран
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

// Конструктор
ConstantValue::ConstantValue(Lexeme value)
{
	this->value = value;
}

// Деструктор
ConstantValue::~ConstantValue()
{
}

// Вывести узел на экран
void ConstantValue::print()
{
	std::cout << value.text;
}

// Конструктор
IdentifierNode::IdentifierNode(Lexeme name)
{
	this->name = name;
}
	
// Деструктор
IdentifierNode::~IdentifierNode()
{
}
	
// Вывести узел на экран
void IdentifierNode::print()
{
	std::cout << name.text;
}

// Конструктор
ArrayNode::ArrayNode(Node* arr, Node* idx)
{
	this->arr = arr;
	this->idx = idx;
}

// Деструктор
ArrayNode::~ArrayNode()
{
	delete arr;
	delete idx;
}

// Вывести узел на экран
void ArrayNode::print()
{
	arr->print();
	std::cout << "[";
	idx->print();
	std::cout << "]";
}

// Конструктор
FuncCallNode::FuncCallNode(Node* func)
{
	this->func = func;
}

// Деструктор
FuncCallNode::~FuncCallNode()
{
	delete func;
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	nodes.clear();
}

// Вывести узел на экран
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

// Добавить узел
void FuncCallNode::addArgument(Node* node)
{
	nodes.push_back(node);
}

// Конструктор
ProgramNode::ProgramNode()
{
}

// Деструктор
ProgramNode::~ProgramNode()
{
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	nodes.clear();
}

// Вывести узел на экран
void ProgramNode::print()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i]->print();
		std::cout << std::endl;
	}
}

// Добавить узел
void ProgramNode::addNode(Node* node)
{
	nodes.push_back(node);
}