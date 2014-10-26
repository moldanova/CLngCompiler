#include "stdafx.h"
#include "Nodes.h"
#include "Lexer.h"
#include "Exceptions.h"

//---------------------------------------------------------------------------

// Конструктор
Node::Node()
{
	this->symbol = NULL;
}

// Конструктор
Node::Node(Symbol* symbol)
{
	this->symbol = symbol;
}

// Конструктор
Node::Node(Lexeme lex, Symbol* symbol)
{
	this->lex = lex;
	this->symbol = symbol;
}

// Деструктор
Node::~Node()
{
}

// Получить тип узла
TypeSymbol* Node::getType()
{
	TypeSymbol* type = dynamic_cast<TypeSymbol*>(symbol);
	ItemSymbol* item = dynamic_cast<ItemSymbol*>(symbol);

	if (type)
		return type;
	else
		return item->type;
}

// Посетить узел
void Node::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//-------------------------------------------------------------------------

// Конструктор
NodesArrayNode::NodesArrayNode()
{
}

// Конструктор
NodesArrayNode::NodesArrayNode(Symbol* symbol)
	: Node(symbol)
{
}

// Конструктор
NodesArrayNode::NodesArrayNode(Lexeme lex, Symbol* symbol)
	: Node(lex, symbol)
{
}

// Деструктор
NodesArrayNode::~NodesArrayNode()
{
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	nodes.clear();
}

// Добавить узел
void NodesArrayNode::addNode(Node* node)
{
	nodes.push_back(node);
}

//-------------------------------------------------------------------------

// Конструктор
ProgramNode::ProgramNode()
{
	globals.addSymbol(new TypeSymbol("void"));
	globals.addSymbol(new TypeSymbol("char"));
	globals.addSymbol(new TypeSymbol("int"));
	globals.addSymbol(new TypeSymbol("float"));
}

// Деструктор
ProgramNode::~ProgramNode()
{
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	nodes.clear();
}

// Посетить узел
void ProgramNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
ExpressionNode::ExpressionNode(Node* node)
	: NodesArrayNode(node->symbol)
{
	addNode(node);
}

// Деструктор
ExpressionNode::~ExpressionNode()
{
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	nodes.clear();
}

// Посетить узел
void ExpressionNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
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

