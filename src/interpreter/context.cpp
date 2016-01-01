#include <string.h>
#include "context.h"
#include "object.h"
#include "context.h"
#include "typemgr.h"

context::context()
{
}

context::~context()
{

}

typemgr& context::types()
{
    return m_types;
}
