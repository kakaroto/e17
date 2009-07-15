#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_evastext.h"

using namespace std;

namespace efl {

EvasText::EvasText( EvasCanvas* canvas, const char* name )
{
    o = evas_object_text_add( canvas->obj() );
    init( name ? name : "text" );

    setFont( EvasFont() );
}

EvasText::EvasText( int x, int y, const char* text, EvasCanvas* canvas, const char* name )
{
    o = evas_object_text_add( canvas->obj() );
    init( name ? name : "text" );;

    move( x, y );
    setFont( EvasFont() );
    setText( text );
}

EvasText::EvasText( const char* font, int size, const char* text, EvasCanvas* canvas, const char* name )
{
    o = evas_object_text_add( canvas->obj() );
    init( name ? name : "text" );

    setFont( font, size );
    setText( text );
}

EvasText::EvasText( const char* font, int size, int x, int y, const char* text, EvasCanvas* canvas, const char* name )
{
    o = evas_object_text_add( canvas->obj() );
    init( name ? name : "text" );

    move( x, y );
    setFont( font, size );
    setText( text );
}

EvasText::EvasText( EvasText* ao, EvasCanvas* canvas )
{
    Dout( dc::notice, *this << " EvasText::EvasText copy constructor" );
    o = evas_object_text_add( canvas->obj() );
    init( "text" );

    setGeometry( ao->geometry() );
    setLayer( ao->layer() );
    setText( ao->text() );
    setClip( ao->clip() );
    setFont( ao->font() );
    setColor( ao->color() );
    setVisible( ao->isVisible() );
}

const Size EvasText::size() const
{
    int width = horizontalAdvance();
    int height = verticalAdvance();
    Dout( dc::notice, *this << " EvasText::size() - width = " << width << ", height = " << height );

    return Size( width, height );
}

void EvasText::setFontSource( const char* source )
{
    evas_object_text_font_source_set( o, source );
}

EvasFont EvasText::font() const
{
    const char* name;
    Evas_Font_Size size;
    evas_object_text_font_get( o, &name, &size );
    return EvasFont( name, size );
}

void EvasText::setFont( const char* font, Evas_Font_Size size )
{
    evas_object_text_font_set( o, font, size );
}

void EvasText::setFont( const EvasFont& f )
{
    setFont( f.name(), f.size() );
}

void EvasText::setText( const char* text )
{
    evas_object_text_text_set( o, text );
}

const char* EvasText::text() const
{
    return evas_object_text_text_get( o );
}

int EvasText::ascent() const
{
    return evas_object_text_ascent_get( o );
}

int EvasText::descent() const
{
    return evas_object_text_descent_get( o );
}

int EvasText::maxAscent() const
{
    return evas_object_text_max_ascent_get( o );
}

int EvasText::maxDescent() const
{
    return evas_object_text_max_descent_get( o );
}

int EvasText::horizontalAdvance() const
{
    return evas_object_text_horiz_advance_get( o );
}

int EvasText::verticalAdvance() const
{
    return evas_object_text_vert_advance_get( o );
}

int EvasText::inset() const
{
    return evas_object_text_inset_get( o );
}


EvasText::~EvasText()
{
    evas_object_del( o );
}

} // end namespace efl
