#include "stdafx.h"
#include "Parser.h"
#include "Exceptions.h"

// �����������
Parser::Parser(Lexer& lexer)
	: lexer(lexer)
{

}

// ����������
Parser::~Parser(void)
{

}

// ��������� ������
Node* Parser::parse()
{
	buffer.clear();
	backStack.clear();
	tables.clear();

	if (lex == -1)
		next();

	ProgramNode* prog = new ProgramNode();
	tables.push_back(&prog->global);

	while (lex != LEX_EOF)
		// �������� ������� ����������
		parseUnit(prog);

	return prog;
}

// ��������� ���������
Node* Parser::parseExpression()
{
	ExpressionNode* result = NULL;
	if (lex == -1)
		next();

	while (true)
	{
		// ��������� ���������
		Node* n = parseAssignmentExpression();
		// ���� ��� �� ���������, �� �����
		if (result == NULL && n == NULL)
			return NULL;
		// ������� ��������� � ��������� ����
		if (result == NULL)
			result = new ExpressionNode(n);
		check(n == NULL, "expected assignment expression", lex);
		result->addAssignment(n);
		// ���� �������, �� �������� ��������� �������
		// � ��������� ��������� ���������
		if (lex == LEX_COMMA)
			next();
		else
			// ����� ������ �������
			break;
	}
	return result;
}

// ��������� ������� ����������
void Parser::parseUnit(ProgramNode* prog)
{
	// ��������� ���
	TypeSymbol* type = parseType();
	// ��������� ����������
	Node* left = parseDeclarator(type);
	// ���� ����������
	if (dynamic_cast<VariableSymbol*>(left->symbol) != NULL)
	{		
		while (true)
		{
			if (lex == LEX_ASSIGNMENT)
			{
				Lexeme op = lex;
				next();
				Node* right = parseAssignmentExpression();
				prog->addNode(new BinaryOpNode(op, BinaryOpNode::makeType(op, left, right), left, right));
			}

			if (lex == LEX_COMMA)
			{
				next();
				left = parseDeclarator(type);
			}
			else
				break;
		}
	}

	if (lex == LEX_SEMICOLON)
	{
		next();
		return ;
	}
}

// ��������� ���
TypeSymbol* Parser::parseType()
{
	TypeSymbol* type = NULL;

	// ��������� ��������� ����
	if (lex == LEX_TYPEDEF)
	{
		next();
		type = parseType();
		check(type == NULL, "expected type definition", lex);
		check(lex != LEX_ID, "expected typedef identifier", lex);
		type = addAliasSymbol(type, lex.text);
		next();
	}
	// ��� ������ ���
	else if (lex == LEX_VOID)
	{
		next();
		type = addTypeSymbol("void", 0);
	}
	// ��� ���������� ���
	else if (lex == LEX_CHAR)
	{
		next();
		type = addTypeSymbol("char", 1);
	}
	// ��� ����� ���
	else if (lex == LEX_INT)
	{
		next();
		type = addTypeSymbol("int", 4);
	}
	// ��� ������������ ���
	else if (lex == LEX_FLOAT)
	{
		next();
		type = addTypeSymbol("float", 4);
	}	
	// ��������� ���������
	else if (lex == LEX_STRUCT)
	{
		type = parseStruct();
	}
	// ��� ������������� - �������� ��� ����
	else if (lex == LEX_ID)
	{
		type = getTypeSymbol(lex.text);
		if (type != NULL)
			next();
		else
		{
			// ��� �� ���, � ���-�� ������
			return NULL;
		}
	}
	// ��� ����������� ���
	else if (lex == LEX_CONST)
	{		
		next();
		type = parseType();
		check(type == NULL, "incomplete type", lex);
		check(type->isConst(), "double \'const\'", lex);
		check(type->isVoid(), "incorrect type \'const void\'", lex);
		type = addTypeSymbol(type, TypeSymbol::MODE_CONST);
	}		

	// ���� const ����� ����� �������� ����
	if (type != NULL && lex == LEX_CONST)
	{
		while (lex == LEX_CONST)
		{
			next();
			check(type->isConst(), "double \'const\'", lex);
			check(type->isVoid(), "incorrect type \'const void\'", lex);
			type = addTypeSymbol(type, TypeSymbol::MODE_CONST);
		}
	}
	return type;
}

