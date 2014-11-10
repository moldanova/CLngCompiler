#include "stdafx.h"
#include "CodeGenerator.h"
#include "Flag.h"
#include "Lexer.h"

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
	valCount = 0;
	labelCount = 0;
	lvalue = false;
	useFPU = false;

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
	add("start");
	for (int i = 0; i < prog->nodes.size(); i++)
	{
		Node* node = prog->nodes[i];
		FunctionNode* fnode = dynamic_cast<FunctionNode*>(node);
		if (!fnode)
			node->visit(this);
	}
	// Добавляем вызов функции main
	add(cmdCALL, makeValArg("f_main"));
	// Выход из программы
	add(cmdRET, makeValArg("0"));

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

bool IsAssignment(int lex)
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

// Посетить узел бинарной операции
void CodeGenerator::OnNode(BinaryOpNode* node)
{
	Flag lv(&lvalue, true);
	node->left->visit(this);		
	lv.reset();
	node->right->visit(this);
	add(cmdPOP, EBX);
	add(cmdPOP, EAX);

	if (node->lex == LEX_ASSIGNMENT)
		add(cmdMOV, makeMemArg(EAX), EBX);
	if (node->lex == LEX_ADD_ASSIGNMENT)
		add(cmdADD, makeMemArg(EAX), EBX);
	if (node->lex == LEX_SUB_ASSIGNMENT)
		add(cmdSUB, makeMemArg(EAX), EBX);
	if (node->lex == LEX_MUL_ASSIGNMENT)
		add(cmdMUL, makeMemArg(EAX), EBX);
	if (node->lex == LEX_DIV_ASSIGNMENT)
		add(cmdDIV, makeMemArg(EAX), EBX);
}

// Посетить узел унарной операции
void CodeGenerator::OnNode(UnaryOpNode* node)
{
	Flag lv(&lvalue, true);
	node->right->visit(this);

	if (node->lex == LEX_AND)
		;
	else if (node->lex == LEX_MUL)
	{
		add(cmdPOP, EAX);
		add(cmdPUSH, makeMemArg(EAX));
	}
	else if (node->lex == LEX_SUB)
	{
		add(cmdPOP, EAX);
		add(cmdNEG, makeMemArg(EAX));
		add(cmdPUSH, makeMemArg(EAX));
	}
	else if (node->lex == LEX_INVERT)
	{
		add(cmdPOP, EAX);
		add(cmdNOT, makeMemArg(EAX));
		add(cmdPUSH, makeMemArg(EAX));
	}
	else if (node->lex == LEX_NOT)
	{
	}
}

// Посетить узел значения
void CodeGenerator::OnNode(ValueNode* node)
{
	if (node->getType()->isPointer())
	{
		valCount++;
		addVar("value_" + std::to_string(valCount), varDB, node->lex.text);
		add(cmdPUSH, makeMemArg("value_" + std::to_string(valCount)));
	}
	else
	{
		add(cmdPUSH, makeValArg(node->lex.text));
	}
}

// Посетить узел идентификатора
void CodeGenerator::OnNode(IdentifierNode* node)
{
	VariableSymbol* v = dynamic_cast<VariableSymbol*>(node->symbol);
	if (v)
	{
		// Это обращение к переменной
		if (lvalue)
		{
			add(cmdLEA, EAX, makeMemArg(node->lex.text));
			add(cmdPUSH, EAX);
		}
		else
		{
			add(cmdMOV, EAX, makeMemArg(node->lex.text));
			add(cmdPUSH, EAX);
		}
	}
}

// Посетить узел обращения к массиву
void CodeGenerator::OnNode(ArrayNode* node)
{
	Flag lv(&lvalue, true);
	node->var->visit(this);
	lv.reset();
	node->idx->visit(this);
	add(cmdPOP, EBX);
	add(cmdMUL, EBX, makeValArg(std::to_string(node->getType()->baseType->getLength())));
	add(cmdPOP, EAX);
	add(cmdADD, EAX, EBX);
	if (lvalue)
		add(cmdPUSH, EAX);
	else
		add(cmdPUSH, makeMemArg(EAX));
}

// Посетить узел вызова функции
void CodeGenerator::OnNode(FuncCallNode* node)
{
	add(cmdCALL, makeValArg(node->func->lex.text));
}

// Посетить узел
void CodeGenerator::OnNode(CompoundNode* node)
{
	// выделяем память под локальные переменные
	int len = node->locals.getLength();
	if (len)
		add(cmdSUB, ESP, makeValArg(std::to_string(len)));

	// генерируем для вложенных узлов
	for (int i = 0; i < node->nodes.size(); i++)
		node->nodes[i]->visit(this);

	// Удалем локальные переменные
	if (len)
		add(cmdADD, ESP, makeValArg(std::to_string(len)));
}

// Посетить узел функции
void CodeGenerator::OnNode(FunctionNode* node)
{
	// Название функции
	std::string name = "f_" + node->symbol->name;
	// Метка и пролог
	add(name);
	add(cmdPUSH, EBP);
	add(cmdMOV, EBP, ESP);

	// Транслируем тело функции
	node->statement->visit(this);

	// Окончание функции
	add(name + "_end");
	add(cmdMOV, ESP, EBP);
	add(cmdPOP, EBP);
	add(cmdRET, makeValArg("0"));
}

