#ifndef EVASTEXTBLOCKSTYLE_H
#define EVASTEXTBLOCKSTYLE_H

#include <Evas.h>

/* STD */
#include <iostream>
using std::ostream;

namespace efl {

/**
 * Evas Textblock Style
 *
 */

class EvasTextblockStyle
{
  friend class EvasTextblock;

public:
  EvasTextblockStyle();
  EvasTextblockStyle( const std::string &format );
  ~EvasTextblockStyle();

  const std::string getFormat() const;
  void setFormat( const std::string &format );

private:
  Evas_Textblock_Style* o;
  bool mFree;
};

inline ostream& operator<<( ostream& s, const EvasTextblockStyle& style )
{
  return s << "(TEXTBLOCKSTYLE " << style.getFormat() << "])";
}

} // end namespace efl

#endif // EVASTEXTBLOCKSTYLE_H
