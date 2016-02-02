#ifndef RAWFN_H
#define RAWFN_H

#include <vector>
#include <functional>
#include <interpreter/marshall.h>

class ast;
class context;

class rawfn final
{
 public:

    rawfn(ast* def, colref closure);
    rawfn(std::function<marshall_fn_t> fn);

    virtual objref operator()(context* pCtx, std::vector<ast*>& args);
    ast* def();
    bool is_builtin() const;
    void regenerate_function(ast* newDef);

 protected:
    colref _closure;
    ast* _def;
    std::function<marshall_fn_t> _fn;
};

#endif
