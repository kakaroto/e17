#ifndef EFLPP_EVASTEXT_H
#define EFLPP_EVASTEXT_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_countedptr.h>
#include "eflpp_evasobject.h"
#include "eflpp_evascanvas.h"
#include "eflpp_evasfont.h"

/* EFL */
#include <Evas.h>

using std::string;

namespace efl {

/**
 * Wraps an Evas Text Object
 */
class EvasText : public EvasObject
{
  public:
    EvasText( EvasText* ao, EvasCanvas* canvas );
    EvasText( EvasCanvas* canvas, const char* name = 0 );
    EvasText( int x, int y, const char* text, EvasCanvas* canvas, const char* name = 0 );
    EvasText( const char* font, int size, const char* text, EvasCanvas* canvas, const char* name = 0 );
    EvasText( const char* font, int size, int x, int y, const char* text, EvasCanvas* canvas, const char* name = 0 );
    virtual ~EvasText();

    virtual const Size size() const;

    void setFontSource( const char* source );
    EvasFont font() const;
    void setFont( const char* font, Evas_Font_Size size );
    void setFont( const EvasFont& font );
    const char* text() const;
    void setText( const char* );

    int ascent() const;
    int descent() const;
    int maxAscent() const;
    int maxDescent() const;
    int horizontalAdvance() const;
    int verticalAdvance() const;
    int inset() const;

    /*
    //int characterPosition() const;
    //int characterCoordinates() const;
    EAPI int               evas_object_text_char_pos_get     (Evas_Object *obj, int pos, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch);
    EAPI int               evas_object_text_char_coords_get  (Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw,        Evas_Coord *ch);
    */

};


} // end namespace efl

#endif // EFLPP_EVASTEXT_H
