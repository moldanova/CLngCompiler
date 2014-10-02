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
	// Лексема узла
	Lexeme lex;
	// Тип узла
	Symbol* symbol;
};

//-------------------------------------------------------------------------

// Класс узла программы
class ProgramNode : public Node
{
public:
	// Конструктор
	ProgramNode();
	// Деструктор
	virtual ~ProgramNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Добавить узел
	void addNode(Node* node);
	// Массив узлов программы
	std::vector<Node*> nodes;
	// таблица глобальных символов
	SymbolsTable global;
};

//-------------------------------------------------------------------------

// Класс выражения
class ExpressionNode : public Node
{
public:
	// Конструктор
	ExpressionNode(Node* node);
	// Деструктор
	virtual ~ExpressionNode();
	// Добавить узел
	void addAssignment(Node* node);
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Массив выражений
	std::vector<Node*> nodes;	
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
class FuncCallNode : public Node
{
public:
	// Конструктор
	FuncCallNode(Symbol* symbol, Node* func);
	// Деструктор
	virtual ~FuncCallNode();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Добавить узел
	void addArgument(Node* node);
	// индекс массива
	Node* func;
	// Массив выражений
	std::vector<Node*> nodes;	
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
};
