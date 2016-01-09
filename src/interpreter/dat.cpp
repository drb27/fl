#include <string.h>
#include <interpreter/object.h>
#include <parser/ast.h>
#include "dat.h"
#include <interpreter/typemgr.h>

using std::string;

dat::dat(typemgr& tm) : _tm(tm)
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
    def->evaluate(nullptr)->render(os);
    os << "OK" << std::endl;
}
