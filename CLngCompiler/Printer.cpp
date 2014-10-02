#include "stdafx.h"
#include "Printer.h"


// �����������
Printer::Printer()
{
	tableCount = 0;
	treeCount = 0;
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
	tableCount++;
	std::cout << "Symbol table "  << tableCount << ":" << std::endl;
	for (int i = 0; i < table->symbols.size(); i++)
	{
		std::cout << std::string(spaces, ' ');
		table->symbols[i]->visit(this);
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

// �������� ������
void Printer::OnSymbol(Symbol* symbol)
{
	throw std::exception("Internal program error");
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
		spaces += 8;
		bool old = shortName;
		shortName = true;
		for (int i = 0; i < symbol->fields.size(); i++)
		{
			std::cout << std::string(spaces, ' ');
			symbol->fields[i]->visit(this);
			std::cout << std::endl;
		}
		shortName = old;
		spaces -= 8;
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
	throw std::exception("Internal program error");
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
	std::cout << "function " << symbol->name << " (" << std::endl;
	bool old = shortName;
	shortName = true;
	for (int i = 0; i < symbol->params.size(); i++)
	{
		if (i > 0)
			std::cout << std::string(spaces, ' ') << "," << std::endl;
		spaces += 8;
		std::cout << std::string(spaces, ' ');
		symbol->params[i]->visit(this);
		std::cout << std::endl;
		spaces -= 8;
	}	
	std::cout << ") return ";
	symbol->type->visit(this);
	shortName = old;
}

// �������� ����
void Printer::OnNode(Node* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� ���������
void Printer::OnNode(ProgramNode* node)
{
	node->global.visit(this);
	
	for (int i = 0; i < node->nodes.size(); i++)
	{
		spaces = 0;
		treeCount++;
		std::cout << "Tree "  << treeCount << ":" << std::endl;
		node->nodes[i]->visit(this);
		std::cout << std::endl;
	}
}

// �������� ���� ���������
void Printer::OnNode(ExpressionNode* node)
{
	for (int i= 0; i < node->nodes.size(); i++)
	{
		if (i > 0)
			std::cout << std::string(spaces, ' ') << "," << std::endl;
		node->nodes[i]->visit(this);
		std::cout << std::endl;
	}
}

// �������� ���� ��������� ����������
void Printer::OnNode(ConditionalNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� �������� ��������
void Printer::OnNode(BinaryOpNode* node)
{
	spaces += 8;
	node->left->visit(this);
	std::cout << std::endl;
	spaces -= 8;
	std::cout << std::string(spaces, ' ') << node->lex.text << " <" << std::endl;
	spaces += 8;
	node->right->visit(this);
	std::cout << std::endl;
	spaces -= 8;
}

// �������� ���� ������� ��������
void Printer::OnNode(UnaryOpNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� ��������
void Printer::OnNode(ValueNode* node)
{
	std::cout << std::string(spaces, ' ') << node->lex.text;
}

// �������� ���� ��������������
void Printer::OnNode(IdentifierNode* node)
{
	std::cout << std::string(spaces, ' ') << node->lex.text;
}

// �������� ���� ��������� � �������
void Printer::OnNode(ArrayNode* node)
{
	node->var->visit(this);
	std::cout << std::endl;
	std::cout << std::string(spaces, ' ') << "[" << std::endl;
	spaces += 8;
	node->idx->visit(this);
	spaces -= 8;
	std::cout << std::string(spaces, ' ') << "]" << std::endl;
}

// �������� ���� ������ �������
void Printer::OnNode(FuncCallNode* node)
{
	node->func->visit(this);
	std::cout << std::endl;
	std::cout << std::string(spaces, ' ') << "(" << std::endl;	
	for (int i = 0; i < node->nodes.size(); i++)
	{
		if (i > 0)
			std::cout << std::string(spaces, ' ') << "," << std::endl;
		spaces += 8;
		node->nodes[i]->visit(this);		
		std::cout << std::endl;
		spaces -= 8;
	}	
	std::cout << std::string(spaces, ' ') << ")" << std::endl;
}