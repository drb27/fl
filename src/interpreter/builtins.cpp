#include <string.h>
#include "builtins.h"
#include <interpreter/class.h>

namespace builtins
{
    std::unique_ptr<fclass> object::build_class()
    {
	typespec spec("object",{});
	std::unique_ptr<fclass> pCls(new fclass(spec));
	return pCls;
    }

    std::unique_ptr<fclass> integer::build_class()
    {
	typespec spec("integer",{});
	std::unique_ptr<fclass> pCls(new fclass(spec));
	return pCls;
    }
}
