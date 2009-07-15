#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/eetxx/EetChunk.h"

namespace efl {

EetChunk::EetChunk (Eet_File *ef, const std::string &key_name)
    : _ef (ef),
    _key_name (key_name),
    _size (0),
    _data (eet_read (_ef, key_name.c_str (), &_size))
{ }

EetChunk::~EetChunk ()
{
  _ef = 0;
  //free (_data);
  _data = 0;
}

EetChunk::ptr_type EetChunk::get () const
{
  return _data;
}

std::string EetChunk::get_key () const
{
  return _key_name;
}

} // end namespace efl
