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

// Получить длину
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
TypeSymbol::TypeSymbol(std::string name)
	: Symbol(name)
{
	baseType = NULL;
	mode = MODE_NONE;
}

// Конструктор
TypeSymbol::TypeSymbol(TypeSymbol* baseType, int mode)
	: Symbol(makeName(baseType, mode))
{
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
	// Если результирующий тип константный
	if (to->isConst())
		return canConvertTo(to->baseType);
	// Если оба типа указатели на указатели
	if (isPointer() && baseType->isPointer() && to->isPointer() && to->baseType->isPointer())
		return baseType->canConvertTo(to->baseType);
	// Совместимость указателя и массива
	if (isPointer() && to->isArray() && *baseType == *(to->baseType))	
		return true;
	// Правила совместимости числовых значений
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

	return false;
}

// Получить длину типа
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

// Конструктор
AliasSymbol::AliasSymbol(TypeSymbol* baseType, std::string name)
	: TypeSymbol(name)
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

// Получить длину типа
int AliasSymbol::getLength()
{
	return baseType->getLength();
}

//-------------------------------------------------------------------------

int StructSymbol::nameCount = 0;

// Конструктор
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

// Деструктор
StructSymbol::~StructSymbol()
{

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

// Получить длину типа
int StructSymbol::getLength()
{
	return fields.getLength();
}

//-------------------------------------------------------------------------

int ArraySymbol::nameCount = 0;

// Конструктор
ArraySymbol::ArraySymbol(TypeSymbol* baseType, int count)
	: TypeSymbol(baseType, TypeSymbol::MODE_NONE)
{
	nameCount++;
	this->name = "array";
	this->name += std::to_string(nameCount);
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
	if (*this == *to)
		return true;
	if (to->isPointer() && *baseType == *(to->baseType))
		return true;	
	return false;
}

// Получить длину типа
int ArraySymbol::getLength()
{
	return count * baseType->getLength();
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

}

// Посетить символ
void FunctionSymbol::visit(ISymbolVisitor* visitor)
{
	visitor->OnSymbol(this);
}


