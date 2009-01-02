#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_etkbox.h"

/* EFL */
#include <etk/etk_widget.h>

namespace efl {

EtkBox::EtkBox( EtkObject* parent, const char* type, const char* name )
    :EtkContainer( parent, type, name )
{
    //_o = ETK_OBJECT( etk_box_new() );
}

EtkBox::~EtkBox()
{
}

} // end namespace efl
