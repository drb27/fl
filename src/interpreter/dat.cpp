#include <memory>
#include <string>
#include <deque>
#include <vector>
#include <interpreter/object.h>
#include <parser/ast.h>
#include <parser/ast_nodes.h>
#include "dat.h"
#include <interpreter/typemgr.h>
#include <interpreter/context.h>

using std::string;
using std::deque;
using std::shared_ptr;
using std::vector;

dat::dat(shared_ptr<context> pContext) : _context(pContext)
{
}

dat::~dat()
{

}

ast* dat::make_int(int x) const
{
    typespec int_spec = typespec("integer",{});
    fclass& int_cls = _context->types().lookup(int_spec);
    objref pObject(new int_object(x,int_cls));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_null() const
{
    typespec obj_spec = typespec("object",{});
    fclass& obj_cls = _context->types().lookup(obj_spec);
    objref pObject(new null_object(obj_cls));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_fundef( ast* arglist,  ast* def) const
{
    return new fundef_node(arglist,def);
}

void dat::respond( ast* def, std::ostream& os) const
{
    def->evaluate(_context.get())->render(os);
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

ast* dat::make_alias(ast* alias, ast* existing) const
{
    return new assign_node(alias,existing,true);
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

ast* dat::build_list( ast* list, ast* element)
{
    auto r = new methodcall_node("duplicate_and_append");
    r->add_target(list);
    r->add_param(element);
    return r;
}

ast* dat::make_empty_list()
{
    return new list_node();
}

ast* dat::make_single_list(ast* item)
{
    auto nl = new list_node();
    nl->push_element(item);
    return nl;
}

ast* dat::make_bool(bool b)
{
    typespec bool_spec = typespec("boolean",{});
    fclass& bool_cls = _context->types().lookup(bool_spec);
    objref pObject(new bool_object(b,bool_cls));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_funcall( ast* fn,  ast* args) const
{
    symbol_node* snode = dynamic_cast<symbol_node*>(fn);
    return new funcall_node(snode->name(),args);
}

ast* dat::make_ifnode( ast* condExpr,  ast* trueExpr, ast* falseExpr) const
{
    return new if_node(condExpr, trueExpr, falseExpr);
}
void dat::render(ast* node)
{
    auto& out = std::cout;
    int uuid=0;

    out << "digraph G {" << std::endl;
    node->render_dot(uuid,"","",out);
    out << "}" << std::endl;
}
