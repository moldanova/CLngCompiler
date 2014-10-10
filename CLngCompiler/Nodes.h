#pragma once

#include "Lexeme.h"
#include "Symbols.h"

class INodeVisitor;

//-------------------------------------------------------------------------

// Класс узла синтаксического дерева
class Node
{
public:
	// Конструктор
	Node();
	// Конструктор
	Node(Symbol* symbol);
	// Конструктор
	Node(Lexeme lex, Symbol* symbol);
	// Деструктор
	virtual ~Node();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Получить тип значения
	TypeSymbol* getType();
	// Признак пустого узла
	virtual bool isEmpty() { return false; }
	// Лексема узла
	Lexeme lex;
	// Тип узла
	Symbol* symbol;
};

//-------------------------------------------------------------------------

// Класс узлаЮ который может содержать массив сложенных узлов
class NodesArrayNode : public Node
{
public:
	// Конструктор
	NodesArrayNode();
	// Конструктор
	NodesArrayNode(Symbol* symbol);
	// Конструктор
	NodesArrayNode(Lexeme lex, Symbol* symbol);
	// Деструктор
	virtual ~NodesArrayNode();
	// Добавить узел
	void addNode(Node* node);
	// Массив узлов программы
	std::vector<Node*> nodes;
};

//-------------------------------------------------------------------------

// Класс пустого узла. Этот класс нужен, чтобы вернуть тип значения
class EmptyNode : public Node
{
public:
	// Конструктор
	EmptyNode(Symbol* symbol) : Node(symbol) {}
	// Признак пустого узла
	virtual bool isEmpty() { return true; }
};

//-------------------------------------------------------------------------

// Класс узла программы
class ProgramNode : public NodesArrayNode
{
public:
	// Конструктор
	ProgramNode();
	// Деструктор
	virtual ~ProgramNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// таблица глобальных символов
	SymbolsTable globals;
};

//-------------------------------------------------------------------------

// Класс выражения
class ExpressionNode : public NodesArrayNode
{
public:
	// Конструктор
	ExpressionNode(Node* node);
	// Деструктор
	virtual ~ExpressionNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
};

//-------------------------------------------------------------------------

// Условное выражение
class ConditionalNode : public Node
{
public:
	// Конструктор
	ConditionalNode(Node* left, Node* first, Node* second);
	// Деструктор
	virtual ~ConditionalNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Левый узел
	Node* left;
	// первый узел
	Node* first;
	// второй узел
	Node* second;
};

//-------------------------------------------------------------------------

// Класс бинарной операции
class BinaryOpNode : public Node
{
public:
	// Вычислить тип результата
	static TypeSymbol* makeType(Lexeme lex, Node* left, Node* right);
	// Конструктор
	BinaryOpNode(Lexeme lex, Symbol* symbol, Node* left, Node* right);
	// Деструктор
	virtual ~BinaryOpNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Левый узел
	Node* left;
	// Правый узел
	Node* right;	
};

//-------------------------------------------------------------------------

// Класс унарной операции
class UnaryOpNode : public Node
{
public:
	// Вычислить тип результата
	static TypeSymbol* makeType(Lexeme lex, Node* right, bool postfix);
	// Конструктор
	UnaryOpNode(Lexeme lex, Symbol* symbol,  Node* right, bool postfix);
	// Деструктор
	virtual ~UnaryOpNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Выражение
	Node* right;	
	// Постфикс
	bool postfix;
};

//-------------------------------------------------------------------------

// Класс константы
class ValueNode : public Node
{
public:
	// Конструктор
	ValueNode(Lexeme lex, Symbol* symbol);
	// Деструктор
	virtual ~ValueNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
};

//-------------------------------------------------------------------------

// Класс идентификатора
class IdentifierNode : public Node
{
public:
	// Конструктор
	IdentifierNode(Lexeme lex, Symbol* symbol);
	// Деструктор
	virtual ~IdentifierNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
};

//-------------------------------------------------------------------------

// Класс обращения к элементу массива
class ArrayNode : public Node
{
public:
	// Конструктор
	ArrayNode(Symbol* symbol, Node* var, Node* idx);
	// Деструктор
	virtual ~ArrayNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Массив
	Node* var;
	// индекс массива
	Node* idx;
};

//-------------------------------------------------------------------------

