#ifndef REFS_H
#define REFS_H

#include <string>
#include <memory>
#include <map>

class ast;
class context;
class object;
class int_object;
class bool_object;
class list_object;
class fn_object;
class class_object;
class string_object;
class void_object;
class enum_object;
class float_object;
class lazy_object;
class signal_object;
class collection;

typedef std::shared_ptr<object> objref; 
typedef std::shared_ptr<int_object> intref;
typedef std::shared_ptr<float_object> floatref;
typedef std::shared_ptr<bool_object> boolref;
typedef std::shared_ptr<list_object> listref;
typedef std::shared_ptr<fn_object> fnref;
typedef std::shared_ptr<class_object> classref;
typedef std::shared_ptr<string_object> stringref;
typedef std::shared_ptr<void_object> voidref;
typedef std::shared_ptr<enum_object> enumref;
typedef std::shared_ptr<collection> colref;
typedef std::shared_ptr<lazy_object> lazyref;
typedef std::shared_ptr<signal_object> sigref;
typedef std::shared_ptr<signal_object> evalsigref;

#endif
