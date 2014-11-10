#pragma once

// ��� ����������
enum
{
	varDB,	// 1 ����
	varDD	// 4 �����
};

// ��� ���������
enum
{
	argVALUE,
	argREG,
	argMEMORY
};

// ��������
enum
{
	EAX,
	EBX,
	ECX,
	EDX,
	EBP,
	ESP
};

// �������
enum
{
	cmdADD,
	cmdCALL,
	cmdCDQ,
	cmdCMP,
	cmdDIV,
	cmdIDIV,
	cmdIMUL,
	cmdJE,
	cmdJMP,
	cmdJNE,
	cmdLEA,
	cmdMOV,
	cmdMUL,
	cmdNEG,
	cmdNOT,
	cmdPOP,
	cmdPUSH,
	cmdRET,
	cmdSUB,

	cmdLABEL
};

// ����� ����������
class AsmVar
{
public:
	
	// �����������
	AsmVar(std::string name, int type, std::string value);
	// �����������
	AsmVar(std::string name, int type, int count);
	// ������� � ����
	void print(std::ofstream& out);

	std::string name;
	int type;
	int count;
	std::string value;
};

// ����� ���������
class AsmArg
{
public:	
	// �����������
	AsmArg(int arg, std::string value);
	// �����������
	AsmArg(int arg, int reg);
	// �����������
	AsmArg(int arg, int reg, int offset);
	// ������� � ����
	void print(std::ofstream& out);

	int arg;			// ��� ��������
	std::string value;	// �������� ��������
	int reg;			// �������
	int offset;			// ��������
};

// ����� ������������ �������
class AsmCmd
{
public:
	
	// �����������
	AsmCmd(int cmd);	
	// �����������
	AsmCmd(int cmd, AsmArg* arg1);
	// �����������
	AsmCmd(int cmd, AsmArg* arg1, AsmArg* arg2);
	// �����������
	AsmCmd(int cmd, std::string val);
	// ����������
	~AsmCmd();
	// ������� � ����
	void print(std::ofstream& out);

	int cmd;
	AsmArg* arg1;
	AsmArg* arg2;
	std::string val;
};

// ����� ��������� �� ����������
class AsmProg
{
public:
	// �����������
	AsmProg();
	// ����������
	~AsmProg();
	// �������� ���������� ���������
	void addVar(AsmVar* var);
	// �������� ��� ���������
	void addCmd(AsmCmd* cmd);
	// ������� ��������� �� ������
	void print(std::string fileName);

	std::vector<AsmVar*> vars;
	std::vector<AsmCmd*> cmds;

};