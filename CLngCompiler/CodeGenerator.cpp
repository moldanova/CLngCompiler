#include "stdafx.h"
#include "CodeGenerator.h"
#include "Flag.h"
#include "Lexer.h"

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
	valCount = 0;
	labelCount = 0;
	lvalue = false;
	useFPU = false;

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
	add("start");
	for (int i = 0; i < prog->nodes.size(); i++)
	{
		Node* node = prog->nodes[i];
		FunctionNode* fnode = dynamic_cast<FunctionNode*>(node);
		if (!fnode)
			node->visit(this);
	}
	// ��������� ����� ������� main
	add(cmdCALL, makeValArg("f_main"));
	// ����� �� ���������
	add(cmdRET, makeValArg("0"));

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

bool IsAssignment(int lex)
{
	return lex == LEX_ASSIGNMENT 
		|| lex == LEX_MUL_ASSIGNMENT 
		|| lex == LEX_DIV_ASSIGNMENT 
		|| lex == LEX_PERSENT_ASSIGNMENT 
		|| lex == LEX_ADD_ASSIGNMENT 
		|| lex == LEX_SUB_ASSIGNMENT
		|| lex == LEX_LSHIFT_ASSIGNMENT 
		|| lex == LEX_RSHIFT_ASSIGNMENT 
		|| lex == LEX_AND_ASSIGNMENT
		|| lex == LEX_XOR_ASSIGNMENT 
		|| lex == LEX_OR_ASSIGNMENT;
}

// �������� ���� �������� ��������
void CodeGenerator::OnNode(BinaryOpNode* node)
{
	Flag lv(&lvalue, true);
	node->left->visit(this);		
	lv.reset();
	node->right->visit(this);
	add(cmdPOP, EBX);
	add(cmdPOP, EAX);

	if (node->lex == LEX_ASSIGNMENT)
		add(cmdMOV, makeMemArg(EAX), EBX);
	if (node->lex == LEX_ADD_ASSIGNMENT)
		add(cmdADD, makeMemArg(EAX), EBX);
	if (node->lex == LEX_SUB_ASSIGNMENT)
		add(cmdSUB, makeMemArg(EAX), EBX);
	if (node->lex == LEX_MUL_ASSIGNMENT)
		add(cmdMUL, makeMemArg(EAX), EBX);
	if (node->lex == LEX_DIV_ASSIGNMENT)
		add(cmdDIV, makeMemArg(EAX), EBX);
}

// �������� ���� ������� ��������
void CodeGenerator::OnNode(UnaryOpNode* node)
{
	Flag lv(&lvalue, true);
	node->right->visit(this);

	if (node->lex == LEX_AND)
		;
	else if (node->lex == LEX_MUL)
	{
		add(cmdPOP, EAX);
		add(cmdPUSH, makeMemArg(EAX));
	}
	else if (node->lex == LEX_SUB)
	{
		add(cmdPOP, EAX);
		add(cmdNEG, makeMemArg(EAX));
		add(cmdPUSH, makeMemArg(EAX));
	}
	else if (node->lex == LEX_INVERT)
	{
		add(cmdPOP, EAX);
		add(cmdNOT, makeMemArg(EAX));
		add(cmdPUSH, makeMemArg(EAX));
	}
	else if (node->lex == LEX_NOT)
	{
	}
}

// �������� ���� ��������
void CodeGenerator::OnNode(ValueNode* node)
{
	if (node->getType()->isPointer())
	{
		valCount++;
		addVar("value_" + std::to_string(valCount), varDB, node->lex.text);
		add(cmdPUSH, makeMemArg("value_" + std::to_string(valCount)));
	}
	else
	{
		add(cmdPUSH, makeValArg(node->lex.text));
	}
}

// �������� ���� ��������������
void CodeGenerator::OnNode(IdentifierNode* node)
{
	VariableSymbol* v = dynamic_cast<VariableSymbol*>(node->symbol);
	if (v)
	{
		// ��� ��������� � ����������
		if (lvalue)
		{
			add(cmdLEA, EAX, makeMemArg(node->lex.text));
			add(cmdPUSH, EAX);
		}
		else
		{
			add(cmdMOV, EAX, makeMemArg(node->lex.text));
			add(cmdPUSH, EAX);
		}
	}
}

// �������� ���� ��������� � �������
void CodeGenerator::OnNode(ArrayNode* node)
{
	Flag lv(&lvalue, true);
	node->var->visit(this);
	lv.reset();
	node->idx->visit(this);
	add(cmdPOP, EBX);
	add(cmdMUL, EBX, makeValArg(std::to_string(node->getType()->baseType->getLength())));
	add(cmdPOP, EAX);
	add(cmdADD, EAX, EBX);
	if (lvalue)
		add(cmdPUSH, EAX);
	else
		add(cmdPUSH, makeMemArg(EAX));
}

// �������� ���� ������ �������
void CodeGenerator::OnNode(FuncCallNode* node)
{
	add(cmdCALL, makeValArg(node->func->lex.text));
}

// �������� ����
void CodeGenerator::OnNode(CompoundNode* node)
{
	// �������� ������ ��� ��������� ����������
	int len = node->locals.getLength();
	if (len)
		add(cmdSUB, ESP, makeValArg(std::to_string(len)));

	// ���������� ��� ��������� �����
	for (int i = 0; i < node->nodes.size(); i++)
		node->nodes[i]->visit(this);

	// ������ ��������� ����������
	if (len)
		add(cmdADD, ESP, makeValArg(std::to_string(len)));
}

