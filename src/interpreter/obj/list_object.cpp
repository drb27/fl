#include <interpreter/obj/list_object.h>
#include <interpreter/obj/void_object.h>
#include <interpreter/smartlist.h>

using std::vector;
using std::list;

list_object::list_object(context* pContext,fclass& cls)
    : object(pContext,cls)
{
    _pList.reset(new smartlist());
}

list_object::list_object( context* pContext, const list_object& other)
    : object(pContext,other.get_class())
{
    _pList.reset( new smartlist(*(other._pList.get())) );
}

list_object::list_object(context* pContext, smartlist* l, fclass& cls)
    : object(pContext,cls)
{
    _pList.reset(l);
}

list_object::list_object(context* pContext,list<objref> startList,fclass& cls)
    : object(pContext,cls)
{
    if ( startList.size() )
    {
	vector<objref> items(startList.size() );
	int index=0;
	for ( auto item : startList )
	{
	    items[index++] = item;
	}

	blockref blk = chunk::make_block(items);
	_pList.reset(new smartlist());
	_pList->inplace_append(blk,items.size());
    }
    else
	_pList.reset( new smartlist() );
}

void list_object::append(objref e)
{
    _pList->inplace_append(e);
}

objref list_object::pop()
{
    objref pObject = _pList->inplace_pop();
    if (!pObject)
    {
	pObject = objref( new void_object(get_context()));
    }

    return pObject;
}

void list_object::prepend(objref e)
{
    _pList->inplace_prefix(e);
}

void list_object::append(listref other )
{
    _pList->inplace_append(other->_pList.get());
}

objref list_object::first()
{
    return get_element(0);
}

int list_object::size() const
{
    return _pList->size();
}

int list_object::chunks() const
{
    return _pList->chunks();
}

void list_object::optimise()
{
    _pList->inplace_chunkify( _pList->size() );
}

objref list_object::get_element(size_t index)
{
    return _pList->get_element(index);
}

listref list_object::tail(context* pContext) const
{
    smartlist* pNewList = _pList->tail();
    return listref( new list_object(pContext,pNewList,get_class()) );
}

void list_object::render( std::ostream& os, bool abbrev)
{
    os << "(";
    int maxindex = abbrev? (_pList->size()>5)?5:_pList->size() : _pList->size(); 
    for ( int index = 0 ; index < maxindex ; index ++ )
    {
	_pList->get_element(index)->render(os); 
	os << " ";
    }
    if (_pList->size()>maxindex) 
	os << "...";
    os << ") <" << size() << "> ";
    object::render(os,abbrev);
}
