#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <cassert>
#include <etk/etk_window.h>
#include <etk/etk_button.h>
#include "eflpp_etkobject.h"

using namespace std;

namespace efl {

EtkObject::EtkObject( EtkObject* parent, const char* type, const char* name )
          :_o( 0 )
{
    cout << "EtkObject::EtkObject - generating object '" << ( name ? name : "<unknown>" ) << "' (" << type << ")" << endl;
    if ( strcmp( type, "TopLevelWidget" ) == 0 )
    {
        assert( not "implemented" );
        //_o = ETK_OBJECT( etk_widget_new() );
    }
    else if ( strcmp( type, "Embed" ) == 0 )
    {
        // already created
    }
    else if ( strcmp( type, "Window" ) == 0 )
    {
        _o = ETK_OBJECT( etk_window_new() );
    }
    else if ( strcmp( type, "Button" ) == 0 )
    {
        _o = ETK_OBJECT( etk_button_new() );
    }
    else
        assert( not "a known widget" );
}

EtkObject::~EtkObject()
{
}

void EtkObject::resize( const Size& size )
{
    etk_widget_size_request_set( ETK_WIDGET(_o), size.width(), size.height() );
}

void EtkObject::setFillPolicy( )
{
}

void EtkObject::setAlignment( )
{
}

void EtkObject::init( )
{
  _managed = true;
}

} // end namespace efl
