#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <vector>
#include <functional>
#include <parser/ast/ast.h>


namespace opt
{

    struct pattern
    {
	std::vector<asttype> typestack;
	astref matchedHead;
	int index;
	bool anchor;
    };
    
    class optimization
    {
	
    public:
	optimization();
	virtual ~optimization();
	
	
	virtual astref search(const fundefnoderef& pRootNode) const;
	virtual bool execute(astref pHeadNode) const=0;
    protected:
	virtual void get_pattern( pattern& p ) const=0;
	virtual astref recursive_search( astref pRootNode, pattern p ) const;
    private:
	
    };
    
    class if_tailcall : public optimization
    {

    public:
	if_tailcall(const std::string& name) : _name(name) {}
	virtual void get_pattern( pattern& p ) const;
	virtual bool execute(astref pHeadNode) const;

    private:

	const std::string _name;
    };
}

#endif
    
