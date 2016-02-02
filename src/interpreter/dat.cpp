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
#include <interpreter/eval_exception.h>
#include <logger/logger.h>

using std::string;
using std::deque;
using std::shared_ptr;
using std::vector;

dat::dat(context* pContext) : _context(pContext)
{
}

dat::~dat()
{

}

ast* dat::make_int(int x) const
{
    objref pObject(new int_object(_context,x));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_string(std::string* x) const
{
    typespec string_spec = typespec("string",{});
    fclass& string_cls = _context->types()->lookup(string_spec);
    objref pObject(new string_object(_context,(*x).substr(1,(*x).length()-2),string_cls));
    delete x;
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_null() const
{
    typespec obj_spec = typespec("void",{});
    fclass& obj_cls = _context->types()->lookup(obj_spec);
    objref pObject(new void_object(_context,obj_cls));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_fundef( ast* arglist,  ast* def) const
{
    return new fundef_node(arglist,def);
}

void dat::respond( ast* def, std::ostream& os) const
{
    try
    {
	def->evaluate(_context)->render(os);
	os << "OK" << std::endl;
    }
    catch( eval_exception& e )
    {
	//wlog(level::error,e.what());
	throw e;
    }
}

ast* dat::make_attr( ast* target, std::string* selector)
{
    attr_node* pAttrNode = new attr_node(target,*selector);
    delete selector;
    return pAttrNode;
}

ast* dat::make_equality(ast* target, ast* other)
{
    list_node* args = new list_node();
    symbol_node* method = new symbol_node("eq");
    args->push_element(other);
    return make_methodcall(target,method,args);
}

ast* dat::make_index(ast* target, ast* index)
{
    list_node* args = new list_node();
    symbol_node* method = new symbol_node(".index");
    args->push_element(index);
    return make_methodcall(target,method,args);
}

ast* dat::make_methodcall( ast* target, ast* method,list_node* args)
{
    symbol_node* pMethodNameNode = dynamic_cast<symbol_node*>(method);
    auto r = new methodcall_node(pMethodNameNode->name());
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

ast* dat::make_assign_node(ast* lvalue, ast* rvalue,bool alias)
{
    return new assign_node(lvalue,rvalue,alias);
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
    fclass& bool_cls = _context->types()->lookup(bool_spec);
    objref pObject(new bool_object(_context,b,bool_cls));
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

ast* dat::make_seq()
{
    auto s = new sequence_node();
    _seq_stack.push_front(s);
    return s;
}

void dat::add_expr(ast* expr)
{
    (*_seq_stack.begin())->add_expr(expr);
}

void dat::finish_seq()
{
    _seq_stack.pop_front();
}

void dat::render(ast* node)
{
    auto& out = std::cout;
    int uuid=0;

    out << "digraph G {" << std::endl;
    node->render_dot(uuid,"","",out);
    out << "}" << std::endl;
}

void dat::enable_debug(bool enable)
{
    (enable)? g_logger.enable(level::debug) : g_logger.disable(level::debug);
}

void dat::enable_trace(bool enable)
{
    (enable)? g_logger.enable(level::trace) : g_logger.disable(level::trace);
}

void dat::done()
{
    throw terminate_exception();
}

ast* dat::make_pair(ast* f,ast* s)
{
    return new pair_node(f,s);
}

ast* dat::make_selector(ast* pSel)
{
    auto s = new selector_node(pSel);
    _sel_stack.push_front(s);
    return s;
}

ast* dat::selector_default(ast* pDefault)
{
    (*_sel_stack.begin())->set_default(pDefault);
}

ast* dat::selector_condition(ast* pCondPair)
{
    (*_sel_stack.begin())->add_condition(pCondPair);
}

ast* dat::finish_selector()
{
    _sel_stack.pop_front();
}
