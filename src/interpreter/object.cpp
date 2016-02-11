#include <functional>
#include <algorithm>
#include <deque>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "object.h"
#include <cassert>
#include <interpreter/eval_exception.h>
#include <parser/ast_nodes.h>
#include <logger/logger.h>
#include <interpreter/smartlist.h>
#include <parser/ast.h>
#include <parser/ast_nodes.h>

using std::string;
using std::function;
using std::deque;
using std::vector;
using std::endl;
using std::map;
using std::set;
using std::pair;

object::object(context* pContext, fclass& c, vector<objref> params) 
    : _class(c),_context(pContext)
{
    if ( c.is_abstract() )
	throw eval_exception(cerror::instantiate_abstract,
			     "An attempt was made to instantiate an object of an abstract class");

    // Create the attributes for this class, and the base chain
    fclass* pCurrentClass = &c;

    auto fn = [this](fclass* pClass)
	{
	    for ( auto a : pClass->attributes() )
	    {
		_attributes[a.first] = a.second;
	    }
	};

    for ( auto pCls : c.hierarchy() )
	fn(pCls);


    construct(pContext,params);
}

objref object::convert_to(objref pThis, fclass* pOther)
{
    // Are we already the right type?
    if (pOther==&(pThis->get_class()))
	return pThis;

    // ... or already a decendant of pOther?
    if ( pThis->get_class().is_in_hierarchy(*pOther) )
	return pThis;

    set<ctnoderef> solutionSet;
    bool canConvert = pThis->get_class().build_conversion_tree(pOther,solutionSet);

    if (!canConvert)
    {
	throw eval_exception(cerror::unsupported_conversion,"Conversion from class " 
			     + pThis->get_class().name() + " to class " 
			     + pOther->name() + " is not supported");
    }

    // Select the shortest solution (fewest number of conversion calls)
    int shortest=0;
    ctnoderef solution;

    for ( auto sln : solutionSet )
    {
	if (!shortest)
	{
	    shortest=sln->length();
	    solution = sln;
	}
	else
	{
	    int thisLength = sln->length();
	    if ( thisLength<shortest)
	    {
		shortest = thisLength;
		solution = sln;
	    }
	}
    }

    // Now solution points to the shortest solution. Built a list of method
    // names to call

    deque<string> methodChain;
    while (solution)
    {
	methodChain.push_front( "->" + solution->cls->name() );
	solution = solution->parent;
    }

    // Remove the first call, it is unnecessary
    methodChain.pop_front();

    // Call each method on ourselves
    objref currentObject = pThis;
    vector<objref> params;
    for (auto mth : methodChain)
    {
	currentObject = currentObject->invoke(mth,pThis->get_context(),params);
    }

    return currentObject;
}

bool object::operator==(const objref other) const
{
    return false;
}

object& object::operator=(const objref other)
{
    throw eval_exception(cerror::invalid_assignment, "Incompatible assignment");
}

object::~object()
{
    wlog_entry();
}

objref object::invoke( const string& mthdName, context* pContext, vector<objref>& params)
{
    // Create a methodcall node to do all of the hard work for us
    methodcall_node* pCallNode = new methodcall_node(mthdName);

    // Set up the node here
    pCallNode->add_target( new literal_node( objref(this) ));

    for ( auto param : params )
    {
	pCallNode->add_param( new literal_node( param ) );
    }

    objref result = pCallNode->evaluate(pContext);
    delete pCallNode;
    return result;
}

void object::construct(context* pContext, vector<objref>& params)
{
   // Call the constructor!
    objref pThis(this, [](object*) {});
    vector<ast*> ps(2+params.size());
    int index=2;
    ps[2]=new literal_node(objref(this));
    for ( auto p : params )
    {
	ps[index++] = new literal_node(p);
    }
    
    _class.instantiator().fn(pContext,pThis,ps);

    // Tidy up
    for ( auto p : ps )
	delete p;

}

