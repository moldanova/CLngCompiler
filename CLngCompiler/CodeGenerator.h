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
	void code(int cmd);
	// добавить команду
	void code(int cmd, AsmArg* arg1);
	// Добавить команду
	void code(int cmd, AsmArg* arg1, AsmArg* arg2);
	// Добавить метку
	void label(std::string name);
	// Создать аргумент
	AsmArg* valArg(std::string val);
	// Создать аргумент
	AsmArg* regArg(int reg);
	// Создать аргумент
	AsmArg* memArg(int reg, int offset = 0);
	// Создать аргумент
	AsmArg* memArg(std::string name);
	// Создать аргумент
	AsmArg* offArg(std::string name);
	// добавить значение
	void value(std::string name, std::string type, std::string value);
	// добавить переменную
	void var(std::string name, std::string type, int count=0);

	// Программа на ассемблере
	AsmProg* asmProg;
	// текущая процедура
	AsmProc* asmProc;
	// Счетчик констант
	int valCount;
	// Счетчик меток
	int labelCount;
	// Счетчик таблиц символов
	int tableCount;
	// Признак разбора параметров функции
	bool isParams;
	// Признак генерации кода для lvalue
	bool lvalue;
	// Признак использования сопросессора
	bool useFPU;
	// Стек меток для выхода из цикла
	std::vector<std::string> breakStack;
	// Стек метод для повторения цикла
	std::vector<std::string> continueStack;
	// Стек меток для выхода из функции
	std::vector<std::string> returnStack;
};