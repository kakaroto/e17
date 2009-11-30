#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/eetxx/List.h"
#include "../include/eetxx/Document.h"

namespace Eetxx {

List::List (Document& doc, const std::string &filter)
    : _doc (doc),
    _list (0),
    _nb_elems (0)
{
  _list = eet_list (_doc.get (), filter.c_str (), &_nb_elems);
}

List::~List ()
{
  free (_list);
}

ChunkIterator List::begin ()
{
  return ChunkIterator(&_list[0], _doc);
}

ChunkIterator List::end ()
{
  return ChunkIterator(&_list[_nb_elems], _doc);
}

const Chunk List::operator [] (const std::string &key_name) const
{
  return Chunk (_doc.get (), key_name);
}

Chunk List::operator [] (const std::string &key_name)
{
  return Chunk (_doc.get (), key_name);
}

} // end namespace Eetxx
