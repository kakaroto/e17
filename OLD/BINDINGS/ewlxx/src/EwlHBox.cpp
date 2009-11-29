#include "../include/ewlxx/EwlHBox.h"

namespace efl {

EwlHBox::EwlHBox( EwlObject* parent, const char* name )
    :EwlBox( parent, name )
{
  ewl_box_orientation_set( EWL_BOX(_o), EWL_ORIENTATION_HORIZONTAL );
}

EwlHBox::~EwlHBox()
{
}

} // end namespace efl
