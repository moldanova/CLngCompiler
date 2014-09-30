#include "stdafx.h"
#include "Parser.h"
#include "ConstExpressionCalc.h"

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
	{
		// �������� ������� ����������
		Node* node = parseUnit();
		if (node != NULL)
			prog->addNode(node);
	}

	return prog;
}

// ������� ������� ������
bool Parser::hasErrors()
{
	return false;
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
		else if (n == NULL)
			OnSyntaxError("invalid expression");
		else
			result->addAssignment(n);
		// ���� �������, �� �������� ��������� �������
		// � ��������� ��������� ���������
		if (lex == LEX_COMMA)
			next();
		else
		{
			// ����� ������ �������
			break;
		}
	}
	return result;
}

// ��������� ������� ����������
Node* Parser::parseUnit()
{
	TypeSymbol* type = parseType();
	ItemSymbol* item = parseDeclarator(type);
	if (dynamic_cast<VariableSymbol*>(item) != NULL)
	{
		while (lex == LEX_COMMA)
		{
			next();
			item = parseDeclarator(type);
		}
	}

	if (lex == LEX_SEMICOLON)
	{
		next();
		return NULL;
	}

	return NULL;
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
			if (right == NULL)
				OnSyntaxError("invalid assignment expression");
			else
				// ���������� ���� ������������
				return new BinaryOpNode(left, op, right);
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
	left = parseLogicalOrExpression(left);
	if (left != NULL)
	{
		// ��� �������� ���������
		if (lex == LEX_QUESTION)
		{
			next();
			Node* first = parseExpression();
			if (first == NULL)
				OnSyntaxError("invalid conditional expression");
			if (lex != LEX_COLON)
				OnSyntaxError("invalid conditional expression");
			next();
			Node* second = parseConditionalExpression(NULL);
			if (second == NULL)
				OnSyntaxError("invalid conditional expression");
			left = new ConditionalNode(left, first, second);
		}
	}
	return left;
}

// ��������� ���������� ���
Node* Parser::parseLogicalOrExpression(Node* left)
{
	left = parseLogicalAndExpression(left);
	if (left != NULL)
	{
		while (lex == LEX_LOGICAL_OR)
		{
			Lexeme op = lex;
			next();
			Node* right = parseLogicalAndExpression(NULL);
			if (right == NULL)
				OnSyntaxError("invalid logical-or expression");
			left = new BinaryOpNode(left, op, right);
		}
	}
	return left;
}

// ��������� ���������� �
Node* Parser::parseLogicalAndExpression(Node* left)
{
	left = parseInclusiveOrExpression(left);
	if (left != NULL)
	{
		while (lex == LEX_LOGICAL_AND)
		{
			Lexeme op = lex;
			next();
			Node* right = parseInclusiveOrExpression(NULL);
			if (right == NULL)
				OnSyntaxError("invalid logical-and expression");
			left = new BinaryOpNode(left, op, right);
		}
	}
	return left;
}

// ��������� ��������� ���
Node* Parser::parseInclusiveOrExpression(Node* left)
{
	left = parseExclusiveOrExpression(left);
	if (left != NULL)
	{
		while (lex == LEX_OR)
		{
			Lexeme op = lex;
			next();
			Node* right = parseExclusiveOrExpression(NULL);
			if (right == NULL)
				OnSyntaxError("invalid inclusive-or expression");
			left = new BinaryOpNode(left, op, right);
		}
	}
	return left;
}

// ��������� ��������� ����������� ���
Node* Parser::parseExclusiveOrExpression(Node* left)
{
	left = parseAndExpression(left);
	if (left != NULL)
	{
		while (lex == LEX_XOR)
		{
			Lexeme op = lex;
			next();
			Node* right = parseAndExpression(NULL);
			if (right == NULL)
				OnSyntaxError("invalid exclusive-or expression");
			left = new BinaryOpNode(left, op, right);
		}
	}
	return left;
}