// ��������� �����������
Node* Parser::parseDeclarator(TypeSymbol* type)
{
	Node* node = NULL;
	type = parsePointer(type);
	if (lex == LEX_ID)
	{
		node = new IdentifierNode(lex, NULL);
		next();
	}
	else if (lex == LEX_LPAREN)
	{
		next();
		node = parseDeclarator(type);
		check(lex != LEX_RPAREN, "expected \')\'", lex);
		next();
	}
	else
		check(true, "expected identifier", lex);

	// ��� ������
	if (lex == LEX_LBRACKET)
	{
		while (lex == LEX_LBRACKET)
		{			
			if (node->symbol == NULL)
				checkSymbol(node->lex.text, false);
			else
				check(dynamic_cast<VariableSymbol*>(node->symbol) == NULL, "expected variable before \'[\'", lex);			
			next();
			// ��������� ������ �������
			Node* expr = parseConditionalExpression(NULL);
			// ��������� ���������			
			int count = 10;
			// ������������ ���
			type = addArraySymbol(type, count);
			if (node->symbol == NULL)
				node->symbol = addVariableSymbol(node->lex.text, type);
			else
				((ItemSymbol*)node->symbol)->type = type;
			// ��������� ����������� ������
			check(lex != LEX_RBRACKET, "expected \']\'", lex);
			next();
		}
	}
	else if (lex == LEX_LPAREN)
	{
		// ��� �������
		check(node->symbol != NULL, "expected identifier before \'(\'", lex);
		checkSymbol(node->lex.text);
		FunctionSymbol* fsymbol = addFunctionSymbol(node->lex.text, type);
		node->symbol = fsymbol;
		next();
		// ��������� ��������� �������
		SymbolsTable local;
		tables.push_back(&local);
		while (lex != LEX_RPAREN)
		{
			TypeSymbol * type = parseType();
			Node* arg = parseDeclarator(type);
			if (arg != NULL)
			{
				check(dynamic_cast<VariableSymbol*>(arg->symbol) == NULL, "function argument must be variable", lex);
				fsymbol->addParam((VariableSymbol*)arg->symbol);
				if (lex == LEX_COMMA)
					next();
				else
					break;
			}
			else
				break;
		}
		tables.pop_back();
		check(lex != LEX_RPAREN, "expected \')\'", lex);
		next();
	}
	else if (node->symbol == NULL)
	{
		checkSymbol(node->lex.text);
		node->symbol = addVariableSymbol(node->lex.text, type);
	}

	return node;
}

// ��������� ���������
TypeSymbol* Parser::parsePointer(TypeSymbol* type)
{
	// ��������� ������������ ����, ������� ����� ��������� ����� ����� ����
	if (type != NULL)
	{		
		while (lex == LEX_MUL)
		{
			next();
			type = addTypeSymbol(type, TypeSymbol::MODE_POINTER);
	
			while (lex == LEX_CONST)
			{
				next();
				check(type->isConst(), "double \'const\'", lex);
				type = addTypeSymbol(type, TypeSymbol::MODE_CONST);
			}
		}
	}

	return type;
}

// ��������� ������������
Node* Parser::parseAssignmentExpression()
{
	// ��������� ������� ��������
	Node* left = parseUnaryExpression();
	// ���� �������
	if (left != NULL)
	{
		// ��������� ��������
		if (isAssignmentOperator())
		{
			// ���������� ��� ���������
			Lexeme op = lex;
			next();
			// �������� ��������� � ������ �������
			Node* right = parseAssignmentExpression();
			// ���� �� �������, �� ������
			check(right == NULL, "expected right operand", op);
			// ���������� ���� ������������
			return new BinaryOpNode(op, BinaryOpNode::makeType(op, left, right), left, right);
		}
		else
		{
			// ��� ������ ��������
			return parseConditionalExpression(left);
		}
	}
	else
		// ��� ��� �� ������� ��������
		return parseConditionalExpression(NULL);

	return left;
}

// ��������� �������� ���������
Node* Parser::parseConditionalExpression(Node* left)
{
	// �������� ������� �����
	left = parseOperation(left, 0);
	if (left != NULL)
	{
		// ��� �������� ���������
		if (lex == LEX_QUESTION)
		{
			next();
			Node* first = parseExpression();
			check(first == NULL, "expected first operand", lex);
			check(lex != LEX_COLON, "expected \':\'", lex);
			next();
			Node* second = parseConditionalExpression(NULL);
			check(second == NULL, "expected second operand", lex);
			left = new ConditionalNode(left, first, second);
		}
	}
	return left;
}

