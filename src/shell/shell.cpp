#include <config.h>
#include <iostream>
#include <list>
#include <common.h>
#include <memory>

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
    yyparse();

    return 0;
}
