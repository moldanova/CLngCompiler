#include "stdafx.h"
#include "Nodes.h"
#include "Lexer.h"
#include "Exceptions.h"

//---------------------------------------------------------------------------

// �����������
Node::Node()
{
	this->symbol = NULL;
}

// �����������
Node::Node(Symbol* symbol)
{
	this->symbol = symbol;
}

// �����������
Node::Node(Lexeme lex, Symbol* symbol)
{
	this->lex = lex;
	this->symbol = symbol;
}

// ����������
Node::~Node()
{
}

// �������� ��� ����
TypeSymbol* Node::getType()
{
	TypeSymbol* type = dynamic_cast<TypeSymbol*>(symbol);
	ItemSymbol* item = dynamic_cast<ItemSymbol*>(symbol);

	if (type)
		return type;
	else
		return item->type;
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
/*
void ProgramNode::print()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i]->print();
		std::cout << std::endl;
	}
}
*/

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
/*
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
*/

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
/*
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
*/

// �������� ����
void ConditionalNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// ��������� ��� ����������
TypeSymbol* BinaryOpNode::makeType(Lexeme lex, Node* left, Node* right)
{
	TypeSymbol* ltype = left->getType();
	TypeSymbol* rtype = right->getType();
	if (lex == LEX_ASSIGNMENT)
	{
		if (!left->symbol->isModificableLvalue())
			throw SyntaxException("left operand must be modificable lvalue", lex);

		// ����� ������������ ������������ ���������� ��������� �� ������
		if (left->getType()->name == "char *" && right->getType()->name == "const char *"
			&& dynamic_cast<ValueNode*>(right) != NULL)
		{
			return ltype;
		}

		if (!right->getType()->canConvertTo(left->getType()))
			throw SyntaxException("type mismath", lex);
		return ltype;
	}
	if (lex == LEX_MUL_ASSIGNMENT || lex == LEX_DIV_ASSIGNMENT)
	{
		if (!left->symbol->isModificableLvalue())
			throw SyntaxException("left operand must be modificable lvalue", lex);
		if (!ltype->isScalar() || !rtype->isScalar())
			throw SyntaxException("operands must be scalar type", lex);
		if (!right->getType()->canConvertTo(left->getType()))
			throw SyntaxException("type mismath", lex);
		return ltype;
		
	}
	if (lex == LEX_PERSENT_ASSIGNMENT || lex == LEX_LSHIFT_ASSIGNMENT || lex == LEX_RSHIFT_ASSIGNMENT
		|| lex == LEX_AND_ASSIGNMENT || lex == LEX_XOR_ASSIGNMENT || lex == LEX_OR_ASSIGNMENT)
	{
		if (!left->symbol->isModificableLvalue())
			throw SyntaxException("left operand must be modificable lvalue", lex);
		if (!ltype->isInt() || !rtype->isInt())
			throw SyntaxException("operands must be integer type", lex);
		return ltype;
	}
	if (lex == LEX_ADD_ASSIGNMENT || lex == LEX_SUB_ASSIGNMENT)
	{
		if (!left->symbol->isModificableLvalue())
			throw SyntaxException("left operand must be modificable lvalue", lex);
		if (!(ltype->isScalar() || ltype->isPointer()))
			throw SyntaxException("operands must be scalar type or pointer", lex);
		if (!right->getType()->canConvertTo(left->getType()))
			throw SyntaxException("type mismath", lex);
		return ltype;
	}	
	if (lex == LEX_LOGICAL_OR || lex == LEX_LOGICAL_AND)
	{
		if (!(ltype->isPointer() || ltype->isScalar()))
			throw SyntaxException("left operand must be pointer or scalar", lex);
		if (!(rtype->isPointer() || ltype->isScalar()))
			throw SyntaxException("right operand must be pointer or scalar", lex);
		return new TypeSymbol("int", 4);
	}
	if (lex == LEX_OR || lex == LEX_XOR || lex == LEX_AND || lex == LEX_LSHIFT || lex == LEX_RSHIFT 
		|| lex == LEX_PERSENT)
	{
		if (!ltype->isInt())
			throw SyntaxException("left operand must be int", lex);
		if (!rtype->isInt())
			throw SyntaxException("right operand must be int", lex);
		return new TypeSymbol("int", 4);
	}
	if (lex == LEX_EQUAL || lex == LEX_NOT_EQUAL || lex == LEX_LESS || lex == LEX_LESS_EQUAL 
		|| lex == LEX_GREAT_EQUAL || lex == LEX_GREAT)
	{
		bool l2r = ltype->canConvertTo(rtype);
		bool r2l = rtype->canConvertTo(ltype);
		if (!l2r && !r2l)
			throw SyntaxException("type mismatch", lex);
		return new TypeSymbol("int", 4); 
	}
	if (lex == LEX_ADD || lex == LEX_SUB || lex == LEX_MUL || lex == LEX_DIV)
	{
		bool l2r = ltype->canConvertTo(rtype);
		bool r2l = rtype->canConvertTo(ltype);
		if (!l2r && !r2l)
			throw SyntaxException("type mismatch", lex);
		if (l2r)
			return ltype;
		else
			return rtype;
	}
	
	throw std::exception("Internal error in \"BinaryOpNode::makeType()\"");
}

// �����������
BinaryOpNode::BinaryOpNode(Lexeme lex, Symbol* symbol, Node* left, Node* right)
	: Node(lex, symbol)
{
	this->left = left;	
	this->right = right;
}

// ����������
BinaryOpNode::~BinaryOpNode()
{
	delete left;
	delete right;
}

// ������� ���� �� �����
/*
void BinaryOpNode::print()
{
	std::cout << "(";
	left->print();
	std::cout << op.text;
	right->print();
	std::cout << ")";
}
*/

// �������� ����
void BinaryOpNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// ��������� ��� ����������
TypeSymbol* UnaryOpNode::makeType(Lexeme lex, Node* right, bool postfix)
{
	TypeSymbol* type = right->getType();

	if (lex == LEX_INCREMENT || lex == LEX_DECREMENT)
	{
		// ��������� ����������� ����������� ��������
		if (!right->symbol->isModificableLvalue())
			throw SyntaxException("operand must be modificable lvalue", lex);		
		if (type->isPointer() || type->name == "char" || type->name == "int" || type->name == "float")
			return type;
		throw SyntaxException("type mismatch", lex);
	}
	if (lex == LEX_AND)
	{
		if (!right->symbol->isLvalue())
			throw SyntaxException("operand must be lvalue", lex);
		return new TypeSymbol(type, TypeSymbol::MODE_POINTER);
	}
	if (lex == LEX_MUL)
	{
		if (!type->isPointer())
			throw SyntaxException("operand must be pointer", lex);
		return type->baseType;

	}
	if (lex == LEX_ADD || lex == LEX_SUB)
	{
		if (type->name == "char" || type->name == "int" || type->name == "float")
			return type;
		throw SyntaxException("type mismatch", lex);
	}
	if (lex == LEX_INVERT || lex == LEX_NOT)
	{
		if (!right->symbol->isModificableLvalue())
			throw SyntaxException("operand must be modificable lvalue", lex);
		if (type->name == "char" || type->name == "int")
			return type;
		throw SyntaxException("type mismatch", lex);
	}
	if (lex == LEX_SIZEOF)
	{
		return new TypeSymbol("int", 4);
	}
	
	throw std::exception("Internal error in \"UnaryOpNode::makeType()\"");
}

// �����������
UnaryOpNode::UnaryOpNode(Lexeme lex, Symbol* symbol,  Node* right, bool postfix)
	: Node(lex, symbol)
{
	this->right = right;
	this->postfix = postfix;
}

// ����������
UnaryOpNode::~UnaryOpNode()
{
	delete right;
}

// ������� ���� �� �����
/*
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
*/

// �������� ����
void UnaryOpNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// �����������
ValueNode::ValueNode(Lexeme lex, Symbol* symbol)
	: Node(lex, symbol)
{
}

// ����������
ValueNode::~ValueNode()
{
}

// ������� ���� �� �����
/*
void ValueNode::print()
{
	std::cout << value.text;
}
*/

// �������� ����
void ValueNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// �����������
IdentifierNode::IdentifierNode(Lexeme lex, Symbol* symbol)
	: Node(lex, symbol)
{
}
	
// ����������
IdentifierNode::~IdentifierNode()
{
}
	
// ������� ���� �� �����
/*
void IdentifierNode::print()
{
	std::cout << name.text;
}
*/

// �������� ����
void IdentifierNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// �����������
ArrayNode::ArrayNode(Symbol* symbol, Node* var, Node* idx)
	: Node(symbol)
{
	this->var = var;
	this->idx = idx;
}

// ����������
ArrayNode::~ArrayNode()
{
	delete var;
	delete idx;
}

// ������� ���� �� �����
/*
void ArrayNode::print()
{
	arr->print();
	std::cout << "[";
	idx->print();
	std::cout << "]";
}
*/

// �������� ����
void ArrayNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// �����������
FuncCallNode::FuncCallNode(Symbol* symbol, Node* func)
	: Node(symbol)
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
/*
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
*/

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