#define PRIORITY_COUNT 10
#define OPERATION_COUNT 4

// ������ ��������
int operations[PRIORITY_COUNT][OPERATION_COUNT] = {
	{LEX_LOGICAL_OR, -1, -1, -1},
	{LEX_LOGICAL_AND, -1, -1, -1},
	{LEX_OR, -1, -1, -1},
	{LEX_XOR, -1, -1, -1},
	{LEX_AND, -1, -1, -1},
	{LEX_EQUAL, LEX_NOT_EQUAL, -1, -1},
	{LEX_LESS, LEX_LESS_EQUAL, LEX_GREAT_EQUAL, LEX_GREAT},
	{LEX_LSHIFT, LEX_RSHIFT},
	{LEX_ADD, LEX_SUB},
	{LEX_MUL, LEX_DIV, LEX_PERSENT}
};

// ��������� ��������
bool checkOperation(Lexeme lex, int priority)
{
	for (int i = 0; i < OPERATION_COUNT; i++)
	{
		if (operations[priority][i] == lex.code)
			return true;
	}
	return false;
}

// ��������� ��������
Node* Parser::parseOperation(Node* left, int priority)
{
	if (priority < PRIORITY_COUNT - 1)
		left = parseOperation(left, priority + 1);
	else if (left == NULL)
		left = parseCastExpression();

	if (left != NULL)
	{
		while (checkOperation(lex, priority))
		{
			Lexeme op = lex;
			next();
			Node* right = parseOperation(NULL, priority + 1);
			check(right == NULL, "expected right operand", lex);
			left = new BinaryOpNode(op, BinaryOpNode::makeType(op, left, right), left, right);
		}
	}

	return left;
}

// ��������� ���������� ����
Node* Parser::parseCastExpression()
{
	/*
	if (lex == LEX_LPAREN)
	{
		Lexeme op = lex;
		next();
		// ���� ���. �� ��������� ���������� ����
		if (isTypeName())
		{
			// ���������� ���
			Lexeme type = lex;
			next();
			if (lex != LEX_RPAREN)
				onSyntaxError("expected \')\'");
			next();
			// ��������� ���������� ���������
			Node* right = parseCastExpression();
			if (right == NULL)
				onSyntaxError("expected operand");
			return new UnaryOpNode(UnaryOpNode::makeType(op, right), op, right, false);
		}
		// ��� �� ���, ������ �������
		back();
	}
	*/
	
	// ��������� ������� ���������
	return parseUnaryExpression();
}

// ��������� ������� ��������
Node* Parser::parseUnaryExpression()
{
	if (lex == LEX_INCREMENT || lex == LEX_DECREMENT)
	{
		Lexeme op = lex;
		next();
		Node* right = parseUnaryExpression();
		check(right == NULL, "expected operand", lex);
		return new UnaryOpNode(op, UnaryOpNode::makeType(op, right, false), right, false);
	}
	else if (isUnaryOperator())
	{
		Lexeme op = lex;
		next();
		Node* right = parseCastExpression();
		check(right == NULL, "expected operand", lex);
		return new UnaryOpNode(op, UnaryOpNode::makeType(op, right, false), right, false);
	}
	else if (lex == LEX_SIZEOF)
	{
		Lexeme op = lex;
		next();
		check(lex != LEX_LPAREN, "expected \'(\'", lex);
		next();
		Node* right = parseUnaryExpression();
		check(right == NULL, "expected operand", lex);
		check(lex != LEX_RPAREN, "expected \')\'", lex);
		next();
		return new UnaryOpNode(op, UnaryOpNode::makeType(op, right, false), right, false);
	}
	else
		return parsePostfixExpression();
}

