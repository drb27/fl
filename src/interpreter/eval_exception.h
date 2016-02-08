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
	instantiate_abstract=4,
	undefined_method=5,
	override_sealed=6,
	missing_attribute=7,
	invalid_assignment=8,
	invalid_lvalue=9,
	selector_nomatch=10,
	invalid_index=11,
	index_out_of_bounds=12,
	unsupported_argument=13,
	equate_float=14,
	unsupported_conversion=15
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

class terminate_exception : public std::runtime_error
{
public:
    terminate_exception();

private:
    static const std::string _what;
};

#endif
