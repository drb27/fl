#include <config.h>
#include <iostream>

extern "C" {
#include <parser/actions.h>
}

extern "C" int yyparse(void);

extern "C" void yyerror(char* c)
{
    std::cout << "ERROR: " << c << std::endl;
}

extern "C" void ac_integer(int i)
{
    std::cout << "You said " << i << std::endl; 
}

int main(void)
{
    std::cout << PACKAGE_STRING << std::endl;
    std::cout << "Hello, world!" << std::endl;
    yyparse();
    return 0;
}
