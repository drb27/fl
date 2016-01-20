#include <iostream>
#include <functional>
#include <string>
#include <list>
#include <set>
#include <deque>
#include <vector>
#include <map>
#include <sstream>
#include <logger/logger.h>
#include "ast.h"
#include "ast_nodes.h"
#include <interpreter/object.h>
#include <interpreter/context.h>
#include <interpreter/marshall.h>

using std::ostream;
using std::endl;
using std::string;
using std::list;
using std::set;
using std::function;
using std::vector;
using std::deque;
using std::map;
using std::stringstream;

ast::ast()
{
}

ast::~ast()
{

}

void ast::invalidate() const
{
}

void ast::render_dot(int& uuid, 
		     const string& parent, 
		     const string& label,
		     ostream& os) const
{
    int myref = uuid++;

    string labelString;
    if (!parent.empty())
    {
	os << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    os << "astnode_" << myref << "[shape=box" << labelString << "];" << std::endl;
}

fclass* ast::type(context* pContext) const
{
    typespec os("object",{});
    return &(pContext->types().lookup(os));
}

literal_node::literal_node(objref pObj)
    : ast(),_object(pObj)
{
}

objref literal_node::evaluate(context* pContext)
{
    return _object;
}

void literal_node::required_symbols(std::set<std::string>&) const
{
}

fclass* literal_node::type(context* pContext) const
{
    return &(_object->get_class());
}

void list_node::push_element(ast* pNode)
{
    _elements.push_back(pNode);
}

void list_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "list_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    int paramCount=0;
    for (auto arg : _elements)
    {
	auto pc = paramCount++;
	arg->render_dot(uuid,myid," el" + std::to_string(pc),out);
    }
}

objref list_node::evaluate(context* pContext)
{
    auto l = new list_object(*(type(pContext)));
    for ( auto e : _elements )
    {
	l->internal_value().push_back(e->evaluate(pContext));
    }
    return objref(l);
}

void list_node::required_symbols(std::set<std::string>& s) const
{
    for ( auto e : _elements)
    {
	e->required_symbols(s);
    }
}

std::list<ast*>& list_node::raw_elements()
{
    return _elements;
}

fclass* list_node::type(context* pContext) const
{

    // Get the types of all the child elements
    set<fclass*> subtypes;

    for ( auto e : _elements )
    {
	subtypes.insert( e->type(pContext) ); 
    }

    // If there is only one type, that's the type of the list
    // Otherwise, it's a list of objects
    if (subtypes.size()==1)
    {
	typespec subtypespec( (*subtypes.begin())->get_spec().full_name(), {} );
	typespec listspec( "list", {subtypespec} );
	return &(pContext->types().lookup(listspec)); 
    }
    else
    {
	typespec objspec( "object", {} );
	typespec listspec( "list", { pContext->types().lookup(objspec).get_spec() } );
	return &(pContext->types().lookup(listspec)); 
    }
    
}

methodcall_node::methodcall_node(const std::string& name)
    : _name(name)
{

}

void methodcall_node::render_dot(int& uuid, 
				 const std::string& parent, 
				 const std::string& label,
				 std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "methodcall_" + _name + "_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;
    _target->render_dot(uuid,myid," target",out);

    int paramCount=0;
    for (auto arg : _params)
    {
	auto pc = paramCount++;
	arg->render_dot(uuid,myid," param" + std::to_string(pc),out);
    }
}

objref methodcall_node::evaluate(context* pContext)
{
    // Evaluate the target
    objref target = _target->evaluate(pContext);

    // Look up the method on the class
    function<marshall_mthd_t> m = target->get_class().lookup_method(_name);
    
    // Prepare the parameter vector
    auto params = vector<ast*>(_params.size()+2);

    int index=2;
    for ( auto p : _params )
    {
	params[index++] = p;
    }
    
    // Dispatch the call
    auto retVal =  m(pContext,target,params);

    // Invalidate cache
    _target->invalidate();

    // Return result
    return retVal;

}

void methodcall_node::required_symbols(std::set<std::string>& s) const
{
    _target->required_symbols(s);
    for (auto p : _params)
    {
	p->required_symbols(s);
    }
}

