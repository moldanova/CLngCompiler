#pragma once

#include "Lexer.h"
#include "Symbols.h"
#include "Nodes.h"


// Класс синтаксического анализатора
class Parser
{
public:
	// Конструктор
	Parser(Lexer& lexer);
	// Деструктор
	~Parser();
	// Выполнить анализ
	Node* parse(bool isExpression = false);	
private:	
	// Разобрать декларацию
	bool parseDeclaration(NodesArrayNode* parent);
	// разобрать тип
	TypeSymbol* parseType();
	// Разобрать структуру
	TypeSymbol* parseStruct();
	// разобрать определение
	Node* parseDeclarator(TypeSymbol* type, bool needParen = false);
	// Разобрать указатель
	TypeSymbol* parsePointer(TypeSymbol* type);
	// Разобрать массив
	TypeSymbol* parseArray(TypeSymbol* type);
	// Разобрать функцию
	Node* parseFunction(Node* node);
	// Разобрать выражение
	Node* parseExpression();
	// Разобраиь присваивание
	Node* parseAssignmentExpression();
	// Разобрать условное выражение
	Node* parseConditionalExpression(Node* left);
	// разобрать операцию
	Node* Parser::parseOperation(Node* left, int priority);
	// Разобрать приведение типа
	Node* parseCastExpression();
	// разобрать унарные операции
	Node* parseUnaryExpression();
	// Разобрать постфиксные операции
	Node* parsePostfixExpression();
	// Разобрать операнд
	Node* parsePrimaryExpression();		
	// разобрать блок
	Node* parseStatement();
	// разобрать блок
	Node* parseCompoundStatement();
	// разобрать блок
	Node* parseIfStatement();
	// разобрать блок
	Node* parseWhileStatement();
	// разобрать блок
	Node* parseDoStatement();
	// разобрать блок
	Node* parseForStatement();
	// Получить следующую лексему
	void next();
	// Вернуть лексему назад
	void back();
	// Сообщение о синтаксической ошибке
	void check(bool cond, const char* msg, Lexeme l);
	// Проверить, что лексема является унарной операцией
	bool isUnaryOperator();
	// Проверить что лексема является присваиванием
	bool isAssignmentOperator();
	// Проверить, что лексема является именем типа
	bool isTypeName();


	// Проверить символ
	void checkSymbol(std::string name, bool forceAll = true);
	// найти тип
	TypeSymbol* getTypeSymbol(std::string name);
	// Добавить тип
	TypeSymbol* addTypeSymbol(std::string name, int length);
	// Добавить тип
	TypeSymbol* addTypeSymbol(TypeSymbol* baseType, int mode);
	// Добавить псевдоним
	AliasSymbol* addAliasSymbol(TypeSymbol* baseType, std::string name);
	// Добавить массив
	ArraySymbol* addArraySymbol(TypeSymbol* baseType, int count);	
	// Добавить структуру
	StructSymbol* addStructSymbol(std::string name);
	// Получить перемнную или функцию
	Symbol* getSymbol(std::string name);
	// Добавить переменную
	VariableSymbol* addVariableSymbol(std::string name, TypeSymbol* type);
	// Добавить функцию
	FunctionSymbol* addFunctionSymbol(std::string name, TypeSymbol* type);

	Lexer& lexer;	// Лексический анализатор
	Lexeme lex;		// Анализируемая лексема
	std::vector<Lexeme> buffer;		// буфер обработанных лексем
	std::vector<Lexeme> backStack;		// стек вернутых лексем
	std::vector<SymbolsTable*> tables;	// Стек таблиц символов
};