bool object::has_attribute(const std::string& name) const
{
    return _attributes.find(name)!=_attributes.end();
}

bool object::has_method(const std::string& name ) const
{
    return get_class().has_method(name);
}
    
objref object::get_attribute(const std::string& selector)
{
    return _attributes[selector];
}

void object::set_attribute(const std::string& selector, objref newValue)
{
    if ( has_attribute(selector) )
    {
	_attributes[selector] = newValue;
    }
    else
	throw eval_exception( cerror::missing_attribute, "Missing attribute " + selector);
}

void object::render( std::ostream& os, bool abbrev )
{
    if ( has_method(".render") )
    {
    	// Call the .render() method to render the object
    	objref pThis(this, [](object* o) {} );
    	literal_node* pThisLiteral = new literal_node(pThis);
    	methodcall_node* pMethodCall = new methodcall_node(".render");
    	pMethodCall->add_target(pThisLiteral);
    	stringref pRendered = std::dynamic_pointer_cast<string_object>(pMethodCall->evaluate(get_context()));
    	os << pRendered->internal_value() << " ";
    	delete pThisLiteral;
    	delete pMethodCall;
    }
    os << "[" << _class.name() << "]"; 
}

void object::dump( std::ostream& out)
{
    out << this << " ";
    render(out);
    out << std::endl;
}

class_object::class_object(context* pContext, fclass* pCls, fclass& cls)
    : object(pContext,cls),_value(pCls)
{
}

void class_object::render(std::ostream& os, bool abbrev )
{
    os << "(class " << _value->name() << ") ";
    object::render(os,abbrev);
}

bool class_object::has_attribute(const std::string& name) const
{
    const map<string,objref>& ca = _value->class_attributes();
    return ca.find(name)!=ca.end();
}

objref class_object::get_attribute(const std::string& name)
{
    return (*(_value->class_attributes().find(name))).second;
}

namespace
{
    inline fclass& get_int_cls(context* pContext)
    {
	typespec ts("integer",{});
	return pContext->types()->lookup(ts);
    }

    inline fclass& get_float_cls(context* pContext)
    {
	typespec ts("float",{});
	return pContext->types()->lookup(ts);
    }
}

int_object::int_object(context* pContext, int value)
    : object(pContext,get_int_cls(pContext)), _value(value)
{
}

int_object::int_object(context* pContext,int value,fclass& cls)
    : object(pContext,cls), _value(value)
{
}

bool int_object::operator==( const objref other ) const
{
    if (&other->get_class()!=&get_class())
	return false;

    intref other_int = std::dynamic_pointer_cast<int_object>(other);

    return (internal_value()==other_int->internal_value());
}

void int_object::render( std::ostream& os, bool abbrev)
{
    os << _value << " ";
    object::render(os,abbrev);
}

string_object::string_object(context* pContext, const std::string& value, fclass& cls) 
    : object(pContext,cls), _value(value)
{

}

string_object::string_object( const string_object& other )
    : string_object(other.get_context(),other._value,other.get_class())
{

}

bool string_object::operator==( const objref other ) const
{
    if (&other->get_class()!=&get_class())
	return false;

    stringref otherRef = std::dynamic_pointer_cast<string_object>(other);
    return (internal_value()==otherRef->internal_value());
}

stringref string_object::operator[](intref index) const
{
    string substr(_value.substr(index->internal_value(),1));
    stringref result = stringref(new string_object(get_context(),
						   substr,
						   get_class()) );
    return result;
}

stringref string_object::operator+(const stringref other) const
{
    // Create a new string object as a copy of this one
    stringref pReturn = stringref( new string_object(*this) );
    pReturn->inplace_join(other);
    return pReturn;
}

void string_object::inplace_join(const stringref other)
{
    _value.append(other->_value);
}

void string_object::render( std::ostream& os, bool abbrev)
{
    os << _value << " ";
    object::render(os,abbrev);
}

bool_object::bool_object(context* pContext,bool b, fclass& cls)
    : _value(b),object(pContext,cls)
{
}

