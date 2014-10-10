#pragma once

class ISymbolVisitor;

//-------------------------------------------------------------------------

// ����� �������
class Symbol
{
public:
	// �����������
	Symbol();
	// �����������
	Symbol(std::string name);
	// ����������
	virtual ~Symbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
	// ������� ����
	virtual bool isType() { return false; }
	// ������� �������������� �������� - ����������
	virtual bool isLvalue() { return false; }
	// ������� ���������� ����������
	virtual bool isModificableLvalue() { return false; }
	// �������� ����
	std::string name;
};

//-------------------------------------------------------------------------

// ����� ������� ����
class SymbolsTable
{
public:
	// �����������
	SymbolsTable();
	// ����������
	virtual ~SymbolsTable();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
	// �������� ������
	void addSymbol(Symbol* symbol);
	// ����� ������� �� �����
	Symbol* findByName(std::string name);
	// ����� � ����������
	Symbol* findOrAdd(Symbol* symbol);
	// �������� ������ �� �������
	Symbol* operator [] (int n) { return symbols[n]; }
	// �������� ����������
	int count() { return symbols.size(); }
	// ������ �����
	std::vector<Symbol*> symbols;
};

//-------------------------------------------------------------------------

// ������� ����� ��� �������� �����
class TypeSymbol : public Symbol
{
public:
	// ������������ ����
	enum
	{
		MODE_NONE,
		MODE_CONST,
		MODE_POINTER
	};
	// ������� ���
	static std::string makeName(TypeSymbol* baseType, int mode);
	// �����������
	TypeSymbol(std::string name, int length);
	// �����������
	TypeSymbol(TypeSymbol* baseType, int mode);
	// ����������
	virtual ~TypeSymbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
	// ������� ����
	virtual bool isType() { return true; }
	// ������� ���������
	bool isConst() { return mode == MODE_CONST; }
	// ������� ���������
	bool isPointer() { return mode == MODE_POINTER; }
	// ������� �������
	virtual bool isArray() { return false; }
	// ������� ���������
	virtual bool isStruct() { return false; }
	// ������� ����������
	virtual bool isAlias() { return false; }
	// ������� ���������� ����
	bool isScalar() { return isChar() || isInt() || isFloat(); }
	// ������� ���� char
	bool isChar() { return name == "char" || name == "const char"; }
	// ������� ���� int
	bool isInt() { return name == "int" || name == "const int"; }
	// ������� ���� float
	bool isFloat() { return name == "float" || name == "const float"; }
	// ������� ������� ����
	bool isVoid() { return name == "void"; }
	// ��������� ���� �����
	bool operator == (const TypeSymbol& type);
	// �������� ����������� ���������������
	virtual bool canConvertTo(TypeSymbol* to);
	// ����� ����
	int length;
	// ������� ���
	TypeSymbol * baseType;	
	// ����������� ���� (����� �������������� ������ ���� �����������)
	int mode;
};

//-------------------------------------------------------------------------

// ����� ����������
class AliasSymbol : public TypeSymbol
{
public:
	// �����������
	AliasSymbol(TypeSymbol* baseType, std::string name);
	// ����������
	virtual ~AliasSymbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
	// �������� ����������� ���������������
	virtual bool canConvertTo(TypeSymbol* to);
	// ������� ����������
	virtual bool isAlias() { return true; }
};

//-------------------------------------------------------------------------

class VariableSymbol;

// ����� ���� ���������
class StructSymbol : public TypeSymbol
{
	static int nameCount;
public:
	// �����������
	StructSymbol(std::string name);
	// ����������
	virtual ~StructSymbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
	// �������� ����������� ���������������
	virtual bool canConvertTo(TypeSymbol* to);
	// ������� ���������
	virtual bool isStruct() { return true; }	
	// ���� ���������
	SymbolsTable fields;
};

//-------------------------------------------------------------------------

// ����� �������
class ArraySymbol : public TypeSymbol
{
	static int nameCount;
public:
	// �����������
	ArraySymbol(TypeSymbol* baseType, int count);
	// ����������
	virtual ~ArraySymbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
	// �������� ����������� ���������������
	virtual bool canConvertTo(TypeSymbol* to);
	// ������� �������
	virtual bool isArray() { return true; }
	// ���������� ��������� � �������
	int count;
};

//-------------------------------------------------------------------------

// ������� ����� ��� ������� � ����������
class ItemSymbol : public Symbol
{
public:
	// �����������
	ItemSymbol(std::string name, TypeSymbol* type);
	// ����������
	virtual ~ItemSymbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
	// ��� ��������
	TypeSymbol* type;
};

//-------------------------------------------------------------------------

// ����� ���������
class ConstantSymbol : public ItemSymbol
{
public:
	// �����������
	ConstantSymbol(std::string name, TypeSymbol* type);
	// ����������
	virtual ~ConstantSymbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
};

//-------------------------------------------------------------------------

// ����� ����������
class VariableSymbol : public ItemSymbol
{
public:
	// �����������
	VariableSymbol(std::string name, TypeSymbol* type);
	// ����������
	virtual ~VariableSymbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
	// ������� �������������� �������� - ����������
	virtual bool isLvalue() { return true; }
	// ������� ���������� ����������
	virtual bool isModificableLvalue() { return !type->isConst(); }
};

//-------------------------------------------------------------------------

// ����� �������
class FunctionSymbol : public ItemSymbol
{
public:
	// �����������
	FunctionSymbol(std::string name, TypeSymbol* type);
	// ����������
	virtual ~FunctionSymbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
	// ��������� �������
	SymbolsTable params;
};

//-------------------------------------------------------------------------

// ��������� ���������� ��� ��������
class ISymbolVisitor
{
public:
	// �������� ������� ��������
	virtual void OnSymbol(SymbolsTable* table) = 0;
	// �������� ������
	virtual void OnSymbol(Symbol* symbol) = 0;
	// �������� ���
	virtual void OnSymbol(TypeSymbol* symbol) = 0;
	// �������� ���������
	virtual void OnSymbol(AliasSymbol* symbol) = 0;
	// �������� ���������
	virtual void OnSymbol(StructSymbol* symbol) = 0;
	// �������� ������
	virtual void OnSymbol(ArraySymbol* symbol) = 0;
	// �������� ������
	virtual void OnSymbol(ItemSymbol* symbol) = 0;
	// �������� ���������
	virtual void OnSymbol(ConstantSymbol* symbol) = 0;
	// �������� ����������
	virtual void OnSymbol(VariableSymbol* symbol) = 0;
	// �������� �������
	virtual void OnSymbol(FunctionSymbol* symbol) = 0;
};