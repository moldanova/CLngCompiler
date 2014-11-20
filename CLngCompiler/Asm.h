#pragma once

// Тип аргумента
enum
{
	argVALUE,
	argREG,
	argMEMORY,
	argOFFSET
};

// Регистры
enum
{
	EAX,
	EBX,
	ECX,
	EDX,
	EBP,
	ESP
};

// Команда
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

// Класс переменной
class AsmVar
{
public:
	AsmVar(std::string name, std::string type, std::string value);
	AsmVar(std::string name, std::string type, int count);
	void print(std::ofstream& out);

	std::string name;
	std::string type;
	int count;
	std::string value;
};

// Класс аргумента
class AsmArg
{
public:	
	AsmArg(int arg, std::string value);
	AsmArg(int arg, int reg);
	AsmArg(int arg, int reg, int offset);
	void print(std::ofstream& out);

	int arg;			// Тип операнда
	std::string value;	// значение операнда
	int reg;			// Регистр
	int offset;			// Смещение
};

// Класс ассемблерной команды
class AsmCmd
{
public:
	AsmCmd(int cmd);	
	AsmCmd(int cmd, AsmArg* arg1);
	AsmCmd(int cmd, AsmArg* arg1, AsmArg* arg2);
	~AsmCmd();
	void print(std::ofstream& out);

	// Код команды
	int cmd;
	// Аргументы
	AsmArg* arg1;
	AsmArg* arg2;
};

// Класс процедуры на ассемблере
class AsmProc
{
public:
	AsmProc(std::string name);
	~AsmProc();	
	// добавить параметр функции
	void addParam(AsmVar* var);
	// добавить локальную переменную
	void addLocal(AsmVar* var);
	// Добавить код программы
	void addCode(AsmCmd* cmd);
	void print(std::ofstream& out);

	std::string name;
	std::vector<AsmVar*> param;
	// локальные переменные
	std::vector<AsmVar*> local;
	// Код функции
	std::vector<AsmCmd*> code;
};

// Класс программы на ассемблере
class AsmProg
{
public:
	AsmProg();
	~AsmProg();
	// добавить глобальную перемнную
	void addGlobal(AsmVar* var);
	// добавить процедуру
	void addProc(AsmProc* proc);
	// Добавить код программы
	void addCode(AsmCmd* cmd);
	void print(std::string fileName);

	// Глобальные переменные
	std::vector<AsmVar*> global;
	// Процедуры
	std::vector<AsmProc*> proc;
	std::vector<AsmCmd*> code;

};