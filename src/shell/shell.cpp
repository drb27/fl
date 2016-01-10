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

using std::list;
using std::shared_ptr;
action_target* target;
context shell_context;

int yyparse(void);

extern "C" void yyerror(char const* c)
{
    throw eval_exception(cerror::syntax_error,"Syntax error");
}


int main(void)
{
    std::cout << PACKAGE_STRING << std::endl;

    target = new dat(shell_context.types(),&shell_context);
    typespec int_spec = typespec("integer",{});
    const fclass& int_cls = shell_context.types().lookup(int_spec);

    objref p1(new int_object(4,int_cls));
    objref p2(new int_object(6,int_cls));

    literal_node ln1(p1);
    literal_node ln2(p2);
    
    auto m{make_marshall(builtins::add_integers,&shell_context)};
    
    std::vector<ast*> ps{&ln1,&ln2};

    intref oo = std::dynamic_pointer_cast<int_object>(m(ps));
    std::cout << "The result of your calculation is " << oo->internal_value() << std::endl;

    yyparse();
    return 0;
}
