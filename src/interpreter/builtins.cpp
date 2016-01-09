#include <string.h>
#include "builtins.h"
#include <interpreter/class.h>

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
}
