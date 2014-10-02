#pragma once

#include "Symbols.h"
#include "Nodes.h"

class Printer : public ISymbolVisitor, public INodeVisitor
{
public:
	// Конструктор
	Printer();
	// деструктор
	virtual ~Printer();

	// Посетить таблицу символов
	virtual void OnSymbol(SymbolsTable* table);
	// Посетить символ
	virtual void OnSymbol(Symbol* symbol);
	// Посетить тип
	virtual void OnSymbol(TypeSymbol* symbol);
	// Посетить псевдоним
	virtual void OnSymbol(AliasSymbol* symbol);
	// Посетить структуру
	virtual void OnSymbol(StructSymbol* symbol);
	// Посетить массив
	virtual void OnSymbol(ArraySymbol* symbol);
	// Посетить символ
	virtual void OnSymbol(ItemSymbol* symbol);
	// Посетить константу
	virtual void OnSymbol(ConstantSymbol* symbol);
	// Посетить переменную
	virtual void OnSymbol(VariableSymbol* symbol);
	// Посетить функцию
	virtual void OnSymbol(FunctionSymbol* symbol);

	// Посетить узел
	virtual void OnNode(Node* node);
	// Посетить узел программы
	virtual void OnNode(ProgramNode* node);
	// Посетить узел выражения
	virtual void OnNode(ExpressionNode* node);
	// Посетить узел условного выраженрия
	virtual void OnNode(ConditionalNode* node);
	// Посетить узел бинарной операции
	virtual void OnNode(BinaryOpNode* node);
	// Посетить узел унарной операции
	virtual void OnNode(UnaryOpNode* node);	
	// Посетить узел значения
	virtual void OnNode(ValueNode* node);
	// Посетить узел идентификатора
	virtual void OnNode(IdentifierNode* node);
	// Посетить узел обращения к массиву
	virtual void OnNode(ArrayNode* node);
	// Посетить узел вызова функции
	virtual void OnNode(FuncCallNode* node);

private:
	// Отступ
	int spaces;
	// Счетчик таблиц
	int tableCount;
	// счетчик деревьев
	int treeCount;
	// Вывод короткого названия 
	bool shortName;
};