bool bool_object::operator==( const objref other ) const
{
    if (&other->get_class()!=&get_class())
	return false;

    boolref other_bool = std::dynamic_pointer_cast<bool_object>(other);

    return (internal_value()==other_bool->internal_value());
}

void bool_object::render( std::ostream& os, bool abbrev)
{
    os << ((_value)?"true":"false") << " ";
    object::render(os,abbrev);
}

list_object::list_object(context* pContext,fclass& cls)
    : object(pContext,cls)
{
    _pList.reset(new smartlist());
}

list_object::list_object( context* pContext, const list_object& other)
    : object(pContext,other.get_class())
{
    _pList.reset( new smartlist(*(other._pList.get())) );
}

list_object::list_object(context* pContext, fclass& cls, smartlist* l)
    : object(pContext,cls)
{
    _pList.reset(l);
}

list_object::list_object(context* pContext,fclass& cls, std::list<objref> startList)
    : object(pContext,cls)
{
    if ( startList.size() )
    {
	vector<objref> items(startList.size() );
	int index=0;
	for ( auto item : startList )
	{
	    items[index++] = item;
	}

	blockref blk = chunk::make_block(items);
	_pList.reset(new smartlist());
	_pList->inplace_append(blk,items.size());
    }
    else
	_pList.reset( new smartlist() );
}

void list_object::append(objref e)
{
    _pList->inplace_append(e);
}

objref list_object::pop()
{
    objref pObject = _pList->inplace_pop();
    if (!pObject)
    {
	typespec tsv("void",{});
	pObject = objref( new void_object(get_context(),get_context()->types()->lookup(tsv)) );
    }

    return pObject;
}

void list_object::prepend(objref e)
{
    _pList->inplace_prefix(e);
}

void list_object::append(listref other )
{
    _pList->inplace_append(other->_pList.get());
}

objref list_object::first()
{
    return get_element(0);
}

int list_object::size() const
{
    return _pList->size();
}

int list_object::chunks() const
{
    return _pList->chunks();
}

void list_object::optimise()
{
    _pList->inplace_chunkify( _pList->size() );
}

objref list_object::get_element(size_t index)
{
    return _pList->get_element(index);
}

listref list_object::tail(context* pContext) const
{
    smartlist* pNewList = _pList->tail();
    return listref( new list_object(pContext,get_class(),pNewList) );
}

void list_object::render( std::ostream& os, bool abbrev)
{
    os << "(";
    int maxindex = abbrev? (_pList->size()>5)?5:_pList->size() : _pList->size(); 
    for ( int index = 0 ; index < maxindex ; index ++ )
    {
	_pList->get_element(index)->render(os); 
	os << " ";
    }
    if (_pList->size()>maxindex) 
	os << "...";
    os << ") <" << size() << "> ";
    object::render(os,abbrev);
}

void void_object::render( std::ostream& os,bool abbrev)
{
    os << "(null) ";
    object::render(os,abbrev);
}

bool void_object::operator==( const objref other ) const
{
    return (&other->get_class()==&get_class());
}

fn_object::fn_object(context* pContext,
		     fclass& cls, 
		     rawfn impl, 
		     hinted_args_t fullArgs,
		     collection&& appliedArgs)
    : _full_args(fullArgs), _expected_args(fullArgs), object(pContext,cls), _fn(impl)
{
    wlog_entry();
    _is_anon=true;
    _applied_arguments=std::move(appliedArgs);
}

void fn_object::set_name(const string& fname)
{
    if (_is_anon)
    {
	_name = fname;
	_is_anon=false;
    }
}

const string& fn_object::name() const
{
    return _name;
}

void fn_object::render(std::ostream& os, bool abbrev)
{
    object::render(os,abbrev);
}

void fn_object::dump( std::ostream& out)
{
    object::dump(out);

    out << "Expected Args: ";
    for ( auto arg : _expected_args )
    {
	out << arg.first << " ";
    }
    out << endl;

    out << "Full Args: ";
    for ( auto arg : _full_args )
    {
	out << arg.first << " ";
    }
    out << endl;

}

