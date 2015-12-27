#include <config.h>
#include <iostream>
#include <list>
#include <common.h>

#include <interpreter/class.h>
#include <interpreter/typemgr.h>

using std::list;

int yyparse(void);

extern "C" void yyerror(char const* c)
{
    std::cout << "ERROR: " << c << std::endl;
}


int main(void)
{
    std::cout << PACKAGE_STRING << std::endl;

    typemgr mgr;
    typespec int_spec = typespec("int",{});
    typespec list_spec = typespec("list",{int_spec});
    
    const fclass& intlist_cls = mgr.lookup(list_spec);

    std::cout << "Typespec is " << intlist_cls.get_spec().full_name() << std::endl;

    yyparse();
    return 0;
}