void methodcall_node::add_target(ast* pObj)
{
    _target = pObj;
}

void methodcall_node::add_param(ast* pNode)
{
    _params.push_back(pNode);
}

void methodcall_node::finalize_params()
{

}

symbol_node::symbol_node(const std::string& name)
    : _name(name)
{

}

void symbol_node::render_dot(int& uuid, 
			     const std::string& parent, 
			     const std::string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "symbol_" + _name + "_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;
}

const std::string& symbol_node::name() const
{
    return _name;
}

void symbol_node::required_symbols(std::set<std::string>& s) const
{
    s.insert(_name);
}

objref symbol_node::evaluate(context* pContext)
{
    return pContext->resolve_symbol(_name); 
}


fclass* symbol_node::type(context* pContext) const
{
    // Attempt to resolve the symbol
    auto o = pContext->resolve_symbol(_name);

    if (o)
	return &(o->get_class());

    throw std::exception();
}

assign_node::assign_node(ast* lvalue,ast* rvalue, bool alias)
    : _lvalue(lvalue),_rvalue(rvalue),_alias(alias)
{

}

void assign_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "assign_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    _lvalue->render_dot(uuid,myid," lvalue",out);
    _rvalue->render_dot(uuid,myid," rvalue",out);
}

void assign_node::required_symbols(std::set<std::string>& s) const
{
    _lvalue->required_symbols(s);
    _rvalue->required_symbols(s);
}

objref assign_node::evaluate(context* pContext)
{
    auto pSymbolNodeL = dynamic_cast<symbol_node*>(_lvalue);
    if (pSymbolNodeL)
    {
	// Evaluate RHS
	auto result = _rvalue->evaluate(pContext);

	if (_alias)
	{
	    // Set up an alias to point to the same object
	    auto pSymbolNodeR = dynamic_cast<symbol_node*>(_rvalue);
	    pContext->alias(pSymbolNodeL,pSymbolNodeR);
	}
	else
	    // Assign the LHS to the symbol
	    pContext->assign(pSymbolNodeL->name(),result);

	// Return RHS
	return result;
       
    }
    else
	throw std::exception();
}

fclass* assign_node::type(context* c) const
{
    return _lvalue->type(c);
}

fundef_node::fundef_node(ast* arglist, ast* definition)
    : _arglist(arglist), _definition(definition)
{
    wlog_entry();
    wlog_exit();
}
void fundef_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;
   
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
   
    string myid = "fundef_" + std::to_string(myref);
    out << myid << "[shape=box" << labelString <<  "];" << std::endl;
    _arglist->render_dot(uuid,myid," args",out);
    _definition->render_dot(uuid,myid," def",out);

}

void fundef_node::required_symbols(std::set<std::string>& s) const
{
    // Definitions which are not arguments
    set<string> tempSet;
    set<string> args;

    // Get all used symbols
    _definition->required_symbols(tempSet);

    // Get argument symbols
    _arglist->required_symbols(args);

    // Subtract arguments from the definition set
    for ( auto arg : args)
    {
	tempSet.erase(arg);
    }

    // Add the remaining non-argument symbols to the result
    for ( auto sym : tempSet )
    {
	s.insert(sym);
    }
    
}


