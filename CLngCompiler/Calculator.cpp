#include "stdafx.h"
#include "Calculator.h"
#include "Exceptions.h"
#include "Lexer.h"


// Калькулятор
Calculator::Calculator(Node* expr)
{
	expr->visit(this);
}

// Посетить узел
void Calculator::OnNode(Node* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел программы
void Calculator::OnNode(ProgramNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел выражения
void Calculator::OnNode(ExpressionNode* node)
{
	if (!node->nodes.empty())
		node->nodes.front()->visit(this);
	else
		throw SyntaxException("can not calculate expression", node->lex);
}

// Посетить узел условного выраженрия
void Calculator::OnNode(ConditionalNode* node)
{
	node->left->visit(this);
	if (result.asChar || result.asInt || result.asFloat)
		node->first->visit(this);
	else
		node->second->visit(this);
}

// Посетить узел бинарной операции
void Calculator::OnNode(BinaryOpNode* node)
{
	node->right->visit(this);
	Value r = result;
	node->left->visit(this);
	Value l = result;	

	switch (node->lex.code)
	{
	case LEX_LOGICAL_OR: result = l || r; break;
	case LEX_LOGICAL_AND: result = l && r; break;
	case LEX_OR: result = l | r; break;
	case LEX_XOR: result = l ^ r; break;
	case LEX_AND: result = l & r; break;
	case LEX_EQUAL: result = l == r; break;
	case LEX_NOT_EQUAL: result = l != r; break;
	case LEX_LESS: result = l < r; break;
	case LEX_LESS_EQUAL: result = l <= r; break;
	case LEX_GREAT_EQUAL: result = l >= r; break;
	case LEX_GREAT: result = l >= r; break;
	case LEX_LSHIFT: result = l << r; break;
	case LEX_RSHIFT: result = l >> r; break;
	case LEX_ADD: result = l + r; break;
	case LEX_SUB: result = l - r; break;
	case LEX_MUL: result = l * r; break;
	case LEX_DIV: result = l / r; break;
	case LEX_PERSENT: result = l % r; break;
	default:
		throw SyntaxException("expression is not constant", node->lex);
	}
}

// Посетить узел унарной операции
void Calculator::OnNode(UnaryOpNode* node)
{
	node->right->visit(this);
	Value r = result;

	switch (node->lex.code)
	{
	case LEX_ADD: result = r; break;
	case LEX_SUB: result = -r; break;
	case LEX_INVERT: result = ~r; break;
	case LEX_NOT: result = !r; break;
	default:
		throw SyntaxException("expression is not constant", node->lex);
	}
}

// Посетить узел значения
void Calculator::OnNode(ValueNode* node)
{
	result.type = node->getType();
	result.asChar = 0;
	result.asInt = 0;
	result.asFloat = 0;

	if (result.type->isChar())
		result.asChar = node->lex.text[0];
	else if (result.type->isInt())
		result.asInt = atoi(node->lex.text.c_str());
	else if (result.type->isFloat())
		result.asFloat = atof(node->lex.text.c_str());
	else
		throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел идентификатора
void Calculator::OnNode(IdentifierNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел обращения к массиву
void Calculator::OnNode(ArrayNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел вызова функции
void Calculator::OnNode(FuncCallNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел
void Calculator::OnNode(CompoundNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел функции
void Calculator::OnNode(FunctionNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел
void Calculator::OnNode(IfNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел
void Calculator::OnNode(WhileNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел
void Calculator::OnNode(DoNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел
void Calculator::OnNode(ForNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел
void Calculator::OnNode(BreakNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел
void Calculator::OnNode(ContinueNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}

// Посетить узел
void Calculator::OnNode(ReturnNode* node)
{
	throw SyntaxException("expression is not constant", node->lex);
}