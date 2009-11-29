#ifndef EVAS_EXCEPTIONS_H
#define EVAS_EXCEPTIONS_H

#include <string>

namespace Evasxx {

class FontCharacterPositionException : public std::exception
{
public:
  FontCharacterPositionException () {}
  const char* what () const throw ();
};

} // end namespace Evasxx

#endif // EVAS_EXCEPTIONS_H

