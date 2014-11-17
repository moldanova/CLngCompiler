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
	// Конструктор
	AsmVar(std::string name, std::string type, std::string value);
	// Конструктор
	AsmVar(std::string name, std::string type, int count);
	// Вывести в файл
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
	// Конструктор
	AsmArg(int arg, std::string value);
	// Конструктор
	AsmArg(int arg, int reg);
	// Конструктор
	AsmArg(int arg, int reg, int offset);
	// Вывести в файл
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
	// Конструктор
	AsmCmd(int cmd);	
	// Конструктор
	AsmCmd(int cmd, AsmArg* arg1);
	// Конструктор
	AsmCmd(int cmd, AsmArg* arg1, AsmArg* arg2);
	// деструктор
	~AsmCmd();
	// Вывести в файл
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
	// Конструктор
	AsmProc(std::string name);
	// деструктор
	~AsmProc();	
	// добавить параметр функции
	void addParam(AsmVar* var);
	// добавить локальную переменную
	void addLocal(AsmVar* var);
	// Добавить код программы
	void addCode(AsmCmd* cmd);
	// Вывести в файл
	void print(std::ofstream& out);

	// название функции
	std::string name;
	// параметры функции
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
	// Конструктор
	AsmProg();
	// деструктор
	~AsmProg();
	// добавить глобальную перемнную
	void addGlobal(AsmVar* var);
	// добавить процедуру
	void addProc(AsmProc* proc);
	// Добавить код программы
	void addCode(AsmCmd* cmd);
	// Вывести программу на печать
	void print(std::string fileName);

	// Глобальные переменные
	std::vector<AsmVar*> global;
	// Процедуры
	std::vector<AsmProc*> proc;
	// Код программы
	std::vector<AsmCmd*> code;

};