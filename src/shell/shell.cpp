#include <config.h>
#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <common.h>
#include <memory>
#include <deque>

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

action_target* target;



int yyparse(void);

extern "C" void yyerror(char const* c)
{
    throw eval_exception(cerror::syntax_error,"Syntax error");
}


int main(void)
{
    std::cout << PACKAGE_STRING << std::endl;
    std::shared_ptr<context> shell_context(new context());
    target = new dat(shell_context);

    deque<string> d;
    d.push_back("a");
    d.push_back("b");
    d.push_back("c");

    auto i = std::find(d.begin(),d.end(),"b");

    if (i==d.end())
	std::cout << "NOT FOUND!!!" << std::endl;
    else
	std::cout << "FOUND!!!" << std::endl;

    yyparse();

    return 0;
}
