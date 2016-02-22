#ifndef AST_FUNCALL
#define AST_FUNCALL

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <inc/references.h>
#include <parser/ast/ast.h>
#include <interpreter/context.h>

class fclass;

class funcall_node : public ast
{
public:
    funcall_node(const symspec&,ast*,fnref capturedFn=fnref(nullptr) );
    virtual objref raw_evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<symspec>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual bool calls_and_returns( const std::string& fname) const;
    virtual asttype type() const;
    virtual void direct_subordinates( std::list<ast*>& ) const;

    virtual ast* args() const { return _arg_list; }
    virtual const std::string& name() const { return _symbol.name(); }

protected:
    const symspec _symbol;
    ast* _arg_list;
    mutable objref _result;
    fnref _captured_fn;
};

#endif
