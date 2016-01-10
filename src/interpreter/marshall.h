#ifndef MARSHALL_H
#define MARSHALL_H

#include <memory>
#include <vector>

class ast;
class context;
class object;
class int_object;

typedef std::shared_ptr<object> objref; 
typedef std::shared_ptr<int_object> intref;

typedef objref (marshall_fn_t)(context*,std::vector<ast*>&);


#endif
