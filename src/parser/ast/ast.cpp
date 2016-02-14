#include <iostream>
#include <string>
#include <parser/ast/ast.h>
#include <interpreter/builtins.h>

class context;

using std::ostream;
using std::string;

ast::ast()
{
}

ast::~ast()
{

}

bool ast::calls_and_returns( const string& name) const
{
    return false;
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
    return builtins::object::get_class();
}
