#include "stdafx.h"
#include "Parser.h"
#include "Exceptions.h"

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
		// Получаем единицу трансляции
		parseUnit(prog);

	return prog;
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
		check(n == NULL, "expected assignment expression", lex);
		result->addAssignment(n);
		// Если запятая, то получить следующую лексему
		// и разобрать следующее выражение
		if (lex == LEX_COMMA)
			next();
		else
			// иначе разбор окончен
			break;
	}
	return result;
}

// Разобрать единицу трансляции
void Parser::parseUnit(ProgramNode* prog)
{
	// Разбираем тип
	TypeSymbol* type = parseType();
	// Разбираем декларатор
	Node* left = parseDeclarator(type);
	// Если переменная
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

// разобрать тип
TypeSymbol* Parser::parseType()
{
	TypeSymbol* type = NULL;

	// разбираем псевдоним типа
	if (lex == LEX_TYPEDEF)
	{
		next();
		type = parseType();
		check(type == NULL, "expected type definition", lex);
		check(lex != LEX_ID, "expected typedef identifier", lex);
		type = addAliasSymbol(type, lex.text);
		next();
	}
	// Это пустой тип
	else if (lex == LEX_VOID)
	{
		next();
		type = addTypeSymbol("void", 0);
	}
	// Это символьный тип
	else if (lex == LEX_CHAR)
	{
		next();
		type = addTypeSymbol("char", 1);
	}
	// Это целый тип
	else if (lex == LEX_INT)
	{
		next();
		type = addTypeSymbol("int", 4);
	}
	// Это вещественный тип
	else if (lex == LEX_FLOAT)
	{
		next();
		type = addTypeSymbol("float", 4);
	}	
	// Разбираем структуру
	else if (lex == LEX_STRUCT)
	{
		type = parseStruct();
	}
	// Это идентификатор - возможно имя типа
	else if (lex == LEX_ID)
	{
		type = getTypeSymbol(lex.text);
		if (type != NULL)
			next();
		else
		{
			// Это не тип, а что-то другое
			return NULL;
		}
	}
	// Это константный тип
	else if (lex == LEX_CONST)
	{		
		next();
		type = parseType();
		check(type == NULL, "incomplete type", lex);
		check(type->isConst(), "double \'const\'", lex);
		check(type->isVoid(), "incorrect type \'const void\'", lex);
		type = addTypeSymbol(type, TypeSymbol::MODE_CONST);
	}		

	// Если const стоит после названия типа
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

// разобрать определение
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

	// это массив
	if (lex == LEX_LBRACKET)
	{
		while (lex == LEX_LBRACKET)
		{			
			if (node->symbol == NULL)
				checkSymbol(node->lex.text, false);
			else
				check(dynamic_cast<VariableSymbol*>(node->symbol) == NULL, "expected variable before \'[\'", lex);			
			next();
			// разбираем размер массива
			Node* expr = parseConditionalExpression(NULL);
			// вычисляем выражение			
			int count = 10;
			// Модифицируем тип
			type = addArraySymbol(type, count);
			if (node->symbol == NULL)
				node->symbol = addVariableSymbol(node->lex.text, type);
			else
				((ItemSymbol*)node->symbol)->type = type;
			// Проверяем закрывающую скобку
			check(lex != LEX_RBRACKET, "expected \']\'", lex);
			next();
		}
	}
	else if (lex == LEX_LPAREN)
	{
		// Это функция
		check(node->symbol != NULL, "expected identifier before \'(\'", lex);
		checkSymbol(node->lex.text);
		FunctionSymbol* fsymbol = addFunctionSymbol(node->lex.text, type);
		node->symbol = fsymbol;
		next();
		// разбираем параметры функции
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

// Разобрать указатель
TypeSymbol* Parser::parsePointer(TypeSymbol* type)
{
	// Разбираем модификаторы типа, которые могут следовать после имени типа
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
			check(right == NULL, "expected right operand", op);
			// Возвращаем узел присваивания
			return new BinaryOpNode(op, BinaryOpNode::makeType(op, left, right), left, right);
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
	left = parseOperation(left, 0);
	if (left != NULL)
	{
		// Это условное выражение
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

// Массив операций
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

// Проверить операцию
bool checkOperation(Lexeme lex, int priority)
{
	for (int i = 0; i < OPERATION_COUNT; i++)
	{
		if (operations[priority][i] == lex.code)
			return true;
	}
	return false;
}

// разобрать операцию
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

// Разобрать приведение типа
Node* Parser::parseCastExpression()
{
	/*
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
				onSyntaxError("expected \')\'");
			next();
			// разбираем приводимое выражение
			Node* right = parseCastExpression();
			if (right == NULL)
				onSyntaxError("expected operand");
			return new UnaryOpNode(UnaryOpNode::makeType(op, right), op, right, false);
		}
		// Это не тип, делаем возврат
		back();
	}
	*/
	
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
				check(!left->getType()->isArray(), "left operand of \'[\' must be array", lex);
				next();
				Node* idx = parseExpression();
				check(idx == NULL, "invalid array index", lex);
				check(lex != LEX_RBRACKET, "expected \']\'", lex);
				next();
				left = new ArrayNode(left->getType()->baseType, left, idx);
			}
			// Если вызов
			else if (lex == LEX_LPAREN)
			{			
				check(dynamic_cast<FunctionSymbol*>(left->symbol) == NULL, "left operand of \'(\' must be function", lex);
				next();
				FuncCallNode* fnode = new FuncCallNode(left->symbol, left);
				left = fnode;
				// Разбор аргументов
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
				// Закрывающая скобка
				check(lex != LEX_RPAREN, "expected \')\'", lex);
				next();
			}
			// Если обращение к элементу структуры
			else if (lex == LEX_DOT || lex == LEX_ARROW)
			{
				// Проверяем тип
				TypeSymbol* type = left->getType();
				check(lex == LEX_DOT && !type->isStruct(), "left operand of \'.\' must be struct", lex);
				if (lex == LEX_ARROW)
				{
					check(!type->isPointer() || !type->baseType->isStruct(), "left operand of \'->\' must be pointer to struct", lex);
					type = type->baseType;
				}
				Lexeme op = lex;
				next();		
				// Проверяем название поля структуры
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

// Разобрать операнд
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

// Получить следующую лексему
void Parser::next()
{
	// Помещаем текущую лексему в стек
	if (lex != -1)
	{
		buffer.push_back(lex);
		if (buffer.size() >= 1000)
			buffer.erase(buffer.begin(), buffer.begin() + 100);
	}

	// Если стек пуст
	if (backStack.empty())
		// то запрашиваем лексему в лексере
		lex = lexer.next();
	else
	{
		// иначе берем лексему из стека
		lex = backStack.back();
		backStack.pop_back();
	}
}

// Вернуть лексему назад
void Parser::back()
{
	if (buffer.empty())
		throw std::exception("Internal error: back buffer is empty");

	// Помещаем лексему в стек
	backStack.push_back(lex);
	// Возвращаем лексему из буфера
	lex = buffer.back();
	buffer.pop_back();
}

// Сообщение о синтаксической ошибке
void Parser::check(bool cond, const char* msg, Lexeme l)
{
	if (cond)
		throw SyntaxException(msg, l.line, l.col);
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

// Проверить символ
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

// найти тип
TypeSymbol* Parser::getTypeSymbol(std::string name)
{
	return dynamic_cast<TypeSymbol*>(tables.front()->findByName(name));
}

// Добавить тип
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

// Добавить тип
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

// Добавить псевдоним
AliasSymbol* Parser::addAliasSymbol(TypeSymbol* baseType, std::string name)
{
	checkSymbol(name);
	AliasSymbol* type = new AliasSymbol(baseType, name);
	tables.front()->addSymbol(type);
	return type;
}

// Добавить массив
ArraySymbol* Parser::addArraySymbol(TypeSymbol* baseType, int count)
{
	ArraySymbol* type = new ArraySymbol(baseType, count);
	tables.front()->addSymbol(type);
	return type;
}

// Получить перемнную или функцию
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

// Добавить переменную
VariableSymbol* Parser::addVariableSymbol(std::string name, TypeSymbol* type)
{
	checkSymbol(name, false);
	VariableSymbol* var = new VariableSymbol(name, type);
	tables.back()->addSymbol(var);
	return var;
}

// Добавить функцию
FunctionSymbol* Parser::addFunctionSymbol(std::string name, TypeSymbol* type)
{
	checkSymbol(name);
	FunctionSymbol* f = new FunctionSymbol(name, type);
	tables.front()->addSymbol(f);
	return f;
}
