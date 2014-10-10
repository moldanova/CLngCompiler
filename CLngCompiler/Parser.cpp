#include "stdafx.h"
#include "Parser.h"
#include "Exceptions.h"
#include "Calculator.h"

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
Node* Parser::parse(bool isExpression)
{
	buffer.clear();
	backStack.clear();
	tables.clear();

	if (lex == -1)
		next();

	ProgramNode* prog = new ProgramNode();
	tables.push_back(&prog->globals);
	// ��������� ������� ���������
	while (lex != LEX_EOF)
	{
		// ��������� ����������
		if (!parseDeclaration(prog))
		{
			// �� �������, ����� ��� ���������
			if (isExpression)
			{
				// ��������� ���������
				Node* node = parseExpression();
				if (node)
				{
					prog->addNode(node);
					check(lex != LEX_SEMICOLON, "expected \';\' after expression", lex);
					next();
				}
				else
					// ���� �� �������, �� ��������� ������
					break;
			}
			else
				// ���� �� �������, �� ��������� ������
				break;
		}
	}

	// ���� �� ������� ��������� ��������� ���������
	check(lex != LEX_EOF, "invalid parsing", lex);

	tables.pop_back();
	return prog;
}

// ��������� ����������
bool Parser::parseDeclaration(NodesArrayNode* parent)
{
	// ��������� ���
	TypeSymbol* type = parseType();
	// ��������� ����������
	Node* node = parseDeclarator(type);
	if (node == NULL)
		return false;
	if (!node->isEmpty())
	{
		if (dynamic_cast<BinaryOpNode*>(node) || dynamic_cast<FunctionNode*>(node))
			parent->addNode(node);
		while (lex == LEX_COMMA)
		{			
			check(!node->symbol->isType() && dynamic_cast<VariableSymbol*>(node->symbol) == NULL, "before \',\' expected variable", lex);
			next();
			node = parseDeclarator(type);
			check(!node->symbol->isType() && dynamic_cast<VariableSymbol*>(node->symbol) == NULL, "after \',\' expected variable", lex);
			if (dynamic_cast<BinaryOpNode*>(node))
				parent->addNode(node);
		}
		if (dynamic_cast<FunctionNode*>(node) == NULL)
		{
			check(lex != LEX_SEMICOLON, "expected \';\'", lex);
			next();
		}
	}
	else
	{
		delete node;
		check(lex != LEX_SEMICOLON, "expected \';\'", lex);
		next();
	}
	return true;
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

// ��������� ���������
TypeSymbol* Parser::parseStruct()
{
	next();
	TypeSymbol* type = NULL;
	Lexeme name;
	if (lex == LEX_ID)
	{
		name = lex;
		next();
		// ��� ��������� � ��� ������������� ����
		if (lex != LEX_LBRACE)
		{
			type = getTypeSymbol(name.text);
			check(type == NULL, "undefined struct type", name);
		}
	}

	// ��� ����������� ���������
	if (lex == LEX_LBRACE)
	{
		next();
		StructSymbol* stype = addStructSymbol(name.text);
		type = stype;

		tables.push_back(&stype->fields);

		while (lex != LEX_RBRACE)
		{
			TypeSymbol* type = parseType();
			while (true)
			{
				Node* node = parseDeclarator(type);
				check(node->isEmpty(), "expected field name", lex);
				check(dynamic_cast<VariableSymbol*>(node->symbol) == NULL, "expected only fields", lex);
				if (lex == LEX_COMMA)
					next();
				else
					break;
			}

			check(lex != LEX_SEMICOLON, "expected \';\'", lex);
			next();
		}
		tables.pop_back();

		check(lex != LEX_RBRACE, "expected \'}\'", lex);
		next();
	}

	return type;
}

// ��������� �����������
Node* Parser::parseDeclarator(TypeSymbol* type, bool needParen)
{
	Node* node = NULL;
	type = parsePointer(type);
	// ��� ������������� 
	if (lex == LEX_ID)
	{
		Symbol* s = getSymbol(lex.text);
		if (type == NULL && s != NULL)
		{
			// �������� ��� ��������� � ���������� ��� �������
			if (dynamic_cast<VariableSymbol*>(s) != NULL || dynamic_cast<FunctionSymbol*>(s) != NULL)
				return NULL;
		}
		node = new IdentifierNode(lex, type);
		next();
	}
	// �����-�� ������� ������
	else if (lex == LEX_LPAREN)
	{
		next();
		node = parseDeclarator(type, true);
		if (type == NULL && node == NULL)
		{
			// �������� ��� ���������
			back();
			return NULL;
		}
		check(lex != LEX_RPAREN, "expected \')\'", lex);
		next();

		if (!node->symbol->isType())
			return node;
	}	
	else if (type != NULL)
	{
		if (lex == LEX_INT_VALUE || lex == LEX_FLOAT_VALUE || lex == LEX_CHAR_VALUE || lex == LEX_STRING_VALUE)
			check(true, "expected identifier", lex);
	}

	if (node == NULL)
	{		
		if (type == NULL)
			return NULL;
		else if (needParen)
			check(true, "expected identifier", lex);
		else
			return new EmptyNode(type);
	}
	
	// ��� ���������� ����
	if (node->isEmpty())
		;
	// ��� ����������� �������
	else if (lex == LEX_LBRACKET)
	{
		checkSymbol(node->lex.text, false);
		// ��������� ���
		check(!node->symbol->isType(), "expected variable before \'[\'", lex);
		// ��������� ������
		type = parseArray(type);
		node->symbol = addVariableSymbol(node->lex.text, type);
	}
	// ��� ����������� �������
	else if (lex == LEX_LPAREN)
	{		
		return parseFunction(node);		
	}
	else if (type == NULL)
	{
		// ��� ������
		check(true, "expected \'(\'", lex);
	}
	// ��� ������� ����������
	else
	{
		node->symbol = addVariableSymbol(node->lex.text, type);
		if (lex == LEX_ASSIGNMENT)
		{
			Lexeme l = lex;
			next();
			Node* val = parseAssignmentExpression();
			return new BinaryOpNode(l, BinaryOpNode::makeType(l, node, val), node, val);
		}
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

// ��������� ������
TypeSymbol* Parser::parseArray(TypeSymbol* type)
{
	while (lex == LEX_LBRACKET)
	{
		next();
		// ��������� ������ �������
		Node* expr = parseConditionalExpression(NULL);
		check(expr == NULL, "expected constant expression", lex);
		// ��������� ���������			
		Calculator calc(expr);
		int count = calc.result.asInt;
		// ������������ ���
		type = addArraySymbol(type, count);
		// ��������� ����������� ������
		check(lex != LEX_RBRACKET, "expected \']\'", lex);
		next();
	}
	return type;
}

// ��������� �������
Node* Parser::parseFunction(Node* node)
{
	// ��� �������	
	checkSymbol(node->lex.text);
	FunctionSymbol* fsymbol;
	// ���� ��� �� �����, �� �� ��������� ��� int
	if (node->symbol == NULL)
		fsymbol = addFunctionSymbol(node->lex.text, getTypeSymbol("int"));
	else
		fsymbol = addFunctionSymbol(node->lex.text, node->getType());
	node->symbol = fsymbol;
	FunctionNode* fnode = new FunctionNode(fsymbol);
	next();
	// ��������� ��������� �������
	tables.push_back(&fnode->params);
	while (lex != LEX_RPAREN)
	{
		TypeSymbol * type = parseType();
		Node* arg = parseDeclarator(type);
		check(arg == NULL || (!arg->isEmpty() && dynamic_cast<VariableSymbol*>(arg->symbol) == NULL), "function argument must be type or variable", lex);
		fsymbol->params.addSymbol(arg->getType());
		if (lex == LEX_COMMA)
			next();
		else
			break;
	}
	tables.pop_back();
	check(lex != LEX_RPAREN, "expected \')\'", lex);
	next();
	// ��������� ���� �������
	if (lex == LEX_LBRACE)
	{
		tables.push_back(&fnode->params);
		fnode->statement = parseStatement();
		tables.pop_back();
		delete node;
		return fnode;
	}
	
	delete fnode;
	return node;
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
		else
		{
			check(n == NULL, "expected assignment expression", lex);
			result->addNode(n);
		}
		// ���� �������, �� �������� ��������� �������
		// � ��������� ��������� ���������
		if (lex == LEX_COMMA)
			next();
		else
			// ����� ������ �������
			break;
	}

	if (result != NULL && result->nodes.size() == 1)
	{
		Node* node = result->nodes[0];
		result->nodes.clear();
		delete result;
		return node;
	}

	return result;
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
	{LEX_LSHIFT, LEX_RSHIFT, -1, -1},
	{LEX_ADD, LEX_SUB, -1, -1},
	{LEX_MUL, LEX_DIV, LEX_PERSENT, -1}
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
				FunctionSymbol* fsymbol = dynamic_cast<FunctionSymbol*>(left->symbol);
				check(fsymbol == NULL, "operand of \'(\' must be function", lex);
				next();
				FuncCallNode* fnode = new FuncCallNode(left->symbol, left);				
				// ������ ����������
				int n = 0;
				while (true)
				{
					Node* arg = parseAssignmentExpression();					
					if (arg)
					{
						check(n >= fsymbol->params.count(), "function has no more params", lex);
						fnode->addNode(arg);
						check(!arg->getType()->canConvertTo((TypeSymbol*)fsymbol->params[n]), "function args type mismatch", lex);
						n++;
						if (lex != LEX_COMMA)
							break;
						next();
					}
					else
						break;
				}
				check(n < fsymbol->params.count(), "function has more params", lex);
				// ����������� ������
				check(lex != LEX_RPAREN, "expected \')\'", lex);
				next();
				left = fnode;
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
				VariableSymbol* field = (VariableSymbol*)stype->fields.findByName(lex.text);
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

// ��������� ����
Node* Parser::parseStatement()
{
	while (true)
	{
		if (lex == LEX_LBRACE)
			return parseCompoundStatement();
		else if (lex == LEX_IF)
			return parseIfStatement();
		else if (lex == LEX_WHILE)
			return parseWhileStatement();
		else if (lex == LEX_DO)
			return parseDoStatement();
		else if (lex == LEX_FOR)
			return parseForStatement();
		else if (lex == LEX_BREAK)
		{
			next();
			check(lex != LEX_SEMICOLON, "expected \';\'", lex);
			next();
			return new BreakNode();
		}
		else if (lex == LEX_RETURN)
		{
			next();
			Node* expr = parseExpression();
			check(lex != LEX_SEMICOLON, "expected \';\'", lex);
			next();
			return new ReturnNode(expr);
		}
		else if (lex == ';')
		{
			next();
			continue;
		}
		else
		{
			Node* node = parseExpression();
			if (node)
			{
				check(lex != LEX_SEMICOLON, "expected \';\'", lex);
				next();
			}
			return node;
		}
		break;
	}
	return NULL;
}

// ��������� ����
Node* Parser::parseCompoundStatement()
{
	next();
	CompoundNode* node = new CompoundNode();
	tables.push_back(&node->locals);
	// ��������� �����������
	while (parseDeclaration(node))
		;
	// ��������� �����
	Node* n = parseStatement();
	while (n)
	{
		node->addNode(n);
		n = parseStatement();
	}
	tables.pop_back();
	check(lex != LEX_RBRACE, "expected \'}\'", lex);
	next();

	if (node->nodes.empty())
	{
		delete node;
		return NULL;
	}
	else
		return node;
}

// ��������� ����
Node* Parser::parseIfStatement()
{
	next();
	check(lex != LEX_LPAREN, "expected \'(\'", lex);
	next();
	Node* expr = parseExpression();
	check(lex != LEX_RPAREN, "expected \')\'", lex);
	next();
	Node* st1 = parseStatement();
	Node* st2 = NULL;
	if (lex == LEX_ELSE)
	{
		next();
		st2 = parseStatement();
	}
	return new IfNode(expr, st1, st2);
}

// ��������� ����
Node* Parser::parseWhileStatement()
{
	next();
	check(lex != LEX_LPAREN, "expected \'(\'", lex);
	next();
	Node* expr = parseExpression();
	check(lex != LEX_RPAREN, "expected \')\'", lex);
	next();
	Node* st = parseStatement();
	return new WhileNode(expr, st);
}

// ��������� ����
Node* Parser::parseDoStatement()
{
	next();
	Node* st = parseStatement();
	check(lex != LEX_WHILE, "expected \'while\'", lex);
	next();
	check(lex != LEX_LPAREN, "expected \'(\'", lex);
	next();
	Node* expr = parseExpression();
	check(lex != LEX_RPAREN, "expected \')\'", lex);
	next();
	check(lex != LEX_SEMICOLON, "expected \';\'", lex);
	next();
	return new DoNode(expr, st);
}

// ��������� ����
Node* Parser::parseForStatement()
{
	next();
	check(lex != LEX_LPAREN, "expected \'(\'", lex);
	next();
	Node* expr1 = parseExpression();
	check(lex != LEX_SEMICOLON, "expected \';\'", lex);
	next();
	Node* expr2 = parseExpression();
	check(lex != LEX_SEMICOLON, "expected \';\'", lex);
	next();
	Node* expr3 = parseExpression();
	check(lex != LEX_RPAREN, "expected \')\'", lex);
	next();
	Node* st = parseStatement();
	return new ForNode(expr1, expr2, expr3, st);
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

// �������� ���������
StructSymbol* Parser::addStructSymbol(std::string name)
{
	if (!name.empty())
		checkSymbol(name);
	StructSymbol* type = new StructSymbol(name);
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
