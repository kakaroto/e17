#include "../include/evasxx/Exceptions.h"

namespace Evasxx {

const char* FontCharacterPositionException::what () const throw ()
{
  static std::string s ("Error while retrieve position information for character!");
  return static_cast <const char*> (s.c_str ());
}

} // end namespace Evasxx

