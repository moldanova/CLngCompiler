#pragma once

#include "Lexer.h"
#include "Node.h"
#include "Symbols.h"

// Класс синтаксического анализатора
class Parser
{
public:
	// Конструктор
	Parser(Lexer& lexer);
	// Деструктор
	~Parser();
	// Выполнить анализ
	void parse();
	// Разобрать выражение
	Node* parseExpression();
	// Вернуть признак ошибок
	bool hasErrors();
	// Получить разобранную программу
	ProgramNode& getProgram() { return prog; }
	// Получить таблицу символов
	SymbolsTable& getSymbols() { return symbols; }
private:	
	// Разобраиь присваивание
	Node* parseAssignmentExpression();
	// Разобрать условное выражение
	Node* parseConditionalExpression(Node* left);
	// Разобрать логическое ИЛИ
	Node* parseLogicalOrExpression(Node* left);
	// Разобрать логическое И
	Node* parseLogicalAndExpression(Node* left);
	// Разобрать побитовое ИЛИ
	Node* parseInclusiveOrExpression(Node* left);
	// Разобрать побитовое исключающее ИЛИ
	Node* parseExclusiveOrExpression(Node* left);
	// Разобрать побитовое И
	Node* parseAndExpression(Node* left);
	// Разобрать равенство
	Node* parseEqualityExpression(Node* left);
	// разобрать сравнение
	Node* parseRelationalExpression(Node* left);
	// разобрать сдвиг
	Node* parseShiftExpression(Node* left);
	// разобать сложение
	Node* parseAdditiveExpression(Node* left);
	// Разобрать умножение
	Node* parseMultiplicativeExpression(Node* left);
	// Разобрать приведение типа
	Node* parseCastExpression();
	// разобрать унарные операции
	Node* parseUnaryExpression();
	// Разобрать постфиксные операции
	Node* parsePostfixExpression();
	// Разобрать операнд
	Node* parsePrimaryExpression();
	// Разобрать функцию
	Node* parseFunction();
	// разобрать тип
	TypeSymbol* parseType();
	// Разобрать структуру
	TypeSymbol* parseStruct();	
	// разобрать переменную
	VariableSymbol* parseVariable(TypeSymbol* type);
	

	// Проверить, что лексема является унарной операцией
	bool isUnaryOperator();
	// Проверить что лексема является присваиванием
	bool isAssignmentOperator();
	// Проверить, что лексема является именем типа
	bool isTypeName();

	// Получить следующую лексему
	void next();
	// Вернуть лексему назад
	void back();
	// Очистить буфер
	void commit();

	// Сообщение о синтаксической ошибке
	void OnSyntaxError(const char* msg);

	Lexer& lexer;	// Лексический анализатор
	Lexeme lex;		// Анализируемая лексема
	std::vector<Lexeme> buffer;		// буфер обработанных лексем
	std::vector<Lexeme> stack;		// стек вернутых лексем
	ProgramNode prog;
	SymbolsTable symbols;			// таблица символов
};

