#include "../include/ewlxx/EwlContainer.h"

namespace efl {

EwlContainer::EwlContainer( EwlObject* parent, const char* name )
    :EwlWidget( parent, name )
{
}

EwlContainer::~EwlContainer()
{
}

void EwlContainer::appendChild( EwlWidget* child )
{
  ewl_container_child_append( EWL_CONTAINER( _o ), EWL_WIDGET( child->obj() ) );
}

} // end namespace efl
