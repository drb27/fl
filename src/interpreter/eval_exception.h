#ifndef EVAL_EXCEPTION_H
#define EVAL_EXCEPTION_H

#include <string>
#include <stdexcept>

enum class cerror
{
    internal_error =   0,
	syntax_error = 1,
	undefined_symbol = 2,
	symbol_redefinition=3,
	instantiate_abstract=4
};

class eval_exception : public std::runtime_error
{

 public:
    eval_exception(cerror errCode, const std::string& arg);
    virtual ~eval_exception();
    virtual cerror code() const;
    virtual const char* what() const noexcept;
 protected:

 private:
    const cerror _errcode;
};

#endif
