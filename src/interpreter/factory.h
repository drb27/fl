#ifndef FACTORY_H
#define FACTORY_H

#include <vector>
#include <map>
#include <functional>
#include <inc/references.h>
#include <interpreter/class.h>

class fclass;
class context;

class factory final
{

 public:

    objref make_object( context*, fclass*, std::vector<ast*>& );
    objref make_object( context*, fclass*, std::vector<objref>& );
    objref make_object( context*, fclass*, listref );

    objref spawn_object(context*,fclass*);
    void add_spawner(fclass*, std::function<objref(context*,fclass*)> );

    static factory& get();

    static objref bootstrap_integer(context*,int);
    static std::vector<ast*> list_to_ast(listref);

 protected:
    static void call_ctor_chain(context*, fclass*, objref, listref );
    static void call_ctor_chain(context*, fclass*, objref, std::vector<ast*>& );
    static void call_constructor( context*, ctorinfo, objref, listref );
    static void call_constructor( context*, ctorinfo, objref, std::vector<ast*>& );

 private:

    factory();

    static factory* _factory_singleton;
    std::map<fclass*,std::function<objref(context*,fclass*)>> _ctor_map;

  };



#endif
