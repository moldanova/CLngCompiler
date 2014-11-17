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
	asmProc = NULL;
	valCount = 0;
	labelCount = 0;
	tableCount = 0;
	isParams = false;
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
	for (int i = 0; i < prog->nodes.size(); i++)
	{
		Node* node = prog->nodes[i];
		FunctionNode* fnode = dynamic_cast<FunctionNode*>(node);
		if (!fnode)
			node->visit(this);
	}

	return asmProg;
}

// Посетить таблицу символов
void CodeGenerator::OnSymbol(SymbolsTable* table)
{
	for (int i = 0; i < table->symbols.size(); i++)
	{
		Symbol* symbol = table->symbols[i];
		VariableSymbol* vs = dynamic_cast<VariableSymbol*>(symbol);
		if (vs)
		{
			if (vs->type->isChar())
				var(vs->asmName, "SBYTE", 1);
			else if (vs->type->isInt())
				var(vs->asmName, "SDWORD", 1);
			else if (vs->type->isFloat())
				var(vs->asmName, "REAL4", 1);
			else if (vs->type->isPointer())
				var(vs->asmName, "DWORD", 1);
			else
				var(vs->asmName, "DWORD", vs->type->getLength() / 4);
		}
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
	// Если это присваивание
	if (node->lex == LEX_ASSIGNMENT || node->lex == LEX_ADD_ASSIGNMENT || node->lex == LEX_SUB_ASSIGNMENT
		|| node->lex == LEX_MUL_ASSIGNMENT || node->lex == LEX_DIV_ASSIGNMENT)
	{		
		// Операнд справа
		node->right->visit(this);
		// Операнд слева
		AsmArg* lArg = NULL;
		VariableSymbol* vs = dynamic_cast<VariableSymbol*>(node->left->symbol);
		if (vs)
			lArg = memArg(vs->asmName);
		else
		{
			Flag lv(&lvalue, true);
			node->left->visit(this);		
			code(cmdPOP, regArg(EAX));
			lArg = memArg(EAX);
		}
		// операнды команды
		code(cmdPOP, regArg(EBX));	

		// Выполняем операцию
		if (node->lex == LEX_ASSIGNMENT)
			code(cmdMOV, lArg, regArg(EBX));
		if (node->lex == LEX_ADD_ASSIGNMENT)
			code(cmdADD, lArg, regArg(EBX));
		if (node->lex == LEX_SUB_ASSIGNMENT)
			code(cmdSUB, lArg, regArg(EBX));
		if (node->lex == LEX_MUL_ASSIGNMENT)
			code(cmdMUL, lArg, regArg(EBX));
		if (node->lex == LEX_DIV_ASSIGNMENT)
			code(cmdDIV, lArg, regArg(EBX));
	}
	// Бинарная операция
	if (node->lex == LEX_ADD || node->lex == LEX_SUB || node->lex == LEX_MUL || node->lex == LEX_DIV)
	{
		// Операнд справа
		node->right->visit(this);
		// Операнд слева
		node->left->visit(this);		
	
		// Подготавливаем операнды
		code(cmdPOP, regArg(EAX));
		code(cmdPOP, regArg(EBX));	

		// Выполняем операцию
		if (node->lex == LEX_ADD)
			code(cmdADD, regArg(EAX), regArg(EBX));
		if (node->lex == LEX_SUB)
			code(cmdSUB, regArg(EAX), regArg(EBX));
		if (node->lex == LEX_MUL)
			code(cmdMUL, regArg(EAX), regArg(EBX));
		if (node->lex == LEX_DIV)
			code(cmdDIV, regArg(EAX), regArg(EBX));
		
		code(cmdPUSH, regArg(EAX));
	}
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
		code(cmdPOP, regArg(EAX));
		code(cmdPUSH, memArg(EAX));
	}
	else if (node->lex == LEX_SUB)
	{
		code(cmdPOP, regArg(EAX));
		code(cmdNEG, memArg(EAX));
		code(cmdPUSH, memArg(EAX));
	}
	else if (node->lex == LEX_INVERT)
	{
		code(cmdPOP, regArg(EAX));
		code(cmdNOT, memArg(EAX));
		code(cmdPUSH, memArg(EAX));
	}
	else if (node->lex == LEX_NOT)
	{
	}
}

