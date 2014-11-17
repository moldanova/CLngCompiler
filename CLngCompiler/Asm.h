#pragma once

// ��� ���������
enum
{
	argVALUE,
	argREG,
	argMEMORY,
	argOFFSET
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
	AsmVar(std::string name, std::string type, std::string value);
	// �����������
	AsmVar(std::string name, std::string type, int count);
	// ������� � ����
	void print(std::ofstream& out);

	std::string name;
	std::string type;
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
	// ����������
	~AsmCmd();
	// ������� � ����
	void print(std::ofstream& out);

	// ��� �������
	int cmd;
	// ���������
	AsmArg* arg1;
	AsmArg* arg2;
};

// ����� ��������� �� ����������
class AsmProc
{
public:
	// �����������
	AsmProc(std::string name);
	// ����������
	~AsmProc();	
	// �������� �������� �������
	void addParam(AsmVar* var);
	// �������� ��������� ����������
	void addLocal(AsmVar* var);
	// �������� ��� ���������
	void addCode(AsmCmd* cmd);
	// ������� � ����
	void print(std::ofstream& out);

	// �������� �������
	std::string name;
	// ��������� �������
	std::vector<AsmVar*> param;
	// ��������� ����������
	std::vector<AsmVar*> local;
	// ��� �������
	std::vector<AsmCmd*> code;
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
	void addGlobal(AsmVar* var);
	// �������� ���������
	void addProc(AsmProc* proc);
	// �������� ��� ���������
	void addCode(AsmCmd* cmd);
	// ������� ��������� �� ������
	void print(std::string fileName);

	// ���������� ����������
	std::vector<AsmVar*> global;
	// ���������
	std::vector<AsmProc*> proc;
	// ��� ���������
	std::vector<AsmCmd*> code;

};