// Посетить узел
void CodeGenerator::OnNode(IfNode* node)
{
	std::string label1 = "label" + std::to_string(labelCount++);
	std::string label2 = "label" + std::to_string(labelCount++);
	std::string label3 = "label" + std::to_string(labelCount++);
	node->expr->visit(this);
	add(cmdPOP, EAX);
	add(cmdCMP, EAX, makeValArg(0));
	add(cmdJNE, makeValArg(label1));
	add(cmdJE, makeValArg(label2));
	add(label1);
	node->statement1->visit(this);
	add(cmdJMP, makeValArg(label3));
	add(label2);
	if (node->statement2)
		node->statement2->visit(this);
	add(label3);
}

// Посетить узел
void CodeGenerator::OnNode(WhileNode* node)
{
	std::string label1 = "label" + std::to_string(labelCount++);
	std::string label2 = "label" + std::to_string(labelCount++);

	continueStack.push_back(label1);
	breakStack.push_back(label2);

	add(label1);
	node->expr->visit(this);
	add(cmdPOP, EAX);
	add(cmdCMP, EAX, makeValArg(0));
	add(cmdJE, makeValArg(label2));
	node->statement->visit(this);
	add(cmdJMP, makeValArg(label1));
	add(label2);

	continueStack.pop_back();
	breakStack.pop_back();
}

// Посетить узел
void CodeGenerator::OnNode(DoNode* node)
{
	std::string label1 = "label" + std::to_string(labelCount++);
	std::string label2 = "label" + std::to_string(labelCount++);

	continueStack.push_back(label1);
	breakStack.push_back(label2);

	add(label1);
	node->statement->visit(this);
	node->expr->visit(this);
	add(cmdPOP, EAX);
	add(cmdCMP, EAX, makeValArg(0));
	add(cmdJNE, makeValArg(label1));
	add(label2);

	continueStack.pop_back();
	breakStack.pop_back();
}

// Посетить узел
void CodeGenerator::OnNode(ForNode* node)
{
	std::string label1 = "label" + std::to_string(labelCount++);
	std::string label2 = "label" + std::to_string(labelCount++);
	std::string label3 = "label" + std::to_string(labelCount++);
	std::string label4 = "label" + std::to_string(labelCount++);

	continueStack.push_back(label2);
	breakStack.push_back(label3);

	node->expr1->visit(this);
	add(label1);
	node->expr2->visit(this);
	add(cmdPOP, EAX);
	add(cmdCMP, EAX, makeValArg("0"));
	add(cmdJE, makeValArg(label2));
	node->statement->visit(this);
	add(label2);
	node->expr3->visit(this);
	add(cmdJMP, makeValArg(label1));
	add(label3);

	continueStack.pop_back();
	breakStack.pop_back();
}

// Посетить узел
void CodeGenerator::OnNode(BreakNode* node)
{
	add(cmdJMP, makeValArg(breakStack.back()));
}

// Посетить узел
void CodeGenerator::OnNode(ContinueNode* node)
{
	add(cmdJMP, makeValArg(continueStack.back()));
}

// Посетить узел
void CodeGenerator::OnNode(ReturnNode* node)
{
	add(cmdJMP, makeValArg(returnStack.back()));
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

// добавить команду
void CodeGenerator::add(int cmd, int reg)
{
	asmProg->addCmd(new AsmCmd(cmd, makeRegArg(reg)));
}

// Добавить команду
void CodeGenerator::add(int cmd, AsmArg* arg1, AsmArg* arg2)
{
	asmProg->addCmd(new AsmCmd(cmd, arg1, arg2));
}

// Добавить команду
void CodeGenerator::add(int cmd, int reg1, AsmArg* arg2)
{
	asmProg->addCmd(new AsmCmd(cmd, makeRegArg(reg1), arg2));
}

// Добавить команду
void CodeGenerator::add(int cmd, AsmArg* arg1, int reg2)
{
	asmProg->addCmd(new AsmCmd(cmd, arg1, makeRegArg(reg2)));
}

// Добавить команду
void CodeGenerator::add(int cmd, int reg1, int reg2)
{
	asmProg->addCmd(new AsmCmd(cmd, makeRegArg(reg1), makeRegArg(reg2)));
}

// Добавить команду
void CodeGenerator::add(std::string name)
{
	asmProg->addCmd(new AsmCmd(cmdLABEL, name));
}

// Создать аргумент
AsmArg* CodeGenerator::makeValArg(std::string val)
{
	return new AsmArg(argVALUE, val);
}

// Создать аргумент
AsmArg* CodeGenerator::makeRegArg(int reg)
{
	return new AsmArg(argREG, reg);
}

// Создать аргумент
AsmArg* CodeGenerator::makeMemArg(int reg, int offset)
{
	return new AsmArg(argMEMORY, reg, offset);
}

// Создать аргумент
AsmArg* CodeGenerator::makeMemArg(std::string name)
{
	return new AsmArg(argMEMORY, name);
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