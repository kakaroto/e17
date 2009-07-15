#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_etktoplevel.h"

namespace efl {

EtkTopLevel::EtkTopLevel( EtkObject* parent, const char* type, const char* name )
    :EtkContainer( parent, type, name )
{
    init( );
}

EtkTopLevel::~EtkTopLevel()
{
}

} // end namespace efl