// Посетить узел
void ConditionalNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Вычислить тип результата
TypeSymbol* BinaryOpNode::makeType(Lexeme lex, Node* left, Node* right)
{
	TypeSymbol* ltype = left->getType();
	TypeSymbol* rtype = right->getType();
	if (lex == LEX_ASSIGNMENT)
	{
		if (!left->symbol->isModificableLvalue())
			throw SyntaxException("left operand must be modificable lvalue", lex);

		// Особо обрабатываем присваивание переменной указателя на строку
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
		return new TypeSymbol("int");
	}
	if (lex == LEX_OR || lex == LEX_XOR || lex == LEX_AND || lex == LEX_LSHIFT || lex == LEX_RSHIFT 
		|| lex == LEX_PERSENT)
	{
		if (!ltype->isInt() && !ltype->isChar())
			throw SyntaxException("left operand must be int", lex);
		if (!rtype->isInt() && !rtype->isChar())
			throw SyntaxException("right operand must be int", lex);
		return new TypeSymbol("int");
	}
	if (lex == LEX_EQUAL || lex == LEX_NOT_EQUAL || lex == LEX_LESS || lex == LEX_LESS_EQUAL 
		|| lex == LEX_GREAT_EQUAL || lex == LEX_GREAT)
	{
		bool l2r = ltype->canConvertTo(rtype);
		bool r2l = rtype->canConvertTo(ltype);
		if (!l2r && !r2l)
			throw SyntaxException("type mismatch", lex);
		return new TypeSymbol("int"); 
	}
	if (lex == LEX_ADD || lex == LEX_SUB || lex == LEX_MUL || lex == LEX_DIV)
	{
		if ((rtype->isArray() || rtype->isPointer()) && (ltype->isInt() || ltype->isChar()))
			return rtype;
		if ((rtype->isInt() || rtype->isChar()) && (ltype->isPointer() || ltype->isArray()))
			return ltype;
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

// Конструктор
BinaryOpNode::BinaryOpNode(Lexeme lex, Symbol* symbol, Node* left, Node* right)
	: Node(lex, symbol)
{
	this->left = left;	
	this->right = right;
}

// Деструктор
BinaryOpNode::~BinaryOpNode()
{
	delete left;
	delete right;
}

// Посетить узел
void BinaryOpNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Вычислить тип результата
TypeSymbol* UnaryOpNode::makeType(Lexeme lex, Node* right, bool postfix)
{
	TypeSymbol* type = right->getType();

	if (lex == LEX_INCREMENT || lex == LEX_DECREMENT)
	{
		// Проверяем возможность модификации значения
		if (!right->symbol->isModificableLvalue())
			throw SyntaxException("operand must be modificable lvalue", lex);		
		if (type->isPointer() || type->name == "char" || type->name == "int" || type->name == "float")
			return type;
		throw SyntaxException("type mismatch", lex);
	}
	if (lex == LEX_AND)
	{
		if (!right->symbol->isLvalue())
			throw SyntaxException("operand of \'&\' must be a lvalue", lex);
		return new TypeSymbol(type, TypeSymbol::MODE_POINTER);
	}
	if (lex == LEX_MUL)
	{
		if (!type->isPointer())
			throw SyntaxException("operand of \'*\' must be a pointer", lex);
		return type->baseType;

	}
	if (lex == LEX_ADD || lex == LEX_SUB)
	{
		if (type->isScalar())
		{
			if (type->isConst())
				return type->baseType;
			else
				return type;
		}
		throw SyntaxException("type mismatch", lex);
	}
	if (lex == LEX_NOT)
	{
		if (type->isScalar() || type->isPointer() || type->isArray())
			return new TypeSymbol("int");
		throw SyntaxException("type mismatch", lex);
	}
	if (lex == LEX_INVERT)
	{
		if (type->isChar() || type->isInt())
			return type;
		throw SyntaxException("type mismatch", lex);
	}
	if (lex == LEX_SIZEOF)
	{
		return new TypeSymbol("int");
	}
	
	throw std::exception("Internal error in \"UnaryOpNode::makeType()\"");
}

// Конструктор
UnaryOpNode::UnaryOpNode(Lexeme lex, Symbol* symbol,  Node* right, bool postfix)
	: Node(lex, symbol)
{
	this->right = right;
	this->postfix = postfix;
}

// Деструктор
UnaryOpNode::~UnaryOpNode()
{
	delete right;
}

// Посетить узел
void UnaryOpNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
ValueNode::ValueNode(Lexeme lex, Symbol* symbol)
	: Node(lex, symbol)
{
}

// Деструктор
ValueNode::~ValueNode()
{
}

// Посетить узел
void ValueNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
IdentifierNode::IdentifierNode(Lexeme lex, Symbol* symbol)
	: Node(lex, symbol)
{
}
	
// Деструктор
IdentifierNode::~IdentifierNode()
{
}
	
// Посетить узел
void IdentifierNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
ArrayNode::ArrayNode(Symbol* symbol, Node* var, Node* idx)
	: Node(symbol)
{
	this->var = var;
	this->idx = idx;
}

// Деструктор
ArrayNode::~ArrayNode()
{
	delete var;
	delete idx;
}

// Посетить узел
void ArrayNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
FuncCallNode::FuncCallNode(Symbol* symbol, Node* func)
	: NodesArrayNode(symbol)
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

// Посетить узел
void FuncCallNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
CompoundNode::CompoundNode()
{
}

// Деструктор
CompoundNode::~CompoundNode()
{
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	nodes.clear();
}

// Посетить узел
void CompoundNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
FunctionNode::FunctionNode(Symbol* symbol)
{
	this->symbol = symbol;
	this->statement = NULL;
}

// Деструктор
FunctionNode::~FunctionNode()
{
	if (statement)
		delete statement;
}

// Посетить узел
void FunctionNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
IfNode::IfNode(Node* expr, Node* statement1, Node* statement2)
{
	this->expr = expr;
	this->statement1 = statement1;
	this->statement2 = statement2;
}

// Деструктор
IfNode::~IfNode()
{
	if (expr)
		delete expr;
	if (statement1)
		delete statement1;
	if (statement2)
		delete statement2;
}

// Посетить узел
void IfNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
WhileNode::WhileNode(Node* expr, Node* statement)
{
	this->expr = expr;
	this->statement = statement;
}

// Деструктор
WhileNode::~WhileNode()
{
	if (expr)
		delete expr;
	if (statement)
		delete statement;
}

// Посетить узел
void WhileNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
DoNode::DoNode(Node* expr, Node* statement)
{
	this->expr = expr;
	this->statement = statement;
}

// Деструктор
DoNode::~DoNode()
{
	if (expr)
		delete expr;
	if (statement)
		delete statement;
}

// Посетить узел
void DoNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
ForNode::ForNode(Node* expr1, Node* expr2, Node* expr3, Node* statement)
{
	this->expr1 = expr1;
	this->expr2 = expr2;
	this->expr3 = expr3;
	this->statement = statement;
}

// Деструктор
ForNode::~ForNode()
{
	if (expr1)
		delete expr1;
	if (expr2)
		delete expr2;
	if (expr3)
		delete expr3;
	if (statement)
		delete statement;
}

// Посетить узел
void ForNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
BreakNode::BreakNode()
{
}

// Деструктор
BreakNode::~BreakNode()
{
}

// Посетить узел
void BreakNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
ContinueNode::ContinueNode()
{
}

// Деструктор
ContinueNode::~ContinueNode()
{
}

// Посетить узел
void ContinueNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}

//---------------------------------------------------------------------------

// Конструктор
ReturnNode::ReturnNode(Node* expr)
{
	this->expr = expr;
}

// Деструктор
ReturnNode::~ReturnNode()
{
	if (expr)
		delete expr;
}

// Посетить узел
void ReturnNode::visit(INodeVisitor* visitor)
{
	visitor->OnNode(this);
}