fnref fn_object::partial_application(context* pContext,const vector<argpair_t>& args) const
{
    wlog_entry();
    hinted_args_t remainingArgs(_full_args);
    collection appliedArgs;

    // For each partial application, add to the context and remove from
    // the remaining args
    for ( auto arg : args )
    {
	appliedArgs[arg.first] = arg.second;
	remainingArgs.erase(std::find_if(remainingArgs.begin(),remainingArgs.end(),
					 [&arg](pair<string,ast*>& e){ return e.first==arg.first;}));
    }
    
    auto result = fnref( new fn_object(pContext,get_class(),_fn,
				       remainingArgs,std::move(appliedArgs)));
    result->_is_anon = _is_anon;
    result->_name = _name;
    return result;

}

rawfn& fn_object::raw()
{
    return _fn;
}

auto fn_object::arglist() const -> const hinted_args_t&
{
    return _full_args;
}

bool fn_object::is_anonymous() const
{
    return _is_anon;
}

bool fn_object::is_tail_recursive()
{
    // TODO
    return _fn.def()->calls_and_returns(_name);
}

void fn_object::optimise_tail_recursion(context*)
{

}

objref fn_object::operator()(context* pContext, vector<objref>& args)
{
    vector<argpair_t> argpairs;

    if ( args.size()>_full_args.size() )
	throw eval_exception(cerror::too_many_arguments, "Too many argument supplied to function call");

    if ( args.size()==_full_args.size() )
    {
	// Zip 'em all up
	int index=0;
	for ( auto a : _full_args )
	{
	    argpairs.push_back(argpair_t(a.first,args[index++]));
	}
    }
    else
    {
	// Just the expected args
	int index=0;
	for ( auto a : args )
	{
	    argpairs.push_back(argpair_t(_expected_args[index++].first,a));
	}
    }

    return (*this)(pContext,argpairs);
}

objref fn_object::operator()(context* pContext, vector<argpair_t>& args)
{
    wlog_entry();

    if ( args.size()>_full_args.size() )
	throw eval_exception(cerror::too_many_arguments, "Too many argument supplied to function call");

    // Is this a full or partial application?
    if (args.size()==_full_args.size() )
    {
	state_guard g(pContext);
	g.new_collection();
	vector<ast*> params;

	for ( auto arg : args )
	{
	    params.push_back( new symbol_node(arg.first) );
	    pContext->assign(arg.first,arg.second);
	}

	for ( auto aa : _applied_arguments )
	{
	    pContext->assign(aa.first,aa.second);
	}
	
	// Apply the accrued arguments to the marshall function
	auto result = _fn(pContext,params);
	return result;
    }
    else
	return partial_application(pContext,args);
}

fn_object::fn_object(context* pContext, const fn_object& other)
    : object(pContext,other.get_class()), _fn(other._fn), 
      _applied_arguments(other._applied_arguments), 
      _expected_args(other._expected_args), _full_args(other._full_args),
      _name(other._name)
{
    wlog_entry();
    _is_anon=other._is_anon;
}

void enum_object::render( std::ostream& os, bool abbrev)
{
    os << _name << " ";
    object::render(os,abbrev);
}

stringref enum_object::str()
{
    typespec tss("string",{});
    auto& str_class = get_context()->types()->lookup(tss);
    return stringref( new string_object(get_context(),_name,str_class) );
}

float_object::float_object(context* pContext, double value)
    : object(pContext,get_float_cls(pContext)), _value(value)
{
}

float_object::float_object(context* pContext,double value,fclass& cls)
    : object(pContext,cls), _value(value)
{
}

bool float_object::operator==( const objref other ) const
{
    throw eval_exception(cerror::equate_float, "Can't equate to floats");
}

void float_object::render( std::ostream& os, bool abbrev)
{
    os << _value << " ";
    object::render(os);
}
