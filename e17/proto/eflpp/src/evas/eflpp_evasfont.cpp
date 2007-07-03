#include <eflpp_sys.h>
#include <eflpp_common.h>
#include <eflpp_ecore.h>

#include "eflpp_evasfont.h"

namespace efl {

EvasFont::EvasFont()
{
    if ( eApp )
    {
        _name = eApp->font().name();
        _size = eApp->font().size();
        _style = eApp->font().style();
    }
    else Dout( dc::warning, *this << "EvasText::setDefaultFont() - can't set default font without EcoreApplication object" );
}

EvasFont::EvasFont( const char* name, Evas_Font_Size size, const char* style )
    : _name( name ), _size( size ), _style( style )
{
}

EvasFont::~EvasFont()
{
}

const char* EvasFont::name() const
{
    return _name;
}

Evas_Font_Size EvasFont::size() const
{
    return _size;
}

const char* EvasFont::style() const
{
    return _style;
}

}
