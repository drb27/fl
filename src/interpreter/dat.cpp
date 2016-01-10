#include <string>
#include <deque>
#include <interpreter/object.h>
#include <parser/ast.h>
#include <parser/ast_nodes.h>
#include "dat.h"
#include <interpreter/typemgr.h>

using std::string;
using std::deque;

dat::dat(typemgr& tm,context* pContext) : _tm(tm),_context(pContext)
{
}

dat::~dat()
{

}

ast* dat::make_int(int x) const
{
    typespec int_spec = typespec("integer",{});
    fclass& int_cls = _tm.lookup(int_spec);
    objref pObject(new int_object(x,int_cls));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_fundef( string* name, ast* def) const
{
    delete name;
    delete def;
    return nullptr;
}

void dat::respond( ast* def, std::ostream& os) const
{
    def->evaluate(_context)->render(os);
    os << "OK" << std::endl;
}

ast* dat::make_methodcall( ast* target, std::string* method,list_node* args)
{
    auto r = new methodcall_node(*method);
    for ( auto p : args->raw_elements() )
    {
	r->add_param(p);
    }
    r->add_target(target);
    delete method;
    return r;
}

ast* dat::make_symbol( std::string* name) const
{
    auto r = new symbol_node(*name);
    delete name;
    return r;
}

ast* dat::make_assign_node(ast* lvalue, ast* rvalue)
{
    return new assign_node(lvalue,rvalue);
}

ast* dat::start_list()
{
    auto n = new list_node();
    _list_stack.push_front(n);
    return n;
}

void dat::push_list_element(ast* n)
{
    (*_list_stack.begin())->push_element(n);
}

ast* dat::finish_list()
{
    auto n = *(_list_stack.begin());
    _list_stack.pop_front();
    return n;
}
