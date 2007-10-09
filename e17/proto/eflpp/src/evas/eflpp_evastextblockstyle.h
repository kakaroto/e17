#ifndef EVAS_TEXTBLOCKSTYLE_H
#define EVAS_TEXTBLOCKSTYLE_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <Evas.h>

/* STD */
#include <iostream>
using std::ostream;

namespace efl {

/**
 * Evas Textblock Style
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

class EvasTextblockStyle
{
  public:
    EvasTextblockStyle();
    EvasTextblockStyle( const char* format );
    ~EvasTextblockStyle();
    const char* format() const;

  private:
    Evas_Textblock_Style* o;
};

inline ostream& operator<<( ostream& s, const EvasTextblockStyle& style )
{
    return s << "(TEXTBLOCKSTYLE " << style.format() << "])";
}

}

#endif
