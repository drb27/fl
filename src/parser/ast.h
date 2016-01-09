#ifndef AST_H
#define AST_H

#include <functional>
#include <list>
#include <memory>
#include <inc/named.h>
#include <interpreter/class.h>

class object;
class int_object;
class context;

typedef std::shared_ptr<object> objref; 
typedef std::shared_ptr<int_object> intref;

class ast
{

 public:

    virtual ~ast();
    virtual objref evaluate(context*)=0;
    virtual objref evaluate(context*) const=0;
    virtual const fclass& type(context*) const=0;

 protected:
    ast();

 private:

};

class symbol_node : public ast
{
public:
    symbol_node( const std::string& );
    virtual objref evaluate(context*);
    virtual objref evaluate(context*) const;
    virtual const fclass& type(context*) const;
    virtual const std::string& name() const;
protected:
    const std::string _name;

};

class literal_node : public ast
{
public:
    literal_node( objref );
    virtual objref evaluate(context*);
    virtual objref evaluate(context*) const;
    virtual const fclass& type(context*) const;
protected:
    const objref _object;
};

class list_node : public literal_node
{
public:
    virtual void push_element(ast*);
    virtual objref evaluate(context*);
    virtual const fclass& type(context*) const;

private:
    std::list<ast*> _elements;
};

class methodcall_node : public ast
{
public:
    methodcall_node(const std::string&);
    virtual objref evaluate(context*);
    virtual objref evaluate(context*) const;
    virtual const fclass& type(context*) const;

    virtual void add_target(objref pObj);
    virtual void add_param(ast*);
    virtual void finalize_params();

protected:
    const std::string _name;
    objref _target;
    std::list<ast*> _params;
};

class fundef_node : public ast, public named
{
public:
    fundef_node(std::string name, const ast* def) : named(name), _definition(def) {}
    virtual objref evaluate(context*) const;
    virtual const fclass& type(context*);
    virtual objref operator()(objref) const;

protected:
    const ast* const  _definition;
};

class assign_node : public ast
{
public:
    assign_node(ast*,ast*);
    virtual objref evaluate(context*);
    virtual objref evaluate(context*) const;
    virtual const fclass& type(context*) const;

private:
    ast* _lvalue;
    ast* _rvalue;
};

#endif
