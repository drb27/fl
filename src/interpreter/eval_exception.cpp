#include <string>
#include <cstring>
#include <sstream>
#include "eval_exception.h"

using std::string;
using std::stringstream;

eval_exception::eval_exception(cerror errCode, const string& arg)
    : std::runtime_error(arg), _errcode(errCode)
{
}

eval_exception::~eval_exception()
{

}

cerror eval_exception::code() const
{
    return _errcode;
}

const char* eval_exception::what() const noexcept
{
    static char buffer[1024];
    stringstream s;
    int i = (int)_errcode;
    s << "E" << i << ": " << std::runtime_error::what() << std::endl;
    strcpy(buffer,s.str().c_str());
    return buffer;
}