// �������� ���� �������
void CodeGenerator::OnNode(FunctionNode* node)
{
	// �������� �������
	std::string name = "f_" + node->symbol->name;
	// ����� � ������
	add(name);
	add(cmdPUSH, EBP);
	add(cmdMOV, EBP, ESP);

	// ����������� ���� �������
	node->statement->visit(this);

	// ��������� �������
	add(name + "_end");
	add(cmdMOV, ESP, EBP);
	add(cmdPOP, EBP);
	add(cmdRET, makeValArg("0"));
}

// �������� ����
void CodeGenerator::OnNode(IfNode* node)
{
	std::string label1 = "label" + std::to_string(labelCount++);
	std::string label2 = "label" + std::to_string(labelCount++);
	std::string label3 = "label" + std::to_string(labelCount++);
	node->expr->visit(this);
	add(cmdPOP, EAX);
	add(cmdCMP, EAX, makeValArg(0));
	add(cmdJNE, makeValArg(label1));
	add(cmdJE, makeValArg(label2));
	add(label1);
	node->statement1->visit(this);
	add(cmdJMP, makeValArg(label3));
	add(label2);
	if (node->statement2)
		node->statement2->visit(this);
	add(label3);
}

// �������� ����
void CodeGenerator::OnNode(WhileNode* node)
{
	std::string label1 = "label" + std::to_string(labelCount++);
	std::string label2 = "label" + std::to_string(labelCount++);

	continueStack.push_back(label1);
	breakStack.push_back(label2);

	add(label1);
	node->expr->visit(this);
	add(cmdPOP, EAX);
	add(cmdCMP, EAX, makeValArg(0));
	add(cmdJE, makeValArg(label2));
	node->statement->visit(this);
	add(cmdJMP, makeValArg(label1));
	add(label2);

	continueStack.pop_back();
	breakStack.pop_back();
}

// �������� ����
void CodeGenerator::OnNode(DoNode* node)
{
	std::string label1 = "label" + std::to_string(labelCount++);
	std::string label2 = "label" + std::to_string(labelCount++);

	continueStack.push_back(label1);
	breakStack.push_back(label2);

	add(label1);
	node->statement->visit(this);
	node->expr->visit(this);
	add(cmdPOP, EAX);
	add(cmdCMP, EAX, makeValArg(0));
	add(cmdJNE, makeValArg(label1));
	add(label2);

	continueStack.pop_back();
	breakStack.pop_back();
}

// �������� ����
void CodeGenerator::OnNode(ForNode* node)
{
	std::string label1 = "label" + std::to_string(labelCount++);
	std::string label2 = "label" + std::to_string(labelCount++);
	std::string label3 = "label" + std::to_string(labelCount++);
	std::string label4 = "label" + std::to_string(labelCount++);

	continueStack.push_back(label2);
	breakStack.push_back(label3);

	node->expr1->visit(this);
	add(label1);
	node->expr2->visit(this);
	add(cmdPOP, EAX);
	add(cmdCMP, EAX, makeValArg("0"));
	add(cmdJE, makeValArg(label2));
	node->statement->visit(this);
	add(label2);
	node->expr3->visit(this);
	add(cmdJMP, makeValArg(label1));
	add(label3);

	continueStack.pop_back();
	breakStack.pop_back();
}

// �������� ����
void CodeGenerator::OnNode(BreakNode* node)
{
	add(cmdJMP, makeValArg(breakStack.back()));
}

// �������� ����
void CodeGenerator::OnNode(ContinueNode* node)
{
	add(cmdJMP, makeValArg(continueStack.back()));
}

// �������� ����
void CodeGenerator::OnNode(ReturnNode* node)
{
	add(cmdJMP, makeValArg(returnStack.back()));
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
void CodeGenerator::add(int cmd, int reg)
{
	asmProg->addCmd(new AsmCmd(cmd, makeRegArg(reg)));
}

// �������� �������
void CodeGenerator::add(int cmd, AsmArg* arg1, AsmArg* arg2)
{
	asmProg->addCmd(new AsmCmd(cmd, arg1, arg2));
}

// �������� �������
void CodeGenerator::add(int cmd, int reg1, AsmArg* arg2)
{
	asmProg->addCmd(new AsmCmd(cmd, makeRegArg(reg1), arg2));
}

// �������� �������
void CodeGenerator::add(int cmd, AsmArg* arg1, int reg2)
{
	asmProg->addCmd(new AsmCmd(cmd, arg1, makeRegArg(reg2)));
}

// �������� �������
void CodeGenerator::add(int cmd, int reg1, int reg2)
{
	asmProg->addCmd(new AsmCmd(cmd, makeRegArg(reg1), makeRegArg(reg2)));
}

// �������� �������
void CodeGenerator::add(std::string name)
{
	asmProg->addCmd(new AsmCmd(cmdLABEL, name));
}

// ������� ��������
AsmArg* CodeGenerator::makeValArg(std::string val)
{
	return new AsmArg(argVALUE, val);
}

// ������� ��������
AsmArg* CodeGenerator::makeRegArg(int reg)
{
	return new AsmArg(argREG, reg);
}

// ������� ��������
AsmArg* CodeGenerator::makeMemArg(int reg, int offset)
{
	return new AsmArg(argMEMORY, reg, offset);
}

// ������� ��������
AsmArg* CodeGenerator::makeMemArg(std::string name)
{
	return new AsmArg(argMEMORY, name);
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