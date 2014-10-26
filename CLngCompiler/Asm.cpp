#include "stdafx.h"
#include "Asm.h"

//---------------------------------------------------------------------------

// �����������
AsmVar::AsmVar(std::string name, int type, std::string value)
{
	this->name = name;
	this->type = type;
	this->count = 0;
	this->value = value;
}

// �����������
AsmVar::AsmVar(std::string name, int type, int count)
{
	this->name = name;
	this->type = type;
	this->count = count;
}

// ������� � ����
void AsmVar::print(std::ofstream& out)
{
	out << "\t" << name;
	if (type == varDB)
		out << " db ";
	else
		out << " dd ";
	if (count == 0)
	{
		//TODO: �������� ���������� ����� ���������� ��������
		out << value << std::endl;
	}
	else
		out << count << " dup(0)" << std::endl;

}

//---------------------------------------------------------------------------

// �����������
AsmArg::AsmArg(int arg)
{
	this->arg = arg;
}

// �����������
AsmArg::AsmArg(int arg, std::string val)
{
	this->arg = arg;
	this->val = val;
}

// ������� � ����
void AsmArg::print(std::ofstream& out)
{
	if (arg == argVAL)
	{
		out << val;
	}
	else
	{
		const char* regName[] = { "eax", "ebx", "ecx", "edx", "ebp", "esp" };
		out << regName[arg];
	}
}

//---------------------------------------------------------------------------

// �����������
AsmCmd::AsmCmd(int cmd)
{
	this->cmd = cmd;
	this->arg1 = NULL;
	this->arg2 = NULL;
}

// �����������
AsmCmd::AsmCmd(int cmd, AsmArg* arg1)
{
	this->cmd = cmd;
	this->arg1 = arg1;
	this->arg2 = NULL;
}

// �����������
AsmCmd::AsmCmd(int cmd, AsmArg* arg1, AsmArg* arg2)
{
	this->cmd = cmd;
	this->arg1 = arg1;
	this->arg2 = arg2;
}

// �����������
AsmCmd::AsmCmd(int cmd, std::string val)
{
	this->cmd = cmd;
	this->arg1 = NULL;
	this->arg2 = NULL;
	this->val = val;
}

// ����������
AsmCmd::~AsmCmd()
{
	if (arg1)
		delete arg1;
	if (arg2)
		delete arg2;
}

// ������� � ����
void AsmCmd::print(std::ofstream& out)
{
	if (cmd == cmdLABEL)
	{
		out << val << ":" << std::endl;
	}
	else
	{
		const char* cmdName[] = { "call", "mov", "pop", "push", "ret" };
		out << "\t" << cmdName[cmd];
		if (arg1)
		{
			out << " ";
			arg1->print(out);
		}
		if (arg2)
		{
			out << ", ";
			arg2->print(out);
		}
		if (!val.empty())
		{
			out << " " << val;
		}
		out << std::endl;
	}
}

//---------------------------------------------------------------------------

// �����������
AsmProg::AsmProg()
{
}

// ����������
AsmProg::~AsmProg()
{
	for (int i = 0; i < vars.size(); i++)
		delete vars[i];
	for (int i = 0; i < cmds.size(); i++)
		delete cmds[i];
	vars.clear();
	cmds.clear();
}

// �������� ���������� ���������
void AsmProg::addVar(AsmVar* var)
{
	vars.push_back(var);
}

// �������� ��� ���������
void AsmProg::addCmd(AsmCmd* cmd)
{
	cmds.push_back(cmd);
}

// ������� ��������� �� ������
void AsmProg::print(std::string fileName)
{
	 std::ofstream out(fileName, std::ios::out | std::ios::trunc);
	 out << ".686" << std::endl;
	 out << ".model flat, stdcall" << std::endl;
	 out << ".data" << std::endl;

	 for (int i = 0; i < vars.size(); i++)
		 vars[i]->print(out);

	 out << ".code" << std::endl;

	 for (int i = 0; i < cmds.size(); i++)
		 cmds[i]->print(out);

	 out << "end start" << std::endl;
}