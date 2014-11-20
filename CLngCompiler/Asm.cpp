#include "stdafx.h"
#include "Asm.h"



const char* regName[] = { 
	"eax", 
	"ebx", 
	"ecx", 
	"edx", 
	"ebp", 
	"esp" 
};
const char* cmdName[] = { 
	"add", 
	"call", 
	"cdq", 
	"cmp", 
	"div",
	"idiv", 
	"imul", 
	"je",
	"jmp",
	"jne",
	"lea", 
	"mov", 
	"mul",
	"neg",
	"not", 
	"pop", 
	"push", 
	"ret", 
	"sub" 
};

//---------------------------------------------------------------------------

// �����������
AsmVar::AsmVar(std::string name, std::string type, std::string value)
{
	this->name = name;
	this->type = type;
	this->count = 0;
	this->value = value;
}

// �����������
AsmVar::AsmVar(std::string name, std::string type, int count)
{
	this->name = name;
	this->type = type;
	this->count = count;
}

// ������� � ����
void AsmVar::print(std::ofstream& out)
{
	if (!value.empty())
		out << "\t" << name << " " << type << " " << value << std::endl;
	else if (count == 0)
		out << "\t" << name << " " << type << " 0" << std::endl;
	else
		out << "\t" << name << " " << type << " " << count << " dup(?)" << std::endl;
}

//---------------------------------------------------------------------------

// �����������
AsmArg::AsmArg(int arg, std::string value)
{
	this->arg = arg;
	this->value = value;
	this->reg = 0;
	this->offset = 0;
}

// �����������
AsmArg::AsmArg(int arg, int reg)
{
	this->arg = arg;
	this->reg = reg;
	this->offset = 0;
}

// �����������
AsmArg::AsmArg(int arg, int reg, int offset)
{
	this->arg = arg;
	this->reg = reg;
	this->offset = offset;
}

// ������� � ����
void AsmArg::print(std::ofstream& out)
{	
	if (arg == argVALUE)
		out << value;
	else if (arg == argREG)
		out << regName[reg];
	else if (arg == argMEMORY)
	{
		if (!value.empty())
			out << value;
		else if (offset)
			out << "[" << regName[reg] << "+" << std::to_string(offset) << "]";
		else
			out << "[" << regName[reg] << "]";
	}
	else
	{
		out << "offset " << value;
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
		arg1->print(out);
		out << ":" << std::endl;
	}
	else
	{
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
		out << std::endl;
	}
}

//---------------------------------------------------------------------------

// �����������
AsmProc::AsmProc(std::string name)
{
	this->name = name;
}

// ����������
AsmProc::~AsmProc()
{
	for (int i = 0; i < param.size(); i++)
		delete param[i];
	param.clear();

	for (int i = 0; i < local.size(); i++)
		delete local[i];
	local.clear();

	for (int i = 0; i < code.size(); i++)
		delete code[i];
	code.clear();
}

// �������� �������� �������
void AsmProc::addParam(AsmVar* var)
{
	param.push_back(var);
}

// �������� ��������� ����������
void AsmProc::addLocal(AsmVar* var)
{
	local.push_back(var);
}

// �������� ��� ���������
void AsmProc::addCode(AsmCmd* cmd)
{
	code.push_back(cmd);
}

// ������� � ����
void AsmProc::print(std::ofstream& out)
{
	// ��������� ���������
	out << ";----------------------------------------" << std::endl;
	out << name << " PROC NEAR C";
	// ��������� ���������
	for (int i = 0; i < param.size(); i++)
		out << ", " << param[i]->name << ":" << param[i]->type;
	out << std::endl;
	// ��������� ����������
	for (int i = 0; i < local.size(); i++)
		out << "LOCAL " << local[i]->name << ":" << local[i]->type << std::endl;
	// ��� ���������
	for (int i = 0; i < code.size(); i++)
		 code[i]->print(out);
	// ���������� ���������
	out << name << "_end:" << std::endl;
	out << "\tret" << std::endl;
	out << name << " ENDP" << std::endl;
}

//---------------------------------------------------------------------------

// �����������
AsmProg::AsmProg()
{
}

// ����������
AsmProg::~AsmProg()
{
	for (int i = 0; i < global.size(); i++)
		delete global[i];
	global.clear();

	for (int i = 0; i < proc.size(); i++)
		delete proc[i];
	proc.clear();

	for (int i = 0; i < code.size(); i++)
		delete code[i];	
	code.clear();
}

// �������� ���������� ���������
void AsmProg::addGlobal(AsmVar* var)
{
	global.push_back(var);
}

// �������� ���������
void AsmProg::addProc(AsmProc* proc)
{
	this->proc.push_back(proc);
}

// �������� ��� ���������
void AsmProg::addCode(AsmCmd* cmd)
{
	code.push_back(cmd);
}

// ������� ��������� �� ������
void AsmProg::print(std::string fileName)
{
	std::ofstream out(fileName, std::ios::out | std::ios::trunc);
	out << ".386" << std::endl;
	out << ".model flat, stdcall" << std::endl;
	out << "include c:\\masm32\\include\\msvcrt.inc" << std::endl;
	out << "includelib c:\\masm32\\lib\\msvcrt.lib" << std::endl;

	 out << ".data" << std::endl;

	 for (int i = 0; i < global.size(); i++)
		 global[i]->print(out);

	 out << ".code" << std::endl;

	 for (int i = 0; i < proc.size(); i++)
		 proc[i]->print(out);

	 out << ";----------------------------------------" << std::endl;
	 out << "start:" << std::endl;

	 for (int i = 0; i < code.size(); i++)
		 code[i]->print(out);

	 out << "\tcall f_main" << std::endl;
	 out << "\tret" << std::endl;
	 out << "end start" << std::endl;
}