objref fundef_node::evaluate(context* pContext)
{
    wlog_entry();

    set<string> requiredSymbols;
    required_symbols(requiredSymbols);

    for ( auto s : requiredSymbols )
    {
	wlog(level::debug, "Required symbol: " + s );
    }

    ast* localDef = _definition;
    typespec ts("function",{});
    fclass& cls = pContext->types().lookup(ts);
    
    deque<string> argnames;

    list_node* pArgList = dynamic_cast<list_node*>(_arglist);
    for ( auto sn : pArgList->raw_elements() )
    {
	symbol_node* pSymNode = dynamic_cast<symbol_node*>(sn);
	argnames.push_back(pSymNode->name());
    }

    wlog(level::debug,"Creating a closure for required symbols...");
    std::shared_ptr<context> pClosure( new context() );
    for ( auto s : requiredSymbols )
    {
	if (pContext->is_defined(s))
	    pClosure->assign(s, pContext->resolve_symbol(s));
    }

    wlog(level::debug,"Contructing lambda for function execution and embedding closure...");
    // Construct a marshall_fn_t compatible lambda expression
    function<marshall_fn_t> fn = [localDef,pClosure](context* pContext, vector<ast*>& arglist)
	{
	    wlog_entry();
	    wlog(level::debug,"Executing lambda for fl function call...");
	    wlog(level::debug,"Copying global context...");
	    std::shared_ptr<context> c( new context(*pContext) );
	    wlog(level::debug,"Merging in the closure...");
	    c->merge_in(*pClosure);
	    wlog_trace("Final context",c->trace());
	    wlog(level::debug,"Evaluating function definition...");
	    auto retVal = localDef->evaluate(c.get());
	    wlog(level::debug,"About to return result from fl function call lambda...");
	    return retVal;
	};

    wlog(level::debug,"Creating new fn_object instance with embedded closure (leaving fundef_node::evaluate)");
    return objref( new fn_object(cls,fn,argnames) );
    
}

fclass* fundef_node::type(context* pContext) const
{
    typespec ts("function",{});
    return &(pContext->types().lookup(ts));
}

funcall_node::funcall_node(const string& name, ast* args)
    : _name(name), _arg_list(args)
{
    wlog_entry();
    wlog_exit();
}

void funcall_node::invalidate() const
{
    _result = nullptr;
}

void funcall_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "funcall_" + _name + "_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    _arg_list->render_dot(uuid,myid," lvalue",out);
}

void funcall_node::required_symbols(std::set<std::string>& s) const
{
    // Add all the arguments
    _arg_list->required_symbols(s);

    // ... and the name of the added function
    s.insert(_name);
}

objref funcall_node::evaluate(context* pContext)
{
    wlog_entry();

    // Look up the function object in the context
    fnref fn = std::dynamic_pointer_cast<fn_object>(pContext->resolve_symbol(_name));

    // Evaluate the argument list
    listref args = std::dynamic_pointer_cast<list_object>(_arg_list->evaluate(pContext));

    // Get a list of argument names expected by the function
    auto argnames(fn->arglist());

    // Construct the argpair vector (string,objref)
    vector<fn_object::argpair_t> argpairs;
    map<string,string> trace_params;
    stringstream argstring;

    for ( auto argval : args->internal_value() )
    {
	argstring.clear();
	argstring.str("");
	string argname = argnames.front();
	argnames.pop_front();
	argpairs.push_back( fn_object::argpair_t(argname,argval));
	argval->render(argstring);
        trace_params[argname] = argstring.str();
    }

    // Call the function and return the result!
    wlog_trace("CALL: " + _name,trace_params);
    auto retVal =  (*fn)(pContext,argpairs);
    wlog_exit();
    return retVal;

}

fclass* funcall_node::type(context* pContext) const
{
    // TODO
    typespec obj_spec("object",{});
    return &(pContext->types().lookup(obj_spec));
    //throw std::exception();
}

if_node::if_node(ast* pCondition, ast* trueExpression, ast* falseExpression)
    : _condition(pCondition), _trueExpr(trueExpression), _falseExpr(falseExpression)
{
}
void if_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "if_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    _condition->render_dot(uuid,myid," cond",out);
    _trueExpr->render_dot(uuid,myid," true",out);
    _falseExpr->render_dot(uuid,myid," false",out);
}

void if_node::required_symbols(std::set<std::string>& s) const
{
    _condition->required_symbols(s);
    _trueExpr->required_symbols(s);
    _falseExpr->required_symbols(s);
}

objref if_node::evaluate(context* pContext)
{
    boolref cond = std::dynamic_pointer_cast<bool_object>(_condition->evaluate(pContext));
    
    if (cond->internal_value())
	return _trueExpr->evaluate(pContext);
    else
	return _falseExpr->evaluate(pContext);
}

fclass* if_node::type(context* pContext) const
{
    typespec ts("object",{});
    return &(pContext->types().lookup(ts));
}
