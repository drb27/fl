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
#include <interpreter/package.h>
#include <interpreter/eval_exception.h>
#include <logger/logger.h>
#include <interpreter/builtins.h>
#include <interpreter/factory.h>

#ifdef HAVE_CHDIR
#include <unistd.h>
#endif

using std::string;
using std::deque;
using std::shared_ptr;
using std::vector;
using std::function;
using std::list;

dat::dat(package* pPkg,
	 function<void(const string&)> fn,
	 function<void(const string&)> fn_eval
	 )
    : _current_pkg(pPkg), _root_pkg(pPkg),_include_fn(fn),_eval_fn(fn_eval)
{
}

dat::~dat()
{

}

ast* dat::make_lazy(const astref& e)
{
    lazyref l = lazyref(new lazy_object(_current_pkg,e));
    return new literal_node(l);
}

ast* dat::make_raise(const astref& clsExpr)
{
    return new raise_node(clsExpr);
}

ast* dat::make_int(int x) const
{
    objref pObject = factory::bootstrap_integer(_current_pkg,x);
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}
ast* dat::make_float(double v)
{
    objref pObject(new float_object(_current_pkg,v));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_string(std::string* x) const
{
    objref pObject(new string_object(_current_pkg, (*x).substr(1,(*x).length()-2) ));
    delete x;
    return new literal_node(pObject);
}

ast* dat::make_null() const
{
    objref pObject(new void_object(_current_pkg));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_fundef( const astref& arglist,  const astref& def) const
{
    return new fundef_node(arglist,def);
}

void dat::respond( const astref& def, bool abbrev, std::ostream& os)
{
    
    {
	astref pDef(def);
	try
	{
	    _current_pkg->set_root_node(pDef);
	    objref result = pDef->evaluate(_current_pkg);
	    _current_pkg->set_root_node(nullptr);
	    result->render(os,abbrev);
	    os << "OK" << std::endl;
	    _current_pkg->assign(std::string("_last"),result);
	}
	catch( eval_exception& e )
	{
	    _current_pkg->set_root_node(nullptr);
	    _symbol_stack.clear();
	    _seq_stack.clear();
	    _sel_stack.clear();
	    _list_stack.clear();
	    throw;
	}
    }
}
void dat::show_cmd( const astref& def, std::ostream& os)
{
    respond(def,false,os);
}

void dat::nodecount_cmd()
{
    std::cout << "Nodes: " << ast::count() << std::endl;
}

void dat::include_cmd( const astref& fname)
{
    // Evaluate the filename
    objref result = fname->evaluate(_current_pkg);

    // Check that it evaluated to a string
    if ( &(result->get_class()) != builtins::string::get_class() )
	throw eval_exception( cerror::unsupported_argument,
			      "The include command requires a string argument" );

    stringref pFnameStr = std::dynamic_pointer_cast<string_object>(result);
 
    // Set a callback!
    _include_fn( pFnameStr->internal_value() );
}

void dat::cd_cmd( const astref& fname)
{
#ifdef HAVE_CHDIR
    
    stringref newDir = object::cast_or_abort<string_object>(fname->evaluate(_current_pkg));
    auto result = chdir(newDir->internal_value().c_str());

#ifdef HAVE_GETCWD
    
    char* buf = getcwd(nullptr,0);
    std::cout << "Working directory is now " << buf << std::endl;
    free(buf);

#endif

#else

    throw eval_exception(cerror::unsupported_feature,
			 "This feature is not supported by the build environment" );
#endif

}

void dat::eval_cmd( const astref& stmtString)
{
    // Evaluate the string argument
    objref result = stmtString->evaluate(_current_pkg);

    // Check that it evaluated to a string
    if ( &(result->get_class()) != builtins::string::get_class() )
	throw eval_exception( cerror::unsupported_argument,
			      "The eval command requires a string argument" );

    stringref pStmt = std::dynamic_pointer_cast<string_object>(result);
 
    // Set a callback!
    _eval_fn(pStmt->internal_value() );
}

ast* dat::make_attr( const astref& target, std::string* selector)
{
    attr_node* pAttrNode = new attr_node(target,*selector);
    delete selector;
    return pAttrNode;
}

ast* dat::make_equality(const astref& target, const astref& other)
{
    listnoderef args = listnoderef(new list_node());
    symnoderef method = symnoderef(new symbol_node("eq"));
    args->push_element(other);
    return make_methodcall(target,method,args);
}

ast* dat::make_index(const astref& target, const astref& index)
{
    listnoderef args = listnoderef(new list_node());
    symnoderef method = symnoderef(new symbol_node(".index"));
    args->push_element(index);
    return make_methodcall(target,method,args);
}

ast* dat::make_methodcall( const astref& target, const astref& method, const astref& args)
{
    symnoderef pMethodNameNode(std::dynamic_pointer_cast<symbol_node>(method));
    auto r = new methodcall_node(pMethodNameNode->name());

    // Hack calls to new
    if ( pMethodNameNode->name()=="new")
    {
	// Syntactic hack: To allow the user to write: 
	//     <class>.new(1,2,3)
	// instead of:
	//     <class>.new( (1,2,3) )
	// we need to encapsulate the parameters into another list node
	
	listnoderef newArgList(new list_node() );
	newArgList->push_element(args);
	r->add_param_list(newArgList);
    }
    else
	r->add_param_list(args);
    
    r->add_target(target);
    return r;
}

ast* dat::make_symbol( std::string* name, const list<string>& scopespec) const
{
    auto r = new symbol_node(*name);
    if (scopespec.size())
	r->add_pkg_spec(scopespec);
    delete name;
    return r;
}

ast* dat::make_alias(const astref& alias, const astref& existing) const
{
    return new assign_node(alias,existing,true);
}

ast* dat::make_assign_node(const astref& lvalue, const astref& rvalue,bool alias)
{
    return new assign_node(lvalue,rvalue,alias);
}

ast* dat::make_enum_class(string* name,const astref& pDefList)
{
    auto r = new enum_node(*name,pDefList);
    delete name;
    return r;
}

ast* dat::make_new_class(std::string* name,const astref& pDeriveList)
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

void dat::push_list_element(const astref& n)
{
    (*_list_stack.begin())->push_element(n);
}

void dat::push_list_element_with_typehint(const astref& n,const astref& t)
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

ast* dat::build_list( const astref& list, const astref& element)
{
    auto argList = new list_node();
    argList->push_element(element);
    auto r = new methodcall_node("duplicate_and_append");
    r->add_target(list);
    r->add_param_list(astref(argList));
    return r;
}

ast* dat::make_empty_list()
{
    return new list_node();
}

ast* dat::make_single_list(const astref& item)
{
    auto nl = new list_node();
    nl->push_element(item);
    return nl;
}

ast* dat::make_bool(bool b)
{
    objref pObject(new bool_object(_current_pkg,b));
    literal_node* pNode = new literal_node(pObject);
    return pNode;
}

ast* dat::make_funcall( const astref& fn,  const astref& args) const
{
    symnoderef snode = std::dynamic_pointer_cast<symbol_node>(fn);
    auto r =  new funcall_node(snode->sym_spec(),args);
    return r;
}

ast* dat::make_ifnode( const astref& condExpr,  const astref& trueExpr, const astref& falseExpr) const
{
    return new if_node(condExpr, trueExpr, falseExpr);
}

ast* dat::make_seq()
{
    auto s = new sequence_node();
    _seq_stack.push_front(s);
    return s;
}

void dat::add_expr(const astref& expr)
{
    (*_seq_stack.begin())->add_expr(expr);
}

void dat::finish_seq()
{
    _seq_stack.pop_front();
}

void dat::render(const astref& node)
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

ast* dat::make_pair(const astref& f,const astref& s)
{
    return new pair_node(f,s);
}

ast* dat::start_observed_expression()
{
    auto s = astref(new symbol_node("_signal"));
    return make_selector(s);
}

ast* dat::make_selector(const astref& pSel)
{
    auto s = new selector_node(pSel);
    _sel_stack.push_front(s);
    return s;
}

void dat::selector_default(const astref& pDefault)
{
    (*_sel_stack.begin())->set_default(pDefault);
}

void dat::selector_predicate(const astref& e)
{
    (*_sel_stack.begin())->set_predicate(e);
}

void dat::selector_handle_predicate(void)
{
    (*_sel_stack.begin())->use_handle_predicate();
}

void dat::selector_condition(const astref& pCondPair)
{
    
    (*_sel_stack.begin())->add_condition(std::dynamic_pointer_cast<pair_node>(pCondPair));
}

selector_node* dat::finish_selector()
{
    auto s = _sel_stack.front();
    _sel_stack.pop_front();
    return s;
}

ast* dat::finish_symbol()
{
    string last = _symbol_stack.back();
    auto pLast = new string(last);
    _symbol_stack.pop_back();
    auto result = make_symbol(pLast,_symbol_stack);
    _symbol_stack.clear();
    return result;
}

void dat::push_symbol_identifier(std::string* s)
{
    _symbol_stack.push_back(*s);
    delete s;
}

ast* dat::make_while(const astref& pCond,const astref& pAction)
{
    return new while_node(pCond,pAction);
}

void dat::switch_package( const astref& symbol )
{
    if ( symbol->type()!=asttype::symbol)
	throw eval_exception(cerror::unsupported_argument,
			     "The package command requires a symbolic argument, e.g. root::mypackage");

    auto pSymNode = std::dynamic_pointer_cast<symbol_node>(symbol);
    package* currentPkg = _root_pkg;
    list<string> package_specs = pSymNode->pkg_spec();
    package_specs.push_back( pSymNode->name() );

    if ( package_specs.front()!="root")
	throw eval_exception(cerror::unsupported_argument,
			     "The argument to the package command must begin with root::");

    package_specs.pop_front();
    for ( auto s : package_specs )
    {
	package* pChild = currentPkg->get_child(s);
	if (!pChild)
	{
	    pChild = currentPkg->add_child(s);
	}
	
	currentPkg = pChild;
    }

    _current_pkg = currentPkg;

}

void dat::push_package()
{
    _pkg_stack.push_back(_current_pkg);
}

void dat::pop_package()
{
    _current_pkg = _pkg_stack.back();
    _pkg_stack.pop_back();
}