// Посетить узел значения
void CodeGenerator::OnNode(ValueNode* node)
{
	if (node->lex == LEX_STRING_VALUE)
	{
		// Необходимо преобразовать строку, так она может содержать управляющие последовательности символов
		
		std::string s;
		int i = 0;
		while (i < node->lex.text.size())
		{
			if (node->lex.text[i] == '\\')
			{
				i++;
				if (node->lex.text[i] = 'n')
					s += "\", 10";
				if (node->lex.text[i] == 'r')
					s += "\", 13";
				i++;
				if (i < node->lex.text.size()-1)
					s += ", \"";
				else
					i++;
			}
			else
			{
				s += node->lex.text[i];
				i++;
			}
		}
		s += ", 0";

		valCount++;
		value("value_" + std::to_string(valCount), "SBYTE", s);
		code(cmdPUSH, offArg("value_" + std::to_string(valCount)));
	}
	else
	{
		code(cmdPUSH, valArg(node->lex.text));
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
			code(cmdPUSH, offArg(v->asmName));
		}
		else
		{
			code(cmdPUSH, memArg(v->asmName));
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
	code(cmdPOP, regArg(EBX));
	code(cmdMUL, regArg(EBX), valArg(std::to_string(node->getType()->baseType->getLength())));
	code(cmdPOP, regArg(EAX));
	code(cmdADD, regArg(EAX), regArg(EBX));
	if (lvalue)
		code(cmdPUSH, regArg(EAX));
	else
		code(cmdPUSH, memArg(EAX));
}

// Посетить узел вызова функции
void CodeGenerator::OnNode(FuncCallNode* node)
{
	// Транслируем операнды
	for (int i = node->nodes.size()-1; i >= 0; i--)
		node->nodes[i]->visit(this);
	// Вызываем функцию
	FunctionSymbol* fs = (FunctionSymbol*)(node->symbol);
	code(cmdCALL, valArg(fs->asmName));
	// Очищаем стек
	code(cmdADD, regArg(ESP), valArg(std::to_string(node->nodes.size() * 4)));
}

// Посетить узел
void CodeGenerator::OnNode(CompoundNode* node)
{
	// транслируем локальные переменные
	node->locals.visit(this);

	// генерируем для вложенных узлов
	for (int i = 0; i < node->nodes.size(); i++)
		node->nodes[i]->visit(this);
}

// Посетить узел функции
void CodeGenerator::OnNode(FunctionNode* node)
{
	FunctionSymbol* fs = (FunctionSymbol*)node->symbol;
	// Название функции
	std::string label1 = fs->asmName + "_end";
	
	asmProc = new AsmProc(fs->asmName);
	returnStack.push_back(label1);

	// Транслируем параметры
	isParams = true;
	node->params.visit(this);
	isParams = false;
	// Транслируем тело функции
	node->statement->visit(this);

	asmProg->addProc(asmProc);
	asmProc = NULL;
	returnStack.pop_back();
}

// Посетить узел
void CodeGenerator::OnNode(IfNode* node)
{
	std::string label1 = "label" + std::to_string(labelCount++);
	std::string label2 = "label" + std::to_string(labelCount++);
	std::string label3 = "label" + std::to_string(labelCount++);
	node->expr->visit(this);
	code(cmdPOP, regArg(EAX));
	code(cmdCMP, regArg(EAX), valArg(0));
	code(cmdJNE, valArg(label1));
	code(cmdJE, valArg(label2));
	label(label1);
	node->statement1->visit(this);
	code(cmdJMP, valArg(label3));
	label(label2);
	if (node->statement2)
		node->statement2->visit(this);
	label(label3);
}

// Посетить узел
void CodeGenerator::OnNode(WhileNode* node)
{
	std::string label1 = "label" + std::to_string(labelCount++);
	std::string label2 = "label" + std::to_string(labelCount++);

	continueStack.push_back(label1);
	breakStack.push_back(label2);

	label(label1);
	node->expr->visit(this);
	code(cmdPOP, regArg(EAX));
	code(cmdCMP, regArg(EAX), valArg(0));
	code(cmdJE, valArg(label2));
	node->statement->visit(this);
	code(cmdJMP, valArg(label1));
	label(label2);

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

	label(label1);
	node->statement->visit(this);
	node->expr->visit(this);
	code(cmdPOP, regArg(EAX));
	code(cmdCMP, regArg(EAX), valArg(0));
	code(cmdJNE, valArg(label1));
	label(label2);

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
	label(label1);
	node->expr2->visit(this);
	code(cmdPOP, regArg(EAX));
	code(cmdCMP, regArg(EAX), valArg("0"));
	code(cmdJE, valArg(label2));
	node->statement->visit(this);
	label(label2);
	node->expr3->visit(this);
	code(cmdJMP, valArg(label1));
	label(label3);

	continueStack.pop_back();
	breakStack.pop_back();
}

// Посетить узел
void CodeGenerator::OnNode(BreakNode* node)
{
	code(cmdJMP, valArg(breakStack.back()));
}

// Посетить узел
void CodeGenerator::OnNode(ContinueNode* node)
{
	code(cmdJMP, valArg(continueStack.back()));
}

// Посетить узел
void CodeGenerator::OnNode(ReturnNode* node)
{
	code(cmdJMP, valArg(returnStack.back()));
}

// Добавить команду
void CodeGenerator::code(int cmd)
{
	if (asmProc)
		asmProc->addCode(new AsmCmd(cmd));
	else
		asmProg->addCode(new AsmCmd(cmd));
}

// добавить команду
void CodeGenerator::code(int cmd, AsmArg* arg1)
{
	if (asmProc)
		asmProc->addCode(new AsmCmd(cmd, arg1));
	else
		asmProg->addCode(new AsmCmd(cmd, arg1));
}

// Добавить команду
void CodeGenerator::code(int cmd, AsmArg* arg1, AsmArg* arg2)
{
	if (asmProc)
		asmProc->addCode(new AsmCmd(cmd, arg1, arg2));
	else
		asmProg->addCode(new AsmCmd(cmd, arg1, arg2));
}

// Добавить команду
void CodeGenerator::label(std::string name)
{
	if (asmProc)
		asmProc->addCode(new AsmCmd(cmdLABEL, valArg(name)));
	else
		asmProg->addCode(new AsmCmd(cmdLABEL, valArg(name)));
}

// Создать аргумент
AsmArg* CodeGenerator::valArg(std::string val)
{
	return new AsmArg(argVALUE, val);
}

// Создать аргумент
AsmArg* CodeGenerator::regArg(int reg)
{
	return new AsmArg(argREG, reg);
}

// Создать аргумент
AsmArg* CodeGenerator::memArg(int reg, int offset)
{
	return new AsmArg(argMEMORY, reg, offset);
}

// Создать аргумент
AsmArg* CodeGenerator::memArg(std::string name)
{
	return new AsmArg(argMEMORY, name);
}

// Создать аргумент
AsmArg* CodeGenerator::offArg(std::string name)
{
	return new AsmArg(argOFFSET, name);
}

// добавить переменную
void CodeGenerator::value(std::string name, std::string type, std::string value)
{
	asmProg->addGlobal(new AsmVar(name, type, value));
}

// добавить переменную
void CodeGenerator::var(std::string name, std::string type, int count)
{
	if (asmProc && isParams)
		asmProc->addParam(new AsmVar(name, type, count));
	else if (asmProc)
		asmProc->addLocal(new AsmVar(name, type, count));
	else
		asmProg->addGlobal(new AsmVar(name, type, count));
}