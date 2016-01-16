#ifndef AST_NODES_H
#define AST_NODES_H

#include <string>
#include <parser/callable.h>

class context;

class symbol_node : public ast
{
public:
    symbol_node( const std::string& );
    virtual objref evaluate(context*);
    virtual objref evaluate(context*) const;
    virtual fclass* type(context*) const;
    virtual const std::string& name() const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
protected:
    const std::string _name;

};

class literal_node : public ast
{
public:
    literal_node( objref );
    virtual objref evaluate(context*);
    virtual objref evaluate(context*) const;
    virtual fclass* type(context*) const;
protected:
    const objref _object;
};

class list_node : public ast
{
public:
    virtual void push_element(ast*);
    virtual objref evaluate(context*);    
    virtual objref evaluate(context*) const;
    virtual fclass* type(context*) const;
    virtual std::list<ast*>& raw_elements();
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
    virtual objref evaluate(context*) const;
    virtual fclass* type(context*) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
protected:
    ast* _condition;
    ast* _trueExpr;
    ast* _falseExpr;
};

class methodcall_node : public ast
{
public:
    methodcall_node(const std::string&);
    virtual objref evaluate(context*);
    virtual objref evaluate(context*) const;

    virtual void add_target(ast* pObj);
    virtual void add_param(ast*);
    virtual void finalize_params();
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
    virtual objref evaluate(context*) const;
    virtual fclass* type(context*) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

protected:
    const std::string _name;
    ast* _arg_list;
};

class fundef_node : public ast
{
public:
    fundef_node(ast* arglist, ast* definition);
    virtual objref evaluate(context*) const;
    virtual objref evaluate(context*);
    virtual fclass* type(context*) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="", 
			    const std::string& label="",
			    std::ostream& out=std::cout) const;
protected:
    ast* _arglist;
    ast* _definition;
};

class assign_node : public ast
{
public:
    assign_node(ast*,ast*,bool alias=false);
    virtual objref evaluate(context*);
    virtual objref evaluate(context*) const;
    virtual fclass* type(context*) const;
    virtual void render_dot(int& uuid, 
			    const std::string& parent="",
			    const std::string& label="",
			    std::ostream& out=std::cout) const;

private:
    ast* _lvalue;
    ast* _rvalue;
    const bool _alias;
};

#endif
