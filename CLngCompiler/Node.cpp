#include "stdafx.h"
#include "Node.h"
#include "Lexer.h"

//---------------------------------------------------------------------------

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

// Посетить узел
void Node::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//-------------------------------------------------------------------------

// Конструктор
ProgramNode::ProgramNode()
{
	global.addSymbol(new TypeSymbol("void", 0));
	global.addSymbol(new TypeSymbol("char", 1));
	global.addSymbol(new TypeSymbol("int", 4));
	global.addSymbol(new TypeSymbol("float", 4));
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

// Посетить узел
void ProgramNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

// Добавить узел
void ProgramNode::addNode(Node* node)
{
	nodes.push_back(node);
}

//---------------------------------------------------------------------------

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

// Посетить узел
void ExpressionNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

// Добавить узел
void ExpressionNode::addAssignment(Node* node)
{
	nodes.push_back(node);
}

//---------------------------------------------------------------------------

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

// Посетить узел
void ConditionalNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

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

// Посетить узел
void BinaryOpNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

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

// Посетить узел
void UnaryOpNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
ValueNode::ValueNode(Lexeme value)
{
	this->value = value;
}

// Деструктор
ValueNode::~ValueNode()
{
}

// Вывести узел на экран
void ValueNode::print()
{
	std::cout << value.text;
}

// Посетить узел
void ValueNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

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

// Посетить узел
void IdentifierNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

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

// Посетить узел
void ArrayNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

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

// Посетить узел
void FuncCallNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

// Добавить узел
void FuncCallNode::addArgument(Node* node)
{
	nodes.push_back(node);
}
