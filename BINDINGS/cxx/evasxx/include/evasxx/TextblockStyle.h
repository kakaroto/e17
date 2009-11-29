#ifndef EVAS_TEXTBLOCKSTYLE_H
#define EVAS_TEXTBLOCKSTYLE_H

#include <Evas.h>

/* STD */
#include <iostream>
using std::ostream;

namespace Evasxx {

/**
 * Evas Textblock Style
 *
 */

class TextblockStyle
{
  friend class Textblock;

public:
  TextblockStyle();
  TextblockStyle( const std::string &format );
  ~TextblockStyle();

  const std::string getFormat() const;
  void setFormat( const std::string &format );

private:
  Evas_Textblock_Style* o;
  bool mFree;
};

inline ostream& operator<<( ostream& s, const TextblockStyle& style )
{
  return s << "(TEXTBLOCKSTYLE " << style.getFormat() << "])";
}

} // end namespace Evasxx

#endif // EVAS_TEXTBLOCKSTYLE_H
