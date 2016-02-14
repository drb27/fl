#ifndef AST_WHILE
#define AST_WHILE

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <inc/references.h>
#include <parser/ast/ast.h>

class context;

class while_node : public ast
{
public:
    while_node(ast* pCond,ast* pAction);
    virtual objref evaluate(context*);
    virtual void required_symbols(std::set<std::string>&) const;

    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

private:

    ast* _cond;
    ast* _action;

};

#endif
