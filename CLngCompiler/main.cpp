// CLngCompiler.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Lexer.h"
#include "Parser.h"
#include "Printer.h"
#include "CodeGenerator.h"
#include <windows.h>

int main(int argc, char* argv[])
{
	try
	{
		//if (argc == 3) 
		//{
		//	std::string fn = argv[2];
		//	std::string fnlxr = fn + ".lxr";
		//	std::string fnprs1 = fn + ".prs1";
		//	std::string fnasm = fn + ".asm";

		//	if (strcmp(argv[1], "-lexer") == 0)
		//	{
		//		Lexer lexer(fn, fnlxr);
		//		while (true)
		//		{
		//			Lexeme lex = lexer.next();
		//			
		//			if (lex.code == LEX_EOF)
		//				break;
		//		}
		//	}
		//	else if (strcmp(argv[1], "-expression") == 0)
		//	{
		//		{
		//			Lexer lexer(fn);
		//			Parser parser(lexer);
		//			Printer prn1(fnprs1);
		//			try
		//			{
		//				Node* node = parser.parse(true);
		//				node->visit(&prn1);
		//			}
		//			catch (std::exception& ex)
		//			{
		//				prn1 << ex.what();
		//			}			
		//		}
		//		//DeleteFile(fnlxr.c_str());
		//	}
		//	else if (strcmp(argv[1], "-parser") == 0)
		//	{
		//		{
		//			Lexer lexer(fn);
		//			Parser parser(lexer);
		//			Printer prn1(fnprs1);
		//			try
		//			{
		//				Node* node = parser.parse();
		//				node->visit(&prn1);
		//			}
		//			catch (std::exception& ex)
		//			{
		//				prn1 << ex.what();
		//			}
		//		}
		//		//DeleteFile(fnlxr.c_str());
		//	}
		//	else if (strcmp(argv[1], "-generator") == 0)
		//	{
		//		{
		//			Lexer lexer(fn);
		//			Parser parser(lexer);
		//			//Printer prn1(fnprs1);
		//			CodeGenerator coder;
		//			try
		//			{
		//				Node* node = parser.parse();
		//				//node->visit(&prn1);
		//				AsmProg* prog = coder.generate((ProgramNode*)node);
		//				prog->print(fnasm);
		//				delete prog;
		//			}
		//			catch (std::exception& ex)
		//			{
		//				//prn1 << ex.what();
		//			}	
		//		}
		//		//DeleteFile(fnlxr.c_str());
		//	}
		//}
		std::string fn = "input.txt";
		std::string fnlxr = fn + ".lxr";
		std::string fnprs1 = fn + ".prs1";
		Lexer lexer(fn);
		Parser parser(lexer);
		Printer prn1(fnprs1);
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
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
	return 0;
}