// ��������� ��������� �
Node* Parser::parseAndExpression(Node* left)
{
	left = parseEqualityExpression(left);
	if (left != NULL)
	{
		while (lex == LEX_AND)
		{
			Lexeme op = lex;
			next();
			Node* right = parseEqualityExpression(NULL);
			if (right == NULL)
				OnSyntaxError("invalid and expression");
			left = new BinaryOpNode(left, op, right);
		}
	}
	return left;
}

// ��������� ���������
Node* Parser::parseEqualityExpression(Node* left)
{
	left = parseRelationalExpression(left);
	if (left != NULL)
	{
		while (lex == LEX_EQUAL || lex == LEX_NOT_EQUAL)
		{
			Lexeme op = lex;
			next();
			Node* right = parseRelationalExpression(NULL);
			if (right == NULL)
				OnSyntaxError("invalid equality expression");
			left = new BinaryOpNode(left, op, right);
		}
	}
	return left;
}

// ��������� ���������
Node* Parser::parseRelationalExpression(Node* left)
{
	left = parseShiftExpression(left);
	if (left != NULL)
	{
		while (lex == LEX_LESS || lex == LEX_LESS_EQUAL || lex == LEX_GREAT_EQUAL || lex == LEX_GREAT)
		{
			Lexeme op = lex;
			next();
			Node* right = parseShiftExpression(NULL);
			if (right == NULL)
				OnSyntaxError("invalid relation expression");
			left = new BinaryOpNode(left, op, right);
		}
	}
	return left;
}

// ��������� �����
Node* Parser::parseShiftExpression(Node* left)
{
	left = parseAdditiveExpression(left);
	if (left != NULL)
	{
		while (lex == LEX_LSHIFT || lex == LEX_RSHIFT)
		{
			Lexeme op = lex;
			next();
			Node* right = parseAdditiveExpression(NULL);
			if (right == NULL)
				OnSyntaxError("invalid shift expression");
			left = new BinaryOpNode(left, op, right);
		}
	}
	return left;
}

// �������� ��������
Node* Parser::parseAdditiveExpression(Node* left)
{
	left = parseMultiplicativeExpression(left);
	if (left != NULL)
	{
		while (lex == LEX_ADD || lex == LEX_SUB)
		{
			Lexeme op = lex;
			next();
			Node* right = parseMultiplicativeExpression(NULL);
			if (right == NULL)
				OnSyntaxError("invalid additive expression");
			left = new BinaryOpNode(left, op, right);
		}
	}
	return left;
}

// ��������� ���������
Node* Parser::parseMultiplicativeExpression(Node* left)
{
	if (left == NULL)
		left = parseCastExpression();
	if (left != NULL)
	{
		while (lex == LEX_MUL || lex == LEX_DIV || lex == LEX_PERSENT)
		{
			Lexeme op = lex;
			next();
			Node* right = parseCastExpression();
			if (right == NULL)
				OnSyntaxError("invalid multiplicative expression");
			left = new BinaryOpNode(left, op, right);
		}
	}
	return left;
}

