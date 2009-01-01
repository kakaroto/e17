#ifndef EVAS_BASE_H
#define EVAS_BASE_H

#include "eflpp_evasevent.h"
#include "eflpp_evasfont.h"
#include "eflpp_evasutils.h"
#include "eflpp_evasobject.h"
#include "eflpp_evascanvas.h"
#include "eflpp_evasimage.h"

/* EFL++ */
#include <eflpp_common.h>

/* EFL */
#include <Evas.h>

/**
 * C++ Wrapper for the Enlightenment Canvas Library (EVAS)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

static const char* const EVAS_LOAD_ERROR[] =
{
    "No error on load",
    "A non-specific error occured",
    "File (or file path) does not exist",
    "Permission denied to an existing file (or path)"
    "Allocation of resources failure prevented load",
    "File corrupt (but was detected as a known format)",
    "File contains an unknown format",
    "File contains an incompatible format",
};

/* Forward declarations */
class EvasCanvas;
class EvasGradient;
class EvasLine;
class EvasObject;
class EvasPolygon;
class EvasRectangle;
class EvasText;
class EvasTextblockStyle;

/**
 * Wraps an Evas Line Object
 */
class EvasLine : public EvasObject
{
  public:
    EvasLine( EvasCanvas* canvas, const char* name = 0 );
    EvasLine( int x1, int y1, int x2, int y2, EvasCanvas* canvas, const char* name = 0 );
    virtual ~EvasLine();

    virtual void setGeometry( int x, int y, int width, int height );
};

/*
 * Wraps an Evas Rectangle Object
 */
class EvasRectangle : public EvasObject
{
  public:
    EvasRectangle( EvasCanvas* canvas, const char* name = 0 );
    EvasRectangle( int x, int y, int width, int height, EvasCanvas* canvas, const char* name = 0 );
    EvasRectangle( const Rect& r, EvasCanvas* canvas, const char* name = 0 );
    virtual ~EvasRectangle();
};

/*
 * Wraps an Evas Polygon Object
 */
class EvasPolygon : public EvasObject
{
  public:
    EvasPolygon( EvasCanvas* canvas, const char* name = 0 );
    virtual ~EvasPolygon();

    void addPoint( int x, int y );
    void clearPoints();
};

/*
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

/*
 * Wraps an Evas Textblock Object
 */
class EvasTextblock : public EvasObject
{
  public:
    EvasTextblock( EvasCanvas* canvas, const char* name = 0 );
    EvasTextblock( int x, int y, const char* text, EvasCanvas* canvas, const char* name = 0 );
    virtual ~EvasTextblock();

    void setStyle( const EvasTextblockStyle* style );
    void setText( const char* text );
    void clear();
};

/*
 * Wraps an Evas Gradient Object
 */
class EvasGradient : public EvasObject
{
  public:
    EvasGradient( int x, int y, int width, int height, EvasCanvas* canvas, const char* name = 0 );
    EvasGradient( EvasCanvas* canvas, const char* name = 0 );
    virtual ~EvasGradient();

   void addColorStop( int r, int g, int b, int a, int distance );
   void clear();
   void setAngle( Evas_Angle angle );
   Evas_Angle angle();

};

inline ostream& operator<<( ostream& s, const EvasCanvas& canvas )
{
    return s << "(Canvas)";
}
inline ostream& operator<<( ostream& s, const EvasObject& obj )
{
    return s << "<" << " Object '" << obj.name() << "' @ " << obj.geometry() << ">";
}

}

#endif
