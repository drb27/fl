#ifndef AST_H
#define AST_H

#include <list>
#include <memory>
#include <inc/named.h>
#include <interpreter/class.h>

class object;
class context;

class ast
{

 public:

    virtual ~ast();
    virtual object* evaluate(context*)=0;
    virtual object* evaluate(context*) const=0;
    virtual const fclass& type(context*) const=0;

 protected:
    ast();

 private:

};

class literal_node : public ast
{
public:
    literal_node( object* );
    virtual object* evaluate(context*);
    virtual object* evaluate(context*) const;
    virtual const fclass& type(context*) const;
protected:
    const std::unique_ptr<object> _object;
};

class list_node : public literal_node
{
public:
    virtual void push_element(ast*);
    virtual object* evaluate(context*);
    virtual const fclass& type(context*) const;

private:
    std::list<ast*> _elements;
};

class fundef_node : public ast, public named
{
public:
    fundef_node(std::string name, const ast* def) : named(name), _definition(def) {}
    virtual object* evaluate(context*) const;
    virtual const fclass& type(context*);
    virtual object* operator()(object*) const;

protected:
    const ast* const  _definition;
};

#endif
