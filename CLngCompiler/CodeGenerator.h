#pragma once

#include "Symbols.h"
#include "Nodes.h"
#include "Asm.h"

class CodeGenerator : public ISymbolVisitor, public INodeVisitor
{
public:
	CodeGenerator();
	~CodeGenerator();
	AsmProg* generate(ProgramNode* prog);
	virtual void OnSymbol(SymbolsTable* table);
	virtual void OnSymbol(Symbol* symbol);
	virtual void OnSymbol(TypeSymbol* symbol);
	virtual void OnSymbol(AliasSymbol* symbol);
	virtual void OnSymbol(StructSymbol* symbol);
	virtual void OnSymbol(ArraySymbol* symbol);
	virtual void OnSymbol(ItemSymbol* symbol);
	virtual void OnSymbol(ConstantSymbol* symbol);
	virtual void OnSymbol(VariableSymbol* symbol);
	virtual void OnSymbol(FunctionSymbol* symbol);
	virtual void OnNode(Node* node);
	virtual void OnNode(ProgramNode* node);
	virtual void OnNode(ExpressionNode* node);	
	virtual void OnNode(ConditionalNode* node);
	virtual void OnNode(BinaryOpNode* node);
	virtual void OnNode(UnaryOpNode* node);	
	virtual void OnNode(ValueNode* node);
	virtual void OnNode(IdentifierNode* node);
	virtual void OnNode(ArrayNode* node);
	virtual void OnNode(FuncCallNode* node);
	virtual void OnNode(CompoundNode* node);
	virtual void OnNode(FunctionNode* node);
	virtual void OnNode(IfNode* node);
	virtual void OnNode(WhileNode* node);
	virtual void OnNode(DoNode* node);
	virtual void OnNode(ForNode* node);
	virtual void OnNode(BreakNode* node);
	virtual void OnNode(ContinueNode* node);
	virtual void OnNode(ReturnNode* node);

private:

	// �������� �������
	void code(int cmd);
	void code(int cmd, AsmArg* arg1);
	void code(int cmd, AsmArg* arg1, AsmArg* arg2);
	// �������� �����
	void label(std::string name);
	// ������� ��������
	AsmArg* valArg(std::string val);
	AsmArg* regArg(int reg);
	AsmArg* memArg(int reg, int offset = 0);
	AsmArg* memArg(std::string name);
	AsmArg* offArg(std::string name);
	void value(std::string name, std::string type, std::string value);
	void var(std::string name, std::string type, int count);

	// ��������� �� ����������
	AsmProg* asmProg;
	// ������� ���������
	AsmProc* asmProc;
	// ������� ��������
	int valCount;
	// ������� �����
	int labelCount;
	// ������� ������ ��������
	int tableCount;
	// ������� ������� ���������� �������
	bool isParams;
	// ������� ��������� ���� ��� lvalue
	bool lvalue;
	// ������� ������������� ������������
	bool useFPU;
	// ���� ����� ��� ������ �� �����
	std::vector<std::string> breakStack;
	// ���� ����� ��� ���������� �����
	std::vector<std::string> continueStack;
	// ���� ����� ��� ������ �� �������
	std::vector<std::string> returnStack;
};