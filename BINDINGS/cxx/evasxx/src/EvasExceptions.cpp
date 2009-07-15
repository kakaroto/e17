#include "../include/evasxx/EvasExceptions.h"

namespace efl {

const char* EvasFontCharacterPositionException::what () const throw ()
{
  static std::string s ("Error while retrieve position information for character!");
  return static_cast <const char*> (s.c_str ());
}

} // end namespace efl
