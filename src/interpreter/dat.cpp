#include <inc/config.h>
#include <memory>
#include <string>
#include <deque>
#include <vector>
#include <parser/ast/ast.h>
#include <parser/ast/nodes.h>
#include <interpreter/obj/all.h>
#include "dat.h"
#include <interpreter/typemgr.h>
#include <interpreter/context.h>
#include <interpreter/eval_exception.h>
#include <logger/logger.h>
#include <interpreter/builtins.h>

#ifdef HAVE_CHDIR
#include <unistd.h>
#endif

using std::string;
using std::deque;
using std::shared_ptr;
using std::vector;
using std::function;

dat::dat(context* pContext,
	 function<void(const string&)> fn,
	 function<void(const string&)> fn_eval
	 )
    : _context(pContext), _include_fn(fn),_eval_fn(fn_eval)
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
ast* dat::make_float(double v)
{
    objref pObject(new float_object(_context,v));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_string(std::string* x) const
{
    objref pObject(new string_object(_context, (*x).substr(1,(*x).length()-2) ));
    delete x;
    return new literal_node(pObject);
}

ast* dat::make_null() const
{
    objref pObject(new void_object(_context));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_fundef( ast* arglist,  ast* def) const
{
    return new fundef_node(arglist,def);
}

void dat::respond( ast* def, bool abbrev, std::ostream& os) const
{
    try
    {
	objref result = def->evaluate(_context);
	result->render(os,abbrev);
	os << "OK" << std::endl;
	_context->assign("_last",result);
    }
    catch( eval_exception& e )
    {
	throw e;
    }
}
void dat::show_cmd( ast* def, std::ostream& os)
{
    respond(def,false,os);
}

void dat::include_cmd( ast* fname)
{
    // Evaluate the filename
    objref result = fname->evaluate(_context);

    // Check that it evaluated to a string
    if ( &(result->get_class()) != builtins::string::get_class() )
	throw eval_exception( cerror::unsupported_argument,
			      "The include command requires a string argument" );

    stringref pFnameStr = std::dynamic_pointer_cast<string_object>(result);
 
    // Set a callback!
    _include_fn( pFnameStr->internal_value() );
}

void dat::cd_cmd( ast* fname)
{
#ifdef HAVE_CHDIR
    
    stringref newDir = object::cast_or_abort<string_object>(fname->evaluate(_context));
    auto result = chdir(newDir->internal_value().c_str());
    delete fname;

#ifdef HAVE_GETCWD
    
    char* buf = getcwd(nullptr,0);
    std::cout << "Working directory is now " << buf << std::endl;
    free(buf);

#endif

#else

    delete fname;
    throw eval_exception(cerror::unsupported_feature,
			 "This feature is not supported by the build environment" );
#endif

}

void dat::eval_cmd( ast* stmtString)
{
    // Evaluate the string argument
    objref result = stmtString->evaluate(_context);

    // Check that it evaluated to a string
    if ( &(result->get_class()) != builtins::string::get_class() )
	throw eval_exception( cerror::unsupported_argument,
			      "The eval command requires a string argument" );

    stringref pStmt = std::dynamic_pointer_cast<string_object>(result);
 
    // Set a callback!
    _eval_fn(pStmt->internal_value() );
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

    if ( pMethodNameNode->name()=="new")
    {
	// Syntactic hack: To allow the user to write: 
	//     <class>.new(1,2,3)
	// instead of:
	//     <class>.new( (1,2,3) )
	// we need to encapsulate the parameters into another list node
	
	list_node* originalArgList = args;
	args = new list_node();
	args->push_element(originalArgList);
    }

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

ast* dat::make_enum_class(string* name,ast* pDefList)
{
    auto r = new enum_node(*name,pDefList);
    delete name;
    return r;
}

ast* dat::make_new_class(std::string* name,ast* pDeriveList)
{
    auto r = new classdef_node(*name,pDeriveList);
    delete name;
    return r;
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

void dat::push_list_element_with_typehint(ast* n,ast* t)
{
    n->set_typehint(t);
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
    objref pObject(new bool_object(_context,b));
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

ast* dat::make_while(ast* pCond,ast* pAction)
{
    return new while_node(pCond,pAction);
}