// ��������� ����������� ��������
Node* Parser::parsePostfixExpression()
{
	Node* left = parsePrimaryExpression();
	if (left != NULL)
	{
		while (true)
		{
			// ���� ��������� � �������� �������
			if (lex == LEX_LBRACKET)
			{
				check(!left->getType()->isArray(), "left operand of \'[\' must be array", lex);
				next();
				Node* idx = parseExpression();
				check(idx == NULL, "invalid array index", lex);
				check(lex != LEX_RBRACKET, "expected \']\'", lex);
				next();
				left = new ArrayNode(left->getType()->baseType, left, idx);
			}
			// ���� �����
			else if (lex == LEX_LPAREN)
			{			
				check(dynamic_cast<FunctionSymbol*>(left->symbol) == NULL, "left operand of \'(\' must be function", lex);
				next();
				FuncCallNode* fnode = new FuncCallNode(left->symbol, left);
				left = fnode;
				// ������ ����������
				while (true)
				{
					Node* arg = parseAssignmentExpression();
					if (arg)
					{
						fnode->addArgument(arg);
						if (lex != LEX_COMMA)
							break;
						next();
					}
					else
						break;
				}
				// ����������� ������
				check(lex != LEX_RPAREN, "expected \')\'", lex);
				next();
			}
			// ���� ��������� � �������� ���������
			else if (lex == LEX_DOT || lex == LEX_ARROW)
			{
				// ��������� ���
				TypeSymbol* type = left->getType();
				check(lex == LEX_DOT && !type->isStruct(), "left operand of \'.\' must be struct", lex);
				if (lex == LEX_ARROW)
				{
					check(!type->isPointer() || !type->baseType->isStruct(), "left operand of \'->\' must be pointer to struct", lex);
					type = type->baseType;
				}
				Lexeme op = lex;
				next();		
				// ��������� �������� ���� ���������
				check(lex != LEX_ID, "expected identifier", lex);
				StructSymbol* stype = (StructSymbol*)type;
				VariableSymbol* field = stype->findFieldByName(lex.text);
				check(field == NULL, "unknown field", lex);
				Node* right = new IdentifierNode(lex, field);
				left = new BinaryOpNode(op, field->type, left, right);
				next();
			}
			else if (lex == LEX_INCREMENT || lex == LEX_DECREMENT)
			{
				left = new UnaryOpNode(lex, UnaryOpNode::makeType(lex, left, true), left, true);
				next();
			}
			else
				break;
		}
	}
	return left;
}

// ��������� �������
Node* Parser::parsePrimaryExpression()
{
	Node* result = NULL;
	if (lex == LEX_ID)
	{
		Symbol* symbol = getSymbol(lex.text);
		check(symbol == NULL, "unknown identifier", lex);
		check(symbol->isType(), "expected variable or function", lex);
		result = new IdentifierNode(lex, symbol);
		next();
	}
	else if (lex == LEX_CHAR_VALUE || lex == LEX_INT_VALUE || lex == LEX_FLOAT_VALUE || lex == LEX_STRING_VALUE)
	{
		TypeSymbol* type = NULL;
		if (lex == LEX_CHAR_VALUE)
		{
			type = addTypeSymbol("char", 1);
			type = addTypeSymbol(type, TypeSymbol::MODE_CONST);
		}
		else if (lex == LEX_INT_VALUE)
		{
			type = addTypeSymbol("int", 4);
			type = addTypeSymbol(type, TypeSymbol::MODE_CONST);
		}
		else if (lex == LEX_FLOAT_VALUE)
		{
			type = addTypeSymbol("float", 4);
			type = addTypeSymbol(type, TypeSymbol::MODE_CONST);
		}
		else
		{
			type = addTypeSymbol("char", 1);
			type = addTypeSymbol(type, TypeSymbol::MODE_CONST);
			type = addTypeSymbol(type, TypeSymbol::MODE_POINTER);
		}
		result = new ValueNode(lex, type);
		next();
	}
	else if (lex == LEX_LPAREN)
	{
		next();
		result = parseExpression();
		if (result == NULL)
			back();
		else
		{
			check(lex != LEX_RPAREN, "expected close parentness", lex);
			next();
		}
	}
	return result;
}

// ��������� ���������
TypeSymbol* Parser::parseStruct()
{
	StructSymbol* symbol = NULL;
	next();	
	if (lex == LEX_ID)
	{
		symbol = new StructSymbol(lex.text);
		next();
	}
	else
		symbol = new StructSymbol("");

	return symbol;
}

// �������� ��������� �������
void Parser::next()
{
	// �������� ������� ������� � ����
	if (lex != -1)
	{
		buffer.push_back(lex);
		if (buffer.size() >= 1000)
			buffer.erase(buffer.begin(), buffer.begin() + 100);
	}

	// ���� ���� ����
	if (backStack.empty())
		// �� ����������� ������� � �������
		lex = lexer.next();
	else
	{
		// ����� ����� ������� �� �����
		lex = backStack.back();
		backStack.pop_back();
	}
}

