#include <string>
#include <cstring>
#include <sstream>
#include "eval_exception.h"
#include <logger/logger.h>

using std::string;
using std::stringstream;

eval_exception::eval_exception(cerror errCode, const string& arg, bool fatal)
    : std::runtime_error(arg), _errcode(errCode),_fatal(fatal)
{
    wlog(level::error,what());
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
    if ( _fatal )
	s << "FATAL: ";
    s << "E" << i << ": " << std::runtime_error::what();
    strcpy(buffer,s.str().c_str());
    return buffer;
}

const std::string terminate_exception::_what{"Program terminated normally"};

terminate_exception::terminate_exception()
    :std::runtime_error(_what)
{
}

