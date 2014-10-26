#pragma once

#include "Symbols.h"
#include "Nodes.h"
#include "Asm.h"

class CodeGenerator : public ISymbolVisitor, public INodeVisitor
{
public:
	// Конструктор
	CodeGenerator();
	// Деструктор
	~CodeGenerator();
	// Генерировать код
	AsmProg* generate(ProgramNode* prog);
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
	// Посетить узел
	virtual void OnNode(CompoundNode* node);
	// Посетить узел функции
	virtual void OnNode(FunctionNode* node);
	// Посетить узел
	virtual void OnNode(IfNode* node);
	// Посетить узел
	virtual void OnNode(WhileNode* node);
	// Посетить узел
	virtual void OnNode(DoNode* node);
	// Посетить узел
	virtual void OnNode(ForNode* node);
	// Посетить узел
	virtual void OnNode(BreakNode* node);
	// Посетить узел
	virtual void OnNode(ContinueNode* node);
	// Посетить узел
	virtual void OnNode(ReturnNode* node);

private:

	// Добавить команду
	void add(int cmd);
	// добавить команду
	void add(int cmd, AsmArg* arg1);
	// Добавить команду
	void add(int cmd, AsmArg* arg1, AsmArg* arg2);
	// Добавить команду
	void add(int cmd, std::string name);
	// Создать аргумент
	AsmArg* makeArg(int arg);
	// Создать аргумент
	AsmArg* makeArg(int arg, std::string val);
	// добавить переменную
	void addVar(std::string name, int type, std::string value);
	// добавить переменную
	void addVar(std::string name, int type, int count);

	// Программа на ассемблере
	AsmProg* asmProg;
};