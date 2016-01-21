#ifndef MARSHALL_H
#define MARSHALL_H

#include <memory>
#include <vector>

class ast;
class context;
class object;
class int_object;
class bool_object;
class list_object;
class fn_object;
class class_object;
class string_object;

typedef std::shared_ptr<object> objref; 
typedef std::shared_ptr<int_object> intref;
typedef std::shared_ptr<bool_object> boolref;
typedef std::shared_ptr<list_object> listref;
typedef std::shared_ptr<fn_object> fnref;
typedef std::shared_ptr<class_object> classref;
typedef std::shared_ptr<string_object> stringref;

typedef objref (marshall_fn_t)(context*,std::vector<ast*>&);
typedef objref (marshall_mthd_t)(context*,objref pThis,std::vector<ast*>&);

#endif
