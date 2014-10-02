#include "stdafx.h"
#include "Symbols.h"

//-------------------------------------------------------------------------

// �����������
Symbol::Symbol()
{
	this->name = "";
}

// �����������
Symbol::Symbol(std::string name)
{
	this->name = name;
}

// ����������
Symbol::~Symbol()
{
}

// �������� ������
void Symbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

//-------------------------------------------------------------------------

// �����������
SymbolsTable::SymbolsTable()
{
}

// ����������
SymbolsTable::~SymbolsTable()
{
	for (int i = 0; i < symbols.size(); i++)
		delete symbols[i];
	symbols.clear();
}

// �������� ������
void SymbolsTable::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// �������� ������
void SymbolsTable::addSymbol(Symbol* symbol)
{
	symbols.push_back(symbol);
}

// ����� ������� �� �����
Symbol* SymbolsTable::findByName(std::string name)
{
	for (int i = 0; i < symbols.size(); i++)
		if (symbols[i]->name == name)
			return symbols[i];
	return NULL;
}

// ����� � ����������
Symbol* SymbolsTable::findOrAdd(Symbol* symbol)
{
	Symbol* finded = findByName(symbol->name);
	if (finded != NULL)
		return finded;
	addSymbol(symbol);
	return symbol;
}


//-------------------------------------------------------------------------

// ������� ���
std::string TypeSymbol::makeName(TypeSymbol* baseType, int mode)
{
	std::string name;
	if (mode == MODE_CONST && baseType->isPointer())
		name = baseType->name + " const";
	else if (mode == MODE_CONST)
		name = "const " + baseType->name;
	else if (mode == MODE_POINTER)
		name = baseType->name + " *";
	return name;
}

// �����������
TypeSymbol::TypeSymbol(std::string name, int length)
	: Symbol(name)
{
	this->length = length;
	baseType = NULL;
	mode = MODE_NONE;
}

// �����������
TypeSymbol::TypeSymbol(TypeSymbol* baseType, int mode)
	: Symbol(makeName(baseType, mode))
{
	this->length = mode == MODE_CONST ? baseType->length : 4;
	this->baseType = baseType;
	this->mode = mode;
}

// ����������
TypeSymbol::~TypeSymbol()
{
	baseType = NULL;
}

// �������� ������
void TypeSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// ��������� ���� �����
bool TypeSymbol::operator == (const TypeSymbol& type)
{
	return name == type.name;
}

// �������� ����������� ���������������
bool TypeSymbol::canConvertTo(TypeSymbol* to)
{
	// ���� ���������, �� ��������� �������� ���
	if (dynamic_cast<AliasSymbol*>(to) != NULL)
		return canConvertTo(to->baseType);
	// ���� ���� �����
	if (*this == *to)
		return true;
	// ���� �� ����� ������
	if (isVoid() || to->isVoid())
		return false;
	// ���� �������������� ��� �����������
	if (to->isConst())
		return canConvertTo(to->baseType);
	// ���� ��� ���� ���������
	if (isPointer() && baseType->isPointer() && to->isPointer() && to->baseType->isPointer())
		return baseType->canConvertTo(to->baseType);
	// ������� ���������� �������� ��������
	if (name == "const char" && to->name == "char")
		return true;	
	if ((name == "char" || name == "const char") && to->name == "int")
		return true;
	if ((name == "char" || name == "const char") && to->name == "float")
		return true;
	if (name == "const int" && to->name == "int")
		return true;
	if ((name == "int" || name == "const int") && to->name == "float")
		return true;
	if (name == "const float" && to->name == "float")
		return true;
	// ������� ���������� ����������
	if (isPointer() && to->name == "void *")
		return true;
	// ������� ���������� � �����������
	if (isPointer() && to->isInt())
		return true;
	if (isInt() && to->isPointer())
		return true;
	return false;
}

//-------------------------------------------------------------------------

// �����������
AliasSymbol::AliasSymbol(TypeSymbol* baseType, std::string name)
	: TypeSymbol(name, baseType->length)
{
	this->baseType = baseType;
}

// ����������
AliasSymbol::~AliasSymbol()
{
}

// �������� ������
void AliasSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// �������� ����������� ���������������
bool AliasSymbol::canConvertTo(TypeSymbol* to)
{
	return baseType->canConvertTo(to);
}

//-------------------------------------------------------------------------

// �����������
StructSymbol::StructSymbol(std::string name)
	: TypeSymbol("", 0)
{
	if (name.empty())
	{
		char s[200];
		sprintf(s, "struct s_%x", this);
		this->name = s;
	}
	else
		this->name = "struct " + name;
}

// ����������
StructSymbol::~StructSymbol()
{
	for (int i = 0; i < fields.size(); i++)
		delete fields[i];
	fields.clear();
}

// �������� ������
void StructSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// �������� ����������� ���������������
bool StructSymbol::canConvertTo(TypeSymbol* to)
{
	return *this == *to;
}

// �������� ����
void StructSymbol::addField(VariableSymbol* field)
{
	fields.push_back(field);
	length += field->type->length;
}

// ����� ���� �� �����
VariableSymbol* StructSymbol::findFieldByName(std::string name)
{
	for (int i = 0; i < fields.size(); i++)
		if (fields[i]->name == name)
			return fields[i];
	return NULL;
}

//-------------------------------------------------------------------------

// �����������
ArraySymbol::ArraySymbol(TypeSymbol* baseType, int count)
	: TypeSymbol(baseType, TypeSymbol::MODE_NONE)
{
	this->length = baseType->length * count;
	this->count = count;
}

// ����������
ArraySymbol::~ArraySymbol()
{
}

// �������� ������
void ArraySymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// �������� ����������� ���������������
bool ArraySymbol::canConvertTo(TypeSymbol* to)
{
	return *this == *to;
}

//-------------------------------------------------------------------------

// �����������
ItemSymbol::ItemSymbol(std::string name, TypeSymbol* type)
	: Symbol(name)
{
	this->type = type;
}

// ����������
ItemSymbol::~ItemSymbol()
{
	type = NULL;
}

// �������� ������
void ItemSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

//-------------------------------------------------------------------------

// �����������
ConstantSymbol::ConstantSymbol(std::string name, TypeSymbol* type)
	: ItemSymbol(name, type)
{
}

// ����������
ConstantSymbol::~ConstantSymbol()
{
}

// �������� ������
void ConstantSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

//-------------------------------------------------------------------------

// �����������
VariableSymbol::VariableSymbol(std::string name, TypeSymbol* type)
	: ItemSymbol(name, type)
{
}

// ����������
VariableSymbol::~VariableSymbol()
{
}

// �������� ������
void VariableSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

//-------------------------------------------------------------------------

// �����������
FunctionSymbol::FunctionSymbol(std::string name, TypeSymbol* type)
	: ItemSymbol(name, type)
{
}

// ����������
FunctionSymbol::~FunctionSymbol()
{
	for (int i = 0; i < params.size(); i++)
		delete params[i];
	params.clear();
}

// �������� ������
void FunctionSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// �������� ��������
void FunctionSymbol::addParam(VariableSymbol* param)
{
	params.push_back(param);
}

