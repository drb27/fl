#include <config.h>
#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <common.h>
#include <memory>
#include <deque>
#include <map>
#include <fstream>

#include <logger/logger.h>
#include <interpreter/context.h>
#include <interpreter/class.h>
#include <interpreter/typemgr.h>
#include <parser/action_target.h>
#include <interpreter/dat.h>
#include <parser/ast.h>
#include <interpreter/object.h>
#include <parser/callable.h>
#include <interpreter/builtins.h>
#include <interpreter/eval_exception.h>
#include <parser/ast_nodes.h>

#include <parser/bison.h>
#include <parser/flex.h>

using std::list;
using std::shared_ptr;
using std::deque;
using std::string;
using std::map;

action_target* target;

logger g_logger(std::cout);
yypstate* g_ps;

extern "C" void yyerror(char const* c)
{
    throw eval_exception(cerror::syntax_error,"Syntax error");
}

yypstate* reset_parser_state()
{
    yypstate_delete(g_ps);
    g_ps = yypstate_new();
    return g_ps;
}

void parse_string(const std::string& inputString,yyscan_t scanner,yypstate* ps)
{
    string str = inputString;
    auto newBuffer = yy_scan_string(str.c_str(),scanner);
    try
    {
	YYSTYPE t;
	int c;
	while ( (c=yylex(&t,scanner)) != EOFF )
	{
	    yypush_parse(ps,c,&t);
	}
    }
    catch ( eval_exception& e )
    {
	std::cout << e.what() << std::endl;
	ps = reset_parser_state();
    }

    yy_delete_buffer(newBuffer,scanner);
}

void read_file(const std::string& fname,yyscan_t scanner,yypstate* ps)
{
    // Library file
    std::ifstream infile(fname);
    if ( infile.good() )
    {
	try
	{
	    while(true)
	    {
		string inputString;
		std::getline(infile,inputString);
		parse_string(inputString,scanner,ps);
		if (infile.eof())
		    throw std::exception();
	    }
	}
	catch( ...)
	{
	    
	}
	infile.close();
    }
}

int main(int argc, char** argv)
{
    g_logger.enable( level::info );
    g_logger.enable( level::warning );
    g_logger.enable( level::error );
    g_logger.enable( level::fatal );
    //g_logger.enable( level::trace );
    //g_logger.enable( level::debug );

    wlog(level::info,PACKAGE_STRING ": Application startup");
    std::cout << PACKAGE_STRING << std::endl;
    wlog(level::debug,"Creating context");
    context* shell_context = new context();
    builtins::build_globals(shell_context);
    target = new dat(shell_context);

    // Reentrant scanner/parser init
    yyscan_t scanner;
    yylex_init(&scanner);
    g_ps = yypstate_new();

    string fname="my.fl";
    if (argc>1)
	fname=argv[1];
    // File input
    read_file(fname,scanner,g_ps);
    
    // User input
    bool more=true;
    while(more)
    {
	std::cout << "fl> ";
	string inputString;
	std::getline(std::cin,inputString);
	inputString = inputString;
	try
	{
	    parse_string(inputString,scanner,g_ps);
	}
	catch( terminate_exception& )
	{
	    more=false;
	}
    }

    yypstate_delete(g_ps);
    yylex_destroy(scanner);

    return 0;
}
