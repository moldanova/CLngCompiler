#include "stdafx.h"
#include "CodeGenerator.h"

// �����������
CodeGenerator::CodeGenerator()
{

}

// ����������
CodeGenerator::~CodeGenerator()
{

}

// ������������ ���
AsmProg* CodeGenerator::generate(ProgramNode* prog)
{
	asmProg = new AsmProg();

	// ����������� ���������� ����������
	prog->globals.visit(this);

	// ����������� �������
	for (int i = 0; i < prog->nodes.size(); i++)
	{
		FunctionNode* fnode = dynamic_cast<FunctionNode*>(prog->nodes[i]);
		if (fnode)
			fnode->visit(this);
	}

	// ����������� ���� ��������� ���
	add(cmdLABEL, "start");
	for (int i = 0; i < prog->nodes.size(); i++)
	{
		Node* node = prog->nodes[i];
		FunctionNode* fnode = dynamic_cast<FunctionNode*>(node);
		if (!fnode)
			node->visit(this);
	}
	// ��������� ����� ������� main
	add(cmdCALL, "f_main");
	// ����� �� ���������
	add(cmdRET, makeArg(argVAL, "0"));

	return asmProg;
}

// �������� ������� ��������
void CodeGenerator::OnSymbol(SymbolsTable* table)
{
	for (int i = 0; i < table->symbols.size(); i++)
	{
		Symbol* symbol = table->symbols[i];
		VariableSymbol* var = dynamic_cast<VariableSymbol*>(symbol);
		if (var)
			addVar("var_" + var->name, varDD, var->type->getLength() / 4);
	}
}

// �������� ������
void CodeGenerator::OnSymbol(Symbol* symbol)
{
	throw std::exception("Internal program error");
}

// �������� ���
void CodeGenerator::OnSymbol(TypeSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// �������� ���������
void CodeGenerator::OnSymbol(AliasSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// �������� ���������
void CodeGenerator::OnSymbol(StructSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// �������� ������
void CodeGenerator::OnSymbol(ArraySymbol* symbol)
{
	throw std::exception("Internal program error");
}

// �������� ������
void CodeGenerator::OnSymbol(ItemSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// �������� ���������
void CodeGenerator::OnSymbol(ConstantSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// �������� ����������
void CodeGenerator::OnSymbol(VariableSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// �������� �������
void CodeGenerator::OnSymbol(FunctionSymbol* symbol)
{
	throw std::exception("Internal program error");
}

// �������� ����
void CodeGenerator::OnNode(Node* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� ���������
void CodeGenerator::OnNode(ProgramNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� ���������
void CodeGenerator::OnNode(ExpressionNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� ��������� ����������
void CodeGenerator::OnNode(ConditionalNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� �������� ��������
void CodeGenerator::OnNode(BinaryOpNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� ������� ��������
void CodeGenerator::OnNode(UnaryOpNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� ��������
void CodeGenerator::OnNode(ValueNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� ��������������
void CodeGenerator::OnNode(IdentifierNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� ��������� � �������
void CodeGenerator::OnNode(ArrayNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� ������ �������
void CodeGenerator::OnNode(FuncCallNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ����
void CodeGenerator::OnNode(CompoundNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ���� �������
void CodeGenerator::OnNode(FunctionNode* node)
{
	// �������� �������
	std::string name = "f_" + node->symbol->name;
	// ����� � ������
	add(cmdLABEL, name);
	add(cmdPUSH, makeArg(argEBP));
	add(cmdMOV, makeArg(argEBP), makeArg(argESP));

	// �������� ������ ��� ��������� ����������

	// ����������� ���� �������
	//node->statement->visit(this);

	// ��������� �������
	add(cmdLABEL, name + "_end");
	add(cmdMOV, makeArg(argESP), makeArg(argEBP));
	add(cmdPOP, makeArg(argEBP));
	add(cmdRET, makeArg(argVAL, "0"));
}

// �������� ����
void CodeGenerator::OnNode(IfNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ����
void CodeGenerator::OnNode(WhileNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ����
void CodeGenerator::OnNode(DoNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ����
void CodeGenerator::OnNode(ForNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ����
void CodeGenerator::OnNode(BreakNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ����
void CodeGenerator::OnNode(ContinueNode* node)
{
	throw std::exception("Internal program error");
}

// �������� ����
void CodeGenerator::OnNode(ReturnNode* node)
{
	throw std::exception("Internal program error");
}

// �������� �������
void CodeGenerator::add(int cmd)
{
	asmProg->addCmd(new AsmCmd(cmd));
}

// �������� �������
void CodeGenerator::add(int cmd, AsmArg* arg1)
{
	asmProg->addCmd(new AsmCmd(cmd, arg1));
}

// �������� �������
void CodeGenerator::add(int cmd, AsmArg* arg1, AsmArg* arg2)
{
	asmProg->addCmd(new AsmCmd(cmd, arg1, arg2));
}

// �������� �������
void CodeGenerator::add(int cmd, std::string name)
{
	asmProg->addCmd(new AsmCmd(cmd, name));
}

// ������� ��������
AsmArg* CodeGenerator::makeArg(int arg)
{
	return new AsmArg(arg);
}

// ������� ��������
AsmArg* CodeGenerator::makeArg(int arg, std::string val)
{
	return new AsmArg(arg, val);
}

// �������� ����������
void CodeGenerator::addVar(std::string name, int type, std::string value)
{
	asmProg->addVar(new AsmVar(name, type, value));
}

// �������� ����������
void CodeGenerator::addVar(std::string name, int type, int count)
{
	asmProg->addVar(new AsmVar(name, type, count));
}