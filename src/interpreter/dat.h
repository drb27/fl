#ifndef DAT_H
#define DAT_H

#include <parser/action_target.h>
#include <interpreter/typemgr.h>


class dat : public action_target
{

 public:
    dat(typemgr&);
    virtual ~dat();

    virtual ast* make_int(int x) const;
    virtual ast* make_fundef( std::string* name, ast* def) const;
    virtual void respond( ast* def, std::ostream& os = std::cout ) const;

 protected:
    mutable typemgr _tm;

 private:

};

#endif
