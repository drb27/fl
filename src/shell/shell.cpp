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

using std::list;
using std::shared_ptr;
using std::deque;
using std::string;
using std::map;

action_target* target;

logger g_logger(std::cout);

struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;

int yyparse(void);
extern YY_BUFFER_STATE yy_scan_string(const char*);
extern void yy_delete_buffer(YY_BUFFER_STATE);

extern "C" void yyerror(char const* c)
{
    throw eval_exception(cerror::syntax_error,"Syntax error");
}


int main(void)
{
    g_logger.enable( level::info );
    g_logger.enable( level::warning );
    g_logger.enable( level::error );
    g_logger.enable( level::fatal );
    //g_logger.enable( level::trace );
    //g_logger.enable( level::debug );

    wlog(level::info,PACKAGE_STRING ": Application startup");
    std::cout << PACKAGE_STRING << std::endl;
    std::shared_ptr<context> shell_context(new context());
    fclass::types = &shell_context->types();
    target = new dat(shell_context);

    // Library file
    std::ifstream infile("my.fl");
    if ( infile.good() )
    {
	try
	{
	    while(true)
	    {
		string inputString;
		std::getline(infile,inputString);
		inputString = inputString + "\n";
		auto newBuffer = yy_scan_string(inputString.c_str());
		try
		{
		    yyparse();
		}
		catch ( eval_exception& e )
		{
		    std::cout << e.what() << std::endl;
		}

		yy_delete_buffer(newBuffer);
		if (infile.eof())
		    throw std::exception();
	    }
	}
	catch( ...)
	{
	    
	}
	infile.close();
    }

    

    // User input
    bool more=true;
    while(more)
    {
	string inputString;
	std::getline(std::cin,inputString);
	inputString = inputString + "\n";
	auto newBuffer = yy_scan_string(inputString.c_str());
	try
	{
	    yyparse();
	}
	catch ( eval_exception& e )
	{
	    std::cout << e.what() << std::endl;
	}
	catch( terminate_exception& )
	{
	    more=false;
	}
	yy_delete_buffer(newBuffer);
    }

    return 0;
}