// ��������� ���������� ����
Node* Parser::parseCastExpression()
{
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
				OnSyntaxError("invalid cast expression");
			next();
			// ��������� ���������� ���������
			Node* right = parseCastExpression();
			if (right == NULL)
				OnSyntaxError("invalid cast expression");
			return new BinaryOpNode(NULL, op, right);
		}
		// ��� �� ���, ������ �������
		back();
	}
	
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
		Node* node = parseUnaryExpression();
		if (node == NULL)
			OnSyntaxError("invalid unary expression");
		return new UnaryOpNode(node, op, false);
	}
	else if (isUnaryOperator())
	{
		Lexeme op = lex;
		next();
		Node* node = parseCastExpression();
		if (node == NULL)
			OnSyntaxError("invalid unary expression");
		return new UnaryOpNode(node, op, false);
	}
	else if (lex == LEX_SIZEOF)
	{
		Lexeme op = lex;
		next();
		Node* node = parseUnaryExpression();
		if (node == NULL)
			OnSyntaxError("invalid unary expression");
		return new UnaryOpNode(node, op, false);
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
				next();
				Node* idx = parseExpression();
				if (idx == NULL)
					OnSyntaxError("invalid array index");
				if (lex != LEX_RBRACKET)
					OnSyntaxError("invalid array index");
				next();
				left = new ArrayNode(left, idx);
			}
			// ���� �����
			else if (lex == LEX_LPAREN)
			{
				next();
				FuncCallNode* fnode = new FuncCallNode(left);
				left = fnode;
				// ������ ����������
				while (true)
				{
					Node* arg = parseAssignmentExpression();
					fnode->addArgument(arg);
					if (lex != LEX_COMMA)
						break;
					next();
				}
				// ����������� ������
				if (lex != LEX_RPAREN)
					OnSyntaxError("invalid function call");				
				next();
			}
			// ���� ��������� � �������� ���������
			else if (lex == LEX_DOT || lex == LEX_ARROW)
			{
				Lexeme op = lex;
				next();
				if (lex != LEX_ID)
					OnSyntaxError("expected identifier");
				left = new BinaryOpNode(left, op, new IdentifierNode(lex));
				next();
			}
			else if (lex == LEX_INCREMENT || lex == LEX_DECREMENT)
			{
				left = new UnaryOpNode(left, lex, true);
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
		result = new IdentifierNode(lex);
		next();
	}
	else if (lex == LEX_INT_VALUE || lex == LEX_FLOAT_VALUE || lex == LEX_CHAR_VALUE)
	{
		result = new ValueNode(lex);
		next();
	}
	else if (lex == LEX_STRING_VALUE)
	{
		result = new ValueNode(lex);
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
			if (lex != LEX_RPAREN)
				OnSyntaxError("expected close parentness");
			next();
		}
	}
	return result;
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
		if (type == NULL)
			OnSyntaxError("expected type definition");
		if (lex != LEX_ID)
			OnSyntaxError("expected typedef identifier");
		if (findSymbolByName(lex.text.c_str()) != NULL)
			OnSyntaxError("identifier already used");
		type = (TypeSymbol*)findOrAddSymbol(new AliasSymbol(type, lex.text));
		next();
	}
	// ��� ������ ���
	else if (lex == LEX_VOID)
	{
		next();
		type = (TypeSymbol*)findOrAddSymbol(new TypeSymbol("void", 0));
	}
	// ��� ���������� ���
	else if (lex == LEX_CHAR)
	{
		next();
		type = (TypeSymbol*)findOrAddSymbol(new TypeSymbol("char", 1));
	}
	// ��� ����� ���
	else if (lex == LEX_INT)
	{
		next();
		type = (TypeSymbol*)findOrAddSymbol(new TypeSymbol("int", 4));
	}
	// ��� ������������ ���
	else if (lex == LEX_FLOAT)
	{
		next();
		type = (TypeSymbol*)findOrAddSymbol(new TypeSymbol("float", 4));
	}	
	// ��������� ���������
	else if (lex == LEX_STRUCT)
	{
		type = parseStruct();
	}
	// ��� ������������� - �������� ��� ����
	else if (lex == LEX_ID)
	{
		type = (TypeSymbol*)findSymbolByName(lex.text);
		if (type != NULL)
			next();
		else
		{
			// ��� �� ���, � ���-�� ������
			back();
			return NULL;
		}
	}
	// ��� ����������� ���
	else if (lex == LEX_CONST)
	{		
		next();
		type = parseType();
		if (type == NULL)
			OnSyntaxError("incomplete type");
		if (type->isConst())
			OnSyntaxError("double \'const\'");
		if (type->isVoid())
			OnSyntaxError("incorrect type \'const void\'");
		type = (TypeSymbol*)findOrAddSymbol(new TypeSymbol(type, TypeSymbol::MODE_CONST));
	}		

	// ���� const ����� ����� �������� ����
	if (type != NULL && lex == LEX_CONST)
	{
		while (lex == LEX_CONST)
		{
			next();
			if (type->isConst())
				OnSyntaxError("double \'const\'");
			if (type->isVoid())
				OnSyntaxError("incorrect type \'const void\'");
			type = (TypeSymbol*)findOrAddSymbol(new TypeSymbol(type, TypeSymbol::MODE_CONST));
		}
	}


	return type;
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
			type = (TypeSymbol*)findOrAddSymbol(new TypeSymbol(type, TypeSymbol::MODE_POINTER));
	
			while (lex == LEX_CONST)
			{
				next();
				if (type->isConst())
					OnSyntaxError("double \'const\'");
				type = (TypeSymbol*)findOrAddSymbol(new TypeSymbol(type, TypeSymbol::MODE_CONST));
			}
		}
	}

	return type;
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

