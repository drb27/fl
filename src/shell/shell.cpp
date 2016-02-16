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
#include <parser/ast/ast.h>
#include <parser/callable.h>
#include <interpreter/builtins.h>
#include <interpreter/eval_exception.h>
#include <interpreter/obj/int_object.h>

#include <parser/bison.h>
#include <parser/flex.h>

using std::list;
using std::shared_ptr;
using std::deque;
using std::string;
using std::map;
using std::function;

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

void process_string(const std::string& inputString,
		    yyscan_t scanner,
		    yypstate* ps,
		    deque<function<objref(void)>>& callbacks )
{
    parse_string(inputString,scanner,ps);

    while(callbacks.size()>0)
    {
	auto fn = callbacks.front();
	callbacks.pop_front();
	fn();
    }
}

void read_file(const std::string& fname,
	       yyscan_t scanner,
	       yypstate* ps,
	       deque<function<objref(void)>>& callbacks )
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
		process_string(inputString,scanner,ps,callbacks);
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

    // Reentrant scanner/parser init
    yyscan_t scanner;
    yylex_init(&scanner);
    g_ps = yypstate_new();

    deque<function<objref(void)>> callbacks;
    target = new dat(shell_context,
		     [&callbacks,&scanner](const std::string& fname)
		     {
			 callbacks.push_back( [&fname,&scanner,&callbacks]()
	                                      {
						  read_file(fname,scanner,g_ps,callbacks);
						  return objref(nullptr);
	                                      }
					     
					     );
		     },
		     [&callbacks,&scanner,&shell_context](const std::string& flStatement)
		     {
			 callbacks.push_back( [&flStatement,&scanner,&callbacks,&shell_context]()
	                                      {
						  process_string(flStatement,scanner,g_ps,callbacks);
						  return shell_context->resolve_symbol("_last");
	                                      }
					     
					     );
		     }
		     );

    string fname="my.fl";
    if (argc>1)
    {
	fname=argv[1];
	// File input
	read_file(fname,scanner,g_ps,callbacks);
    }
    else
    {
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
		process_string(inputString,scanner,g_ps,callbacks);
	    }
	    catch( terminate_exception& )
	    {
		more=false;
	    }
	}
    }

    yypstate_delete(g_ps);
    yylex_destroy(scanner);

    if ( shell_context->is_defined("exit") )
    {
	intref exitCode = std::dynamic_pointer_cast<int_object>(shell_context->resolve_symbol("exit"));
	return exitCode->internal_value();
    }
    else
	return 0;
}
