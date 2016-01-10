#include <string.h>
#include "builtins.h"
#include <interpreter/class.h>
#include <interpreter/context.h>

namespace builtins
{
    std::shared_ptr<fclass> object::build_class()
    {
	typespec spec("object",{});
	std::shared_ptr<fclass> pCls(new fclass(spec));
	return pCls;
    }

    std::shared_ptr<fclass> integer::build_class()
    {
	typespec spec("integer",{});
	std::shared_ptr<fclass> pCls(new fclass(spec));
	return pCls;
    }

    objref add_integers(context* pContext, intref a,intref b)
    {
	const int result = a->internal_value() + b->internal_value();
	typespec int_spec = typespec("integer",{});
	const fclass& int_cls = pContext->types().lookup(int_spec);
	objref pObject(new int_object(result,int_cls));
    
	return pObject;
    }

}