// ��������� �����������
ItemSymbol* Parser::parseDeclarator(TypeSymbol* type)
{
	ItemSymbol* item = NULL;
	type = parsePointer(type);
	Lexeme name;
	if (lex == LEX_ID)
	{
		name = lex;
		next();
	}
	else if (lex == LEX_LPAREN)
	{
		item = parseDeclarator(type);
		if (lex != LEX_RPAREN)
			OnSyntaxError("expected \')\'");
		next();
	}

	// ��� ������
	if (lex == LEX_LBRACKET)
	{
		while (lex == LEX_LBRACKET)
		{
			next();
			// ���� ��� �� ����������, �� ������
			if (item != NULL && dynamic_cast<VariableSymbol*>(item) == NULL)
				OnSyntaxError("invalid array declaration");
			// ��������� ������ �������
			Node* node = parseConditionalExpression(NULL);
			// ��������� ���������
			ConstExpressionCalc calc;
			if (!calc.calculate(node))
				OnSyntaxError(calc.errorMsg().c_str());
			int count = calc.asInt();
			// ������������ ���
			type = (TypeSymbol*)findOrAddSymbol(new ArraySymbol(type, count));
			if (item == NULL)
			{
				if (findSymbolByName(name.text, false))
					OnSyntaxError("name already used");
				item = new VariableSymbol(name.text, type);
				addSymbol(item);
			}
			else
				item->type = type;
			// ��������� ����������� ������
			if (lex != LEX_RBRACKET)
				OnSyntaxError("expected \']\'");
			next();
		}
	}
	else if (lex == LEX_LPAREN)
	{
		next();

		// ��� �������
		if (item != NULL)
			OnSyntaxError("invalid function declaration");
		if (findSymbolByName(name.text))
			OnSyntaxError("name already used");
		item = new FunctionSymbol(name.text, type);
		addSymbol(item);

		// ��������� ��������� �������

		if (lex != LEX_RPAREN)
			OnSyntaxError("expected \')\'");
		next();
	}
	else if (item == NULL)
	{
		if (findSymbolByName(name.text, false))
			OnSyntaxError("name already used");
		item = new VariableSymbol(name.text, type);
		addSymbol(item);
	}

	return item;
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

// ����� �������
Symbol* Parser::findSymbolByName(std::string name, bool forceAll)
{
	// ����� �� ���� ��������
	if (forceAll)
	{
		for (int i = tables.size() - 1; i >= 0; i--)
		{
			Symbol* s = tables[i]->findByName(name);
			if (s != NULL)
				return s;
		}
	}
	else
	{
		// ����� � ������� �������
		return tables.back()->findByName(name);
	}

	return NULL;
}

// �����  ������� ��� ��������
Symbol* Parser::findOrAddSymbol(Symbol* symbol, bool forceAll)
{
	// ����� �������
	Symbol* s = findSymbolByName(symbol->name, forceAll);
	if (s != NULL)
		return s;
	// �������� ������
	tables.back()->addSymbol(symbol);
	return symbol;
}

// �������� ������
void Parser::addSymbol(Symbol* symbol)
{
	// �������� ������
	tables.back()->addSymbol(symbol);
}

// ��������� � �������������� ������
void Parser::OnSyntaxError(const char* msg)
{
	char c[1000];
	sprintf(c, "Syntax error: %d:%d %s", lex.line, lex.col, msg);
	throw std::exception(c);
}