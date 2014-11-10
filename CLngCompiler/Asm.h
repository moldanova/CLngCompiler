#pragma once

// Тип переменной
enum
{
	varDB,	// 1 байт
	varDD	// 4 байта
};

// Тип аргумента
enum
{
	argVALUE,
	argREG,
	argMEMORY
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
	AsmVar(std::string name, int type, std::string value);
	// Конструктор
	AsmVar(std::string name, int type, int count);
	// Вывести в файл
	void print(std::ofstream& out);

	std::string name;
	int type;
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
	// Конструктор
	AsmCmd(int cmd, std::string val);
	// деструктор
	~AsmCmd();
	// Вывести в файл
	void print(std::ofstream& out);

	int cmd;
	AsmArg* arg1;
	AsmArg* arg2;
	std::string val;
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
	void addVar(AsmVar* var);
	// Добавить код программы
	void addCmd(AsmCmd* cmd);
	// Вывести программу на печать
	void print(std::string fileName);

	std::vector<AsmVar*> vars;
	std::vector<AsmCmd*> cmds;

};