#ifndef EET_BASE_H
#define EET_BASE_H

//! Eet includes
#include <Eet.h>

//! STL includes
#include <iterator>
#include <memory>

/**
 * C++ Wrapper for the Enlightenment EET
 *
 * @author Jonathan Muller <jonathan.muller@drylm.org>
 */

namespace efl
{
  struct eet_document
  {
    eet_document (const std::string &file_name, Eet_File_Mode mode):
      _ef (eet_open (file_name.c_str (), mode)) 
    {}
      
    ~eet_document () { /*eet_close (_ef);*/ }
    void close () { eet_close (_ef); }

    Eet_File_Mode mode () { return eet_mode_get (_ef); }

    Eet_File *get () { return _ef; }
  private:
    Eet_File  *_ef;
  };

  template <typename _Ty>
  struct data_information
  {
    data_information (_Ty data, 
                      bool compress = false, 
                      unsigned int size = sizeof (_Ty) ) 
      : _data (data),
        _compress (compress),
        _size (size)
    {}
      
    _Ty _data;
    bool _compress;
    unsigned int _size;
  };

  /* Helper function to avoid explicit template instantiation by the user */
  template<typename _Ty>
  data_information<_Ty> 
  make_data (_Ty data, bool compress = false, unsigned int size = sizeof (_Ty))
  {
    return data_information<_Ty> (data, compress, size);
  }

  template<typename _Ty>
  data_information<_Ty *> 
  make_data (_Ty *data, bool compress = false, unsigned int size = sizeof (_Ty))
  {
    return data_information<_Ty *> (data, compress, size);
  }

  struct eet_chunk
  {
    typedef void* ptr_type;
    typedef const ptr_type const_ptr_type;
      
    eet_chunk (Eet_File *ef, const std::string &key_name)
      : _ef (ef),
        _key_name (key_name),
        _size (0),
        _data (eet_read (_ef, key_name.c_str (), &_size))
    { }
      
    ~eet_chunk ()
    {
      _ef = 0;
      //free (_data);
      _data = 0;
    }
      
    ptr_type
    get () const
    { return _data; }

    std::string
    get_key () const
    { return _key_name; }
      

    template <typename _Ty>
    eet_chunk &
    operator = (const _Ty& raw_data)
    { 
      data_information<_Ty> data_to_store = make_data (raw_data);
      operator = (data_to_store);

      return *this;
    }


    template<typename _Ty>
    eet_chunk&
    operator = (const data_information<_Ty>& data_to_store)
    { 
      eet_write  (_ef, 
                  _key_name.c_str (), 
                  &data_to_store._data, 
                  data_to_store._size, 
                  data_to_store._compress);
      return *this;
    }

    template<typename _Ty>
    eet_chunk&
    operator = (const data_information<_Ty *>& data_to_store)
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

  struct iterator : 
    public std::iterator<std::forward_iterator_tag, char *, void>
  {
      
    iterator (char **chunk, eet_document &doc)
      : the_chunk (chunk),
        _doc(doc) 
    {}

      
    eet_chunk
    operator * () const throw ()
    { return eet_chunk (_doc.get (), *the_chunk); }
      
    bool operator == (const iterator &i)
    { return the_chunk == i.the_chunk; }
      
    bool operator != (const iterator &i)
    { return the_chunk != i.the_chunk; }
      
    iterator &operator ++ () throw ()
    { 
      the_chunk = &the_chunk[1];
      return *this;
    }

    iterator operator ++ (int) throw ()
    { iterator tem (the_chunk, _doc); ++ *this; return tem; }
      
  private:
    iterator ();
    char     **the_chunk;
    eet_document  &_doc;
  };

    
  struct eetlist
  {
    eetlist (eet_document& doc, const std::string &filter = "*" ) 
      : _doc (doc), 
        _list (0),
        _nb_elems (0)
    { 
      _list = eet_list (_doc.get (), filter.c_str (), &_nb_elems);
    }

    ~eetlist () { free(_list); }

    iterator begin ()
    { return iterator(&_list[0], _doc); }
      
    iterator end ()
    { return iterator(&_list[_nb_elems], _doc); }

    const eet_chunk
    operator [] (const std::string &key_name) const
    { return eet_chunk (_doc.get (), key_name); }

    eet_chunk operator [] (const std::string &key_name)
    { return eet_chunk (_doc.get (), key_name); }

  private:
    eet_document &_doc;
    char     **_list;
    int        _nb_elems;
  };

} //ns efl


#endif //EET_BASE_H
