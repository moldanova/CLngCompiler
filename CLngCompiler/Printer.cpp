#include "stdafx.h"
#include "Printer.h"
#include "Flag.h"

#define INDENT 4

// Конструктор
Printer::Printer(std::string fileName)
	: output(fileName, std::ios::out | std::ios::trunc)
{
	tableCount = 0;
	treeCount = 0;
	isShort = false;
}

// деструктор
Printer::~Printer()
{
}

// Вывести текст
Printer& Printer::operator << (std::string txt)
{
	output << txt << std::endl;
	return *this;
}

// Посетить таблицу символов
void Printer::OnSymbol(SymbolsTable* table)
{
	if (table->symbols.empty())
		return;

	tableCount++;
	output << std::string(spaces, ' ') << "Symbol table "  << tableCount << ":" << std::endl;
	spaces += INDENT;
	for (int i = 0; i < table->symbols.size(); i++)
	{
		if (table->symbols[i]->isType())
		{
			TypeSymbol* type = (TypeSymbol*)table->symbols[i];
			/*
			if (isShort)
			{
				if (type->isConst())
					continue;			
				if (type->isPointer())
					continue;
				if (type->isVoid())
					continue;
				if (type->isScalar())
					continue;
			}
			*/			
			output << std::string(spaces, ' ');
			table->symbols[i]->visit(this);
			output << std::endl;			
		}
	}

	for (int i = 0; i < table->symbols.size(); i++)
	{
		if (!table->symbols[i]->isType())
		{
			Flag flag(&isShort, true);
			output << std::string(spaces, ' ');
			table->symbols[i]->visit(this);
			output << std::endl;
		}
	}

	spaces -= INDENT;
}

