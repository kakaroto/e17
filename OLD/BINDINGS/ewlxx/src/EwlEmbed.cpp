#include "../include/ewlxx/EwlEmbed.h"

namespace efl {

EwlEmbed::EwlEmbed( EwlObject* parent, const char* name )
    :EwlOverlay( parent, name )
{
  _o = EWL_OBJECT( ewl_embed_new() );
}

EwlEmbed::~EwlEmbed()
{
}

void EwlEmbed::setFocus( bool b )
{
  ewl_embed_focus_set( EWL_EMBED( _o ), b );
}

} // end namespace efl
