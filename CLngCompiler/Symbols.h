#pragma once

class ISymbolVisitor;

//-------------------------------------------------------------------------

// Класс символа
class Symbol
{
public:
	Symbol();
	Symbol(std::string name);
	virtual ~Symbol();
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
	SymbolsTable();
	virtual ~SymbolsTable();
	virtual void visit(ISymbolVisitor* visitor);
	void addSymbol(Symbol* symbol);
	Symbol* findByName(std::string name);
	Symbol* findOrAdd(Symbol* symbol);
	Symbol* operator [] (int n) { return symbols[n]; }
	int count() { return symbols.size(); }
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
	static std::string makeName(TypeSymbol* baseType, int mode);
	TypeSymbol(std::string name);
	TypeSymbol(TypeSymbol* baseType, int mode);
	virtual ~TypeSymbol();
	virtual void visit(ISymbolVisitor* visitor);
	virtual bool isType() { return true; }
	bool isConst() { return mode == MODE_CONST; }
	bool isPointer() { return mode == MODE_POINTER; }
	virtual bool isAlias() { return false; }
	virtual bool isStruct() { return false; }	
	virtual bool isArray() { return false; }	
	bool isScalar() { return isChar() || isInt() || isFloat(); }
	bool isChar() { return name == "char" || name == "const char";} 
	bool isInt() { return name == "int" || name == "const int"; }
	bool isFloat() { return name == "float" || name == "const float"; }
	bool isVoid() { return name == "void"; }
	// Сравнение двух типов
	bool operator == (const TypeSymbol& type);
	virtual bool canConvertTo(TypeSymbol* to);
	virtual int getLength();
	TypeSymbol* baseType;	
	// Модификатор типа (может использоваться только один модификатор)
	int mode;
};

//-------------------------------------------------------------------------

// Класс псевдонима
class AliasSymbol : public TypeSymbol
{
public:
	AliasSymbol(TypeSymbol* baseType, std::string name);
	virtual ~AliasSymbol();
	virtual void visit(ISymbolVisitor* visitor);
	virtual bool canConvertTo(TypeSymbol* to);
	virtual bool isAlias() { return true; }
	virtual int getLength();
};

//-------------------------------------------------------------------------

class VariableSymbol;

// Класс типа структуры
class StructSymbol : public TypeSymbol
{
	static int nameCount;
public:
	StructSymbol(std::string name);
	virtual ~StructSymbol();
	virtual void visit(ISymbolVisitor* visitor);
	virtual bool canConvertTo(TypeSymbol* to);
	virtual bool isStruct() { return true; }
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
	ArraySymbol(TypeSymbol* baseType, int count);
	virtual ~ArraySymbol();
	virtual void visit(ISymbolVisitor* visitor);
	virtual bool canConvertTo(TypeSymbol* to);
	virtual bool isArray() { return true; }
	virtual int getLength();
	// Количество элементов в массиве
	int count;
};

//-------------------------------------------------------------------------

// базовый класс для функции и переменной
class ItemSymbol : public Symbol
{
public:
	ItemSymbol(std::string name, TypeSymbol* type);
	virtual ~ItemSymbol();
	virtual void visit(ISymbolVisitor* visitor);
	virtual bool isConstant() { return false; }
	virtual bool isVariable() { return false; }
	virtual bool isFunction() { return false; }
	TypeSymbol* type;
	// Имя в коде программы
	std::string asmName;
};

//-------------------------------------------------------------------------

// Класс константы
class ConstantSymbol : public ItemSymbol
{
public:
	ConstantSymbol(std::string name, TypeSymbol* type);
	virtual ~ConstantSymbol();
	virtual void visit(ISymbolVisitor* visitor);
	virtual bool isConstant() { return true; }	
};

//-------------------------------------------------------------------------

class VariableSymbol : public ItemSymbol
{
public:
	VariableSymbol(std::string name, TypeSymbol* type);
	virtual ~VariableSymbol();
	virtual void visit(ISymbolVisitor* visitor);
	// Признак левостороннего операнда - переменной
	virtual bool isLvalue() { return true; }
	// Признак изменяемой переменной
	virtual bool isModificableLvalue() { return !type->isConst(); }
	virtual bool isVariable() { return true; }
};

//-------------------------------------------------------------------------

// Класс функции
class FunctionSymbol : public ItemSymbol
{
public:
	FunctionSymbol(std::string name, TypeSymbol* type);
	virtual ~FunctionSymbol();
	virtual void visit(ISymbolVisitor* visitor);
	virtual bool isFunction() { return true; }
	SymbolsTable params;
	bool isVarParams;
};

//-------------------------------------------------------------------------

// Интерфейс посетителя для символов
class ISymbolVisitor
{
public:
	virtual void OnSymbol(SymbolsTable* table) = 0;
	virtual void OnSymbol(Symbol* symbol) = 0;
	virtual void OnSymbol(TypeSymbol* symbol) = 0;
	virtual void OnSymbol(AliasSymbol* symbol) = 0;
	virtual void OnSymbol(StructSymbol* symbol) = 0;
	virtual void OnSymbol(ArraySymbol* symbol) = 0;
	virtual void OnSymbol(ItemSymbol* symbol) = 0;
	virtual void OnSymbol(ConstantSymbol* symbol) = 0;
	virtual void OnSymbol(VariableSymbol* symbol) = 0;
	virtual void OnSymbol(FunctionSymbol* symbol) = 0;
};