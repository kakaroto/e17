#ifndef EVASEXCEPTIONS_H
#define EVASEXCEPTIONS_H

#include <string>

namespace efl {

class EvasFontCharacterPositionException : public std::exception
{
public:
  EvasFontCharacterPositionException () {}
  const char* what () const throw ();
};

} // end namespace efl

#endif // EVASEXCEPTIONS_H