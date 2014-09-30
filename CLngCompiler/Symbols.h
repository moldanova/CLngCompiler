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
	// �������� ����
	std::string name;
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
	// ������� ������� ����
	bool isVoid() { return name == "void"; }
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
};

//-------------------------------------------------------------------------

class VariableSymbol;

// ����� ���� ���������
class StructSymbol : public TypeSymbol
{
public:
	// �����������
	StructSymbol(std::string name);
	// ����������
	virtual ~StructSymbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
	// �������� ����
	void addField(VariableSymbol* field);
	// ���� ���������
	std::vector<VariableSymbol*> fields;
};

//-------------------------------------------------------------------------

// ����� �������
class ArraySymbol : public TypeSymbol
{
public:
	// �����������
	ArraySymbol(TypeSymbol* baseType, int count);
	// ����������
	virtual ~ArraySymbol();
	// �������� ������
	virtual void visit(ISymbolVisitor* visitor);
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
	// �������� ��������
	void addParam(VariableSymbol* param);
	// ��������� �������
	std::vector<VariableSymbol*> params;
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
	// ������ �����
	std::vector<Symbol*> symbols;
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