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

// �������� �����
int SymbolsTable::getLength()
{
	int length = 0;
	for (int i = 0; i < symbols.size(); i++)
	{
		if (!symbols[i]->isType())
			length += ((ItemSymbol*)(symbols[i]))->type->getLength();
	}
	return length;
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
TypeSymbol::TypeSymbol(std::string name)
	: Symbol(name)
{
	baseType = NULL;
	mode = MODE_NONE;
}

// �����������
TypeSymbol::TypeSymbol(TypeSymbol* baseType, int mode)
	: Symbol(makeName(baseType, mode))
{
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
	// ���� �������������� ��� �����������
	if (to->isConst())
		return canConvertTo(to->baseType);
	// ���� ��� ���� ��������� �� ���������
	if (isPointer() && baseType->isPointer() && to->isPointer() && to->baseType->isPointer())
		return baseType->canConvertTo(to->baseType);
	// ������������� ��������� � �������
	if (isPointer() && to->isArray() && *baseType == *(to->baseType))	
		return true;
	// ������� ������������� �������� ��������
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

	return false;
}

// �������� ����� ����
int TypeSymbol::getLength()
{
	if (isChar())
		return 1;
	if (isInt())
		return 4;
	if (isFloat())
		return 4;
	if (isPointer())
		return 4;
	if (isConst())
		return baseType->getLength();
	return 0;
}

//-------------------------------------------------------------------------

// �����������
AliasSymbol::AliasSymbol(TypeSymbol* baseType, std::string name)
	: TypeSymbol(name)
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

// �������� ����� ����
int AliasSymbol::getLength()
{
	return baseType->getLength();
}

//-------------------------------------------------------------------------

int StructSymbol::nameCount = 0;

// �����������
StructSymbol::StructSymbol(std::string name)
	: TypeSymbol("")
{
	if (name.empty())
	{
		nameCount++;
		this->name = "struct";
		this->name += std::to_string(nameCount);
	}
	else
		this->name = name;
}

// ����������
StructSymbol::~StructSymbol()
{

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

// �������� ����� ����
int StructSymbol::getLength()
{
	return fields.getLength();
}

//-------------------------------------------------------------------------

int ArraySymbol::nameCount = 0;

// �����������
ArraySymbol::ArraySymbol(TypeSymbol* baseType, int count)
	: TypeSymbol(baseType, TypeSymbol::MODE_NONE)
{
	nameCount++;
	this->name = "array";
	this->name += std::to_string(nameCount);
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
	if (*this == *to)
		return true;
	if (to->isPointer() && *baseType == *(to->baseType))
		return true;	
	return false;
}

// �������� ����� ����
int ArraySymbol::getLength()
{
	return count * baseType->getLength();
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

}

// �������� ������
void FunctionSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}


