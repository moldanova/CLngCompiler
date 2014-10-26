#pragma once

// ��� ����������
enum
{
	varDB,	// 1 ����
	varDD	// 4 �����
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

// ��� ���������
enum
{
	argEAX,
	argEBX,
	argECX,
	argEDX,
	argEBP,
	argESP,

	argVAL
};

// ����� ���������
class AsmArg
{
public:	
	// �����������
	AsmArg(int arg);
	// �����������
	AsmArg(int arg, std::string val);
	// ������� � ����
	void print(std::ofstream& out);

	int arg;	// ��� ��������
	std::string val;	// �������� ��������
};

// �������
enum
{
	cmdCALL,
	cmdMOV,
	cmdPOP,
	cmdPUSH,
	cmdRET,

	cmdLABEL
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