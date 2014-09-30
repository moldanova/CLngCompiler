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
	Node* parse();
	// Разобрать выражение
	Node* parseExpression();
	// Вернуть признак ошибок
	bool hasErrors();
private:	
	// Разобрать единицу трансляции
	Node* parseUnit();
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
	// разобрать тип
	TypeSymbol* parseType();
	// Разобрать указатель
	TypeSymbol* parsePointer(TypeSymbol* type);
	// Разобрать структуру
	TypeSymbol* parseStruct();	
	// разобрать определение
	ItemSymbol* parseDeclarator(TypeSymbol* type);

	// Получить следующую лексему
	void next();
	// Вернуть лексему назад
	void back();
	// Проверить, что лексема является унарной операцией
	bool isUnaryOperator();
	// Проверить что лексема является присваиванием
	bool isAssignmentOperator();
	// Проверить, что лексема является именем типа
	bool isTypeName();
	// Поиск символа
	Symbol* findSymbolByName(std::string name, bool forceAll = true);
	// Поиск  символа или добавить
	Symbol* findOrAddSymbol(Symbol* symbol, bool forceAll = true);
	// Добавить символ
	void addSymbol(Symbol* symbol);

	// Сообщение о синтаксической ошибке
	void OnSyntaxError(const char* msg);

	Lexer& lexer;	// Лексический анализатор
	Lexeme lex;		// Анализируемая лексема
	std::vector<Lexeme> buffer;		// буфер обработанных лексем
	std::vector<Lexeme> backStack;		// стек вернутых лексем
	std::vector<SymbolsTable*> tables;	// Стек таблиц символов
};

