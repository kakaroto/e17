#include "../include/ewlxx/EwlVBox.h"

namespace efl {

EwlVBox::EwlVBox( EwlObject* parent, const char* name )
    :EwlBox( parent, name )
{
  ewl_box_orientation_set( EWL_BOX(_o), EWL_ORIENTATION_VERTICAL );
}

EwlVBox::~EwlVBox()
{
}

} // end namespace efl
