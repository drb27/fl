#include <string.h>
#include <interpreter/object.h>
#include <parser/ast.h>
#include "dat.h"
#include <interpreter/typemgr.h>

using std::string;

dat::dat(typemgr& tm,context* pContext) : _tm(tm),_context(pContext)
{
}

dat::~dat()
{

}

ast* dat::make_int(int x) const
{
    typespec int_spec = typespec("integer",{});
    const fclass& int_cls = _tm.lookup(int_spec);
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

ast* dat::make_methodcall( std::string* target, std::string* method)
{
    auto r = new methodcall_node(*method);
    delete method;
    delete target;
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
