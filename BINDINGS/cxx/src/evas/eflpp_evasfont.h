#ifndef EVAS_FONT_H
#define EVAS_FONT_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <Evas.h>

/* STD */
#include <iostream>
using std::ostream;

namespace efl {

/**
 * Evas Font
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

class EvasFont
{
  public:
    EvasFont();
    EvasFont( const char* name, Evas_Font_Size size, const char* style = "plain" );
    ~EvasFont();
    const char* name() const;
    Evas_Font_Size size() const;
    const char* style() const;

  private:
    const char* _name;
    Evas_Font_Size _size;
    const char* _style;
};

inline ostream& operator<<( ostream& s, const EvasFont& font )
{
    return s << "(FONT " << font.name() << "/" << font.size() << " [" << font.style() << "])";
}

}

#endif
