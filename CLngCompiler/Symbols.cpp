#include "stdafx.h"
#include "Symbols.h"

//-------------------------------------------------------------------------

// Конструктор
Symbol::Symbol()
{
	this->name = "";
}

// Конструктор
Symbol::Symbol(std::string name)
{
	this->name = name;
}

// Деструктор
Symbol::~Symbol()
{
}

// Посетить символ
void Symbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

//-------------------------------------------------------------------------

// Конструктор
SymbolsTable::SymbolsTable()
{
}

// Деструктор
SymbolsTable::~SymbolsTable()
{
	for (int i = 0; i < symbols.size(); i++)
		delete symbols[i];
	symbols.clear();
}

// Посетить символ
void SymbolsTable::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// добавить символ
void SymbolsTable::addSymbol(Symbol* symbol)
{
	symbols.push_back(symbol);
}

// Поиск символа по имени
Symbol* SymbolsTable::findByName(std::string name)
{
	for (int i = 0; i < symbols.size(); i++)
		if (symbols[i]->name == name)
			return symbols[i];
	return NULL;
}

// Поиск и добавление
Symbol* SymbolsTable::findOrAdd(Symbol* symbol)
{
	Symbol* finded = findByName(symbol->name);
	if (finded != NULL)
		return finded;
	addSymbol(symbol);
	return symbol;
}


//-------------------------------------------------------------------------

// Создать имя
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

// Конструктор
TypeSymbol::TypeSymbol(std::string name, int length)
	: Symbol(name)
{
	this->length = length;
	baseType = NULL;
	mode = MODE_NONE;
}

// Конструктор
TypeSymbol::TypeSymbol(TypeSymbol* baseType, int mode)
	: Symbol(makeName(baseType, mode))
{
	this->length = mode == MODE_CONST ? baseType->length : 4;
	this->baseType = baseType;
	this->mode = mode;
}

// Деструктор
TypeSymbol::~TypeSymbol()
{
	baseType = NULL;
}

// Посетить символ
void TypeSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// Сравнение двух типов
bool TypeSymbol::operator == (const TypeSymbol& type)
{
	return name == type.name;
}

// Проверка возможности конвертирования
bool TypeSymbol::canConvertTo(TypeSymbol* to)
{
	// Если псевдоним, то проверяем исходный тип
	if (dynamic_cast<AliasSymbol*>(to) != NULL)
		return canConvertTo(to->baseType);
	// Если типы равны
	if (*this == *to)
		return true;
	// один из типов пустой
	if (isVoid() || to->isVoid())
		return false;
	// Если результирующий тип константный
	if (to->isConst())
		return canConvertTo(to->baseType);
	// Если оба типа указатели
	if (isPointer() && baseType->isPointer() && to->isPointer() && to->baseType->isPointer())
		return baseType->canConvertTo(to->baseType);
	// Правила приведения числовых значений
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
	// Правила приведения указателей
	if (isPointer() && to->name == "void *")
		return true;
	// Правила арифметики с указателями
	if (isPointer() && to->isInt())
		return true;
	if (isInt() && to->isPointer())
		return true;
	return false;
}

//-------------------------------------------------------------------------

// Конструктор
AliasSymbol::AliasSymbol(TypeSymbol* baseType, std::string name)
	: TypeSymbol(name, baseType->length)
{
	this->baseType = baseType;
}

// Деструктор
AliasSymbol::~AliasSymbol()
{
}

// Посетить символ
void AliasSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// Проверка возможности конвертирования
bool AliasSymbol::canConvertTo(TypeSymbol* to)
{
	return baseType->canConvertTo(to);
}

//-------------------------------------------------------------------------

// Конструктор
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

// Деструктор
StructSymbol::~StructSymbol()
{
	for (int i = 0; i < fields.size(); i++)
		delete fields[i];
	fields.clear();
}

// Посетить символ
void StructSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// Проверка возможности конвертирования
bool StructSymbol::canConvertTo(TypeSymbol* to)
{
	return *this == *to;
}

// Добавить поле
void StructSymbol::addField(VariableSymbol* field)
{
	fields.push_back(field);
	length += field->type->length;
}

// Поиск поля по имени
VariableSymbol* StructSymbol::findFieldByName(std::string name)
{
	for (int i = 0; i < fields.size(); i++)
		if (fields[i]->name == name)
			return fields[i];
	return NULL;
}

//-------------------------------------------------------------------------

// Конструктор
ArraySymbol::ArraySymbol(TypeSymbol* baseType, int count)
	: TypeSymbol(baseType, TypeSymbol::MODE_NONE)
{
	this->length = baseType->length * count;
	this->count = count;
}

// Деструктор
ArraySymbol::~ArraySymbol()
{
}

// Посетить символ
void ArraySymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// Проверка возможности конвертирования
bool ArraySymbol::canConvertTo(TypeSymbol* to)
{
	return *this == *to;
}

//-------------------------------------------------------------------------

// Конструктор
ItemSymbol::ItemSymbol(std::string name, TypeSymbol* type)
	: Symbol(name)
{
	this->type = type;
}

// Деструктор
ItemSymbol::~ItemSymbol()
{
	type = NULL;
}

// Посетить символ
void ItemSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

//-------------------------------------------------------------------------

// Конструктор
ConstantSymbol::ConstantSymbol(std::string name, TypeSymbol* type)
	: ItemSymbol(name, type)
{
}

// Деструктор
ConstantSymbol::~ConstantSymbol()
{
}

// Посетить символ
void ConstantSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

//-------------------------------------------------------------------------

// Конструктор
VariableSymbol::VariableSymbol(std::string name, TypeSymbol* type)
	: ItemSymbol(name, type)
{
}

// Деструктор
VariableSymbol::~VariableSymbol()
{
}

// Посетить символ
void VariableSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

//-------------------------------------------------------------------------

// Конструктор
FunctionSymbol::FunctionSymbol(std::string name, TypeSymbol* type)
	: ItemSymbol(name, type)
{
}

// Деструктор
FunctionSymbol::~FunctionSymbol()
{
	for (int i = 0; i < params.size(); i++)
		delete params[i];
	params.clear();
}

// Посетить символ
void FunctionSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}

// Добавить параметр
void FunctionSymbol::addParam(VariableSymbol* param)
{
	params.push_back(param);
}

