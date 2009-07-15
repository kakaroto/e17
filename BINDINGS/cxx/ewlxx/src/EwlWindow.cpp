#include "../include/ewlxx/EwlWindow.h"

#include <cstring>

namespace efl {

EwlWindow::EwlWindow( EwlObject* parent, const char* name )
    :EwlEmbed( parent, name )
{
  _o = EWL_OBJECT( ewl_window_new() );

  ewl_window_name_set( EWL_WINDOW( _o ), "EWL_WINDOW" );
  ewl_window_class_set( EWL_WINDOW( _o ), "EWLWindow" );
}

EwlWindow::~EwlWindow()
{
}

void EwlWindow::setTitle( const char* title )
{
  ewl_window_title_set( EWL_WINDOW( _o ), strdup( title ) );
}

} // end namespace efl
