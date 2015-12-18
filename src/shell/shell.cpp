#include <config.h>
#include <iostream>
#include <map>

typedef std::map<std::string,int> varmap_t;

static varmap_t varmap;

#include <parser/actions.h>
#include <common.h>

int yyparse(void);

extern "C" void yyerror(char const* c)
{
    std::cout << "ERROR: " << c << std::endl;
}

void ac_integer(int i)
{
    std::cout << "You said " << i << std::endl; 
}

void ac_fraction(int a,int b)
{
    fraction_t f;
    std::cout << "You said " << a << "/" << b << std::endl; 
}

void ac_assign(std::string* s, int v)
{
    std::cout << (*s) << ": " << v << std::endl;
    varmap[*s]=v;
    delete s;
}

void ac_show_def(std::string* s)
{
    if ( varmap.find(*s)!=varmap.end())
    {
    std::cout << (*s) << ": " << varmap[(*s)] << std::endl;
    }
    else
    {
    std::cout << "Undefined symbol" << std::endl;
    }
    delete s;
}

int main(void)
{
    std::cout << PACKAGE_STRING << std::endl;
    std::cout << "Hello, world!" << std::endl;
    yyparse();
    return 0;
}
