#ifndef BASE_H
#define BASE_H

template<class T>
class static_base
{
 public:
    const T& base(void) const { return _base; }    
    bool is_root() const { return &_base==this; }
 
protected:
    static_base( const T& base) : _base(base) {}
 
 private:
    const T& _base;
};

#endif