// ������� ������� �����
void Parser::back()
{
	if (buffer.empty())
		throw std::exception("Internal error: back buffer is empty");

	// �������� ������� � ����
	backStack.push_back(lex);
	// ���������� ������� �� ������
	lex = buffer.back();
	buffer.pop_back();
}

// ��������� � �������������� ������
void Parser::check(bool cond, const char* msg, Lexeme l)
{
	if (cond)
		throw SyntaxException(msg, l.line, l.col);
}

// ���������, ��� ������� �������� ������� ���������
bool Parser::isUnaryOperator()
{
	return lex == LEX_AND 
		|| lex == LEX_MUL 
		|| lex == LEX_ADD 
		|| lex == LEX_SUB 
		|| lex == LEX_INVERT 
		|| lex == LEX_NOT;
}

// ��������� ��� ������� �������� �������������
bool Parser::isAssignmentOperator()
{
	return lex == LEX_ASSIGNMENT 
		|| lex == LEX_MUL_ASSIGNMENT 
		|| lex == LEX_DIV_ASSIGNMENT 
		|| lex == LEX_PERSENT_ASSIGNMENT 
		|| lex == LEX_ADD_ASSIGNMENT 
		|| lex == LEX_SUB_ASSIGNMENT
		|| lex == LEX_LSHIFT_ASSIGNMENT 
		|| lex == LEX_RSHIFT_ASSIGNMENT 
		|| lex == LEX_AND_ASSIGNMENT
		|| lex == LEX_XOR_ASSIGNMENT 
		|| lex == LEX_OR_ASSIGNMENT;
}

// ���������, ��� ������� �������� ������ ����
bool Parser::isTypeName()
{
	//TODO
	return false;
}

// ��������� ������
void Parser::checkSymbol(std::string name, bool forceAll)
{
	if (forceAll)
		for (int i = tables.size() - 1; i >= 0; i--)
		{
			Symbol* s = tables[i]->findByName(name);
			check(s != NULL, "identifier already used", lex);
		}
	else
		check(tables.back()->findByName(name), "identifier already used", lex);
}

// ����� ���
TypeSymbol* Parser::getTypeSymbol(std::string name)
{
	return dynamic_cast<TypeSymbol*>(tables.front()->findByName(name));
}

// �������� ���
TypeSymbol* Parser::addTypeSymbol(std::string name, int length)
{
	TypeSymbol* type = getTypeSymbol(name);
	if (!type)
	{
		type = new TypeSymbol(name, length);
		tables.front()->addSymbol(type);
	}
	return type;
}

// �������� ���
TypeSymbol* Parser::addTypeSymbol(TypeSymbol* baseType, int mode)
{
	TypeSymbol* type = getTypeSymbol(TypeSymbol::makeName(baseType, mode));
	if (!type)
	{
		type = new TypeSymbol(baseType, mode);
		tables.front()->addSymbol(type);
	}
	return type;
}

// �������� ���������
AliasSymbol* Parser::addAliasSymbol(TypeSymbol* baseType, std::string name)
{
	checkSymbol(name);
	AliasSymbol* type = new AliasSymbol(baseType, name);
	tables.front()->addSymbol(type);
	return type;
}

// �������� ������
ArraySymbol* Parser::addArraySymbol(TypeSymbol* baseType, int count)
{
	ArraySymbol* type = new ArraySymbol(baseType, count);
	tables.front()->addSymbol(type);
	return type;
}

// �������� ��������� ��� �������
Symbol* Parser::getSymbol(std::string name)
{
	for (int i = tables.size() - 1; i >= 0; i--)
	{
		Symbol* s = tables[i]->findByName(name);
		if (s)
			return s;
	}
	return NULL;
}

// �������� ����������
VariableSymbol* Parser::addVariableSymbol(std::string name, TypeSymbol* type)
{
	checkSymbol(name, false);
	VariableSymbol* var = new VariableSymbol(name, type);
	tables.back()->addSymbol(var);
	return var;
}

// �������� �������
FunctionSymbol* Parser::addFunctionSymbol(std::string name, TypeSymbol* type)
{
	checkSymbol(name);
	FunctionSymbol* f = new FunctionSymbol(name, type);
	tables.front()->addSymbol(f);
	return f;
}
