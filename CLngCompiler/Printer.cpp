#include "stdafx.h"
#include "Printer.h"


// �����������
Printer::Printer()
{
	tablesCount = 0;
	shortName = false;
}

// ����������
Printer::~Printer()
{
}

// �������� ������� ��������
void Printer::OnSymbol(SymbolsTable* table)
{
	spaces = 0;
	tablesCount++;
	std::cout << "Symbol table "  << tablesCount << std::endl;
	for (int i = 0; i < table->symbols.size(); i++)
	{
		std::cout << std::string(spaces, ' ');
		table->symbols[i]->visit(this);
		std::cout << std::endl;
	}
}

// �������� ������
void Printer::OnSymbol(Symbol* symbol)
{
	std::exception("Internal program error");
}

// �������� ���
void Printer::OnSymbol(TypeSymbol* symbol)
{
	if (symbol->isConst())
		std::cout << "const ";
	else if (symbol->isPointer())
		std::cout << "pointer to ";
	else 
		std::cout << symbol->name << " ";
	
	if (symbol->baseType != NULL)
	{
		bool old = shortName;
		shortName = true;
		symbol->baseType->visit(this);
		shortName = old;
	}
}

// �������� ���������
void Printer::OnSymbol(AliasSymbol* symbol)
{
	std::cout << "typedef " << symbol->name << " of ";
	if (symbol->baseType != NULL)
	{
		bool old = shortName;
		shortName = true;
		symbol->baseType->visit(this);
		shortName = old;
	}
}

// �������� ���������
void Printer::OnSymbol(StructSymbol* symbol)
{
	if (shortName)
	{
		std::cout << "struct " << symbol->name << " ";
	}
	else
	{
		std::cout << "struct " << symbol->name << " {" << std::endl;
		spaces += 4;
		bool old = shortName;
		shortName = true;
		for (int i = 0; i < symbol->fields.size(); i++)
		{
			std::cout << std::string(spaces, ' ');
			symbol->fields[i]->visit(this);
			std::cout << std::endl;
		}
		shortName = old;
		spaces -= 4;
		std::cout << std::string(spaces, ' ') << "} ";
	}
}

// �������� ������
void Printer::OnSymbol(ArraySymbol* symbol)
{
	std::cout << "array " << symbol->count << " of ";
	if (symbol->baseType != NULL)
	{
		bool old = shortName;
		shortName = true;
		symbol->baseType->visit(this);
		shortName = old;
	}
}

// �������� ������
void Printer::OnSymbol(ItemSymbol* symbol)
{
	std::exception("Internal program error");
}

// �������� ���������
void Printer::OnSymbol(ConstantSymbol* symbol)
{
	std::cout << "constant " << symbol->name << " type of ";
	bool old = shortName;
	shortName = true;
	symbol->type->visit(this);
	shortName = old;
}

// �������� ����������
void Printer::OnSymbol(VariableSymbol* symbol)
{
	std::cout << "variable " << symbol->name << " type of ";
	bool old = shortName;
	shortName = true;
	symbol->type->visit(this);
	shortName = old;
}

// �������� �������
void Printer::OnSymbol(FunctionSymbol* symbol)
{
	std::cout << "function " << symbol->name << " (";
	bool old = shortName;
	shortName = true;
	for (int i = 0; i < symbol->params.size(); i++)
	{
		symbol->params[i]->visit(this);
	}	
	std::cout << ") return ";
	symbol->type->visit(this);
	shortName = old;
}

// �������� ����
void Printer::OnNode(Node* node)
{
	std::exception("Internal program error");
}

// �������� ���� ���������
void Printer::OnNode(ProgramNode* node)
{
	node->global.visit(this);
}

// �������� ���� ���������
void Printer::OnNode(ExpressionNode* node)
{
	std::exception("Internal program error");
}

// �������� ���� ��������� ����������
void Printer::OnNode(ConditionalNode* node)
{
	std::exception("Internal program error");
}

// �������� ���� �������� ��������
void Printer::OnNode(BinaryOpNode* node)
{
	std::exception("Internal program error");
}

// �������� ���� ������� ��������
void Printer::OnNode(UnaryOpNode* node)
{
	std::exception("Internal program error");
}

// �������� ���� ��������
void Printer::OnNode(ValueNode* node)
{
	std::exception("Internal program error");
}

// �������� ���� ��������������
void Printer::OnNode(IdentifierNode* node)
{
	std::exception("Internal program error");
}

// �������� ���� ��������� � �������
void Printer::OnNode(ArrayNode* node)
{
	std::exception("Internal program error");
}

// �������� ���� ������ �������
void Printer::OnNode(FuncCallNode* node)
{
	std::exception("Internal program error");
}