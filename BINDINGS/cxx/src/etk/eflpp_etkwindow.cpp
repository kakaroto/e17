#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_etkwindow.h"
#include <cstring>

/* EFL */
#include <etk/Etk.h>

namespace efl {

EtkWindow::EtkWindow( EtkObject* parent, const char* type, const char* name )
    :EtkTopLevel( parent, type, name )
{
}

EtkWindow::~EtkWindow()
{
}

void EtkWindow::setTitle( const char* title )
{
    etk_window_title_set( ETK_WINDOW( _o ), strdup( title ) );
}

} // end namespace efl
