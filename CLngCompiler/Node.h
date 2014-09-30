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
	// Деструктор
	virtual ~Node();
	// Вывести узел на экран
	virtual void print();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
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
	// Вывести узел на экран
	virtual void print();
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
	// Вывести узел на экран
	virtual void print();
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
	// Вывести узел на экран
	virtual void print();
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
	// Конструктор
	BinaryOpNode(Node* left, Lexeme op, Node* right);
	// Деструктор
	virtual ~BinaryOpNode();
	// Вывести узел на экран
	virtual void print();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Левый узел
	Node* left;
	// Правый узел
	Node* right;
	// Операция
	Lexeme op;
};

//-------------------------------------------------------------------------

// Класс унарной операции
class UnaryOpNode : public Node
{
public:
	// Конструктор
	UnaryOpNode(Node* node, Lexeme op, bool postfix);
	// Деструктор
	virtual ~UnaryOpNode();
	// Вывести узел на экран
	virtual void print();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Выражение
	Node* node;
	// Операция
	Lexeme op;
	// Постфикс
	bool postfix;
};

//-------------------------------------------------------------------------

// Класс константы
class ValueNode : public Node
{
public:
	// Конструктор
	ValueNode(Lexeme value);
	// Деструктор
	virtual ~ValueNode();
	// Вывести узел на экран
	virtual void print();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Имя переменной
	Lexeme value;
};

//-------------------------------------------------------------------------

// Класс идентификатора
class IdentifierNode : public Node
{
public:
	// Конструктор
	IdentifierNode(Lexeme name);
	// Деструктор
	virtual ~IdentifierNode();
	// Вывести узел на экран
	virtual void print();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Идентификатор
	Lexeme name;
};

//-------------------------------------------------------------------------

// Класс обращения к элементу массива
class ArrayNode : public Node
{
public:
	// Конструктор
	ArrayNode(Node* arr, Node* idx);
	// Деструктор
	virtual ~ArrayNode();
	// Вывести узел на экран
	virtual void print();
	// Посетить узел
	virtual void visit(INodeVisitor* visitor);
	// Массив
	Node* arr;
	// индекс массива
	Node* idx;
};

//-------------------------------------------------------------------------

// Класс вызова функции
class FuncCallNode : public Node
{
public:
	// Конструктор
	FuncCallNode(Node* func);
	// Деструктор
	virtual ~FuncCallNode();
	// Вывести узел на экран
	virtual void print();
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
