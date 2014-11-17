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
	// Признак типа
	virtual bool isType() { return false; }
	// Признак левостороннего операнда - переменной
	virtual bool isLvalue() { return false; }
	// Признак изменяемой переменной
	virtual bool isModificableLvalue() { return false; }
	// Название типа
	std::string name;	
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
	// Получить символ по индексу
	Symbol* operator [] (int n) { return symbols[n]; }
	// Получить количество
	int count() { return symbols.size(); }
	// Получить длину
	int getLength();

	// название таблицы
	std::string name;
	// Список типов
	std::vector<Symbol*> symbols;
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
	TypeSymbol(std::string name);
	// Конструктор
	TypeSymbol(TypeSymbol* baseType, int mode);
	// Деструктор
	virtual ~TypeSymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
	// Признак типа
	virtual bool isType() { return true; }
	// Признак константы
	bool isConst() { return mode == MODE_CONST; }
	// Признак указателя
	bool isPointer() { return mode == MODE_POINTER; }
	// Признак псевдонима
	virtual bool isAlias() { return false; }
	// Признак структуры
	virtual bool isStruct() { return false; }	
	// Признак массива
	virtual bool isArray() { return false; }	
	// Признак скалярного типа
	bool isScalar() { return isChar() || isInt() || isFloat(); }
	// Признак типа char
	bool isChar() { return name == "char" || name == "const char"; }
	// Признак типа int
	bool isInt() { return name == "int" || name == "const int"; }
	// Признак типа float
	bool isFloat() { return name == "float" || name == "const float"; }
	// Признак пустого типа
	bool isVoid() { return name == "void"; }
	// Сравнение двух типов
	bool operator == (const TypeSymbol& type);
	// Проверка возможности конвертирования
	virtual bool canConvertTo(TypeSymbol* to);
	// Получить длину типа
	virtual int getLength();
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
	// Проверка возможности конвертирования
	virtual bool canConvertTo(TypeSymbol* to);
	// Признак псевдонима
	virtual bool isAlias() { return true; }
	// Получить длину типа
	virtual int getLength();
};

//-------------------------------------------------------------------------

class VariableSymbol;

// Класс типа структуры
class StructSymbol : public TypeSymbol
{
	static int nameCount;
public:
	// Конструктор
	StructSymbol(std::string name);
	// Деструктор
	virtual ~StructSymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
	// Проверка возможности конвертирования
	virtual bool canConvertTo(TypeSymbol* to);
	// Признак структуры
	virtual bool isStruct() { return true; }
	// Получить длину типа
	virtual int getLength();
	// Поля структуры
	SymbolsTable fields;
};

//-------------------------------------------------------------------------

// Класс массива
class ArraySymbol : public TypeSymbol
{
	static int nameCount;
public:
	// Конструктор
	ArraySymbol(TypeSymbol* baseType, int count);
	// Деструктор
	virtual ~ArraySymbol();
	// Посетить символ
	virtual void visit(ISymbolVisitor* visitor);
	// Проверка возможности конвертирования
	virtual bool canConvertTo(TypeSymbol* to);
	// Признак массива
	virtual bool isArray() { return true; }
	// Получить длину типа
	virtual int getLength();
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
	// Признак константы
	virtual bool isConstant() { return false; }
	// Признак переменной
	virtual bool isVariable() { return false; }
	// Признак функции
	virtual bool isFunction() { return false; }
	// Тип значения
	TypeSymbol* type;
	// Имя в коде программы
	std::string asmName;
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
	// Признак константы
	virtual bool isConstant() { return true; }	
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
	// Признак левостороннего операнда - переменной
	virtual bool isLvalue() { return true; }
	// Признак изменяемой переменной
	virtual bool isModificableLvalue() { return !type->isConst(); }
	// Признак переменной
	virtual bool isVariable() { return true; }
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
	// Признак функции
	virtual bool isFunction() { return true; }
	// Параметры функции
	SymbolsTable params;
	// Признак бесконечного количества параметров
	bool isVarParams;
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