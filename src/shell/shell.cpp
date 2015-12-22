#include <config.h>
#include <iostream>
#include <common.h>

int yyparse(void);

extern "C" void yyerror(char const* c)
{
    std::cout << "ERROR: " << c << std::endl;
}


int main(void)
{
    std::cout << PACKAGE_STRING << std::endl;
    std::cout << "Hello, world!" << std::endl;
    yyparse();
    return 0;
}
