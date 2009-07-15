#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/eetxx/EetList.h"
#include "../include/eetxx/EetDocument.h"

namespace efl {

EetList::EetList (EetDocument& doc, const std::string &filter)
    : _doc (doc),
    _list (0),
    _nb_elems (0)
{
  _list = eet_list (_doc.get (), filter.c_str (), &_nb_elems);
}

EetList::~EetList ()
{
  free(_list);
}

ChunkIterator EetList::begin ()
{
  return ChunkIterator(&_list[0], _doc);
}

ChunkIterator EetList::end ()
{
  return ChunkIterator(&_list[_nb_elems], _doc);
}

const EetChunk EetList::operator [] (const std::string &key_name) const
{
  return EetChunk (_doc.get (), key_name);
}

EetChunk EetList::operator [] (const std::string &key_name)
{
  return EetChunk (_doc.get (), key_name);
}

} // end namespace efl
