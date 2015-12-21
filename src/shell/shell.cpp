#include <config.h>
#include <iostream>
#include <map>

#include <interpreter/class.h>
#include <interpreter/nametable.h>
#include <interpreter/object.h>

typedef std::map<std::string,object*> varmap_t;

static varmap_t varmap;

static nametable<fclass> classes;

#include <parser/actions.h>
#include <common.h>

int yyparse(void);

extern "C" void yyerror(char const* c)
{
    std::cout << "ERROR: " << c << std::endl;
}

void ac_integer(int i)
{
    ftinteger fti(i);
    fti.render( std::cout );
    std::cout << std::endl;
}

void ac_fraction(int a,int b)
{
    fraction_t f;
    std::cout << "You said " << a << "/" << b << std::endl; 
}

void ac_assign(std::string* s, int v)
{
    ftinteger* i = new ftinteger(v);
    varmap[*s]=i;

    std::cout << (*s) << ": ";
    (*i).render(std::cout);
    std::cout << std::endl;
    delete s;
}

void ac_show_def(std::string* s)
{
    if ( varmap.find(*s)!=varmap.end())
    {
	std::cout << (*s) << ": ";
	varmap[*s]->render(std::cout);
	std::cout << std::endl;
    }
    else
    {
	std::cout << "Undefined symbol" << std::endl;
    }
    delete s;
}

int main(void)
{

    typespec tint("integer");
    typespec sublist("list");
    sublist.push_param(tint);
    typespec tl("list");
    tl.push_param(sublist);
    std::cout << tint.full_name() << std::endl;
    std::cout << sublist.full_name() << std::endl;
    std::cout << tl.full_name() << std::endl;

    ftlist l( object::root_object_class() );
    object o(object::root_object_class() );
    l.append(&o);
    l.render(std::cout);

    std::cout << PACKAGE_STRING << std::endl;
    std::cout << "Hello, world!" << std::endl;
    yyparse();
    return 0;
}
