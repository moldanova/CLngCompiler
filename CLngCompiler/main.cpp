// CLngCompiler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Lexer.h"
#include "Parser.h"
#include "Printer.h"

// Названия лексем для вывода на экран
const char* lexName[] = {
	"LEX_NOT",
	"LEX_PERSENT",
	"LEX_AND",
	"LEX_LPAREN",
	"LEX_RPAREN",
	"LEX_MUL",
	"LEX_ADD",
	"LEX_COMMA",
	"LEX_SUB",
	"LEX_DOT",
	"LEX_DIV",
	"LEX_COLON",
	"LEX_SEMICOLON",
	"LEX_LESS",
	"LEX_ASSIGNMENT",
	"LEX_GREAT",
	"LEX_QUESTION",
	"LEX_LBRACKET",
	"LEX_RBRACKET",
	"LEX_XOR",
	"LEX_LBRACE",
	"LEX_OR",
	"LEX_RBRACE",
	"LEX_INVERT",
	"LEX_NOT_EQUAL",
	"LEX_PERSENT_ASSIGNMENT",
	"LEX_LOGICAL_AND",
	"LEX_AND_ASSIGNMENT",
	"LEX_MUL_ASSIGNMENT",
	"LEX_INCREMENT",
	"LEX_ADD_ASSIGNMENT",
	"LEX_DECREMENT",
	"LEX_SUB_ASSIGNMENT",
	"LEX_PTR",
	"LEX_DIV_ASSIGNMENT",
	"LEX_LSHIFT",
	"LEX_LESS_EQUAL",
	"LEX_LSHIFT_ASSIGNMENT",
	"LEX_EQUAL",
	"LEX_GREAT_EQUAL",
	"LEX_RSHIFT",
	"LEX_RSHIFT_ASSIGNMENT",
	"LEX_XOR_ASSIGNMENT",
	"LEX_OR_ASSIGNMENT",
	"LEX_LOGICAL_OR",
	"LEX_ID",
	"LEX_BREAK",
	"LEX_CHAR",
	"LEX_CONST",
	"LEX_DO",
	"LEX_ELSE",
	"LEX_FLOAT",
	"LEX_FOR",
	"LEX_IF",
	"LEX_INT",
	"LEX_RETURN",
	"LEX_SIZEOF",
	"LEX_STRUCT",
	"LEX_TYPEDEF",
	"LEX_VOID",
	"LEX_WHILE",
	"LEX_INT_VALUE",
	"LEX_FLOAT_VALUE",
	"LEX_STRING_VALUE",
	"LEX_CHAR_VALUE",
	"LEX_EOF",
	"LEX_ERROR"
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc == 3) 
		{
			if (strcmp(argv[1], "-lex") == 0)
			{
				Lexer lexer(argv[2]);
				while (true)
				{
					Lexeme lex = lexer.next();
					std::cout << lex.line << ":" << lex.col << "\t" << lexName[lex.code] << " \"" << lex.text << "\"" << std::endl;
					if (lex.code == LEX_EOF)
						break;
				}
			}
			else if (strcmp(argv[1], "-expr") == 0)
			{
				Lexer lexer(argv[2]);
				Parser parser(lexer);
				while (true)
				{
					Node* node = parser.parseExpression();
					if (node == NULL)
						break;
					Printer prn;
					node->visit(&prn);
					std::cout << std::endl;
				};
			}	
			else if (strcmp(argv[1], "-parser") == 0)
			{
				Lexer lexer(argv[2]);
				Parser parser(lexer);
				Node* node = parser.parse();
				Printer prn;
				node->visit(&prn);
			}
		}
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

