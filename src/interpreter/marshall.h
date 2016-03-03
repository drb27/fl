#ifndef MARSHALL_H
#define MARSHALL_H

#include <memory>
#include <vector>
#include <inc/references.h>

class context;
class fclass;

typedef objref (marshall_fn_t)(context*,std::vector<ast*>&);
typedef objref (marshall_mthd_t)(context*,objref pThis,std::vector<ast*>&);

#endif
