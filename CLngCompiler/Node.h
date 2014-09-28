#pragma once

#include "Lexeme.h"

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
};

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
private:
	std::vector<Node*> nodes;	// Массив выражений
};

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
	// Выражение
	Node* node;
	// Операция
	Lexeme op;
	// Постфикс
	bool postfix;
};

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
	// Левый узел
	Node* left;
	// Правый узел
	Node* right;
	// Операция
	Lexeme op;
};

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
	// Левый узел
	Node* left;
	// первый узел
	Node* first;
	// второй узел
	Node* second;
};

// Класс константы
class ConstantValue : public Node
{
public:
	// Конструктор
	ConstantValue(Lexeme value);
	// Деструктор
	virtual ~ConstantValue();
	// Вывести узел на экран
	virtual void print();
	// Имя переменной
	Lexeme value;
};

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
	// Идентификатор
	Lexeme name;
};

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
	// Массив
	Node* arr;
	// индекс массива
	Node* idx;
};

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
	// Добавить узел
	void addArgument(Node* node);
	// индекс массива
	Node* func;
private:
	std::vector<Node*> nodes;	// Массив выражений
};

// Класс программы
class ProgramNode : public Node
{
public:
	// Конструктор
	ProgramNode();
	// Деструктор
	virtual ~ProgramNode();
	// Вывести узел на экран
	virtual void print();
	// Добавить узел
	void addNode(Node* node);
private:
	// Массив узлов программы
	std::vector<Node*> nodes;
};

