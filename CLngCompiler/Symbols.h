#pragma once

class ISymbolVisitor;

//-------------------------------------------------------------------------

// Класс символа
class Symbol
{
public:
	// Конструктор
	Symbol();
	// Конструктор
	Symbol(std::string name);
	// Деструктор
	virtual ~Symbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
	// Название типа
	std::string name;
};

//-------------------------------------------------------------------------

// базовый класс для символов типов
class TypeSymbol : public Symbol
{
public:
	// Модификаторы типа
	enum
	{
		MODE_NONE,
		MODE_CONST,
		MODE_POINTER
	};
	// Создать имя
	static std::string makeName(TypeSymbol* baseType, int mode);
	// Конструктор
	TypeSymbol(std::string name, int length);
	// Конструктор
	TypeSymbol(TypeSymbol* baseType, int mode);
	// Деструктор
	virtual ~TypeSymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
	// Признак константы
	bool isConst() { return mode == MODE_CONST; }
	// Признак указателя
	bool isPointer() { return mode == MODE_POINTER; }
	// Признак массива
	virtual bool isArray() { return false; }
	// Признак структуры
	virtual bool isStruct() { return false; }
	// Признак псевдонима
	virtual bool isAlias() { return false; }
	// Признак пустого типа
	bool isVoid() { return name == "void"; }
	// Длина типа
	int length;
	// базовый тип
	TypeSymbol * baseType;	
	// Модификатор типа (может использоваться только один модификатор)
	int mode;
};

//-------------------------------------------------------------------------

// Класс псевдонима
class AliasSymbol : public TypeSymbol
{
public:
	// Конструктор
	AliasSymbol(TypeSymbol* baseType, std::string name);
	// Деструктор
	virtual ~AliasSymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
};

//-------------------------------------------------------------------------

class VariableSymbol;

// Класс типа структуры
class StructSymbol : public TypeSymbol
{
public:
	// Конструктор
	StructSymbol(std::string name);
	// Деструктор
	virtual ~StructSymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
	// Добавить поле
	void addField(VariableSymbol* field);
	// Поля структуры
	std::vector<VariableSymbol*> fields;
};

//-------------------------------------------------------------------------

// Класс массива
class ArraySymbol : public TypeSymbol
{
public:
	// Конструктор
	ArraySymbol(TypeSymbol* baseType, int count);
	// Деструктор
	virtual ~ArraySymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
	// Признак массива
	virtual bool isArray() { return true; }
	// Количество элементов в массиве
	int count;
};

//-------------------------------------------------------------------------

// базовый класс для функции и переменной
class ItemSymbol : public Symbol
{
public:
	// Конструктор
	ItemSymbol(std::string name, TypeSymbol* type);
	// Деструктор
	virtual ~ItemSymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
	// Тип значения
	TypeSymbol* type;
};

//-------------------------------------------------------------------------

// Класс константы
class ConstantSymbol : public ItemSymbol
{
public:
	// Конструктор
	ConstantSymbol(std::string name, TypeSymbol* type);
	// Деструктор
	virtual ~ConstantSymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
};

//-------------------------------------------------------------------------

// Класс переменной
class VariableSymbol : public ItemSymbol
{
public:
	// Конструктор
	VariableSymbol(std::string name, TypeSymbol* type);
	// Деструктор
	virtual ~VariableSymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
};

//-------------------------------------------------------------------------

// Класс функции
class FunctionSymbol : public ItemSymbol
{
public:
	// Конструктор
	FunctionSymbol(std::string name, TypeSymbol* type);
	// Деструктор
	virtual ~FunctionSymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
	// Добавить параметр
	void addParam(VariableSymbol* param);
	// Параметры функции
	std::vector<VariableSymbol*> params;
};

//-------------------------------------------------------------------------

// Класс таблицы имен
class SymbolsTable
{
public:
	// Конструктор
	SymbolsTable();
	// Деструктор
	virtual ~SymbolsTable();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
	// добавить символ
	void addSymbol(Symbol* symbol);
	// Поиск символа по имени
	Symbol* findByName(std::string name);
	// Поиск и добавление
	Symbol* findOrAdd(Symbol* symbol);
	// Список типов
	std::vector<Symbol*> symbols;
};

//-------------------------------------------------------------------------

// Интерфейс посетителя для символов
class ISymbolVisitor
{
public:
	// Посетить таблицу символов
	virtual void OnSymbol(SymbolsTable* table) = 0;
	// Посетить символ
	virtual void OnSymbol(Symbol* symbol) = 0;
	// Посетить тип
	virtual void OnSymbol(TypeSymbol* symbol) = 0;
	// Посетить псевдоним
	virtual void OnSymbol(AliasSymbol* symbol) = 0;
	// Посетить структуру
	virtual void OnSymbol(StructSymbol* symbol) = 0;
	// Посетить массив
	virtual void OnSymbol(ArraySymbol* symbol) = 0;
	// Посетить символ
	virtual void OnSymbol(ItemSymbol* symbol) = 0;
	// Посетить константу
	virtual void OnSymbol(ConstantSymbol* symbol) = 0;
	// Посетить переменную
	virtual void OnSymbol(VariableSymbol* symbol) = 0;
	// Посетить функцию
	virtual void OnSymbol(FunctionSymbol* symbol) = 0;
};