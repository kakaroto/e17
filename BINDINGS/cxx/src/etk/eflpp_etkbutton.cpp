#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_etkbutton.h"

/* EFL */
#include <etk/etk_button.h>

namespace efl {

EtkButton::EtkButton( EtkObject* parent, const char* type, const char* name )
    :EtkBox( parent, type, name )
{
    init( );
    setLabel( name ? name : "unnamed" );
}

EtkButton::EtkButton( const char* text, EtkObject* parent, const char* type, const char* name )
    :EtkBox( parent, type, name )
{
    setLabel( text );
}

EtkButton::EtkButton( Etk_Object *o )
{
    _o = o;
    _managed = false;
}

EtkButton::~EtkButton()
{
}

void EtkButton::setLabel( const string &label )
{
    etk_button_label_set( ETK_BUTTON(_o), label.c_str () );
}

const string EtkButton::getLabel( )
{
    return etk_button_label_get (ETK_BUTTON(_o));
}

void EtkButton::setImage( EtkImage *image )
{
    etk_button_image_set( ETK_BUTTON( _o ), ETK_IMAGE( image->obj( ) ));
}

EtkImage *EtkButton::getImage( )
{
    return EtkImage::wrap ( ETK_OBJECT( etk_button_image_get( ETK_BUTTON (_o ) )));
}

EtkButton *EtkButton::wrap( Etk_Object* o )
{
  return new EtkButton( o );
}

void EtkButton::setFromStock( Etk_Stock_Id stock_id )
{
    etk_button_set_from_stock( ETK_BUTTON( _o ), stock_id );
}

void EtkButton::setStyle( Etk_Button_Style style )
{
    etk_button_style_set( ETK_BUTTON( _o ), style );
}

Etk_Button_Style EtkButton::getStyle( )
{
    return etk_button_style_get( ETK_BUTTON( _o ));
}

void EtkButton::setStockSize( Etk_Stock_Size size )
{
    etk_button_stock_size_set( ETK_BUTTON( _o ), size );
}

Etk_Stock_Size EtkButton::getStockSize( )
{
    return etk_button_stock_size_get( ETK_BUTTON( _o ) );
}

void EtkButton::setAlignment( float xalign, float yalign )
{
    etk_button_alignment_set( ETK_BUTTON( _o ), xalign, yalign );
}

void EtkButton::getAlignment( float &xalign, float &yalign )
{
    etk_button_alignment_get( ETK_BUTTON( _o ), &xalign, &yalign);
}

} // end namespace efl
