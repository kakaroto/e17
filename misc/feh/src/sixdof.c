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

void slideshow_zoom_in()
{
    winwidget w = winwidget_get_first_window_of_type( WIN_TYPE_SLIDESHOW );
    if( w )
    {
        w->zoom = w->zoom * 1.05;
        w->im_x = (w->w - (w->zoom*w->im_w)) / 2;
        w->im_y = (w->h - (w->zoom*w->im_h)) / 2;
        winwidget_render_image(w, 0, 0);
    }
}

void slideshow_zoom_out()
{
    winwidget w = winwidget_get_first_window_of_type( WIN_TYPE_SLIDESHOW );
    if( w )
    {
        w->zoom = w->zoom * 0.95;
        w->im_x = (w->w - (w->zoom*w->im_w)) / 2;
        w->im_y = (w->h - (w->zoom*w->im_h)) / 2;
        winwidget_render_image(w, 0, 0);
    }
}


void slideshow_rotate_clockwise()
{
    winwidget w = winwidget_get_first_window_of_type( WIN_TYPE_SLIDESHOW );
    if( w )
    {
        gib_imlib_image_orientate( w->im, 1 );
        int t = w->im_h;
        w->im_h = w->im_w;
        w->im_w = t;
        w->im_x = (w->w - (w->zoom*w->im_w)) / 2;
        w->im_y = (w->h - (w->zoom*w->im_h)) / 2;
        winwidget_render_image(w, 0, 0);
    }
}

void slideshow_rotate_anticlockwise()
{
    winwidget w = winwidget_get_first_window_of_type( WIN_TYPE_SLIDESHOW );
    if( w )
    {
        gib_imlib_image_orientate( w->im, 3 );
        int t = w->im_h;
        w->im_h = w->im_w;
        w->im_w = t;
        w->im_x = (w->w - (w->zoom*w->im_w)) / 2;
        w->im_y = (w->h - (w->zoom*w->im_h)) / 2;
        winwidget_render_image(w, 0, 0);
    }
}



void sixdof_init( Display* disp )
{
   sdof = libsixdof_init("feh" );
   libsixdof_setDisplay( sdof, disp );

   libsixdof_registerTrivialCallableFunction( sdof, "slideshow-next", "", &slideshow_next );
   libsixdof_registerTrivialCallableFunction( sdof, "slideshow-prev", "", &slideshow_prev );
   libsixdof_registerTrivialCallableFunction( sdof, "zoom-in",        "", &slideshow_zoom_in );
   libsixdof_registerTrivialCallableFunction( sdof, "zoom-out",       "", &slideshow_zoom_out );
   libsixdof_registerTrivialCallableFunction( sdof, "rotate-clockwise","", &slideshow_rotate_clockwise );
   libsixdof_registerTrivialCallableFunction( sdof, "rotate-anticlockwise","", &slideshow_rotate_anticlockwise );
}


#endif
