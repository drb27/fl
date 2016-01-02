#include <config.h>
#include <iostream>
#include <list>
#include <common.h>

#include <interpreter/class.h>
#include <interpreter/typemgr.h>
#include <parser/action_target.h>
#include <interpreter/dat.h>

using std::list;

action_target* target;
typemgr tm;

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
    const fclass& root_cls = tm.lookup(root_spec);
    

    typespec int_spec = typespec("int",{});
    typespec list_spec = typespec("list",{int_spec});
    fclass int_cls(int_spec,root_cls);
    tm.add(int_cls);
    const fclass& intlist_cls = tm.lookup(list_spec);

    std::cout << "Typespec is " << intlist_cls.get_spec().full_name() << std::endl;

    yyparse();
    return 0;
}
