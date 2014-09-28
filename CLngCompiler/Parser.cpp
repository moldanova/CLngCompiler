#include "stdafx.h"
#include "Parser.h"

// Конструктор
Parser::Parser(Lexer& lexer)
	: lexer(lexer)
{

}

// Деструктор
Parser::~Parser(void)
{

}

// Выполнить анализ
void Parser::parse()
{
	if (lex == -1)
		next();

	while (lex != LEX_EOF)
	{
		// Получаем единицу трансляции
		Node* node = parseFunction();
		if (node != NULL)
			prog.addNode(node);
	}
}

// Вернуть признак ошибок
bool Parser::hasErrors()
{
	return false;
}

// Разобрать выражение
Node* Parser::parseExpression()
{
	ExpressionNode* result = NULL;
	if (lex == -1)
		next();

	while (true)
	{
		// разбираем выражение
		Node* n = parseAssignmentExpression();
		// Если это не выражение, то выход
		if (result == NULL && n == NULL)
			return NULL;
		// Создаем выражение и добавляем узел
		if (result == NULL)
			result = new ExpressionNode(n);
		else if (n == NULL)
			OnSyntaxError("invalid expression");
		else
			result->addAssignment(n);
		// Если запятая, то получить следующую лексему
		// и разобрать следующее выражение
		if (lex == LEX_COMMA)
			next();
		else
		{
			// иначе разбор окончен
			commit();
			break;
		}
	}
	return result;
}

// Разобраиь присваивание
Node* Parser::parseAssignmentExpression()
{
	// разбираем унарный оператор
	Node* left = parseUnaryExpression();
	// Если удалось
	if (left != NULL)
	{
		// Проверяем оператор
		if (isAssignmentOperator())
		{
			// Запоминаем код оператора
			Lexeme op = lex;
			next();
			// Получаем выражение с правой стороны
			Node* right = parseAssignmentExpression();
			// Если не удалось, то ошибка
			if (right == NULL)
				OnSyntaxError("invalid assignment expression");
			else
				// Возвращаем узел присваивания
				return new BinaryOpNode(left, op, right);
		}
		else
		{
			// Это другая операция
			return parseConditionalExpression(left);
		}
	}
	else
		// Это был не унарный оператор
		return parseConditionalExpression(NULL);

	return left;
}

