#include "../include/ewlxx/EwlButton.h"

namespace efl {

EwlButton::EwlButton( EwlObject* parent, const char* name )
    :EwlBox( parent, name )
{
  _o = EWL_OBJECT( ewl_button_new() );
  setText( name ? name : "unnamed" );
}

EwlButton::EwlButton( const char* text, EwlObject* parent, const char* name )
    :EwlBox( parent, name )
{
  _o = EWL_OBJECT( ewl_button_new() );
  setText( text );
}

EwlButton::~EwlButton()
{
}

void EwlButton::setText( const char* text )
{
  ewl_button_label_set( EWL_BUTTON(_o), const_cast<char*>( text ) );
}

} // end namespace efl
