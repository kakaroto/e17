#ifndef EET_CHUNK_H
#define EET_CHUNK_H

//! Eet includes
#include <Eet.h>

//! STL includes
#include <string>

//! EFLxx includes
#include "DataInformation.h"

namespace eet {

/* forward declarations */
class Document;

class Chunk
{
public:
  typedef void* ptr_type;
  typedef const ptr_type const_ptr_type;

  Chunk (Eet_File *ef, const std::string &key_name);

  ~Chunk ();

  ptr_type get () const;

  std::string get_key () const;

  template <typename _Ty>
  Chunk &
  operator = (const _Ty& raw_data)
  {
    DataInformation<_Ty> data_to_store = make_data (raw_data);
    operator = (data_to_store);

    return *this;
  }

  template<typename _Ty>
  Chunk&
  operator = (const DataInformation<_Ty>& data_to_store)
  {
    eet_write  (_ef,
                _key_name.c_str (),
                &data_to_store._data,
                data_to_store._size,
                data_to_store._compress);
    return *this;
  }

  template<typename _Ty>
  Chunk&
  operator = (const DataInformation<_Ty *>& data_to_store)
  {
    eet_write  (_ef,
                _key_name.c_str (),
                data_to_store._data,
                data_to_store._size,
                data_to_store._compress);
    return *this;
  }

private:
  Eet_File    *_ef;
  std::string  _key_name;
  int          _size;
  ptr_type     _data;
};

} // end namespace eet

#endif // EET_CHUNK_H
