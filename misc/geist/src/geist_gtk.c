
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
