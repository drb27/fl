#ifndef AST_NODES_H
#define AST_NODES_H

#include <string>
#include <set>
#include <parser/callable.h>

class context;

class lvalue_node : public ast
{
public:
    virtual bool is_lvalue() const { return true; }
    virtual std::function<void(objref)> setter(context*)=0;
};

class symbol_node : public lvalue_node
{
public:
    symbol_node( const std::string& );
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual const std::string& name() const;
    virtual void required_symbols(std::set<std::string>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual std::function<void(objref)> setter(context*);
protected:
    const std::string _name;

};

class literal_node : public ast
{
public:
    literal_node( objref );
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<std::string>&) const;
protected:
    const objref _object;
};

class pair_node : public ast
{
public:
    pair_node( ast*,ast*);
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<std::string>&) const;
    virtual ast* first() const { return _first; }
    virtual ast* second() const { return _second; }

protected:
    ast* _first;
    ast* _second;
};

class selector_node : public ast
{
public:
    selector_node( ast* selector);
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<std::string>&) const;

    virtual void add_condition(ast* pair);
    virtual void set_default(ast* defaultExpr);
    virtual bool calls_and_returns( const std::string& fname) const;
protected:
    ast* _selector;
    ast* _default{nullptr};
    std::list<pair_node*> _conditions;

};

class sequence_node : public ast
{
public:
    sequence_node();
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<std::string>&) const;
    virtual void add_expr(ast*);
    virtual  bool calls_and_returns( const std::string& fname) const;
protected:
    std::list<ast*> _sequence;
};

class list_node : public ast
{
public:
    virtual void push_element(ast*);
    virtual objref evaluate(context*);    
    virtual fclass* type(context*) const;
    virtual std::list<ast*>& raw_elements();
    virtual void required_symbols(std::set<std::string>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

private:
    std::list<ast*> _elements;
};

class if_node : public ast
{
public:
    if_node(ast* pCondition, ast* trueExpression, ast* falseExpression);
    virtual objref evaluate(context*);    
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<std::string>&) const;
    virtual bool calls_and_returns( const std::string& fname) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
protected:
    ast* _condition;
    ast* _trueExpr;
    ast* _falseExpr;
};

class attr_node : public lvalue_node
{
public:
    attr_node(ast* target, const std::string& selector);
    virtual objref evaluate(context*);
    virtual void required_symbols(std::set<std::string>&) const;
    virtual std::function<void(objref)> setter(context*);
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

protected:
    ast* _target;
    std::string _selector;
};

class methodcall_node : public ast
{
public:
    methodcall_node(const std::string&);
    virtual objref evaluate(context*);

    virtual void add_target(ast* pObj);
    virtual void add_param(ast*);
    virtual void required_symbols(std::set<std::string>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
protected:
    const std::string _name;
    ast* _target;
    std::list<ast*> _params;
};

class funcall_node : public ast
{
public:
    funcall_node(const std::string&,ast* args);
    virtual objref evaluate(context*);
    virtual objref evaluate(context*,fnref);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<std::string>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
    virtual void invalidate() const;
    virtual bool calls_and_returns( const std::string& fname) const;
protected:
    const std::string _name;
    ast* _arg_list;
    mutable objref _result;
};

class fundef_node : public ast
{
public:
    fundef_node(ast* arglist, ast* definition);
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<std::string>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

    virtual bool is_tail_recursive() const;

protected:
    ast* _arglist;
    ast* _definition;
};

class assign_node : public ast
{
public:
    assign_node(ast*,ast*,bool alias=false);
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void required_symbols(std::set<std::string>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
private:
    ast* _lvalue;
    ast* _rvalue;
    const bool _alias;
};

class while_node : public ast
{
public:
    while_node(ast* pCond,ast* pAction);
    virtual objref evaluate(context*);
    virtual void required_symbols(std::set<std::string>&) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

private:

    ast* _cond;
    ast* _action;

};

#endif