// Посетить символ
void Printer::OnSymbol(Symbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить тип
void Printer::OnSymbol(TypeSymbol* symbol)
{
	if (symbol->isConst())
		output << "const ";
	else if (symbol->isPointer())
		output << "pointer to ";
	else 
		output << symbol->name << " ";
	
	if (symbol->baseType != NULL)
	{
		Flag flag(&isShort, true);
		symbol->baseType->visit(this);
	}
}

// Посетить псевдоним
void Printer::OnSymbol(AliasSymbol* symbol)
{
	output << "typedef " << symbol->name << " of ";
	if (symbol->baseType != NULL)
	{
		Flag flag(&isShort, true);
		symbol->baseType->visit(this);
	}
}

// Посетить структуру
void Printer::OnSymbol(StructSymbol* symbol)
{
	if (isShort)
	{
		output << "struct " << symbol->name << " ";
	}
	else
	{
		output << "struct " << symbol->name << std::endl;
		output << std::string(spaces, ' ') << "fileds:" << std::endl;
		spaces += INDENT;		
		symbol->fields.visit(this);
		spaces -= INDENT;
	}
}

// Посетить массив
void Printer::OnSymbol(ArraySymbol* symbol)
{
	output << "array " << symbol->count << " of ";
	if (symbol->baseType != NULL)
	{
		Flag flag(&isShort, true);
		symbol->baseType->visit(this);
	}
}

// Посетить символ
void Printer::OnSymbol(ItemSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// Посетить константу
void Printer::OnSymbol(ConstantSymbol* symbol)
{
	output << "constant " << symbol->name << " type of ";
	Flag flag(&isShort, true);
	symbol->type->visit(this);
}

// Посетить переменную
void Printer::OnSymbol(VariableSymbol* symbol)
{
	output << "variable " << symbol->name << " type of ";
	Flag flag(&isShort, true);
	symbol->type->visit(this);
}

// Посетить функцию
void Printer::OnSymbol(FunctionSymbol* symbol)
{
	output << "function " << symbol->name << " (" << std::endl;
	Flag flag(&isShort, true);
	spaces += INDENT;
	symbol->params.visit(this);
	output << std::string(spaces, ' ') << ") return ";
	symbol->type->visit(this);
	spaces -= INDENT;
}

// Посетить узел
void Printer::OnNode(Node* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел программы
void Printer::OnNode(ProgramNode* node)
{
	spaces = 0;

	node->globals.visit(this);
	output << std::endl;

	for (int i = 0; i < node->nodes.size(); i++)
	{
		spaces = 0;
		treeCount++;
		output << "Tree "  << treeCount << ":" << std::endl;
		spaces += INDENT;
		node->nodes[i]->visit(this);
		output << std::endl;
		spaces -= INDENT;
	}
}

// Посетить узел выражения
void Printer::OnNode(ExpressionNode* node)
{
	for (int i= 0; i < node->nodes.size(); i++)
	{
		if (i > 0)
			output << std::string(spaces, ' ') << "," << std::endl;
		node->nodes[i]->visit(this);
		output << std::endl;
	}
}

// Посетить узел условного выраженрия
void Printer::OnNode(ConditionalNode* node)
{
	throw std::exception("Internal program error");
}

// Посетить узел бинарной операции
void Printer::OnNode(BinaryOpNode* node)
{
	spaces += INDENT;
	node->left->visit(this);
	spaces -= INDENT;
	output << std::string(spaces, ' ') << node->lex.text << " <" << std::endl;
	spaces += INDENT;
	node->right->visit(this);
	spaces -= INDENT;
}

// Посетить узел унарной операции
void Printer::OnNode(UnaryOpNode* node)
{
	if (node->postfix)
	{
		node->right->visit(this);
		output << std::string(spaces + INDENT, ' ') << node->lex.text << std::endl;
	}
	else
	{
		output << std::string(spaces, ' ') << node->lex.text << std::endl;
		spaces += INDENT;
		node->right->visit(this);
		spaces -= INDENT;
	}
}

// Посетить узел значения
void Printer::OnNode(ValueNode* node)
{
	output << std::string(spaces, ' ') << node->lex.text << std::endl;
}

// Посетить узел идентификатора
void Printer::OnNode(IdentifierNode* node)
{
	output << std::string(spaces, ' ') << node->lex.text << std::endl;
}

// Посетить узел обращения к массиву
void Printer::OnNode(ArrayNode* node)
{
	output << std::string(spaces, ' ') << "array:" << std::endl;
	spaces += INDENT;
	node->var->visit(this);
	spaces -= INDENT;
	output << std::string(spaces, ' ') << "index:" << std::endl;
	spaces += INDENT;
	node->idx->visit(this);
	spaces -= INDENT;
}

// Посетить узел вызова функции
void Printer::OnNode(FuncCallNode* node)
{
	output << std::string(spaces, ' ') << "function:" << std::endl;
	{
		Flag flag(&isShort, false);
		spaces += INDENT;
		node->func->visit(this);
		spaces -= INDENT;	
	}
	if (!node->nodes.empty())
	{
		output << std::string(spaces, ' ') << "args:" << std::endl;	
		spaces += INDENT;
		for (int i = 0; i < node->nodes.size(); i++)
			node->nodes[i]->visit(this);		
		spaces -= INDENT;
	}
}

// Посетить узел
void Printer::OnNode(CompoundNode* node)
{
	output << std::string(spaces, ' ') << "{" << std::endl;
	spaces += INDENT;
	node->locals.visit(this);
	for (int i = 0; i < node->nodes.size(); i++)
		node->nodes[i]->visit(this);
	spaces -= INDENT;
	output << std::string(spaces, ' ') << "}" << std::endl;
}

// Посетить узел функции
void Printer::OnNode(FunctionNode* node)
{
	output << std::string(spaces, ' ') << "function: " << node->symbol->name << std::endl;
	spaces += INDENT;
	node->params.visit(this);
	spaces -= INDENT;
	output << std::string(spaces, ' ') << "body:" << std::endl;
	if (node->statement != NULL)
		node->statement->visit(this);
}

// Посетить узел
void Printer::OnNode(IfNode* node)
{
	output << std::string(spaces, ' ') << "if:" << std::endl;
	spaces += INDENT;
	node->expr->visit(this);
	spaces -= INDENT;
	output << std::string(spaces, ' ') << "then:" << std::endl;
	spaces += INDENT;
	node->statement1->visit(this);
	spaces -= INDENT;
	if (node->statement2 != NULL)
	{
		output << std::string(spaces, ' ') << "else:" << std::endl;
		spaces += INDENT;
		node->statement2->visit(this);
		spaces -= INDENT;
	}
}

// Посетить узел
void Printer::OnNode(WhileNode* node)
{
	output << std::string(spaces, ' ') << "while:" << std::endl;
	spaces += INDENT;
	node->expr->visit(this);
	spaces -= INDENT;
	output << std::string(spaces, ' ') << "statement:" << std::endl;
	spaces += INDENT;
	node->statement->visit(this);
	spaces -= INDENT;
}

// Посетить узел
void Printer::OnNode(DoNode* node)
{
	output << std::string(spaces, ' ') << "do:" << std::endl;
	spaces += INDENT;
	node->statement->visit(this);
	spaces -= INDENT;
	output << std::string(spaces, ' ') << "expression:" << std::endl;
	spaces += INDENT;
	node->expr->visit(this);
	spaces -= INDENT;
}

// Посетить узел
void Printer::OnNode(ForNode* node)
{
	output << std::string(spaces, ' ') << "for:" << std::endl;
	if (node->expr1 != NULL)
	{
		output << std::string(spaces, ' ') << "expression1:" << std::endl;
		spaces += INDENT;
		node->expr1->visit(this);
		spaces -= INDENT;
	}	
	if (node->expr2 != NULL)
	{
		output << std::string(spaces, ' ') << "expression2:" << std::endl;
		spaces += INDENT;
		node->expr2->visit(this);
		spaces -= INDENT;
	}
	if (node->expr3 != NULL)
	{
		output << std::string(spaces, ' ') << "expression2:" << std::endl;
		spaces += INDENT;
		node->expr3->visit(this);
		spaces -= INDENT;
	}
	output << std::string(spaces, ' ') << "statement:" << std::endl;
	spaces += INDENT;
	node->statement->visit(this);
	spaces -= INDENT;
}

// Посетить узел
void Printer::OnNode(BreakNode* node)
{
	output << std::string(spaces, ' ') << "break" << std::endl;
}

// Посетить узел
void Printer::OnNode(ContinueNode* node)
{
	output << std::string(spaces, ' ') << "continue" << std::endl;
}

// Посетить узел
void Printer::OnNode(ReturnNode* node)
{
	output << std::string(spaces, ' ') << "return" << std::endl;
	if (node->expr != NULL)
	{
		spaces += INDENT;
		node->expr->visit(this);
		spaces -= INDENT;
	}
}