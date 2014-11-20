#pragma once

#include "Lexer.h"
#include "Symbols.h"
#include "Nodes.h"


// Класс синтаксического анализатора
class Parser
{
public:
	Parser(Lexer& lexer);
	~Parser();
	Node* parse(bool isExpression = false);	
private:	
	bool parseDeclaration(NodesArrayNode* parent);
	TypeSymbol* parseType();
	TypeSymbol* parseStruct();
	// разобрать определение
	Node* parseDeclarator(TypeSymbol* type, bool needParen = false);
	TypeSymbol* parsePointer(TypeSymbol* type);
	TypeSymbol* parseArray(TypeSymbol* type);
	Node* parseFunction(Node* node);
	Node* parseExpression();
	Node* parseAssignmentExpression();
	Node* parseConditionalExpression(Node* left);
	Node* Parser::parseOperation(Node* left, int priority);
	// Разобрать приведение типа
	Node* parseCastExpression();
	Node* parseUnaryExpression();
	Node* parsePostfixExpression();
	// Разобрать операнд
	Node* parsePrimaryExpression();		
	Node* parseStatement();
	Node* parseCompoundStatement();
	Node* parseIfStatement();
	Node* parseWhileStatement();
	Node* parseDoStatement();
	Node* parseForStatement();
	// Получить следующую лексему
	void next();
	// Вернуть лексему назад
	void back();
	// Сообщение о синтаксической ошибке
	void check(bool cond, const char* msg, Lexeme l);
	bool isUnaryOperator();
	bool isAssignmentOperator();


	// Проверить символ (используется ли данный идентификатор)
	void checkSymbol(std::string name, bool forceAll = true);
	// найти тип
	TypeSymbol* getTypeSymbol(std::string name);
	// Добавить тип
	TypeSymbol* addTypeSymbol(std::string name);
	TypeSymbol* addTypeSymbol(TypeSymbol* baseType, int mode);
	// Добавить псевдоним
	AliasSymbol* addAliasSymbol(TypeSymbol* baseType, std::string name);
	ArraySymbol* addArraySymbol(TypeSymbol* baseType, int count);	
	StructSymbol* addStructSymbol(std::string name);
	// Получить перемнную или функцию
	Symbol* getSymbol(std::string name);
	// Добавить переменную
	VariableSymbol* addVariableSymbol(std::string name, TypeSymbol* type);
	FunctionSymbol* addFunctionSymbol(std::string name, TypeSymbol* type);

	// Перечисление циклов
	enum
	{
		LOOP_FOR,
		LOOP_WHILE,
		LOOP_DO
	};

	Lexer& lexer;
	Lexeme lex;
	std::vector<Lexeme> buffer;		// буфер обработанных лексем
	std::vector<Lexeme> backStack;		// стек вернутых лексем
	std::vector<SymbolsTable*> tables;	// Стек таблиц символов
	std::vector<int> loops;	// Стек циклов
	std::vector<FunctionNode*> funcs;	// Стек функций
};