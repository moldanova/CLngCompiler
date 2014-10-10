// CLngCompiler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Lexer.h"
#include "Parser.h"
#include "Printer.h"

int main(int argc, char* argv[])
{
	try
	{
		if (argc == 3) 
		{
			std::string fn = argv[2];

			if (strcmp(argv[1], "-lexer") == 0)
			{
				Lexer lexer(fn, fn + ".lxr");
				while (true)
				{
					Lexeme lex = lexer.next();
					
					if (lex.code == LEX_EOF)
						break;
				}
			}
			else if (strcmp(argv[1], "-expression") == 0)
			{
				Lexer lexer(fn);
				Parser parser(lexer);
				Printer prn1(fn + ".prs1");
				try
				{
					Node* node = parser.parse(true);
					node->visit(&prn1);
				}
				catch (std::exception& ex)
				{
					prn1 << ex.what();
				}							
			}
			else if (strcmp(argv[1], "-parser") == 0)
			{
				Lexer lexer(fn);
				Parser parser(lexer);
				Printer prn1(fn + ".prs1");
				try
				{
					Node* node = parser.parse();
					node->visit(&prn1);
				}
				catch (std::exception& ex)
				{
					prn1 << ex.what();
				}							
			}
		}
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	if (1)
	{
	    {
	        int a = 1;
	    }
	    int b = 2;
	}
	return 0;
}

