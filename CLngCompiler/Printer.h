#pragma once

#include "Symbols.h"
#include "Node.h"

class Printer : public ISymbolVisitor, public INodeVisitor
{
public:
	// �����������
	Printer();
	// ����������
	virtual ~Printer();

	// �������� ������� ��������
	virtual void OnSymbol(SymbolsTable* table);
	// �������� ������
	virtual void OnSymbol(Symbol* symbol);
	// �������� ���
	virtual void OnSymbol(TypeSymbol* symbol);
	// �������� ���������
	virtual void OnSymbol(AliasSymbol* symbol);
	// �������� ���������
	virtual void OnSymbol(StructSymbol* symbol);
	// �������� ������
	virtual void OnSymbol(ArraySymbol* symbol);
	// �������� ������
	virtual void OnSymbol(ItemSymbol* symbol);
	// �������� ���������
	virtual void OnSymbol(ConstantSymbol* symbol);
	// �������� ����������
	virtual void OnSymbol(VariableSymbol* symbol);
	// �������� �������
	virtual void OnSymbol(FunctionSymbol* symbol);

	// �������� ����
	virtual void OnNode(Node* node);
	// �������� ���� ���������
	virtual void OnNode(ProgramNode* node);
	// �������� ���� ���������
	virtual void OnNode(ExpressionNode* node);
	// �������� ���� ��������� ����������
	virtual void OnNode(ConditionalNode* node);
	// �������� ���� �������� ��������
	virtual void OnNode(BinaryOpNode* node);
	// �������� ���� ������� ��������
	virtual void OnNode(UnaryOpNode* node);	
	// �������� ���� ��������
	virtual void OnNode(ValueNode* node);
	// �������� ���� ��������������
	virtual void OnNode(IdentifierNode* node);
	// �������� ���� ��������� � �������
	virtual void OnNode(ArrayNode* node);
	// �������� ���� ������ �������
	virtual void OnNode(FuncCallNode* node);

private:
	// ������
	int spaces;
	// ������� ������
	int tablesCount;
	// ����� ��������� �������� 
	bool shortName;
};