// Разобрать условное выражение
Node* Parser::parseConditionalExpression(Node* left)
{
	// Получаем операнд слева
	left = parseLogicalOrExpression(left);
	if (left != NULL)
	{
		// Это условное выражение
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

// Разобрать логическое ИЛИ
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

// Разобрать логическое И
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

// Разобрать побитовое ИЛИ
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

// Разобрать побитовое исключающее ИЛИ
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

// Разобрать побитовое И
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

// Разобрать равенство
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

// разобрать сравнение
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

// разобрать сдвиг
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

// разобать сложение
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

// Разобрать умножение
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

// Разобрать приведение типа
Node* Parser::parseCastExpression()
{
	if (lex == LEX_LPAREN)
	{
		Lexeme op = lex;
		next();
		// Если тип. то разбираем приведение типа
		if (isTypeName())
		{
			// Запоминаем тип
			Lexeme type = lex;
			next();
			if (lex != LEX_RPAREN)
				OnSyntaxError("invalid cast expression");
			next();
			// разбираем приводимое выражение
			Node* right = parseCastExpression();
			if (right == NULL)
				OnSyntaxError("invalid cast expression");
			return new BinaryOpNode(NULL, op, right);
		}
		// Это не тип, делаем возврат
		back();
	}
	
	// разбираем унарное выражение
	return parseUnaryExpression();
}

// разобрать унарные операции
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

// Разобрать постфиксные операции
Node* Parser::parsePostfixExpression()
{
	Node* left = parsePrimaryExpression();
	if (left != NULL)
	{
		while (true)
		{
			// Если обращение к элементу массива
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
			// Если вызов
			else if (lex == LEX_LPAREN)
			{
				next();
				FuncCallNode* fnode = new FuncCallNode(left);
				left = fnode;
				// Разбор аргументов
				while (true)
				{
					Node* arg = parseAssignmentExpression();
					fnode->addArgument(arg);
					if (lex != LEX_COMMA)
						break;
					next();
				}
				// Закрывающая скобка
				if (lex != LEX_RPAREN)
					OnSyntaxError("invalid function call");				
				next();
			}
			// Если обращение к элементу структуры
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

// Разобрать операнд
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
		result = new ConstantValue(lex);
		next();
	}
	else if (lex == LEX_STRING_VALUE)
	{
		result = new ConstantValue(lex);
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

// Разобрать функцию
Node* Parser::parseFunction()
{
	TypeSymbol* type = parseType();

	return NULL;
}

// разобрать тип
TypeSymbol* Parser::parseType()
{
	TypeSymbol* type = NULL;
	if (lex == LEX_TYPEDEF)
	{
		next();
		type = parseType();
		if (type == NULL)
			OnSyntaxError("invalid typedef");
		if (lex != LEX_ID)
			OnSyntaxError("expected typedef identifier");
		if (symbols.findByName(lex.text.c_str()) != NULL)
			OnSyntaxError("identifier already used");
		type = new AliasSymbol(type, lex.text);
		next();
		symbols.addSymbol(type);		
	}
	else if (lex == LEX_STRUCT)
	{
		type = parseStruct();
	}
	else if (lex == LEX_CONST)
	{
		// Это тип константы
		next();
		type = parseType();
		if (type->isConst() && !type->isPointer())
			OnSyntaxError("invalid type");
		type = (TypeSymbol*)symbols.findOrAdd(new TypeSymbol(type, TypeSymbol::MODE_CONST));
	}
	else if (lex == LEX_INT)
	{
		next();
		type = (TypeSymbol*)symbols.findOrAdd(new TypeSymbol("int", 4));
	}
	else if (lex == LEX_FLOAT)
	{
		next();
		type = (TypeSymbol*)symbols.findOrAdd(new TypeSymbol("float", 8));
	}
	else if (lex == LEX_CHAR)
	{
		next();
		type = (TypeSymbol*)symbols.findOrAdd(new TypeSymbol("char", 1));
	}
	else if (lex == LEX_VOID)
	{
		next();
	}	
	else
	{
		type = (TypeSymbol*)symbols.findByName(lex.text);
		if (type == NULL)
			OnSyntaxError("expected type");
	}

	return type;
}

// Разобрать структуру
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

// разобрать переменную
VariableSymbol* Parser::parseVariable(TypeSymbol* type)
{
	return NULL;
}

// Проверить, что лексема является унарной операцией
bool Parser::isUnaryOperator()
{
	return lex == LEX_AND 
		|| lex == LEX_MUL 
		|| lex == LEX_ADD 
		|| lex == LEX_SUB 
		|| lex == LEX_INVERT 
		|| lex == LEX_NOT;
}

// Проверить что лексема является присваиванием
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

// Проверить, что лексема является именем типа
bool Parser::isTypeName()
{
	//TODO
	return false;
}

// Получить следующую лексему
void Parser::next()
{
	// Помещаем текущую лексему в стек
	if (lex != -1)
		buffer.push_back(lex);

	// Если стек пуст
	if (stack.empty())
		// то запрашиваем лексему в лексере
		lex = lexer.next();
	else
	{
		// иначе берем лексему из стека
		lex = stack.back();
		stack.pop_back();
	}
}

// Вернуть лексему назад
void Parser::back()
{
	if (buffer.empty())
		throw std::exception("Internal error: back buffer is empty");

	// Помещаем лексему в стек
	stack.push_back(lex);
	// Возвращаем лексему из буфера
	lex = buffer.back();
	buffer.pop_back();
}

// Очистить буфер
void Parser::commit()
{
	buffer.clear();
}

// Сообщение о синтаксической ошибке
void Parser::OnSyntaxError(const char* msg)
{
	char c[1000];
	sprintf(c, "Syntax error: %d:%d %s", lex.line, lex.col, msg);
	throw std::exception(c);
}