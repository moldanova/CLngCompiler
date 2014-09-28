#include "stdafx.h"
#include "Symbols.h"

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

// Вывод названия типа
void Symbol::print(int spaces)
{
	std::cout << name;
}

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

// Вывод символа
void AliasSymbol::print(int spaces)
{
	std::cout << "typedef ";
	baseType->print(spaces);
	std::cout << " " << name;
}

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

// Вывод символа
void ArraySymbol::print(int spaces)
{
	baseType->print(spaces);
	std::cout << "[" << count << "]";
}

// Конструктор
VariableSymbol::VariableSymbol(std::string name, TypeSymbol* type)
	: ItemSymbol(name, type)
{
}

// Деструктор
VariableSymbol::~VariableSymbol()
{
}

// Вывод символа
void VariableSymbol::print(int spaces)
{
	if (type->isArray())
		type->baseType->print(spaces);
	else
		type->print(spaces);

	std::cout << name;

	if (type->isArray())
		std::cout << "[" << ((ArraySymbol*)type)->count << "]";
}

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

// Вывод символа
void StructSymbol::print(int spaces)
{
	std::cout << name << " {" << std::endl;
	for (int i = 0; i < fields.size(); i++)
	{
		std::cout << std::string(spaces + 4, ' ');
		fields[i]->print(spaces + 4);
	}
	std::cout << std::string(spaces, ' ') << "}";
}


// Добавить поле
void StructSymbol::addField(VariableSymbol* field)
{
	fields.push_back(field);
	length += field->type->length;
}

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

// Вывод символа
void FunctionSymbol::print(int spaces)
{
	type->print(spaces);
	std::cout << name << "(";
	for (int i = 0; i < params.size(); i++)
		params[i]->print(spaces + 4);
	std::cout << ")";
}

// Добавить параметр
void FunctionSymbol::addParam(VariableSymbol* param)
{
	params.push_back(param);
}

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

// Вывод таблицы символов
void SymbolsTable::print()
{
	for (int i = 0; i < symbols.size(); i++)
	{
		symbols[i]->print(0);
		std::cout << ";" << std::endl;
	}
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

