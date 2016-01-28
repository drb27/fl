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
#include <interpreter/eval_exception.h>

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
    return &(pContext->types()->lookup(os));
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
    // Make a list of evaluated elements
    list<objref> items;
    
    for ( auto e : _elements )
    {
	items.push_back(e->evaluate(pContext));
    }

    auto l = new list_object(pContext,*(type(pContext)),items);
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
	return &(pContext->types()->lookup(listspec)); 
    }
    else
    {
	typespec objspec( "object", {} );
	typespec listspec( "list", { pContext->types()->lookup(objspec).get_spec() } );
	return &(pContext->types()->lookup(listspec)); 
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
    state_guard g(pContext);
    // Evaluate the target
    objref target = _target->evaluate(pContext);

    // Look up the method on the class
    function<marshall_mthd_t> m = target->get_class().lookup_method(_name).fn;
    
    // Prepare the parameter vector
    auto params = vector<ast*>(_params.size()+2);

    int index=2;
    for ( auto p : _params )
    {
	params[index++] = p;
    }

    g.new_collection();

    // Dispatch the call
    auto retVal =  m(pContext,target,params);

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

attr_node::attr_node(ast* target, const string& selector)
    : _target(target), _selector(selector)
{

}

objref attr_node::evaluate(context* pContext)
{
    // Get the target
    objref target = _target->evaluate(pContext);

    // Check the attribute exists
    if ( target->has_attribute(_selector))
    {
	// Attribute exists
	return target->get_attribute(_selector);
    }
    else
    {
	// Attribute does not exist
	throw eval_exception(cerror::missing_attribute,"Could not find attribute " + _selector);
    }
}

void attr_node::required_symbols( set<string>& rs ) const
{
    _target->required_symbols(rs);
}

void attr_node::render_dot(int& uuid, 
			   const std::string& parent, 
			   const std::string& label, 
			   std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "attr_" + _selector + "_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;
    _target->render_dot(uuid,myid," target",out);
}

symbol_node::symbol_node(const std::string& name)
    : _name(name)
{

}

function<void(objref)> symbol_node::setter(context* pContext)
{
    return [pContext,this](objref pNewVal)
    {
	pContext->assign(_name,pNewVal); 
    };
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
    // evaluate RHS and LHS

    auto rhs = _rvalue->evaluate(pContext);
    rhs->optimise();

    if (_alias)
    {
	if ( _lvalue->is_lvalue())
	{
	    lvalue_node* pLvalue = dynamic_cast<lvalue_node*>(_lvalue);
	    auto setfn = pLvalue->setter(pContext);
	    setfn(rhs);
	    auto lhs = _lvalue->evaluate(pContext);
	    return lhs;
	}
	else
	{
	    throw eval_exception(cerror::invalid_lvalue,"Invalid lvalue");
	}
    }
    else
    {
	auto lhs = _lvalue->evaluate(pContext);
	// Call the assign method on the lhs
	methodinfo m = lhs->get_class().lookup_method(".assign");
	vector<ast*> params(3);
	params[2] = _rvalue;
	m.fn(pContext,lhs,params);
	return lhs;
    }

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
    fclass& cls = pContext->types()->lookup(ts);
    
    deque<string> argnames;

    list_node* pArgList = dynamic_cast<list_node*>(_arglist);
    for ( auto sn : pArgList->raw_elements() )
    {
	symbol_node* pSymNode = dynamic_cast<symbol_node*>(sn);
	argnames.push_back(pSymNode->name());
    }

    wlog(level::debug,"Creating a closure for required symbols...");
    colref pClosure( new collection );
    for ( auto s : requiredSymbols )
    {
	if (pContext->is_defined(s))
	    (*pClosure)[s] = pContext->resolve_symbol(s);
    }

    wlog(level::debug,"Contructing lambda for function execution and embedding closure...");
    // Construct a marshall_fn_t compatible lambda expression
    function<marshall_fn_t> fn = [localDef,pClosure](context* pContext, vector<ast*>& arglist)
	{
	    state_guard g(pContext);
	    wlog_entry();
	    wlog(level::debug,"Executing lambda for fl function call...");
	    wlog(level::debug,"Merging in the closure...");
	    g.new_collection(pClosure);
	    g.new_collection();
	    wlog_trace("Final context",pContext->trace());
	    wlog(level::debug,"Evaluating function definition...");
	    auto retVal = localDef->evaluate(pContext);
	    wlog(level::debug,"About to return result from fl function call lambda...");
	    return retVal;
	};

    wlog(level::debug,"Creating new fn_object instance with embedded closure (leaving fundef_node::evaluate)");
    return objref( new fn_object(pContext,cls,fn,argnames,{}) );
    
}

fclass* fundef_node::type(context* pContext) const
{
    typespec ts("function",{});
    return &(pContext->types()->lookup(ts));
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
    // Look up the function object in the context
    fnref fn = std::dynamic_pointer_cast<fn_object>(pContext->resolve_symbol(_name));
    return evaluate(pContext,fn);
}

objref funcall_node::evaluate(context* pContext, fnref fn)
{
    // Evaluate the argument list
    listref args = std::dynamic_pointer_cast<list_object>(_arg_list->evaluate(pContext));

    // Get a list of argument names expected by the function
    auto argnames(fn->arglist());

    // map<string,string> p;
    // p["_name"] = _name;
    // for (auto n : argnames )
    // {
    // 	p[n] = n;
    // }
    // p["args->size()"] = std::to_string(args->internal_value().size());
    // wlog_entry_params(p);

    // Construct the argpair vector (string,objref)
    vector<fn_object::argpair_t> argpairs;

    // stringstream s;
    // for (auto argval : args->internal_value() )
    // {
    // 	s.clear();
    // 	s.str("");
    // 	argval->render(s);
    // 	wlog(level::debug,s.str());
    // }

    for ( int index=0; index < args->size() ; index++ )
    {
	wlog(level::debug,argnames.front());
	string argname = argnames.front();
	argnames.pop_front();
	argpairs.push_back( fn_object::argpair_t(argname,args->get_element(index)));
    }

    // Call the function and return the result!
    wlog_trace("CALL: " + _name,{});//trace_params);
    auto retVal =  (*fn)(pContext,argpairs);
    wlog_exit();
    return retVal;

}

fclass* funcall_node::type(context* pContext) const
{
    // TODO
    typespec obj_spec("object",{});
    return &(pContext->types()->lookup(obj_spec));
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
    return &(pContext->types()->lookup(ts));
}

function<void(objref)> attr_node::setter(context* pContext)
{
    return [this,pContext](objref pNewValue)
    {
	objref target = _target->evaluate(pContext);
	target->set_attribute(_selector,pNewValue);
    };
}

sequence_node::sequence_node()
{

}

objref sequence_node::evaluate(context* pContext)
{
    objref latestResult;
    for ( auto expr : _sequence )
    {
	latestResult=expr->evaluate(pContext);
    }

    return latestResult;
}

void sequence_node::required_symbols(set<string>& s) const
{
    for ( auto expr : _sequence )
    {
	expr->required_symbols(s);
    }
}

void sequence_node::add_expr(ast* expr)
{
    _sequence.push_back(expr);
}

fclass* sequence_node::type(context* pContext) const
{
    return _sequence.back()->type(pContext);
}

pair_node::pair_node(ast* f, ast* s )
    : _first(f), _second(s)
{
}

objref pair_node::evaluate(context* pContext)
{
    return _second->evaluate(pContext);
}

fclass* pair_node::type(context* pContext) const
{
    return _second->type(pContext);
}

void pair_node::required_symbols(set<string>& s) const
{
    _first->required_symbols(s);
    _second->required_symbols(s);
}

selector_node::selector_node(ast* selector)
    : _selector(selector)
{
}

objref selector_node::evaluate(context* pContext)
{
    // Evaluate the selector
    objref selResult = _selector->evaluate(pContext);

    objref result=objref(nullptr);

    typespec bts("boolean",{});
    auto bool_cls = pContext->types()->lookup(bts);
    boolref trueObj = boolref( new bool_object(pContext,true,bool_cls));

    // Search the conditions in order
    for ( auto pair : _conditions )
    {
	// Evaluate the guard
	auto firstResult = pair->first()->evaluate(pContext);

	// Compare the guard to the selector value
	vector<objref> params{firstResult};
	boolref is_equal = std::dynamic_pointer_cast<bool_object>(selResult->invoke("eq",pContext,params));
	if ( is_equal->internal_value() )
	{
	    // We have a match!
	    result = pair->second()->evaluate(pContext);
	    break;
	}   
    }
    
    // Match found?
    if (result)
    {
	// Yes, return it now
	return result;
    }
    else
    {
	// No match - do we have a default?
	if (_default)
	{
	    // Evaluate and return the default
	    return _default->evaluate(pContext);
	}
	else
	{
	    // Failed to match anything
	    throw eval_exception(cerror::selector_nomatch, "Selector failed to match anything");
	}
    }

    // TODO
    return objref(nullptr);
}

fclass* selector_node::type(context* pContext) const
{
    typespec ts("object",{});
    return &(pContext->types()->lookup(ts));    
}

void selector_node::required_symbols(set<string>& s ) const
{
    _selector->required_symbols(s);
    if (_default)
	_default->required_symbols(s);

    for ( auto pair : _conditions )
    {
	pair->first()->required_symbols(s);
	pair->second()->required_symbols(s);
    }
}

void selector_node::add_condition(ast* pair)
{
    pair_node* pPairNode = dynamic_cast<pair_node*>(pair);
    _conditions.push_back(pPairNode);
}

void selector_node::set_default(ast* defaultExpr)
{
    _default = defaultExpr;
}

tailrec_node::tailrec_node(ast* pFn, ast* pCond, ast* paramUpdates)
    : _fncall(pFn), _cond(pCond), _params(paramUpdates)
{

}

#define BOOL_CAST(x) (std::dynamic_pointer_cast<bool_object>(x))->internal_value()

objref tailrec_node::evaluate(context* pContext )
{
    // Create a new context level for this *series* of calls
    state_guard g(pContext);
    g.new_collection();

    // Evaluate the function call
    auto fn = std::dynamic_pointer_cast<fn_object>(_fncall->evaluate(pContext) );

    objref result(nullptr);

    // Loop around executing the function until the condition is true
    while ( BOOL_CAST( _cond->evaluate(pContext) ) )
    {
	// Evaluate the parameters

	// Execute the function NON-RECURSIVELY
	//objref result = (*fn)(pContext,{}); // TODO - insert params here!

	// Set up the parameters for the next call
    }
    
    return result;
}

void tailrec_node::required_symbols(set<string>& s ) const
{
    _fncall->required_symbols(s);
    _cond->required_symbols(s);
    _params->required_symbols(s);
}

void tailrec_node::render_dot(int& uuid, 
			     const string& parent,
			     const string& label,
			     std::ostream& out) const
{
    int myref=uuid++;
    string labelString;

    string myid = "tailrec_" + std::to_string(myref);
    if (!parent.empty())
    {
	out << parent << " -> ";
	labelString = ",label=\"" + label + "\"";
    }
    out << myid << "[shape=box" << labelString << "];" << std::endl;

    _fncall->render_dot(uuid,myid," fn",out);
    _cond->render_dot(uuid,myid," cond",out);
    _params->render_dot(uuid,myid," params",out);
}

