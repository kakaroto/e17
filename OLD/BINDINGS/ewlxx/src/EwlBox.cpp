#include "../include/ewlxx/EwlBox.h"

namespace efl {

EwlBox::EwlBox( EwlObject* parent, const char* name )
    :EwlContainer( parent, name )
{
  _o = EWL_OBJECT( ewl_box_new() );
}

EwlBox::~EwlBox()
{
}

} // end namespace efl
