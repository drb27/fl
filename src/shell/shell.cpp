#include <config.h>
#include <iostream>
#include <list>
#include <common.h>

#include <interpreter/class.h>
#include <interpreter/typemgr.h>
#include <parser/action_target.h>
#include <interpreter/dat.h>
#include <parser/ast.h>
#include <interpreter/object.h>
#include <parser/callable.h>

using std::list;

action_target* target;
typemgr tm;

object* bf_add2(int_object* a,int_object* b)
{
    const int result = a->internal_value() + b->internal_value();
    typespec int_spec = typespec("int",{});
    const fclass& int_cls = tm.lookup(int_spec);
    int_object* pObject = new int_object(result,int_cls);

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

    target = new dat(tm);
    typespec root_spec("object",{});
    typespec int_spec = typespec("int",{});
    const fclass& root_cls = tm.lookup(root_spec);
    const fclass& int_cls = tm.lookup(int_spec);

    auto m = internal_typed_method<object*,int_object*,int_object*>(&bf_add2);
    int_object obj1(4,int_cls);
    int_object obj2(6,int_cls);

    literal_node ln1(&obj1);
    literal_node ln2(&obj2);

    int aa = dynamic_cast<int_object*>(ln1.evaluate(nullptr))->internal_value();
    std::cout << "aa=" << aa << std::endl;

    std::array<ast*,2> p;
    std::get<0>(p) = &ln1;
    std::get<1>(p) = &ln2;

    m.set_params(p);
    int_object* oo = dynamic_cast<int_object*>(m());
    std::cout << "The result of your calculation is " << oo->internal_value() << std::endl;
    delete oo;

    typespec list_spec = typespec("list",{int_spec});
    const fclass& intlist_cls = tm.lookup(list_spec);

    std::cout << "Typespec is " << intlist_cls.get_spec().full_name() << std::endl;

    yyparse();
    return 0;
}
