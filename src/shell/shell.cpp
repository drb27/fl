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

using std::list;
using std::shared_ptr;
action_target* target;
context shell_context;

objref bf_add2(intref a,intref b)
{
    const int result = a->internal_value() + b->internal_value();
    typespec int_spec = typespec("integer",{});
    const fclass& int_cls = shell_context.types().lookup(int_spec);
    objref pObject(new int_object(result,int_cls));

    return pObject;
}

int yyparse(void);

extern "C" void yyerror(char const* c)
{
    std::cout << "ERROR: " << c << std::endl;
}


int main(void)
{
    std::cout << PACKAGE_STRING << std::endl;

    target = new dat(shell_context.types(),&shell_context);
    typespec root_spec("object",{});
    typespec int_spec = typespec("integer",{});
    const fclass& root_cls = shell_context.types().lookup(root_spec);
    const fclass& int_cls = shell_context.types().lookup(int_spec);

    auto m = internal_typed_method<objref,intref,intref>(&bf_add2);
    objref p1(new int_object(4,int_cls));
    objref p2(new int_object(6,int_cls));

    literal_node ln1(p1);
    literal_node ln2(p2);

    int aa = std::dynamic_pointer_cast<int_object>(ln1.evaluate(nullptr))->internal_value();
    std::cout << "aa=" << aa << std::endl;

    std::array<ast*,2> p;
    std::get<0>(p) = &ln1;
    std::get<1>(p) = &ln2;

    m.set_params(p);
    intref oo = std::dynamic_pointer_cast<int_object>(m());
    std::cout << "The result of your calculation is " << oo->internal_value() << std::endl;

    typespec list_spec = typespec("list",{int_spec});
    const fclass& intlist_cls = shell_context.types().lookup(list_spec);

    std::cout << "Typespec is " << intlist_cls.get_spec().full_name() << std::endl;

    yyparse();
    return 0;
}
