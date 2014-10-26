#include "stdafx.h"
#include "CodeGenerator.h"

// Конструктор
CodeGenerator::CodeGenerator()
{

}

// Деструктор
CodeGenerator::~CodeGenerator()
{

}

// Генерировать код
AsmProg* CodeGenerator::generate(ProgramNode* prog)
{
	asmProg = new AsmProg();

	// Транслируем глобальные переменные
	prog->globals.visit(this);

	// Транслируем функции
	for (int i = 0; i < prog->nodes.size(); i++)
	{
		FunctionNode* fnode = dynamic_cast<FunctionNode*>(prog->nodes[i]);
		if (fnode)
			fnode->visit(this);
	}

	// Транслируем весь остальной код
	add(cmdLABEL, "start");
	for (int i = 0; i < prog->nodes.size(); i++)
	{
		Node* node = prog->nodes[i];
		FunctionNode* fnode = dynamic_cast<FunctionNode*>(node);
		if (!fnode)
			node->visit(this);
	}
	// Добавляем вызов функции main
	add(cmdCALL, "f_main");
	// Выход из программы
	add(cmdRET, makeArg(argVAL, "0"));

	return asmProg;
}

// Посетить таблицу символов
void CodeGenerator::OnSymbol(SymbolsTable* table)
{
	for (int i = 0; i < table->symbols.size(); i++)
	{
		Symbol* symbol = table->symbols[i];
		VariableSymbol* var = dynamic_cast<VariableSymbol*>(symbol);
		if (var)
			addVar("var_" + var->name, varDD, var->type->getLength() / 4);
	}
}

// Посетить символ
void CodeGenerator::OnSymbol(Symbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить тип
void CodeGenerator::OnSymbol(TypeSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить псевдоним
void CodeGenerator::OnSymbol(AliasSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить структуру
void CodeGenerator::OnSymbol(StructSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить массив
void CodeGenerator::OnSymbol(ArraySymbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить символ
void CodeGenerator::OnSymbol(ItemSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить константу
void CodeGenerator::OnSymbol(ConstantSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить переменную
void CodeGenerator::OnSymbol(VariableSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить функцию
void CodeGenerator::OnSymbol(FunctionSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить узел
void CodeGenerator::OnNode(Node* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел программы
void CodeGenerator::OnNode(ProgramNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел выражения
void CodeGenerator::OnNode(ExpressionNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел условного выраженрия
void CodeGenerator::OnNode(ConditionalNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел бинарной операции
void CodeGenerator::OnNode(BinaryOpNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел унарной операции
void CodeGenerator::OnNode(UnaryOpNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел значения
void CodeGenerator::OnNode(ValueNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел идентификатора
void CodeGenerator::OnNode(IdentifierNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел обращения к массиву
void CodeGenerator::OnNode(ArrayNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел вызова функции
void CodeGenerator::OnNode(FuncCallNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел
void CodeGenerator::OnNode(CompoundNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел функции
void CodeGenerator::OnNode(FunctionNode* node)
{
	// Название функции
	std::string name = "f_" + node->symbol->name;
	// Метка и пролог
	add(cmdLABEL, name);
	add(cmdPUSH, makeArg(argEBP));
	add(cmdMOV, makeArg(argEBP), makeArg(argESP));

	// Выделяем память под локальные переменные

	// Транслируем тело функции
	//node->statement->visit(this);

	// Окончание функции
	add(cmdLABEL, name + "_end");
	add(cmdMOV, makeArg(argESP), makeArg(argEBP));
	add(cmdPOP, makeArg(argEBP));
	add(cmdRET, makeArg(argVAL, "0"));
}

// Посетить узел
void CodeGenerator::OnNode(IfNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел
void CodeGenerator::OnNode(WhileNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел
void CodeGenerator::OnNode(DoNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел
void CodeGenerator::OnNode(ForNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел
void CodeGenerator::OnNode(BreakNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел
void CodeGenerator::OnNode(ContinueNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел
void CodeGenerator::OnNode(ReturnNode* node)
{
	throw std::exception("Internal program error");
}

// Добавить команду
void CodeGenerator::add(int cmd)
{
	asmProg->addCmd(new AsmCmd(cmd));
}

// добавить команду
void CodeGenerator::add(int cmd, AsmArg* arg1)
{
	asmProg->addCmd(new AsmCmd(cmd, arg1));
}

// Добавить команду
void CodeGenerator::add(int cmd, AsmArg* arg1, AsmArg* arg2)
{
	asmProg->addCmd(new AsmCmd(cmd, arg1, arg2));
}

// Добавить команду
void CodeGenerator::add(int cmd, std::string name)
{
	asmProg->addCmd(new AsmCmd(cmd, name));
}

// Создать аргумент
AsmArg* CodeGenerator::makeArg(int arg)
{
	return new AsmArg(arg);
}

// Создать аргумент
AsmArg* CodeGenerator::makeArg(int arg, std::string val)
{
	return new AsmArg(arg, val);
}

// добавить переменную
void CodeGenerator::addVar(std::string name, int type, std::string value)
{
	asmProg->addVar(new AsmVar(name, type, value));
}

// добавить переменную
void CodeGenerator::addVar(std::string name, int type, int count)
{
	asmProg->addVar(new AsmVar(name, type, count));
}