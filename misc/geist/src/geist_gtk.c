
#include "geist_gtk.h"

void
geist_document_render_to_gtk_window(geist_document * doc, GtkWidget * win)
{
   Window xwin;

   D_ENTER(3);


   xwin = GDK_WINDOW_XWINDOW(win->window);

   XSetWindowBackgroundPixmap(disp, xwin, doc->pmap);

   XClearWindow(disp, xwin);

   D_RETURN_(3);
}

void
geist_document_render_to_gtk_window_partial(geist_document * doc,
                                            GtkWidget * win, int x, int y,
                                            int w, int h)
{
   Window xwin;

   D_ENTER(3);

   xwin = GDK_WINDOW_XWINDOW(win->window);

   XSetWindowBackgroundPixmap(disp, xwin, doc->pmap);

   XClearArea(disp, xwin, x, y, w, h, False);

   D_RETURN_(3);
}
