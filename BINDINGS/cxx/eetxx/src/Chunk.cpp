#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/eetxx/Chunk.h"

namespace eet {

Chunk::Chunk (Eet_File *ef, const std::string &key_name)
    : _ef (ef),
    _key_name (key_name),
    _size (0),
    _data (eet_read (_ef, key_name.c_str (), &_size))
{ }

Chunk::~Chunk ()
{
  _ef = 0;
  //free (_data);
  _data = 0;
}
Chunk::ptr_type Chunk::get () const
{
  return _data;
}

std::string Chunk::get_key () const
{
  return _key_name;
}

} // end namespace eet
