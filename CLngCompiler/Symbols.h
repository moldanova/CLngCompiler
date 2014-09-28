#pragma once

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
	// ����� �������
	virtual void print(int spaces);
	// �������� ����
	std::string name;
};

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
	// ����� ����
	int length;
	// ������� ���
	TypeSymbol * baseType;	
	// ����������� ���� (����� �������������� ������ ���� �����������)
	int mode;
};


// ����� ����������
class AliasSymbol : public TypeSymbol
{
public:
	// �����������
	AliasSymbol(TypeSymbol* baseType, std::string name);
	// ����������
	virtual ~AliasSymbol();
	// ����� �������
	virtual void print(int spaces);
};

// ������� ����� ��� ������� � ����������
class ItemSymbol : public Symbol
{
public:
	// �����������
	ItemSymbol(std::string name, TypeSymbol* type);
	// ����������
	virtual ~ItemSymbol();
	// ��� ��������
	TypeSymbol* type;
};

// ����� �������
class ArraySymbol : public TypeSymbol
{
public:
	// �����������
	ArraySymbol(TypeSymbol* baseType, int count);
	// ����������
	virtual ~ArraySymbol();
	// ����� �������
	virtual void print(int spaces);
	// ������� �������
	virtual bool isArray() { return true; }
	// ���������� ��������� � �������
	int count;
};

// ����� ����������
class VariableSymbol : public ItemSymbol
{
public:
	// �����������
	VariableSymbol(std::string name, TypeSymbol* type);
	// ����������
	virtual ~VariableSymbol();
	// ����� �������
	virtual void print(int spaces);
};

// ����� ���� ���������
class StructSymbol : public TypeSymbol
{
public:
	// �����������
	StructSymbol(std::string name);
	// ����������
	virtual ~StructSymbol();
	// ����� �������
	virtual void print(int spaces);
	// �������� ����
	void addField(VariableSymbol* field);
	// ���� ���������
	std::vector<VariableSymbol*> fields;
};

// ����� �������
class FunctionSymbol : public ItemSymbol
{
public:
	// �����������
	FunctionSymbol(std::string name, TypeSymbol* type);
	// ����������
	virtual ~FunctionSymbol();
	// ����� �������
	virtual void print(int spaces);
	// �������� ��������
	void addParam(VariableSymbol* param);
	// ��������� �������
	std::vector<VariableSymbol*> params;
};

// ����� ������� ����
class SymbolsTable
{
public:
	// �����������
	SymbolsTable();
	// ����������
	virtual ~SymbolsTable();
	// ����� ������� ��������
	void print();
	// �������� ������
	void addSymbol(Symbol* symbol);
	// ����� ������� �� �����
	Symbol* findByName(std::string name);
	// ����� � ����������
	Symbol* findOrAdd(Symbol* symbol);
	// ������ �����
	std::vector<Symbol*> symbols;
};
