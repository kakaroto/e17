#ifndef EET_DOCUMENT_H
#define EET_DOCUMENT_H

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

namespace efl {

class EetDocument
{
public:
  EetDocument (const std::string &file_name, Eet_File_Mode mode):
      _ef (eet_open (file_name.c_str (), mode))
  {}

  ~EetDocument () { /*eet_close (_ef);*/ }
    
  static void init ();

  void close ();

  Eet_File_Mode mode ();

  Eet_File *get ();

private:
  Eet_File  *_ef;
};

} // end namespace efl

#endif //EET_DOCUMENT_H
