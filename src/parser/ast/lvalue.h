#ifndef AST_LVALUE
#define AST_LVALUE

#include <iostream>
#include <string>
#include <functional>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;

class lvalue_node : public ast
{
public:
    virtual bool is_lvalue() const { return true; }
    virtual std::function<void(objref)> setter(context*)=0;
};

#endif
