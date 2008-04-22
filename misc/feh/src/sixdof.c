#include "feh.h"
#include "winwidget.h"

#ifdef HAVE_SIXDOF
#include "sixdof.h"

sixdof_t sdof = 0;

void slideshow_next()
{
    winwidget w = winwidget_get_first_window_of_type( WIN_TYPE_SLIDESHOW );
    if( w )
    {
        slideshow_change_image( w, SLIDE_NEXT );
    }
}

void slideshow_prev()
{
    winwidget w = winwidget_get_first_window_of_type( WIN_TYPE_SLIDESHOW );
    if( w )
    {
        slideshow_change_image( w, SLIDE_PREV );
    }
}

void sixdof_init( Display* disp )
{
   sdof = libsixdof_init("feh" );
   libsixdof_setDisplay( sdof, disp );

   libsixdof_registerTrivialCallableFunction( sdof, "slideshow-next", "", &slideshow_next );
   libsixdof_registerTrivialCallableFunction( sdof, "slideshow-prev", "", &slideshow_prev );

}


#endif
