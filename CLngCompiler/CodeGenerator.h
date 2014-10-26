#pragma once

#include "Symbols.h"
#include "Nodes.h"
#include "Asm.h"

class CodeGenerator : public ISymbolVisitor, public INodeVisitor
{
public:
	// �����������
	CodeGenerator();
	// ����������
	~CodeGenerator();
	// ������������ ���
	AsmProg* generate(ProgramNode* prog);
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
	// �������� ����
	virtual void OnNode(CompoundNode* node);
	// �������� ���� �������
	virtual void OnNode(FunctionNode* node);
	// �������� ����
	virtual void OnNode(IfNode* node);
	// �������� ����
	virtual void OnNode(WhileNode* node);
	// �������� ����
	virtual void OnNode(DoNode* node);
	// �������� ����
	virtual void OnNode(ForNode* node);
	// �������� ����
	virtual void OnNode(BreakNode* node);
	// �������� ����
	virtual void OnNode(ContinueNode* node);
	// �������� ����
	virtual void OnNode(ReturnNode* node);

private:

	// �������� �������
	void add(int cmd);
	// �������� �������
	void add(int cmd, AsmArg* arg1);
	// �������� �������
	void add(int cmd, AsmArg* arg1, AsmArg* arg2);
	// �������� �������
	void add(int cmd, std::string name);
	// ������� ��������
	AsmArg* makeArg(int arg);
	// ������� ��������
	AsmArg* makeArg(int arg, std::string val);
	// �������� ����������
	void addVar(std::string name, int type, std::string value);
	// �������� ����������
	void addVar(std::string name, int type, int count);

	// ��������� �� ����������
	AsmProg* asmProg;
};