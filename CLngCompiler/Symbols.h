#pragma once

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
	// Вывод символа
	virtual void print(int spaces);
	// Название типа
	std::string name;
};

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
	// Длина типа
	int length;
	// базовый тип
	TypeSymbol * baseType;	
	// Модификатор типа (может использоваться только один модификатор)
	int mode;
};


// Класс псевдонима
class AliasSymbol : public TypeSymbol
{
public:
	// Конструктор
	AliasSymbol(TypeSymbol* baseType, std::string name);
	// Деструктор
	virtual ~AliasSymbol();
	// Вывод символа
	virtual void print(int spaces);
};

// базовый класс для функции и переменной
class ItemSymbol : public Symbol
{
public:
	// Конструктор
	ItemSymbol(std::string name, TypeSymbol* type);
	// Деструктор
	virtual ~ItemSymbol();
	// Тип значения
	TypeSymbol* type;
};

// Класс массива
class ArraySymbol : public TypeSymbol
{
public:
	// Конструктор
	ArraySymbol(TypeSymbol* baseType, int count);
	// Деструктор
	virtual ~ArraySymbol();
	// Вывод символа
	virtual void print(int spaces);
	// Признак массива
	virtual bool isArray() { return true; }
	// Количество элементов в массиве
	int count;
};

// Класс переменной
class VariableSymbol : public ItemSymbol
{
public:
	// Конструктор
	VariableSymbol(std::string name, TypeSymbol* type);
	// Деструктор
	virtual ~VariableSymbol();
	// Вывод символа
	virtual void print(int spaces);
};

// Класс типа структуры
class StructSymbol : public TypeSymbol
{
public:
	// Конструктор
	StructSymbol(std::string name);
	// Деструктор
	virtual ~StructSymbol();
	// Вывод символа
	virtual void print(int spaces);
	// Добавить поле
	void addField(VariableSymbol* field);
	// Поля структуры
	std::vector<VariableSymbol*> fields;
};

// Класс функции
class FunctionSymbol : public ItemSymbol
{
public:
	// Конструктор
	FunctionSymbol(std::string name, TypeSymbol* type);
	// Деструктор
	virtual ~FunctionSymbol();
	// Вывод символа
	virtual void print(int spaces);
	// Добавить параметр
	void addParam(VariableSymbol* param);
	// Параметры функции
	std::vector<VariableSymbol*> params;
};

// Класс таблицы имен
class SymbolsTable
{
public:
	// Конструктор
	SymbolsTable();
	// Деструктор
	virtual ~SymbolsTable();
	// Вывод таблицы символов
	void print();
	// добавить символ
	void addSymbol(Symbol* symbol);
	// Поиск символа по имени
	Symbol* findByName(std::string name);
	// Поиск и добавление
	Symbol* findOrAdd(Symbol* symbol);
	// Список типов
	std::vector<Symbol*> symbols;
};