// Класс вызова функции
class FuncCallNode : public NodesArrayNode
{
public:
	// Конструктор
	FuncCallNode(Symbol* symbol, Node* func);
	// Деструктор
	virtual ~FuncCallNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Узел функции
	Node* func;
};

//-------------------------------------------------------------------------

// набора операторов
class CompoundNode : public NodesArrayNode
{
public:
	// Конструктор
	CompoundNode();
	// Деструктор
	virtual ~CompoundNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Список определений
	SymbolsTable locals;
};

//-------------------------------------------------------------------------

// Класс функции
class FunctionNode : public Node
{
public:
	// Конструктор
	FunctionNode(Symbol* symbol);
	// Деструктор
	virtual ~FunctionNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Список параметров
	SymbolsTable params;
	// Тело функции
	Node* statement;
};

//-------------------------------------------------------------------------

// Класс условного оператора
class IfNode : public Node
{
public:
	// Конструктор
	IfNode(Node* expr, Node* statement1, Node* statement2);
	// Деструктор
	virtual ~IfNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Условный оператор
	Node* expr;
	// операторы
	Node* statement1;
	Node* statement2;
};

//-------------------------------------------------------------------------

// Класс цикла
class WhileNode : public Node
{
public:
	// Конструктор
	WhileNode(Node* expr, Node* statement);
	// Деструктор
	virtual ~WhileNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Условный оператор
	Node* expr;
	// операторы
	Node* statement;
};

//-------------------------------------------------------------------------

// Класс цикла
class DoNode : public Node
{
public:
	// Конструктор
	DoNode(Node* expr, Node* statement);
	// Деструктор
	virtual ~DoNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Условный оператор
	Node* expr;
	// операторы
	Node* statement;
};

//-------------------------------------------------------------------------

// Класс цикла
class ForNode : public Node
{
public:
	// Конструктор
	ForNode(Node* expr1, Node* expr2, Node* expr3, Node* statement);
	// Деструктор
	virtual ~ForNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Условный оператор
	Node* expr1;
	Node* expr2;
	Node* expr3;
	// операторы
	Node* statement;
};

//-------------------------------------------------------------------------

class BreakNode : public Node
{
public:
	// Конструктор
	BreakNode();
	// Деструктор
	virtual ~BreakNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
};

//-------------------------------------------------------------------------

class ContinueNode : public Node
{
public:
	// Конструктор
	ContinueNode();
	// Деструктор
	virtual ~ContinueNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
};

//-------------------------------------------------------------------------

class ReturnNode : public Node
{
public:
	// Конструктор
	ReturnNode(Node* expr);
	// Деструктор
	virtual ~ReturnNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Возвращаемое значение
	Node* expr;
};

//-------------------------------------------------------------------------

// Интерфейс посетителя для узлов дерева
class INodeVisitor
{
public:
	// Посетить узел
	virtual void OnNode(Node* node) = 0;
	// Посетить узел программы
	virtual void OnNode(ProgramNode* node) = 0;
	// Посетить узел выражения
	virtual void OnNode(ExpressionNode* node) = 0;	
	// Посетить узел условного выраженрия
	virtual void OnNode(ConditionalNode* node) = 0;
	// Посетить узел бинарной операции
	virtual void OnNode(BinaryOpNode* node) = 0;
	// Посетить узел унарной операции
	virtual void OnNode(UnaryOpNode* node) = 0;	
	// Посетить узел значения
	virtual void OnNode(ValueNode* node) = 0;
	// Посетить узел идентификатора
	virtual void OnNode(IdentifierNode* node) = 0;
	// Посетить узел обращения к массиву
	virtual void OnNode(ArrayNode* node) = 0;
	// Посетить узел вызова функции
	virtual void OnNode(FuncCallNode* node) = 0;
	// Посетить узел
	virtual void OnNode(CompoundNode* node) = 0;
	// Посетить узел функции
	virtual void OnNode(FunctionNode* node) = 0;
	// Посетить узел
	virtual void OnNode(IfNode* node) = 0;
	// Посетить узел
	virtual void OnNode(WhileNode* node) = 0;
	// Посетить узел
	virtual void OnNode(DoNode* node) = 0;
	// Посетить узел
	virtual void OnNode(ForNode* node) = 0;
	// Посетить узел
	virtual void OnNode(BreakNode* node) = 0;
	// Посетить узел
	virtual void OnNode(ContinueNode* node) = 0;
	// Посетить узел
	virtual void OnNode(ReturnNode* node) = 0;
};
