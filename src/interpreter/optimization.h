#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <vector>
#include <functional>
#include <parser/ast.h>


namespace opt
{

    struct pattern
    {
	std::vector<asttype> typestack;
	ast* matchedHead;
	int index;
	bool anchor;
    };
    
    class optimization
    {
	
    public:
	optimization();
	virtual ~optimization();
	
	
	virtual ast* search( ast* pRootNode) const;
	virtual bool execute(ast* pHeadNode) const=0;
	virtual bool search_and_destroy(ast* pRootNode) const;
    protected:
	virtual void get_pattern( pattern& p ) const=0;
	virtual ast* recursive_search( ast* pRootNode, pattern p ) const;
    private:
	
    };
    
    class if_tailcall : public optimization
    {

    public:
	if_tailcall(const std::string& name) : _name(name) {}
	virtual void get_pattern( pattern& p ) const;
	virtual bool execute(ast* pHeadNode) const;

    private:

	const std::string _name;
    };
}

#endif
    
